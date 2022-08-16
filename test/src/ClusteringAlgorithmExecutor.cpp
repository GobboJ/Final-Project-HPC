/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-16
 * @since version date
 */
#include "ClusteringAlgorithmExecutor.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"

using cluster::parallel::DistanceComputers;
using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;

namespace cluster::test::main {

ClusteringAlgorithmExecutor::ClusteringAlgorithmExecutor(
        const std::size_t dataElementsCount,
        const std::size_t dimension,
        const std::size_t distanceComputationThreadsCount,
        const std::size_t structuralFixThreadsCount,
        const std::size_t sqrtComputationThreadsCount) :
    dataElementsCount(dataElementsCount),
    dimension(dimension),
    distanceComputationThreadsCount(distanceComputationThreadsCount),
    structuralFixThreadsCount(structuralFixThreadsCount),
    sqrtComputationThreadsCount(sqrtComputationThreadsCount) {
}

void ClusteringAlgorithmExecutor::executeClusteringAlgorithm(
        bool isParallel,
        std::size_t version,
        const std::vector<double *> &data,
        const std::vector<double *> &sseAlignedIndirectData,
        const std::vector<double *> &avxAlignedIndirectData,
        const double *sseAlignedData,
        const double *avxAlignedData,
        const double *uniqueArrayData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) {

    if (isParallel) {
        switch (version) {
            case 1:
                this->executeParallelV1(data, piIterator, lambdaIterator);
                break;
            case 2:
                this->executeParallelV2(sseAlignedIndirectData, piIterator, lambdaIterator);
                break;
            case 3:
                this->executeParallelV3(avxAlignedIndirectData, piIterator, lambdaIterator);
                break;
            case 4:
                this->executeParallelV4(avxAlignedData, piIterator, lambdaIterator);
                break;
            case 5:
                this->executeParallelV5(sseAlignedIndirectData, piIterator, lambdaIterator);
                break;
            case 6:
                this->executeParallelV6(sseAlignedIndirectData, piIterator, lambdaIterator);
                break;
            case 7:
                this->executeParallelV7(avxAlignedIndirectData, piIterator, lambdaIterator);
                break;
            case 8:
                this->executeParallelV8(sseAlignedData, piIterator, lambdaIterator);
                break;
            case 9:
                this->executeParallelV9(avxAlignedData, piIterator, lambdaIterator);
                break;
            case 10:
                this->executeParallelV10(avxAlignedData, piIterator, lambdaIterator);
                break;
            case 11:
                this->executeParallelV11(avxAlignedData, piIterator, lambdaIterator);
                break;
            default:
                std::cerr << "Unknown parallel version" << ' ' << version << std::endl;
                exit(1);
        }
    } else {
        switch (version) {
            case 1:
                this->executeSequentialV1(data, piIterator, lambdaIterator);
                break;
            case 2:
                this->executeSequentialV2(uniqueArrayData, piIterator, lambdaIterator);
                break;
            default:
                std::cerr << "Unknown sequential version" << ' ' << version << std::endl;
                exit(1);
        }
    }
}

void ClusteringAlgorithmExecutor::executeParallelV1(const std::vector<double *> &data,
                                                    std::vector<std::size_t>::iterator piIterator,
                                                    std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<>::cluster<DistanceComputers::CLASSICAL>(
            data,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV2(
        const std::vector<double *> &sseAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<>::cluster<DistanceComputers::SSE>(sseAlignedIndirectData,
                                                          this->dataElementsCount,
                                                          this->dimension,
                                                          piIterator,
                                                          lambdaIterator,
                                                          this->distanceComputationThreadsCount,
                                                          this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV3(
        const std::vector<double *> &avxAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<>::cluster<DistanceComputers::AVX>(avxAlignedIndirectData,
                                                          this->dataElementsCount,
                                                          this->dimension,
                                                          piIterator,
                                                          lambdaIterator,
                                                          this->distanceComputationThreadsCount,
                                                          this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV4(const double *avxAlignedData,
                                                    std::vector<std::size_t>::iterator piIterator,
                                                    std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<>::cluster<DistanceComputers::AVX>(avxAlignedData,
                                                          this->dataElementsCount,
                                                          this->dimension,
                                                          piIterator,
                                                          lambdaIterator,
                                                          this->distanceComputationThreadsCount,
                                                          this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV5(
        const std::vector<double *> &sseAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<true, true>::cluster<DistanceComputers::SSE>(
            sseAlignedIndirectData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV6(
        const std::vector<double *> &sseAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<true, true>::cluster<DistanceComputers::SSE_OPTIMIZED>(
            sseAlignedIndirectData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV7(
        const std::vector<double *> &avxAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<true, true>::cluster<DistanceComputers::AVX_OPTIMIZED>(
            avxAlignedIndirectData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV8(const double *sseAlignedData,
                                                    std::vector<std::size_t>::iterator piIterator,
                                                    std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<true, true>::cluster<DistanceComputers::SSE_OPTIMIZED>(
            sseAlignedData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV9(const double *avxAlignedData,
                                                    std::vector<std::size_t>::iterator piIterator,
                                                    std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<true, true>::cluster<DistanceComputers::AVX_OPTIMIZED>(
            avxAlignedData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV10(const double *avxAlignedData,
                                                     std::vector<std::size_t>::iterator piIterator,
                                                     std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<true, true>::cluster<DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
            avxAlignedData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

void ClusteringAlgorithmExecutor::executeParallelV11(const double *avxAlignedData,
                                                     std::vector<std::size_t>::iterator piIterator,
                                                     std::vector<double>::iterator lambdaIterator) {

    ParallelClustering<true, true, true>::cluster<DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
            avxAlignedData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount,
            this->sqrtComputationThreadsCount);
}

void ClusteringAlgorithmExecutor::executeSequentialV1(
        const std::vector<double *> &data,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) {

    SequentialClustering::cluster(
            data.begin(), this->dataElementsCount, this->dimension, piIterator, lambdaIterator);
}

void ClusteringAlgorithmExecutor::executeSequentialV2(
        const double *uniqueArrayData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) {

    SequentialClustering::cluster(
            uniqueArrayData, this->dataElementsCount, this->dimension, piIterator, lambdaIterator);
}
}  // namespace cluster::test::main
