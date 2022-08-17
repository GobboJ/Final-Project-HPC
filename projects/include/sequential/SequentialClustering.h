#ifndef FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H
#define FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H

#include "../../include/utils/Types.h"
#include "Logger.h"
#include "Timer.h"

namespace cluster::sequential {

/**
 * Class providing a sequential implementation of the clustering algorithm.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.3 2022-08-16
 * @since 1.0
 */
class SequentialClustering {
    // Imports
    using Logger = utils::Logger;
    using Timer = utils::Timer;
    using PiLambdaIteratorUtils = utils::PiLambdaIteratorUtils;

public:
    /**
     * Sequential implementation of the clustering algorithm.
     *
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
     * structure <b>MUST BE</b> big enough to contain all the <code>pi</code> values, which are
     * exactly <code>dataSamplesCount</code>.
     */
    template <utils::SequentialDataIterator D, utils::PiIterator P, utils::LambdaIterator L>
    static void cluster(const D &data,
                        const std::size_t dataSamplesCount,
                        const std::size_t dimension,
                        P &pi,
                        L &lambda) {

        Timer::start<0>();
        // Array containing the part-row values
        auto *__restrict__ const m = new double[dataSamplesCount];

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
        auto dataBegin =
                utils::DataIteratorUtils::createEfficientIterator(data, "First element of data");

        Timer::stop<0>();

        // Log the initial progress
        Logger::startLoggingProgress<0, 1, 2, 3, 4>(dataSamplesCount);

        Timer::start<1>();
        // **** 1) Set pi(1) to 0, lambda(1) to infinity ****
        initializeNewPoint<P, L>(currentPi, currentLambda, 0);
        Timer::stop<1>();

        Timer::start<9>();
        // No more operations need to be performed for the first point, so move to the second
        utils::DataIteratorUtils::moveNext<D>(currentData, dimension);

        // Useful pointer pointing to the element following the last element of m containing a valid
        // distance
        const double *distanceEnd = &(m[1]);

        // Perform the clustering algorithm for all the remaining data samples
        for (std::size_t n = 1; n < dataSamplesCount; n++) {

            Timer::stop<0>();

            // Log the progress every 1000 samples
            Logger::updateProgress<1000, 0, 1, 2, 3, 4>(n, dataSamplesCount);

            Timer::start<1>();
            // **** 1) Set pi(n + 1) to n + 1, lambda(n + 1) to infinity ****
            initializeNewPoint<P, L>(currentPi, currentLambda, n);
            Timer::stop<1>();

            Timer::start<2>();
            // Pointer to the n-th data sample of the dataset
            const double *__restrict__ const currentDataN =
                    utils::DataIteratorUtils::getCurrentSample<D>(currentData);
            // Pointer to the first byte after the n-th data sample of the dataset
            const double *__restrict__ const currentDataNEnd = currentDataN + dimension;
            // **** 2) Set M(i) to d(i, n + 1) for i = 1,..,n ****
            computeDistances<D>(
                    dataBegin, dimension, currentDataN, currentDataNEnd, m, distanceEnd);
            Timer::stop<2>();

            Timer::start<3>();
            // **** 3) For i from 1 to n ****
            addNewPoint<P, L>(piBegin, lambdaBegin, m, distanceEnd, n);
            Timer::stop<3>();

            Timer::start<4>();
            // **** 4) For i from 1 to n
            fixStructure<P, L>(piBegin, lambdaBegin, n);
            Timer::stop<4>();

            Timer::start<0>();
            // Move to the next data sample
            utils::DataIteratorUtils::moveNext<D>(currentData, dimension);
            ++distanceEnd;
        }

        // Deallocate m, since it is not needed anymore
        delete[] m;
        Timer::stop<0>();

        // Log the final progress
        Logger::updateProgress<1, 0, 1, 2, 3, 4>(dataSamplesCount, dataSamplesCount);
    }

private:
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
     * @tparam D Type of the data structure/iterator holding the data samples to cluster.
     * @tparam ED Type of the efficient iterator iterating over the data structure holding the data
     * samples to cluster.
     * @param dataBegin Efficient iterator pointing to the first data sample.
     * @param dimension Dimension of the data samples.
     * @param currentDataN Pointer to the first attribute of the point to add.
     * @param currentDataNEnd Pointer to the attribute following the last attribute of the point to
     * add. This value acts as a placeholder, and it is used to identify the end of the data sample.
     * @param mBegin Pointer to the first element of <code>m</code>.
     * @param mEnd Pointer to the element following the last valid element of <code>m</code>. This
     * value acts as a placeholder, and it is used to identify the last element of <code>m</code>
     * that will be filled with valid distances.
     */
    template <typename D, typename ED>
    static inline void computeDistances(ED dataBegin,
                                        const std::size_t dimension,
                                        const double *__restrict__ const currentDataN,
                                        const double *__restrict__ const currentDataNEnd,
                                        double *__restrict__ const mBegin,
                                        const double *__restrict__ const mEnd) {

        // Iterator over all the m[i] with 0 <= i < n
        auto *__restrict__ mIterator = mBegin;

        // Compute the distances between the n-th element of the data set and all the elements
        // already added to the dendrogram
        while (mIterator != mEnd) {
            // Extract the data sample
            const double *const __restrict__ dataSample =
                    utils::DataIteratorUtils::getCurrentSample<D>(dataBegin);
            // Compute the distance
            *mIterator = SequentialClustering::distance(currentDataN, currentDataNEnd, dataSample);
            // Move to the next element
            ++mIterator;
            utils::DataIteratorUtils::moveNext<D>(dataBegin, dimension);
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

        // Iterator over the first n-1 values of m
        const double *__restrict__ distanceIterator = mBegin;

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
            utils::PiLambdaIteratorUtils::moveNext<std::size_t, P>(piBegin);
            utils::PiLambdaIteratorUtils::moveNext<double, L>(lambdaBegin);
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
     */
    template <typename P, typename L, typename EP, typename EL>
    static inline void fixStructure(const EP &piBegin, const EL &lambdaBegin, const std::size_t n) {

        // Iterator pointing to pi[i]
        EP currentPi = piBegin;
        // Iterator pointing to lambda[i]
        EL currentLambda = lambdaBegin;

        // Loop over all the previously added points
        for (std::size_t i = 0; i <= n - 1; i++) {
            // Reference to pi[i]
            std::size_t &piI = PiLambdaIteratorUtils::getCurrentElement<std::size_t, P>(currentPi);
            // Value of lambda[i]
            const double lambdaI =
                    PiLambdaIteratorUtils::getCurrentElement<double, L>(currentLambda);
            // Value of lambda[pi[i]]
            const double lambdaPiI =
                    PiLambdaIteratorUtils::getElementAt<double, L>(lambdaBegin, piI);

            // **** if lambda(i) >= lambda(pi(i)) ****
            if (lambdaI >= lambdaPiI) {
                // **** set pi(i) to n + 1 ****
                piI = n;
            }
            // Move to the next element of pi and lambda
            utils::PiLambdaIteratorUtils::moveNext<std::size_t, P>(currentPi);
            utils::PiLambdaIteratorUtils::moveNext<double, L>(currentLambda);
        }
    }

    /**
     * Computes the Euclidean distance between two points.
     *
     * * @param firstPointBegin Pointer to the first attribute of the first data sample.
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
};
}  // namespace cluster::sequential
#endif  // FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H
