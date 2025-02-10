#!/bin/bash

# Function to display usage information
display_usage() {
    echo "Usage: $0 PATTERN [directory] [OPTIONS]"
    echo "Finds and opens .cpp files containing the specified pattern"
    echo ""
    echo "Options:"
    echo "  -h, --help     Display this help message"
    echo "  -l, --list     Only list files, don't open them"
    echo "  -e, --editor   Specify editor to use (default: system's default editor)"
    echo "  -r, --regex    Treat pattern as regular expression"
    echo ""
    echo "If no directory is specified, the current directory will be searched"
    echo ""
    echo "Examples:"
    echo "  $0 'Logger logger' ..        # Search parent directory"
    echo "  $0 'Logger logger' /path --list    # List files only"
    echo "  $0 -r 'Logger.*logger' .     # Search with regex"
}

# Function to find matching files
find_matching_files() {
    local pattern="$1"
    local search_dir="$2"
    local use_regex="$3"

    if [[ $use_regex == true ]]; then
        find "$search_dir" -type f -name "*.cpp" -exec grep -l -E "$pattern" {} +
    else
        find "$search_dir" -type f -name "*.cpp" -exec grep -l -F "$pattern" {} +
    fi
}

# Initialize variables
EDITOR=${EDITOR:-$(which vim 2>/dev/null || which nano 2>/dev/null || which vi)}
LIST_ONLY=false
SEARCH_DIR="."
USE_REGEX=false
PATTERN=""

# Ensure we have at least one argument
if [[ $# -eq 0 ]]; then
    display_usage
    exit 1
fi

# Get the pattern (first argument)
PATTERN="$1"
shift

# Get the directory if it's provided as the second argument and is actually a directory
if [[ $# -gt 0 ]] && [[ -d "$1" ]]; then
    SEARCH_DIR="$1"
    shift
fi

# Parse remaining options
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            display_usage
            exit 0
            ;;
        -l|--list)
            LIST_ONLY=true
            shift
            ;;
        -r|--regex)
            USE_REGEX=true
            shift
            ;;
        -e|--editor)
            if [[ -n $2 ]]; then
                EDITOR=$2
                shift 2
            else
                echo "Error: Editor not specified"
                exit 1
            fi
            ;;
        *)
            echo "Error: Invalid argument: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Check if pattern is provided
if [[ -z $PATTERN ]]; then
    echo "Error: Search pattern not provided"
    echo "Use --help for usage information"
    exit 1
fi

# Check if editor exists and is executable
if [[ $LIST_ONLY == false && ! -x $(which "$EDITOR") ]]; then
    echo "Error: Editor '$EDITOR' not found or not executable"
    exit 1
fi

echo "Searching in directory: $SEARCH_DIR"
echo "Pattern: $PATTERN"

# Find matching files and store in array
matching_files=()
while IFS= read -r file; do
    [[ -n $file ]] && matching_files+=("$file")
done < <(find_matching_files "$PATTERN" "$SEARCH_DIR" "$USE_REGEX")

# Check if any files were found
if [[ ${#matching_files[@]} -eq 0 ]]; then
    echo "No .cpp files containing '$PATTERN' found in $SEARCH_DIR"
    exit 0
fi

# Process found files
echo "Found ${#matching_files[@]} file(s):"
for file in "${matching_files[@]}"; do
    echo "$file"
done

# Open files if not in list-only mode
if [[ $LIST_ONLY == false ]]; then
    echo -e "\nOpening files with $EDITOR..."
    "$EDITOR" "${matching_files[@]}"
fi
