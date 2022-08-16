#ifndef FINAL_PROJECT_HPC_POINTERONLYCONSTITERABLESUBCONTAINER_H
#define FINAL_PROJECT_HPC_POINTERONLYCONSTITERABLESUBCONTAINER_H

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
class PointerOnlyConstIterableSubContainer {

public:
    PointerOnlyConstIterableSubContainer(const std::string &secondLevelName,
                                         utils::DataIteratorType iteratorType,
                                         utils::DataLevelIteratorType secondLevelIteratorType) :
        normal{PointerTypeNameComposer::composeName(secondLevelName, ""),
               iteratorType,
               utils::DataLevelIteratorType::ITERATOR,
               secondLevelIteratorType},
        normalConstIterator{PointerTypeNameComposer::composeName(secondLevelName, "const_iterator"),
                            iteratorType,
                            utils::DataLevelIteratorType::ITERATOR,
                            utils::DataLevelIteratorType::ITERATOR} {

        normal = new IC[N];
        normalConstIterator = new typename IC::const_iterator[N];
    }

    PointerOnlyConstIterableSubContainer(const PointerOnlyConstIterableSubContainer &) = delete;
    PointerOnlyConstIterableSubContainer(PointerOnlyConstIterableSubContainer &&) = delete;
    PointerOnlyConstIterableSubContainer &operator= (
            const PointerOnlyConstIterableSubContainer &) = delete;
    PointerOnlyConstIterableSubContainer &operator= (
            PointerOnlyConstIterableSubContainer &&) = delete;

    ~PointerOnlyConstIterableSubContainer() {

        delete[] normal;
        delete[] normalConstIterator;
    }

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field[index] = element;
    }

    void initIterators() {

        for (std::size_t i = 0; i < N; i++) {
            normalConstIterator.field[i] = normal.field[i].cbegin();
        }
    }

    std::tuple<CollectionInfo<IC *> &, CollectionInfo<typename IC::const_iterator *> &>
    getCollectionsInfo() {

        return std::tuple<CollectionInfo<IC *> &, CollectionInfo<typename IC::const_iterator *> &>(

                normal, normalConstIterator);
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    CollectionInfo<IC *> normal;
    CollectionInfo<typename IC::const_iterator *> normalConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::pointer

#endif  // FINAL_PROJECT_HPC_POINTERONLYCONSTITERABLESUBCONTAINER_H
