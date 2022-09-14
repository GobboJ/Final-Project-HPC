# Projects

This directory contains all the files offered by the library.

In particular:

- The `include` directory contains all the header files that you need to include to use the
  library. This directory contains the implementations of all the clustering algorithms, together
  with some
  utility classes;
- The `src` directory contains the actual code of the utility classes.

The `CMakeLists.txt` file in this directory offers two useful global variables, namely:

- The `SEQUENTIAL_IMPLEMENTATION_INCLUDE_DIR` variable, which refers to the directories of the
  library containing the header files required to use the sequential implementation of the
  clustering algorithm;
- The `PARALLEL_IMPLEMENTATION_INCLUDE_DIR` variable, which refers to the directories of the library
  containing the header files required to use the parallel implementations of the clustering
  algorithm.

To use these variables, just include in your main CMake file the `CMakeLists.txt` file in this
directory.

## How to build the library

The `CMakeLists.txt` file offers also the targets that can be used to compile the code of the
library. In particular:

- The `Final_Project_HPC_Sequential_Library` target allows to compile all the files required by the
  sequential implementation of the clustering algorithm;
- The `Final_Project_HPC_Parallel_Library`  target allows to compile all the files required by the
  parallel implementations of the clustering algorithm.

You can build the library using the G++ compiler either on Windows or on Linux.
Moreover, you can directly use those targets as link dependencies in your CMake applications, so
that CMake will take care of compiling the code for you.
