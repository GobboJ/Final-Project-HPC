# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC_Sequential CXX)
set(CMAKE_CXX_STANDARD 20)

add_library(
    Final_Project_HPC_Sequential STATIC
    include/sequential/SequentialClustering.h src/sequential/SequentialClustering.cpp
)

# Export the include directory
get_filename_component(includeDirectoryPath "include/sequential" ABSOLUTE)
set(SEQUENTIAL_INCLUDE_DIR ${includeDirectoryPath}
    CACHE INTERNAL "Sequential implementation Include Directory" FORCE
)
