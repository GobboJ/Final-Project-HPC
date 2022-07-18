# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.21)

# The project is written in C++ 20
project(Final_Project_HPC_Test CXX)
set(CMAKE_CXX_STANDARD 20)

function (set_test_resources)

    # Clear the result
    set(resourcesPaths)
    set(copiedResourcesPaths)

    # Loop over the arguments
    foreach (argument IN LISTS ARGN)
        # Transform the relative path into an absolute path
        get_filename_component(resourceAbsolutePath "${argument}" ABSOLUTE)
        # Retrieve the name of the resource file
        get_filename_component(resourceFileName "${argument}" NAME)

        # Check the existence of the file
        if (NOT EXISTS ${resourceAbsolutePath})
            message(FATAL_ERROR "set_test_resources: File ${resourceAbsolutePath} does not exist")
        endif ()
        # Add the path to the final lists
        list(APPEND resourcesPaths "${resourceAbsolutePath}")
        list(APPEND copiedResourcesPaths "${CMAKE_CURRENT_BINARY_DIR}/${resourceFileName}")
    endforeach ()

    # Copy the resources in the build directory
    add_custom_command(
        # Copied resources files
        OUTPUT ${copiedResourcesPaths}
        # Resources files, which once modified trigger the copy
        DEPENDS ${resourcesPaths}
        # Command to execute
        COMMAND ${CMAKE_COMMAND} -E copy "${resourcesPaths}" "${CMAKE_CURRENT_BINARY_DIR}"
        # Expand the CMake list into a space-separated string
        COMMAND_EXPAND_LISTS
        # Comment
        COMMENT "Copying test resources"
    )

    # Create a variable in the parent scope containing the paths of the copied resources files
    set(testResources ${copiedResourcesPaths} PARENT_SCOPE)
endfunction ()

# Set the resources used by the test programs
set_test_resources(resources/a.data)
# Define the test executable
add_executable(
    Final_Project_HPC_Test
    ${testResources} src/main.cpp
)

#
# Tests
#
add_test(TestA Final_Project_HPC_Test "abc")
add_test(TestB Final_Project_HPC_Test "bcd")
