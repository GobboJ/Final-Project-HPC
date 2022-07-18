#include <iostream>
#include <vector>
#include <cmath>

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
        while (lambdaToCheck > expected[*orderedExpectedIndexesIterator].second) {
            ++orderedExpectedIndexesIterator;
        }

        while ((lambdaToCheck + 0.0001) >= expected[*orderedExpectedIndexesIterator].second &&
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
    std::vector<std::pair<std::size_t, double>> expected{{2, 1.2},
                                                         {4, 3.4},
                                                         {4, 2.1},
                                                         {10, 4.1},
                                                         {6, 1.9},
                                                         {10, 4.2},
                                                         {10, 3.6},
                                                         {10, 2.9},
                                                         {10, 1.4},
                                                         {10, INFINITY}};

    // TODO: Populate the vector

    std::vector<std::pair<std::size_t, double>> result{{2, 1.2},
                                                       {4, 3.4},
                                                       {4, 2.1},
                                                       {10, 4.1},
                                                       {6, 1.9},
                                                       {10, 4.2},
                                                       {10, 3.6},
                                                       {10, 2.9},
                                                       {10, 1.4},
                                                       {10, INFINITY}};

    // TODO: Execute the algorithm

    if (check(expected, result)) {
        return 0;
    }
    return 1;
}
