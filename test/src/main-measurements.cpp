#include "ContiguousDoubleMemoryDataIterator.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"
#include "data/DataReader.h"
#include "data/ResultsChecker.h"
#include "DistanceComputers.h"
#include "types/collections/AlignedArray.h"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <mm_malloc.h>
#include <vector>
#include <cmath>

using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;
using cluster::utils::ParallelDataIterator;
using cluster::utils::Timer;
using DistanceComputers = cluster::parallel::DistanceComputers;
using cluster::test::ResultsChecker;
using cluster::test::data::DataReader;
using cluster::test::types::collections::AlignedArray;

template <bool S2 = true, bool S4 = false, bool S5 = false>
class ClusteringAlgorithmExecutor {

public:
    template <DistanceComputers C, ParallelDataIterator D>
    static void iterateParallelClustering(const std::string &title,
                                          const D &data,
                                          std::size_t rows,
                                          std::size_t dimension,
                                          std::vector<std::size_t> threadCounts,
                                          const std::vector<std::size_t> &expectedPi,
                                          const std::vector<double> &expectedLambda) {

        std::vector<std::size_t> pi{};
        pi.resize(rows);
        std::vector<double> lambda{};
        lambda.resize(rows);

        std::cout << "--------------" << std::endl
                  << title << std::endl
                  << "--------------" << std::endl
                  << std::endl;

        for (std::size_t threadCount : threadCounts) {
            std::cout << "*********" << std::endl
                      << title << std::endl
                      << "Testing with" << ' ' << threadCount << " threads." << std::endl
                      << "*********" << std::endl;
            Timer::reset();

            for (std::size_t iteration = 0; iteration < 3; iteration++) {
                std::cout << "[!] Iteration: " << iteration + 1 << std::endl;
                auto piBegin = pi.begin();
                auto lambdaBegin = lambda.begin();
                ParallelClustering<S2, S4, S5>::template cluster<C>(data,
                                                                    rows,
                                                                    dimension,
                                                                    piBegin,
                                                                    lambdaBegin,
                                                                    threadCount,
                                                                    threadCount,
                                                                    threadCount);
            }
            // Print a text informing of what stage the timer is related to
            std::cout << std::endl << "[!] Mean" << std::endl;
            std::cout << "Init   : ";
            Timer::print<0>(3);
            std::cout << "Stage 1: ";
            Timer::print<1>(3);
            std::cout << "Stage 2: ";
            Timer::print<2>(3);
            std::cout << "Stage 3: ";
            Timer::print<3>(3);
            std::cout << "Stage 4: ";
            Timer::print<4>(3);
            std::cout << "Stage 5: ";
            Timer::print<5>(3);
            std::cout << "Total  : ";
            Timer::printTotal<0, 1, 2, 3, 4, 5>(3);
            std::cout << std::endl;

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

template <typename D>
void iterateSequentialClustering(const std::string &title,
                                 const D &data,
                                 std::size_t rows,
                                 std::size_t dimension,
                                 const std::vector<std::size_t> &expectedPi,
                                 const std::vector<double> &expectedLambda) {

    std::vector<std::size_t> pi{};
    pi.resize(rows);
    std::vector<double> lambda{};
    lambda.resize(rows);

    std::cout << "--------------" << std::endl
              << title << std::endl
              << "--------------" << std::endl
              << std::endl;

    Timer::reset();
    for (std::size_t iteration = 0; iteration < 3; iteration++) {
        std::cout << "[!] Iteration: " << iteration + 1 << std::endl;
        auto piBegin = pi.begin();
        auto lambdaBegin = lambda.begin();
        SequentialClustering::cluster(data, rows, dimension, piBegin, lambdaBegin);
    }
    // Print a text informing of what stage the timer is related to
    std::cout << std::endl << "[!] Mean" << std::endl;
    std::cout << "Init   : ";
    Timer::print<0>(3);
    std::cout << "Stage 1: ";
    Timer::print<1>(3);
    std::cout << "Stage 2: ";
    Timer::print<2>(3);
    std::cout << "Stage 3: ";
    Timer::print<3>(3);
    std::cout << "Stage 4: ";
    Timer::print<4>(3);
    std::cout << "Stage 5: ";
    Timer::print<5>(3);
    std::cout << "Total  : ";
    Timer::printTotal<0, 1, 2, 3, 4, 5>(3);
    std::cout << std::endl;

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
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-30
 * @since version date
 */
int main() {

    Timer::initTimers();

    /**
     * iris
     * birm
     * gen many attributes
     * acc
     * gen many attributes
     */
    std::vector<std::tuple<std::string, std::size_t, std::size_t>> datasets{
            {"accelerometer.csv", 3, 5}, {"generated.data", 1, 45}};
    std::vector<std::size_t> threads{2, 4, 8, 12, 16};

    for (auto &dataset : datasets) {

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

        double *sseMMAlignedData = nullptr;
        double *avxMMAlignedData = nullptr;
        double *uniqueVectorData = nullptr;
        std::vector<double *> twoLevels{};
        std::vector<double *> sseTwoLevels{};
        std::vector<double *> avxTwoLevels{};
        auto *newData = new std::array<AlignedArray<double, 45, 32>, 153'000>;
        auto *newDataIterators = new std::array<AlignedArray<double, 45, 32>::const_iterator, 153'000>;

        auto *dataIterator = data.data();
        std::size_t dataElementsCount = data.size() / dimension;

        // Fill two levels
        for (std::size_t i = 0; i < data.size(); i += dimension) {
            auto *point = new double[dimension];
            // Copy the doubles
            memcpy(point, &(dataIterator[i]), dimension * sizeof(double));
            twoLevels.push_back(point);
        }

        std::size_t sseStride = ContiguousDoubleMemoryDataIterator::computeSseStride(dimension);

        for (std::size_t i = 0; i < data.size(); i += dimension) {
            auto *reallocatedPoint = static_cast<double *>(
                    _mm_malloc(sseStride * sizeof(double), 2 * sizeof(double)));
            // Copy the doubles
            memcpy(reallocatedPoint, &(dataIterator[i]), dimension * sizeof(double));
            // Fill the remaining coordinates with 0
            memset(&(reallocatedPoint[dimension]), 0, (sseStride - dimension) * sizeof(double));

            // Add the point
            sseTwoLevels.push_back(reallocatedPoint);
        }

        std::size_t avxStride = ContiguousDoubleMemoryDataIterator::computeAvxStride(dimension);

        for (std::size_t i = 0; i < data.size(); i += dimension) {
            auto *reallocatedPoint = static_cast<double *>(
                    _mm_malloc(avxStride * sizeof(double), 4 * sizeof(double)));
            // Copy the doubles
            memcpy(reallocatedPoint, &(dataIterator[i]), dimension * sizeof(double));
            // Fill the remaining coordinates with 0
            memset(&(reallocatedPoint[dimension]), 0, (avxStride - dimension) * sizeof(double));

            // Add the point
            avxTwoLevels.push_back(reallocatedPoint);
        }

        std::size_t size = sizeof(double) * (dataElementsCount * sseStride);
        sseMMAlignedData = static_cast<double *>(_mm_malloc(size, 2 * sizeof(double)));
        memset(sseMMAlignedData, 0, size);

        std::size_t start = 0;
        for (std::size_t i = 0; i < data.size(); i += dimension) {
            memcpy(&(sseMMAlignedData[start]), &(dataIterator[i]), dimension * sizeof(double));
            start += sseStride;
        }

        size = sizeof(double) * (dataElementsCount * avxStride);
        avxMMAlignedData = static_cast<double *>(_mm_malloc(size, 4 * sizeof(double)));
        memset(avxMMAlignedData, 0, size);

        start = 0;
        for (std::size_t i = 0; i < data.size(); i += dimension) {
            memcpy(&(avxMMAlignedData[start]), &(dataIterator[i]), dimension * sizeof(double));
            start += avxStride;
        }

        uniqueVectorData = new double[data.size()];
        for (std::size_t i = 0; i < data.size(); i += dimension) {
            memcpy(&(uniqueVectorData[i]), &(dataIterator[i]), dimension * sizeof(double));
        }

        std::size_t index = 0;
        for (std::size_t i = 0; i < data.size(); i += dimension) {
            for (std::size_t d = 0; d < dimension; d++) {
                (*newData)[index][d] = dataIterator[i + d];
            }
            (*newDataIterators)[index] = (*newData)[index].cbegin();
            index++;
        }

        std::vector<std::size_t> pi{};
        pi.resize(twoLevels.size());
        std::vector<double> lambda{};
        lambda.resize(twoLevels.size());

        std::cout << "Executing the sequential implementation to check the results" << std::endl;
        auto dataBegin = twoLevels.begin();
        auto piBegin = pi.begin();
        auto lambdaBegin = lambda.begin();

        SequentialClustering::cluster(
                dataBegin, dataElementsCount, dimension, piBegin, lambdaBegin);

        iterateSequentialClustering(
                "Sequential 1", twoLevels.begin(), twoLevels.size(), dimension, pi, lambda);

        iterateSequentialClustering("Sequential 2: Linearized",
                                    uniqueVectorData,
                                    twoLevels.size(),
                                    dimension,
                                    pi,
                                    lambda);

        ClusteringAlgorithmExecutor<>::iterateParallelClustering<DistanceComputers::CLASSICAL>(
                "Parallel 1: Multi-threaded Distance Computation",
                twoLevels.begin(),
                twoLevels.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<false>::iterateParallelClustering<DistanceComputers::SSE>(
                "Parallel 2: SSE",
                sseTwoLevels.begin(),
                sseTwoLevels.size(),
                dimension,
                {0},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<false>::iterateParallelClustering<DistanceComputers::AVX>(
                "Parallel 3: AVX",
                avxTwoLevels.begin(),
                avxTwoLevels.size(),
                dimension,
                {0},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<>::iterateParallelClustering<DistanceComputers::SSE>(
                "Parallel 4: Multi-threaded Distance Computation + SSE",
                sseTwoLevels.begin(),
                sseTwoLevels.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<>::iterateParallelClustering<DistanceComputers::AVX>(
                "Parallel 5: Multi-threaded Distance Computation + AVX",
                avxTwoLevels.begin(),
                avxTwoLevels.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<>::iterateParallelClustering<DistanceComputers::SSE>(
                "Parallel 6: Multi-threaded Distance Computation + SSE + Linearized",
                sseMMAlignedData,
                sseTwoLevels.size(),
                dimension,
                {4, 8, 12},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<>::iterateParallelClustering<DistanceComputers::AVX>(
                "Parallel 7: Multi-threaded Distance Computation + AVX + Linearized",
                avxMMAlignedData,
                avxTwoLevels.size(),
                dimension,
                {4, 8, 12},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<true, true>::iterateParallelClustering<DistanceComputers::AVX>(
                "Parallel 8: Multi-threaded Distance Computation and Stage 4 + AVX + Linearized",
                avxMMAlignedData,
                avxTwoLevels.size(),
                dimension,
                {4, 8, 12},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<true, true>::iterateParallelClustering<
                DistanceComputers::AVX_OPTIMIZED>(
                "Parallel 9: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + "
                "Linearized",
                avxMMAlignedData,
                avxTwoLevels.size(),
                dimension,
                {4, 8, 12},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<true, true, true>::iterateParallelClustering<
                DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT>(
                "Parallel 10: Multi-threaded Distance Computation and Stage 4 + SSE Optimized + "
                "Linearized + No Square Root",
                sseMMAlignedData,
                sseTwoLevels.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<true, true, true>::iterateParallelClustering<
                DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
                "Parallel 11: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + "
                "Linearized + No Square Root",
                avxMMAlignedData,
                avxTwoLevels.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        ClusteringAlgorithmExecutor<true, true, true>::iterateParallelClustering<
                DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
                "Parallel 12: Multi-threaded Distance Computation and Stage 4 + AVX Optimized + "
                "Linearized + No Square Root + std::array<AlignedArray::const_iterator>",
                *newDataIterators,
                avxTwoLevels.size(),
                dimension,
                {2, 4, 8, 12, 16},
                pi,
                lambda);

        delete newData;
    }

    return 0;
}
