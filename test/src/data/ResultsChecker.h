#ifndef FINAL_PROJECT_HPC_RESULTSCHECKER_H
#define FINAL_PROJECT_HPC_RESULTSCHECKER_H

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <vector>

namespace cluster::test {

/**
 * Class allowing to check if the <code>pi</code> and <code>lambda</code> vector computed by the
 * clustering algorithms are correct.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-08-07
 * @since 1.0
 */
class ResultsChecker {

private:
    /**
     * Maximum difference between two values that are considered to be equal.
     */
    static const constexpr double EPSILON = 0.0001;

public:
    /**
     * Checks if the data structures holding <code>pi</code> and <code>lambda</code> are correct,
     * i.e., if they hold the same elements of the data structure holding the expected values of
     * <code>pi</code> and <code>lambda</code>
     *
     * @tparam P Type of the iterator iterating over the data structure holding the values of
     * <code>pi</code>.
     * @tparam L Type of the iterator iterating over the data structure holding the values of
     * <code>lambda</code>.
     * @tparam EP Type of the iterator iterating over the data structure holding the correct values
     * of <code>pi</code>.
     * @tparam EL Type of the iterator iterating over the data structure holding the correct values
     * of <code>lambda</code>.
     * @param resultPiBegin Iterator pointing at the first element of the data structure holding the
     * values of <code>pi</code>.
     * @param resultPiEnd Iterator pointing to the element past the end of the data structure
     * holding the values of <code>pi</code>.
     * @param resultLambdaBegin Iterator pointing at the first element of the data structure holding
     * the values of <code>lambda</code>.
     * @param resultLambdaEnd Iterator pointing to the element past the end of the data structure
     * holding the values of <code>lambda</code>.
     * @param expectedPiBegin Iterator pointing at the first element of the data structure holding
     * the correct values of <code>pi</code>.
     * @param expectedPiEnd Iterator pointing to the element past the end of the data structure
     * the correct values of <code>pi</code>.
     * @param expectedLambdaBegin Iterator pointing at the first element of the data structure
     * holding the correct values of <code>lambda</code>.
     * @param expectedLambdaEnd Iterator pointing to the element past the end of the data structure
     * the correct values of <code>lambda</code>.
     * @return <code>true</code> if the data structures holding <code>pi</code> and
     * <code>lambda</code> contain the correct values, <code>false</code> otherwise.
     */
    template <typename P, typename L, typename EP, typename EL>
    static bool checkResults(const P resultPiBegin,
                             const P resultPiEnd,
                             const L resultLambdaBegin,
                             const L resultLambdaEnd,
                             const EP expectedPiBegin,
                             const EP expectedPiEnd,
                             const EL expectedLambdaBegin,
                             const EL expectedLambdaEnd) {

        // Copy the iterators
        P currentResultPi = resultPiBegin;
        L currentResultLambda = resultLambdaBegin;
        EP currentExpectedPi = expectedPiBegin;
        EL currentExpectedLambda = expectedLambdaBegin;
        // Assume the data structures are equal
        bool equal = true;
        // Index of the index being analyzed
        std::size_t dataSampleIndex = 0;

        // Iterate over all the four data structures
        while (currentResultPi != resultPiEnd && currentResultLambda != resultLambdaEnd &&
               currentExpectedPi != expectedPiEnd && currentExpectedLambda != expectedLambdaEnd &&
               equal) {

            // Extract the values to check
            std::size_t piValue = *currentResultPi;
            double lambdaValue = *currentResultLambda;
            std::size_t expectedPiValue = *currentExpectedPi;
            double expectedLambdaValue = *currentExpectedLambda;

            // Compare the values of pi and lambda
            if (piValue != expectedPiValue || !areAlmostEqual(lambdaValue, expectedLambdaValue)) {
                // Print the error message
                std::cerr << "Error at data sample" << ' ' << dataSampleIndex << ":" << std::endl;
                std::cerr << "    "
                          << "Expected to be connected to" << ' ' << expectedPiValue
                          << " at distance" << ' ' << expectedLambdaValue << std::endl;
                std::cerr << "    "
                          << "Found it is connected to" << ' ' << piValue << " at distance" << ' '
                          << lambdaValue << std::endl;
                equal = false;
            }
            // Move to the next element of all the iterators
            dataSampleIndex++;
            ++currentResultPi;
            ++currentResultLambda;
            ++currentExpectedPi;
            ++currentExpectedLambda;
        }

        // Check if all the iterators are at the end
        if (equal) {
            // Check if the vectors holding the computed values of pi and lambda have been fully
            // iterated
            if ((currentResultPi != resultPiEnd && currentResultLambda == resultLambdaEnd) ||
                (currentResultPi == resultPiEnd && currentResultLambda != resultLambdaEnd)) {
                std::cerr << "The result pi and lambda data structures have different sizes."
                          << std::endl;
                return false;
            }
            // Check if the vectors holding the expected values for pi and lambda have been fully
            // iterated
            if ((currentExpectedPi != expectedPiEnd &&
                 currentExpectedLambda == expectedLambdaEnd) ||
                (currentExpectedPi == expectedPiEnd &&
                 currentExpectedLambda != expectedLambdaEnd)) {
                std::cerr << "The expected pi and lambda vectors have different sizes."
                          << std::endl;
                return false;
            }
            // Check if all the data structures have been fully iterated
            if ((currentResultPi != resultPiEnd && currentExpectedPi == expectedPiEnd) ||
                (currentResultPi == resultPiEnd && currentExpectedPi != expectedPiEnd)) {
                std::cerr << "The result and expected data structures have different sizes."
                          << std::endl;
                return false;
            }
        }

        return equal;
    }

private:
    /**
     * Check is the two values are almost equal, i.e., if they differ of at most
     * <code>ResultsChecker::EPSILON</code>.<br>
     * Note that if the two values are both infinity, then they are considered equal.
     *
     * @param first First point to check.
     * @param second Second point to check.
     * @return <code>true</code> if the two values are almost equal, <code>false</code> otherwise.
     */
    static bool areAlmostEqual(double first, double second);
};
}  // namespace cluster::test
#endif  // FINAL_PROJECT_HPC_RESULTSCHECKER_H
