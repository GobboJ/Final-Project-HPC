#!/bin/sh

#
# Builds all the make targets relative to the executables that measure the time taken to execute one
# implementation of the clustering algorithm.<br>
# This script assumes to be run inside the CMake build directory.
#
# @author DeB
# @author Jonathan
# @version 1.0 2022-09-11
# @since 1.0
#

#
# Builds the specified target.
#
# @param name Name of the target to build
#
build() {
    cmake --build . --target "$1" -- -j 9
}

# Build all the executables that measure the time taken to execute the sequential implementations
version=1
while [ $version -le 2 ]; do
    build "Final_Project_HPC_Measurement_Sequential_${version}"
    version=$((version + 1))
done

# Build all the executables that measure the time taken to execute the parallel implementations
version=1
while [ $version -le 11 ]; do
    build "Final_Project_HPC_Measurement_Parallel_${version}"
    version=$((version + 1))
done

# Build the executable that prints the mean execution times
build "Mean_Times_Printer"
