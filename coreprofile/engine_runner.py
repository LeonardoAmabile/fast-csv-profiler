import subprocess
import json
import os

def run_cpp_profiler(csv_path: str, executable_path: str = "../cpp/profiler_test", output_json: str = "temp_report.json") -> dict:
    """Launches the C++ executable and returns the statistics as a Python dictionary."""
    
    if not os.path.exists(executable_path):
        raise FileNotFoundError(f"C++ executable not found: {executable_path}. Did you compile it?")
        
    print(f"Starting C++ engine on: {csv_path}")
    
    # Executes the bash command: ./profiler_test path/to/file.csv temp_report.json
    result = subprocess.run(
        [executable_path, csv_path, output_json], 
        capture_output=True, 
        text=True
    )
    
    # If the C++ backend crashes (e.g., file not found), catch the error
    if result.returncode != 0:
        raise RuntimeError(f"Error in C++ backend:\n{result.stderr}")
        
    # Print the C++ standard output (e.g., execution times)
    print(result.stdout)
    
    # Load the newly generated JSON into Python memory
    with open(output_json, "r") as f:
        data = json.load(f)
        
    # Cleanup: delete the temporary JSON file to keep the directory clean
    if os.path.exists(output_json):
        os.remove(output_json)
        
    return data