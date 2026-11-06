/**
 * @file mmap_reader.hpp
 * @brief High-performance CSV reader using Memory-Mapped I/O (mmap).
 * 
 * Streams massive datasets directly from disk to CPU without loading 
 * the entire file into RAM, ensuring O(1) memory complexity.
 */

#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <functional>

class MmapReader {
private:
    std::string filepath;

public:
    /**
     * Initializes the reader with the target CSV filepath.
     */
    explicit MmapReader(const std::string& filepath);

    /**
     * Streams the file and triggers the provided callback for every parsed row.
     * Utilizes std::string_view to reference memory directly, completely 
     * avoiding expensive string allocations and deep copies.
     * 
     * @param callback Function to execute on each parsed row.
     */
    void process_lines(std::function<void(const std::vector<std::string_view>&)> callback);
};