/*
 * Implementation of the sequential clustering algorithm.
 *
 * @author DeB, Jonathan
 * @version 1.1 2022-07-18
 * @since 1.0
 */
#include <vector>
#include <climits>
#include <cmath>
#include "../../include/sequential/SequentialClustering.h"
#include "../utils/Timer.h"

/**
 * Sequential clustering algorithm.
 *
 * @param data Data to process.
 * @param dimension Dimension of each point in the data,
 * @param pi Output vector that will be filled with the representative of the clusters each point
 * belongs to.
 * @param lambda Output vector that will be filled with the distance between each point and the
 * cluster it belongs to.
 */
void SequentialClustering::cluster(const std::vector<double *> &data,
                                   std::size_t dimension,
                                   std::vector<std::size_t> &pi,
                                   std::vector<double> &lambda) noexcept {

    std::size_t dataSize = data.size();

    // Initializes pi and lambda vectors

    pi.resize(dataSize);
    lambda.resize(dataSize);

    // Initializes the distance matrix
    std::vector<double> partRow{};
    partRow.resize(dataSize);

    // 1. Set pi(0) to 0, lambda(0) to infinity
    pi[0] = 0;
    lambda[0] = std::numeric_limits<double>::infinity();

    for (std::size_t n = 1; n < dataSize; n++) {

        Timer::start<0>();

        // 1. Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
        pi[n] = n;
        lambda[n] = std::numeric_limits<double>::infinity();

        Timer::stop<0>();
        Timer::start<1>();
        // 2. Set M(i) to d(i, n + 1) for i = 1,..,n
        for (std::size_t i = 0; i <= n - 1; i++) {
            partRow[i] = distance(data[i], data[n], dimension);
        }

        Timer::stop<1>();
        Timer::start<2>();

        // 3. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= M(i)
            if (lambda[i] >= partRow[i]) {
                // set M(pi(i)) to min { M(pi(i)), lambda(i) }
                partRow[pi[i]] = std::min(partRow[pi[i]], lambda[i]);
                // set lambda(i) to M(i)
                lambda[i] = partRow[i];
                // set pi(i) to n + 1
                pi[i] = n;
            }
            // if lambda(i) < M(i)
            if (lambda[i] < partRow[i]) {
                // set M(pi(i)) to min { M(pi(i)), M(i) }
                partRow[pi[i]] = std::min(partRow[pi[i]], partRow[i]);
            }
        }

        Timer::stop<2>();
        Timer::start<3>();

        // 4. For i from 1 to n
        for (std::size_t i = 0; i <= n - 1; i++) {
            // if lambda(i) >= lambda(pi(i))
            if (lambda[i] >= lambda[pi[i]]) {
                // set pi(i) to n + 1
                pi[i] = n;
            }
        }

        Timer::stop<3>();

#ifdef PRINT_ITERATIONS
        if (n % 1000 == 0) {
            std::cout << "Processed" << ' ' << n << " rows" << std::endl;
        }
#endif
    }

    std::cout << "Stage 1: ";
    Timer::print<0>();
    std::cout << "Stage 2: ";
    Timer::print<1>();
    std::cout << "Stage 3: ";
    Timer::print<2>();
    std::cout << "Stage 4: ";
    Timer::print<3>();
    std::cout << "Total  : ";
    // TODO: Timer::printTotal(0ULL, 1ULL, 2ULL, 3ULL);
}

/**
 * Computes the distance between two points.
 *
 * @param firstPoint First point.
 * @param secondPoint Second point.
 * @param dimension Dimension of each point.
 * @return The distance between the points.
 */
double SequentialClustering::distance(const double *__restrict__ const firstPoint,
                                      const double *__restrict__ const secondPoint,
                                      const std::size_t dimension) noexcept {

    double sum = 0;
    for (std::size_t i = 0; i < dimension; i++) {
        sum += pow(firstPoint[i] - secondPoint[i], 2.0);
    }

    return sqrt(sum);
}
