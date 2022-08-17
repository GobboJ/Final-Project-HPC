# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC_Sequential CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)
add_compile_definitions(TIMERS)
add_compile_definitions(PRINT_ITERATIONS)

# Define the file needed by the sequential implementation of the clustering algorithm
add_library(
    Final_Project_HPC_Sequential_Library STATIC
    include/sequential/SequentialClustering.h
    include/utils/DataIteratorUtils.h
    include/utils/IteratorType.h
    include/utils/Logger.h
    include/utils/PiLambdaIteratorUtils.h
    include/utils/Timer.h
    include/utils/Types.h
    src/utils/DataIteratorUtils.cpp
    src/utils/PiLambdaIteratorUtils.cpp
    src/utils/Timer.cpp
)

# Define the file needed by the parallel implementation of the clustering algorithm
add_library(
    Final_Project_HPC_Parallel_Library STATIC
    include/parallel/DistanceComputers.h
    include/parallel/ParallelClustering.h
    include/utils/DataIteratorUtils.h
    include/utils/IteratorType.h
    include/utils/Logger.h
    include/utils/PiLambdaIteratorUtils.h
    include/utils/Timer.h
    include/utils/Types.h
    src/utils/DataIteratorUtils.cpp
    src/utils/PiLambdaIteratorUtils.cpp
    src/utils/Timer.cpp
)
target_link_libraries(Final_Project_HPC_Parallel_Library PRIVATE OpenMP::OpenMP_CXX)

# Export the include directory so to make available the two implementations of the clustering
# algorithm
get_filename_component(utilsIncludeDirectoryPath "include/utils" ABSOLUTE)
get_filename_component(sequentialIncludeDirectoryPath "include/sequential" ABSOLUTE)
set(SEQUENTIAL_IMPLEMENTATION_INCLUDE_DIR
    ${sequentialIncludeDirectoryPath} ${utilsIncludeDirectoryPath}
    CACHE INTERNAL "Sequential implementation include directory" FORCE
)

get_filename_component(parallelIncludeDirectoryPath "include/parallel" ABSOLUTE)
set(PARALLEL_IMPLEMENTATION_INCLUDE_DIR ${parallelIncludeDirectoryPath} ${utilsIncludeDirectoryPath}
    CACHE INTERNAL "Parallel implementations include directory" FORCE
)
