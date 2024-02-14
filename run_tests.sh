#!/bin/bash

# Capture the start time
startTime=$(date +%s)

# Define the arrays of arguments
tests=("prio" "rr" "fq_codel" "loom" "leaf_spine_bw" "tbf")
modes=("random" "front_back" "back_front")

# Base directory for research tests
baseDir="research_tests"

# Function to run a single test with a mode and track progress
run_test() {
    local test=$1
    local mode=$2
    local baseDir=$3

    # Create a subdirectory for the mode if it doesn't exist
    mkdir -p "$baseDir/$mode"

    # Path for the output file
    local outputFile="$baseDir/$mode/$test.txt"

    echo "Running ./build/fperf $test $mode..."

    # Execute the command and save the output
    ./build/fperf "$test" "$mode" > "$outputFile"

    echo "Output saved to $outputFile"
}

export -f run_test # Export the function for parallel to use
mkdir -p "$baseDir" # Ensure base directory exists

# Generate combinations of tests and modes and run them in parallel
parallel run_test ::: "${tests[@]}" ::: "${modes[@]}" ::: "$baseDir"

echo "All tests completed."

# Capture the end time
endTime=$(date +%s)

# Calculate the duration
duration=$((endTime - startTime))

echo "Total execution time: $duration seconds."
