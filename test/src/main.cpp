#include "ClusteringAlgorithmExecutor.h"
#include "ContiguousDoubleMemoryDataIterator.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"
#include "Types.h"
#include "cli/CliArgumentsParser.h"
#include "data/DataReader.h"
#include "data/DataWriter.h"
#include "data/ResultsChecker.h"
#include <atomic>
#include <cmath>
#include <cstring>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <xmmintrin.h>

using cluster::parallel::DistanceComputers;
using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;
using cluster::test::ResultsChecker;
using cluster::test::cli::CliArguments;
using cluster::test::cli::CliArgumentsParser;
using cluster::test::data::DataReader;
using cluster::test::data::DataWriter;
using cluster::test::main::ClusteringAlgorithmExecutor;

void initializeDataStructure(bool isParallel,
                             std::size_t version,
                             std::size_t dimension,
                             const double *dataBegin,
                             std::size_t dataElementsCount,
                             double *&uniqueVectorData,
                             std::vector<double *> &indirectData,
                             std::vector<double *> &alignedIndirectData,
                             double *&alignedData,
                             std::function<void()> &freeFunction);

void printValues(const std::vector<double *> &data,
                 std::size_t dimension,
                 const std::vector<std::size_t> &pi,
                 const std::vector<double> &lambda);

/*std::function<void(std::vector<std::size_t> &, std::vector<double> &)> getClusteringAlgorithm(
        const bool isParallel,
        const std::size_t version,
        const std::size_t distanceComputationThreadsCount,
        const std::size_t stage4ThreadsCount,
        const std::size_t stage5ThreadsCount,
        std::vector<double *> &data,
        std::size_t dataElementsCount,
        double *mmAlignedData,
        double *uniqueArrayData,
        const std::size_t dimension);*/

bool checkTest(const std::filesystem::path &filePath,
               const std::vector<double *> &data,
               std::size_t dimension,
               const std::vector<std::size_t> &resultPi,
               const std::vector<double> &resultLambda,
               bool usePreviousResults,
               const std::filesystem::path &previousResultsPath);

template <typename T>
void addAll(std::vector<T> &vector, std::initializer_list<T> values);

CliArguments parseArguments(int argc, char *argv[]);

void printThreadsCount(std::size_t threadsCount);

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
    std::size_t dataElementsCount = data.size() / dimension;

    bool isParallel = arguments.isParallel();
    std::size_t version = arguments.getAlgorithmVersion();
    std::size_t distanceComputationThreadsCount = arguments.getDistanceComputationThreadsCount();
    std::size_t stage4ThreadsCount = arguments.getStructuralFixThreadsCount();
    std::size_t stage5ThreadsCount = arguments.getSqrtComputationThreadsCount();

    double *uniqueVectorData = nullptr;
    std::vector<double *> indirectData{};
    std::vector<double *> alignedIndirectData{};
    double *alignedData = nullptr;
    std::function<void()> freeFunction{};

    initializeDataStructure(isParallel,
                            version,
                            dimension,
                            &(data.begin()[0]),
                            dataElementsCount,
                            uniqueVectorData,
                            indirectData,
                            alignedIndirectData,
                            alignedData,
                            freeFunction);

    // Decide the clustering algorithm
    /*std::function<void(std::vector<std::size_t> &, std::vector<double> &)> clusteringAlgorithm =
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
    */
    if (isParallel) {
        std::cout << "Parallel clustering version" << ' ' << version;
    } else {
        std::cout << "Sequential clustering";
    }
    std::cout << " of '" << arguments.getInputFilePath().string() << "' (columns from "
              << arguments.getFirstColumnNumber() << " to " << arguments.getLastColumnNumber()
              << ')';

    if (isParallel) {
        std::cout << " with:" << std::endl << "    ";
        printThreadsCount(distanceComputationThreadsCount);
        std::cout << " to compute the distance" << std::endl;
        if (version >= 5) {
            printThreadsCount(stage4ThreadsCount);
            std::cout << " to execute the stage 4" << std::endl;
        }
        if (version == 11) {
            printThreadsCount(stage5ThreadsCount);
            std::cout << " to execute the stage 5" << std::endl;
        }
    } else {
        std::cout << std::endl;
    }

    ClusteringAlgorithmExecutor executor{dataElementsCount,
                                         dimension,
                                         distanceComputationThreadsCount,
                                         stage4ThreadsCount,
                                         stage5ThreadsCount};

    std::vector<std::size_t> pi{};
    std::vector<double> lambda{};

    // Initializes pi and lambda vectors
    pi.resize(dataElementsCount);
    lambda.resize(dataElementsCount);

    executor.executeClusteringAlgorithm(isParallel,
                                        version,
                                        indirectData,
                                        indirectData,
                                        indirectData,
                                        alignedData,
                                        alignedData,
                                        uniqueVectorData,
                                        pi.begin(),
                                        lambda.begin());

    // Print the values
    // printValues(data, dimension, pi, lambda);

    if (arguments.isTestModeEnabled()) {
        std::cout << std::endl;
        for (int i = 0; i < 80; i++) {
            std::cout << '-';
        }
        std::cout << std::endl << std::endl;
        if (!checkTest(arguments.getInputFilePath(),
                       indirectData,
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

    freeFunction();

    return 0;
}

CliArguments parseArguments(int argc, char **argv) {

    try {
        CliArgumentsParser parser{argc, argv};
        CliArguments arguments = parser.parseArguments();

        return arguments;
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        exit(1);
    }
}

void printThreadsCount(std::size_t threadsCount) {

    if (threadsCount == 0) {
        std::cout << "all the available CPU core threads";
    } else {
        std::cout << threadsCount << " thread";
        if (threadsCount != 1) {
            std::cout << 's';
        }
    }
}

void initializeDataStructure(bool isParallel,
                             std::size_t version,
                             std::size_t dimension,
                             const double *dataBegin,
                             std::size_t dataElementsCount,
                             double *&uniqueVectorData,
                             std::vector<double *> &indirectData,
                             std::vector<double *> &alignedIndirectData,
                             double *&alignedData,
                             std::function<void()> &freeFunction) {
    if (version == 1) {
        // Fill two levels
        for (std::size_t i = 0; i < dataElementsCount; i += dimension) {
            auto *point = new double[dimension];
            // Copy the doubles
            memcpy(point, &(dataBegin[i]), dimension * sizeof(double));
            indirectData.push_back(point);
        }
        freeFunction = [&indirectData]() -> void {
            for (double *point : indirectData) {
                delete[] point;
            }
        };
    } else if (isParallel) {
        switch (version) {
            case 2:
            case 3:
            case 5:
            case 6:
            case 7: {
                const std::size_t pointDimension =
                        (version == 3 || version == 7)
                                ? ParallelClustering<>::computeAvxDimension(dimension)
                                : ParallelClustering<>::computeSseDimension(dimension);
                const std::size_t alignment = (version == 3 || version == 7)
                                                      ? ParallelClustering<>::AVX_PACK_SIZE
                                                      : ParallelClustering<>::SSE_PACK_SIZE;

                std::size_t elementIndex = 0;
                for (std::size_t i = 0; i < dataElementsCount; i += dimension) {
                    auto *reallocatedPoint = static_cast<double *>(_mm_malloc(
                            pointDimension * sizeof(double), alignment * sizeof(double)));
                    // Copy the doubles
                    memcpy(reallocatedPoint, &(dataBegin[i]), dimension * sizeof(double));
                    // Fill the remaining coordinates with 0
                    memset(&(reallocatedPoint[dimension]),
                           0,
                           (pointDimension - dimension) * sizeof(double));

                    // Add the point
                    alignedIndirectData[elementIndex] = reallocatedPoint;
                    elementIndex++;
                }
                freeFunction = [&alignedIndirectData]() -> void {
                    for (double *point : alignedIndirectData) {
                        _mm_free(point);
                    }
                };
            } break;
            case 4:
            case 8:
            case 9:
            case 10:
            case 11: {
                const std::size_t pointDimension =
                        (version == 8) ? ParallelClustering<>::computeSseDimension(dimension)
                                       : ParallelClustering<>::computeAvxDimension(dimension);
                const std::size_t alignment = (version == 8) ? ParallelClustering<>::SSE_PACK_SIZE
                                                             : ParallelClustering<>::AVX_PACK_SIZE;

                std::size_t size = sizeof(double) * (dataElementsCount * pointDimension);
                alignedData = static_cast<double *>(_mm_malloc(size, alignment * sizeof(double)));
                memset(alignedData, 0, size);

                std::size_t start = 0;
                for (std::size_t i = 0; i < dataElementsCount; i += dimension) {
                    memcpy(&(alignedData[start]), &(dataBegin[i]), dimension * sizeof(double));
                    start += pointDimension;
                }
                freeFunction = [&alignedData]() -> void { _mm_free(alignedData); };
            } break;
            default:
                std::cerr << "The version" << ' ' << version << " is not implemented." << std::endl;
                exit(1);
        }
    } else if (version == 2) {
        uniqueVectorData = new double[dataElementsCount];
        for (std::size_t i = 0; i < dataElementsCount; i += dimension) {
            memcpy(&(uniqueVectorData[i]), &(dataBegin[i]), dimension * sizeof(double));
        }
        freeFunction = [&uniqueVectorData]() -> void { delete[] uniqueVectorData; };
    } else {
        std::cerr << "The version" << ' ' << version << " is not implemented." << std::endl;
        exit(1);
    }
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
            SequentialClustering::cluster(
                    data.begin(), data.size(), dimension, piIterator, lambdaIterator);
            if (usePreviousResults) {
                DataWriter::writePiLambda(previousResultsPath, expectedPi, expectedLambda);
            }
        }
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    return ResultsChecker::checkResults(resultPi.cbegin(),
                                        resultPi.cend(),
                                        resultLambda.cbegin(),
                                        resultLambda.cend(),
                                        expectedPi.cbegin(),
                                        expectedPi.cend(),
                                        expectedLambda.cbegin(),
                                        expectedLambda.cend());
}

template <typename T>
void addAll(std::vector<T> &vector, const std::initializer_list<T> values) {

    for (const T &value : values) {
        vector.template emplace_back(value);
    }
}
