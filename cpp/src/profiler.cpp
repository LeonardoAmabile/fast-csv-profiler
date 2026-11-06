#include "profiler.hpp"
#include <iostream>
#include <fstream>

Profiler::Profiler() : is_first_row(true), total_rows(0) {}

void Profiler::process_row(const std::vector<std::string_view>& row) {
    // Treat the first row as the CSV header
    if (is_first_row) {
        for (const auto& cell : row) {
            std::string col_name = std::string(cell);
            // Sanitize potential trailing carriage returns from header names
            if (!col_name.empty() && col_name.back() == '\r') col_name.pop_back(); 
            column_names.emplace_back(col_name);
        }
        // Initialize the hybrid trackers for each column
        columns.resize(row.size());
        is_first_row = false;
        return;
    }

    total_rows++; 

    // Safety check: resize column trackers if a malformed row has extra columns
    if (row.size() > columns.size()) {
        columns.resize(row.size());
    }

    // Process each cell in the current row
    for (size_t i = 0; i < row.size(); ++i) {
        std::string_view cell = row[i];
        
        // Strip trailing carriage returns to prevent corrupted categorical labels
        if (!cell.empty() && cell.back() == '\r') {
            cell.remove_suffix(1);
        }

        // Handle missing/empty data
        if (cell.empty()) {
            columns[i].null_count++;
            continue;
        }

        // DYNAMIC DISPATCH: Attempt to parse the cell as a floating-point number.
        // If it throws an exception (invalid argument), route it to the categorical text engine.
        try {
            double val = std::stod(std::string(cell)); 
            columns[i].numeric.update(val);
        } catch (...) {
            // The data is textual (or malformed). Route to the Categorical Tracker.
            columns[i].text.update(cell);
        }
    }
}

void Profiler::generate_json(const std::string& output_path, double exec_time_sec) {
    std::ofstream out(output_path);
    
    // Inject global metadata including performance metrics
    out << "{\n  \"metadata\": {\n    \"total_rows\": " << total_rows << ",\n    \"execution_time_sec\": " << exec_time_sec << "\n  },\n";
    out << "  \"columns\": {\n";
    
    for (size_t i = 0; i < columns.size(); ++i) {
        // Fallback column naming in case the header was missing or empty
        std::string col_name = (i < column_names.size() && !column_names[i].empty()) ? column_names[i] : "col_" + std::to_string(i);
        
        // Dynamically infer the column type based on which engine processed the data
        std::string type = "mixed";
        if (columns[i].numeric.get_count() > 0 && columns[i].text.get_count() == 0) type = "numeric";
        if (columns[i].text.get_count() > 0 && columns[i].numeric.get_count() == 0) type = "categorical";

        out << "    \"" << col_name << "\": {\n";
        out << "      \"type\": \"" << type << "\",\n";
        
        // Export Numeric Metrics (included even for mixed-type columns)
        out << "      \"mean\": " << columns[i].numeric.get_mean() << ",\n";
        out << "      \"std_dev\": " << columns[i].numeric.get_std_dev() << ",\n";
        out << "      \"min\": " << (columns[i].numeric.get_count() > 0 ? std::to_string(columns[i].numeric.get_min()) : "null") << ",\n";
        out << "      \"max\": " << (columns[i].numeric.get_count() > 0 ? std::to_string(columns[i].numeric.get_max()) : "null") << ",\n";
        
        // Export Categorical Metrics (High Cardinality flag and Top K frequencies)
        out << "      \"is_high_cardinality\": " << (columns[i].text.is_high_cardinality() ? "true" : "false") << ",\n";
        out << "      \"top_values\": {";
        auto top_k = columns[i].text.get_top_k(5);
        for (size_t k = 0; k < top_k.size(); ++k) {
            out << "\"" << top_k[k].first << "\": " << top_k[k].second;
            if (k < top_k.size() - 1) out << ", ";
        }
        out << "},\n";
        
        // Export Missing/Null Value Count
        out << "      \"null_count\": " << columns[i].null_count << "\n";
        out << "    }";
        
        if (i < columns.size() - 1) out << ",";
        out << "\n";
    }
    
    out << "  }\n}\n";
    out.close();
}