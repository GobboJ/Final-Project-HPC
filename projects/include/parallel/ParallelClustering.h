#ifndef FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
#define FINAL_PROJECT_HPC_PARALLELCLUSTERING_H

#include "Timer.h"
#include "../utils/Types.h"
#include "DistanceComputers.h"
#include "../utils/DataIteratorUtils.h"
#include "Logger.h"
#include "PiLambdaIteratorUtils.h"
#include <cmath>
#include <immintrin.h>
#include <limits>
#include <memory>
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
    using PiLambdaIteratorUtils = utils::PiLambdaIteratorUtils;

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
     * @param data Iterator over the set of data samples to cluster.
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
    template <DistanceComputers C, typename D, utils::PiIterator P, utils::LambdaIterator L>
    static void cluster(const D &data,
                        const std::size_t dataSamplesCount,
                        const std::size_t dimension,
                        P &piIterator,
                        L &lambdaIterator,
                        const std::size_t distanceComputationThreadsCount = 0,
                        const std::size_t stage4ThreadsCount = 0,
                        const std::size_t squareRootThreadsCount = 0) {

        Timer::start<0>();
        // Computes the number of SSE registers that are needed to store a data sample
        const std::size_t sseDimension =
                ParallelClustering::computeSseBlocksCount(dimension) * SSE_PACK_SIZE;
        // Computes the number of AVX registers that are needed to store a data sample
        const std::size_t avxDimension =
                ParallelClustering::computeAvxBlocksCount(dimension) * AVX_PACK_SIZE;

        std::size_t pointRealDimension;
        std::size_t stride;
        if constexpr (C == DistanceComputers::AVX || C == DistanceComputers::AVX_OPTIMIZED ||
                      C == DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT) {
            pointRealDimension = avxDimension;
            stride = avxDimension;
        } else if constexpr (C == DistanceComputers::SSE || C == DistanceComputers::SSE_OPTIMIZED ||
                             C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT) {
            pointRealDimension = sseDimension;
            stride = sseDimension;
        } else {
            pointRealDimension = dimension;
            stride = dimension;
        }

        // Initialize the part-row values
        auto *__restrict__ m = new double[dataSamplesCount];

        const auto piBegin = PiLambdaIteratorUtils::createEfficientIterator<std::size_t, P>(
                piIterator, "First element of pi");
        const auto lambdaBegin = PiLambdaIteratorUtils::createEfficientIterator<double, L>(
                lambdaIterator, "First element of lambda");

        // Iterator pointing to pi[i], with i ranging from 0 to dataSamplesCount-1.
        // Initially it points to pi[0], i.e., the first element of pi.
        auto currentPi = piBegin;
        // Iterator pointing to lambda[i], with i ranging from 0 to dataSamplesCount-1.
        // Initially it points to lambda[0], i.e., the first element of lambda.
        auto currentLambda = lambdaBegin;

        // Iterator pointing to the n-th element of the dataset.
        // Initially it points to the first element of the dataset.
        auto currentData = utils::DataIteratorUtils::createEfficientIterator(data, "Current data");
        auto startData =
                utils::DataIteratorUtils::createEfficientIterator(data, "First element of data");

        Timer::stop<0>();

        // Initialize the timers and start logging the console output, if requested
        // Timer::initTimers();
        Logger::startLoggingProgress<0, 1, 2, 3, 4, 5>(dataSamplesCount);

        /*******************************************************************************************
         * 1) Set pi(1) to 0, lambda(1) to infinity
         ******************************************************************************************/
        Timer::start<1>();

        initializeNewPoint<P, L>(currentPi, currentLambda, 0);
        Timer::stop<1>();

        Timer::start<4>();
        // No more operations need to be performed for the first point, so move to the second
        // element of the dataset
        utils::DataIteratorUtils::moveNext<D>(currentData, stride);

        Timer::stop<4>();

        double *distanceEnd = &(m[1]);

        // Perform the clustering algorithm for all the remaining data samples
        for (std::size_t n = 1; n < dataSamplesCount; n++) {

            /***************************************************************************************
             * 1) Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
             **************************************************************************************/
            Timer::start<1>();

            initializeNewPoint<P, L>(currentPi, currentLambda, n);

            Timer::stop<1>();

            /***************************************************************************************
             * 2) Set M(i) to d(i, n + 1) for i = 1,..,n
             **************************************************************************************/
            Timer::start<2>();

            // Value of the n-th value of the dataset
            const double *__restrict__ const currentDataN =
                    utils::DataIteratorUtils::getCurrentElement<D>(currentData);
            // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
            const double *currentDataNEnd = currentDataN + pointRealDimension;

            computeDistances<C, D>(n,
                                   startData,
                                   stride,
                                   currentDataN,
                                   currentDataNEnd,
                                   m,
                                   distanceComputationThreadsCount);

            Timer::stop<2>();

            /***************************************************************************************
             * 3) For i from 1 to n
             **************************************************************************************/
            Timer::start<3>();

            // Iterator over the first n-1 values of m

            addNewPoint<P, L>(piBegin, lambdaBegin, m, distanceEnd, n);

            Timer::stop<3>();

            /***************************************************************************************
             * 4) For i from 1 to n
             **************************************************************************************/
            Timer::start<4>();

            fixStructure(piBegin, lambdaBegin, n, stage4ThreadsCount);
            // Move to the next data sample
            utils::DataIteratorUtils::moveNext<D>(currentData, stride);
            ++distanceEnd;
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
#pragma omp parallel for default(none) shared(lambdaBegin, dataSamplesCount) \
        num_threads(squareRootThreadsCount) if (S5)
            for (std::size_t i = 0; i < dataSamplesCount - 1; i++) {
                double &lambdaToModify =
                        utils::PiLambdaIteratorUtils::getElementAt<double, L>(lambdaBegin, i);
                lambdaToModify = sqrt(lambdaToModify);
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
    static void checkAlignment() {
        /*
         * // Check alignment
if constexpr (C != DistanceComputers::CLASSICAL) {
    constexpr std::size_t pointSize =
            (C == DistanceComputers::SSE || C == DistanceComputers::SSE_OPTIMIZED ||
             C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT)
                    ? SSE_PACK_SIZE
                    : AVX_PACK_SIZE;
    constexpr std::size_t alignment = pointSize * sizeof(double);

D iterator = dataIterator;
for (std::size_t i = 0; i < dataSamplesCount; i++) {
    std::size_t remainingSize = 1;
    const void *pointer = const_cast<double *>(*iterator);
    if (static_cast<uintptr_t>(pointer) % alignment == 0) {
        std::string errorMessage{"The element"};
        errorMessage += " ";
        errorMessage += std::to_string(i);
        errorMessage += " is not";
        errorMessage += " ";
        errorMessage += std::to_string(alignment);
        errorMessage += "-bytes aligned";
        throw std::invalid_argument(errorMessage);
    }
    for (int j = 0; j < pointSize; j++) {
        ++iterator;
    }
}
}
         */
    }

    template <typename P, typename L, typename EP, typename EL>
    static inline void initializeNewPoint(EP &currentPi, EL &currentLambda, std::size_t n) {

        // Set pi[n] to n
        PiLambdaIteratorUtils::getCurrentElement<std::size_t, P>(currentPi) = n;
        PiLambdaIteratorUtils::moveNext<std::size_t, P>(currentPi);

        // Set lambda[n] to infinity
        PiLambdaIteratorUtils::getCurrentElement<double, L>(currentLambda) =
                std::numeric_limits<double>::infinity();
        PiLambdaIteratorUtils::moveNext<double, L>(currentLambda);
    }

    template <DistanceComputers C, typename D, typename ED>
    static inline void computeDistances(std::size_t n,
                                        const ED &dataIterator,
                                        const std::size_t stride,
                                        const double *__restrict__ currentDataN,
                                        const double *__restrict__ currentDataNEnd,
                                        double *__restrict__ const m,
                                        std::size_t distanceComputationThreadsCount) {

        // Compute the distance between the n-th element of the dataset and all the
// already-processed ones
#pragma omp parallel for default(none)                                    \
        shared(n, m, dataIterator, currentDataN, currentDataNEnd, stride) \
                num_threads(distanceComputationThreadsCount) if (S2)
        for (std::size_t i = 0; i <= n - 1; i++) {
            const double *const __restrict__ element =
                    utils::DataIteratorUtils::getElementAt<D>(dataIterator, i, stride);
            // Compute the distance by using the requested algorithm
            if constexpr (C == DistanceComputers::CLASSICAL) {
                m[i] = ParallelClustering::distance(currentDataN, currentDataNEnd, element);
            } else if constexpr (C == DistanceComputers::SSE) {
                m[i] = ParallelClustering::distanceSse(currentDataN, currentDataNEnd, element);
            } else if constexpr (C == DistanceComputers::AVX) {
                m[i] = ParallelClustering::distanceAvx(currentDataN, currentDataNEnd, element);
            } else if constexpr (C == DistanceComputers::SSE_OPTIMIZED) {
                m[i] = ParallelClustering::distanceSseOptimized(
                        currentDataN, currentDataNEnd, element);
            } else if constexpr (C == DistanceComputers::AVX_OPTIMIZED) {
                m[i] = ParallelClustering::distanceAvxOptimized(
                        currentDataN, currentDataNEnd, element);
            } else if constexpr (C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT) {
                m[i] = ParallelClustering::distanceSseOptimizedNoSquareRoot(
                        currentDataN, currentDataNEnd, element);
            } else if constexpr (C == DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT) {
                m[i] = ParallelClustering::distanceAvxOptimizedNoSquareRoot(
                        currentDataN, currentDataNEnd, element);
            } else {
                // https://stackoverflow.com/a/53945549
                static_assert(always_false<ED>,
                              "The specified distance computer is not supported.");
            }
        }
    }

    template <typename P, typename L, typename EP, typename EL>
    static inline void addNewPoint(EP currentPi,
                                   EL currentLambda,
                                   double *distanceBegin,
                                   const double *const distanceEnd,
                                   std::size_t n) {

        double *mBegin = distanceBegin;
        double *distanceIterator = distanceBegin;

        while (distanceIterator != distanceEnd) {
            // Reference to pi[i]
            std::size_t &piI = PiLambdaIteratorUtils::getCurrentElement<std::size_t, P>(currentPi);
            // Reference to lambda[i]
            double &lambdaI = PiLambdaIteratorUtils::getCurrentElement<double, L>(currentLambda);
            // Value of m[i]
            const double currentDistance = *distanceIterator;
            // Reference to m[pi[i]]
            double &mPiI = mBegin[piI];

            /***********************************************************************************
             * if lambda(i) >= M(i)
             **********************************************************************************/
            if (lambdaI >= currentDistance) {
                /*******************************************************************************
                 * set M(pi(i)) to min { M(pi(i)), lambda(i) }
                 ******************************************************************************/
                mPiI = std::min(mPiI, lambdaI);

                /*******************************************************************************
                 * set lambda(i) to M(i)
                 ******************************************************************************/
                lambdaI = currentDistance;

                /*******************************************************************************
                 * set pi(i) to n + 1
                 ******************************************************************************/
                piI = n;
            } else {  // if lambda(i) < M(i)
                /*******************************************************************************
                 * set M(pi(i)) to min { M(pi(i)), M(i) }
                 ******************************************************************************/
                mPiI = std::min(mPiI, currentDistance);
            }
            // Move to the next element
            PiLambdaIteratorUtils::moveNext<std::size_t, P>(currentPi);
            PiLambdaIteratorUtils::moveNext<double, L>(currentLambda);
            ++distanceIterator;
        }
    }

    template <typename EP, typename EL>
    static inline void fixStructure(const EP &piIterator,
                                    const EL &lambdaIterator,
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
    static inline double distance(const double *__restrict__ const firstPointBegin,
                                  const double *__restrict__ const firstPointEnd,
                                  const double *__restrict__ const secondPointBegin) noexcept {

        double sum = 0;
        const double *__restrict__ firstPoint = firstPointBegin;
        const double *__restrict__ secondPoint = secondPointBegin;

        while (firstPoint != firstPointEnd) {
            sum += pow(*firstPoint - *secondPoint, 2.0);
            firstPoint++;
            secondPoint++;
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
    static inline double distanceSse(const double *__restrict__ const firstPointBegin,
                                     const double *__restrict__ const firstPointEnd,
                                     const double *__restrict__ const secondPointBegin) noexcept {

        // Initialize the partial sum of squares
        double sum = 0;

        const double *__restrict__ firstPoint = firstPointBegin;
        const double *__restrict__ secondPoint = secondPointBegin;

        while (firstPoint != firstPointEnd) {
            // Load the next 2 coordinates of the first point into an SSE register
            __m128d dataI = _mm_load_pd(firstPoint);
            // Load the next 2 coordinates of the second point into an SSE register
            __m128d dataN = _mm_load_pd(secondPoint);

            // Compute the pairwise differences
            __m128d difference = _mm_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m128d square = _mm_mul_pd(difference, difference);

            // Update the partial sum
            sum += _mm_hadd_pd(square, square)[0];

            firstPoint += SSE_PACK_SIZE;
            secondPoint += SSE_PACK_SIZE;
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
    static inline double distanceSseOptimized(
            const double *__restrict__ const firstPointBegin,
            const double *__restrict__ const firstPointEnd,
            const double *__restrict__ const secondPointBegin) noexcept {

        // Initialize the register that accumulates the partial sum of squares
        __m128d accumulator = _mm_setzero_pd();

        const double *__restrict__ firstPoint = firstPointBegin;
        const double *__restrict__ secondPoint = secondPointBegin;

        while (firstPoint != firstPointEnd) {
            // Load the next 2 coordinates of the first point into an SSE register
            __m128d dataI = _mm_load_pd(firstPoint);
            // Load the next 2 coordinates of the second point into an SSE register
            __m128d dataN = _mm_load_pd(secondPoint);

            // Compute the pairwise differences
            __m128d difference = _mm_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m128d square = _mm_mul_pd(difference, difference);

            // Update the partial sum
            accumulator = _mm_add_pd(accumulator, square);

            firstPoint += SSE_PACK_SIZE;
            secondPoint += SSE_PACK_SIZE;
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
            const double *__restrict__ const firstPointBegin,
            const double *__restrict__ const firstPointEnd,
            const double *__restrict__ const secondPointBegin) noexcept {

        // Initialize the register that accumulates the partial sum of squares
        __m128d accumulator = _mm_setzero_pd();

        const double *__restrict__ firstPoint = firstPointBegin;
        const double *__restrict__ secondPoint = secondPointBegin;

        while (firstPoint != firstPointEnd) {
            // Load the next 2 coordinates of the first point into an SSE register
            __m128d dataI = _mm_load_pd(firstPoint);
            // Load the next 2 coordinates of the second point into an SSE register
            __m128d dataN = _mm_load_pd(secondPoint);

            // Compute the pairwise differences
            __m128d difference = _mm_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m128d square = _mm_mul_pd(difference, difference);

            // Update the partial sum
            accumulator = _mm_add_pd(accumulator, square);

            firstPoint += SSE_PACK_SIZE;
            secondPoint += SSE_PACK_SIZE;
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
    static inline double distanceAvx(const double *__restrict__ const firstPointBegin,
                                     const double *__restrict__ const firstPointEnd,
                                     const double *__restrict__ const secondPointBegin) noexcept {

        // Initialize the partial sum of squares
        double sum = 0;

        const double *__restrict__ firstPoint = firstPointBegin;
        const double *__restrict__ secondPoint = secondPointBegin;

        while (firstPoint != firstPointEnd) {
            // Load the next 4 coordinates of the first point into an AVX register
            __m256d dataI = _mm256_load_pd(firstPoint);
            // Load the next 4 coordinates of the second point into an AVX register
            __m256d dataN = _mm256_load_pd(secondPoint);

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

            firstPoint += AVX_PACK_SIZE;
            secondPoint += AVX_PACK_SIZE;
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
    static inline double distanceAvxOptimized(
            const double *__restrict__ const firstPointBegin,
            const double *__restrict__ const firstPointEnd,
            const double *__restrict__ const secondPointBegin) noexcept {

        // Initialize the register that accumulates the partial sum of squares
        __m256d accumulator = _mm256_setzero_pd();

        const double *__restrict__ firstPoint = firstPointBegin;
        const double *__restrict__ secondPoint = secondPointBegin;

        while (firstPoint != firstPointEnd) {
            // Load the next 4 coordinates of the first point into an AVX register
            __m256d dataI = _mm256_load_pd(firstPoint);
            // Load the next 4 coordinates of the second point into an AVX register
            __m256d dataN = _mm256_load_pd(secondPoint);

            // Compute the pairwise differences
            __m256d difference = _mm256_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m256d square = _mm256_mul_pd(difference, difference);

            // Update the partial sum
            accumulator = _mm256_add_pd(accumulator, square);

            firstPoint += AVX_PACK_SIZE;
            secondPoint += AVX_PACK_SIZE;
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
            const double *__restrict__ const firstPointBegin,
            const double *__restrict__ const firstPointEnd,
            const double *__restrict__ const secondPointBegin) noexcept {

        // Initialize the register that accumulates the partial sum of squares
        __m256d accumulator = _mm256_setzero_pd();

        const double *__restrict__ firstPoint = firstPointBegin;
        const double *__restrict__ secondPoint = secondPointBegin;

        while (firstPoint != firstPointEnd) {
            // Load the next 4 coordinates of the first point into an AVX register
            __m256d dataI = _mm256_load_pd(firstPoint);
            // Load the next 4 coordinates of the second point into an AVX register
            __m256d dataN = _mm256_load_pd(secondPoint);

            // Compute the pairwise differences
            __m256d difference = _mm256_sub_pd(dataI, dataN);
            // Compute the square of two differences
            __m256d square = _mm256_mul_pd(difference, difference);

            // Update the partial sum
            accumulator = _mm256_add_pd(accumulator, square);

            firstPoint += AVX_PACK_SIZE;
            secondPoint += AVX_PACK_SIZE;
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
