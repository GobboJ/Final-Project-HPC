#ifndef FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H
#define FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H

#include <vector>

/**
 * Class implementing the sequential clustering algorithm.
 *
 * @author DeB, Jonathan
 * @version 1.1 2022-07-18
 * @since 1.0
 */
class SequentialClustering {

public:
    /**
     * Sequential clustering algorithm.
     *
     * @param data The set of samples to apply the algorithm to.
     * @param dimension The number of attributes of the samples.
     * @param pi Reference to the vector that will be filled with the reference to the cluster each
     * point belongs to.
     * @param lambda Reference to the vector that will be filled with the distance between each
     * point and the cluster it belongs to.
     */
    static void cluster(const std::vector<double *> &data,
                        std::size_t dimension,
                        std::vector<std::size_t> &pi,
                        std::vector<double> &lambda) noexcept;

private:
    /**
     * Computes the distance between two points.
     *
     * @param firstPoint The first point.
     * @param secondPoint The second point.
     * @param dimension The dimension of each point.
     * @return The distance between the two points.
     */
    static inline double distance(const double *__restrict__ firstPoint,
                           const double *__restrict__ secondPoint,
                           std::size_t dimension) noexcept;
    
    static inline std::size_t computeNumberDigits(std::size_t number);
};

#endif  // FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H
