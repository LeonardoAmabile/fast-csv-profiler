#include "profiler.hpp"
#include <iostream>
#include <fstream>

Profiler::Profiler() : is_first_row(true), total_rows(0) {}

void Profiler::process_row(const std::vector<std::string_view>& row) {
    if (is_first_row) {
        for (const auto& cell : row) {
            std::string col_name = std::string(cell);
            if (!col_name.empty() && col_name.back() == '\r') col_name.pop_back(); 
            column_names.emplace_back(col_name);
        }
        columns.resize(row.size());
        is_first_row = false;
        return;
    }

    total_rows++; 

    if (row.size() > columns.size()) {
        columns.resize(row.size());
    }

    for (size_t i = 0; i < row.size(); ++i) {
        std::string_view cell = row[i];
        
        // Puliamo il \r di Windows per evitare bug sui testi
        if (!cell.empty() && cell.back() == '\r') {
            cell.remove_suffix(1);
        }

        if (cell.empty()) {
            columns[i].null_count++;
            continue;
        }

        try {
            double val = std::stod(std::string(cell)); 
            columns[i].numeric.update(val);
        } catch (...) {
            // LA MAGIA E' QUI: Se non è un numero, lo analizza come Testo!
            columns[i].text.update(cell);
        }
    }
}

void Profiler::generate_json(const std::string& output_path, double exec_time_sec) {
    std::ofstream out(output_path);
    out << "{\n  \"metadata\": {\n    \"total_rows\": " << total_rows << ",\n    \"execution_time_sec\": " << exec_time_sec << "\n  },\n";
    out << "  \"columns\": {\n";
    
    for (size_t i = 0; i < columns.size(); ++i) {
        std::string col_name = (i < column_names.size() && !column_names[i].empty()) ? column_names[i] : "col_" + std::to_string(i);
        
        // Determiniamo il tipo di colonna
        std::string type = "mixed";
        if (columns[i].numeric.get_count() > 0 && columns[i].text.get_count() == 0) type = "numeric";
        if (columns[i].text.get_count() > 0 && columns[i].numeric.get_count() == 0) type = "categorical";

        out << "    \"" << col_name << "\": {\n";
        out << "      \"type\": \"" << type << "\",\n";
        
        // Metriche Numeriche (Anche se misto)
        out << "      \"mean\": " << columns[i].numeric.get_mean() << ",\n";
        out << "      \"std_dev\": " << columns[i].numeric.get_std_dev() << ",\n";
        out << "      \"min\": " << (columns[i].numeric.get_count() > 0 ? std::to_string(columns[i].numeric.get_min()) : "null") << ",\n";
        out << "      \"max\": " << (columns[i].numeric.get_count() > 0 ? std::to_string(columns[i].numeric.get_max()) : "null") << ",\n";
        
        // Metriche Nominali
        out << "      \"is_high_cardinality\": " << (columns[i].text.is_high_cardinality() ? "true" : "false") << ",\n";
        out << "      \"top_values\": {";
        auto top_k = columns[i].text.get_top_k(5);
        for (size_t k = 0; k < top_k.size(); ++k) {
            out << "\"" << top_k[k].first << "\": " << top_k[k].second;
            if (k < top_k.size() - 1) out << ", ";
        }
        out << "},\n";
        
        // Valori Nulli
        out << "      \"null_count\": " << columns[i].null_count << "\n";
        out << "    }";
        
        if (i < columns.size() - 1) out << ",";
        out << "\n";
    }
    out << "  }\n}\n";
    out.close();
}