#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counter for cleaned directories
cleaned_count=0
failed_count=0

# Function to log messages with timestamp
log() {
    local level=$1
    local message=$2
    local color=$3
    echo -e "${color}[$(date '+%Y-%m-%d %H:%M:%S')] [${level}] ${message}${NC}"
}

# Function to clean a build directory
clean_project() {
    local project_dir=$1
    local build_dir="${project_dir}/build"

    if [ -d "${build_dir}" ]; then
        log "INFO" "Cleaning ${build_dir}" "${YELLOW}"
        rm -rf "${build_dir}" || {
            log "ERROR" "Failed to clean ${build_dir}" "${RED}"
            return 1
        }
        log "SUCCESS" "Successfully cleaned ${build_dir}" "${GREEN}"
        return 0
    else
        log "INFO" "No build directory found in ${project_dir}" "${YELLOW}"
        return 0
    fi
}

# Main script
echo "Starting cleanup process..."

# Find all directories containing CMakeLists.txt
while IFS= read -r -d '' cmake_file; do
    project_dir=$(dirname "${cmake_file}")

    # Skip if already in a build directory
    if [[ "${project_dir}" == *"/build/"* ]]; then
        continue
    fi

    if clean_project "${project_dir}"; then
        ((cleaned_count++))
    else
        ((failed_count++))
    fi

done < <(find . -name "CMakeLists.txt" -print0)

# Print summary
echo
log "SUMMARY" "Cleanup process completed" "${GREEN}"
log "SUMMARY" "Successfully cleaned projects: ${cleaned_count}" "${GREEN}"
log "SUMMARY" "Failed to clean: ${failed_count}" "${RED}"

# Exit with failure if any cleanups failed
if [ ${failed_count} -gt 0 ]; then
    exit 1
fi

exit 0