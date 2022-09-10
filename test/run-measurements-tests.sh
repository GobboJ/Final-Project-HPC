#!/bin/sh

executeParallel() {
    version=$1
    dataset=$2
    threadsCount=$3

    iteration=1
    while [ $iteration -le 3 ]; do
        echo "[!] Iteration: ${iteration}"
        "./Final_Project_HPC_Measurement_Parallel_$version" "$dataset" "times-parallel-$version" "$threadsCount"
        iteration=$((iteration + 1))
    done
    ./Mean_Times_Printer "times-parallel-${version}"
}

dataset=1 # TODO: 0
while [ $dataset -le 1 ]; do
    version=1
    while [ $version -le 2 ]; do
        printf "" >"times-sequential-${version}"
        iteration=1
        while [ $iteration -le 3 ]; do
            echo "[!] Iteration: ${iteration}"
            ./Final_Project_HPC_Measurement_Sequential_${version} ${dataset} "times-sequential-${version}" 0
            iteration=$((iteration + 1))
        done
        ./Mean_Times_Printer "times-sequential-${version}"
        version=$((version + 1))
    done

    version=1
    while [ $version -le 10 ]; do
        printf "" >"times-parallel-${version}"
        executeParallel ${version} ${dataset} 2
        executeParallel ${version} ${dataset} 4
        executeParallel ${version} ${dataset} 8
        executeParallel ${version} ${dataset} 12
        executeParallel ${version} ${dataset} 16

        version=$((version + 1))
    done
done
