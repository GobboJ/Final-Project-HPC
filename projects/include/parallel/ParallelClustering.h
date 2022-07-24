#ifndef FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
#define FINAL_PROJECT_HPC_PARALLELCLUSTERING_H

#include <vector>
#include <limits>

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-18
 * @since version date
 */
class ParallelClustering {

public:
    static const constexpr int DISTANCE_PARALLEL_THREADS_COUNT = 6;
    static const constexpr int STAGE_4_PARALLEL_THREADS_COUNT = 6;
    
    static void clusterV1(const std::vector<double *> &data,
                   std::size_t dimension,
                   std::vector<std::size_t> &pi,
                   std::vector<double> &lambda) noexcept;
    
    static void clusterV2(const std::vector<double *> &data,
                          std::size_t dimension,
                          std::vector<std::size_t> &pi,
                          std::vector<double> &lambda) noexcept;
    
    static void clusterV3(const std::vector<double *> &data,
                          std::size_t dimension,
                          std::vector<std::size_t> &pi,
                          std::vector<double> &lambda) noexcept;
    
    static void clusterV4(double *data,
                   std:: size_t dimension,
                   std::size_t dataSize,
                   std::vector<std::size_t> &pi,
                   std::vector<double> &lambda) noexcept;
    
    static void clusterV5(const std::vector<double *> &data,
                   std::size_t dimension,
                   std::vector<std::size_t> &pi,
                   std::vector<double> &lambda) noexcept;
    
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

    
};

#endif  // FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
