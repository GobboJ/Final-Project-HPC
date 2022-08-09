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
        src/data/DataReader.cpp
        src/data/DataReader.h
        src/data/DataWriter.cpp
        src/data/DataWriter.h
        src/data/IoException.cpp
        src/data/IoException.h
        src/data/MalformedFileException.cpp
        src/data/MalformedFileException.h
        src/main.cpp
        src/ResultsChecker.cpp
        src/ResultsChecker.h
)
target_link_libraries(
        Final_Project_HPC_Main Final_Project_HPC_Sequential_Library Final_Project_HPC_Parallel_Library
)
target_compile_definitions(Final_Project_HPC_Main PRIVATE TIMERS PRINT_ITERATIONS)

# Define the measurements main executable
add_executable(
        Final_Project_HPC_Measurement
        src/data/DataReader.cpp
        src/data/DataReader.h
        src/data/DataWriter.cpp
        src/data/DataWriter.h
        src/data/IoException.cpp
        src/data/IoException.h
        src/data/MalformedFileException.cpp
        src/data/MalformedFileException.h
        src/measurements.cpp
        src/ResultsChecker.cpp
        src/ResultsChecker.h
)

target_link_libraries(
        Final_Project_HPC_Measurement Final_Project_HPC_Sequential_Library
        Final_Project_HPC_Parallel_Library
)

target_compile_definitions(Final_Project_HPC_Measurement PRIVATE TIMERS PRINT_ITERATIONS)

# Define the test types executable
add_executable(
        Final_Project_HPC_Test_Types
        src/data/DataReader.cpp
        src/data/DataReader.h
        src/data/DataWriter.cpp
        src/data/DataWriter.h
        src/data/IoException.cpp
        src/data/IoException.h
        src/data/MalformedFileException.cpp
        src/data/MalformedFileException.h
        src/main-test-types.cpp
        src/ResultsChecker.cpp
        src/ResultsChecker.h
        src/types/AlignedArray.h
        src/types/CollectionCreator.h
        src/types/PointerWrapper.h
        src/types/CollectionContainer.h
        src/types/ArrayCollectionContainer.h src/types/LinearCollectionContainer.h)

target_link_libraries(
        Final_Project_HPC_Test_Types Final_Project_HPC_Sequential_Library
        Final_Project_HPC_Parallel_Library
)

#TODO: target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE ITERATORS_SUMMARY)

# Define the data generator main executable
add_executable(
        Data_Generator
        src/data-generator.cpp
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
