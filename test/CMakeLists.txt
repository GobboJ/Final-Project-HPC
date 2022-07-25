# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC_Test CXX)
set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(TIMERS)
add_compile_definitions(PRINT_ITERATIONS)

# Include the headers
include_directories(${SEQUENTIAL_IMPLEMENTATION_INCLUDE_DIR})
include_directories(${PARALLEL_IMPLEMENTATION_INCLUDE_DIR})

# Define the test executable
add_executable(
    Final_Project_HPC_Test
    ${testResources}
    src/main-test.cpp
    src/data/DataReader.cpp
    src/data/DataReader.h
    src/data/MalformedFileException.cpp
    src/data/MalformedFileException.h
    src/data/DataWriter.cpp
    src/data/DataWriter.h
)
target_link_libraries(
    Final_Project_HPC_Test Final_Project_HPC_Sequential_Library Final_Project_HPC_Parallel_Library
)

add_executable(
    Final_Project_HPC_Main
    ${testResources}
    src/data/DataReader.cpp
    src/data/DataReader.h
    src/data/DataWriter.cpp
    src/data/DataWriter.h
    src/main.cpp
    src/data/MalformedFileException.cpp
    src/data/MalformedFileException.h
)
target_link_libraries(
    Final_Project_HPC_Main Final_Project_HPC_Sequential_Library Final_Project_HPC_Parallel_Library
)

add_executable(
    Data_Generator
    src/data-generator.cpp
)

# add_executable( Final_Project_HPC_Test_Parallel ${testResources} src/data/DataReader.cpp
# src/data/DataReader.h src/data/DataWriter.cpp src/data/DataWriter.h src/mainParallel.cpp
# src/data/MalformedFileException.cpp src/data/MalformedFileException.h )
# target_link_libraries(Final_Project_HPC_Test_Parallel Final_Project_HPC_Parallel)

#
# Tests
#
add_test(TwoPoints Final_Project_HPC_Test "two-points")
add_test(Samples Final_Project_HPC_Test "samples")
add_test(SameDistance Final_Project_HPC_Test "same-distance")
add_test(Slide Final_Project_HPC_Test "slide")
