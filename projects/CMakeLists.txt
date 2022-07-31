# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC_Sequential CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)
add_compile_definitions(TIMERS)
add_compile_definitions(PRINT_ITERATIONS)

add_library(
    Final_Project_HPC_Sequential_Library STATIC
    include/sequential/SequentialClustering.h
    include/utils/Types.h
    src/utils/Logger.h
    src/utils/Timer.cpp
    src/utils/Timer.h
)

add_library(
    Final_Project_HPC_Parallel_Library STATIC
    include/parallel/ParallelClustering.h
    include/utils/Types.h
    src/utils/Logger.h
    src/utils/Timer.cpp
    src/utils/Timer.h
)
target_link_libraries(Final_Project_HPC_Parallel_Library PRIVATE OpenMP::OpenMP_CXX)

# Export the include directory
get_filename_component(sequentialIncludeDirectoryPath "include/sequential" ABSOLUTE)
set(SEQUENTIAL_IMPLEMENTATION_INCLUDE_DIR ${sequentialIncludeDirectoryPath}
    CACHE INTERNAL "Sequential implementation include directory" FORCE
)

get_filename_component(parallelIncludeDirectoryPath "include/parallel" ABSOLUTE)
set(PARALLEL_IMPLEMENTATION_INCLUDE_DIR ${parallelIncludeDirectoryPath}
    CACHE INTERNAL "Parallel implementations include directory" FORCE
)
