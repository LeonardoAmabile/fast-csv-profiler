#pragma once

#include "welford.hpp"
#include "categorical.hpp"
#include <vector>
#include <string>
#include <string_view>

// Nuova struttura: ogni colonna ha il suo motore numerico e il suo motore testuale
struct ColumnTracker {
    Welford numeric;
    Categorical text;
    unsigned long long null_count = 0;
};

class Profiler {
private:
    std::vector<ColumnTracker> columns; // <-- Sostituisce columns_stats
    std::vector<std::string> column_names;
    bool is_first_row;
    unsigned long long total_rows;

public:
    Profiler();
    void process_row(const std::vector<std::string_view>& row);
    void generate_json(const std::string& output_path, double exec_time_sec); 
};