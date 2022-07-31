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
#include <atomic>
#include <unistd.h>
#include "data/DataReader.h"
#include "SequentialClustering.h"
#include "data/DataWriter.h"
#include "ParallelClustering.h"
#include "cli/CliArgumentsParser.h"

template <typename C, typename D>
void printElapsedTime(std::chrono::time_point<C, D> start, std::chrono::time_point<C, D> end);

void printValues(const std::vector<double *> &data,
                 std::size_t dimension,
                 const std::vector<std::size_t> &pi,
                 const std::vector<double> &lambda);

std::function<void(std::vector<std::size_t> &, std::vector<double> &)> getClusteringAlgorithm(
        bool isParallel,
        std::size_t version,
        std::size_t distanceComputationThreadsCount,
        std::size_t stage4ThreadsCount,
        std::vector<double *> &data,
        double *reallocatedData,
        std::size_t dimension);

bool checkTest(const std::filesystem::path &filePath,
               const std::vector<double *> &data,
               std::size_t dimension,
               const std::vector<std::size_t> &pi,
               const std::vector<double> &lambda);

bool areAlmostLess(const double first, const double second);

bool areAlmostEqual(const double first, const double second);

bool check(std::vector<std::pair<std::size_t, double>> &expected,
           std::vector<std::pair<std::size_t, double>> &result);

using DistanceComputers = ParallelClustering::DistanceComputers;

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
int main(int argc, char *argv[]) {

    CliArgumentsParser::CliArguments arguments = CliArgumentsParser::parseArguments(argc, argv);
    std::size_t dimension = arguments.getEndColumnIndex() - arguments.getStartColumnIndex() + 1;

    // Read the data
    std::vector<double *> data = DataReader::readData(arguments.getStartColumnIndex(),
                                                      arguments.getEndColumnIndex(),
                                                      arguments.getFilePath());

    double *reallocatedData = nullptr;

    bool isParallel = arguments.isParallel();
    std::size_t version = arguments.getParallelVersion();
    std::size_t distanceComputationThreadsCount = arguments.getDistanceComputationThreadsCount();
    std::size_t stage4ThreadsCount = arguments.getStage4ThreadsCount();

    if (isParallel) {
        std::size_t numberOfDoubles = (version == 2) ? 2 : 4;
        std::size_t quotient = 1 + ((dimension - 1) / numberOfDoubles);
        std::size_t newDimension = numberOfDoubles * quotient;
        std::size_t bytes = newDimension * sizeof(double);

        if (version != 1) {
            if (version == 2 || version == 3 || version == 5 || version == 6 || version == 7) {
                for (std::size_t i = 0; i < data.size(); i++) {
                    auto *reallocated = static_cast<double *>(
                            _mm_malloc(bytes, numberOfDoubles * sizeof(double)));
                    memcpy(reallocated, data[i], dimension * sizeof(double));
                    memset(&(reallocated[dimension]),
                           0,
                           (newDimension - dimension) * sizeof(double));
                    delete data[i];
                    data[i] = reallocated;
                }
            } else if (version == 4 || version == 8 || version == 9 || version == 10 ||
                       version == 11) {
                reallocatedData = static_cast<double *>(
                        _mm_malloc(bytes * data.size(), numberOfDoubles * sizeof(double)));
                memset(reallocatedData, 0, bytes * data.size());

                std::size_t start = 0;
                for (std::size_t i = 0; i < data.size(); i++) {
                    memcpy(&(reallocatedData[start]), data[i], dimension * sizeof(double));
                    start += newDimension;
                }
            } else {
                std::cerr << "Warning: Possibly unaligned data" << std::endl;
                sleep(20);
            }
        }
    }

    // Decide the clustering algorithm
    std::function<void(std::vector<std::size_t> &, std::vector<double> &)> clusteringAlgorithm =
            getClusteringAlgorithm(isParallel,
                                   version,
                                   distanceComputationThreadsCount,
                                   stage4ThreadsCount,
                                   data,
                                   reallocatedData,
                                   dimension);

    if (isParallel) {
        std::cout << "Parallel clustering version" << ' ' << version;
    } else {
        std::cout << "Sequential clustering";
    }
    std::cout << " of '"
              << std::filesystem::absolute(arguments.getFilePath()).lexically_normal().string()
              << "' (columns from " << arguments.getStartColumnIndex() + 1 << " to "
              << arguments.getEndColumnIndex() + 1 << ')';

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::cout << " started at" << ' ' << std::put_time(&tm, "%H:%M:%S");

    if (isParallel) {
        std::cout << " with:" << std::endl << "    ";
        if (distanceComputationThreadsCount == 0) {
            std::cout << "all the available CPU core threads";
        } else {
            std::cout << distanceComputationThreadsCount << " thread";
            if (distanceComputationThreadsCount != 1) {
                std::cout << 's';
            }
        }
        std::cout << " to compute the distance" << std::endl;
        if (version >= 5) {
            std::cout << "    ";
            if (stage4ThreadsCount == 0) {
                std::cout << "all the available CPU core threads";
            } else {
                std::cout << stage4ThreadsCount << " thread";
                if (stage4ThreadsCount != 1) {
                    std::cout << 's';
                }
            }
            std::cout << " to execute the stage 4" << std::endl;
        }
        if (version == 11) {
            std::cout << "    ";
            if (stage4ThreadsCount == 0) {
                std::cout << "all the available CPU core threads";
            } else {
                std::cout << stage4ThreadsCount << " thread";
                if (stage4ThreadsCount != 1) {
                    std::cout << 's';
                }
            }
            std::cout << " to execute the stage 4" << std::endl;
        }
    } else {
        std::cout << std::endl;
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::size_t> pi{};
    std::vector<double> lambda{};

    // Initializes pi and lambda vectors
    std::size_t dataSize = data.size();
    pi.resize(dataSize);
    lambda.resize(dataSize);

    clusteringAlgorithm(pi, lambda);

    auto end = std::chrono::high_resolution_clock::now();

    // Print the values
    // printValues(data, dimension, pi, lambda);

    // Print the elapsed time
    printElapsedTime(start, end);

    if (arguments.isTestEnabled()) {
        std::cout << std::endl;
        for (int i = 0; i < 80; i++) {
            std::cout << '-';
        }
        std::cout << std::endl << std::endl;
        if (!checkTest(arguments.getFilePath(), data, dimension, pi, lambda)) {
            std::cerr << "Test failed!" << std::endl;
            return 1;
        } else {
            std::cerr << "Test completed successfully" << std::endl;
        }
    }

    // Output the files
    std::cout << "Outputting result: ";
    if (arguments.isOutputEnabled()) {
        std::cout << "yes" << std::endl;
        std::filesystem::path outputDirectory{".."};
        outputDirectory = outputDirectory / ".." / "out";

        auto a = std::filesystem::absolute(outputDirectory);

        DataWriter::createOutputFile(outputDirectory / "out.txt", data, dimension, pi, lambda);
        DataWriter::createMathematicaOutputFile(outputDirectory / "mat.txt", pi, lambda);
    } else {
        std::cout << "no" << std::endl;
    }

    return 0;
}

std::function<void(std::vector<std::size_t> &, std::vector<double> &)> getClusteringAlgorithm(
        const bool isParallel,
        const std::size_t version,
        const std::size_t distanceComputationThreadsCount,
        const std::size_t stage4ThreadsCount,
        std::vector<double *> &data,
        double *reallocatedData,
        const std::size_t dimension) {

    std::function<void(std::vector<std::size_t> &, std::vector<double> &)> clusteringAlgorithm;

    if (isParallel) {
        switch (version) {
            case 1:
                clusteringAlgorithm =
                        [&data, dimension, distanceComputationThreadsCount, stage4ThreadsCount](
                                auto &pi, auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::CLASSICAL>(
                            data,
                            data.size(),
                            dimension,
                            pi.begin(),
                            lambda.begin(),
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 2:
                clusteringAlgorithm =
                        [&data, dimension, distanceComputationThreadsCount, stage4ThreadsCount](
                                auto &pi, auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::SSE>(
                            data,
                            data.size(),
                            dimension,
                            pi.begin(),
                            lambda.begin(),
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 3:
                clusteringAlgorithm =
                        [&data, dimension, distanceComputationThreadsCount, stage4ThreadsCount](
                                auto &pi, auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::AVX>(
                            data,
                            data.size(),
                            dimension,
                            pi.begin(),
                            lambda.begin(),
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 4:
                clusteringAlgorithm = [&data,
                                       dimension,
                                       reallocatedData,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi,
                                                           auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::AVX>(
                            reallocatedData,
                            data.size(),
                            dimension,
                            pi.begin(),
                            lambda.begin(),
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 5:
                clusteringAlgorithm =
                        [&data, dimension, distanceComputationThreadsCount, stage4ThreadsCount](
                                auto &pi, auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::SSE,
                                                std::vector<double *>,
                                                std::vector<std::size_t>::iterator,
                                                std::vector<double>::iterator,
                                                true>(data,
                                                      data.size(),
                                                      dimension,
                                                      pi.begin(),
                                                      lambda.begin(),
                                                      distanceComputationThreadsCount,
                                                      stage4ThreadsCount);
                };
                break;
            case 6:
                clusteringAlgorithm =
                        [&data, dimension, distanceComputationThreadsCount, stage4ThreadsCount](
                                auto &pi, auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::SSE_OPTIMIZED,
                                                std::vector<double *>,
                                                std::vector<std::size_t>::iterator,
                                                std::vector<double>::iterator,
                                                true>(data,
                                                      data.size(),
                                                      dimension,
                                                      pi.begin(),
                                                      lambda.begin(),
                                                      distanceComputationThreadsCount,
                                                      stage4ThreadsCount);
                };
                break;
            case 7:
                clusteringAlgorithm =
                        [&data, dimension, distanceComputationThreadsCount, stage4ThreadsCount](
                                auto &pi, auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::AVX_OPTIMIZED,
                                                std::vector<double *>,
                                                std::vector<std::size_t>::iterator,
                                                std::vector<double>::iterator,
                                                true>(data,
                                                      data.size(),
                                                      dimension,
                                                      pi.begin(),
                                                      lambda.begin(),
                                                      distanceComputationThreadsCount,
                                                      stage4ThreadsCount);
                };
                break;
            case 8:
                clusteringAlgorithm = [&data,
                                       dimension,
                                       reallocatedData,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi,
                                                           auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::SSE_OPTIMIZED,
                                                double *,
                                                std::vector<std::size_t>::iterator,
                                                std::vector<double>::iterator,
                                                true>(reallocatedData,
                                                      data.size(),
                                                      dimension,
                                                      pi.begin(),
                                                      lambda.begin(),
                                                      distanceComputationThreadsCount,
                                                      stage4ThreadsCount);
                };
                break;
            case 9:
                clusteringAlgorithm = [&data,
                                       dimension,
                                       reallocatedData,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi,
                                                           auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::AVX_OPTIMIZED,
                                                double *,
                                                std::vector<std::size_t>::iterator,
                                                std::vector<double>::iterator,
                                                true>(reallocatedData,
                                                      data.size(),
                                                      dimension,
                                                      pi.begin(),
                                                      lambda.begin(),
                                                      distanceComputationThreadsCount,
                                                      stage4ThreadsCount);
                };
                break;
            case 10:
                clusteringAlgorithm = [&data,
                                       dimension,
                                       reallocatedData,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi,
                                                           auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT,
                                                double *,
                                                std::vector<std::size_t>::iterator,
                                                std::vector<double>::iterator,
                                                true,
                                                false>(reallocatedData,
                                                       data.size(),
                                                       dimension,
                                                       pi.begin(),
                                                       lambda.begin(),
                                                       distanceComputationThreadsCount,
                                                       stage4ThreadsCount);
                };
                break;
            case 11:
                clusteringAlgorithm = [&data,
                                       dimension,
                                       reallocatedData,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi,
                                                           auto &lambda) noexcept -> void {
                    ParallelClustering::cluster<DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT,
                                                double *,
                                                std::vector<std::size_t>::iterator,
                                                std::vector<double>::iterator,
                                                true,
                                                true>(reallocatedData,
                                                      data.size(),
                                                      dimension,
                                                      pi.begin(),
                                                      lambda.begin(),
                                                      distanceComputationThreadsCount,
                                                      stage4ThreadsCount);
                };
                break;
            default:
                std::cerr << "Unknown version" << ' ' << version << std::endl;
                exit(1);
        }
    } else {
        clusteringAlgorithm = [&data, dimension](auto &pi, auto &lambda) noexcept -> void {
            SequentialClustering::cluster(data, dimension, pi, lambda);
        };
    }

    return clusteringAlgorithm;
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

bool checkTest(const std::filesystem::path &filePath,
               const std::vector<double *> &data,
               const std::size_t dimension,
               const std::vector<std::size_t> &pi,
               const std::vector<double> &lambda) {

    std::vector<std::pair<std::size_t, double>> expectedResult{};

    std::string fileName{filePath.filename().string()};

    if (fileName == "two-points.data") {
        expectedResult = {{1, 3.6055}, {1, INFINITY}};
    } else if (fileName == "samples.data") {
        expectedResult = {
                {1, 0.4472},   // A
                {4, 0.8944},   // B
                {3, 0.4},      // C
                {4, 0.7211},   // D
                {5, 1.2806},   // E
                {5, INFINITY}  // F
        };
    } else if (fileName == "same-distance.data") {
        expectedResult = {
                {3, 2.2360},   // A
                {2, 1.4142},   // B
                {3, 2.2360},   // C
                {3, INFINITY}  // D
        };
    } else if (fileName == "slide.data") {
        expectedResult = {
                {1, 0.7071},   // 1
                {5, 2.5},      // 2
                {5, 1.4142},   // 3,
                {5, 0.5},      // 4
                {5, 1},        // 5
                {5, INFINITY}  // 6
        };
    } else {
        std::vector<std::size_t> expectedPi{};
        std::vector<double> expectedLambda{};

        std::cout << "Running sequential implementation of '" << fileName
                  << "' to check the results" << std::endl;
        SequentialClustering::cluster(data, dimension, expectedPi, expectedLambda);

        for (std::size_t i = 0; i < expectedPi.size(); i++) {
            expectedResult.emplace_back(std::make_pair(expectedPi[i], expectedLambda[i]));
        }
    }

    std::vector<std::pair<std::size_t, double>> result{};
    for (std::size_t i = 0; i < pi.size(); i++) {
        result.emplace_back(std::make_pair(pi[i], lambda[i]));
    }

    return check(expectedResult, result);
}

bool check(std::vector<std::pair<std::size_t, double>> &expected,
           std::vector<std::pair<std::size_t, double>> &result) {

    std::vector<std::size_t> orderedIndexes{};
    for (std::size_t i = 0; i < expected.size(); i++) {
        orderedIndexes.push_back(i);
    }
    std::sort(orderedIndexes.begin(),
              orderedIndexes.end(),
              [&expected](const auto &a, const auto &b) -> bool {
                  return expected[a].second < expected[b].second;
              });

    std::vector<std::size_t> orderedExpectedIndexes{};
    for (std::size_t i = 0; i < result.size(); i++) {
        orderedExpectedIndexes.push_back(i);
    }
    std::sort(orderedExpectedIndexes.begin(),
              orderedExpectedIndexes.end(),
              [&result](const auto &a, const auto &b) -> bool {
                  return result[a].second < result[b].second;
              });

    auto orderedExpectedIndexesIterator = orderedExpectedIndexes.cbegin();
    for (std::size_t index : orderedIndexes) {

        std::size_t piToCheck = result[index].first;
        double lambdaToCheck = result[index].second;

        std::vector<std::size_t>::difference_type back = 0;
        bool continueSearch = true;
        while (areAlmostLess(expected[*orderedExpectedIndexesIterator].second, lambdaToCheck)) {
            ++orderedExpectedIndexesIterator;
        }

        while (areAlmostEqual(lambdaToCheck, expected[*orderedExpectedIndexesIterator].second) &&
               continueSearch) {
            if (piToCheck != expected[*orderedExpectedIndexesIterator].first) {
                ++orderedExpectedIndexesIterator;
                back++;
            } else {
                continueSearch = false;
            }
        }

        if (continueSearch) {
            std::cerr << "Expected" << ' ' << index << " being in the cluster" << ' ' << piToCheck
                      << " at distance" << ' ' << lambdaToCheck << " but it is not";
            return false;
        }
        orderedExpectedIndexesIterator -= back;
    }

    return true;
}

[[gnu::const]] bool areAlmostLess(const double first, const double second) {

    return (first - second) < -0.0001;
}

[[gnu::const]] bool areAlmostEqual(const double first, const double second) {

    if (first > std::numeric_limits<double>::max() && second > std::numeric_limits<double>::max()) {
        return true;
    }

    return fabs(first - second) < 0.0001;
}
