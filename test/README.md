# Test

This directory contains several applications used to test the correctness and the performance of the
implementations of the clustering algorithm.

The `src` directory contains the main test applications. In particular:

- The `main.cpp` is the main file of the application that allows to test the correctness and the
  performance of one of the implementations of the clustering algorithm;
- The `main-data-generator.cpp` is the main file of the application that creates a file containing
  a randomly generated dataset;
- The `main-measurements.cpp` is the main file of the application that runs an implementation of the
  clustering algorithm so to measure its running time;
- The `main-mean-times-printer.cpp` is the main file of the application that computes the mean
  execution time of 3 executions of one implementation of the clustering algorithm. Refer
  to [Measuring the execution time](#measuring-the-execution-time) for more information.

To list all the options each application supports, just run it specifying the `-h` option.

The `main-sample.cpp` file, instead, contains a simple interactive application showing how to use
the
library.

Finally, the `other` directory contains some applications that test whether the developed
implementations accept the correct C++ types for the data structure holding the samples to
cluster, and the data structures holding the values of `pi` and `lambda`.\
Refer to the `README.md` file inside that directory for further information.

## Measuring the execution time

The `CMakeLists.txt` file in this folder provides one target for each implementation of the
clustering algorithm.
This allows to test in isolation one execution of a specific implementation.

The `build-all.sh` file contains a convenient script that builds all those targets at once. This
avoids to manually build each target.

The `run-measurements` file, instead, contains a convenient script that measures the mean execution
time of 3 executions of all the implementations on two different datasets.\
To do so, it runs 3 times the executable that measure the running time of a specific implementation,
collects the result in a file that will be created in the current working directory, and then runs
the `Mean_Times_Printer` executable to compute the mean execution time.
