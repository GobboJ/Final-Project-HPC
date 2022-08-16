#ifndef FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H
#define FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H

#include "Timer.h"
#include "../../include/utils/Types.h"
#include "Logger.h"

namespace cluster::sequential {

/**
 * Class providing a sequential implementation of the clustering algorithm.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-08-01
 * @since 1.0
 */
class SequentialClustering {
    using Logger = utils::Logger;
    using Timer = utils::Timer;
    using PiLambdaIteratorUtils = utils::PiLambdaIteratorUtils;

public:
    /**
     * Sequential implementation of the clustering algorithm.
     *
     * @tparam D Type of the iterator that iterates over the data samples to cluster.
     * @tparam P Type of the iterator that allows to fill the data structure holding the
     * <code>pi</code> values.
     * @tparam L Type of the iterator that allows to fill the data structure holding the
     * <code>lambda</code> values.
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
     */
    template <utils::SequentialDataIterator D, utils::PiIterator P, utils::LambdaIterator L>
    static void cluster(const D &data,
                        const std::size_t dataSamplesCount,
                        const std::size_t dimension,
                        P &piIterator,
                        L &lambdaIterator) {

        // Initialize the timers and start logging the console output, if requested
        // Timer::initTimers();
        Logger::startLoggingProgress<0, 1, 2, 3, 4>(dataSamplesCount);

        Timer::start<0>();
        // Initialize the part-row values
        auto *__restrict__ const m = new double[dataSamplesCount];

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

        /*******************************************************************************************
         * 1) Set pi(1) to 0, lambda(1) to infinity
         ******************************************************************************************/
        Timer::start<1>();

        initializeNewPoint<P, L>(currentPi, currentLambda, 0);
        Timer::stop<1>();

        Timer::start<9>();
        // No more operations need to be performed for the first point, so move to the second
        // element of the dataset
        utils::DataIteratorUtils::moveNext<D>(currentData, dimension);

        double *distanceEnd = &(m[1]);

        // Perform the clustering algorithm for all the remaining data samples
        for (std::size_t n = 1; n < dataSamplesCount; n++) {

            Timer::stop<0>();

            // Log the progress every 1000 samples
            Logger::updateProgress<1000, 0, 1, 2, 3, 4>(n, dataSamplesCount);

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

            // Iterator over all the m[i] with 0 <= i < n
            auto *__restrict__ mIterator = &(m[0]);
            // Iterator over the first n-1 elements of the dataset
            auto distanceDataIterator = startData;
            // Value of the n-th value of the dataset
            const double *__restrict__ const currentDataN =
                    utils::DataIteratorUtils::getCurrentElement<D>(currentData);
            const double *currentDataNEnd = currentDataN + dimension;

            // Compute the distances between *distanceDataIterator, which refers to the i-th element
            // of the dataset, and *currentDataN, which instead refers to the i-th element of the
            // dataset
            while (mIterator != distanceEnd) {
                const double *const __restrict__ element =
                        utils::DataIteratorUtils::getCurrentElement<D>(distanceDataIterator);
                *mIterator = SequentialClustering::distance(currentDataN, currentDataNEnd, element);
                ++mIterator;
                utils::DataIteratorUtils::moveNext<D>(distanceDataIterator, dimension);
            }

            Timer::stop<2>();

            /***************************************************************************************
             * 3) For i from 1 to n
             **************************************************************************************/
            Timer::start<3>();

            // Iterator over the first n-1 values of pi
            auto iteratorOverPi = piBegin;
            // Iterator over the first n-1 values of lambda
            auto iteratorOverLambda = lambdaBegin;

            // Iterator over the first n-1 values of m
            const double *__restrict__ distanceIterator = &(m[0]);

            while (distanceIterator != distanceEnd) {
                // Reference to pi[i]
                std::size_t &piOfI =
                        PiLambdaIteratorUtils::getCurrentElement<std::size_t, P>(iteratorOverPi);
                // Reference to lambda[i]
                double &lambdaOfI =
                        PiLambdaIteratorUtils::getCurrentElement<double, L>(iteratorOverLambda);
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
                utils::PiLambdaIteratorUtils::moveNext<std::size_t, P>(iteratorOverPi);
                utils::PiLambdaIteratorUtils::moveNext<double, L>(iteratorOverLambda);
                ++distanceIterator;
            }
            Timer::stop<3>();

            /***************************************************************************************
             * 4) For i from 1 to n
             **************************************************************************************/
            Timer::start<4>();

            // Move the iterators to the beginning
            iteratorOverPi = piBegin;
            iteratorOverLambda = lambdaBegin;

            for (std::size_t i = 0; i <= n - 1; i++) {
                // Reference to pi[i]
                std::size_t &piOfI =
                        PiLambdaIteratorUtils::getCurrentElement<std::size_t, P>(iteratorOverPi);

                /***********************************************************************************
                 * if lambda(i) >= lambda(pi(i))
                 **********************************************************************************/
                if (*iteratorOverLambda >= lambdaIterator[piOfI]) {
                    /*******************************************************************************
                     * set pi(i) to n + 1
                     ******************************************************************************/
                    piOfI = n;
                }
                // Move to the next element of pi and lambda
                utils::PiLambdaIteratorUtils::moveNext<std::size_t, P>(iteratorOverPi);
                utils::PiLambdaIteratorUtils::moveNext<double, L>(iteratorOverLambda);
            }
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
};
}  // namespace cluster::sequential
#endif  // FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H
