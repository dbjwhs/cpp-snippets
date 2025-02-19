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
        if [ "$DRY_RUN" = true ]; then
            log "DRY-RUN" "Would clean ${build_dir}" "${BLUE}"
            return 0
        else
            log "INFO" "Cleaning ${build_dir}" "${BLUE}"
            rm -rf "${build_dir}" || {
                log "ERROR" "Failed to clean ${build_dir}" "${RED}"
                return 1
            }
            log "SUCCESS" "Successfully cleaned ${build_dir}" "${GREEN}"
            return 0
        fi
    else
        if [ "$DRY_RUN" = true ]; then
            log "DRY-RUN" "No build directory found in ${project_dir}" "${BLUE}"
        else
            log "INFO" "No build directory found in ${project_dir}" "${BLUE}"
        fi
        return 0
    fi
}

# Main script
if [ "$DRY_RUN" = true ]; then
    echo "Starting cleanup process (DRY RUN - no directories will be removed)..."
else
    echo "Starting cleanup process..."
fi

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
done < <(find .. -name "CMakeLists.txt" -print0)

# Print summary
echo
if [ "$DRY_RUN" = true ]; then
    log "SUMMARY" "Cleanup process completed (DRY RUN)" "${GREEN}"
    log "SUMMARY" "Would clean ${cleaned_count} projects" "${GREEN}"
else
    log "SUMMARY" "Cleanup process completed" "${GREEN}"
    log "SUMMARY" "Successfully cleaned projects: ${cleaned_count}" "${GREEN}"
    log "SUMMARY" "Failed to clean: ${failed_count}" "${RED}"
fi

# Exit with failure if any cleanups failed (only in non-dry-run mode)
if [ "$DRY_RUN" = false ] && [ ${failed_count} -gt 0 ]; then
    exit 1
fi

exit 0
