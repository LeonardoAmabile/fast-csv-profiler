#include "profiler.hpp"
#include <iostream>
#include <fstream>

// Il costruttore ora è vuoto
Profiler::Profiler() {}

void Profiler::process_row(const std::vector<std::string_view>& row) {
    // MAGIA DINAMICA: Se la riga ha più colonne del nostro array, lo allarghiamo
    if (row.size() > columns_stats.size()) {
        columns_stats.resize(row.size());
    }

    for (size_t i = 0; i < row.size(); ++i) {
        if (row[i].empty()) {
            columns_stats[i].add_null();
            continue;
        }
        try {
            double val = std::stod(std::string(row[i])); 
            columns_stats[i].update(val);
        } catch (...) {
            // Salta i valori non numerici
        }
    }
}

void Profiler::generate_json(const std::string& output_path) {
    std::ofstream out_file(output_path);
    if (!out_file.is_open()) {
        std::cerr << "Errore: impossibile aprire il file " << output_path << " per la scrittura.\n";
        return;
    }

    out_file << "{\n";
    
    for (size_t i = 0; i < columns_stats.size(); ++i) {
        // Write key for column
        out_file << "  \"col_" << i << "\": {\n";
        
        // Write internal values
        out_file << "    \"mean\": " << columns_stats[i].get_mean() << ",\n";
        out_file << "    \"variance\": " << columns_stats[i].get_variance() << ",\n";
        out_file << "    \"min\": " << columns_stats[i].get_min() << ",\n";
        out_file << "    \"max\": " << columns_stats[i].get_max() << ",\n";
        out_file << "    \"null_count\": " << columns_stats[i].get_null_count() << "\n";
        
        // Close the object
        out_file << "  }";
        
        // Add comma only if it is not the last row
        if (i < columns_stats.size() - 1) {
            out_file << ",";
        }
        out_file << "\n";
    }

    out_file << "}\n";
    out_file.close();
}