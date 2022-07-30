#ifndef FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
#define FINAL_PROJECT_HPC_PARALLELCLUSTERING_H

#include <vector>
#include <limits>
#include <immintrin.h>
#include <cmath>
#include <omp.h>
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

    template <typename...>
    static constexpr std::false_type always_false{};

public:
    enum class DistanceComputers { CLASSICAL, SSE, AVX, SSE_OPTIMIZED, AVX_OPTIMIZED };

    template <DistanceComputers C, typename D, bool S4 = false>
    static void cluster(const D &data,
                        std::size_t dataSize,
                        std::size_t dimension,
                        std::vector<std::size_t> &pi,
                        std::vector<double> &lambda,
                        std::size_t distanceComputationThreadsCount = 0,
                        std::size_t stage4ThreadsCount = 0) {

        Timer::initTimers();

#ifdef PRINT_ITERATIONS
        const std::size_t dataSizeLength = ParallelClustering::computeNumberDigits(dataSize);
        std::size_t lastPrintedN = 0;
#endif

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
                } else {
                    // https://stackoverflow.com/a/53945549
                    static_assert(always_false<D>, "Unknown distance computer");
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
#pragma omp parallel for default(none) shared(n, lambda, pi) num_threads(stage4ThreadsCount) if (S4)
            for (std::size_t i = 0; i <= n - 1; i++) {
                // if lambda(i) >= lambda(pi(i))
                if (lambda[i] >= lambda[pi[i]]) {
                    // set pi(i) to n + 1
                    pi[i] = n;
                }
            }
            Timer::stop<3>();
#ifdef PRINT_ITERATIONS
            if (n == 1) {
                std::cout << "Processed 0 / " << dataSize << " rows";
                std::cout.flush();
                lastPrintedN = 1;
            } else if (n % 1000 == 0) {
                std::size_t nLength = ParallelClustering::computeNumberDigits(lastPrintedN);
                // "\033[<N>D"  dataSizeLength
                std::cout << "\033[" << (5 + dataSizeLength + 3 + nLength) << "D" << n << " / "
                          << dataSize << " rows";
                lastPrintedN = n;
                std::cout.flush();
            }
#endif
        }

#ifdef PRINT_ITERATIONS
        std::size_t nLength = ParallelClustering::computeNumberDigits(lastPrintedN);
        std::cout << "\033[" << (5 + dataSizeLength + 3 + nLength) << "D" << dataSize << " / "
                  << dataSize << " rows" << std::endl;
#endif
        std::cout << "Stage 1: ";
        Timer::print<0>();
        std::cout << "Stage 2: ";
        Timer::print<1>();
        std::cout << "Stage 3: ";
        Timer::print<2>();
        std::cout << "Stage 4: ";
        Timer::print<3>();
        std::cout << "Total  : ";
        Timer::printTotal(0ULL, 1ULL, 2ULL, 3ULL);
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

            /**
             * {a,b,c,d}
             * {e,f,g,h}
             *
             * {a, b}   {e, f}
             * {a-e, b-f}
             * {(a-e)^2, (b-f)^2}
             * {(a-e)^2 + (b-f)^2, (a-e)^2 + (b-f)^2}
             * //sum += (a-e)^2 + (b-f)^2
             *
             * {c, d}  {g, h}
             * {c-g, d-h}
             * {(c-g)^2, (d-h)^2}
             * {(c-g)^2 + (d-h)^2, (c-g)^2 + (d-h)^2}
             * sum += (c-g)^2 + (d-h)^2
             *
             *
             *
             * r = {(a-e)^2 + (b-f)^2, (a-e)^2 + (b-f)^2} + {(c-g)^2 + (d-h)^2, (c-g)^2 + (d-h)^2}
             *
             * sum = r[0]
             */
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
