#!/bin/sh

# Format all the CMake configuration files
exec find ".." \
    \( -name "*.cmake" -o -name "CMakeLists.txt" \) ! -path "*/cmake-build-*" \
    -exec cmake-format -c cmake-format.yaml -i {} \;
