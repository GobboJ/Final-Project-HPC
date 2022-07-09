# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC CXX)
set(CMAKE_CXX_STANDARD 20)

# Include the compiler options
include(cmake/CompilerOptions.cmake)

# Include the two sub-projects
add_subdirectory(projects/sequential)
add_subdirectory(projects/parallel)
