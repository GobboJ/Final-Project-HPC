#ifndef FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
#define FINAL_PROJECT_HPC_PARALLELCLUSTERING_H

#include <vector>
#include <limits>
#include <immintrin.h>
#include <cmath>
#include "../../src/utils/Timer.h"

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-18
 * @since version date
 */
class ParallelClustering {

private:
    static const constexpr std::size_t SSE_PACK_SIZE = 2;
    static const constexpr std::size_t AVX_PACK_SIZE = 4;

public:
    enum class DistanceComputers { CLASSICAL, SSE, AVX };
    static const constexpr int DISTANCE_PARALLEL_THREADS_COUNT = 6;
    static const constexpr int STAGE_4_PARALLEL_THREADS_COUNT = 6;

    template <DistanceComputers C, typename D, bool S4 = false>
    static void cluster(const D &data,
                        std::size_t dataSize,
                        std::size_t dimension,
                        std::vector<std::size_t> &pi,
                        std::vector<double> &lambda) {

        // Initializes pi and lambda vectors
        pi.resize(dataSize);
        lambda.resize(dataSize);

        // Initializes the distance matrix
        std::vector<double> m{};
        m.resize(dataSize);

        // 1. Set pi(0) to 0, lambda(0) to infinity
        pi[0] = 0;
        lambda[0] = std::numeric_limits<double>::infinity();

        const std::size_t sseBlocksCount = ParallelClustering::computeSseBlocksCount(dimension);
        const std::size_t avxBlocksCount = ParallelClustering::computeAvxBlocksCount(dimension);

        for (std::size_t n = 1; n < dataSize; n++) {

            Timer::start<0>();

            // 1. Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
            pi[n] = n;
            lambda[n] = std::numeric_limits<double>::infinity();

            Timer::stop<0>();
            Timer::start<1>();

            // 2. Set M(i) to d(i, n + 1) for i = 1,..,n

#pragma omp parallel for default(none)                                \
        shared(n, data, dimension, m, sseBlocksCount, avxBlocksCount) \
                num_threads(ParallelClustering::DISTANCE_PARALLEL_THREADS_COUNT)
            for (std::size_t i = 0; i <= n - 1; i++) {
                if constexpr (C == DistanceComputers::CLASSICAL &&
                              std::is_same_v<D, std::vector<double *>>) {
                    m[i] = ParallelClustering::distance(data[i], data[n], dimension);
                } else if constexpr (
                        C == DistanceComputers::SSE && std::is_same_v<D, std::vector<double *>>) {
                    m[i] = ParallelClustering::distanceSse(data[i], data[n], sseBlocksCount);
                } else if constexpr (
                        C == DistanceComputers::AVX && std::is_same_v<D, std::vector<double *>>) {
                    m[i] = ParallelClustering::distanceAvx(data[i], data[n], avxBlocksCount);
                } else if constexpr (C == DistanceComputers::AVX && std::is_same_v<D, double *>) {
                    m[i] = ParallelClustering::distanceAvx(&(data[i * AVX_PACK_SIZE * avxBlocksCount]),
                                                           &(data[n * AVX_PACK_SIZE * avxBlocksCount]),
                                                           avxBlocksCount);
                }
            }

            Timer::stop<1>();
            Timer::start<2>();

            // 3. For i from 1 to n
            for (std::size_t i = 0; i <= n - 1; i++) {
                // if lambda(i) >= M(i)
                if (lambda[i] >= m[i]) {
                    // set M(pi(i)) to min { M(pi(i)), lambda(i) }
                    m[pi[i]] = std::min(m[pi[i]], lambda[i]);
                    // set lambda(i) to M(i)
                    lambda[i] = m[i];
                    // set pi(i) to n + 1
                    pi[i] = n;
                }
                // if lambda(i) < M(i)
                if (lambda[i] < m[i]) {
                    // set M(pi(i)) to min { M(pi(i)), M(i) }
                    m[pi[i]] = std::min(m[pi[i]], m[i]);
                }
            }

            Timer::stop<2>();
            Timer::start<3>();

            // 4. For i from 1 to n
#pragma omp parallel for default(none) shared(n, lambda, pi) \
        num_threads(ParallelClustering::STAGE_4_PARALLEL_THREADS_COUNT) if (S4)
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
                std::cout << "Processed" << ' ' << n << " " << std::endl;
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

private:
    /**
     * Computes the distance between two points.
     *
     * @param firstPoint The first point.
     * @param secondPoint The second point.
     * @param dimension The dimension of each point.
     * @return The distance between the two points.
     */
    static double distance(const double *__restrict__ const firstPoint,
                                        const double *__restrict__ const secondPoint,
                                        const std::size_t dimension) noexcept {
        
        double sum = 0;
        for (std::size_t i = 0; i < dimension; i++) {
            sum += pow(firstPoint[i] - secondPoint[i], 2.0);
        }

        return sqrt(sum);
    }

    static std::size_t computeSseBlocksCount(std::size_t dimension) {

        return 1 + ((dimension - 1) / SSE_PACK_SIZE);
    }

    static std::size_t computeAvxBlocksCount(std::size_t dimension) {

        return 1 + ((dimension - 1) / AVX_PACK_SIZE);
    }

    static double distanceSse(const double *__restrict__ const firstPoint,
                                           const double *__restrict__ const secondPoint,
                                           std::size_t blocksCount) noexcept {

        double sum = 0;

        for (std::size_t j = 0; j < blocksCount; j++) {
            __m128d dataI = _mm_load_pd(&(firstPoint[j * SSE_PACK_SIZE]));   // data[i]{0, 1}
            __m128d dataN = _mm_load_pd(&(secondPoint[j * SSE_PACK_SIZE]));  // data[n]{0, 1}

            __m128d sub = _mm_sub_pd(dataI, dataN);
            __m128d square = _mm_mul_pd(sub, sub);

            sum += _mm_hadd_pd(square, square)[0];
        }

        return sqrt(sum);
    }

    static double distanceAvx(const double *__restrict__ const firstPoint,
                                           const double *__restrict__ const secondPoint,
                                           std::size_t blocksCount) noexcept {

        double sum = 0;
        for (std::size_t j = 0; j < blocksCount; j++) {
            __m256d dataI = _mm256_load_pd(&(firstPoint[j * AVX_PACK_SIZE]));
            __m256d dataN = _mm256_load_pd(&(secondPoint[j * AVX_PACK_SIZE]));

            __m256d sub = _mm256_sub_pd(dataI, dataN);
            __m256d square = _mm256_mul_pd(sub, sub);  // x^2, y^2, z^2, a^2
            sum += _mm256_hadd_pd(square, square)[0];  // (x^2 + y^2 + z^2 + a^2) ...
        }

        return sqrt(sum);
    }
};

#endif  // FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
