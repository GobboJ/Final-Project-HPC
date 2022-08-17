/*
 * PiLambdaIteratorUtils implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-08-16
 * @since 1.0
 */
#include "../../include/utils/PiLambdaIteratorUtils.h"

namespace cluster::utils {

/**
 * Type of the last specified data structure or iterator holding pi.
 */
PiLambdaType PiLambdaIteratorUtils::lastPiType = PiLambdaType::NONE;

/**
 * Type of the last specified data structure or iterator holding lambda.
 */
PiLambdaType PiLambdaIteratorUtils::lastLambdaType = PiLambdaType::NONE;

/**
 * Flag allowing to enable or disable printing the summary.
 */
bool PiLambdaIteratorUtils::printSummaries = false;
}  // namespace cluster::utils
