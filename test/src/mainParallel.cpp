#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include "data/DataReader.h"
#include "ParallelClustering.h"
#include "data/DataWriter.h"

static const constexpr int VERSION = 2;

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "Too few arguments" << std::endl;
        return 1;
    }
    std::string fileName{argv[1]};
    unsigned long startColumnIndex = -1;
    unsigned long endColumnIndex = -1;
    
    if (fileName == "pb") {
        fileName = "Parking Birmingham.data";
        startColumnIndex = 2;
        endColumnIndex = 3;
    } else if (fileName == "slide") {
        fileName = "slide.data";
        startColumnIndex = 1;
        endColumnIndex = 2;
    } else if (fileName == "acc") {
        fileName = "accelerometer.csv";
        startColumnIndex = 3;
        endColumnIndex = 4;
    } else {
        if (argc < 4) {
            std::cerr << "Too few arguments" << std::endl;
            return 1;
        }
        char *nextNonParsedIndex = &(argv[2][0]);
        startColumnIndex = std::strtoul(argv[2], &nextNonParsedIndex, 10);
        if (nextNonParsedIndex == &(argv[2][0])) {
            std::cerr << "Wrong start column index" << std::endl;
            return 1;
        }
        nextNonParsedIndex = &(argv[3][0]);
        endColumnIndex = std::strtoul(argv[3], &nextNonParsedIndex, 10);
        if (nextNonParsedIndex == &(argv[3][0])) {
            std::cerr << "Wrong end column index" << std::endl;
            return 1;
        }
    }

    if (endColumnIndex < startColumnIndex) {
        std::cerr << "Wrong start and end column index" << std::endl;
        return 1;
    }

    const unsigned long dimension = endColumnIndex - startColumnIndex + 1;

    std::vector<double *> data =
            DataReader::readData(startColumnIndex - 1, endColumnIndex - 1, fileName);

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::cout << "Started at " << std::put_time(&tm, "%H:%M:%S") << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::size_t> pi{};
    std::vector<double> lambda{};

    if constexpr (VERSION == 1) {
        ParallelClustering::clusterV1(data, dimension, pi, lambda);
    } else if constexpr (VERSION == 2) {
        ParallelClustering::clusterV2(data, dimension, pi, lambda);
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = end - start;

    using namespace std::literals::chrono_literals;

    auto nanoseconds = (duration % 1s) / 1ns;
    auto milliseconds = (duration % 1s) / 1ms;
    auto seconds = (duration % 1min) / 1s;
    auto minute = duration / 1min;

    std::string nanos{};
    std::string zero{"0"};
    while (nanoseconds > 0) {
        int number = nanoseconds % 1000;
        std::string str = std::to_string(number);
        while (str.length() < 3) {
            str = zero + str;
        }
        if (nanos.empty()) {
            nanos = str;
        } else {
            nanos = str + "." + nanos;
        }
        nanoseconds /= 1000;
    }

    std::cout << minute << " minutes " << seconds << " seconds " << milliseconds << " ms (" << nanos
              << " ns)" << std::endl;

    std::filesystem::path outputDirectory{".."};
    outputDirectory = outputDirectory / ".." / "visualizer" / "DeB_Cluster_Visualizer" / "out";

    DataWriter::createOutputFile(outputDirectory / "out.txt", data, dimension, pi, lambda);
    DataWriter::createMathematicaOutputFile(outputDirectory / "mat.txt", pi, lambda);

    return 0;
}
