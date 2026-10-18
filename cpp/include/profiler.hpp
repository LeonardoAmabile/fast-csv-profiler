#pragma once

#include "welford.hpp"
#include <vector>
#include <string>
#include <string_view>

class Profiler {
private:
    // A dynamic array holding a Welford statistics object for each column
    std::vector<Welford> columns_stats;

public:
    // Constructor: initializes the profiler
    explicit Profiler();

    // Takes a single parsed row and updates the statistics for all columns
    void process_row(const std::vector<std::string_view>& row);

    // Packages the final statistics and writes them to a JSON file
    void generate_json(const std::string& output_path);
};