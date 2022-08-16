#ifndef FINAL_PROJECT_HPC_POINTERNONITERABLESUBCONTAINER_H
#define FINAL_PROJECT_HPC_POINTERNONITERABLESUBCONTAINER_H

#include "../CollectionInfo.h"
#include "PointerTypeNameComposer.h"

namespace cluster::test::types::containers::pointer {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-14
 * @since version date
 */
template <typename IC, std::size_t N>
class PointerNonIterableSubContainer {

public:
    PointerNonIterableSubContainer(const std::string &secondLevelName,
                                   utils::DataIteratorType iteratorType,
                                   utils::DataLevelIteratorType secondLevelIteratorType) :
        normal{PointerTypeNameComposer::composeName(secondLevelName, ""),
               iteratorType,
               utils::DataLevelIteratorType::ITERATOR,
               secondLevelIteratorType} {

        normal = new IC[N];
    }

    PointerNonIterableSubContainer(const PointerNonIterableSubContainer &) = delete;
    PointerNonIterableSubContainer(PointerNonIterableSubContainer &&) = delete;
    PointerNonIterableSubContainer &operator= (const PointerNonIterableSubContainer &) = delete;
    PointerNonIterableSubContainer &operator= (PointerNonIterableSubContainer &&) = delete;

    ~PointerNonIterableSubContainer() {

        delete[] normal;
    }

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field[index] = element;
    }

    void initIterators() {
    }

    std::tuple<CollectionInfo<IC *> &> getCollectionsInfo() {

        return std::tuple<CollectionInfo<IC *> &>(

                normal);
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    CollectionInfo<IC *> normal;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::pointer

#endif  // FINAL_PROJECT_HPC_POINTERNONITERABLESUBCONTAINER_H
