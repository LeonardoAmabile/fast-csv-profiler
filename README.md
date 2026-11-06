# CoreProfile: High-Performance Out-of-Core CSV Profiler

CoreProfile is a high-performance, cross-language tool designed to generate exploratory data analysis (EDA) reports for massive CSV datasets.

Instead of loading data into RAM (like Pandas or standard Python scripts), CoreProfile utilizes a C++ backend leveraging POSIX mmap and streaming algorithms to process gigabytes of data with near-zero memory footprint. A Python frontend then orchestrates the execution and renders the statistical output into a beautiful, zero-dependency HTML dashboard.

## Why CoreProfile?

Data scientists often face the "RAM bottleneck": you cannot easily profile a 10GB dataset on an 8GB laptop using standard tools. CoreProfile solves this by reading the data directly from the disk to the CPU, processing it row by row, and immediately discarding the raw strings.

## Performance Benchmark

On a standard machine, a stress test on a 0.5 GB CSV file containing 21.4 Million rows (over 107 million individual cells) yielded the following results:

* **Execution Time:** ~56 Seconds (Single-threaded)

* **Processing Speed:** ~380,000 rows/second

* **RAM Usage:** Practically 0 MB (O(1) memory complexity)

## Key Features

- Out-of-Core I/O: Uses memory-mapped files (sys/mman.h) avoiding expensive read() syscalls and heavy std::string allocations by using std::string_view.

- Streaming Math: Implements Welford's Algorithm to compute the Mean, Variance, and Standard Deviation in a single pass without storing arrays and completely preventing floating-point overflow.

- Smart Categorical Handling: Automatically detects non-numeric columns. Computes frequency counts for categories and includes a Cardinality Safeguard to flush memory if a column contains free-text or unique IDs.

- Elegant UI: The Python orchestrator converts the backend's JSON output into a styled HTML report using Jinja2, complete with CSS proportional bars.

## Architecture

The C++ Engine (cpp/): Maps the file, streams the text, calculates math and frequencies, and dumps a lightweight temp_report.json.

The Python Orchestrator (coreprofile/): Provides a seamless CLI, spawns the C++ subprocess, reads the JSON, and merges it with a Jinja2 HTML template.

## Getting Started

Prerequisites

* A POSIX-compliant system (Linux, macOS, or WSL on Windows).

* g++ compiler (supporting C++17).

* Python 3.6+ with jinja2 installed (pip install jinja2).

1. Compile the C++ Engine

Navigate to the cpp directory and compile the source code:

```bash
cd cpp
g++ -std=c++17 -I include src/*.cpp -o profiler_test
```

2. Run the Profiler

Navigate to the Python module and run the CLI against any CSV file:

```bash
cd ../coreprofile
python cli.py path/to/your_dataset.csv --out dashboard.html
```

Open dashboard.html in any web browser to view your statistics!

## Future Roadmap

While the current architecture is highly optimized for single-threaded execution, future enhancements could push the performance even further:

- SIMD & Zero-cost Parsing: Replace std::stod with std::from_chars or fast_float libraries to drastically reduce string-to-double conversion times.

- Multithreaded Mmap Chunking: Divide the memory-mapped file into distinct chunks (respecting newline boundaries) to distribute the processing load across multiple CPU cores via std::thread.

- CI/CD Pipeline: Implement GitHub Actions with pytest (for Python) and GTest (for C++) to automatically validate mathematical accuracy and ensure the Welford algorithm remains stable on edge cases.

*Developed as a portfolio project bridging high-performance Systems Engineering with Data Science.*
