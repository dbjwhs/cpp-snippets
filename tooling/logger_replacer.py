#!/usr/bin/env python3
"""
Logger Replacer - A script to replace Logger::getInstance().log calls with LOG_* macros

Usage:
    python logger_replacer.py [directory]
"""

import os
import re
import sys
import glob
from pathlib import Path

def replace_logger_calls(content):
    """
    Replace Logger calls with appropriate LOG_* macros.
    
    Args:
        content (str): The content of a C++ file
        
    Returns:
        str: Modified content with Logger calls replaced by macros
    """
    # Pattern 1: Simple logger calls without formatting
    # Logger::getInstance().log(LogLevel::INFO, "message");
    pattern1 = r'Logger::getInstance\(\)\.log\(LogLevel::([A-Z]+), "(.*?)"\);'
    replacement1 = r'LOG_\1("\2");'
    content = re.sub(pattern1, replacement1, content)
    
    # Pattern 2: Logger calls with std::format
    # Logger::getInstance().log(LogLevel::INFO, std::format("message {}"), arg1);
    pattern2 = r'Logger::getInstance\(\)\.log\(LogLevel::([A-Z]+), std::format\((.*?)\)(.*?)\);'
    
    def format_replacer(match):
        """Handle the formatting of the replacement string for the second pattern."""
        level = match.group(1)
        format_str = match.group(2)
        args = match.group(3)
        
        # If args exist, we need to move them inside the format call
        if args and args.strip():
            # Extract the format string and move the args inside std::format
            if format_str.endswith('"'):
                # Remove the trailing double quote
                format_str = format_str[:-1]
                return f'LOG_{level}(std::format({format_str}{args}"));'
            else:
                return f'LOG_{level}(std::format({format_str}{args}));'
        else:
            return f'LOG_{level}(std::format({format_str}));'
    
    content = re.sub(pattern2, format_replacer, content)
    
    return content

def process_file(file_path):
    """
    Process a single C++ file and replace Logger calls.
    
    Args:
        file_path (str): Path to the C++ file
        
    Returns:
        bool: True if file was modified, False otherwise
    """
    # Read file content
    with open(file_path, 'r', encoding='utf-8') as f:
        original_content = f.read()
    
    # Replace logger calls
    modified_content = replace_logger_calls(original_content)
    
    # Write back if changed
    if original_content != modified_content:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(modified_content)
        return True
    
    return False

def main():
    """Main function to process files in the given directory."""
    # Get directory from command line argument or use current directory
    target_dir = sys.argv[1] if len(sys.argv) > 1 else "."
    
    print(f"Scanning for .cpp files in {target_dir}...")
    
    # Find all .cpp files
    cpp_files = list(Path(target_dir).rglob("*.cpp"))
    
    if not cpp_files:
        print("No .cpp files found.")
        return
    
    # Process each file
    modified_count = 0
    for file_path in cpp_files:
        print(f"Processing {file_path}...")
        if process_file(file_path):
            print(f"  Modified {file_path}")
            modified_count += 1
        else:
            print(f"  No changes needed in {file_path}")
    
    print(f"Logger replacement completed! Modified {modified_count} of {len(cpp_files)} files.")

if __name__ == "__main__":
    main()

