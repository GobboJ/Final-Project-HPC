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

target_compile_definitions(Final_Project_HPC_Measurement PRIVATE TIMERS PRINT_ITERATIONS ITERATORS_SUMMARY)

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
    src/types/CollectionCreator.h
    src/types/collections/AlignedArray.h
    src/types/collections/Alignments.h
    src/types/collections/OnlyConstIterableVector.h
    src/types/collections/OnlyIterableVector.h
    src/types/containers/array/ArrayNonIterableSubContainer.h
    src/types/containers/array/ArrayOnlyConstIterableSubContainer.h
    src/types/containers/array/ArrayOnlyIterableSubContainer.h
    src/types/containers/array/ArraySubContainer.h
    src/types/containers/array/ArrayTypeNameComposer.h
    src/types/containers/collection/CollectionContainer.h
    src/types/containers/collection/CollectionNonIterableSubContainer.h
    src/types/containers/collection/CollectionOnlyConstIterableSubContainer.h
    src/types/containers/collection/CollectionOnlyIterableSubContainer.h
    src/types/containers/collection/CollectionSubContainer.h
    src/types/containers/collection/CollectionTypeNameComposer.h
    src/types/containers/CollectionInfo.h
    src/types/containers/LinearCollectionContainer.h
    src/types/containers/PiLambdaContainer.h
    src/types/containers/pointer/PointerContainer.h
    src/types/containers/pointer/PointerNonIterableSubContainer.h
    src/types/containers/pointer/PointerOnlyConstIterableSubContainer.h
    src/types/containers/pointer/PointerOnlyIterableSubContainer.h
    src/types/containers/pointer/PointerSubContainer.h
    src/types/containers/pointer/PointerTypeNameComposer.h
    src/types/DataTypesTester.h
    src/types/PiLambdaTypesTester.h
    src/types/TypesPrinter.h)

target_link_libraries(
    Final_Project_HPC_Test_Types Final_Project_HPC_Sequential_Library
    Final_Project_HPC_Parallel_Library
)

# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE TIMERS PRINT_ITERATIONS) TODO:
# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE ITERATORS_SUMMARY)
target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE ITERATORS_SUMMARY_TEST)

add_executable(
    Final_Project_HPC_Test_Array_Types
    src/data/DataReader.cpp
    src/data/DataReader.h
    src/data/DataWriter.cpp
    src/data/DataWriter.h
    src/data/IoException.cpp
    src/data/IoException.h
    src/data/MalformedFileException.cpp
    src/data/MalformedFileException.h
    src/main-test-array-types.cpp
    src/ResultsChecker.cpp
    src/ResultsChecker.h
    src/types/CollectionCreator.h
    src/types/collections/AlignedArray.h
    src/types/collections/Alignments.h
    src/types/collections/OnlyConstIterableVector.h
    src/types/collections/OnlyIterableVector.h
    src/types/containers/array/ArrayNonIterableSubContainer.h
    src/types/containers/array/ArrayOnlyConstIterableSubContainer.h
    src/types/containers/array/ArrayOnlyIterableSubContainer.h
    src/types/containers/array/ArraySubContainer.h
    src/types/containers/array/ArrayTypeNameComposer.h
    src/types/containers/CollectionInfo.h
    src/types/DataTypesTester.h
 #     src/types/DataTypesTest.h)
)

target_link_libraries(
    Final_Project_HPC_Test_Array_Types Final_Project_HPC_Sequential_Library
    Final_Project_HPC_Parallel_Library
)

# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE TIMERS PRINT_ITERATIONS) TODO:
# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE ITERATORS_SUMMARY)
target_compile_definitions(Final_Project_HPC_Test_Array_Types PRIVATE ITERATORS_SUMMARY_TEST)

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
