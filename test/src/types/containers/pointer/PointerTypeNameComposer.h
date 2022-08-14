#ifndef FINAL_PROJECT_HPC_POINTERTYPENAMECOMPOSER_H
#define FINAL_PROJECT_HPC_POINTERTYPENAMECOMPOSER_H

#include <string>

namespace cluster::test::types::containers::pointer {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-14
 * @since version date
 */
class PointerTypeNameComposer {
public:
    static std::string composeName(const std::string &secondLevel,
                                   const std::string &secondLevelIterator = "") {
        std::string result{secondLevel};
        if (!secondLevelIterator.empty()) {
            result += "::" + secondLevelIterator;
        }
        result += " *";
        return result;
    }
};

}  // namespace cluster::test::types::containers::pointer

#endif  // FINAL_PROJECT_HPC_POINTERTYPENAMECOMPOSER_H
