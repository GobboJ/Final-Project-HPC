/*
 * Application that measures the time taken to execute the implementations of the clustering
 * algorithm with several data structures.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2.1 2022-08-17
 * @since 1.0
 */
#include "DistanceComputers.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"
#include "data/DataReader.h"
#include "data/ResultsChecker.h"
#include "data/DataWriter.h"
#include <cmath>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <mm_malloc.h>
#include <vector>
#include <fstream>

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
 * Utility class allowing to measure the time taken to execute the parallel implementation
 * clustering algorithm.
 *
 * @tparam PD <code>true</code> if the parallel implementation of the clustering algorithm should
 * parallelize the computation of the distance using threads, <code>false</code> otherwise.
 * @tparam PF <code>true</code> if the parallel implementation of the clustering algorithm should
 * parallelize using threads the computation of the structural fix after a new point is added to the
 * dendrogram, <code>false</code> otherwise.
 * @tparam PS <code>true</code> if the parallel implementation of the clustering algorithm should
 * parallelize the computation of the square roots using threads, <code>false</code> otherwise.
 */
template <bool PD = true, bool PF = false, bool PS = false>
class ParallelClusteringAlgorithmExecutor {

public:
    /**
     * Measures the mean time of 3 executions of the parallel implementation of the clustering
     * algorithm.
     *
     * @tparam C Distance computer to use to compute the distance between two data samples.
     * @tparam D Type of the data structure/iterator holding the data samples to cluster.
     * @param title Title to print to the console.
     * @param data Data to cluster.
     * @param dataElementsCount Number of samples to cluster.
     * @param dimension Dimension of the samples.
     * @param threadCounts Initializer list containing the number of threads to use to parallelize
     * the various steps of the implementation.
     * @param expectedPi Vector containing the expected values for <code>pi</code>.
     * @param expectedLambda Vector containing the expected values for <code>lambda</code>.
     */
    template <DistanceComputers C, ParallelDataIterator D>
    static inline void iterateParallelClustering(const std::string &title,
                                                 const D &data,
                                                 const std::size_t dataElementsCount,
                                                 const std::size_t dimension,
                                                 const std::size_t threadCount,
                                                 const std::vector<std::size_t> &expectedPi,
                                                 const std::vector<double> &expectedLambda) {

        // Initialize pi and lambda
        std::vector<std::size_t> pi{};
        pi.resize(dataElementsCount);
        std::vector<double> lambda{};
        lambda.resize(dataElementsCount);

        // Print the title
        std::cout << "--------------" << std::endl
                  << title << std::endl
                  << "--------------" << std::endl
                  << std::endl;

        // Execute the algorithm for any of the specified number of threads
        // Print the number of threads used
        std::cout << "*********" << std::endl
                  << title << std::endl
                  << "Testing with" << ' ' << threadCount << " threads." << std::endl
                  << "*********" << std::endl;
        // Zero the timers
        Timer::zeroTimers();

        // Execute the algorithm
        auto piBegin = pi.begin();
        auto lambdaBegin = lambda.begin();
        ParallelClustering<PD, PF, PS, CHECK_ALIGNMENT>::template cluster<C>(data,
                                                                             dataElementsCount,
                                                                             dimension,
                                                                             piBegin,
                                                                             lambdaBegin,
                                                                             threadCount,
                                                                             threadCount,
                                                                             threadCount);

        // Check if the computed pi and lambda are correct
        if (!ResultsChecker::checkResults(pi.cbegin(),
                                          pi.cend(),
                                          lambda.cbegin(),
                                          lambda.cend(),
                                          expectedPi.cbegin(),
                                          expectedPi.cend(),
                                          expectedLambda.cbegin(),
                                          expectedLambda.cend())) {
            std::cerr << "Error!" << std::endl;
        } else {
            std::cout << "Ok!" << std::endl;
        }
    }
};

/**
 * Measures the mean time of 3 executions of the sequential implementation of the clustering
 * algorithm.
 *
 * @tparam D Type of the data structure/iterator holding the data samples to cluster.
 * @param title Title to print to the console.
 * @param data Data to cluster.
 * @param dataElementsCount Number of samples to cluster.
 * @param dimension Dimension of the samples.
 * @param expectedPi Vector containing the expected values for <code>pi</code>.
 * @param expectedLambda Vector containing the expected values for <code>lambda</code>.
 */
template <typename D>
inline void iterateSequentialClustering(const std::string &title,
                                        const D &data,
                                        const std::size_t dataElementsCount,
                                        const std::size_t dimension,
                                        const std::vector<std::size_t> &expectedPi,
                                        const std::vector<double> &expectedLambda) {

    // Resize pi and lambda
    std::vector<std::size_t> pi{};
    pi.resize(dataElementsCount);
    std::vector<double> lambda{};
    lambda.resize(dataElementsCount);

    // Print the title
    std::cout << "--------------" << std::endl
              << title << std::endl
              << "--------------" << std::endl
              << std::endl;

    // Zero the timers
    Timer::zeroTimers();

    // Execute the algorithm
    auto piBegin = pi.begin();
    auto lambdaBegin = lambda.begin();
    SequentialClustering::cluster(data, dataElementsCount, dimension, piBegin, lambdaBegin);

    // Check if the computed pi and lambda are correct
    if (!ResultsChecker::checkResults(pi.cbegin(),
                                      pi.cend(),
                                      lambda.cbegin(),
                                      lambda.cend(),
                                      expectedPi.cbegin(),
                                      expectedPi.cend(),
                                      expectedLambda.cbegin(),
                                      expectedLambda.cend())) {
        std::cerr << "Error!" << std::endl;
    } else {
        std::cout << "Ok!" << std::endl;
    }
}

/**
 * Convenient declaration that allows to produce a compile-time assertion error.
 *
 * @tparam Ts This must be one or more type parameters of the function that uses this
 * declaration.
 */
template <int T>
static constexpr std::false_type always_false{};

/*
 * Main entrypoint of the application.
 *
 * IS_PARALLEL
 * VERSION
 * DATASET_INDEX
 */
int main(int argc, char *argv[]) {

    // Dataset to measure
    std::tuple<std::string, std::size_t, std::size_t> dataset;

    if (argc != 4) {
        std::cerr << "Too few arguments." << std::endl << std::endl;
        std::cerr << "Usage: main-measurements <0 | 1> OUT_FILE TH_COUNT" << std::endl;
        std::cerr << "  0 = Accelerometer, 1 = Generated" << std::endl;
        return 1;
    }

    std::size_t last = 0;
    int dataSetIndex = std::stoi(argv[1], &last);
    if (last == 0) {
        std::cerr << "Wrong dataset index " << argv[1] << '.' << std::endl << std::endl;
        return 2;
    }

    last = 0;
    std::size_t threadsCount = std::stoull(argv[3], &last);
    if (last == 0) {
        std::cerr << "Wrong threads count " << argv[3] << '.' << std::endl << std::endl;
        return 2;
    }

    if (dataSetIndex == 0) {
        dataset = {"accelerometer.csv", 3, 5};
    } else {
        dataset = {"Parking Birmingham.data", 2, 3};
    }

    // Print the name of the dataset
    std::cout << std::endl
              << "===================" << std::endl
              << "Dataset: " << std::get<0>(dataset) << std::endl
              << "===================" << std::endl
              << std::endl;

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

    // Compute the strides
    const std::size_t sseStride = ParallelClustering<>::computeSseDimension(dimension);
    const std::size_t avxStride = ParallelClustering<>::computeAvxDimension(dimension);
    // Extract the data to cluster
    const auto *const dataIterator = data.data();

    // Fill the indirect data
    std::vector<double *> indirectData{};
    std::vector<double *> sseIndirectData{};
    std::vector<double *> avxIndirectData{};
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
    auto *sseMMAlignedData = static_cast<double *>(
            _mm_malloc(sseSize, ParallelClustering<>::SSE_PACK_SIZE * sizeof(double)));
    auto *avxMMAlignedData = static_cast<double *>(
            _mm_malloc(avxSize, ParallelClustering<>::AVX_PACK_SIZE * sizeof(double)));
    auto *uniqueVectorData = new double[data.size()];

    // Zero the aligned data
    memset(sseMMAlignedData, 0, sseSize);
    memset(avxMMAlignedData, 0, avxSize);

    // Fill the contiguous data
    for (std::size_t i = 0; i < dataElementsCount; i++) {
        const double *currentDataBegin = &(dataIterator[i * dimension]);
        // Copy the values
        memcpy(&(sseMMAlignedData[i * sseStride]), currentDataBegin, dimension * sizeof(double));
        memcpy(&(avxMMAlignedData[i * avxStride]), currentDataBegin, dimension * sizeof(double));
        memcpy(&(uniqueVectorData[i * dimension]), currentDataBegin, dimension * sizeof(double));
    }

    // Resize pi and lambda
    std::vector<std::size_t> pi{};
    pi.resize(dataElementsCount);
    std::vector<double> lambda{};
    lambda.resize(dataElementsCount);

    std::string resultsPath = "./results-";
    resultsPath += std::to_string(dataSetIndex);
    if (std::filesystem::exists(resultsPath)) {
        std::cout << "Reading the results" << std::endl;
        DataReader::readPiLambda(resultsPath, pi, lambda);
    } else {
        // Execute the sequential implementation to compute the correct values for pi and lambda
        std::cout << "Executing the sequential implementation to check the results" << std::endl;
        auto dataBegin = indirectData.begin();
        auto piBegin = pi.begin();
        auto lambdaBegin = lambda.begin();

        SequentialClustering::cluster(
                dataBegin, dataElementsCount, dimension, piBegin, lambdaBegin);

        DataWriter::writePiLambda(resultsPath, pi, lambda);
    }

    if constexpr (IS_PARALLEL == 0) {
        // Measure the time taken to execute the sequential implementations
        if constexpr (VERSION == 1) {
            iterateSequentialClustering(
                    "Sequential 1", indirectData.begin(), dataElementsCount, dimension, pi, lambda);
        } else if constexpr (VERSION == 2) {
            iterateSequentialClustering("Sequential 2: Linearized",
                                        uniqueVectorData,
                                        dataElementsCount,
                                        dimension,
                                        pi,
                                        lambda);
        } else {
            always_false<VERSION>();
        }
    } else {
        if constexpr (VERSION == 1) {
            // Measure the time taken to execute the parallel implementations
            ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<
                    DistanceComputers::CLASSICAL>("Parallel 1: Multi-threaded Distance Computation",
                                                  indirectData.begin(),
                                                  dataElementsCount,
                                                  dimension,
                                                  threadsCount,
                                                  pi,
                                                  lambda);
        } else if constexpr (VERSION == 2) {
            ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<
                    DistanceComputers::SSE>("Parallel 2: Multi-threaded Distance Computation + SSE",
                                            sseIndirectData.begin(),
                                            dataElementsCount,
                                            dimension,
                                            threadsCount,
                                            pi,
                                            lambda);
        } else if constexpr (VERSION == 3) {
            ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<
                    DistanceComputers::AVX>("Parallel 3: Multi-threaded Distance Computation + AVX",
                                            avxIndirectData.begin(),
                                            dataElementsCount,
                                            dimension,
                                            threadsCount,
                                            pi,
                                            lambda);
        } else if constexpr (VERSION == 4) {
            ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<
                    DistanceComputers::SSE>(
                    "Parallel 4: Multi-threaded Distance Computation + SSE + Linearized",
                    sseMMAlignedData,
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    pi,
                    lambda);
        } else if constexpr (VERSION == 5) {
            ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<
                    DistanceComputers::AVX>(
                    "Parallel 5: Multi-threaded Distance Computation + AVX + Linearized",
                    avxMMAlignedData,
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    pi,
                    lambda);
        } else if constexpr (VERSION == 6) {
            ParallelClusteringAlgorithmExecutor<true, true>::iterateParallelClustering<
                    DistanceComputers::AVX>(
                    "Parallel 6: Multi-threaded Distance Computation and Stage 4 + AVX + "
                    "Linearized",
                    avxMMAlignedData,
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    pi,
                    lambda);
        } else if constexpr (VERSION == 7) {
            ParallelClusteringAlgorithmExecutor<true, true>::iterateParallelClustering<
                    DistanceComputers::AVX_OPTIMIZED>(
                    "Parallel 7: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + "
                    "Linearized",
                    avxMMAlignedData,
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    pi,
                    lambda);
        } else if constexpr (VERSION == 8) {
            ParallelClusteringAlgorithmExecutor<true, true, true>::iterateParallelClustering<
                    DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT>(
                    "Parallel 8: Multi-threaded Distance Computation and Stage 4 + SSE Optimized "
                    "+ "
                    "Linearized + No Square Root",
                    sseMMAlignedData,
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    pi,
                    lambda);
        } else if constexpr (VERSION == 9) {
            ParallelClusteringAlgorithmExecutor<true, true, true>::iterateParallelClustering<
                    DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
                    "Parallel 9: Multi-threaded Distance Computation and Stage 4 + AVX Optimized "
                    "+ "
                    "Linearized + No Square Root",
                    avxMMAlignedData,
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    pi,
                    lambda);
        } else if constexpr (VERSION == 10) {
            ParallelClusteringAlgorithmExecutor<true, true, true>::iterateParallelClustering<
                    DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
                    "Parallel 10: Multi-threaded Distance Computation and Stage 4 + AVX Optimized "
                    "+ "
                    "Linearized + No Square Root + std::vector<double*>",
                    avxIndirectData,
                    dataElementsCount,
                    dimension,
                    threadsCount,
                    pi,
                    lambda);
        } else {
            always_false<VERSION>();
        }
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

    if (meanTimesFile) {
        Timer::print<0, true>(meanTimesFile);
        Timer::print<1, true>(meanTimesFile);
        Timer::print<2, true>(meanTimesFile);
        Timer::print<3, true>(meanTimesFile);
        Timer::print<4, true>(meanTimesFile);
        Timer::print<5, true>(meanTimesFile);
        Timer::printTotal<true, 0, 1, 2, 3, 4, 5>(meanTimesFile);
    }
    meanTimesFile << std::endl;

    return 0;
}
