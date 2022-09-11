#!/bin/sh

build() {
    cmake --build . --target "$1" -- -j 9
}

version=1
while [ $version -le 2 ]; do
    build "Final_Project_HPC_Measurement_Sequential_${version}"
    version=$((version + 1))
done

version=1
while [ $version -le 10 ]; do
    build "Final_Project_HPC_Measurement_Parallel_${version}"
    version=$((version + 1))
done

build "Mean_Times_Printer"
