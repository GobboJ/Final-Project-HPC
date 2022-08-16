#ifndef FINAL_PROJECT_HPC_RESULTSCHECKER_H
#define FINAL_PROJECT_HPC_RESULTSCHECKER_H

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <vector>

namespace cluster::test {
/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-07
 * @since version date
 */
class ResultsChecker {

private:
    static const constexpr double EPSILON = 0.0001;

public:
    template <typename P, typename L, typename EP, typename EL>
    static bool checkResults(const P resultPiBegin,
                             const P resultPiEnd,
                             const L resultLambdaBegin,
                             const L resultLambdaEnd,
                             const EP expectedPiBegin,
                             const EP expectedPiEnd,
                             const EL expectedLambdaBegin,
                             const EL expectedLambdaEnd) {

        P currentResultPi = resultPiBegin;
        L currentResultLambda = resultLambdaBegin;
        EP currentExpectedPi = expectedPiBegin;
        EL currentExpectedLambda = expectedLambdaBegin;
        bool equal = true;
        std::size_t dataSampleIndex = 0;

        while (currentResultPi != resultPiEnd && currentResultLambda != resultLambdaEnd &&
               currentExpectedPi != expectedPiEnd && currentExpectedLambda != expectedLambdaEnd &&
               equal) {

            std::size_t piValue = *currentResultPi;
            double lambdaValue = *currentResultLambda;
            std::size_t expectedPiValue = *currentExpectedPi;
            double expectedLambdaValue = *currentExpectedLambda;

            if (piValue != expectedPiValue || !areAlmostEqual(lambdaValue, expectedLambdaValue)) {
                std::cerr << "Error at data sample" << ' ' << dataSampleIndex << ":" << std::endl;
                std::cerr << "    "
                          << "Expected to be connected to" << ' ' << expectedPiValue
                          << " at distance" << ' ' << expectedLambdaValue << std::endl;
                std::cerr << "    "
                          << "Found it is connected to" << ' ' << piValue << " at distance" << ' '
                          << lambdaValue << std::endl;
                equal = false;
            }
            dataSampleIndex++;
            ++currentResultPi;
            ++currentResultLambda;
            ++currentExpectedPi;
            ++currentExpectedLambda;
        }

        if (equal) {
            if ((currentResultPi != resultPiEnd && currentResultLambda == resultLambdaEnd) ||
                (currentResultPi == resultPiEnd && currentResultLambda != resultLambdaEnd)) {
                std::cerr << "The result pi and lambda data structures have different sizes."
                          << std::endl;
                return false;
            }
            if ((currentExpectedPi != expectedPiEnd &&
                 currentExpectedLambda == expectedLambdaEnd) ||
                (currentExpectedPi == expectedPiEnd &&
                 currentExpectedLambda != expectedLambdaEnd)) {
                std::cerr << "The expected pi and lambda vectors have different sizes."
                          << std::endl;
                return false;
            }
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
    [[gnu::const]] static bool areAlmostEqual(double first, double second);
};
}  // namespace cluster::test
#endif  // FINAL_PROJECT_HPC_RESULTSCHECKER_H
