#ifndef FINAL_PROJECT_HPC_ARRAYTYPENAMECOMPOSER_H
#define FINAL_PROJECT_HPC_ARRAYTYPENAMECOMPOSER_H

#include <string>

namespace cluster::test::types::containers::array {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-14
 * @since version date
 */
class ArrayTypeNameComposer {

public:
    static std::string composeName(const std::string &firstLevel,
                                   const std::string &secondLevel,
                                   const std::string &secondLevelIterator = "",
                                   const std::string &firstLevelIterator = "") {
        std::string result{firstLevel};
        result += "<" + secondLevel;
        if (!secondLevelIterator.empty()) {
            result += "::" + secondLevelIterator;
        }
        result += ", N>";
        if (!firstLevelIterator.empty()) {
            result += "::" + firstLevelIterator;
        }

        return result;
    }
};

}  // namespace cluster::test::types::containers::array

#endif  // FINAL_PROJECT_HPC_ARRAYTYPENAMECOMPOSER_H
