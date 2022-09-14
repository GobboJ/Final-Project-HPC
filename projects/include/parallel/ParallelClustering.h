#ifndef FINAL_PROJECT_HPC_PARALLELCLUSTERING_H
#define FINAL_PROJECT_HPC_PARALLELCLUSTERING_H

#include "../utils/DataIteratorUtils.h"
#include "../utils/Types.h"
#include "DistanceComputers.h"
#include "Logger.h"
#include "PiLambdaIteratorUtils.h"
#include "Timer.h"
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
 * customize its behaviour.<br>
 * Moreover, this method uses the first 6 timers offered by the <code>Timer</code> class to measure
 * the time taken by each step of the algorithm.
 *
 * @tparam PD <code>true</code> if the clustering method should parallelize the computation of the
 * distance using threads, <code>false</code> otherwise.
 * @tparam PF <code>true</code> if the clustering method should parallelize using threads the
 * computation of the structural fix after a new point is added to the dendrogram,
 * <code>false</code> otherwise.
 * @tparam PS <code>true</code> if the clustering method should parallelize the computation of the
 * square roots using threads, <code>false</code> otherwise. This template argument take effect
 * only if the distance computation algorithm is one of:
 * <ul>
 *      <li><code>DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT</code>;</li>
 *      <li><code>DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT</code>.</li>
 * </ul>
 * In all the other cases, it has no effect.
 * @tparam A <code>true</code> if the clustering method should check for the memory alignment of the
 * data samples, <code>false</code> otherwise. This template argument take effect only if the
 * distance computation algorithm is not <code>DistanceComputers::CLASSICAL</code>.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.6 2022-08-16
 * @since 1.0
 */
template <bool PD = true, bool PF = false, bool PS = false, bool A = false>
class ParallelClustering {

    // Imports
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

public:
    /**
     * Size of the SSE pack.
     */
    static const constexpr std::size_t SSE_PACK_SIZE = 2;

    /**
     * Size of the AVX pack.
     */
    static const constexpr std::size_t AVX_PACK_SIZE = 4;

    /**
     * Parallel implementation of the clustering algorithm.
     *
     * @tparam C Enumeration constant specifying the algorithm to use to compute the distance
     * between two data samples.
     * @tparam D Type of the data structure/iterator holding the data samples to cluster.
     * @tparam P Type of the data structure/iterator holding the <code>pi</code> values.
     * @tparam L Type of the data structure/iterator holding the <code>lambda</code> values.
     * @param data Data structure/iterator holding the data samples to cluster.
     * @param dataSamplesCount Number of data samples.
     * @param dimension Number of attributes of each sample.
     * @param pi Data structure/iterator holding the <code>pi</code> values. This data structure
     * <b>MUST BE</b> big enough to contain all the <code>pi</code> values, which are exactly
     * <code>dataSamplesCount</code>.
     * @param lambda Data structure/iterator holding the <code>lambda</code> values. This data
     * structure <b>MUST BE</b> big enough to contain all the <code>lambda</code> values, which are
     * exactly <code>dataSamplesCount</code>.
     * @param distanceComputationThreadsCount Number of threads to use to parallelize the
     * computation of the distance between the data samples. If not specified, or if <code>0</code>
     * is specified, then this method uses the default number of threads computed by OpenMP.<br>
     * This parameter takes effect only if <code>PD</code> is <code>true</code>.
     * @param structuralFixThreadsCount Number of threads to use to parallelize the computation of
     * structural fix after a new data sample is added to the dendrogram. If not specified, or if
     * <code>0</code> is specified, then this method uses the default number of threads computed by
     * OpenMP.<br> This parameter takes effect only if <code>PF</code> is <code>true</code>.
     * @param squareRootThreadsCount Number of threads to use to parallelize the computation of
     * square roots of the distances. If not specified, or if <code>0</code> is specified, then this
     * method uses the default number of threads computed by OpenMP.<br> This parameter takes effect
     * only if <code>PS</code> is <code>true</code>, and if the distance computation algorithm is
     * either <code>DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT</code> or
     * <code>DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT</code>.
     * @throws std::invalid_argument If the data samples alignment check has been requested and one
     * of the data samples in not correctly aligned.
     */
    template <DistanceComputers C,
              utils::ParallelDataIterator D,
              utils::PiIterator P,
              utils::LambdaIterator L>
    static void cluster(const D &data,
                        const std::size_t dataSamplesCount,
                        const std::size_t dimension,
                        P &pi,
                        L &lambda,
                        const std::size_t distanceComputationThreadsCount = 0,
                        const std::size_t structuralFixThreadsCount = 0,
                        const std::size_t squareRootThreadsCount = 0) {

        Timer::start<0>();

        // Computes the stride of the data samples, i.e., the number of doubles between the first
        // attribute of a data sample and the first attribute of the following sample if they are
        // contiguous in memory
        // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
        std::size_t stride;
        if constexpr (C == DistanceComputers::AVX || C == DistanceComputers::AVX_OPTIMIZED ||
                      C == DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT) {
            stride = ParallelClustering::computeAvxDimension(dimension);
        } else if constexpr (C == DistanceComputers::SSE || C == DistanceComputers::SSE_OPTIMIZED ||
                             C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT) {
            stride = ParallelClustering::computeSseDimension(dimension);
        } else {
            stride = dimension;
        }

        // Array containing the part-row values
        auto *const __restrict__ m = new double[dataSamplesCount];

        // Efficient iterator pointing to the first element of pi
        const auto piBegin = PiLambdaIteratorUtils::createEfficientIterator<std::size_t, P>(
                pi, "First element of pi");
        // Efficient iterator pointing to the first element of lambda
        const auto lambdaBegin = PiLambdaIteratorUtils::createEfficientIterator<double, L>(
                lambda, "First element of lambda");

        // Efficient iterator pointing to the n-th element of pi
        auto currentPi = piBegin;
        // Efficient iterator pointing to the n-th element of lambda
        auto currentLambda = lambdaBegin;

        // Efficient iterator pointing to the n-th element of the dataset.
        auto currentData = utils::DataIteratorUtils::createEfficientIterator(data, "Current data");
        // Efficient iterator pointing to the first element of the dataset
        auto dataBegin =
                utils::DataIteratorUtils::createEfficientIterator(data, "First element of data");

        Timer::stop<0>();

        // Check the alignment of the first data sample, if requested
        checkAlignment<C>(0, utils::DataIteratorUtils::getCurrentSample<D>(currentData));

        // Log the initial progress
        Logger::startLoggingProgress<0, 1, 2, 3, 4, 5>(dataSamplesCount);

        Timer::start<1>();
        // **** 1) Set pi(1) to 0, lambda(1) to infinity ****
        initializeNewPoint<P, L>(currentPi, currentLambda, 0);
        Timer::stop<1>();

        Timer::start<0>();
        // No more operations need to be performed for the first data sample, so move to the second
        utils::DataIteratorUtils::moveNext<D>(currentData, stride);

        // Useful pointer pointing to the element following the last element of m containing a valid
        // distance
        const double *distanceEnd = &(m[1]);

        // Perform the clustering algorithm for all the remaining data samples
        for (std::size_t n = 1; n < dataSamplesCount; n++) {
            Timer::stop<0>();
            // Check the alignment of the data sample, if requested
            checkAlignment<C>(n, utils::DataIteratorUtils::getCurrentSample<D>(currentData));

            // Log the progress every 1000 samples
            Logger::updateProgress<1000, 0, 1, 2, 3, 4, 5>(n, dataSamplesCount);

            Timer::start<1>();
            // **** 1) Set pi(n + 1) to n + 1, lambda(n + 1) to infinity ****
            initializeNewPoint<P, L>(currentPi, currentLambda, n);
            Timer::stop<1>();

            Timer::start<2>();
            // Pointer to the n-th data sample of the dataset
            const double *__restrict__ const currentDataN =
                    utils::DataIteratorUtils::getCurrentSample<D>(currentData);
            // Pointer to the first byte after the n-th data sample of the dataset
            const double *__restrict__ const currentDataNEnd = currentDataN + stride;

            // **** 2) Set M(i) to d(i, n + 1) for i = 1,..,n ****
            computeDistances<C, D>(n,
                                   dataBegin,
                                   stride,
                                   currentDataN,
                                   currentDataNEnd,
                                   m,
                                   distanceComputationThreadsCount);
            Timer::stop<2>();

            Timer::start<3>();
            // **** 3) For i from 1 to n ****
            addNewPoint<P, L>(piBegin, lambdaBegin, m, distanceEnd, n);
            Timer::stop<3>();

            Timer::start<4>();
            //***  4) For i from 1 to n ****
            fixStructure<P, L>(piBegin, lambdaBegin, n, structuralFixThreadsCount);
            Timer::stop<4>();

            Timer::start<0>();
            // Move to the next data sample
            utils::DataIteratorUtils::moveNext<D>(currentData, stride);
            ++distanceEnd;
        }
        Timer::stop<6>();

        // Compute the square roots, if until now the algorithm has used the squares of the
        // distances
        Timer::start<5>();
        if constexpr (C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT ||
                      C == DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT) {

            // Compute the square root of all the values in lambda
#pragma omp parallel for default(none) shared(lambdaBegin, dataSamplesCount) \
        num_threads(squareRootThreadsCount) if (PS)
            for (std::size_t i = 0; i < dataSamplesCount - 1; i++) {
                double &lambdaToModify =
                        utils::PiLambdaIteratorUtils::getElementAt<double, L>(lambdaBegin, i);
                lambdaToModify = sqrt(lambdaToModify);
            }
        }
        Timer::stop<5>();

        Timer::start<0>();
        // Deallocate m, since it is not needed anymore
        delete[] m;
        Timer::stop<0>();

        // Log the final progress
        Logger::updateProgress<1, 0, 1, 2, 3, 4, 5>(dataSamplesCount, dataSamplesCount);
    }

    /**
     * Computes the dimension the data sample must have if the distance computer uses SSE
     * instructions.
     *
     * @param dimension Dimension of the data sample.
     * @return The computed dimension.
     */
    static inline std::size_t computeSseDimension(const std::size_t dimension) {

        return (1 + (((dimension - 1) / SSE_PACK_SIZE))) * SSE_PACK_SIZE;
    }

    /**
     * Computes the dimension the data sample must have if the distance computer uses AVX
     * instructions.
     *
     * @param dimension Dimension of the data sample.
     * @return The computed dimension.
     */
    static inline std::size_t computeAvxDimension(const std::size_t dimension) {

        return (1 + (((dimension - 1) / AVX_PACK_SIZE))) * AVX_PACK_SIZE;
    }

private:
    /**
     * Utility method that checks if the specified data sample is correctly aligned. If it is not,
     * then an exception is thrown.<br>
     * This method takes effect only if <code>A</code> is <code>true</code> and only if the
     * specified distance computer is not <code>DistanceComputers::CLASSICAL</code>.
     *
     * @tparam C Distance computer to use to compute the distances between the data samples. Based
     * on this parameter, this method computes the correct alignment the data samples must have.
     * @param n Index of the sample to check.
     * @param currentDataSample Data of the sample to check.
     * @throws std::invalid_argument If the sample is not correctly aligned.
     */
    template <DistanceComputers C>
    static inline void checkAlignment(const std::size_t n, const double *const currentDataSample) {

        if constexpr (A && C != DistanceComputers::CLASSICAL) {
            // Compute the alignment
            const constexpr std::size_t packSize =
                    (C == DistanceComputers::SSE || C == DistanceComputers::SSE_OPTIMIZED ||
                     C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT)
                            ? SSE_PACK_SIZE
                            : AVX_PACK_SIZE;
            const constexpr std::size_t alignment = packSize * sizeof(double);

            // Check the pointer alignment
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            const auto pointer = reinterpret_cast<uintptr_t>(currentDataSample);
            if (pointer % alignment != 0) {
                // Compose the message of the exception
                std::string errorMessage{"The data sample"};
                errorMessage += " ";
                errorMessage += std::to_string(n);
                errorMessage += " is not";
                errorMessage += " ";
                errorMessage += std::to_string(alignment);
                errorMessage += "-bytes aligned";
                // Throw the exception signalling that the sample is not correctly aligned
                throw std::invalid_argument(errorMessage);
            }
        }
    }

    /**
     * Initialize the value of <code>pi</code> and <code>lambda</code> for a new point.<br>
     * This method takes care of advancing the specified efficient iterators to the next element of
     * <code>pi</code> and <code>lambda</code>.
     *
     * @tparam P Type of the data structure/iterator holding <code>pi</code>.
     * @tparam L Type of the data structure/iterator holding <code>lambda</code>.
     * @tparam EP Type of the efficient iterator iterating over <code>pi</code>.
     * @tparam EL Type of the efficient iterator iterating over <code>lambda</code>.
     * @param currentPi Efficient iterator pointing to the current element of <code>pi</code>.
     * @param currentLambda Efficient iterator pointing to the current element of
     * <code>lambda</code>.
     * @param n Index of the new point to add.
     */
    template <typename P, typename L, typename EP, typename EL>
    static inline void initializeNewPoint(EP &currentPi, EL &currentLambda, const std::size_t n) {

        // **** Set pi[n] to n ****
        PiLambdaIteratorUtils::getCurrentElement<std::size_t, P>(currentPi) = n;
        PiLambdaIteratorUtils::moveNext<std::size_t, P>(currentPi);

        // **** Set lambda[n] to infinity ****
        PiLambdaIteratorUtils::getCurrentElement<double, L>(currentLambda) =
                std::numeric_limits<double>::infinity();
        PiLambdaIteratorUtils::moveNext<double, L>(currentLambda);
    }

    /**
     * Computes the distances between the point to add and all the points that have already
     * been added to the dendrogram.
     *
     * @tparam C Distance computer to use to compute the distances.
     * @tparam D Type of the data structure/iterator holding the data samples to cluster.
     * @tparam ED Type of the efficient iterator iterating over the data structure holding the data
     * samples to cluster.
     * @param n Index of the point to add.
     * @param dataBegin Efficient iterator pointing to the first data sample.
     * @param stride Number of <code>double</code>s between the first attribute of a data samples
     * and the first attribute of the following sample. This parameter is ignored if the data
     * samples are not contiguous in memory.
     * @param currentDataN Pointer to the first attribute of the point to add.
     * @param currentDataNEnd Pointer to the attribute following the last attribute of the point to
     * add. This value acts as a placeholder, and it is used to identify the end of the data sample.
     * @param m Array containing the part row values this method will initialize.
     * @param distanceComputationThreadsCount Number of threads to use to compute the distances in
     * parallel. This parameter is ignored if <code>PD</code> is <code>false</code>.
     */
    template <DistanceComputers C, typename D, typename ED>
    static inline void computeDistances(const std::size_t n,
                                        const ED &dataBegin,
                                        const std::size_t stride,
                                        const double *const __restrict__ currentDataN,
                                        const double *const __restrict__ currentDataNEnd,
                                        double *__restrict__ const m,
                                        const std::size_t distanceComputationThreadsCount) {

        // Compute the distance between the n-th element of the dataset and all the
// already-processed ones
#pragma omp parallel for default(none)                                 \
        shared(n, m, dataBegin, currentDataN, currentDataNEnd, stride) \
                num_threads(distanceComputationThreadsCount) if (PD)
        for (std::size_t i = 0; i <= n - 1; i++) {
            // Extract the i-th data sample
            const double *const __restrict__ dataSample =
                    utils::DataIteratorUtils::getSampleAt<D>(dataBegin, i, stride);

            // Compute the distance between the newly added point and the extracted data sample by
            // using the requested algorithm
            if constexpr (C == DistanceComputers::CLASSICAL) {
                m[i] = ParallelClustering::distance(currentDataN, currentDataNEnd, dataSample);
            } else if constexpr (C == DistanceComputers::SSE) {
                m[i] = ParallelClustering::distanceSse(currentDataN, currentDataNEnd, dataSample);
            } else if constexpr (C == DistanceComputers::AVX) {
                m[i] = ParallelClustering::distanceAvx(currentDataN, currentDataNEnd, dataSample);
            } else if constexpr (C == DistanceComputers::SSE_OPTIMIZED) {
                m[i] = ParallelClustering::distanceSseOptimized(
                        currentDataN, currentDataNEnd, dataSample);
            } else if constexpr (C == DistanceComputers::AVX_OPTIMIZED) {
                m[i] = ParallelClustering::distanceAvxOptimized(
                        currentDataN, currentDataNEnd, dataSample);
            } else if constexpr (C == DistanceComputers::SSE_OPTIMIZED_NO_SQUARE_ROOT) {
                m[i] = ParallelClustering::distanceSseOptimizedNoSquareRoot(
                        currentDataN, currentDataNEnd, dataSample);
            } else if constexpr (C == DistanceComputers::AVX_OPTIMIZED_NO_SQUARE_ROOT) {
                m[i] = ParallelClustering::distanceAvxOptimizedNoSquareRoot(
                        currentDataN, currentDataNEnd, dataSample);
            } else {
                static_assert(always_false<ED>,
                              "The specified distance computer is not supported.");
            }
        }
    }

    /**
     * Adds to the dendrogram the new point.
     *
     * @tparam P Type of the data structure/iterator holding <code>pi</code>.
     * @tparam L Type of the data structure/iterator holding <code>lambda</code>.
     * @tparam EP Type of the efficient iterator iterating over <code>pi</code>.
     * @tparam EL Type of the efficient iterator iterating over <code>lambda</code>.
     * @param piBegin Efficient iterator pointing to the first element of <code>pi</code>.
     * @param lambdaBegin Efficient iterator pointing to the first element of <code>lambda</code>.
     * @param mBegin Pointer pointing to the first element of <code>m</code>.
     * @param mEnd Pointer pointing to the element <code>m[n]</code>. This value acts as a
     * placeholder, and it is used to identify the last element of <code>m</code> that contains a
     * valid distance.
     * @param n Index of the point to add.
     */
    template <typename P, typename L, typename EP, typename EL>
    static inline void addNewPoint(EP piBegin,
                                   EL lambdaBegin,
                                   double *__restrict__ const mBegin,
                                   const double *__restrict__ const mEnd,
                                   const std::size_t n) {

        // Pointer used as iterator to iterate over m
        const double *distanceIterator = mBegin;

        // Iterate all the valid distances in m
        while (distanceIterator != mEnd) {
            // Reference to pi[i]
            std::size_t &piI = PiLambdaIteratorUtils::getCurrentElement<std::size_t, P>(piBegin);
            // Reference to lambda[i]
            double &lambdaI = PiLambdaIteratorUtils::getCurrentElement<double, L>(lambdaBegin);
            // Value of m[i]
            const double currentDistance = *distanceIterator;
            // Reference to m[pi[i]]
            double &mPiI = mBegin[piI];

            // **** if lambda(i) >= M(i) ****
            if (lambdaI >= currentDistance) {
                // **** set M(pi(i)) to min { M(pi(i)), lambda(i) } ****
                mPiI = std::min(mPiI, lambdaI);
                // **** set lambda(i) to M(i) ****
                lambdaI = currentDistance;

                // **** set pi(i) to n + 1 ****
                piI = n;
            } else {  // **** if lambda(i) < M(i) ****
                // **** set M(pi(i)) to min { M(pi(i)), M(i) } ****
                mPiI = std::min(mPiI, currentDistance);
            }
            // Move to the next element
            PiLambdaIteratorUtils::moveNext<std::size_t, P>(piBegin);
            PiLambdaIteratorUtils::moveNext<double, L>(lambdaBegin);
            ++distanceIterator;
        }
    }

    /**
     * Fixes the structure of the dendrogram by updating the representative of a point if its
     * representative connects to the newly added point before connecting to the point.
     *
     * @tparam P Type of the data structure/iterator holding <code>pi</code>.
     * @tparam L Type of the data structure/iterator holding <code>lambda</code>.
     * @tparam EP Type of the efficient iterator iterating over <code>pi</code>.
     * @tparam EL Type of the efficient iterator iterating over <code>lambda</code>.
     * @param piBegin Efficient iterator pointing to the first element of <code>pi</code>.
     * @param lambdaBegin Efficient iterator pointing to the first element of <code>lambda</code>.
     * @param n Index of the newly added point.
     * @param structuralFixThreadsCount Number of threads to use to execute the structural fix in
     * parallel.
     */
    template <typename P, typename L, typename EP, typename EL>
    static inline void fixStructure(const EP &piBegin,
                                    const EL &lambdaBegin,
                                    const std::size_t n,
                                    const std::size_t structuralFixThreadsCount) {

        // Loop over all the previously added points
#pragma omp parallel for default(none) shared(n, piBegin, lambdaBegin) \
        num_threads(structuralFixThreadsCount) if (PF)
        for (std::size_t i = 0; i <= n - 1; i++) {
            // Reference to pi[i]
            std::size_t &piI = PiLambdaIteratorUtils::getElementAt<std::size_t, P>(piBegin, i);
            // Value of lambda[i]
            const double lambdaI = PiLambdaIteratorUtils::getElementAt<double, L>(lambdaBegin, i);
            // Value of lambda[pi[i]]
            const double lambdaPiI =
                    PiLambdaIteratorUtils::getElementAt<double, L>(lambdaBegin, piI);

            // **** if lambda(i) >= lambda(pi(i)) ****
            if (lambdaI >= lambdaPiI) {
                // **** set pi(i) to n + 1 ****
                piI = n;
            }
        }
    }

    /**
     * Computes the Euclidean distance between two points.
     *
     * @param firstPointBegin Pointer to the first attribute of the first data sample.
     * @param firstPointEnd Pointer to the attribute following the last attribute of the first
     * data sample. This value acts as a placeholder, and it is used to identify the end of the
     * first point.
     * @param secondPointBegin Pointer to the first attribute of the second data sample.
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
     * @param firstPointBegin Pointer to the first attribute of the first data sample.
     * @param firstPointEnd Pointer to the attribute following the last attribute of the first
     * data sample. This value acts as a placeholder, and it is used to identify the end of the
     * first point.
     * @param secondPointBegin Pointer to the first attribute of the second data sample.
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
     * @param firstPointBegin Pointer to the first attribute of the first data sample.
     * @param firstPointEnd Pointer to the attribute following the last attribute of the first
     * data sample. This value acts as a placeholder, and it is used to identify the end of the
     * first point.
     * @param secondPointBegin Pointer to the first attribute of the second data sample.
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
     * @param firstPointBegin Pointer to the first attribute of the first data sample.
     * @param firstPointEnd Pointer to the attribute following the last attribute of the first
     * data sample. This value acts as a placeholder, and it is used to identify the end of the
     * first point.
     * @param secondPointBegin Pointer to the first attribute of the second data sample.
     * @return The distance between the two points.
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
     * @param firstPointBegin Pointer to the first attribute of the first data sample.
     * @param firstPointEnd Pointer to the attribute following the last attribute of the first
     * data sample. This value acts as a placeholder, and it is used to identify the end of the
     * first point.
     * @param secondPointBegin Pointer to the first attribute of the second data sample.
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
     * @param firstPointBegin Pointer to the first attribute of the first data sample.
     * @param firstPointEnd Pointer to the attribute following the last attribute of the first
     * data sample. This value acts as a placeholder, and it is used to identify the end of the
     * first point.
     * @param secondPointBegin Pointer to the first attribute of the second data sample.
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
     * @param firstPointBegin Pointer to the first attribute of the first data sample.
     * @param firstPointEnd Pointer to the attribute following the last attribute of the first
     * data sample. This value acts as a placeholder, and it is used to identify the end of the
     * first point.
     * @param secondPointBegin Pointer to the first attribute of the second data sample.
     * @return The distance between the two points.
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
