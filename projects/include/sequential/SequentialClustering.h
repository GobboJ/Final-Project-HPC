#ifndef FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H
#define FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H

#include <vector>
#include "../../src/utils/Timer.h"
#include "../../src/utils/Logger.h"
#include "../../include/utils/Types.h"

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
    template <DataIterator D, PiIterator P, LambdaIterator L>
    static void cluster(
            D data, std::size_t dataSize, std::size_t dimension, P pi, L lambda) noexcept {

        Timer::initTimers();
        Logger::startLoggingProgress<0, 1, 2, 3, 4>(dataSize);

        // Initializes the distance matrix
        auto *m = new double[dataSize];
        P currentPi = pi;
        L currentLambda = lambda;
        D currentData = data;

        // 1. Set pi(0) to 0, lambda(0) to infinity
        *currentPi = 0;
        *currentLambda = std::numeric_limits<double>::infinity();
        ++currentPi;
        ++currentLambda;
        ++currentData;

        for (std::size_t n = 1; n < dataSize; n++) {

            Timer::start<0>();

            // 1. Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
            *currentPi = n;
            *currentLambda = std::numeric_limits<double>::infinity();
            ++currentPi;
            ++currentLambda;

            Timer::stop<0>();
            Timer::start<1>();
            // 2. Set M(i) to d(i, n + 1) for i = 1,..,n
            auto *mIterator = &(m[0]);
            D stage2DataIterator = data;
            double *currentDataN = *currentData;
            for (std::size_t i = 0; i <= n - 1; i++) {
                *mIterator = distance(*stage2DataIterator, currentDataN, dimension);
                ++mIterator;
                ++stage2DataIterator;
            }
            ++currentData;

            Timer::stop<1>();
            Timer::start<2>();

            // 3. For i from 1 to n
            P stage3Pi = pi;
            L stage3Lambda = lambda;
            auto *stage3mIterator = &(m[0]);
            for (std::size_t i = 0; i <= n - 1; i++) {
                std::size_t &piOfI = *stage3Pi;
                double &lambdaOfI = *stage3Lambda;
                double mOfI = *stage3mIterator;
                double &mOfPiOfI = m[piOfI];
                // if lambda(i) >= M(i)
                if (lambdaOfI >= mOfI) {
                    // set M(pi(i)) to min { M(pi(i)), lambda(i) }
                    mOfPiOfI = std::min(mOfPiOfI, lambdaOfI);
                    // set lambda(i) to M(i)
                    lambdaOfI = mOfI;
                    // set pi(i) to n + 1
                    piOfI = n;
                } else {  // if lambda(i) < M(i)
                    // set M(pi(i)) to min { M(pi(i)), M(i) }
                    mOfPiOfI = std::min(mOfPiOfI, mOfI);
                }
                ++stage3Pi;
                ++stage3Lambda;
                ++stage3mIterator;
            }

            Timer::stop<2>();
            Timer::start<3>();

            // 4. For i from 1 to n
            P stage4Pi = pi;
            L stage4Lambda = lambda;
            for (std::size_t i = 0; i <= n - 1; i++) {
                std::size_t &piOfI = *stage4Pi;
                // if lambda(i) >= lambda(pi(i))
                if (*stage4Lambda >= lambda[piOfI]) {
                    // set pi(i) to n + 1
                    piOfI = n;
                }
                ++stage4Pi;
                ++stage4Lambda;
            }

            Timer::stop<3>();
            Logger::logProgress<1000, 0, 1, 2, 3, 4>(n, dataSize);
        }

        Logger::logProgress<1, 0, 1, 2, 3, 4>(dataSize, dataSize);
        delete[] m;
    }

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
                                  std::size_t dimension) noexcept {
        double sum = 0;
        for (std::size_t i = 0; i < dimension; i++) {
            sum += pow(firstPoint[i] - secondPoint[i], 2.0);
        }

        return sqrt(sum);
    }
};

#endif  // FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H
