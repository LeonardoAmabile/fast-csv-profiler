/**
 * @file profiler.hpp
 * @brief Core profiling engine that orchestrates data analysis.
 * 
 * Manages column-by-column statistics by dynamically routing data 
 * to either numeric or categorical trackers based on cell content.
 */

#pragma once

#include "welford.hpp"
#include "categorical.hpp"
#include <vector>
#include <string>
#include <string_view>

/**
 * @struct ColumnTracker
 * @brief Hybrid tracking structure for a single CSV column.
 * 
 * Since column types are not known in advance, each column possesses 
 * both a numeric engine and a textual engine. The profiler dynamically 
 * populates the appropriate engine based on whether the cell casting succeeds.
 */
struct ColumnTracker {
    Welford numeric;
    Categorical text;
    unsigned long long null_count = 0;
};

class Profiler {
private:
    std::vector<ColumnTracker> columns; 
    std::vector<std::string> column_names;
    bool is_first_row;
    unsigned long long total_rows;

public:
    Profiler();
    
    /**
     * @brief Processes a single CSV row, updating column statistics.
     * 
     * The first row is treated as the header. Subsequent rows update the 
     * internal Welford or Categorical state for each corresponding column.
     * 
     * @param row A vector of string_views representing the parsed CSV cells.
     */
    void process_row(const std::vector<std::string_view>& row);
    
    /**
     * @brief Exports the accumulated statistics to a JSON report.
     * 
     * @param output_path The destination path for the generated JSON file.
     * @param exec_time_sec Total execution time to be injected into the metadata.
     */
    void generate_json(const std::string& output_path, double exec_time_sec); 
};