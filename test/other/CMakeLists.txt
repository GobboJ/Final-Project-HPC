# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC_Test CXX)
set(CMAKE_CXX_STANDARD 20)

# Include the headers
include_directories(${SEQUENTIAL_IMPLEMENTATION_INCLUDE_DIR})
include_directories(${PARALLEL_IMPLEMENTATION_INCLUDE_DIR})

# Define the test types executable
add_executable(
    Final_Project_HPC_Test_Types
    ../src/data/DataReader.cpp
    ../src/data/DataReader.h
    ../src/data/DataWriter.cpp
    ../src/data/DataWriter.h
    ../src/data/IoException.cpp
    ../src/data/IoException.h
    ../src/data/MalformedFileException.cpp
    ../src/data/MalformedFileException.h
    ../src/data/ResultsChecker.cpp
    ../src/data/ResultsChecker.h
    main-test-types.cpp
    types/CollectionCreator.h
    types/collections/AlignedArray.h
    types/collections/Alignments.h
    types/collections/OnlyConstIterableVector.h
    types/collections/OnlyIterableVector.h
    types/containers/array/ArrayNonIterableSubContainer.h
    types/containers/array/ArrayOnlyConstIterableSubContainer.h
    types/containers/array/ArrayOnlyIterableSubContainer.h
    types/containers/array/ArraySubContainer.h
    types/containers/array/ArrayTypeNameComposer.h
    types/containers/collection/CollectionContainer.h
    types/containers/collection/CollectionNonIterableSubContainer.h
    types/containers/collection/CollectionOnlyConstIterableSubContainer.h
    types/containers/collection/CollectionOnlyIterableSubContainer.h
    types/containers/collection/CollectionSubContainer.h
    types/containers/collection/CollectionTypeNameComposer.h
    types/containers/CollectionInfo.h
    types/containers/LinearCollectionContainer.h
    types/containers/PiLambdaContainer.h
    types/containers/pointer/PointerContainer.h
    types/containers/pointer/PointerNonIterableSubContainer.h
    types/containers/pointer/PointerOnlyConstIterableSubContainer.h
    types/containers/pointer/PointerOnlyIterableSubContainer.h
    types/containers/pointer/PointerSubContainer.h
    types/containers/pointer/PointerTypeNameComposer.h
    types/DataTypesTester.h
    types/PiLambdaTypesTester.h
    types/TypesPrinter.h)

target_link_libraries(
    Final_Project_HPC_Test_Types Final_Project_HPC_Sequential_Library
    Final_Project_HPC_Parallel_Library
)

# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE TIMERS PRINT_ITERATIONS) TODO:
# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE ITERATORS_SUMMARY)
target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE ITERATORS_SUMMARY_TEST)

add_executable(
    Final_Project_HPC_Test_Array_Types
    ../src/data/DataReader.cpp
    ../src/data/DataReader.h
    ../src/data/DataWriter.cpp
    ../src/data/DataWriter.h
    ../src/data/IoException.cpp
    ../src/data/IoException.h
    ../src/data/MalformedFileException.cpp
    ../src/data/MalformedFileException.h
    ../src/data/ResultsChecker.cpp
    ../src/data/ResultsChecker.h
    main-test-array-types.cpp
    types/CollectionCreator.h
    types/collections/AlignedArray.h
    types/collections/Alignments.h
    types/collections/OnlyConstIterableVector.h
    types/collections/OnlyIterableVector.h
    types/containers/array/ArrayNonIterableSubContainer.h
    types/containers/array/ArrayOnlyConstIterableSubContainer.h
    types/containers/array/ArrayOnlyIterableSubContainer.h
    types/containers/array/ArraySubContainer.h
    types/containers/array/ArrayTypeNameComposer.h
    types/containers/CollectionInfo.h
    types/DataTypesTester.h
    types/DataTypesTest.h
)

target_link_libraries(
    Final_Project_HPC_Test_Array_Types Final_Project_HPC_Sequential_Library
    Final_Project_HPC_Parallel_Library
)

# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE TIMERS PRINT_ITERATIONS) TODO:
# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE ITERATORS_SUMMARY)
target_compile_definitions(Final_Project_HPC_Test_Array_Types PRIVATE ITERATORS_SUMMARY_TEST)


add_executable(
        Final_Project_HPC_Test_Lists_Types
        ../src/data/DataReader.cpp
        ../src/data/DataReader.h
        ../src/data/DataWriter.cpp
        ../src/data/DataWriter.h
        ../src/data/IoException.cpp
        ../src/data/IoException.h
        ../src/data/MalformedFileException.cpp
        ../src/data/MalformedFileException.h
        ../src/data/ResultsChecker.cpp
        ../src/data/ResultsChecker.h
        main-test-lists.cpp
        types/collections/AlignedArray.h
        types/collections/Alignments.h
        types/collections/OnlyConstIterableVector.h
        types/collections/OnlyIterableVector.h
        types/DataTypesTester.h
        types/DataTypesTest.h
)

target_link_libraries(
        Final_Project_HPC_Test_Lists_Types Final_Project_HPC_Sequential_Library
        Final_Project_HPC_Parallel_Library
)

# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE TIMERS PRINT_ITERATIONS) TODO:
# target_compile_definitions(Final_Project_HPC_Test_Types PRIVATE ITERATORS_SUMMARY)
