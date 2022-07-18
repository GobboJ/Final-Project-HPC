# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC_Sequential CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

add_library(
    Final_Project_HPC_Sequential STATIC
    include/sequential/SequentialClustering.h src/sequential/SequentialClustering.cpp
)

add_library(
    Final_Project_HPC_Parallel STATIC
    include/parallel/ParallelClustering.h src/parallel/ParallelClustering.cpp
)
target_link_libraries(Final_Project_HPC_Parallel PRIVATE OpenMP::OpenMP_CXX)

# Export the include directory
get_filename_component(sequentialIncludeDirectoryPath "include/sequential" ABSOLUTE)
set(SEQUENTIAL_INCLUDE_DIR ${sequentialIncludeDirectoryPath}
    CACHE INTERNAL "Sequential implementation Include Directory" FORCE
)

get_filename_component(parallelIncludeDirectoryPath "include/parallel" ABSOLUTE)
set(PARALLEL_INCLUDE_DIR ${parallelIncludeDirectoryPath}
    CACHE INTERNAL "Parallel implementations Include Directory" FORCE
)
