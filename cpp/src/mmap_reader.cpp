#include "mmap_reader.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/mman.h>  // For mmap, munmap
#include <sys/stat.h>  // For getting the file size
#include <fcntl.h>     // For open, O_RDONLY
#include <unistd.h>    // For close

MmapReader::MmapReader(const std::string& filepath) : filepath(filepath) {}

void MmapReader::process_lines(std::function<void(const std::vector<std::string_view>&)> callback) {
    // 1. Open the file descriptor at a low POSIX level
    int fd = open(filepath.c_str(), O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Unable to open file: " + filepath);
    }

    // 2. Retrieve the exact file size to map the correct memory bounds
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("Unable to read file size.");
    }

    size_t file_size = sb.st_size;
    if (file_size == 0) {
        close(fd);
        return; // File is empty, exit gracefully
    }

    // 3. Memory Map (mmap) the file
    // The OS handles paging data from disk to RAM dynamically, ensuring O(1) memory usage
    const char* mapped_data = static_cast<const char*>(mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (mapped_data == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Fatal error: mmap failed.");
    }

    // 4. Zero-copy parsing directly on the mapped memory bytes
    std::vector<std::string_view> tokens;
    tokens.reserve(256); // Pre-allocate to prevent expensive vector reallocations per row

    const char* current = mapped_data;
    const char* end = mapped_data + file_size;
    const char* token_start = current;

    while (current < end) {
        if (*current == ',') {
            // Comma delimiter found: close the current string_view token
            tokens.emplace_back(token_start, current - token_start);
            token_start = current + 1;
        } else if (*current == '\n') {
            // Line break found: close the final token of the row
            size_t len = current - token_start;
            
            // Sanitize Windows-style carriage returns (\r\n)
            if (len > 0 && *(current - 1) == '\r') {
                len--;
            }
            
            tokens.emplace_back(token_start, len);
            
            // Dispatch the parsed row to the statistical profiler
            callback(tokens);
            
            // Reset the token vector for the next row
            tokens.clear();
            token_start = current + 1;
        }
        current++;
    }

    // Handle datasets that do not end with a trailing newline
    if (token_start < end) {
        size_t len = end - token_start;
        if (len > 0 && *(end - 1) == '\r') {
            len--;
        }
        tokens.emplace_back(token_start, len);
        callback(tokens);
    }

    // 5. Cleanup: Unmap the memory and close the file descriptor to prevent memory leaks
    munmap((void*)mapped_data, file_size);
    close(fd);
}