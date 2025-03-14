#!/bin/bash

# Define source and destination directories
SOURCE_DIR="cpp-snippets"
DEST_DIR="python-snippets"

# Make sure the destination directory exists
mkdir -p "$DEST_DIR"

# Function to process a directory
process_directory() {
    local source_dir="$1"
    local dest_dir="$2"

    # Check if cmake-build-debug exists in this directory
    if [ -d "$source_dir/cmake-build-debug" ]; then
        # Create cpp-ex directory at this level
        mkdir -p "$dest_dir/cpp-ex"

        # Copy all files and directories from this level to cpp-ex
        # except build, cmake-build-debug directories and custom.log
        for item in "$source_dir"/*; do
            local base_name=$(basename "$item")

            # Skip build and cmake-build-debug directories and custom.log file
            if [ "$base_name" != "build" ] && [ "$base_name" != "cmake-build-debug" ] && [ "$base_name" != "custom.log" ]; then
                cp -r "$item" "$dest_dir/cpp-ex/"
            fi
        done
    else
        # No cmake-build-debug found, just create the structure
        for item in "$source_dir"/*; do
            if [ -d "$item" ]; then
                local base_name=$(basename "$item")
                mkdir -p "$dest_dir/$base_name"
                process_directory "$item" "$dest_dir/$base_name"
            else
                # This is a file, we don't copy it in this basic structure copy
                # Uncomment this if you want to copy files too
                # cp "$item" "$dest_dir/"
                continue
            fi
        done
    fi
}

# Start the process
process_directory "$SOURCE_DIR" "$DEST_DIR"

echo "Directory structure copied from $SOURCE_DIR to $DEST_DIR"
echo "Directories containing cmake-build-debug have been processed with cpp-ex subdirectories"
echo "Excluded build, cmake-build-debug directories and custom.log files from cpp-ex"
