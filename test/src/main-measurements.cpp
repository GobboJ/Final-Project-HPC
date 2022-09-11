/*
 * Application that measures the time taken to execute one implementation of the clustering
 * algorithm.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.3 2022-09-11
 * @since 1.0
 */
#include "DistanceComputers.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"
#include "data/DataReader.h"
#include "data/DataWriter.h"
#include "data/ResultsChecker.h"
#include <cmath>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mm_malloc.h>
#include <vector>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;
using cluster::utils::ParallelDataIterator;
using cluster::utils::Timer;
using DistanceComputers = cluster::parallel::DistanceComputers;
using cluster::test::ResultsChecker;
using cluster::test::data::DataReader;
using cluster::test::data::DataWriter;

const constexpr bool CHECK_ALIGNMENT = false;

/**
 * Utility class allowing to measure the time taken to execute an implementation of clustering
 * algorithm, either sequential or parallel.
 *
 * @tparam PD <code>true</code> if the parallel implementation of the clustering algorithm should
 * parallelize the computation of the distance using threads, <code>false</code> otherwise.
 * @tparam PF <code>true</code> if the parallel implementation of the clustering algorithm should
 * parallelize using threads the computation of the structural fix after a new point is added to the
 * dendrogram, <code>false</code> otherwise.
 * @tparam PS <code>true</code> if the parallel implementation of the clustering algorithm should
 * parallelize the computation of the square roots using threads, <code>false</code> otherwise.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-09-11
 * @since 1.0
 */
template <bool PD = true, bool PF = false, bool PS = false>
class ClusteringAlgorithmExecutor {

public:
    /**
     * Measures the time taken to execute a sequential implementation of the clustering algorithm.
     *
     * @tparam D Type of the data structure/iterator holding the data samples to cluster.
     * @param data Data to cluster.
     * @param dataElementsCount Number of samples to cluster.
     * @param dimension Dimension of the samples.
     * @param piVector Vector where the computed values of <code>pi</code> will be placed.
     * @param lambdaVector Vector where the computed values of <code>lambda</code> will be placed.
     */
    template <typename D>
    static inline void executeSequentialClustering(const D &data,
                                                   const std::size_t dataElementsCount,
                                                   const std::size_t dimension,
                                                   std::vector<std::size_t> &piVector,
                                                   std::vector<double> &lambdaVector) {

        // Zero the timers
        Timer::zeroTimers();

        // Execute the algorithm
        auto piBegin = piVector.begin();
        auto lambdaBegin = lambdaVector.begin();
        SequentialClustering::cluster(data, dataElementsCount, dimension, piBegin, lambdaBegin);
    }

    /**
     * Measures the time taken to execute a parallel implementation of the clustering algorithm.
     *
     * @tparam C Distance computer to use to compute the distance between two data samples.
     * @tparam D Type of the data structure/iterator holding the data samples to cluster.
     * @param data Data to cluster.
     * @param dataElementsCount Number of samples to cluster.
     * @param dimension Dimension of the samples.
     * @param threadCount Number of threads to use to parallelize the various steps of the
     * implementation.
     * @param piVector Vector where the computed values of <code>pi</code> will be placed.
     * @param lambdaVector Vector where the computed values of <code>lambda</code> will be placed.
     */
    template <DistanceComputers C, ParallelDataIterator D>
    static inline void executeParallelClustering(const D &data,
                                                 const std::size_t dataElementsCount,
                                                 const std::size_t dimension,
                                                 const std::size_t threadCount,
                                                 std::vector<std::size_t> &piVector,
                                                 std::vector<double> &lambdaVector) {

        // Zero the timers
        Timer::zeroTimers();

        // Execute the algorithm
        auto piBegin = piVector.begin();
        auto lambdaBegin = lambdaVector.begin();
        ParallelClustering<PD, PF, PS, CHECK_ALIGNMENT>::template cluster<C>(data,
                                                                             dataElementsCount,
                                                                             dimension,
                                                                             piBegin,
                                                                             lambdaBegin,
                                                                             threadCount,
                                                                             threadCount,
                                                                             threadCount);
    }
};

/**
 * Convenient declaration that allows to produce a compile-time assertion error.
 *
 * @tparam T A numeric value.
 */
template <int T>
static constexpr std::false_type always_false{};

/**
 * Initializes the specified data structures.
 *
 * @param dataIterator Iterator over the data samples to cluster. The data samples as well as the
 * attributes of each data sample must be contiguous in memory.
 * @param dataElementsCount Number of samples to cluster.
 * @param dimension Number of attributes of each sample.
 * @param indirectData Vector that will contain the samples to cluster organized in two levels.
 * @param sseIndirectData Vector that will contain the samples to cluster organized in two levels,
 * where the second level is properly aligned so to be used in SSE instructions.
 * @param avxIndirectData Vector that will contain the samples to cluster organized in two levels,
 * where the second level is properly aligned so to be used in AVX instructions.
 * @param uniqueVectorData Array where all the samples to cluster will be stored one after the
 * other.
 * @param sseMMAlignedData Array where all the samples to cluster will be stored one after the
 * other, and aligned so that each data samples can be used in SSE instructions.
 * @param avxMMAlignedData Array where all the samples to cluster will be stored one after the
 * other, and aligned so that each data samples can be used in AVX instructions.
 */
void initializeDataStructures(const double *dataIterator,
                              std::size_t dataElementsCount,
                              std::size_t dimension,
                              std::vector<double *> &indirectData,
                              std::vector<double *> &sseIndirectData,
                              std::vector<double *> &avxIndirectData,
                              double *&uniqueVectorData,
                              double *&sseMMAlignedData,
                              double *&avxMMAlignedData);

/**
 * Prints the usage to the console.
 */
void usage();

#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"
#pragma ide diagnostic ignored "UnreachableCode"

/**
 * Main entrypoint of the application.<br>
 * The behaviour of this method can be customized using the following macros:
 * <ul>
 *      <li><code>IS_PARALLEL</code>: if <code>0</code> executes a sequential implementation of the
 * clustering algorithm, if <code>1</code> executes a parallel implementation of the clustering
 * algorithm.</li>
 *      <li><code>VERSION</code>: Version of the clustering algorithm to execute.</li>
 * </ul>
 *
 * @param argc Number of arguments specified via the command line.
 * @param argv Array containing the arguments specified via the command line.
 * @return The status code.
 */
int main(int argc, char *argv[]) {

    // Check if the help has been requested
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        usage();
        return 0;
    }

    // Check the number of arguments
    if (argc != 4) {
        std::cerr << "Wrong number of arguments specified." << std::endl << std::endl;
        usage();
        return 1;
    }
    // Parse the index of the dataset
    // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
    std::size_t dataSetIndex;
    try {
        std::size_t lastParsedCharacterIndex = 0;
        dataSetIndex = std::stoull(argv[1], &lastParsedCharacterIndex);
        // Check if it has been parsed
        if (lastParsedCharacterIndex == 0) {
            std::cerr << "Wrong dataset index " << argv[1] << '.' << std::endl << std::endl;
            return 2;
        }
    } catch (std::exception &) {
        std::cerr << "Wrong dataset index " << argv[1] << '.' << std::endl << std::endl;
        return 2;
    }

    // Parse the number of threads to use
    // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
    std::size_t threadsCount;
    try {
        std::size_t lastParsedCharacterIndex = 0;
        threadsCount = std::stoull(argv[3], &lastParsedCharacterIndex);
        // Check if it has been parsed
        if (lastParsedCharacterIndex == 0) {
            std::cerr << "Wrong threads count" << ' ' << argv[3] << '.' << std::endl << std::endl;
            return 2;
        }
    } catch (std::exception &) {
        std::cerr << "Wrong threads count" << ' ' << argv[3] << '.' << std::endl << std::endl;
        return 2;
    }

    // Compute the dataset to test
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    std::tuple<std::string, std::size_t, std::size_t> dataset;
    if (dataSetIndex == 0) {
        dataset = {"accelerometer.csv", 3, 5};
    } else {
        dataset = {"generated.data", 1, 45};
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    // Print a summary
    std::cout << " * "
              << "Running" << ' ' << ((IS_PARALLEL == 0) ? "sequential" : "parallel") << " version"
              << ' ' << VERSION;
    if (IS_PARALLEL) {
        std::cout << " with" << ' ' << threadsCount << "threads";
    }
    std::cout << " on" << ' ' << std::get<0>(dataset) << std::endl;

    // Read the data
    std::vector<double> data{};
    std::size_t dimension = DataReader::readAndParseData(
            std::filesystem::path{"../../test/resources"} / std::get<0>(dataset),
            data,
            0,
            0,
            std::get<1>(dataset),
            std::get<2>(dataset));
    std::size_t dataElementsCount = data.size() / dimension;

    // Allocate the indirect data
    std::vector<double *> indirectData{};
    std::vector<double *> sseIndirectData{};
    std::vector<double *> avxIndirectData{};
    // Allocate the containers
    double *sseMMAlignedData = nullptr;
    double *avxMMAlignedData = nullptr;
    double *uniqueVectorData = nullptr;

    // Fill the data structures
    initializeDataStructures(data.data(),
                             dataElementsCount,
                             dimension,
                             indirectData,
                             sseIndirectData,
                             avxIndirectData,
                             uniqueVectorData,
                             sseMMAlignedData,
                             avxMMAlignedData);

    // Resize pi and lambda
    std::vector<std::size_t> expectedPi{};
    expectedPi.resize(dataElementsCount);
    std::vector<double> expectedLambda{};
    expectedLambda.resize(dataElementsCount);

    // Compute the path of the file that contains the correct values of pi and lambda
    std::string resultsFileName{"./results-"};
    resultsFileName += std::to_string(dataSetIndex);
    std::filesystem::path resultsPath{resultsFileName};
    resultsPath = absolute(resultsPath).lexically_normal();
    // Check for its existence
    if (std::filesystem::exists(resultsPath)) {
        // Retrieve the values
        std::cout << "Reading the results from the file \'" << resultsPath.string() << '\''
                  << std::endl;
        DataReader::readPiLambda(resultsPath, expectedPi, expectedLambda);
    } else {
        // Execute the sequential implementation to compute the correct values for pi and lambda
        std::cout << "Executing the sequential implementation to check the results" << std::endl;
        auto dataBegin = indirectData.begin();
        auto piBegin = expectedPi.begin();
        auto lambdaBegin = expectedLambda.begin();

        SequentialClustering::cluster(
                dataBegin, dataElementsCount, dimension, piBegin, lambdaBegin);

        DataWriter::writePiLambda(resultsPath, expectedPi, expectedLambda);
    }

    // Initialize pi and lambda
    std::vector<std::size_t> piVector{};
    piVector.resize(dataElementsCount);
    std::vector<double> lambdaVector{};
    lambdaVector.resize(dataElementsCount);

    // Execute the requested implementation
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    if constexpr (IS_PARALLEL == 0) {
        // Measure the time taken to execute the sequential implementation with the specified
        // version
        if constexpr (VERSION == 1) {
            ClusteringAlgorithmExecutor<>::executeSequentialClustering(
                    indirectData.begin(), dataElementsCount, dimension, piVector, lambdaVector);
        } else if constexpr (VERSION == 2) {
            ClusteringAlgorithmExecutor<>::executeSequentialClustering(
                    uniqueVectorData, dataElementsCount, dimension, piVector, lambdaVector);
        } else {
            always_false<VERSION>();
        }
    } else if constexpr (IS_PARALLEL == 1) {
        // Measure the time taken to execute the parallel implementation with the specified version
        if constexpr (VERSION == 1) {
            ClusteringAlgorithmExecutor<>::executeParallelClustering<DistanceComputers::CLASSICAL>(
                    indirectData.begin(),
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    piVector,
                    lambdaVector);
        } else if constexpr (VERSION == 2) {
            ClusteringAlgorithmExecutor<>::executeParallelClustering<DistanceComputers::SSE>(
                    sseIndirectData.begin(),
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    piVector,
                    lambdaVector);
        } else if constexpr (VERSION == 3) {
            ClusteringAlgorithmExecutor<>::executeParallelClustering<DistanceComputers::AVX>(
                    avxIndirectData.begin(),
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    piVector,
                    lambdaVector);
        } else if constexpr (VERSION == 4) {
            ClusteringAlgorithmExecutor<>::executeParallelClustering<DistanceComputers::SSE>(
                    sseMMAlignedData,
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    piVector,
                    lambdaVector);
        } else if constexpr (VERSION == 5) {
            ClusteringAlgorithmExecutor<>::executeParallelClustering<DistanceComputers::AVX>(
                    avxMMAlignedData,
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    piVector,
                    lambdaVector);
        } else if constexpr (VERSION == 6) {
            ClusteringAlgorithmExecutor<true, true>::executeParallelClustering<
                    DistanceComputers::AVX>(avxMMAlignedData,
                                            dataElementsCount,
                                            dimension,
                                            threadsCount,
                                            piVector,
                                            lambdaVector);
        } else if constexpr (VERSION == 7) {
            ClusteringAlgorithmExecutor<true, true>::executeParallelClustering<
                    DistanceComputers::AVX_OPTIMIZED>(avxMMAlignedData,
                                                      dataElementsCount,
                                                      dimension,
                                                      threadsCount,
                                                      piVector,
                                                      lambdaVector);
        } else if constexpr (VERSION == 8) {
            ClusteringAlgorithmExecutor<true, true, true>::executeParallelClustering<
                    DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT>(sseMMAlignedData,
                                                                     dataElementsCount,
                                                                     dimension,
                                                                     threadsCount,
                                                                     piVector,
                                                                     lambdaVector);
        } else if constexpr (VERSION == 9) {
            ClusteringAlgorithmExecutor<true, true, true>::executeParallelClustering<
                    DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(avxMMAlignedData,
                                                                     dataElementsCount,
                                                                     dimension,
                                                                     threadsCount,
                                                                     piVector,
                                                                     lambdaVector);
        } else if constexpr (VERSION == 10) {
            ClusteringAlgorithmExecutor<true, true, true>::executeParallelClustering<
                    DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(avxIndirectData,
                                                                     dataElementsCount,
                                                                     dimension,
                                                                     threadsCount,
                                                                     piVector,
                                                                     lambdaVector);
        } else {
            always_false<VERSION>();
        }
    } else {
        always_false<IS_PARALLEL>();
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    // Check if the computed pi and lambda are correct
    if (!ResultsChecker::checkResults(piVector.cbegin(),
                                      piVector.cend(),
                                      lambdaVector.cbegin(),
                                      lambdaVector.cend(),
                                      expectedPi.cbegin(),
                                      expectedPi.cend(),
                                      expectedLambda.cbegin(),
                                      expectedLambda.cend())) {
        std::cerr << "Test result: Error!" << std::endl;
    } else {
        std::cout << "Test result: OK" << std::endl;
    }

    // Deallocate the data
    for (double *point : indirectData) {
        delete[] point;
    }
    for (double *ssePoint : sseIndirectData) {
        _mm_free(ssePoint);
    }
    for (double *avxPoint : avxIndirectData) {
        _mm_free(avxPoint);
    }

    _mm_free(sseMMAlignedData);
    _mm_free(avxMMAlignedData);
    delete[] uniqueVectorData;

    // Print the mean execution times to file
    std::ofstream meanTimesFile{argv[2], std::ios_base::app};

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    if (meanTimesFile) {
        Timer::print<0, true>(meanTimesFile);
        Timer::print<1, true>(meanTimesFile);
        Timer::print<2, true>(meanTimesFile);
        Timer::print<3, true>(meanTimesFile);
        Timer::print<4, true>(meanTimesFile);
        Timer::print<5, true>(meanTimesFile);
        Timer::printTotal<true, 0, 1, 2, 3, 4, 5>(meanTimesFile);
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    meanTimesFile << std::endl;

    return 0;
}

#pragma clang diagnostic pop

/**
 * Initializes the specified data structures.
 *
 * @param dataIterator Iterator over the data samples to cluster. The data samples as well as the
 * attributes of each data sample must be contiguous in memory.
 * @param dataElementsCount Number of samples to cluster.
 * @param dimension Number of attributes of each sample.
 * @param indirectData Vector that will contain the samples to cluster organized in two levels.
 * @param sseIndirectData Vector that will contain the samples to cluster organized in two levels,
 * where the second level is properly aligned so to be used in SSE instructions.
 * @param avxIndirectData Vector that will contain the samples to cluster organized in two levels,
 * where the second level is properly aligned so to be used in AVX instructions.
 * @param uniqueVectorData Array where all the samples to cluster will be stored one after the
 * other.
 * @param sseMMAlignedData Array where all the samples to cluster will be stored one after the
 * other, and aligned so that each data samples can be used in SSE instructions.
 * @param avxMMAlignedData Array where all the samples to cluster will be stored one after the
 * other, and aligned so that each data samples can be used in AVX instructions.
 */
void initializeDataStructures(const double *const dataIterator,
                              const std::size_t dataElementsCount,
                              const std::size_t dimension,
                              std::vector<double *> &indirectData,
                              std::vector<double *> &sseIndirectData,
                              std::vector<double *> &avxIndirectData,
                              double *&uniqueVectorData,
                              double *&sseMMAlignedData,
                              double *&avxMMAlignedData) {

    // Compute the strides
    const std::size_t sseStride = ParallelClustering<>::computeSseDimension(dimension);
    const std::size_t avxStride = ParallelClustering<>::computeAvxDimension(dimension);

    // Initialize the indirect data structures
    for (std::size_t i = 0; i < dataElementsCount; i++) {
        // Initialize the non-aligned indirect point
        auto *point = new double[dimension];
        // Copy the doubles
        memcpy(point, &(dataIterator[i * dimension]), dimension * sizeof(double));
        indirectData.push_back(point);

        // Initialize the SSE-aligned indirect point
        auto *ssePoint = static_cast<double *>(_mm_malloc(
                sseStride * sizeof(double), ParallelClustering<>::SSE_PACK_SIZE * sizeof(double)));
        // Copy the doubles
        memcpy(ssePoint, &(dataIterator[i * dimension]), dimension * sizeof(double));
        // Fill the remaining coordinates with 0
        memset(&(ssePoint[dimension]), 0, (sseStride - dimension) * sizeof(double));

        // Add the point
        sseIndirectData.push_back(ssePoint);

        // Initialize the AVX-aligned indirect point
        auto *avxPoint = static_cast<double *>(_mm_malloc(
                avxStride * sizeof(double), ParallelClustering<>::AVX_PACK_SIZE * sizeof(double)));
        // Copy the doubles
        memcpy(avxPoint, &(dataIterator[i * dimension]), dimension * sizeof(double));
        // Fill the remaining coordinates with 0
        memset(&(avxPoint[dimension]), 0, (avxStride - dimension) * sizeof(double));

        // Add the point
        avxIndirectData.push_back(avxPoint);
    }

    // Compute the aligned data arrays size
    std::size_t sseSize = sizeof(double) * (dataElementsCount * sseStride);
    std::size_t avxSize = sizeof(double) * (dataElementsCount * avxStride);

    // Allocate the containers
    sseMMAlignedData = static_cast<double *>(
            _mm_malloc(sseSize, ParallelClustering<>::SSE_PACK_SIZE * sizeof(double)));
    avxMMAlignedData = static_cast<double *>(
            _mm_malloc(avxSize, ParallelClustering<>::AVX_PACK_SIZE * sizeof(double)));
    uniqueVectorData = new double[dataElementsCount * dimension];

    // Zero the aligned data
    memset(sseMMAlignedData, 0, sseSize);
    memset(avxMMAlignedData, 0, avxSize);

    // Fill the containers
    for (std::size_t i = 0; i < dataElementsCount; i++) {
        const double *currentDataBegin = &(dataIterator[i * dimension]);
        // Copy the values
        memcpy(&(sseMMAlignedData[i * sseStride]), currentDataBegin, dimension * sizeof(double));
        memcpy(&(avxMMAlignedData[i * avxStride]), currentDataBegin, dimension * sizeof(double));
        memcpy(&(uniqueVectorData[i * dimension]), currentDataBegin, dimension * sizeof(double));
    }
}

#pragma clang diagnostic pop

/**
 * Prints the usage to the console.
 */
void usage() {

    std::cout <<
            R""(SYNOPSYS
    main-measurements -h | --help
    main-measurements DATASET_INDEX OUTPUT_FILE THREADS_COUNT

DESCRIPTION
    Measures the time taken to execute the compiled clustering algorithm
    implementation against the specified dataset.
    In particular, DATASET_INDEX expresses the index of the dataset to test
    where:
        - 0 tests the "accelerometer.csv" dataset, columns from 3 to 5;
        - 1 tests the "generated.data" dataset, columns from 1 to 45.
    
    The measured execution times are appended to the file OUTPUT_FILE, which
    must be a valid path.
    
    If the compiled implementation is a parallel one, then THREADS_COUNT
    specifies the number of threads to use when parallelizing the steps of the
    algorithm.

OPTIONS
    -h, --help
        Prints this help.

EXIT CODE
    0   If the test has been executed successfully, or the help has been
            requested.
    1   If a wrong number of arguments has been specified in the command line.
    2   If a wrong argument has been specified.

)"";
}
