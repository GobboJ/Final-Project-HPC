# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC CXX)
set(CMAKE_CXX_STANDARD 20)

# Include the compiler options
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    include(cmake/CompilerOptionsWindows.cmake)
else ()
    include(cmake/CompilerOptionsLinux.cmake)
endif ()

# Enable CTest
enable_testing()

# Include the two sub-projects
add_subdirectory(projects/src/sequential)
add_subdirectory(projects/src/parallel)
add_subdirectory(test)
