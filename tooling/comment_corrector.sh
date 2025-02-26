#!/bin/bash

# Script to convert all comments in C/C++ files to lowercase
# Handles both single-line (//) and multi-line (/* */) comments

# Function to process a single file
process_file() {
    local file="$1"
    echo "Processing $file..."

    # Create a temporary file
    temp_file=$(mktemp)

    # Use awk to process the file
    # Note: awk uses curly braces in its syntax, which is correct for awk (not a bash syntax issue)
    awk '
    BEGIN {
        in_multiline_comment = 0
        in_string = 0
    }

    {
        line = $0
        output = ""
        i = 1

        # Special case: check for license and copyright lines that should be preserved
        if (line ~ /^\/\/ MIT License$/ || line ~ /^\/\/ Copyright \(c\) [0-9]+ dbjwhs$/) {
            print line
            next
        }

        while (i <= length(line)) {
            # Get the current character and the next one
            c = substr(line, i, 1)
            next_c = (i < length(line)) ? substr(line, i+1, 1) : ""

            # Check for string boundaries (to avoid processing comments inside strings)
            if (!in_multiline_comment && c == "\"" && (i == 1 || substr(line, i-1, 1) != "\\")) {
                in_string = !in_string
                output = output c
                i++
                continue
            }

            # Skip processing if inside a string
            if (in_string) {
                output = output c
                i++
                continue
            }

            # Check for multi-line comment start
            if (!in_multiline_comment && c == "/" && next_c == "*") {
                in_multiline_comment = 1
                output = output "/*"
                i += 2
                continue
            }

            # Check for multi-line comment end
            if (in_multiline_comment && c == "*" && next_c == "/") {
                in_multiline_comment = 0
                output = output "*/"
                i += 2
                continue
            }

            # Check for single-line comment
            if (!in_multiline_comment && c == "/" && next_c == "/") {
                # Append the comment markers
                output = output "//"
                i += 2

                # Convert the rest of the line to lowercase
                rest_of_line = tolower(substr(line, i))
                output = output rest_of_line

                # We processed the whole line, break out
                break
            }

            # Inside multi-line comment, convert to lowercase
            if (in_multiline_comment) {
                output = output tolower(c)
            } else {
                output = output c
            }

            i++
        }

        print output
    }' "$file" > "$temp_file"

    # Check if the file was changed
    if ! cmp -s "$file" "$temp_file"; then
        mv "$temp_file" "$file"
        echo "  Comments converted to lowercase in $file"
    else
        rm "$temp_file"
        echo "  No changes needed in $file"
    fi
}

# Check command line arguments
if [ $# -eq 0 ]; then
    # No arguments provided, use parent directory
    directory=".."
else
    directory="$1"
fi

# Find all C/C++ files and process them, excluding cmake-build-debug and build directories
find "$directory" -type f \( -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "*.hpp" \) \
    -not -path "*/cmake-build-debug/*" \
    -not -path "*/build/*" | while read -r file; do
    process_file "$file"
done

echo "All files processed!"
