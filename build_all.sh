#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

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
echo "Starting build process..."

# Find all directories containing CMakeLists.txt
while IFS= read -r -d '' cmake_file; do
    project_dir=$(dirname "${cmake_file}")

    # Skip if build directory is in path
    if [[ "${project_dir}" == *"/build/"* ]]; then
        continue
    fi

    # Get executable name from CMakeLists.txt
    exe_name=$(find_executable_name "${cmake_file}")

    log "INFO" "Found CMake project in: ${project_dir}" "${YELLOW}"

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
log "SUMMARY" "Build process completed" "${GREEN}"
log "SUMMARY" "Successful builds: ${successful_builds}" "${GREEN}"
# Print successful builds
for path in "${successful_paths[@]}"; do
    if [ -f "${path}" ]; then
        log "SUMMARY" "-> ${path}" "${GREEN}"
    else
        log "SUMMARY" "-> ${path} (executable not found)" "${YELLOW}"
    fi
done

log "SUMMARY" "Failed builds: ${failed_builds}" "${RED}"
# Print failed builds
for path in "${failed_paths[@]}"; do
    log "SUMMARY" "-> ${path}" "${RED}"
done

# Exit with failure if any builds failed
if [ ${failed_builds} -gt 0 ]; then
    exit 1
fi

exit 0