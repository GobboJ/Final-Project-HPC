/**
 * Main test file.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.8 2022-06-16
 * @since 1.0
 */
#include "ClusteringAlgorithmExecutor.h"
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

using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;
using cluster::test::ResultsChecker;
using cluster::test::cli::CliArguments;
using cluster::test::cli::CliArgumentsParser;
using cluster::test::data::DataReader;
using cluster::test::data::DataWriter;
using cluster::test::main::ClusteringAlgorithmExecutor;

/**
 * Parses the arguments specified in the command line.
 *
 * @param argc Number of arguments specified in the command line.
 * @param argv Array containing the arguments specified in the command line.
 * @return The parsed arguments.
 */
CliArguments parseArguments(int argc, const char *const *argv);

/**
 * Utility function that prints the specified number of threads.
 *
 * @param threadsCount Number of threads to print.
 */
void printThreadsCount(std::size_t threadsCount);

/**
 * Initializes the data structures
 *
 * @param isParallel <code>true</code> if the parallel implementation of the clustering
 * algorithm should be executed, <code>false</code> otherwise.
 * @param version Version of the algorithm to execute.
 * @param dimension Dimension of the data samples to cluster.
 * @param dataBegin Pointer to the first data sample to cluster.
 * @param dataElementsCount Number of data samples to cluster.
 * @param uniqueVectorData Data samples to cluster, stored as a unique array.
 * @param indirectData Data samples to cluster, stored as indirect pointers.
 * @param alignedIndirectData Data samples to cluster, stored as indirect pointers and
 * aligned for SSE or AVX.
 * @param alignedData Data samples to cluster, aligned for SSE or AVX.
 * @param freeFunction Function that should be called to deallocate the memory this function has
 * allocated.
 */
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

/**
 * Print the values of pi and lambda, as well as the distance matrix.
 *
 * @param data Data structure holding the samples to cluster.
 * @param dimension Dimension of the samples to cluster.
 * @param pi Data structure holding the values of <code>pi</code>.
 * @param lambda  Data structure holding the values of <code>lambda</code>.
 */
void printValues(const std::vector<double *> &data,
                 std::size_t dimension,
                 const std::vector<std::size_t> &pi,
                 const std::vector<double> &lambda);

/**
 * Utility function that checks the results of the clustering algorithms.
 *
 * @param filePath Path of the file containing the data samples to cluster.
 * @param data Data structure holding the samples to cluster.
 * @param dimension Dimension of the samples to cluster.
 * @param resultPi Data structure holding the values of <code>pi</code> computed by the clustering
 * algorithm.
 * @param resultLambda Data structure holding the values of <code>lambda</code> computed by the
 * clustering algorithm.
 * @param usePreviousResults <code>true</code> if the function should use the results already
 * computed in a previous execution and stored in a file, <code>false</code> if the function should
 * execute the sequential implementation of the clustering algorithm to compute the correct results.
 * @param previousResultsPath Path of the file that contains the results already computed in a
 * previous execution. If the file does not exist, this function executed the sequential
 * implementation of the clustering algorithm and then stores the results in the file with the
 * specified path.
 * @return <code>true</code> if the results of the clustering algorithm are correct,
 * <code>false</code> otherwise.
 */
bool checkTest(const std::filesystem::path &filePath,
               const std::vector<double *> &data,
               std::size_t dimension,
               const std::vector<std::size_t> &resultPi,
               const std::vector<double> &resultLambda,
               bool usePreviousResults,
               const std::filesystem::path &previousResultsPath);

/**
 * Utility function that appends to the specified vector the specified elements.
 *
 * @tparam T Type of the elements to append.
 * @param vector Vector where the elements will be appended.
 * @param elements Elements to append.
 */
template <typename T>
void addAll(std::vector<T> &vector, std::initializer_list<T> elements);

/**
 * Main entrypoint of the application.
 *
 * @param argc Number of arguments specified in the command line.
 * @param argv Array containing the arguments specified in the command line.
 * @return The exit code.
 */
int main(int argc, char *argv[]) {

    // Parse the arguments
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

    // Extract the information about the clustering algorithm to execute
    bool isParallel = arguments.isParallel();
    std::size_t version = arguments.getAlgorithmVersion();
    std::size_t distanceComputationThreadsCount = arguments.getDistanceComputationThreadsCount();
    std::size_t structuralFixThreadsCount = arguments.getStructuralFixThreadsCount();
    std::size_t sqrtComputationThreadsCount = arguments.getSqrtComputationThreadsCount();

    // Initialize the data structures holding the data samples to cluster
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

    // Print the informational summary
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
        std::cout << " to compute the distance" << std::endl << "    ";
        if (version >= 5) {
            printThreadsCount(structuralFixThreadsCount);
            std::cout << " to execute the structural fix" << std::endl << "    ";
        }
        if (version == 11) {
            printThreadsCount(sqrtComputationThreadsCount);
            std::cout << " to execute the square roots computation" << std::endl << "    ";
        }
    } else {
        std::cout << std::endl;
    }

    // Initializes pi and lambda vectors
    std::vector<std::size_t> pi{};
    pi.resize(dataElementsCount);
    std::vector<double> lambda{};
    lambda.resize(dataElementsCount);

    // Execute the requested clustering algorithm
    ClusteringAlgorithmExecutor executor{dataElementsCount,
                                         dimension,
                                         distanceComputationThreadsCount,
                                         structuralFixThreadsCount,
                                         sqrtComputationThreadsCount};
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

    // Check the computed pi and lambda, if requested
    if (arguments.isTestModeEnabled()) {
        // Print a separator
        std::cout << std::endl;
        for (int i = 0; i < 80; i++) {
            std::cout << '-';
        }
        std::cout << std::endl << std::endl;
        // Check the results
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

    // Output the files, if requested
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

    // Deallocate the memory
    freeFunction();

    return 0;
}

/**
 * Parses the arguments specified in the command line.
 *
 * @param argc Number of arguments specified in the command line.
 * @param argv Array containing the arguments specified in the command line.
 * @return The parsed arguments.
 */
CliArguments parseArguments(const int argc, const char *const *const argv) {

    try {
        // Parse the arguments
        CliArgumentsParser parser{argc, argv};
        CliArguments arguments = parser.parseArguments();

        return arguments;
    } catch (std::exception &exception) {
        // Inform the user about the wrong arguments
        std::cerr << exception.what() << std::endl;
        exit(1);
    }
}

/**
 * Utility function that prints the specified number of threads.
 *
 * @param threadsCount Number of threads to print.
 */
void printThreadsCount(const std::size_t threadsCount) {

    if (threadsCount == 0) {
        std::cout << "all the available CPU core threads";
    } else {
        std::cout << threadsCount << " thread";
        if (threadsCount != 1) {
            std::cout << 's';
        }
    }
}

/**
 * Initializes the data structures
 *
 * @param isParallel <code>true</code> if the parallel implementation of the clustering
 * algorithm should be executed, <code>false</code> otherwise.
 * @param version Version of the algorithm to execute.
 * @param dimension Dimension of the data samples to cluster.
 * @param dataBegin Pointer to the first data sample to cluster.
 * @param dataElementsCount Number of data samples to cluster.
 * @param uniqueVectorData Data samples to cluster, stored as a unique array.
 * @param indirectData Data samples to cluster, stored as indirect pointers.
 * @param alignedIndirectData Data samples to cluster, stored as indirect pointers and
 * aligned for SSE or AVX.
 * @param alignedData Data samples to cluster, aligned for SSE or AVX.
 * @param freeFunction Function that should be called to deallocate the memory this function has
 * allocated.
 */
void initializeDataStructure(const bool isParallel,
                             const std::size_t version,
                             const std::size_t dimension,
                             const double *dataBegin,
                             std::size_t dataElementsCount,
                             double *&uniqueVectorData,
                             std::vector<double *> &indirectData,
                             std::vector<double *> &alignedIndirectData,
                             double *&alignedData,
                             std::function<void()> &freeFunction) {

    // Fill two levels
    for (std::size_t i = 0; i < dataElementsCount; i++) {
        auto *point = new double[dimension];
        // Copy the doubles
        memcpy(point, &(dataBegin[i * dimension]), dimension * sizeof(double));
        indirectData.push_back(point);
    }
    // Set the de-allocation function
    std::function<void()> indirectDataFreeFunction = [&indirectData]() noexcept -> void {
        for (double *point : indirectData) {
            delete[] point;
        }
    };
    freeFunction = indirectDataFreeFunction;

    if (version != 1) {
        if (isParallel) {
            switch (version) {
                case 2:
                case 3:
                case 5:
                case 6:
                case 7: {
                    // Compute the alignment and dimension
                    const std::size_t pointDimension =
                            (version == 3 || version == 7)
                                    ? ParallelClustering<>::computeAvxDimension(dimension)
                                    : ParallelClustering<>::computeSseDimension(dimension);
                    const std::size_t alignment = (version == 3 || version == 7)
                                                          ? ParallelClustering<>::AVX_PACK_SIZE
                                                          : ParallelClustering<>::SSE_PACK_SIZE;
                    // Fill the indirect aligned data structure
                    for (std::size_t i = 0; i < dataElementsCount; i++) {
                        // Create the point
                        auto *point = static_cast<double *>(_mm_malloc(
                                pointDimension * sizeof(double), alignment * sizeof(double)));
                        // Copy the doubles
                        memcpy(point, &(dataBegin[i * dimension]), dimension * sizeof(double));
                        // Fill the remaining coordinates with 0
                        memset(&(point[dimension]),
                               0,
                               (pointDimension - dimension) * sizeof(double));

                        // Add the point
                        alignedIndirectData[i] = point;
                    }
                    // Set the de-allocation function
                    freeFunction = [&alignedIndirectData,
                                    indirectDataFreeFunction]() noexcept -> void {
                        indirectDataFreeFunction();
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
                    // Compute the alignment and dimension
                    const std::size_t pointDimension =
                            (version == 8) ? ParallelClustering<>::computeSseDimension(dimension)
                                           : ParallelClustering<>::computeAvxDimension(dimension);
                    const std::size_t alignment = (version == 8)
                                                          ? ParallelClustering<>::SSE_PACK_SIZE
                                                          : ParallelClustering<>::AVX_PACK_SIZE;
                    // Create and clear the data structure
                    std::size_t size = sizeof(double) * (dataElementsCount * pointDimension);
                    alignedData =
                            static_cast<double *>(_mm_malloc(size, alignment * sizeof(double)));
                    memset(alignedData, 0, size);

                    // Fill the indirect aligned data structure
                    for (std::size_t i = 0; i < dataElementsCount; i++) {
                        memcpy(&(alignedData[i * pointDimension]),
                               &(dataBegin[i * dimension]),
                               dimension * sizeof(double));
                    }
                    // Set the de-allocation function
                    freeFunction = [&alignedData, indirectDataFreeFunction]() noexcept -> void {
                        indirectDataFreeFunction();
                        _mm_free(alignedData);
                    };
                } break;
                default:
                    std::cerr << "The parallel version" << ' ' << version << " is not implemented."
                              << std::endl;
                    exit(1);
            }
        } else if (version == 2) {
            // Create the data structure
            uniqueVectorData = new double[dataElementsCount];
            // Fill data structure
            for (std::size_t i = 0; i < dataElementsCount; i++) {
                memcpy(&(uniqueVectorData[i]),
                       &(dataBegin[i * dimension]),
                       dimension * sizeof(double));
            }
            // Set the de-allocation function
            freeFunction = [&uniqueVectorData, indirectDataFreeFunction]() noexcept -> void {
                indirectDataFreeFunction();
                delete[] uniqueVectorData;
            };
        } else {
            std::cerr << "The version" << ' ' << version << " is not implemented." << std::endl;
            exit(1);
        }
    }
}

/**
 * Print the values of pi and lambda, as well as the distance matrix.
 *
 * @param data Data structure holding the samples to cluster.
 * @param dimension Dimension of the samples to cluster.
 * @param pi Data structure holding the values of <code>pi</code>.
 * @param lambda  Data structure holding the values of <code>lambda</code>.
 */
void printValues(const std::vector<double *> &data,
                 const std::size_t dimension,
                 const std::vector<std::size_t> &pi,
                 const std::vector<double> &lambda) {

    // Print the values of pi
    for (std::size_t i = 0; i < pi.size(); i++) {
        std::cout << "pi[" << i + 1 << "] = " << pi[i] + 1 << std::endl;
    }
    std::cout << std::endl << std::endl;
    // Print the values of lambda
    for (std::size_t i = 0; i < pi.size(); i++) {
        std::cout << "l[" << i + 1 << "] = " << std::fixed << std::setprecision(2) << lambda[i]
                  << std::endl;
    }
    std::cout << std::endl;

    // String containing a row of the distance matrix, properly formatted
    std::vector<std::string> values{};

    for (std::size_t i = 1; i < data.size(); i++) {
        std::cout << "M[" << i + 1 << "] = ";
        values.clear();

        for (std::size_t j = 0; j < i; j++) {
            // Compute the distance between the points
            double distance = 0;
            for (std::size_t d = 0; d < dimension; d++) {
                distance += pow(data[i][d] - data[j][i], 2.0);
            }
            distance = sqrt(distance);
            // Append the value
            std::ostringstream stream{};
            stream << std::fixed << std::setprecision(2) << distance;
            std::cout << stream.str() << ' ';
            values.push_back(stream.str());
        }
        // Checks if all the distances are different
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

/**
 * Utility function that checks the results of the clustering algorithms.
 *
 * @param filePath Path of the file containing the data samples to cluster.
 * @param data Data structure holding the samples to cluster.
 * @param dimension Dimension of the samples to cluster.
 * @param resultPi Data structure holding the values of <code>pi</code> computed by the clustering
 * algorithm.
 * @param resultLambda Data structure holding the values of <code>lambda</code> computed by the
 * clustering algorithm.
 * @param usePreviousResults <code>true</code> if the function should use the results already
 * computed in a previous execution and stored in a file, <code>false</code> if the function should
 * execute the sequential implementation of the clustering algorithm to compute the correct results.
 * @param previousResultsPath Path of the file that contains the results already computed in a
 * previous execution. If the file does not exist, this function executed the sequential
 * implementation of the clustering algorithm and then stores the results in the file with the
 * specified path.
 * @return <code>true</code> if the results of the clustering algorithm are correct,
 * <code>false</code> otherwise.
 */
bool checkTest(const std::filesystem::path &filePath,
               const std::vector<double *> &data,
               const std::size_t dimension,
               const std::vector<std::size_t> &resultPi,
               const std::vector<double> &resultLambda,
               const bool usePreviousResults,
               const std::filesystem::path &previousResultsPath) {

    // Create the vectors containing the correct results
    std::vector<std::size_t> expectedPi{};
    std::vector<double> expectedLambda{};

    std::string fileName{filePath.filename().string()};

    // Check if the test is a well-known test
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
        // Retrieve the correct results
        if (usePreviousResults && std::filesystem::is_regular_file(previousResultsPath)) {
            // Read the results from file
            std::cout << "Reading file '" << previousResultsPath.string()
                      << "' to retrieve the results of the sequential implementation of '"
                      << fileName << "' so to check the results" << std::endl;
            DataReader::readPiLambda(previousResultsPath, expectedPi, expectedLambda);
        } else {
            // Resize pi and lambda to hold the results
            expectedPi.resize(data.size());
            expectedLambda.resize(data.size());

            // Extract the iterators
            auto piIterator = expectedPi.begin();
            auto lambdaIterator = expectedLambda.begin();
            // Execute the sequential implementation to get the correct results
            cluster::utils::Timer::zeroTimers();
            std::cout << "Running sequential implementation of '" << fileName
                      << "' to check the results" << std::endl;
            SequentialClustering::cluster(
                    data.begin(), data.size(), dimension, piIterator, lambdaIterator);
            // Store the results, if requested
            if (usePreviousResults) {
                DataWriter::writePiLambda(previousResultsPath, expectedPi, expectedLambda);
            }
        }
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    // CHeck if the computed results are correct
    return ResultsChecker::checkResults(resultPi.cbegin(),
                                        resultPi.cend(),
                                        resultLambda.cbegin(),
                                        resultLambda.cend(),
                                        expectedPi.cbegin(),
                                        expectedPi.cend(),
                                        expectedLambda.cbegin(),
                                        expectedLambda.cend());
}

/**
 * Utility function that appends to the specified vector the specified elements.
 *
 * @tparam T Type of the elements to append.
 * @param vector Vector where the elements will be appended.
 * @param elements Elements to append.
 */
template <typename T>
void addAll(std::vector<T> &vector, const std::initializer_list<T> elements) {

    for (const T &value : elements) {
        vector.template emplace_back(value);
    }
}
