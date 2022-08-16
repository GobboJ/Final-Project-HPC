#ifndef FINAL_PROJECT_HPC_COLLECTIONTYPENAMECOMPOSER_H
#define FINAL_PROJECT_HPC_COLLECTIONTYPENAMECOMPOSER_H

#include <string>

namespace cluster::test::types::containers::collection {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-14
 * @since version date
 */
class CollectionTypeNameComposer {

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
        result += ">";
        if (!firstLevelIterator.empty()) {
            result += "::" + firstLevelIterator;
        }

        return result;
    }
};

}  // namespace cluster::test::types::containers::collection

#endif  // FINAL_PROJECT_HPC_COLLECTIONTYPENAMECOMPOSER_H
