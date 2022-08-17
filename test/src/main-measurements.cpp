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
#include <cmath>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <mm_malloc.h>
#include <vector>

using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;
using cluster::utils::ParallelDataIterator;
using cluster::utils::Timer;
using DistanceComputers = cluster::parallel::DistanceComputers;
using cluster::test::ResultsChecker;
using cluster::test::data::DataReader;

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
    static void iterateParallelClustering(const std::string &title,
                                          const D &data,
                                          const std::size_t dataElementsCount,
                                          const std::size_t dimension,
                                          const std::initializer_list<std::size_t> threadCounts,
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
        for (std::size_t threadCount : threadCounts) {
            // Print the number of threads used
            std::cout << "*********" << std::endl
                      << title << std::endl
                      << "Testing with" << ' ' << threadCount << " threads." << std::endl
                      << "*********" << std::endl;
            // Zero the timers
            Timer::zeroTimers();

            // Execute the algorithm 3 times
            for (std::size_t iteration = 0; iteration < 3; iteration++) {
                // Print the number of the iteration
                std::cout << "[!] Iteration: " << iteration + 1 << std::endl;
                // Execute the algorithm
                auto piBegin = pi.begin();
                auto lambdaBegin = lambda.begin();
                ParallelClustering<PD, PF, PS, CHECK_ALIGNMENT>::template cluster<C>(
                        data,
                        dataElementsCount,
                        dimension,
                        piBegin,
                        lambdaBegin,
                        threadCount,
                        threadCount,
                        threadCount);
            }
            // Print the mean execution times
            std::cout << std::endl << "[!] Mean" << std::endl;
            std::cout << "Service:" << ' ';
            Timer::print<0>(3);
            std::cout << "Stage 1:" << ' ';
            Timer::print<1>(3);
            std::cout << "Stage 2:" << ' ';
            Timer::print<2>(3);
            std::cout << "Stage 3:" << ' ';
            Timer::print<3>(3);
            std::cout << "Stage 4:" << ' ';
            Timer::print<4>(3);
            std::cout << "Stage 5:" << ' ';
            Timer::print<5>(3);
            std::cout << "Total  :" << ' ';
            Timer::printTotal<0, 1, 2, 3, 4, 5>(3);
            std::cout << std::endl;

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
void iterateSequentialClustering(const std::string &title,
                                 const D &data,
                                 const std::size_t dataElementsCount,
                                 const std::size_t dimension,
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

    // Zero the timers
    Timer::zeroTimers();

    // Execute the algorithm 3 times
    for (std::size_t iteration = 0; iteration < 3; iteration++) {
        // Print the number of the iteration
        std::cout << "[!] Iteration: " << iteration + 1 << std::endl;
        // Execute the algorithm
        auto piBegin = pi.begin();
        auto lambdaBegin = lambda.begin();
        SequentialClustering::cluster(data, dataElementsCount, dimension, piBegin, lambdaBegin);
    }
    // Print the mean execution times
    std::cout << std::endl << "[!] Mean" << std::endl;
    std::cout << "Service:" << ' ';
    Timer::print<0>(3);
    std::cout << "Stage 1:" << ' ';
    Timer::print<1>(3);
    std::cout << "Stage 2:" << ' ';
    Timer::print<2>(3);
    std::cout << "Stage 3:" << ' ';
    Timer::print<3>(3);
    std::cout << "Stage 4:" << ' ';
    Timer::print<4>(3);
    std::cout << "Stage 5:" << ' ';
    Timer::print<5>(3);
    std::cout << "Total  : ";
    Timer::printTotal<0, 1, 2, 3, 4, 5>(3);
    std::cout << std::endl;

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

/*
 * Main entrypoint of the application.
 */
int main() {

    // Dataset to measure
    std::vector<std::tuple<std::string, std::size_t, std::size_t>> datasets{
            {"accelerometer.csv", 3, 5}, {"generated.data", 1, 45}};

    // Loop over the datasets
    for (auto &dataset : datasets) {

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
            auto *ssePoint = static_cast<double *>(
                    _mm_malloc(sseStride * sizeof(double),
                               ParallelClustering<>::SSE_PACK_SIZE * sizeof(double)));
            // Copy the doubles
            memcpy(ssePoint, &(dataIterator[i * dimension]), dimension * sizeof(double));
            // Fill the remaining coordinates with 0
            memset(&(ssePoint[dimension]), 0, (sseStride - dimension) * sizeof(double));

            // Add the point
            sseIndirectData.push_back(ssePoint);

            // Initialize the AVX-aligned indirect point
            auto *avxPoint = static_cast<double *>(
                    _mm_malloc(avxStride * sizeof(double),
                               ParallelClustering<>::AVX_PACK_SIZE * sizeof(double)));
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
                _mm_malloc(sseSize, ParallelClustering<>::AVX_PACK_SIZE * sizeof(double)));
        auto *uniqueVectorData = new double[data.size()];

        // Zero the aligned data
        memset(sseMMAlignedData, 0, sseSize);
        memset(avxMMAlignedData, 0, avxSize);

        // Fill the contiguous data
        for (std::size_t i = 0; i < dataElementsCount; i++) {
            const double *currentDataBegin = &(dataIterator[i * dimension]);
            // Copy the values
            memcpy(&(sseMMAlignedData[i * sseStride]),
                   currentDataBegin,
                   dimension * sizeof(double));
            memcpy(&(avxMMAlignedData[i * avxStride]),
                   currentDataBegin,
                   dimension * sizeof(double));
            memcpy(&(uniqueVectorData[i * dimension]),
                   currentDataBegin,
                   dimension * sizeof(double));
        }

        // Resize pi and lambda
        std::vector<std::size_t> pi{};
        pi.resize(indirectData.size());
        std::vector<double> lambda{};
        lambda.resize(indirectData.size());

        // Execute the sequential implementation to compute the correct values for pi and lambda
        std::cout << "Executing the sequential implementation to check the results" << std::endl;
        auto dataBegin = indirectData.begin();
        auto piBegin = pi.begin();
        auto lambdaBegin = lambda.begin();

        SequentialClustering::cluster(
                dataBegin, dataElementsCount, dimension, piBegin, lambdaBegin);

        // Measure the time taken to execute the sequential implementations
        iterateSequentialClustering(
                "Sequential 1", indirectData.begin(), indirectData.size(), dimension, pi, lambda);

        iterateSequentialClustering("Sequential 2: Linearized",
                                    uniqueVectorData,
                                    indirectData.size(),
                                    dimension,
                                    pi,
                                    lambda);

        // Measure the time taken to execute the parallel implementations
        ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<
                DistanceComputers::CLASSICAL>("Parallel 1: Multi-threaded Distance Computation",
                                              indirectData.begin(),
                                              indirectData.size(),
                                              dimension,
                                              {2, 4, 8, 12, 16},
                                              pi,
                                              lambda);

        ParallelClusteringAlgorithmExecutor<false>::iterateParallelClustering<
                DistanceComputers::SSE>("Parallel 2: SSE",
                                        sseIndirectData.begin(),
                                        sseIndirectData.size(),
                                        dimension,
                                        {0},
                                        pi,
                                        lambda);

        ParallelClusteringAlgorithmExecutor<false>::iterateParallelClustering<
                DistanceComputers::AVX>("Parallel 3: AVX",
                                        avxIndirectData.begin(),
                                        avxIndirectData.size(),
                                        dimension,
                                        {0},
                                        pi,
                                        lambda);

        ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<DistanceComputers::SSE>(
                "Parallel 4: Multi-threaded Distance Computation + SSE",
                sseIndirectData.begin(),
                sseIndirectData.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<DistanceComputers::AVX>(
                "Parallel 5: Multi-threaded Distance Computation + AVX",
                avxIndirectData.begin(),
                avxIndirectData.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<DistanceComputers::SSE>(
                "Parallel 6: Multi-threaded Distance Computation + SSE + Linearized",
                sseMMAlignedData,
                sseIndirectData.size(),
                dimension,
                {4, 8, 12},
                pi,
                lambda);

        ParallelClusteringAlgorithmExecutor<>::iterateParallelClustering<DistanceComputers::AVX>(
                "Parallel 7: Multi-threaded Distance Computation + AVX + Linearized",
                avxMMAlignedData,
                avxIndirectData.size(),
                dimension,
                {4, 8, 12},
                pi,
                lambda);

        ParallelClusteringAlgorithmExecutor<true, true>::iterateParallelClustering<
                DistanceComputers::AVX>(
                "Parallel 8: Multi-threaded Distance Computation and Stage 4 + AVX + Linearized",
                avxMMAlignedData,
                avxIndirectData.size(),
                dimension,
                {4, 8, 12},
                pi,
                lambda);

        ParallelClusteringAlgorithmExecutor<true, true>::iterateParallelClustering<
                DistanceComputers::AVX_OPTIMIZED>(
                "Parallel 9: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + "
                "Linearized",
                avxMMAlignedData,
                avxIndirectData.size(),
                dimension,
                {4, 8, 12},
                pi,
                lambda);

        ParallelClusteringAlgorithmExecutor<true, true, true>::iterateParallelClustering<
                DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT>(
                "Parallel 10: Multi-threaded Distance Computation and Stage 4 + SSE Optimized + "
                "Linearized + No Square Root",
                sseMMAlignedData,
                sseIndirectData.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        ParallelClusteringAlgorithmExecutor<true, true, true>::iterateParallelClustering<
                DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
                "Parallel 11: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + "
                "Linearized + No Square Root",
                avxMMAlignedData,
                avxIndirectData.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        ParallelClusteringAlgorithmExecutor<true, true, true>::iterateParallelClustering<
                DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
                "Parallel 12: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + "
                "Linearized + No Square Root + std::vector<double*>",
                avxIndirectData,
                avxIndirectData.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

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
    }

    return 0;
}
