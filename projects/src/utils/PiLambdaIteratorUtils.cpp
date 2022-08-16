/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-16
 * @since version date
 */
#include "../../include/utils/PiLambdaIteratorUtils.h"

namespace cluster::utils {
PiLambdaIteratorType PiLambdaIteratorUtils::lastPiIteratorType = PiLambdaIteratorType::NONE;
PiLambdaIteratorType PiLambdaIteratorUtils::lastLambdaIteratorType = PiLambdaIteratorType::NONE;
bool PiLambdaIteratorUtils::printSummaries = false;
}  // namespace cluster::utils
