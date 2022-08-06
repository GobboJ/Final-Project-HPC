#include "ContiguousDoubleMemoryDataIterator.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"
#include "data/DataReader.h"
#include <cstddef>
#include <iostream>
#include <mm_malloc.h>
#include <vector>
#include <cstring>

using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;
using cluster::utils::Timer;
using DistanceComputers = cluster::parallel::ParallelClustering::DistanceComputers;
using cluster::test::data::DataReader;

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-30
 * @since version date
 */
int main() {

    /**
     * iris
     * birm
     * gen many attributes
     * acc
     * gen many attributes
     */
    std::vector<std::tuple<std::string, int, int>> datasets{
                                                            {"Parking Birmingham.data", 2, 3},
                                                            {"generated.data", 1, 32}};
    std::vector<std::size_t> threads{2, 4, 8, 12, 16};

    for (auto &dataset : datasets) {
        
        for (std::size_t iteration = 0; iteration < 3; iteration++) {
            
            std::cout << "------ Iteration: " << iteration << " ------" << std::endl;
            
            for (std::size_t threadCount : threads) {
                // Read the data
                std::vector<double> data{};
                std::size_t dimension = DataReader::readAndParseData(
                        std::get<0>(dataset), data, 0, 0, std::get<1>(dataset), std::get<2>(dataset));
                
                double *sseMMAlignedData = nullptr;
                double *avxMMAlignedData = nullptr;
                double *uniqueVectorData = nullptr;
                std::vector<double *> twoLevels{};
                std::vector<double *> sseTwoLevels{};
                std::vector<double *> avxTwoLevels{};

                auto *dataIterator = data.data();
                std::size_t dataElementsCount = data.size() / dimension;

                // Fill two levels
                for (std::size_t i = 0; i < data.size(); i += dimension) {
                    auto *point = new double[dimension];
                    // Copy the doubles
                    memcpy(point, &(dataIterator[i]), dimension * sizeof(double));
                    twoLevels.push_back(point);
                }

                std::size_t quotient = 1 + ((dimension - 1) / 2);
                std::size_t newDimension = 2 * quotient;
                std::size_t sseStride = newDimension;

                for (std::size_t i = 0; i < data.size(); i += dimension) {
                    auto *reallocatedPoint = static_cast<double *>(
                            _mm_malloc(newDimension * sizeof(double), 2 * sizeof(double)));
                    // Copy the doubles
                    memcpy(reallocatedPoint, &(dataIterator[i]), dimension * sizeof(double));
                    // Fill the remaining coordinates with 0
                    memset(&(reallocatedPoint[dimension]),
                           0,
                           (newDimension - dimension) * sizeof(double));

                    // Add the point
                    sseTwoLevels.push_back(reallocatedPoint);
                }

                quotient = 1 + ((dimension - 1) / 4);
                newDimension = 4 * quotient;
                std::size_t avxStride = newDimension;

                for (std::size_t i = 0; i < data.size(); i += dimension) {
                    auto *reallocatedPoint = static_cast<double *>(
                            _mm_malloc(newDimension * sizeof(double), 4 * sizeof(double)));
                    // Copy the doubles
                    memcpy(reallocatedPoint, &(dataIterator[i]), dimension * sizeof(double));
                    // Fill the remaining coordinates with 0
                    memset(&(reallocatedPoint[dimension]),
                           0,
                           (newDimension - dimension) * sizeof(double));

                    // Add the point
                    avxTwoLevels.push_back(reallocatedPoint);
                }

                std::size_t size = sizeof(double) * (data.size() / dimension) * newDimension;
                sseMMAlignedData = static_cast<double *>(_mm_malloc(size, 2 * sizeof(double)));
                memset(sseMMAlignedData, 0, size);

                std::size_t start = 0;
                for (std::size_t i = 0; i < data.size(); i += dimension) {
                    memcpy(&(sseMMAlignedData[start]), &(dataIterator[i]), dimension * sizeof(double));
                    start += newDimension;
                }

                size = sizeof(double) * (data.size() / dimension) * newDimension;
                avxMMAlignedData = static_cast<double *>(_mm_malloc(size, 4 * sizeof(double)));
                memset(avxMMAlignedData, 0, size);

                start = 0;
                for (std::size_t i = 0; i < data.size(); i += dimension) {
                    memcpy(&(avxMMAlignedData[start]), &(dataIterator[i]), dimension * sizeof(double));
                    start += newDimension;
                }

                uniqueVectorData = new double[data.size()];
                for (std::size_t i = 0; i < data.size(); i += dimension) {
                    memcpy(&(uniqueVectorData[i]), &(dataIterator[i]), dimension * sizeof(double));
                }

                std::vector<std::size_t> pi{};
                pi.resize(twoLevels.size());
                std::vector<double> lambda{};
                lambda.resize(twoLevels.size());

                // Seq 1: classic sequential
                std::cout << std::endl << "Seq 1: classic sequential" << std::endl << std::endl;
                SequentialClustering::cluster(
                        twoLevels.begin(), twoLevels.size(), dimension, pi.begin(), lambda.begin());
                
                Timer::reset();

                // Seq 2: classic sequential + Linearized
                std::cout << std::endl << "Seq 2: classic sequential + Linearized" << std::endl << std::endl;
                SequentialClustering::cluster(
                        ContiguousDoubleMemoryDataIterator(uniqueVectorData, dimension),
                        twoLevels.size(),
                        dimension,
                        pi.begin(),
                        lambda.begin());
                
                Timer::reset();

                // Par 1: multithreaded distance
                std::cout << std::endl << "Par 1: multithreaded distance" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::CLASSICAL>(twoLevels.begin(),
                                                                          twoLevels.size(),
                                                                          dimension,
                                                                          pi.begin(),
                                                                          lambda.begin(),
                                                                          threadCount,
                                                                          threadCount,
                                                                          threadCount);
                
                Timer::reset();
                
                // Par 2: sse only
                std::cout << std::endl << "Par 2: sse only" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::SSE,
                                            std::vector<double *>::iterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            false>(sseTwoLevels.begin(),
                                                   sseTwoLevels.size(),
                                                   dimension,
                                                   pi.begin(),
                                                   lambda.begin(),
                                                   threadCount,
                                                   threadCount,
                                                   threadCount);
                
                Timer::reset();
                
                // Par 3: avx only
                std::cout << std::endl << "Par 3: avx only" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::AVX,
                                            std::vector<double *>::iterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            false>(avxTwoLevels.begin(),
                                                   avxTwoLevels.size(),
                                                   dimension,
                                                   pi.begin(),
                                                   lambda.begin(),
                                                   threadCount,
                                                   threadCount,
                                                   threadCount);
                
                Timer::reset();
            
                // Par 4: multithreaded distance + SSE
                std::cout << std::endl << "Par 4: multithreaded distance + SSE" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::SSE,
                                            std::vector<double *>::iterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator>(sseTwoLevels.begin(),
                                                                           sseTwoLevels.size(),
                                                                           dimension,
                                                                           pi.begin(),
                                                                           lambda.begin(),
                                                                           threadCount,
                                                                           threadCount,
                                                                           threadCount);
                
                Timer::reset();
                
                // Par 5: multithreaded distance + AVX
                std::cout << std::endl << "Par 5: multithreaded distance + AVX" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::AVX,
                                            std::vector<double *>::iterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator>(avxTwoLevels.begin(),
                                                                           avxTwoLevels.size(),
                                                                           dimension,
                                                                           pi.begin(),
                                                                           lambda.begin(),
                                                                           threadCount,
                                                                           threadCount,
                                                                           threadCount);
                
                Timer::reset();
            
                // Par 6: multithreaded distance + SSE + linearized
                std::cout << std::endl << "Par 6: multithreaded distance + SSE + linearized" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::SSE,
                                            ContiguousDoubleMemoryDataIterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator>(
                        ContiguousDoubleMemoryDataIterator(sseMMAlignedData, sseStride),
                        sseTwoLevels.size(),
                        dimension,
                        pi.begin(),
                        lambda.begin(),
                        threadCount,
                        threadCount,
                        threadCount);
                
                Timer::reset();

                // Par 7: multithreaded distance + AVX + linearized
                std::cout << std::endl << "Par 7: multithreaded distance + AVX + linearized" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::AVX,
                                            ContiguousDoubleMemoryDataIterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator>(
                        ContiguousDoubleMemoryDataIterator(avxMMAlignedData, avxStride),
                        avxTwoLevels.size(),
                        dimension,
                        pi.begin(),
                        lambda.begin(),
                        threadCount,
                        threadCount,
                        threadCount);
                
                Timer::reset();

                // Par 8: multithreaded distance & stage 4 + SSE
                std::cout << std::endl << "Par 8: multithreaded distance & stage 4 + SSE" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::SSE,
                                            std::vector<double *>::iterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            true,
                                            true>(sseTwoLevels.begin(),
                                                  sseTwoLevels.size(),
                                                  dimension,
                                                  pi.begin(),
                                                  lambda.begin(),
                                                  threadCount,
                                                  threadCount,
                                                  threadCount);
                
                Timer::reset();
            
                // Par 9: multithreaded distance & stage 4 + AVX
                std::cout << std::endl << "Par 9: multithreaded distance & stage 4 + AVX" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::AVX,
                                            std::vector<double *>::iterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            true,
                                            true>(avxTwoLevels.begin(),
                                                  avxTwoLevels.size(),
                                                  dimension,
                                                  pi.begin(),
                                                  lambda.begin(),
                                                  threadCount,
                                                  threadCount,
                                                  threadCount);
                
                Timer::reset();

                // Par 10: multithreaded distance & stage 4 + SSE Optimized
                std::cout << std::endl << "Par 10: multithreaded distance & stage 4 + SSE Optimized" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::SSE_OPTIMIZED,
                                            std::vector<double *>::iterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            true,
                                            true>(sseTwoLevels.begin(),
                                                  sseTwoLevels.size(),
                                                  dimension,
                                                  pi.begin(),
                                                  lambda.begin(),
                                                  threadCount,
                                                  threadCount,
                                                  threadCount);
                
                Timer::reset();

                // Par 11: multithreaded distance & stage 4 + AVX Optimized
                std::cout << std::endl << "Par 11: multithreaded distance & stage 4 + AVX Optimized" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::AVX_OPTIMIZED,
                                            std::vector<double *>::iterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            true,
                                            true>(avxTwoLevels.begin(),
                                                  avxTwoLevels.size(),
                                                  dimension,
                                                  pi.begin(),
                                                  lambda.begin(),
                                                  threadCount,
                                                  threadCount,
                                                  threadCount);
                
                Timer::reset();

                // Par 12: multithreaded distance & stage 4 + SSE Optimized + Linearized
                std::cout << std::endl << "Par 12: multithreaded distance & stage 4 + SSE Optimized + Linearized" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::SSE_OPTIMIZED,
                                            ContiguousDoubleMemoryDataIterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            true,
                                            true>(
                        ContiguousDoubleMemoryDataIterator(sseMMAlignedData, sseStride),
                        sseTwoLevels.size(),
                        dimension,
                        pi.begin(),
                        lambda.begin(),
                        threadCount,
                        threadCount,
                        threadCount);
                
                Timer::reset();

                // Par 13: multithreaded distance & stage 4 + AVX Optimized + Linearized
                std::cout << std::endl << "Par 13: multithreaded distance & stage 4 + AVX Optimized + Linearized" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::AVX_OPTIMIZED,
                                            ContiguousDoubleMemoryDataIterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            true,
                                            true>(
                        ContiguousDoubleMemoryDataIterator(avxMMAlignedData, avxStride),
                        avxTwoLevels.size(),
                        dimension,
                        pi.begin(),
                        lambda.begin(),
                        threadCount,
                        threadCount,
                        threadCount);
                
                Timer::reset();

                // Par 14: multithreaded distance & stage 4 + SSE Optimized + Linearized + No Square
                // root
                std::cout << std::endl << "Par 14: multithreaded distance & stage 4 + SSE Optimized + Linearized + No Square" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT,
                                            ContiguousDoubleMemoryDataIterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            true,
                                            true,
                                            true>(
                        ContiguousDoubleMemoryDataIterator(sseMMAlignedData, sseStride),
                        sseTwoLevels.size(),
                        dimension,
                        pi.begin(),
                        lambda.begin(),
                        threadCount,
                        threadCount,
                        threadCount);
                
                Timer::reset();

                // Par 15: multithreaded distance & stage 4 + AVX Optimized + Linearized + No Square
                // root
                std::cout << std::endl << "Par 15: multithreaded distance & stage 4 + AVX Optimized + Linearized + No Square" << std::endl << std::endl;
                ParallelClustering::cluster<DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT,
                                            ContiguousDoubleMemoryDataIterator,
                                            std::vector<std::size_t>::iterator,
                                            std::vector<double>::iterator,
                                            true,
                                            true,
                                            true>(
                        ContiguousDoubleMemoryDataIterator(avxMMAlignedData, avxStride),
                        avxTwoLevels.size(),
                        dimension,
                        pi.begin(),
                        lambda.begin(),
                        threadCount,
                        threadCount,
                        threadCount);
                
                Timer::reset();
            }
        }

        
    }

    return 0;
}
