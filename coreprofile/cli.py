import argparse
import sys
from engine_runner import run_cpp_profiler
from report_builder import build_report  # <-- New import

def main():
    parser = argparse.ArgumentParser(
        description="CoreProfile: Out-of-Core Analyzer for massive datasets."
    )
    
    parser.add_argument("csv_file", help="Path to the CSV file to analyze")
    parser.add_argument("--out", default="report.html", help="Name of the output HTML report (default: report.html)")
    
    args = parser.parse_args()
    
    try:
        # 1. Run the C++ backend
        stats = run_cpp_profiler(args.csv_file)
        
        # 2. Generate the HTML report
        build_report(data=stats, output_path=args.out)
        
        print("\n✅ All done! Open the HTML file in your web browser to view the results.")
        
    except Exception as e:
        print(f"❌ Execution error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()