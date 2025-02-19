#!/bin/bash

# ANSI Color Codes Explanation
# ---------------------------
# These variables contain ANSI escape codes for text coloring in the terminal
#
# Format: \033[<style>;<color>m
# - \033 is the escape character
# - Style values: 0=normal, 1=bold, 2=dim, 4=underlined
# - Color values: 31=red, 32=green, 33=BLUE, 34=blue, 35=purple, 36=cyan, 37=white
#
RED='\033[0;31m'    # Normal red text
GREEN='\033[0;32m'  # Normal green text
BLUE='\033[0;94m'   # Normal blue text
NC='\033[0m'        # No Color - resets color to terminal default
#
# Usage example:
#   echo -e "${GREEN}Success${NC}"
#   echo -e "${RED}Error${NC}"
#
# The ${NC} is essential - without it, all following terminal output
# would continue in the last used color. Always reset color after usage
# to prevent terminal color bleeding.

# Flags for script modes
DRY_RUN=false
RUN_AFTER_BUILD=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --dry-run|-d)
            DRY_RUN=true
            shift
            ;;
        --run|-r)
            RUN_AFTER_BUILD=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--dry-run|-d] [--run|-r]"
            exit 1
            ;;
    esac
done

# Counter for successful and failed builds
successful_builds=0
failed_builds=0
successful_runs=0
failed_runs=0

# Arrays to store build paths
declare -a successful_paths
declare -a failed_paths
declare -a failed_runs_paths

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
    # Look for add_executable command in CMakeLists.txt and handle potential spaces/special chars
    local exe_name=$(grep -m 1 "add_executable(" "${cmake_file}" | \
                    sed -E 's/add_executable[[:space:]]*\(([^[:space:]]+).*/\1/' | \
                    tr -d '[:space:]')
    echo "${exe_name}"
}

# Function to run a built executable
run_executable() {
    local exe_path=$1
    local project_dir=$2

    if [ ! -f "${exe_path}" ]; then
        log "ERROR" "Executable not found: ${exe_path}" "${RED}"
        failed_runs_paths+=("${project_dir}")
        ((failed_runs++))
        return 1
    fi

    log "INFO" "Running executable: ${exe_path}" "${BLUE}"
    if "${exe_path}"; then
        log "SUCCESS" "Successfully ran ${exe_path}" "${GREEN}"
        ((successful_runs++))
        return 0
    else
        log "ERROR" "Failed to run ${exe_path}" "${RED}"
        failed_runs_paths+=("${project_dir}")
        ((failed_runs++))
        return 1
    fi
}

# Function to build a single CMake project
build_project() {
    local project_dir=$1
    local build_dir="build"
    local exe_name=$2

    if [ "$DRY_RUN" = true ]; then
        log "DRY-RUN" "Would build project in ${project_dir}" "${BLUE}"
        log "DRY-RUN" "Would create build directory: ${build_dir}" "${BLUE}"
        log "DRY-RUN" "Would run: cmake .." "${BLUE}"
        log "DRY-RUN" "Would run: make" "${BLUE}"
        if [ "$RUN_AFTER_BUILD" = true ]; then
            log "DRY-RUN" "Would run executable: ${exe_name}" "${BLUE}"
        fi
        return 0
    fi

    log "INFO" "Building project in ${project_dir}" "${BLUE}"

    # Create and enter build directory
    mkdir -p "${build_dir}"
    cd "${build_dir}" || {
        log "ERROR" "Failed to create/enter build directory for ${project_dir}" "${RED}"
        return 1
    }

    # Run CMake
    log "INFO" "Running CMake..." "${BLUE}"
    cmake .. || {
        log "ERROR" "CMake configuration failed for ${project_dir}" "${RED}"
        return 1
    }

    # Run Make
    log "INFO" "Running Make..." "${BLUE}"
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
        log "INFO" "Skipping build directory: ${project_dir}" "${BLUE}"
        continue
    fi

    # Get executable name from CMakeLists.txt
    exe_name=$(find_executable_name "${cmake_file}")

    if [ "$DRY_RUN" = true ]; then
        log "DRY-RUN" "Found CMake project in: ${project_dir}" "${BLUE}"
    else
        log "INFO" "Found CMake project in: ${project_dir}" "${BLUE}"
    fi

    # Store current directory
    pushd "${project_dir}" > /dev/null || continue

    # Attempt to build the project
    if build_project "${project_dir}" "${exe_name}"; then
        ((successful_builds++))
        # Store both project path and executable path
        current_dir=$(pwd)
        exe_path="${current_dir}/${exe_name}"  # Use absolute path
        successful_paths+=("${exe_path}")

        # Run the executable if requested
        if [ "$RUN_AFTER_BUILD" = true ] && [ "$DRY_RUN" = false ]; then
            run_executable "${exe_path}" "${project_dir}"
        fi
    else
        ((failed_builds++))
        failed_paths+=("${project_dir}")
        log "FAILED" "Build failed for ${project_dir}" "${RED}"
    fi

    # Return to original directory
    popd > /dev/null

done < <(find .. -name "CMakeLists.txt" -print0)

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
            log "SUMMARY" "-> ${path} (executable not found)" "${BLUE}"
        fi
    fi
done

if [ "$DRY_RUN" = false ]; then
    log "SUMMARY" "Failed builds: ${failed_builds}" "${RED}"
    # Print failed builds
    for path in "${failed_paths[@]}"; do
        log "SUMMARY" "-> ${path}" "${RED}"
    done

    # Print run summary if run mode was enabled
    if [ "$RUN_AFTER_BUILD" = true ]; then
        echo
        log "SUMMARY" "Run summary:" "${BLUE}"
        log "SUMMARY" "Successful runs: ${successful_runs}" "${GREEN}"
        log "SUMMARY" "Failed runs: ${failed_runs}" "${RED}"
        if [ ${failed_runs} -gt 0 ]; then
            for path in "${failed_runs_paths[@]}"; do
                log "SUMMARY" "-> Run failed: ${path}" "${RED}"
            done
        fi
    fi
fi

# Exit with failure if any builds failed or runs failed (only in non-dry-run mode)
if [ "$DRY_RUN" = false ] && ([ ${failed_builds} -gt 0 ] || [ ${failed_runs} -gt 0 ]); then
    exit 1
fi

exit 0

