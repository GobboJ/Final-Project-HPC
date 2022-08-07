#ifndef FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
#define FINAL_PROJECT_HPC_PARALLELCLUSTERING_H

#include "Timer.h"
#include "../../include/utils/Types.h"
#include "DistanceComputers.h"
#include "Logger.h"
#include <cmath>
#include <immintrin.h>
#include <limits>
#include <omp.h>
#include <vector>

namespace cluster::parallel {

/**
 * Class providing a parallel implementation of the clustering algorithm.<br>
 * This class offers only one method that takes several template arguments that can be used to
 * customize its behaviour.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.5 2022-08-01
 * @since 1.0
 */
template <bool S2 = true, bool S4 = false, bool S5 = false>
class ParallelClustering {

    using Logger = utils::Logger;
    using Timer = utils::Timer;

private:
    /**
     * Convenient declaration that allows to produce a compile-time assertion error.
     *
     * @tparam Ts This must be one or more type parameters of the function that uses this
     * declaration.
     */
    template <typename... Ts>
    static constexpr std::false_type always_false{};

    static const constexpr std::size_t SSE_PACK_SIZE = 2;
    static const constexpr std::size_t AVX_PACK_SIZE = 4;

public:
    /**
     * Parallel implementation of the clustering algorithm.
     *
     * @tparam C Enumeration constant specifying the algorithm to use to compute the distance
     * between two data samples.
     * @tparam D Type of the iterator that iterates over the data samples to cluster.
     * @tparam P Type of the iterator that allows to fill the data structure holding the
     * <code>pi</code> values.
     * @tparam L Type of the iterator that allows to fill the data structure holding the
     * <code>lambda</code> values.
     * @tparam S2 <code>true</code> if this method should parallelize the computation of the
     * distance using threads, <code>false</code> otherwise.
     * @tparam S4 <code>true</code> if this method should parallelize the computation of the
     * stage 4 using threads, <code>false</code> otherwise.
     * @tparam S5 <code>true</code> if this method should parallelize the computation of the
     * square roots using threads, <code>false</code> otherwise. This template argument take effect
     * only if the distance computation algorithm is one of:
     * <ul>
     *      <li><code>SSE_OPTIMIZED_NO_SQUARE_ROOT</code>;</li>
     *      <li><code>AVX_OPTIMIZED_NO_SQUARE_ROOT</code>.</li>
     * </ul>
     * In all the other cases, it has no effect.
     * @param dataIterator Iterator over the set of data samples to cluster.
     * @param dataSamplesCount Number of data samples.
     * @param dimension Number of attributes of each sample.
     * @param piIterator Output iterator that will be used to fill the underlying data structure
     * with the <code>pi</code> values. The underlying data structure <b>MUST BE</b> big enough to
     * contain all the <code>pi</code> values, which are exactly <code>dataSamplesCount</code>.
     * @param lambdaIterator Output iterator that will be used to fill the underlying data structure
     * with the <code>lambda</code> values. The underlying data structure <b>MUST BE</b> big enough
     * to contain all the <code>lambda</code> values, which are exactly
     * <code>dataSamplesCount</code>.
     * @param distanceComputationThreadsCount Number of threads to use to parallelize the
     * computation of the distance between the data samples. If not specified, or if <code>0</code>
     * is specified, then this method uses the default number of threads computed by OpenMP.<br>
     * This parameter is ignored only if <code>S2</code> is <code>false</code>.
     * @param stage4ThreadsCount Number of threads to use to parallelize the computation of stage 4.
     * If not specified, or if <code>0</code> is specified, then this method uses the default number
     * of threads computed by OpenMP.<br>
     * This parameter is ignored only if <code>S4</code> is <code>false</code>.
     * @param squareRootThreadsCount Number of threads to use to parallelize the computation of
     * square roots of the distances. If not specified, or if <code>0</code> is specified, then this
     * method uses the default number of threads computed by OpenMP.<br> This parameter is ignored
     * only if <code>S5</code> is <code>false</code>, or if the distance computation algorithm is
     * neither <code>SSE_OPTIMIZED_NO_SQUARE_ROOT</code> nor
     * <code>AVX_OPTIMIZED_NO_SQUARE_ROOT</code>.
     */
    template <DistanceComputers C, ParallelDataIterator D, PiIterator P, LambdaIterator L>
    static void cluster(const D dataIterator,
                        const std::size_t dataSamplesCount,
                        const std::size_t dimension,
                        const P piIterator,
                        const L lambdaIterator,
                        const std::size_t distanceComputationThreadsCount = 0,
                        const std::size_t stage4ThreadsCount = 0,
                        const std::size_t squareRootThreadsCount = 0) {

        // Initialize the timers and start logging the console output, if requested
        //Timer::initTimers();
        Logger::startLoggingProgress<0, 1, 2, 3, 4, 5>(dataSamplesCount);

        Timer::start<0>();
        // Initialize the part-row values
        auto *__restrict__ m = new double[dataSamplesCount];

        // Iterator pointing to pi[i], with i ranging from 0 to dataSamplesCount-1.
        // Initially it points to pi[0], i.e., the first element of pi.
        P currentPi = piIterator;
        // Iterator pointing to lambda[i], with i ranging from 0 to dataSamplesCount-1.
        // Initially it points to lambda[0], i.e., the first element of lambda.
        L currentLambda = lambdaIterator;
        // Iterator pointing to the n-th element of the dataset.
        // Initially it points to the first element of the dataset.
        D currentData = dataIterator;

        Timer::stop<0>();

        /*******************************************************************************************
         * 1) Set pi(1) to 0, lambda(1) to infinity
         ******************************************************************************************/
        Timer::start<1>();

        initializeNewPoint(currentPi, currentLambda, 0);
        Timer::stop<1>();

        Timer::start<4>();
        // No more operations need to be performed for the first point, so move to the second
        // element of the dataset
        ++currentData;

        Timer::stop<4>();

        // Computes the number of SSE registers that are needed to store a data sample
        const std::size_t sseBlocksCount = ParallelClustering::computeSseBlocksCount(dimension);
        // Computes the number of AVX registers that are needed to store a data sample
        const std::size_t avxBlocksCount = ParallelClustering::computeAvxBlocksCount(dimension);

        // Perform the clustering algorithm for all the remaining data samples
        for (std::size_t n = 1; n < dataSamplesCount; n++) {

            /***************************************************************************************
             * 1) Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
             **************************************************************************************/
            Timer::start<1>();

            initializeNewPoint(currentPi, currentLambda, n);

            Timer::stop<1>();

            /***************************************************************************************
             * 2) Set M(i) to d(i, n + 1) for i = 1,..,n
             **************************************************************************************/
            Timer::start<2>();

            // Value of the n-th value of the dataset
            const double *__restrict__ const currentDataN = *currentData;

            computeDistances<C, D>(n,
                                   dataIterator,
                                   currentDataN,
                                   dimension,
                                   m,
                                   sseBlocksCount,
                                   avxBlocksCount,
                                   distanceComputationThreadsCount);

            Timer::stop<2>();

            /***************************************************************************************
             * 3) For i from 1 to n
             **************************************************************************************/
            Timer::start<3>();

            // Iterator over the first n-1 values of pi
            P iteratorOverPi = piIterator;
            // Iterator over the first n-1 values of lambda
            L iteratorOverLambda = lambdaIterator;

            // Iterator over the first n-1 values of m
            const double *__restrict__ distanceIterator = &(m[0]);

            addNewPoint(iteratorOverPi, iteratorOverLambda, distanceIterator, m, n);

            Timer::stop<3>();

            /***************************************************************************************
             * 4) For i from 1 to n
             **************************************************************************************/
            Timer::start<4>();

            fixStructure<P, L>(piIterator, lambdaIterator, n, stage4ThreadsCount);
            // Move to the next data sample
            ++currentData;

            Timer::stop<4>();

            // Log the progress every 1000 samples
            Logger::updateProgress<1000, 0, 1, 2, 3, 4, 5>(n, dataSamplesCount);
        }

        // Compute the square roots, if until now the algorithm has used the squares of the
        // distances
        Timer::start<5>();
        if constexpr (C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT ||
                      C == DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT) {

            // Compute the square root of all the values in lambda
#pragma omp parallel for default(none) shared(lambdaIterator, dataSamplesCount) \
        num_threads(squareRootThreadsCount) if (S5)
            for (std::size_t i = 0; i < dataSamplesCount - 1; i++) {
                lambdaIterator[i] = sqrt(lambdaIterator[i]);
            }
        }
        Timer::stop<5>();

        // Deallocate m, since it is not needed anymore
        delete[] m;

        // Log the final progress
        Logger::updateProgress<1, 0, 1, 2, 3, 4, 5>(dataSamplesCount, dataSamplesCount);
    }

    static inline std::size_t computeSseBlocksCount(std::size_t dimension) {

        return 1 + ((dimension - 1) / SSE_PACK_SIZE);
    }

    static inline std::size_t computeAvxBlocksCount(std::size_t dimension) {

        return 1 + ((dimension - 1) / AVX_PACK_SIZE);
    }

private:
    template <typename P, typename L>
    static inline void initializeNewPoint(P &currentPi, L &currentLambda, std::size_t n) {

        // Set pi[n] to n
        *currentPi = n;
        ++currentPi;
        // Set lambda[n] to infinity
        *currentLambda = std::numeric_limits<double>::infinity();
        ++currentLambda;
    }

    template <DistanceComputers C, ParallelDataIterator D>
    static inline void computeDistances(std::size_t n,
                                        const D &dataIterator,
                                        const double *currentDataN,
                                        std::size_t dimension,
                                        double *const m,
                                        std::size_t sseBlocksCount,
                                        std::size_t avxBlocksCount,
                                        std::size_t distanceComputationThreadsCount) {

        // Compute the distance between the n-th element of the dataset and all the
// already-processed ones
#pragma omp parallel for default(none)                                                      \
        shared(n, dataIterator, currentDataN, dimension, m, sseBlocksCount, avxBlocksCount) \
                num_threads(distanceComputationThreadsCount) if (S2)
        for (std::size_t i = 0; i <= n - 1; i++) {
            // Compute the distance by using the requested algorithm
            if constexpr (C == DistanceComputers::CLASSICAL) {
                m[i] = ParallelClustering::distance(dataIterator[i], currentDataN, dimension);
            } else if constexpr (C == DistanceComputers::SSE) {
                m[i] = ParallelClustering::distanceSse(
                        dataIterator[i], currentDataN, sseBlocksCount);
            } else if constexpr (C == DistanceComputers::AVX) {
                m[i] = ParallelClustering::distanceAvx(
                        dataIterator[i], currentDataN, avxBlocksCount);
            } else if constexpr (C == DistanceComputers::SSE_OPTIMIZED) {
                m[i] = ParallelClustering::distanceSseOptimized(
                        dataIterator[i], currentDataN, sseBlocksCount);
            } else if constexpr (C == DistanceComputers::AVX_OPTIMIZED) {
                m[i] = ParallelClustering::distanceAvxOptimized(
                        dataIterator[i], currentDataN, avxBlocksCount);
            } else if constexpr (C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT) {
                m[i] = ParallelClustering::distanceSseOptimizedNoSquareRoot(
                        dataIterator[i], currentDataN, sseBlocksCount);
            } else if constexpr (C == DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT) {
                m[i] = ParallelClustering::distanceAvxOptimizedNoSquareRoot(
                        dataIterator[i], currentDataN, avxBlocksCount);
            } else {
                // https://stackoverflow.com/a/53945549
                static_assert(always_false<D>, "The specified distance computer is not supported.");
            }
        }
    }

    template <typename P, typename L>
    static inline void addNewPoint(P iteratorOverPi,
                                   L iteratorOverLambda,
                                   const double *distanceIterator,
                                   double *m,
                                   std::size_t n) {

        for (std::size_t i = 0; i <= n - 1; i++) {
            // Reference to pi[i]
            std::size_t &piOfI = *iteratorOverPi;
            // Reference to lambda[i]
            double &lambdaOfI = *iteratorOverLambda;
            // Value of m[i]
            const double mOfI = *distanceIterator;
            // Reference to m[pi[i]]
            double &mOfPiOfI = m[piOfI];

            /***********************************************************************************
             * if lambda(i) >= M(i)
             **********************************************************************************/
            if (lambdaOfI >= mOfI) {
                /*******************************************************************************
                 * set M(pi(i)) to min { M(pi(i)), lambda(i) }
                 ******************************************************************************/
                mOfPiOfI = std::min(mOfPiOfI, lambdaOfI);

                /*******************************************************************************
                 * set lambda(i) to M(i)
                 ******************************************************************************/
                lambdaOfI = mOfI;

                /*******************************************************************************
                 * set pi(i) to n + 1
                 ******************************************************************************/
                piOfI = n;
            } else {  // if lambda(i) < M(i)
                /*******************************************************************************
                 * set M(pi(i)) to min { M(pi(i)), M(i) }
                 ******************************************************************************/
                mOfPiOfI = std::min(mOfPiOfI, mOfI);
            }
            // Move to the next element
            ++iteratorOverPi;
            ++iteratorOverLambda;
            ++distanceIterator;
        }
    }

    template <typename P, typename L>
    static inline void fixStructure(const P &piIterator,
                                    const L &lambdaIterator,
                                    std::size_t n,
                                    std::size_t stage4ThreadsCount) {

#pragma omp parallel for default(none) shared(n, lambdaIterator, piIterator) \
        num_threads(stage4ThreadsCount) if (S4)
        for (std::size_t i = 0; i <= n - 1; i++) {
            // Reference to pi[i]
            std::size_t &piOfI = piIterator[i];

            /***********************************************************************************
             * if lambda(i) >= lambda(pi(i))
             **********************************************************************************/
            if (lambdaIterator[i] >= lambdaIterator[piOfI]) {
                /*******************************************************************************
                 * set pi(i) to n + 1
                 ******************************************************************************/
                piOfI = n;
            }
        }
    }

    /**
     * Computes the Euclidean distance between two points.
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

    /**
     * Computes the distance between two points using SSE instructions.
     *
     * @param firstPoint The first point.
     * @param secondPoint The second point.
     * @param blocksCount Number of SSE registers that are needed to store one of the two points.
     * @return The distance between the two points.
     */
    static inline double distanceSse(const double *__restrict__ const firstPoint,
                                     const double *__restrict__ const secondPoint,
                                     std::size_t blocksCount) noexcept {

        // Initialize the partial sum of squares
        double sum = 0;

        for (std::size_t j = 0; j < blocksCount; j++) {
            // Load the next 2 coordinates of the first point into an SSE register
            __m128d dataI = _mm_load_pd(&(firstPoint[j * ParallelClustering::SSE_PACK_SIZE]));
            // Load the next 2 coordinates of the second point into an SSE register
            __m128d dataN = _mm_load_pd(&(secondPoint[j * ParallelClustering::SSE_PACK_SIZE]));

            // Compute the pairwise differences
            __m128d difference = _mm_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m128d square = _mm_mul_pd(difference, difference);

            // Update the partial sum
            sum += _mm_hadd_pd(square, square)[0];
        }

        // Compute the square root of the partial sum
        return sqrt(sum);
    }

    /**
     * Computes the distance between two points using SSE instructions. This implementation keeps
     * the partial sum in the registers instead of storing it into memory.
     *
     * @param firstPoint The first point.
     * @param secondPoint The second point.
     * @param blocksCount Number of SSE registers that are needed to store one of the two points.
     * @return The distance between the two points.
     */
    static inline double distanceSseOptimized(const double *__restrict__ const firstPoint,
                                              const double *__restrict__ const secondPoint,
                                              std::size_t blocksCount) noexcept {

        // Initialize the register that accumulates the partial sum of squares
        __m128d accumulator = _mm_setzero_pd();

        for (std::size_t j = 0; j < blocksCount; j++) {
            // Load the next 2 coordinates of the first point into an SSE register
            __m128d dataI = _mm_load_pd(&(firstPoint[j * ParallelClustering::SSE_PACK_SIZE]));
            // Load the next 2 coordinates of the second point into an SSE register
            __m128d dataN = _mm_load_pd(&(secondPoint[j * ParallelClustering::SSE_PACK_SIZE]));

            // Compute the pairwise differences
            __m128d difference = _mm_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m128d square = _mm_mul_pd(difference, difference);

            // Update the partial sum
            accumulator = _mm_add_pd(accumulator, square);
        }

        // Compute the final partial sum
        __m128d horizontalSum = _mm_hadd_pd(accumulator, accumulator);

        // Compute the square root of the final partial sum
        return sqrt(horizontalSum[0]);
    }

    /**
     * Computes the square of the distance between two points using SSE instructions. This
     * implementation keeps the partial sum in the registers instead of storing it into memory.
     *
     * @param firstPoint The first point.
     * @param secondPoint The second point.
     * @param blocksCount Number of SSE registers that are needed to store one of the two points.
     * @return The square of the distance between the two points.
     */
    static inline double distanceSseOptimizedNoSquareRoot(
            const double *__restrict__ const firstPoint,
            const double *__restrict__ const secondPoint,
            std::size_t blocksCount) noexcept {

        // Initialize the register that accumulates the partial sum of squares
        __m128d accumulator = _mm_setzero_pd();

        for (std::size_t j = 0; j < blocksCount; j++) {
            // Load the next 2 coordinates of the first point into an SSE register
            __m128d dataI = _mm_load_pd(&(firstPoint[j * ParallelClustering::SSE_PACK_SIZE]));
            // Load the next 2 coordinates of the second point into an SSE register
            __m128d dataN = _mm_load_pd(&(secondPoint[j * ParallelClustering::SSE_PACK_SIZE]));

            // Compute the pairwise differences
            __m128d difference = _mm_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m128d square = _mm_mul_pd(difference, difference);

            // Update the partial sum
            accumulator = _mm_add_pd(accumulator, square);
        }

        // Compute the final partial sum
        __m128d horizontalSum = _mm_hadd_pd(accumulator, accumulator);

        // Return the partial sum of squares
        return horizontalSum[0];
    }

    /**
     * Computes the distance between two points using AVX instructions.
     *
     * @param firstPoint The first point.
     * @param secondPoint The second point.
     * @param blocksCount Number of AVX registers that are needed to store one of the two points.
     * @return The distance between the two points.
     */
    static inline double distanceAvx(const double *__restrict__ const firstPoint,
                                     const double *__restrict__ const secondPoint,
                                     std::size_t blocksCount) noexcept {

        // Initialize the partial sum of squares
        double sum = 0;

        for (std::size_t j = 0; j < blocksCount; j++) {
            // Load the next 4 coordinates of the first point into an AVX register
            __m256d dataI = _mm256_load_pd(&(firstPoint[j * ParallelClustering::AVX_PACK_SIZE]));
            // Load the next 4 coordinates of the second point into an AVX register
            __m256d dataN = _mm256_load_pd(&(secondPoint[j * ParallelClustering::AVX_PACK_SIZE]));

            // Compute the pairwise differences
            __m256d difference = _mm256_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m256d square = _mm256_mul_pd(difference, difference);

            // Sum adjacent double values, i.e., it sums the first value with the second, and stores
            // the result in the upper 128 bits of the register, and then it sums the third value
            // with the fourth, and stores the result in the lower 128 bits of the register
            __m256d squaresSum = _mm256_hadd_pd(square, square);

            // Extract the upper 128 bits of the register
            __m128d highBits = _mm256_extractf128_pd(squaresSum, 1);
            // Extract the lower 128 bits of the register
            __m128d lowBits = _mm256_castpd256_pd128(squaresSum);
            // Sum the upper 128 bits to the lower 128 ones, and then update the partial sum
            sum += _mm_add_pd(highBits, lowBits)[0];
        }

        // Compute the square root of the partial sum
        return sqrt(sum);
    }

    /**
     * Computes the distance between two points using AVX instructions. This implementation keeps
     * the partial sum in the registers instead of storing it into memory.
     *
     * @param firstPoint The first point.
     * @param secondPoint The second point.
     * @param blocksCount Number of AVX registers that are needed to store one of the two points.
     * @return The distance between the two points.
     */
    static inline double distanceAvxOptimized(const double *__restrict__ const firstPoint,
                                              const double *__restrict__ const secondPoint,
                                              std::size_t blocksCount) noexcept {

        // Initialize the register that accumulates the partial sum of squares
        __m256d accumulator = _mm256_setzero_pd();

        for (std::size_t j = 0; j < blocksCount; j++) {
            // Load the next 4 coordinates of the first point into an AVX register
            __m256d dataI = _mm256_load_pd(&(firstPoint[j * ParallelClustering::AVX_PACK_SIZE]));
            // Load the next 4 coordinates of the second point into an AVX register
            __m256d dataN = _mm256_load_pd(&(secondPoint[j * ParallelClustering::AVX_PACK_SIZE]));

            // Compute the pairwise differences
            __m256d difference = _mm256_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m256d square = _mm256_mul_pd(difference, difference);

            // Update the partial sum
            accumulator = _mm256_add_pd(accumulator, square);
        }

        // Sum adjacent double values, i.e., it sums the first value with the second, and stores
        // the result in the upper 128 bits of the register, and then it sums the third value
        // with the fourth, and stores the result in the lower 128 bits of the register
        __m256d horizontalSum = _mm256_hadd_pd(accumulator, accumulator);

        // Extract the upper 128 bits of the register
        __m128d highBits = _mm256_extractf128_pd(horizontalSum, 1);
        // Extract the lower 128 bits of the register
        __m128d lowBits = _mm256_castpd256_pd128(horizontalSum);

        // Sum the upper 128 bits to the lower 128 ones, and then return the square root of the
        // resulting sum
        return sqrt(_mm_add_pd(highBits, lowBits)[0]);
    }

    /**
     * Computes the square the distance between two points using AVX instructions. This
     * implementation keeps the partial sum in the registers instead of storing it into memory.
     *
     * @param firstPoint The first point.
     * @param secondPoint The second point.
     * @param blocksCount Number of AVX registers that are needed to store one of the two points.
     * @return The square of the distance between the two points.
     */
    static inline double distanceAvxOptimizedNoSquareRoot(
            const double *__restrict__ const firstPoint,
            const double *__restrict__ const secondPoint,
            std::size_t blocksCount) noexcept {

        // Initialize the register that accumulates the partial sum of squares
        __m256d accumulator = _mm256_setzero_pd();

        for (std::size_t j = 0; j < blocksCount; j++) {
            // Load the next 4 coordinates of the first point into an AVX register
            __m256d dataI = _mm256_load_pd(&(firstPoint[j * ParallelClustering::AVX_PACK_SIZE]));
            // Load the next 4 coordinates of the second point into an AVX register
            __m256d dataN = _mm256_load_pd(&(secondPoint[j * ParallelClustering::AVX_PACK_SIZE]));

            // Compute the pairwise differences
            __m256d difference = _mm256_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m256d square = _mm256_mul_pd(difference, difference);

            // Update the partial sum
            accumulator = _mm256_add_pd(accumulator, square);
        }

        // Sum adjacent double values, i.e., it sums the first value with the second, and stores
        // the result in the upper 128 bits of the register, and then it sums the third value
        // with the fourth, and stores the result in the lower 128 bits of the register
        __m256d horizontalSum = _mm256_hadd_pd(accumulator, accumulator);

        // Extract the upper 128 bits of the register
        __m128d highBits = _mm256_extractf128_pd(horizontalSum, 1);
        // Extract the lower 128 bits of the register
        __m128d lowBits = _mm256_castpd256_pd128(horizontalSum);

        // Sum the upper 128 bits to the lower 128 ones, and then return the resulting sum
        return _mm_add_pd(highBits, lowBits)[0];
    }
};
}  // namespace cluster::parallel
#endif  // FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
