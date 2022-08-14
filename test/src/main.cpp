#include "ContiguousDoubleMemoryDataIterator.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"
#include "Types.h"
#include "cli/CliArgumentsParser.h"
#include "data/DataReader.h"
#include "data/DataWriter.h"
#include "ResultsChecker.h"
#include <atomic>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <xmmintrin.h>
#include <deque>

using cluster::parallel::DistanceComputers;
using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;
using cluster::test::ResultsChecker;
using cluster::test::cli::CliArguments;
using cluster::test::cli::CliArgumentsParser;
using cluster::test::data::DataReader;
using cluster::test::data::DataWriter;

template <typename C, typename D>
void printElapsedTime(std::chrono::time_point<C, D> start, std::chrono::time_point<C, D> end);

void printValues(const std::vector<double *> &data,
                 std::size_t dimension,
                 const std::vector<std::size_t> &pi,
                 const std::vector<double> &lambda);

std::function<void(std::vector<std::size_t> &, std::vector<double> &)> getClusteringAlgorithm(
        const bool isParallel,
        const std::size_t version,
        const std::size_t distanceComputationThreadsCount,
        const std::size_t stage4ThreadsCount,
        const std::size_t stage5ThreadsCount,
        std::vector<double *> &data,
        std::size_t dataElementsCount,
        double *mmAlignedData,
        double *uniqueArrayData,
        const std::size_t dimension);

bool checkTest(const std::filesystem::path &filePath,
               const std::vector<double *> &data,
               std::size_t dimension,
               const std::vector<std::size_t> &resultPi,
               const std::vector<double> &resultLambda,
               bool usePreviousResults,
               const std::filesystem::path &previousResultsPath);

bool areAlmostLess(const double first, const double second);

bool areAlmostEqual(const double first, const double second);

bool check(std::vector<std::pair<std::size_t, double>> &expected,
           std::vector<std::pair<std::size_t, double>> &result);

template <typename T>
void addAll(std::vector<T> &vector, std::initializer_list<T> values);

CliArguments parseArguments(int argc, char *argv[]) {

    try {
        CliArgumentsParser parser{argc, argv};
        CliArguments arguments = parser.parseArguments();

        return arguments;
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        exit(1);
    }
}

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
int main(int argc, char *argv[]) {

    // static_assert(std::random_access_iterator<ContiguousDoubleMemoryIterator>);
    CliArguments arguments = parseArguments(argc, argv);

    // Read the data
    std::vector<double> data{};
    std::size_t dimension = DataReader::readAndParseData(arguments.getInputFilePath(),
                                                         data,
                                                         arguments.getFirstLineNumber(),
                                                         arguments.getLastLineNumber(),
                                                         arguments.getFirstColumnNumber(),
                                                         arguments.getLastColumnNumber());

    double *mmAlignedData = nullptr;
    double *uniqueVectorData = nullptr;

    bool isParallel = arguments.isParallel();
    std::size_t version = arguments.getAlgorithmVersion();
    std::size_t distanceComputationThreadsCount = arguments.getDistanceComputationThreadsCount();
    std::size_t stage4ThreadsCount = arguments.getStructuralFixThreadsCount();
    std::size_t stage5ThreadsCount = arguments.getSqrtComputationThreadsCount();
    auto *dataIterator = data.data();
    std::vector<double *> twoLevels{};
    std::size_t dataElementsCount = data.size() / dimension;

    // Fill two levels
    for (std::size_t i = 0; i < data.size(); i += dimension) {
        auto *point = new double[dimension];
        // Copy the doubles
        memcpy(point, &(dataIterator[i]), dimension * sizeof(double));
        twoLevels.push_back(point);
    }

    if (isParallel) {
        std::size_t alignment = (version == 2) ? 2 : 4;
        std::size_t quotient = 1 + ((dimension - 1) / alignment);
        std::size_t newDimension = alignment * quotient;

        if (version != 1) {
            if (version == 2 || version == 3 || version == 5 || version == 6 || version == 7) {
                std::size_t elementIndex = 0;
                for (std::size_t i = 0; i < data.size(); i += dimension) {
                    auto *reallocatedPoint = static_cast<double *>(
                            _mm_malloc(newDimension * sizeof(double), alignment * sizeof(double)));
                    // Copy the doubles
                    memcpy(reallocatedPoint, &(dataIterator[i]), dimension * sizeof(double));
                    // Fill the remaining coordinates with 0
                    memset(&(reallocatedPoint[dimension]),
                           0,
                           (newDimension - dimension) * sizeof(double));

                    // Add the point
                    delete[] twoLevels[elementIndex];
                    twoLevels[elementIndex] = reallocatedPoint;
                    elementIndex++;
                }
            } else if (version == 4 || version == 8 || version == 9 || version == 10 ||
                       version == 11) {
                std::size_t size = sizeof(double) * (dataElementsCount * newDimension);
                mmAlignedData = static_cast<double *>(_mm_malloc(size, alignment * sizeof(double)));
                memset(mmAlignedData, 0, size);

                std::size_t start = 0;
                for (std::size_t i = 0; i < data.size(); i += dimension) {
                    memcpy(&(mmAlignedData[start]), &(dataIterator[i]), dimension * sizeof(double));
                    start += newDimension;
                }
            } else {
                std::cerr << "Warning: Possibly unaligned data" << std::endl;
                sleep(20);
            }
        }
    } else {
        if (version == 2) {
            uniqueVectorData = new double[data.size()];
            for (std::size_t i = 0; i < data.size(); i += dimension) {
                memcpy(&(uniqueVectorData[i]), &(dataIterator[i]), dimension * sizeof(double));
            }
        }
    }

    // Decide the clustering algorithm
    std::function<void(std::vector<std::size_t> &, std::vector<double> &)> clusteringAlgorithm =
            getClusteringAlgorithm(isParallel,
                                   version,
                                   distanceComputationThreadsCount,
                                   stage4ThreadsCount,
                                   stage5ThreadsCount,
                                   twoLevels,
                                   dataElementsCount,
                                   mmAlignedData,
                                   uniqueVectorData,
                                   dimension);

    if (isParallel) {
        std::cout << "Parallel clustering version" << ' ' << version;
    } else {
        std::cout << "Sequential clustering";
    }
    std::cout << " of '" << arguments.getInputFilePath().string() << "' (columns from "
              << arguments.getFirstColumnNumber() + 1 << " to "
              << arguments.getLastColumnNumber() + 1 << ')';

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
            if (stage5ThreadsCount == 0) {
                std::cout << "all the available CPU core threads";
            } else {
                std::cout << stage5ThreadsCount << " thread";
                if (stage5ThreadsCount != 1) {
                    std::cout << 's';
                }
            }
            std::cout << " to execute the stage 5" << std::endl;
        }
    } else {
        std::cout << std::endl;
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::size_t> pi{};
    std::vector<double> lambda{};

    // Initializes pi and lambda vectors
    pi.resize(dataElementsCount);
    lambda.resize(dataElementsCount);

    clusteringAlgorithm(pi, lambda);

    auto end = std::chrono::high_resolution_clock::now();

    // Print the values
    // printValues(data, dimension, pi, lambda);

    // Print the elapsed time
    printElapsedTime(start, end);

    if (arguments.isTestModeEnabled()) {
        std::cout << std::endl;
        for (int i = 0; i < 80; i++) {
            std::cout << '-';
        }
        std::cout << std::endl << std::endl;
        if (!checkTest(arguments.getInputFilePath(),
                       twoLevels,
                       dimension,
                       pi,
                       lambda,
                       arguments.isPreviousTestResultsToBeUsed(),
                       arguments.getTestResultsFilePath())) {
            std::cerr << std::endl << "Test failed!" << std::endl << std::endl;
            return 1;
        }
        std::cout << std::endl << "Test completed successfully" << std::endl << std::endl;
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

    delete[] uniqueVectorData;
    _mm_free(mmAlignedData);

    return 0;
}

std::function<void(std::vector<std::size_t> &, std::vector<double> &)> getClusteringAlgorithm(
        const bool isParallel,
        const std::size_t version,
        const std::size_t distanceComputationThreadsCount,
        const std::size_t stage4ThreadsCount,
        const std::size_t stage5ThreadsCount,
        std::vector<double *> &data,
        std::size_t dataElementsCount,
        double *mmAlignedData,
        double *uniqueArrayData,
        const std::size_t dimension) {

    std::function<void(std::vector<std::size_t> &, std::vector<double> &)> clusteringAlgorithm;

    auto sseMmAlignedDataIterator = mmAlignedData;
    auto avxMmAlignedDataIterator = mmAlignedData;

    // static_assert(cluster::utils::IndirectContiguousAccessIterator<double *, double>, "No");
    static_assert(cluster::utils::ContiguousIterator<double *, double>, "No");

    if (isParallel) {
        switch (version) {
            case 1:
                clusteringAlgorithm = [&data,
                                       dataElementsCount,
                                       dimension,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi,
                                                           auto &lambda) noexcept -> void {
                    ParallelClustering<>::cluster<DistanceComputers::CLASSICAL>(
                            data,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 2:
                clusteringAlgorithm = [&data,
                                       dataElementsCount,
                                       dimension,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi, auto &lambda) -> void {
                    ParallelClustering<>::cluster<DistanceComputers::SSE>(
                            data,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 3:
                clusteringAlgorithm = [&data,
                                       dataElementsCount,
                                       dimension,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi, auto &lambda) -> void {
                    ParallelClustering<>::cluster<DistanceComputers::AVX>(
                            data,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 4:
                clusteringAlgorithm = [dataElementsCount,
                                       dimension,
                                       avxMmAlignedDataIterator,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi, auto &lambda) -> void {
                    ParallelClustering<>::cluster<DistanceComputers::AVX>(
                            avxMmAlignedDataIterator,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };

                break;
            case 5:
                clusteringAlgorithm = [&data,
                                       dataElementsCount,
                                       dimension,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi, auto &lambda) -> void {
                    ParallelClustering<true, true>::cluster<DistanceComputers::SSE>(
                            data,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 6:
                clusteringAlgorithm = [&data,
                                       dataElementsCount,
                                       dimension,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi, auto &lambda) -> void {
                    ParallelClustering<true, true>::cluster<DistanceComputers::SSE_OPTIMIZED>(
                            data,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 7:
                clusteringAlgorithm = [&data,
                                       dataElementsCount,
                                       dimension,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi, auto &lambda) -> void {
                    ParallelClustering<true, true>::cluster<DistanceComputers::AVX_OPTIMIZED>(
                            data,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 8:
                clusteringAlgorithm = [dataElementsCount,
                                       dimension,
                                       sseMmAlignedDataIterator,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi, auto &lambda) -> void {
                    ParallelClustering<true, true>::cluster<DistanceComputers::SSE_OPTIMIZED>(
                            sseMmAlignedDataIterator,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 9:
                clusteringAlgorithm = [dataElementsCount,
                                       dimension,
                                       avxMmAlignedDataIterator,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi, auto &lambda) -> void {
                    ParallelClustering<true, true>::cluster<DistanceComputers::AVX_OPTIMIZED>(
                            avxMmAlignedDataIterator,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 10:
                clusteringAlgorithm = [dataElementsCount,
                                       dimension,
                                       avxMmAlignedDataIterator,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount](auto &pi, auto &lambda) -> void {
                    ParallelClustering<true, true>::cluster<
                            DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
                            avxMmAlignedDataIterator,
                            dataElementsCount,
                            dimension,
                            pi,
                            lambda,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount);
                };
                break;
            case 11:
                clusteringAlgorithm = [dataElementsCount,
                                       dimension,
                                       avxMmAlignedDataIterator,
                                       distanceComputationThreadsCount,
                                       stage4ThreadsCount,
                                       stage5ThreadsCount](auto &pi, auto &lambda) -> void {
                    auto piIterator = pi.begin();
                    auto lambdaIterator = lambda.begin();

                    ParallelClustering<true, true, true>::cluster<
                            DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
                            avxMmAlignedDataIterator,
                            dataElementsCount,
                            dimension,
                            piIterator,
                            lambdaIterator,
                            distanceComputationThreadsCount,
                            stage4ThreadsCount,
                            stage5ThreadsCount);
                };
                break;
            default:
                std::cerr << "Unknown version" << ' ' << version << std::endl;
                exit(1);
        }
    } else {
        switch (version) {
            case 1:
                clusteringAlgorithm = [&data, dataElementsCount, dimension](
                                              auto &pi, auto &lambda) noexcept -> void {
                    SequentialClustering::cluster(
                            data.begin(), dataElementsCount, dimension, pi.begin(), lambda.begin());
                };
                break;
            case 2:
                clusteringAlgorithm = [&data, dataElementsCount, uniqueArrayData, dimension](
                                              auto &pi, auto &lambda) noexcept -> void {
                    SequentialClustering::cluster(
                            ContiguousDoubleMemoryDataIterator(&(uniqueArrayData[0]), dimension),
                            dataElementsCount,
                            dimension,
                            pi.begin(),
                            lambda.begin());
                };
                break;
            default:
                std::cerr << "Unknown version" << ' ' << version << std::endl;
                exit(1);
        }
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
               const std::vector<std::size_t> &resultPi,
               const std::vector<double> &resultLambda,
               const bool usePreviousResults,
               const std::filesystem::path &previousResultsPath) {

    std::vector<std::size_t> expectedPi{};
    std::vector<double> expectedLambda{};

    std::string fileName{filePath.filename().string()};

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    if (fileName == "two-points.data") {
        addAll<std::size_t>(expectedPi, {1, 1});
        addAll(expectedLambda, {3.6055, std::numeric_limits<double>::infinity()});
    } else if (fileName == "samples.data") {
        addAll<std::size_t>(expectedPi, {1, 4, 3, 4, 5, 5});
        addAll(expectedLambda,
               {0.4472,                                   // A
                0.8944,                                   // B
                0.4,                                      // C
                0.7211,                                   // D
                1.2806,                                   // E
                std::numeric_limits<double>::infinity()}  // F
        );
    } else if (fileName == "same-distance.data") {
        addAll<std::size_t>(expectedPi, {3, 2, 3, 3});
        addAll(expectedLambda,
               {
                       2.2360,                                  // A
                       1.4142,                                  // B
                       2.2360,                                  // C
                       std::numeric_limits<double>::infinity()  // D
               });
    } else if (fileName == "slide.data") {
        addAll<std::size_t>(expectedPi, {1, 5, 5, 5, 5, 5});
        addAll(expectedLambda,
               {0.7071,                                   // 1
                2.5,                                      // 2
                1.4142,                                   // 3,
                0.5,                                      // 4
                1.0,                                      // 5
                std::numeric_limits<double>::infinity()}  // 6
        );
    } else {

        expectedPi.resize(data.size());
        expectedLambda.resize(data.size());

        if (usePreviousResults && std::filesystem::is_regular_file(previousResultsPath)) {
            std::cout << "Reading file '" << previousResultsPath.string()
                      << "' to retrieve the results of the sequential implementation of '"
                      << fileName << "' so to check the results" << std::endl;
            DataReader::readPiLambda(previousResultsPath, expectedPi, expectedLambda);
        } else {
            cluster::utils::Timer::initTimers();
            auto piIterator = expectedPi.begin();
            auto lambdaIterator = expectedLambda.begin();
            std::cout << "Running sequential implementation of '" << fileName
                      << "' to check the results" << std::endl;
            SequentialClustering::cluster(data.begin(),
                                          data.size(),
                                          dimension,
                                          piIterator,
                                          lambdaIterator);
            if (usePreviousResults) {
                DataWriter::writePiLambda(previousResultsPath, expectedPi, expectedLambda);
            }
        }
        /*for (std::size_t i = 0; i < expectedPi.size(); i++) {
            expectedResult.emplace_back(std::make_pair(expectedPi[i], expectedLambda[i]));
        }*/
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    /*std::vector<std::pair<std::size_t, double>> result{};
    for (std::size_t i = 0; i < pi.size(); i++) {
        result.emplace_back(std::make_pair(pi[i], lambda[i]));
    }*/

    return ResultsChecker::checkResults(resultPi.cbegin(),
                                        resultPi.cend(),
                                        resultLambda.cbegin(),
                                        resultLambda.cend(),
                                        expectedPi.cbegin(),
                                        expectedPi.cend(),
                                        expectedLambda.cbegin(),
                                        expectedLambda.cend());  // check(expectedResult, result);
}

template <typename T>
void addAll(std::vector<T> &vector, const std::initializer_list<T> values) {

    for (const T &value : values) {
        vector.template emplace_back(value);
    }
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
                      << " at distance" << ' ' << lambdaToCheck << " but it is not" << std::endl;
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
