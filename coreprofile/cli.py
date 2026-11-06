"""
CoreProfile CLI Entry Point
Handles command-line arguments, orchestrates the C++ backend execution, 
and triggers the generation of the final HTML dashboard.
"""

import argparse
import sys
from engine_runner import run_cpp_profiler
from report_builder import build_report

def main():
    """
    Main execution flow: parses arguments, runs the high-performance C++ engine,
    and formats the resulting JSON data into an interactive HTML report.
    """
    parser = argparse.ArgumentParser(
        description="CoreProfile: Out-of-Core Analyzer for massive datasets."
    )
    
    # Define mandatory and optional command-line arguments
    parser.add_argument("csv_file", help="Path to the CSV file to analyze")
    parser.add_argument("--out", default="report.html", help="Name of the output HTML report (default: report.html)")
    
    args = parser.parse_args()
    
    try:
        # Step 1: Execute the C++ engine to process the CSV out-of-core.
        # The engine streams the file, calculates stats, and returns a Python dictionary.
        stats = run_cpp_profiler(args.csv_file)
        
        # Step 2: Pass the computed statistics to Jinja2 to build the final HTML report.
        build_report(data=stats, output_path=args.out)
        
        # Success message with clear instructions for the user
        print("\n✅ All done! Open the HTML file in your web browser to view the results.")
        
    except Exception as e:
        # Graceful error handling in case of missing files, C++ execution failures, etc.
        print(f"❌ Execution error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()