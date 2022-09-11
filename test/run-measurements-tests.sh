#!/bin/sh

execute() {
    lowerCaseType=$1
    pascalCaseType=$2
    datasetIndex=$3
    version=$4
    title=$5
    csvFile=$6
    threadsCount=$7

    # Print the summary
    echo "****"
    printf " * %s\n" "$title"
    echo "****"

    # Clear the output file
    printf "" >"times-${lowerCaseType}-${version}"
    # Add the line header to the CSV file
    echo  "\"$title\",,,,,," >> "$csvFile"

    # Execute the 3 iterations
    iteration=1
    while [ $iteration -le 3 ]; do
        "./Final_Project_HPC_Measurement_${pascalCaseType}_$version" "$datasetIndex" "times-${lowerCaseType}-$version" "$threadsCount"
        iteration=$((iteration + 1))
    done
    # Print the final time
    ./Mean_Times_Printer "times-${lowerCaseType}-${version}" "$csvFile"
}

executeSequential() {

    execute "sequential" "Sequential" "$1" "$2" "$3" "$4" 0
}

executeParallel() {

    execute "parallel" "Parallel" "$1" "$2" "$3" "$4" 2
    execute "parallel" "Parallel" "$1" "$2" "$3" "$4" 4
    execute "parallel" "Parallel" "$1" "$2" "$3" "$4" 8
    execute "parallel" "Parallel" "$1" "$2" "$3" "$4" 12
    execute "parallel" "Parallel" "$1" "$2" "$3" "$4" 16
}

datasetIndex=1 # TODO: 0
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

    csvFile="results.csv" #TODO:

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
    executeParallel 10 $datasetIndex "Parallel 10: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + std::vector<double*> + No Square Root" "$csvFile"

    datasetIndex=$((datasetIndex + 1))
done
