#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <functional>

class MmapReader {
private:
    std::string filepath;

public:
    // Constructor taking the path to the CSV file
    explicit MmapReader(const std::string& filepath);

    // Reads the file and triggers the callback for every parsed row.
    // We use std::string_view to avoid copying string data, maximizing speed.
    void process_lines(std::function<void(const std::vector<std::string_view>&)> callback);
};