#include "mmap_reader.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/mman.h>  // For mmap, munmap
#include <sys/stat.h>  // For getting the file size
#include <fcntl.h>     // For open, O_RDONLY
#include <unistd.h>    // For close

MmapReader::MmapReader(const std::string& filepath) : filepath(filepath) {}

void MmapReader::process_lines(std::function<void(const std::vector<std::string_view>&)> callback) {
    // 1. Open the file at a low level
    int fd = open(filepath.c_str(), O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Unable to open file: " + filepath);
    }

    // 2. Get the exact file size
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("Unable to read file size.");
    }

    size_t file_size = sb.st_size;
    if (file_size == 0) {
        close(fd);
        return; // Empty file, exit immediately
    }

    // 3. MAP the file into memory
    // The OS will handle loading it into RAM dynamically ("Out-of-core")
    const char* mapped_data = static_cast<const char*>(mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (mapped_data == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Fatal error: mmap failed.");
    }

    // 4. Ultra-fast parsing directly on the mapped bytes
    std::vector<std::string_view> tokens;
    tokens.reserve(256); // Reserve space to avoid unnecessary memory reallocations

    const char* current = mapped_data;
    const char* end = mapped_data + file_size;
    const char* token_start = current;

    while (current < end) {
        if (*current == ',') {
            // Comma found: close the token
            tokens.emplace_back(token_start, current - token_start);
            token_start = current + 1;
        } else if (*current == '\n') {
            // End of line found
            size_t len = current - token_start;
            
            // Handle Windows-style carriage returns (\r\n)
            if (len > 0 && *(current - 1) == '\r') {
                len--;
            }
            
            tokens.emplace_back(token_start, len);
            
            // Send the complete parsed row to the orchestrator
            callback(tokens);
            
            // Reset for the next line
            tokens.clear();
            token_start = current + 1;
        }
        current++;
    }

    // Handle the very last line (in case the file doesn't end with a \n)
    if (token_start < end) {
        size_t len = end - token_start;
        if (len > 0 && *(end - 1) == '\r') {
            len--;
        }
        tokens.emplace_back(token_start, len);
        callback(tokens);
    }

    // 5. Cleanup (Crucial in C++ to prevent memory leaks)
    munmap((void*)mapped_data, file_size);
    close(fd);
}