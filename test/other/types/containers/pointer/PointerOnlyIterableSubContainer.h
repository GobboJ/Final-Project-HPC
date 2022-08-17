#ifndef FINAL_PROJECT_HPC_POINTERONLYITERABLESUBCONTAINER_H
#define FINAL_PROJECT_HPC_POINTERONLYITERABLESUBCONTAINER_H

#include "../CollectionInfo.h"
#include "PointerTypeNameComposer.h"
#include <tuple>
#include <string>

namespace cluster::test::types::containers::pointer {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-14
 * @since version date
 */
template <typename IC, std::size_t N>
class PointerOnlyIterableSubContainer {

public:
    PointerOnlyIterableSubContainer(const std::string &secondLevelName,
                                    utils::DataType iteratorType,
                                    utils::DataLevelType secondLevelIteratorType) :
        normal{PointerTypeNameComposer::composeName(secondLevelName, ""),
               iteratorType,
               utils::DataLevelType::ITERATOR,
               secondLevelIteratorType},
        normalIterator{PointerTypeNameComposer::composeName(secondLevelName, "iterator"),
                       iteratorType,
                       utils::DataLevelType::ITERATOR,
                       utils::DataLevelType::ITERATOR} {

        normal = new IC[N];
        normalIterator = new typename IC::iterator[N];
    }

    PointerOnlyIterableSubContainer(const PointerOnlyIterableSubContainer &) = delete;
    PointerOnlyIterableSubContainer(PointerOnlyIterableSubContainer &&) = delete;
    PointerOnlyIterableSubContainer &operator= (const PointerOnlyIterableSubContainer &) = delete;
    PointerOnlyIterableSubContainer &operator= (PointerOnlyIterableSubContainer &&) = delete;

    ~PointerOnlyIterableSubContainer() {

        delete[] normal;
        delete[] normalIterator;
    }

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field[index] = element;
    }

    void initIterators() {

        for (std::size_t i = 0; i < N; i++) {
            normalIterator.field[i] = normal.field[i].begin();
        }
    }

    std::tuple<CollectionInfo<IC *> &, CollectionInfo<typename IC::iterator *> &>
    getCollectionsInfo() {

        return std::tuple<CollectionInfo<IC *> &, CollectionInfo<typename IC::iterator *> &>(

                normal, normalIterator);
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    CollectionInfo<IC *> normal;
    CollectionInfo<typename IC::iterator *> normalIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::pointer

#endif  // FINAL_PROJECT_HPC_POINTERONLYITERABLESUBCONTAINER_H
