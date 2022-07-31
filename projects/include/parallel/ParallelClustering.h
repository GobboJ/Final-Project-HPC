#ifndef FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
#define FINAL_PROJECT_HPC_PARALLELCLUSTERING_H

#include <vector>
#include <limits>
#include <immintrin.h>
#include <cmath>
#include <omp.h>
#include "../../src/utils/Timer.h"
#include "../../src/utils/Logger.h"
#include "../../include/utils/Types.h"

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

    template <typename...>
    static constexpr std::false_type always_false{};

public:
    enum class DistanceComputers {
        CLASSICAL,
        SSE,
        AVX,
        SSE_OPTIMIZED,
        AVX_OPTIMIZED,
        SSE_OPTIMIZED_NO_SQUARE_ROOT,
        AVX_OPTIMIZED_NO_SQUARE_ROOT
    };

    template <DistanceComputers C,
              typename D,
              PiIterator P,
              LambdaIterator L,
              bool S4 = false,
              bool S5 = false>
    static void cluster(const D &data,
                        std::size_t dataSize,
                        std::size_t dimension,
                        P pi,
                        L lambda,
                        std::size_t distanceComputationThreadsCount = 0,
                        std::size_t stage4ThreadsCount = 0,
                        std::size_t squareRootThreadsCount = 0) {

        Timer::initTimers();
        Logger::startLoggingProgress<0, 1, 2, 3, 4>(dataSize);

        // Initializes the distance matrix
        auto *m = new double[dataSize];

        // 1. Set pi(0) to 0, lambda(0) to infinity
        P currentPi = pi;
        L currentLambda = lambda;
        *currentPi = 0;
        *currentLambda = std::numeric_limits<double>::infinity();

        ++currentPi;
        ++currentLambda;

        const std::size_t sseBlocksCount = ParallelClustering::computeSseBlocksCount(dimension);
        const std::size_t avxBlocksCount = ParallelClustering::computeAvxBlocksCount(dimension);

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

#pragma omp parallel for default(none)                                \
        shared(n, data, dimension, m, sseBlocksCount, avxBlocksCount) \
                num_threads(distanceComputationThreadsCount)
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
                    m[i] = ParallelClustering::distanceAvx(
                            &(data[i * AVX_PACK_SIZE * avxBlocksCount]),
                            &(data[n * AVX_PACK_SIZE * avxBlocksCount]),
                            avxBlocksCount);
                } else if constexpr (C == DistanceComputers::SSE_OPTIMIZED &&
                                     std::is_same_v<D, std::vector<double *>>) {
                    m[i] = ParallelClustering::distanceSseOptimized(
                            data[i], data[n], sseBlocksCount);
                } else if constexpr (C == DistanceComputers::AVX_OPTIMIZED &&
                                     std::is_same_v<D, std::vector<double *>>) {
                    m[i] = ParallelClustering::distanceAvxOptimized(
                            data[i], data[n], avxBlocksCount);
                } else if constexpr (
                        C == DistanceComputers::SSE_OPTIMIZED && std::is_same_v<D, double *>) {
                    m[i] = ParallelClustering::distanceSseOptimized(
                            &(data[i * SSE_PACK_SIZE * sseBlocksCount]),
                            &(data[n * SSE_PACK_SIZE * sseBlocksCount]),
                            sseBlocksCount);
                } else if constexpr (
                        C == DistanceComputers::AVX_OPTIMIZED && std::is_same_v<D, double *>) {
                    m[i] = ParallelClustering::distanceAvxOptimized(
                            &(data[i * AVX_PACK_SIZE * avxBlocksCount]),
                            &(data[n * AVX_PACK_SIZE * avxBlocksCount]),
                            avxBlocksCount);
                } else if constexpr (C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT &&
                                     std::is_same_v<D, double *>) {
                    m[i] = ParallelClustering::distanceSseOptimizedNoSquareRoot(
                            &(data[i * SSE_PACK_SIZE * sseBlocksCount]),
                            &(data[n * SSE_PACK_SIZE * sseBlocksCount]),
                            sseBlocksCount);
                } else if constexpr (C == DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT &&
                                     std::is_same_v<D, double *>) {
                    m[i] = ParallelClustering::distanceAvxOptimizedNoSquareRoot(
                            &(data[i * AVX_PACK_SIZE * avxBlocksCount]),
                            &(data[n * AVX_PACK_SIZE * avxBlocksCount]),
                            avxBlocksCount);
                } else {
                    // https://stackoverflow.com/a/53945549
                    static_assert(always_false<D>, "Unknown distance computer");
                }
            }

            Timer::stop<1>();
            Timer::start<2>();

            // 3. For i from 1 to n
            P stage3Pi = pi;
            L stage3Lambda = lambda;
            auto *mIterator = &(m[0]);
            for (std::size_t i = 0; i <= n - 1; i++) {
                // if lambda(i) >= M(i)
                if (*stage3Lambda >= *mIterator) {
                    // set M(pi(i)) to min { M(pi(i)), lambda(i) }
                    m[*stage3Pi] = std::min(m[*stage3Pi], *stage3Lambda);
                    // set lambda(i) to M(i)
                    *stage3Lambda = *mIterator;
                    // set pi(i) to n + 1
                    *stage3Pi = n;
                }
                // if lambda(i) < M(i)
                if (*stage3Lambda < *mIterator) {
                    // set M(pi(i)) to min { M(pi(i)), M(i) }
                    m[*stage3Pi] = std::min(m[*stage3Pi], *mIterator);
                }
                ++stage3Pi;
                ++stage3Lambda;
                ++mIterator;
            }

            Timer::stop<2>();
            Timer::start<3>();

            // 4. For i from 1 to n
#pragma omp parallel for default(none) shared(n, lambda, pi) num_threads(stage4ThreadsCount) if (S4)
            for (std::size_t i = 0; i <= n - 1; i++) {
                // if lambda(i) >= lambda(pi(i))
                if (lambda[i] >= lambda[pi[i]]) {
                    // set pi(i) to n + 1
                    pi[i] = n;
                }
            }
            Timer::stop<3>();
            Logger::logProgress<1000, 0, 1, 2, 3, 4>(n, dataSize);
        }

        // Compute the square roots
        Timer::start<4>();
        if constexpr (C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT ||
                      C == DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT) {
#pragma omp parallel for default(none) shared(lambda, dataSize) \
        num_threads(squareRootThreadsCount) if (S5)
            for (std::size_t i = 0; i < dataSize - 1; i++) {
                lambda[i] = sqrt(lambda[i]);
            }
        }
        Timer::stop<4>();
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
    static inline double distance(const double *__restrict__ const firstPoint,
                                  const double *__restrict__ const secondPoint,
                                  const std::size_t dimension) noexcept {

        double sum = 0;
        for (std::size_t i = 0; i < dimension; i++) {
            sum += pow(firstPoint[i] - secondPoint[i], 2.0);
        }

        return sqrt(sum);
    }

    static inline std::size_t computeSseBlocksCount(std::size_t dimension) {

        return 1 + ((dimension - 1) / SSE_PACK_SIZE);
    }

    static inline std::size_t computeAvxBlocksCount(std::size_t dimension) {

        return 1 + ((dimension - 1) / AVX_PACK_SIZE);
    }

    static inline double distanceSse(const double *__restrict__ const firstPoint,
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

    static inline double distanceSseOptimized(const double *__restrict__ const firstPoint,
                                              const double *__restrict__ const secondPoint,
                                              std::size_t blocksCount) noexcept {

        __m128d accumulator = _mm_setzero_pd();

        for (std::size_t j = 0; j < blocksCount; j++) {
            __m128d dataI = _mm_load_pd(&(firstPoint[j * SSE_PACK_SIZE]));   // data[i]{0, 1}
            __m128d dataN = _mm_load_pd(&(secondPoint[j * SSE_PACK_SIZE]));  // data[n]{0, 1}

            __m128d sub = _mm_sub_pd(dataI, dataN);
            __m128d square = _mm_mul_pd(sub, sub);  // x^2 y^2

            accumulator = _mm_add_pd(accumulator, square);
        }

        __m128d horizontalSum = _mm_hadd_pd(accumulator, accumulator);
        return sqrt(horizontalSum[0]);
    }

    static inline double distanceSseOptimizedNoSquareRoot(
            const double *__restrict__ const firstPoint,
            const double *__restrict__ const secondPoint,
            std::size_t blocksCount) noexcept {

        __m128d accumulator = _mm_setzero_pd();

        for (std::size_t j = 0; j < blocksCount; j++) {
            __m128d dataI = _mm_load_pd(&(firstPoint[j * SSE_PACK_SIZE]));   // data[i]{0, 1}
            __m128d dataN = _mm_load_pd(&(secondPoint[j * SSE_PACK_SIZE]));  // data[n]{0, 1}

            __m128d sub = _mm_sub_pd(dataI, dataN);
            __m128d square = _mm_mul_pd(sub, sub);  // x^2 y^2

            accumulator = _mm_add_pd(accumulator, square);
        }

        __m128d horizontalSum = _mm_hadd_pd(accumulator, accumulator);
        return horizontalSum[0];
    }

    static inline double distanceAvx(const double *__restrict__ const firstPoint,
                                     const double *__restrict__ const secondPoint,
                                     std::size_t blocksCount) noexcept {

        double sum = 0;
        for (std::size_t j = 0; j < blocksCount; j++) {
            __m256d dataI = _mm256_load_pd(&(firstPoint[j * AVX_PACK_SIZE]));
            __m256d dataN = _mm256_load_pd(&(secondPoint[j * AVX_PACK_SIZE]));

            __m256d sub = _mm256_sub_pd(dataI, dataN);
            __m256d square = _mm256_mul_pd(sub, sub);  // x^2, y^2, z^2, a^2

            __m256d squareSum = _mm256_hadd_pd(square, square);
            // extract upper 128 bits of result
            __m128d sumHighBits = _mm256_extractf128_pd(squareSum, 1);
            __m128d sumLowBits = _mm256_castpd256_pd128(squareSum);
            // add upper 128 bits of sum to its lower 128 bits
            sum += _mm_add_pd(sumHighBits, sumLowBits)[0];
        }

        return sqrt(sum);
    }

    static inline double distanceAvxOptimized(const double *__restrict__ const firstPoint,
                                              const double *__restrict__ const secondPoint,
                                              std::size_t blocksCount) noexcept {

        __m256d accumulator = _mm256_setzero_pd();

        for (std::size_t j = 0; j < blocksCount; j++) {
            __m256d dataI = _mm256_load_pd(&(firstPoint[j * AVX_PACK_SIZE]));
            __m256d dataN = _mm256_load_pd(&(secondPoint[j * AVX_PACK_SIZE]));

            __m256d sub = _mm256_sub_pd(dataI, dataN);
            __m256d square = _mm256_mul_pd(sub, sub);  // x^2, y^2, z^2, a^2

            accumulator = _mm256_add_pd(accumulator, square);
        }

        __m256d horizontalSum = _mm256_hadd_pd(accumulator, accumulator);

        // Extract upper 128 bits of result
        __m128d sumHighBits = _mm256_extractf128_pd(horizontalSum, 1);
        __m128d sumLowBits = _mm256_castpd256_pd128(horizontalSum);

        // Add upper 128 bits of sum to its lower 128 bits
        return sqrt(_mm_add_pd(sumHighBits, sumLowBits)[0]);
    }

    static inline double distanceAvxOptimizedNoSquareRoot(
            const double *__restrict__ const firstPoint,
            const double *__restrict__ const secondPoint,
            std::size_t blocksCount) noexcept {

        __m256d accumulator = _mm256_setzero_pd();

        for (std::size_t j = 0; j < blocksCount; j++) {
            __m256d dataI = _mm256_load_pd(&(firstPoint[j * AVX_PACK_SIZE]));
            __m256d dataN = _mm256_load_pd(&(secondPoint[j * AVX_PACK_SIZE]));

            __m256d sub = _mm256_sub_pd(dataI, dataN);
            __m256d square = _mm256_mul_pd(sub, sub);  // x^2, y^2, z^2, a^2

            accumulator = _mm256_add_pd(accumulator, square);
        }

        __m256d horizontalSum = _mm256_hadd_pd(accumulator, accumulator);

        // Extract upper 128 bits of result
        __m128d sumHighBits = _mm256_extractf128_pd(horizontalSum, 1);
        __m128d sumLowBits = _mm256_castpd256_pd128(horizontalSum);

        // Add upper 128 bits of sum to its lower 128 bits
        return _mm_add_pd(sumHighBits, sumLowBits)[0];
    }

#ifdef PRINT_ITERATIONS
    static inline std::size_t computeNumberDigits(std::size_t number) {

        if (number == 0) {
            return 1;
        }
        std::size_t digits = 0;
        while (number > 0) {
            digits++;
            number /= 10;
        }

        return digits;
    }
#endif
};

#endif  // FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
