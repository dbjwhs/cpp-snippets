#!/bin/bash

# Function to check if file ends with a newline and add one if missing
ensure_trailing_newline() {
    local file="$1"

    # Check if file exists and is readable
    if [ ! -f "$file" ] || [ ! -r "$file" ]; then
        echo "Warning: Cannot read file: $file"
        return 1
    fi

    # Check if file is empty
    if [ ! -s "$file" ]; then
        echo "Warning: File is empty: $file"
        return 0
    fi

    # Check if file ends with a newline
    if [ -n "$(tail -c1 "$file")" ]; then
        echo "Adding newline to: $file"
        echo "" >> "$file"
    else
        echo "File already has trailing newline: $file"
    fi
}

# Main script
echo "Starting newline check process..."

# Find all matching files recursively in current directory, excluding CMake directories
find .. \( -path "*/CMakeFiles" -o -path "*/cmake-build-debug" \) -prune -o \
    -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.md" -o -name "*.sh" \) \
    -print0 | \
while IFS= read -r -d '' file; do
    ensure_trailing_newline "$file"
done

echo "Process completed."
