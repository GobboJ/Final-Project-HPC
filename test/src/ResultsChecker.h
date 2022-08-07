#ifndef FINAL_PROJECT_HPC_RESULTSCHECKER_H
#define FINAL_PROJECT_HPC_RESULTSCHECKER_H

#include <vector>
#include <filesystem>
#include <iostream>
#include <cmath>
#include <algorithm>

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-07
 * @since version date
 */
class ResultsChecker {

public:
    static bool checkTest(const std::vector<std::size_t> &pi,
                   const std::vector<double> &lambda,

                   std::vector<std::size_t> expectedPi,
                   std::vector<double> expectedLambda) {

        std::vector<std::pair<std::size_t, double>> expectedResult{};

        for (std::size_t i = 0; i < expectedPi.size(); i++) {
            expectedResult.emplace_back(std::make_pair(expectedPi[i], expectedLambda[i]));
        }

        std::vector<std::pair<std::size_t, double>> result{};
        for (std::size_t i = 0; i < pi.size(); i++) {
            result.emplace_back(std::make_pair(pi[i], lambda[i]));
        }

        return check(expectedResult, result);
    }

private:
    static bool check(std::vector<std::pair<std::size_t, double>> &expected,
                      std::vector<std::pair<std::size_t, double>> &result) {

        std::vector<std::size_t> orderedIndexes{};
        for (std::size_t i = 0; i < expected.size(); i++) {
            orderedIndexes.push_back(i);
        }
        std::sort(orderedIndexes.begin(),
                  orderedIndexes.end(),
                  [&expected](const auto &a, const auto &b) -> bool {
                      return expected[a].second < expected[b].second;
                  });

        std::vector<std::size_t> orderedExpectedIndexes{};
        for (std::size_t i = 0; i < result.size(); i++) {
            orderedExpectedIndexes.push_back(i);
        }
        std::sort(orderedExpectedIndexes.begin(),
                  orderedExpectedIndexes.end(),
                  [&result](const auto &a, const auto &b) -> bool {
                      return result[a].second < result[b].second;
                  });

        auto orderedExpectedIndexesIterator = orderedExpectedIndexes.cbegin();
        for (std::size_t index : orderedIndexes) {

            std::size_t piToCheck = result[index].first;
            double lambdaToCheck = result[index].second;

            std::vector<std::size_t>::difference_type back = 0;
            bool continueSearch = true;
            while (areAlmostLess(expected[*orderedExpectedIndexesIterator].second, lambdaToCheck)) {
                ++orderedExpectedIndexesIterator;
            }

            while (areAlmostEqual(lambdaToCheck,
                                  expected[*orderedExpectedIndexesIterator].second) &&
                   continueSearch) {
                if (piToCheck != expected[*orderedExpectedIndexesIterator].first) {
                    ++orderedExpectedIndexesIterator;
                    back++;
                } else {
                    continueSearch = false;
                }
            }

            if (continueSearch) {
                std::cerr << "Expected" << ' ' << index << " being in the cluster" << ' '
                          << piToCheck << " at distance" << ' ' << lambdaToCheck << " but it is not"
                          << std::endl;
                return false;
            }
            orderedExpectedIndexesIterator -= back;
        }

        return true;
    }

    [[gnu::const]] static bool areAlmostLess(const double first, const double second) {

        return (first - second) < -0.0001;
    }

    [[gnu::const]] static bool areAlmostEqual(const double first, const double second) {

        if (first > std::numeric_limits<double>::max() &&
            second > std::numeric_limits<double>::max()) {
            return true;
        }

        return fabs(first - second) < 0.0001;
    }
};

#endif  // FINAL_PROJECT_HPC_RESULTSCHECKER_H
