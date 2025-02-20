#!/bin/bash

# update_cmake_license.sh
# Script to add MIT License header to CMakeLists.txt files

# define the exact strings we're looking for
MIT_LINE="# MIT License"
COPYRIGHT_LINE="# Copyright (c) 2025 dbjwhs"

function check_and_update_file() {
    local file="$1"

    # check if file exists and is readable
    if [[ ! -r "$file" ]]; then
        echo "Error: Cannot read file $file"
        return 1
    fi

    # read first two lines for comparison
    local first_line=$(head -n 1 "$file" | tr -d '\r\n')
    local second_line=$(head -n 2 "$file" | tail -n 1 | tr -d '\r\n')

    if [[ "$first_line" == "$MIT_LINE" ]] && [[ "$second_line" == "$COPYRIGHT_LINE" ]]; then
        echo "License header already exists in $file"
        return 0
    fi

    # Create temporary file
    local temp_file=$(mktemp)
    if [[ ! -f "$temp_file" ]]; then
        echo "Error: Failed to create temporary file"
        return 1
    fi

    # add license header and original content
    echo "$MIT_LINE" > "$temp_file"
    echo "$COPYRIGHT_LINE" >> "$temp_file"
    echo >> $temp_file
    cat "$file" >> "$temp_file"

    # Replace original file with updated content
    if ! mv "$temp_file" "$file"; then
        echo "Error: Failed to update $file"
        rm -f "$temp_file"
        return 1
    fi

    echo "Successfully updated $file"
    return 0
}

# find all CMakeLists.txt files starting from parent directory
while IFS= read -r -d '' cmake_file; do
    check_and_update_file "$cmake_file"
done < <(find "../" -name "CMakeLists.txt" -type f -print0)
