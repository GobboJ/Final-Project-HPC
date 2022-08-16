/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-16
 * @since version date
 */
#include "../../include/utils/DataIteratorUtils.h"

namespace cluster::utils {
DataIteratorType DataIteratorUtils::lastIteratorType = DataIteratorType::NONE;
DataLevelIteratorType DataIteratorUtils::lastFirstLevelIteratorType = DataLevelIteratorType::NONE;
DataLevelIteratorType DataIteratorUtils::lastSecondLevelIteratorType = DataLevelIteratorType::NONE;
bool DataIteratorUtils::printSummaries = false;
}  // namespace cluster::utils
