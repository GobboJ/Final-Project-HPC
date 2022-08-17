/*
 * ClusteringAlgorithmExecutor implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-08-16
 * @since 1.0
 */
#include "ClusteringAlgorithmExecutor.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"

using cluster::parallel::DistanceComputers;
using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;

namespace cluster::test::main {

/**
 * Creates a new executor of the clustering algorithm.
 *
 * @param dataElementsCount Number of data samples to cluster.
 * @param dimension Dimension of the data samples to cluster.
 * @param distanceComputationThreadsCount Number of threads to use to compute the distance
 * between two points in parallel.
 * @param structuralFixThreadsCount Number of threads to use to fix in parallel the structure of
 * the dendrogram after a new point is added.
 * @param sqrtComputationThreadsCount Number of threads to use to compute the square roots of
 * the distances stored in <code>lambda</code>.
 */
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

/**
 * Executes the requested version of the clustering algorithm
 *
 * @param isParallel <code>true</code> if the parallel implementation of the clustering
 * algorithm should be executed, <code>false</code> otherwise.
 * @param version Version of the algorithm to execute.
 * @param indirectData Data samples to cluster, stored as indirect pointers.
 * @param sseAlignedIndirectData Data samples to cluster, stored as indirect pointers and
 * aligned for SSE.
 * @param avxAlignedIndirectData Data samples to cluster, stored as indirect pointers and
 * aligned for AVX.
 * @param sseAlignedData Data samples to cluster, aligned for SSE.
 * @param avxAlignedData Data samples to cluster, aligned for AVX.
 * @param uniqueArrayData Data samples to cluster, stored as a unique array.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeClusteringAlgorithm(
        const bool isParallel,
        const std::size_t version,
        const std::vector<double *> &indirectData,
        const std::vector<double *> &sseAlignedIndirectData,
        const std::vector<double *> &avxAlignedIndirectData,
        const double *sseAlignedData,
        const double *avxAlignedData,
        const double *uniqueArrayData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) {

    // Choose the right version to execute
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    if (isParallel) {
        switch (version) {
            case 1:
                this->executeParallelV1(indirectData, piIterator, lambdaIterator);
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
                this->executeSequentialV1(indirectData, piIterator, lambdaIterator);
                break;
            case 2:
                this->executeSequentialV2(uniqueArrayData, piIterator, lambdaIterator);
                break;
            default:
                std::cerr << "Unknown sequential version" << ' ' << version << std::endl;
                exit(1);
        }
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
}

/**
 * Executes the first version of the parallel clustering algorithm.
 *
 * @param data Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV1(
        const std::vector<double *> &data,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<>::cluster<DistanceComputers::CLASSICAL>(
            data,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

/**
 * Executes the second version of the parallel clustering algorithm.
 *
 * @param sseAlignedIndirectData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV2(
        const std::vector<double *> &sseAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<>::cluster<DistanceComputers::SSE>(sseAlignedIndirectData,
                                                          this->dataElementsCount,
                                                          this->dimension,
                                                          piIterator,
                                                          lambdaIterator,
                                                          this->distanceComputationThreadsCount,
                                                          this->structuralFixThreadsCount);
}

/**
 * Executes the third version of the parallel clustering algorithm.
 *
 * @param avxAlignedIndirectData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV3(
        const std::vector<double *> &avxAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<>::cluster<DistanceComputers::AVX>(avxAlignedIndirectData,
                                                          this->dataElementsCount,
                                                          this->dimension,
                                                          piIterator,
                                                          lambdaIterator,
                                                          this->distanceComputationThreadsCount,
                                                          this->structuralFixThreadsCount);
}

/**
 * Executes the fourth version of the parallel clustering algorithm.
 *
 * @param avxAlignedData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV4(
        const double *avxAlignedData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<>::cluster<DistanceComputers::AVX>(avxAlignedData,
                                                          this->dataElementsCount,
                                                          this->dimension,
                                                          piIterator,
                                                          lambdaIterator,
                                                          this->distanceComputationThreadsCount,
                                                          this->structuralFixThreadsCount);
}

/**
 * Executes the fifth version of the parallel clustering algorithm.
 *
 * @param sseAlignedIndirectData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV5(
        const std::vector<double *> &sseAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<true, true>::cluster<DistanceComputers::SSE>(
            sseAlignedIndirectData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

/**
 * Executes the sixth version of the parallel clustering algorithm.
 *
 * @param sseAlignedIndirectData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV6(
        const std::vector<double *> &sseAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<true, true>::cluster<DistanceComputers::SSE_OPTIMIZED>(
            sseAlignedIndirectData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

/**
 * Executes the seventh version of the parallel clustering algorithm.
 *
 * @param avxAlignedIndirectData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV7(
        const std::vector<double *> &avxAlignedIndirectData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<true, true>::cluster<DistanceComputers::AVX_OPTIMIZED>(
            avxAlignedIndirectData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

/**
 * Executes the eighth version of the parallel clustering algorithm.
 *
 * @param sseAlignedData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV8(
        const double *sseAlignedData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<true, true>::cluster<DistanceComputers::SSE_OPTIMIZED>(
            sseAlignedData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

/**
 * Executes the ninth version of the parallel clustering algorithm.
 *
 * @param avxAlignedData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV9(
        const double *avxAlignedData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<true, true>::cluster<DistanceComputers::AVX_OPTIMIZED>(
            avxAlignedData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

/**
 * Executes the tenth version of the parallel clustering algorithm.
 *
 * @param avxAlignedData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV10(
        const double *avxAlignedData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    ParallelClustering<true, true>::cluster<DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT>(
            avxAlignedData,
            this->dataElementsCount,
            this->dimension,
            piIterator,
            lambdaIterator,
            this->distanceComputationThreadsCount,
            this->structuralFixThreadsCount);
}

/**
 * Executes the eleventh version of the parallel clustering algorithm.
 *
 * @param avxAlignedData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeParallelV11(
        const double *avxAlignedData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

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

/**
 * Executes the first version of the sequential clustering algorithm.
 *
 * @param data Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeSequentialV1(
        const std::vector<double *> &data,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    SequentialClustering::cluster(
            data.begin(), this->dataElementsCount, this->dimension, piIterator, lambdaIterator);
}

/**
 * Executes the second version of the sequential clustering algorithm.
 *
 * @param uniqueArrayData Data to cluster.
 * @param piIterator Iterator over the data structure holding <code>pi</code>.
 * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
 */
void ClusteringAlgorithmExecutor::executeSequentialV2(
        const double *uniqueArrayData,
        std::vector<std::size_t>::iterator piIterator,
        std::vector<double>::iterator lambdaIterator) const {

    SequentialClustering::cluster(
            uniqueArrayData, this->dataElementsCount, this->dimension, piIterator, lambdaIterator);
}
}  // namespace cluster::test::main
