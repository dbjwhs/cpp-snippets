#!/usr/bin/env python3
"""
Logger Replacer - A script to replace Logger::getInstance().log calls with LOG_* macros

Usage:
    python logger_replacer.py [directory]

Note:
	not perfect but got me 95% there
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

    # Pattern 2: Format-handling with two approaches:
    # 2a - Simple std::format cases where format is at end of line
    def format_replacer_single_line(match):
        level = match.group(1)
        format_str = match.group(2)
        return f'LOG_{level}(std::format({format_str}));'

    content = re.sub(
        r'Logger::getInstance\(\)\.log\(LogLevel::([A-Z]+),\s*std::format\((.*?)\)\);',
        format_replacer_single_line,
        content
    )

    # Pattern 3: Complex multi-line format calls with trailing arguments
    # First, identify multiline Logger calls that span multiple lines
    pattern3 = r'Logger::getInstance\(\)\.log\(LogLevel::([A-Z]+), std::format\((.*?)\)\);'

    def multiline_format_replacer(match):
        level = match.group(1)
        format_content = match.group(2)
        return f'LOG_{level}(std::format({format_content}));'

    # Use re.DOTALL to make the dot character match newlines
    #
	# flags=re.DOTALL ensures that when the pattern contains .*? (which means "match any character, as few as possible"),
	# it will match across multiple lines
	#
	#   This is crucial for your script because some of your logger calls span multiple lines with indentation, like:
	#   Logger::getInstance().log(LogLevel::INFO,
	#       std::format("Some formatted message with {} placeholders",
	#                    value1, value2));
    content = re.sub(pattern3, multiline_format_replacer, content, flags=re.DOTALL)

    # Pattern 4: More complex cases with std::format and arguments
    # This more carefully handles multiline std::format calls with args
    pattern4 = r'Logger::getInstance\(\)\.log\(LogLevel::([A-Z]+),\s*std::format\((.*?)\)(.*?)\);'

    def format_replacer_complex(match):
        """Handle the formatting of the replacement string for complex format patterns."""
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

    # Use re.DOTALL to make the dot character match newlines
    content = re.sub(pattern4, format_replacer_complex, content, flags=re.DOTALL)

    # Pattern 5: Simple string messages with commas or special characters
    # This pattern is less strict about the message content to catch more cases
    pattern5 = r'Logger::getInstance\(\)\.log\(LogLevel::([A-Z]+), (["](.*?)["]\));'
    replacement5 = r'LOG_\1(\2);'
    content = re.sub(pattern5, replacement5, content)

    # Pattern 6: Handle the indented/multi-line format cases
    # This finds Logger::getInstance across multiple lines with indentation
    pattern6 = r'Logger::getInstance\(\)\.log\(LogLevel::([A-Z]+),\s*\n\s*(.*?)\);'

    def multiline_replacer(match):
        level = match.group(1)
        content_text = match.group(2).strip()
        return f'LOG_{level}({content_text});'

    content = re.sub(pattern6, multiline_replacer, content, flags=re.DOTALL)

    # Pattern 7: Logger calls with string message and additional arguments (like exception message)
    # Example: Logger::getInstance().log(LogLevel::INFO, "Test passed: Empty account holder name rejected", e.what());
    pattern7 = r'Logger::getInstance\(\)\.log\(LogLevel::([A-Z]+), "(.*?)", (.*?)\);'
    replacement7 = r'LOG_\1("\2", \3);'
    content = re.sub(pattern7, replacement7, content)

    # Pattern 8: Complex multi-line format with multiple parameters and nested braces
    # This is a catch-all pattern for complex log statements that weren't caught by other patterns
    pattern8 = r'Logger::getInstance\(\)\.log\(LogLevel::([A-Z]+),(.*?)\);'

    def complex_replacer(match):
        level = match.group(1)
        args_text = match.group(2).strip()
        return f'LOG_{level}({args_text});'

    content = re.sub(pattern8, complex_replacer, content, flags=re.DOTALL)

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
    """Main function to process files in the given directory or a specific file."""
    # Get argument from command line or use current directory
    target = sys.argv[1] if len(sys.argv) > 1 else "."

    # Check if the target is a file or directory
    target_path = Path(target)
    cpp_files = []

    if target_path.is_file() and target_path.suffix == '.cpp':
        # If it's a direct cpp file
        print(f"Processing single file: {target_path}")
        cpp_files = [target_path]
    else:
        # Treat as directory
        print(f"Scanning for .cpp files in {target}...")
        cpp_files = list(Path(target).rglob("*.cpp"))

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

