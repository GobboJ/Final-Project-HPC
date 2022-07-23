#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <unordered_map>
#include <functional>
#include <cstring>
#include <xmmintrin.h>
#include "data/DataReader.h"
#include "SequentialClustering.h"
#include "data/DataWriter.h"
#include "ParallelClustering.h"

template <typename C, typename D>
void printElapsedTime(std::chrono::time_point<C, D> start, std::chrono::time_point<C, D> end);

void printValues(const std::vector<double *> &data,
                 std::size_t dimension,
                 const std::vector<std::size_t> &pi,
                 const std::vector<double> &lambda);

void usage();

bool parseSizeT(char *string, std::size_t &result);

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
int main(int argc, char *argv[]) {

    if (argc < 3) {
        std::cerr << "Too few arguments" << std::endl;
        usage();
        return 1;
    }

    std::unordered_map<std::string, std::tuple<std::string, std::size_t, std::size_t>> tests = {
            // Sequential :  6 s 362.728.561 ns
            // Parallel v1:  3 s 984.201.053 ns
            // Parallel v2:  3 s 910.884.877 ns
            // Parallel v3:  3 s 932.660.255 ns
            {"birm", {"Parking Birmingham.data", 2, 3}},
            {"slide", {"slide.data", 1, 2}},
            // Sequential: 122.690 ns
            {"iris", {"iris.data", 1, 4}},
            // Sequential :  2 m 10 s 448.206.284 ns
            // Parallel v1:  1 m 44 s 508.938.089 ns
            // Parallel v2:  1 m 38 s 773.972.452 ns
            // Parallel v3:  1 m 33 s 123.307.274 ns
            {"acc", {"accelerometer.csv", 3, 5}},
            {"gen", {"generated.data", 1, 32}}};

    std::string option{argv[1]};
    std::size_t version = 0;
    std::string fileName{};
    // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
    bool isParallel;

    if (option == "-p") {
        if (argc < 4) {
            std::cerr << "Too few arguments for option -p" << std::endl;
            usage();
            return 1;
        }
        if (!parseSizeT(argv[2], version)) {
            std::cerr << "Wrong version number" << std::endl;
            usage();
            return 1;
        }
        fileName = argv[3];
        isParallel = true;
    } else if (option == "-s") {
        fileName = argv[2];
        isParallel = false;
    } else {
        std::cerr << "Unknown option" << ' ' << option << std::endl;
        usage();
        return 1;
    }

    std::size_t startColumnIndex = 0;
    std::size_t endColumnIndex = 0;

    if (tests.contains(fileName)) {
        std::tuple<std::string, std::size_t, std::size_t> test = tests[fileName];
        fileName = std::get<0>(test);
        startColumnIndex = std::get<1>(test);
        endColumnIndex = std::get<2>(test);
    } else {
        // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
        char *start;
        // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
        char *end;

        if (isParallel) {
            if (argc < 6) {
                std::cerr << "Too few arguments for start and end" << std::endl;
                return 1;
            }
            start = argv[4];
            end = argv[5];
        } else {
            if (argc < 5) {
                std::cerr << "Too few arguments for start and end" << std::endl;
                return 1;
            }
            start = argv[3];
            end = argv[4];
        }

        if (!parseSizeT(start, startColumnIndex)) {
            std::cerr << "Wrong start column index" << std::endl;
            usage();
            return 1;
        }
        if (!parseSizeT(end, endColumnIndex)) {
            std::cerr << "Wrong end column index" << std::endl;
            usage();
            return 1;
        }

        if (endColumnIndex < startColumnIndex) {
            std::cerr << "Wrong start and end column index" << std::endl;
            return 1;
        }
    }

    std::size_t dimension = endColumnIndex - startColumnIndex + 1;

    // Read the data
    std::vector<double *> data =
            DataReader::readData(startColumnIndex - 1, endColumnIndex - 1, fileName);

    double *reallocatedData = nullptr;

    if (isParallel) {
        std::size_t quotient = 1 + ((dimension - 1) / 4);
        std::size_t newDimension = 4 * quotient;
        std::size_t bytes = newDimension * sizeof(double);
        
        if (version == 2) {
            for (std::size_t i = 0; i < data.size(); i++) {
                auto *reallocated = static_cast<double *>(_mm_malloc(bytes, 32));
                memcpy(reallocated, data[i], dimension * sizeof(double));
                memset(&(reallocated[dimension]), 0, (newDimension - dimension) * sizeof(double));
                delete data[i];
                data[i] = reallocated;
            }
        } else if (version == 3) {
            auto a = bytes * data.size();
            reallocatedData = static_cast<double *>(_mm_malloc(bytes * data.size(), 32));
            memset(reallocatedData, 0, bytes * data.size());
            
            std::size_t start = 0;
            for (std::size_t i = 0; i < data.size(); i++) {
                memcpy(&(reallocatedData[start]), data[i], dimension * sizeof(double));
                delete data[i];
                start += newDimension;
            }
        }
    }

    // Decide the clustering algorithm
    std::function<void(std::vector<std::size_t> &, std::vector<double> &)> clusteringAlgorithm;

    if (isParallel) {
        switch (version) {
            case 1:
                std::cout << "Parallel clustering version 1";
                clusteringAlgorithm = [&data, &dimension](auto &pi, auto &lambda) noexcept -> void {
                    ParallelClustering::clusterV1(data, dimension, pi, lambda);
                };
                break;
            case 2:
                std::cout << "Parallel clustering version 2";
                clusteringAlgorithm = [&data, &dimension](auto &pi, auto &lambda) noexcept -> void {
                    ParallelClustering::clusterV2(data, dimension, pi, lambda);
                };
                break;
            case 3:
                std::cout << "Parallel clustering version 3";
                clusteringAlgorithm = [&data, &dimension, reallocatedData](auto &pi,
                                                                           auto &lambda) noexcept ->  void {
                    ParallelClustering::clusterV3(
                            reallocatedData, dimension, data.size(), pi, lambda);
                };
                break;
            default:
                std::cerr << "Unknown version" << ' ' << version << std::endl;
                return 1;
        }
    } else {
        std::cout << "Sequential clustering";
        clusteringAlgorithm = [&data, &dimension](auto &pi, auto &lambda) noexcept -> void {
            SequentialClustering::cluster(data, dimension, pi, lambda);
        };
    }
    std::cout << " of '" << fileName << "' (columns from " << startColumnIndex << " to "
              << endColumnIndex << ')';

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::cout << " started at " << std::put_time(&tm, "%H:%M:%S") << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::size_t> pi{};
    std::vector<double> lambda{};

    clusteringAlgorithm(pi, lambda);

    auto end = std::chrono::high_resolution_clock::now();

    // Print the values
    // printValues(data, dimension, pi, lambda);

    // Print the elapsed time
    printElapsedTime(start, end);

    // Output the files
    std::cout << "Outputting result: ";
    if (data.size() < 100) {
        std::cout << "yes" << std::endl;
        std::filesystem::path outputDirectory{".."};
        outputDirectory = outputDirectory / ".." / "out";

        DataWriter::createOutputFile(outputDirectory / "out.txt", data, dimension, pi, lambda);
        DataWriter::createMathematicaOutputFile(outputDirectory / "mat.txt", pi, lambda);
    } else {
        std::cout << "no" << std::endl;
    }

    return 0;
}

void usage() {

    std::cout << "Usage" << std::endl;
    std::cout << "main -s FILENAME [START END] " << std::endl;
    std::cout << "main -p VERSION FILENAME [START END] " << std::endl;
}

template <typename C, typename D>
void printElapsedTime(std::chrono::time_point<C, D> start, std::chrono::time_point<C, D> end) {

    using namespace std::literals::chrono_literals;

    auto duration = end - start;

    auto nanoseconds = duration % 1us;
    auto microseconds = (duration % 1ms) / 1us;
    auto milliseconds = (duration % 1s) / 1ms;
    auto seconds = (duration % 1min) / 1s;
    auto minute = duration / 1min;

    std::cout << minute << " minutes " << seconds << " seconds " << milliseconds << " ms (";
    if (milliseconds > 0) {
        std::cout << std::setfill('0') << milliseconds;
        std::cout << ".";
    }
    if (microseconds > 0) {
        int width = (milliseconds > 0) ? 3 : 0;
        std::cout << std::setfill('0') << std::setw(width) << microseconds;
        std::cout << ".";
    }
    int width = (microseconds > 0) ? 3 : 0;
    std::cout << std::setfill('0') << std::setw(width) << nanoseconds.count();
    std::cout << " ns)" << std::endl;
}

void printValues(const std::vector<double *> &data,
                 std::size_t dimension,
                 const std::vector<std::size_t> &pi,
                 const std::vector<double> &lambda) {

    for (std::size_t i = 0; i < pi.size(); i++) {
        std::cout << "pi[" << i + 1 << "] = " << pi[i] + 1 << std::endl;
    }
    std::cout << std::endl << std::endl;
    for (std::size_t i = 0; i < pi.size(); i++) {
        std::cout << "l[" << i + 1 << "] = " << std::fixed << std::setprecision(2) << lambda[i]
                  << std::endl;
    }
    std::cout << std::endl;

    std::vector<std::string> values{};

    for (std::size_t i = 1; i < data.size(); i++) {
        std::cout << "M[" << i + 1 << "] = ";
        values.clear();

        for (std::size_t j = 0; j < i; j++) {
            double distance = 0;
            for (std::size_t d = 0; d < dimension; d++) {
                distance += pow(data[i][d] - data[j][i], 2.0);
            }

            distance = sqrt(distance);
            std::ostringstream stream{};
            stream << std::fixed << std::setprecision(2) << distance;
            std::cout << stream.str() << ' ';
            values.push_back(stream.str());
        }

        bool different = true;
        for (std::size_t k = 0; k < values.size() - 1; k++) {
            for (std::size_t j = k + 1; j < values.size(); j++) {
                if (values[k] == values[j]) {
                    different = false;
                }
            }
        }
        std::cout << ((!different) ? "false" : "true") << std::endl;
    }
}

bool parseSizeT(char *string, std::size_t &result) {

    char *nextNonParsedIndex = string;
    std::size_t convertedValue = std::strtoul(string, &nextNonParsedIndex, 10);

    if (nextNonParsedIndex == string) {
        return false;
    }

    result = convertedValue;
    return true;
}
