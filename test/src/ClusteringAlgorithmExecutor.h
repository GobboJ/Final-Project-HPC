#ifndef FINAL_PROJECT_HPC_CLUSTERINGALGORITHMEXECUTOR_H
#define FINAL_PROJECT_HPC_CLUSTERINGALGORITHMEXECUTOR_H

#include <cstddef>
#include <vector>

namespace cluster::test::main {

/**
 * Utility class allowing to execute a specific version of the clustering algorithm.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-08-16
 * @since 1.0
 */
class ClusteringAlgorithmExecutor {

public:
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
    ClusteringAlgorithmExecutor(std::size_t dataElementsCount,
                                std::size_t dimension,
                                std::size_t distanceComputationThreadsCount,
                                std::size_t structuralFixThreadsCount,
                                std::size_t sqrtComputationThreadsCount);

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
     * @param uniqueArrayData ata samples to cluster, stored as a unique array.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeClusteringAlgorithm(bool isParallel,
                                    std::size_t version,
                                    const std::vector<double *> &indirectData,
                                    const std::vector<double *> &sseAlignedIndirectData,
                                    const std::vector<double *> &avxAlignedIndirectData,
                                    const double *sseAlignedData,
                                    const double *avxAlignedData,
                                    const double *uniqueArrayData,
                                    std::vector<std::size_t>::iterator piIterator,
                                    std::vector<double>::iterator lambdaIterator);

private:
    /**
     * Executes the first version of the parallel clustering algorithm.
     *
     * @param data Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV1(const std::vector<double *> &data,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the second version of the parallel clustering algorithm.
     *
     * @param sseAlignedIndirectData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV2(const std::vector<double *> &sseAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the third version of the parallel clustering algorithm.
     *
     * @param avxAlignedIndirectData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV3(const std::vector<double *> &avxAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the fourth version of the parallel clustering algorithm.
     *
     * @param avxAlignedData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV4(const double *avxAlignedData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the fifth version of the parallel clustering algorithm.
     *
     * @param sseAlignedIndirectData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV5(const std::vector<double *> &sseAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the sixth version of the parallel clustering algorithm.
     *
     * @param sseAlignedIndirectData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV6(const std::vector<double *> &sseAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the seventh version of the parallel clustering algorithm.
     *
     * @param avxAlignedIndirectData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV7(const std::vector<double *> &avxAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the eighth version of the parallel clustering algorithm.
     *
     * @param sseAlignedData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV8(const double *sseAlignedData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the ninth version of the parallel clustering algorithm.
     *
     * @param avxAlignedData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV9(const double *avxAlignedData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the tenth version of the parallel clustering algorithm.
     *
     * @param avxAlignedData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV10(const double *avxAlignedData,
                            std::vector<std::size_t>::iterator piIterator,
                            std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the eleventh version of the parallel clustering algorithm.
     *
     * @param avxAlignedData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeParallelV11(const double *avxAlignedData,
                            std::vector<std::size_t>::iterator piIterator,
                            std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the first version of the sequential clustering algorithm.
     *
     * @param data Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeSequentialV1(const std::vector<double *> &data,
                             std::vector<std::size_t>::iterator piIterator,
                             std::vector<double>::iterator lambdaIterator) const;

    /**
     * Executes the second version of the sequential clustering algorithm.
     *
     * @param uniqueArrayData Data to cluster.
     * @param piIterator Iterator over the data structure holding <code>pi</code>.
     * @param lambdaIterator Iterator over the data structure holding <code>lambda</code>.
     */
    void executeSequentialV2(const double *uniqueArrayData,
                             std::vector<std::size_t>::iterator piIterator,
                             std::vector<double>::iterator lambdaIterator) const;

    /**
     * Number of data samples to cluster.
     */
    const std::size_t dataElementsCount;

    /**
     * Dimension of each data sample.
     */
    const std::size_t dimension;

    /**
     * Number of threads to use to compute the distance between two points in parallel.
     */
    const std::size_t distanceComputationThreadsCount;

    /**
     * Number of threads to use to fix in parallel the structure of the dendrogram after a new point
     * is added.
     */
    const std::size_t structuralFixThreadsCount;

    /**
     * Number of threads to use to compute the square roots of the distances stored in
     * <code>lambda</code>.
     */
    const std::size_t sqrtComputationThreadsCount;
};

}  // namespace cluster::test::main

#endif  // FINAL_PROJECT_HPC_CLUSTERINGALGORITHMEXECUTOR_H
