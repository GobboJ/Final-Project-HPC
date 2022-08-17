/*
 * DataIteratorUtils implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-08-16
 * @since 1.0
 */
#include "../../include/utils/DataIteratorUtils.h"

namespace cluster::utils {

/**
 * Type of the last specified iterator or iterable holding the first level of indirection of the
 * data samples to cluster.
 */
DataType DataIteratorUtils::lastDataType = DataType::NONE;

/**
 * Type of the last specified data structure or iterator holding the first level of indirection of
 * the data samples to cluster.
 */
DataLevelType DataIteratorUtils::lastFirstLevelDataType = DataLevelType::NONE;

/**
 * Type of the last specified data structure or iterator holding the second level of indirection of
 * the data samples to cluster.
 */
DataLevelType DataIteratorUtils::lastSecondLevelDataType = DataLevelType::NONE;

/**
 * Flag allowing to enable or disable the printing of the summary.
 */
bool DataIteratorUtils::printSummaries = false;
}  // namespace cluster::utils
