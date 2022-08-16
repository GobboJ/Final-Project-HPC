#ifndef FINAL_PROJECT_HPC_CLUSTERINGALGORITHMEXECUTOR_H
#define FINAL_PROJECT_HPC_CLUSTERINGALGORITHMEXECUTOR_H

#include <cstddef>
#include <vector>

namespace cluster::test::main {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-16
 * @since version date
 */
class ClusteringAlgorithmExecutor {

public:
    ClusteringAlgorithmExecutor(std::size_t dataElementsCount,
                                std::size_t dimension,
                                std::size_t distanceComputationThreadsCount,
                                std::size_t structuralFixThreadsCount,
                                std::size_t sqrtComputationThreadsCount);

    void executeClusteringAlgorithm(bool isParallel,
                                    std::size_t version,
                                    const std::vector<double *> &data,
                                    const std::vector<double *> &sseAlignedIndirectData,
                                    const std::vector<double *> &avxAlignedIndirectData,
                                    const double *sseAlignedData,
                                    const double *avxAlignedData,
                                    const double *uniqueArrayData,
                                    std::vector<std::size_t>::iterator piIterator,
                                    std::vector<double>::iterator lambdaIterator);

private:
    void executeParallelV1(const std::vector<double *> &data,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator);
    void executeParallelV2(const std::vector<double *> &sseAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator);
    void executeParallelV3(const std::vector<double *> &avxAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator);
    void executeParallelV4(const double *avxAlignedData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator);
    void executeParallelV5(const std::vector<double *> &sseAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator);
    void executeParallelV6(const std::vector<double *> &sseAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator);
    void executeParallelV7(const std::vector<double *> &avxAlignedIndirectData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator);
    void executeParallelV8(const double *sseAlignedData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator);
    void executeParallelV9(const double *avxAlignedData,
                           std::vector<std::size_t>::iterator piIterator,
                           std::vector<double>::iterator lambdaIterator);
    void executeParallelV10(const double *avxAlignedData,
                            std::vector<std::size_t>::iterator piIterator,
                            std::vector<double>::iterator lambdaIterator);
    void executeParallelV11(const double *avxAlignedData,
                            std::vector<std::size_t>::iterator piIterator,
                            std::vector<double>::iterator lambdaIterator);

    void executeSequentialV1(const std::vector<double *> &data,
                             std::vector<std::size_t>::iterator piIterator,
                             std::vector<double>::iterator lambdaIterator);
    void executeSequentialV2(const double *uniqueArrayData,
                             std::vector<std::size_t>::iterator piIterator,
                             std::vector<double>::iterator lambdaIterator);

    const std::size_t dataElementsCount;
    const std::size_t dimension;
    const std::size_t distanceComputationThreadsCount;
    const std::size_t structuralFixThreadsCount;
    const std::size_t sqrtComputationThreadsCount;
};

}  // namespace cluster::test::main

#endif  // FINAL_PROJECT_HPC_CLUSTERINGALGORITHMEXECUTOR_H
