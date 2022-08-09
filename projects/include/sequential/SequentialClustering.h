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
    template <utils::DataIterator D, utils::PiIterator P, utils::LambdaIterator L>
    static void cluster(const D dataIterator,
                        const std::size_t dataSamplesCount,
                        const std::size_t dimension,
                        const P piIterator,
                        const L lambdaIterator) noexcept {

        // Initialize the timers and start logging the console output, if requested
       // Timer::initTimers();
        Logger::startLoggingProgress<0, 1, 2, 3, 4>(dataSamplesCount);

        Timer::start<0>();
        // Initialize the part-row values
        auto *__restrict__ const m = new double[dataSamplesCount];

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

        // Set the first element of pi to 0
        *currentPi = 0;
        ++currentPi;
        // Set the first element of lambda to infinity
        *currentLambda = std::numeric_limits<double>::infinity();
        ++currentLambda;
        Timer::stop<1>();

        Timer::start<4>();
        // No more operations need to be performed for the first point, so move to the second
        // element of the dataset
        ++currentData;

        Timer::stop<4>();

        // Perform the clustering algorithm for all the remaining data samples
        for (std::size_t n = 1; n < dataSamplesCount; n++) {

            /***************************************************************************************
             * 1) Set pi(n + 1) to n + 1, lambda(n + 1) to infinity
             **************************************************************************************/
            Timer::start<1>();

            // Set pi[n] to n
            *currentPi = n;
            ++currentPi;
            // Set lambda[n] to infinity
            *currentLambda = std::numeric_limits<double>::infinity();
            ++currentLambda;

            Timer::stop<1>();

            /***************************************************************************************
             * 2) Set M(i) to d(i, n + 1) for i = 1,..,n
             **************************************************************************************/
            Timer::start<2>();

            // Iterator over all the m[i] with 0 <= i < n
            auto *__restrict__ mIterator = &(m[0]);
            // Iterator over the first n-1 elements of the dataset
            D distanceDataIterator = dataIterator;
            // Value of the n-th value of the dataset
            const double *__restrict__ const currentDataN = *currentData;

            // Compute the distances between *distanceDataIterator, which refers to the i-th element
            // of the dataset, and *currentDataN, which instead refers to the i-th element of the
            // dataset
            for (std::size_t i = 0; i <= n - 1; i++) {
                *mIterator = SequentialClustering::distance(
                        *distanceDataIterator, currentDataN, dimension);
                ++mIterator;
                ++distanceDataIterator;
            }

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
            Timer::stop<3>();

            /***************************************************************************************
             * 4) For i from 1 to n
             **************************************************************************************/
            Timer::start<4>();

            // Move the iterators to the beginning
            iteratorOverPi = piIterator;
            iteratorOverLambda = lambdaIterator;

            for (std::size_t i = 0; i <= n - 1; i++) {
                // Reference to pi[i]
                std::size_t &piOfI = *iteratorOverPi;

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
                ++iteratorOverPi;
                ++iteratorOverLambda;
            }

            // Move to the next data sample
            ++currentData;

            Timer::stop<4>();

            // Log the progress every 1000 samples
            Logger::updateProgress<1000, 0, 1, 2, 3, 4>(n, dataSamplesCount);
        }

        // Deallocate m, since it is not needed anymore
        delete[] m;

        // Log the final progress
        Logger::updateProgress<1, 0, 1, 2, 3, 4>(dataSamplesCount, dataSamplesCount);
    }

private:
    /**
     * Computes the Euclidean distance between two points.
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
}  // namespace cluster::sequential
#endif  // FINAL_PROJECT_HPC_SEQUENTIALCLUSTERING_H
