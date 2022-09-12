#!/bin/sh

#
# Runs all the executables that measure the time taken to execute the implementations of the
# clustering algorithm, printing to the results to both the console and a CSV file.<br>
# This script assumes to be run inside the CMake build directory.
#
# @author DeB
# @author Jonathan
# @version 1.1 2022-09-12
# @since 1.0
#

#
# Runs the executable that measure the time taken to execute the specified implementation of the
# clustering algorithm.
#
# @param isParallel <code>0</code> if the executable to run is a sequential version, <code>1</code>
# @param version Version of the implementation to measure the execution time of. This number is used
#                   to compose the name of the executable to run.
# @param datasetIndex Index of the dataset to fed to the executable.
# @param title Title to print to the console describing the implementation that will be run.
# @param csvFile Name of the CSV file where the measured times will be printed to.
# @param threadsCount Number of threads to use when executing the specified implementation of the
#                       clustering algorithm.
#
execute() {
    # Extract the arguments
    isParallel=$1
    version=$2
    datasetIndex=$3
    title=$4
    csvFile=$5
    threadsCount=$6

    # Compute the first variable part of the executable name
    lowerCaseType=""
    pascalCaseType=""
    if [ "$isParallel" -eq 0 ]; then
        lowerCaseType="sequential"
        pascalCaseType="Sequential"
    else
        lowerCaseType="parallel"
        pascalCaseType="Parallel"
    fi

    # Print the summary
    echo "****"
    printf " * %s\n" "$title"
    echo "****"

    # Clear the output file
    printf "" >"times-${lowerCaseType}-${version}"
    # Add the line header to the CSV file
    echo "\"$title\",,,,,," >>"$csvFile"

    # Execute the 3 iterations
    iteration=1
    while [ $iteration -le 3 ]; do
        "./Final_Project_HPC_Measurement_${pascalCaseType}_$version" \
            "$datasetIndex" \
            "times-${lowerCaseType}-$version" \
            "$threadsCount"

        # Wait 2 seconds to make the operating system perform the cleaning operations
        sleep 2
        # Move to the next iteration
        iteration=$((iteration + 1))
    done
    # Print the mean execution time
    ./Mean_Times_Printer "times-${lowerCaseType}-${version}" "$csvFile"
}

#
# Runs the executable that measure the time taken to execute the specified sequential implementation
# of the clustering algorithm.
#
# @param version Version of the sequential implementation to measure the execution time of. This
#                number is used to compose the name of the executable to run.
# @param datasetIndex Index of the dataset to fed to the executable.
# @param title Title to print to the console describing the sequential implementation that will be
#               run.
# @param csvFile Name of the CSV file where the measured times will be printed to.
#
executeSequential() {

    execute 0 "$1" "$2" "$3" "$4" 0
}

#
# Runs the executable that measure the time taken to execute the specified parallel implementation
# of the clustering algorithm with 2, 4, 8, 12 and 16 threads.
#
# @param version Version of the parallel implementation to measure the execution time of. This
#                number is used to compose the name of the executable to run.
# @param datasetIndex Index of the dataset to fed to the executable.
# @param title Title to print to the console describing the parallel implementation that will be
#               run.
# @param csvFile Name of the CSV file where the measured times will be printed to.
#
executeParallel() {

    # Run the same implementation with 2, 4, 8, 12 and 16 threads
    execute 1 "$1" "$2" "$3" "$4" 2
    execute 1 "$1" "$2" "$3" "$4" 4
    execute 1 "$1" "$2" "$3" "$4" 8
    execute 1 "$1" "$2" "$3" "$4" 12
    execute 1 "$1" "$2" "$3" "$4" 16
}

# Index of the dataset to test, where 0 is "accelerometer.csv" while 1 is "generated.data"
datasetIndex=0

# Measure the times to execute the implementations on the two datasets
while [ $datasetIndex -le 1 ]; do

    # Print the name of the datasetIndex
    echo ""
    echo "==================="
    if [ $datasetIndex -eq 0 ]; then
        echo "Dataset: Accelerometer"
    else
        echo "Dataset: Generated"
    fi
    echo "==================="
    echo ""

    # Set the CSV file name
    csvFile="results-${datasetIndex}.csv"

    # Measure the execution time of the sequential implementations
    executeSequential 1 $datasetIndex "Sequential 1" "$csvFile"
    executeSequential 2 $datasetIndex "Sequential 2: Linearized" "$csvFile"

    # Measure the execution time of the parallel implementations
    executeParallel 1 $datasetIndex "Parallel 1: Multi-threaded Distance Computation" "$csvFile"
    executeParallel 2 $datasetIndex "Parallel 2: Multi-threaded Distance Computation + SSE" "$csvFile"
    executeParallel 3 $datasetIndex "Parallel 3: Multi-threaded Distance Computation + AVX" "$csvFile"
    executeParallel 4 $datasetIndex "Parallel 4: Multi-threaded Distance Computation + SSE + Linearized" "$csvFile"
    executeParallel 5 $datasetIndex "Parallel 5: Multi-threaded Distance Computation + AVX + Linearized" "$csvFile"
    executeParallel 6 $datasetIndex "Parallel 6: Multi-threaded Distance Computation and Stage 4 + AVX + Linearized" "$csvFile"
    executeParallel 7 $datasetIndex "Parallel 7: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + Linearized" "$csvFile"
    executeParallel 8 $datasetIndex "Parallel 8: Multi-threaded Distance Computation and Stage 4 + SSE Optimized + Linearized + No Square Root" "$csvFile"
    executeParallel 9 $datasetIndex "Parallel 9: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + Linearized + No Square Root" "$csvFile"
    executeParallel 10 $datasetIndex "Parallel 10: Multi-threaded Distance Computation and Stage 4 + SSE Optimized + std::vector<double*> + No Square Root" "$csvFile"
    executeParallel 11 $datasetIndex "Parallel 11: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + std::vector<double*> + No Square Root" "$csvFile"

    # Move to the next dataset
    datasetIndex=$((datasetIndex + 1))
done
