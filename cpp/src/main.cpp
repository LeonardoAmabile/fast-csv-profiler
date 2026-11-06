#include "mmap_reader.hpp"
#include "profiler.hpp"
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    // Ensure the user provides the required input and output file paths
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file.csv> <output.json>\n";
        return 1;
    }

    std::string input_csv = argv[1];
    std::string output_json = argv[2];

    std::cout << "Analyzing " << input_csv << "...\n";

    // Start the high-resolution execution timer
    auto start_time = std::chrono::high_resolution_clock::now();

    Profiler profiler; 
    MmapReader reader(input_csv);

    // Stream the file and process it line-by-line using a lambda callback
    reader.process_lines([&profiler](const std::vector<std::string_view>& row) {
        profiler.process_row(row);
    });

    // Stop the timer after the file parsing and statistical processing is complete
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;

    // Generate the final JSON report, injecting the execution time metadata
    profiler.generate_json(output_json, diff.count());

    std::cout << "Analysis completed in " << diff.count() << " seconds.\n";
    std::cout << "Report saved to " << output_json << "\n";

    return 0;
}