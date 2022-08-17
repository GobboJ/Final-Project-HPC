/*
 * Results checker implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-08-07
 * @since 1.0
 */
#include "ResultsChecker.h"

namespace cluster::test {

/**
 * Check is the two values are almost equal, i.e., if they differ of at most
 * <code>ResultsChecker::EPSILON</code>.
 *
 * @param first First point to check.
 * @param second Second point to check.
 * @return <code>true</code> if the two values are almost equal, <code>false</code> otherwise.
 */
bool ResultsChecker::areAlmostEqual(const double first, const double second) {

    // The two points are equal if they are both infinity
    if (first > std::numeric_limits<double>::max() && second > std::numeric_limits<double>::max()) {
        return true;
    }

    // Check if the absolute difference is less than EPSILON
    return fabs(first - second) < EPSILON;
}
}  // namespace cluster::test
