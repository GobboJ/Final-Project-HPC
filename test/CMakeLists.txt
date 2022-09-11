# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC_Test CXX)
set(CMAKE_CXX_STANDARD 20)

# Include the headers
include_directories(${SEQUENTIAL_IMPLEMENTATION_INCLUDE_DIR})
include_directories(${PARALLEL_IMPLEMENTATION_INCLUDE_DIR})

# Define the main executable
add_executable(
    Final_Project_HPC_Main
    src/cli/CliArgumentException.cpp
    src/cli/CliArgumentException.h
    src/cli/CliArguments.cpp
    src/cli/CliArguments.h
    src/cli/CliArgumentsParser.cpp
    src/cli/CliArgumentsParser.h
    src/ClusteringAlgorithmExecutor.cpp
    src/ClusteringAlgorithmExecutor.h
    src/data/DataReader.cpp
    src/data/DataReader.h
    src/data/DataWriter.cpp
    src/data/DataWriter.h
    src/data/IoException.cpp
    src/data/IoException.h
    src/data/MalformedFileException.cpp
    src/data/MalformedFileException.h
    src/data/ResultsChecker.cpp
    src/data/ResultsChecker.h
    src/main.cpp
)
target_link_libraries(
    Final_Project_HPC_Main Final_Project_HPC_Sequential_Library Final_Project_HPC_Parallel_Library
)
target_compile_definitions(Final_Project_HPC_Main PRIVATE TIMERS PRINT_ITERATIONS)

# Define the measurements main executables
foreach (isParallel RANGE 0 1)
    foreach (version RANGE 1 13)
        # Set the name
        if (isParallel EQUAL 0)
            set(name Sequential)
        else ()
            set(name Parallel)
        endif ()

        # Create the target
        if (isParallel EQUAL 1 OR (isParallel EQUAL 0 AND version LESS 3))
            add_executable(
                Final_Project_HPC_Measurement_${name}_${version}
                src/data/DataReader.cpp
                src/data/DataReader.h
                src/data/DataWriter.cpp
                src/data/DataWriter.h
                src/data/IoException.cpp
                src/data/IoException.h
                src/data/MalformedFileException.cpp
                src/data/MalformedFileException.h
                src/data/ResultsChecker.cpp
                src/data/ResultsChecker.h
                src/main-measurements.cpp
            )

            target_link_libraries(
                Final_Project_HPC_Measurement_${name}_${version}
                Final_Project_HPC_Sequential_Library Final_Project_HPC_Parallel_Library
            )

            target_compile_definitions(
                Final_Project_HPC_Measurement_${name}_${version}
                PRIVATE TIMERS
                # PRINT_ITERATIONS
                IS_PARALLEL=${isParallel}
                VERSION=${version}
            )
        endif ()
    endforeach ()
endforeach ()

# Define the data generator main executable
add_executable(
    Data_Generator
    src/main-data-generator.cpp
    )

# Define the mean execution times printer
add_executable(
        Mean_Times_Printer
        src/main-main-times-printer.cpp
)

# Define the automated tests
set(testList)
# list(APPEND testList "two-points" "samples" "same-distance" "slide" "iris" "birm")
list(APPEND testList "iris")

foreach (i RANGE 1 7)
    foreach (testName IN LISTS testList)
        add_test(
            "${testName}-parallel-v${i}"
            Final_Project_HPC_Main
            -t
            -p
            ${i}
            "${testName}"
        )
    endforeach ()
endforeach ()

# add_test(TwoPoints Final_Project_HPC_Main -t -p 1 two-points) add_test(Samples
# Final_Project_HPC_Main -t -p 1 samples) add_test(SameDistance Final_Project_HPC_Main -t -p 1
# same-distance) add_test(Slide Final_Project_HPC_Main -t -p 1 slide)

# add_subdirectory(other)
