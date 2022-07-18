#include <iostream>
#include <vector>
#include <cmath>
#include "DataReader.h"
#include "SequentialClustering.h"
#include "ParallelClustering.h"

bool areAlmostLess(const double first, const double second) {

    return (first - second) < -0.0001;
}

bool areAlmostEqual(const double first, const double second) {

    if (first > std::numeric_limits<double>::max() &&
        second > std::numeric_limits<double>::max()) {
        return true;
    }

    return fabs(first - second) < 0.0001;
}

bool check(std::vector<std::pair<std::size_t, double>> &expected,
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

        while (areAlmostEqual(lambdaToCheck, expected[*orderedExpectedIndexesIterator].second) &&
               continueSearch) {
            if (piToCheck != expected[*orderedExpectedIndexesIterator].first) {
                ++orderedExpectedIndexesIterator;
                back++;
            } else {
                continueSearch = false;
            }
        }

        if (continueSearch) {
            std::cerr << "Expected" << ' ' << index << " being in the cluster" << ' ' << piToCheck
                      << " at distance" << ' ' << lambdaToCheck << " but it is not";
            return false;
        }
        orderedExpectedIndexesIterator -= back;
    }

    return true;
}

/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-18
 * @since version date
 */
int main(int arcg, char *argv[]) {

    std::string fileName{};
    std::vector<std::pair<std::size_t, double>> expected{};
    std::string argument{argv[1]};

    if (argument == "two-points") {
        fileName = "two-points.data";
        expected = {{1, 3.6055}, {1, INFINITY}};
    } else if (argument == "samples") {
        fileName = "samples.data";
        expected = {
                {1, 0.4472},   // A
                {4, 0.8944},   // B
                {3, 0.4},      // C
                {4, 0.7211},   // D
                {5, 1.2806},   // E
                {5, INFINITY}  // F
        };
    } else if (argument == "same-distance") {
        fileName = "same-distance.data";
        expected = {
                {3, 2.2360},   // A
                {2, 1.4142},   // B
                {3, 2.2360},   // C
                {3, INFINITY}  // D
        };
    } else if (argument == "slide") {
        fileName = "slide.data";
        expected = {
                {1, 0.7071},   // 1
                {5, 2.5},      // 2
                {5, 1.4142},   // 3,
                {5, 0.5},      // 4
                {5, 1},        // 5
                {5, INFINITY}  // 6
        };
    } else {
        std::cout << "Unknown test" << ' ' << argument << '.' << std::endl;
        return 1;
    }

    std::vector<double *> data = DataReader::readData(0, 1, fileName);

    std::vector<std::size_t> pi{};
    std::vector<double> lambda{};

    ParallelClustering::clusterV1(data, 2, pi, lambda);

    std::vector<std::pair<std::size_t, double>> result{};
    for (std::size_t i = 0; i < pi.size(); i++) {
        result.emplace_back(std::make_pair(pi[i], lambda[i]));
    }

    if (check(expected, result)) {
        return 0;
    }
    return 1;
}
