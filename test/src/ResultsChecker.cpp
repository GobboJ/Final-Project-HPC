/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-07
 * @since version date
 */
#include "ResultsChecker.h"

namespace cluster::test {



bool ResultsChecker::areAlmostEqual(const double first, const double second) {

    if (first > std::numeric_limits<double>::max() && second > std::numeric_limits<double>::max()) {
        return true;
    }

    return fabs(first - second) < EPSILON;
}
}  // namespace cluster::test
