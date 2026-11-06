"""
CoreProfile Engine Runner
Provides the bridge between the Python frontend and the high-performance C++ backend.
It manages process execution, data handoff, and temporary file cleanup.
"""

import subprocess
import json
import os

def run_cpp_profiler(csv_path: str, executable_path: str = "../cpp/profiler_test", output_json: str = "temp_report.json") -> dict:
    """
    Launches the compiled C++ executable to process the dataset out-of-core,
    parses the resulting JSON output, and returns it as a Python dictionary.
    
    Args:
        csv_path (str): The file path to the target CSV dataset.
        executable_path (str): The file path to the compiled C++ binary.
        output_json (str): The temporary file path for the intermediate JSON data.
        
    Returns:
        dict: The parsed statistical data containing metadata and column metrics.
        
    Raises:
        FileNotFoundError: If the C++ binary is missing (e.g., not compiled yet).
        RuntimeError: If the C++ subprocess exits with an error code.
    """
    
    if not os.path.exists(executable_path):
        raise FileNotFoundError(f"C++ executable not found: {executable_path}. Did you compile it?")
        
    print(f"🚀 Starting C++ engine on: {csv_path}")
    
    # Execute the C++ binary as a subprocess (equivalent to running it in the terminal)
    result = subprocess.run(
        [executable_path, csv_path, output_json], 
        capture_output=True, 
        text=True
    )
    
    # Catch and bubble up any errors from the C++ backend (e.g., corrupted file)
    if result.returncode != 0:
        raise RuntimeError(f"Error in C++ backend:\n{result.stderr}")
        
    # Print the C++ standard output (e.g., header detection, total execution time)
    print(result.stdout)
    
    # Load the newly generated JSON report into Python memory
    with open(output_json, "r") as f:
        data = json.load(f)
            
    # Cleanup: remove the temporary JSON file to keep the working directory pristine
    if os.path.exists(output_json):
        os.remove(output_json)
        
    return data