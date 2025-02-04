#!/bin/bash

# ANSI Color Codes Explanation
# ---------------------------
# These variables contain ANSI escape codes for text coloring in the terminal
#
# Format: \033[<style>;<color>m
# - \033 is the escape character
# - Style values: 0=normal, 1=bold, 2=dim, 4=underlined
# - Color values: 31=red, 32=green, 33=yellow, 34=blue, 35=purple, 36=cyan, 37=white
#
RED='\033[0;31m'    # Normal red text
GREEN='\033[0;32m'  # Normal green text
YELLOW='\033[1;33m' # Bold yellow text
NC='\033[0m'        # No Color - resets color to terminal default
#
# Usage example:
#   echo -e "${GREEN}Success${NC}"
#   echo -e "${RED}Error${NC}"
#
# The ${NC} is essential - without it, all following terminal output
# would continue in the last used color. Always reset color after usage
# to prevent terminal color bleeding.

# Flag for dry run mode
DRY_RUN=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --dry-run|-d)
            DRY_RUN=true
            shift # Remove argument from processing
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--dry-run|-d]"
            exit 1
            ;;
    esac
done

# Counter for successful and failed builds
successful_builds=0
failed_builds=0

# Arrays to store build paths
declare -a successful_paths
declare -a failed_paths

# Function to log messages with timestamp
log() {
    local level=$1
    local message=$2
    local color=$3
    echo -e "${color}[$(date '+%Y-%m-%d %H:%M:%S')] [${level}] ${message}${NC}"
}

# Function to find executable name from CMakeLists.txt
find_executable_name() {
    local cmake_file=$1
    # Look for add_executable command in CMakeLists.txt
    local exe_name=$(grep -m 1 "add_executable(" "${cmake_file}" | sed 's/add_executable(//' | cut -d' ' -f1 | tr -d '[:space:]')
    echo "${exe_name}"
}

# Function to build a single CMake project
build_project() {
    local project_dir=$1
    local build_dir="build"
    local exe_name=$2

    if [ "$DRY_RUN" = true ]; then
        log "DRY-RUN" "Would build project in ${project_dir}" "${YELLOW}"
        log "DRY-RUN" "Would create build directory: ${build_dir}" "${YELLOW}"
        log "DRY-RUN" "Would run: cmake .." "${YELLOW}"
        log "DRY-RUN" "Would run: make" "${YELLOW}"
        return 0
    fi

    log "INFO" "Building project in ${project_dir}" "${YELLOW}"

    # Create and enter build directory
    mkdir -p "${build_dir}"
    cd "${build_dir}" || {
        log "ERROR" "Failed to create/enter build directory for ${project_dir}" "${RED}"
        return 1
    }

    # Run CMake
    log "INFO" "Running CMake..." "${YELLOW}"
    cmake .. || {
        log "ERROR" "CMake configuration failed for ${project_dir}" "${RED}"
        return 1
    }

    # Run Make
    log "INFO" "Running Make..." "${YELLOW}"
    make || {
        log "ERROR" "Make failed for ${project_dir}" "${RED}"
        return 1
    }

    log "SUCCESS" "Successfully built ${project_dir}" "${GREEN}"
    return 0
}

# Main script
if [ "$DRY_RUN" = true ]; then
    echo "Starting build process (DRY RUN - no actual builds will occur)..."
else
    echo "Starting build process..."
fi

# Find all directories containing CMakeLists.txt
while IFS= read -r -d '' cmake_file; do
    project_dir=$(dirname "${cmake_file}")

    # Skip if build directory is in path
    if [[ "${project_dir}" == *"/build/"* ]]; then
        continue
        log "ERROR" "Found CMake project in: ${project_dir} skipping" "${RED}"
    fi

    # Get executable name from CMakeLists.txt
    exe_name=$(find_executable_name "${cmake_file}")

    if [ "$DRY_RUN" = true ]; then
        log "DRY-RUN" "Found CMake project in: ${project_dir}" "${YELLOW}"
    else
        log "INFO" "Found CMake project in: ${project_dir}" "${YELLOW}"
    fi

    # Store current directory
    pushd "${project_dir}" > /dev/null || continue

    # Attempt to build the project
    if build_project "${project_dir}" "${exe_name}"; then
        ((successful_builds++))
        # Store both project path and executable path
        successful_paths+=("${project_dir}/build/${exe_name}")
    else
        ((failed_builds++))
        failed_paths+=("${project_dir}")
        log "FAILED" "Build failed for ${project_dir}" "${RED}"
    fi

    # Return to original directory
    popd > /dev/null

done < <(find . -name "CMakeLists.txt" -print0)

# Print summary
echo
if [ "$DRY_RUN" = true ]; then
    log "SUMMARY" "Build process completed (DRY RUN)" "${GREEN}"
    log "SUMMARY" "Would build ${successful_builds} projects:" "${GREEN}"
else
    log "SUMMARY" "Build process completed" "${GREEN}"
    log "SUMMARY" "Successful builds: ${successful_builds}" "${GREEN}"
fi

# Print successful builds
for path in "${successful_paths[@]}"; do
    if [ "$DRY_RUN" = true ]; then
        log "SUMMARY" "-> Would build: ${path}" "${GREEN}"
    else
        if [ -f "${path}" ]; then
            log "SUMMARY" "-> ${path}" "${GREEN}"
        else
            log "SUMMARY" "-> ${path} (executable not found)" "${YELLOW}"
        fi
    fi
done

if [ "$DRY_RUN" = false ]; then
    log "SUMMARY" "Failed builds: ${failed_builds}" "${RED}"
    # Print failed builds
    for path in "${failed_paths[@]}"; do
        log "SUMMARY" "-> ${path}" "${RED}"
    done
fi

# Exit with failure if any builds failed (only in non-dry-run mode)
if [ "$DRY_RUN" = false ] && [ ${failed_builds} -gt 0 ]; then
    exit 1
fi

exit 0