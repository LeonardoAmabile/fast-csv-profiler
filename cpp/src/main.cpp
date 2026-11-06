#include "mmap_reader.hpp"
#include "profiler.hpp"
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file.csv> <output.json>\n";
        return 1;
    }

    std::string input_csv = argv[1];
    std::string output_json = argv[2];

    std::cout << "Analyzing " << input_csv << "...\n";

    // Fa partire il cronometro
    auto start_time = std::chrono::high_resolution_clock::now();

    Profiler profiler; 
    MmapReader reader(input_csv);

    reader.process_lines([&profiler](const std::vector<std::string_view>& row) {
        profiler.process_row(row);
    });

    // Ferma il cronometro
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;

    // Genera il JSON passando ENTRAMBI gli argomenti (file e tempo)
    profiler.generate_json(output_json, diff.count());

    std::cout << "Analysis completed in " << diff.count() << " seconds.\n";
    std::cout << "Report saved to " << output_json << "\n";

    return 0;
}