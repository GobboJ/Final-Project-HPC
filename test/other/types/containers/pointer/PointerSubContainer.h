#ifndef FINAL_PROJECT_HPC_POINTERSUBCONTAINER_H
#define FINAL_PROJECT_HPC_POINTERSUBCONTAINER_H

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
class PointerSubContainer {

public:
    PointerSubContainer(const std::string &secondLevelName,
                        utils::DataIteratorType iteratorType,
                        utils::DataLevelIteratorType secondLevelIteratorType) :
        normal{PointerTypeNameComposer::composeName(secondLevelName, ""),
               iteratorType,
               utils::DataLevelIteratorType::ITERATOR,
               secondLevelIteratorType},
        normalIterator{PointerTypeNameComposer::composeName(secondLevelName, "iterator"),
                       iteratorType,
                       utils::DataLevelIteratorType::ITERATOR,
                       utils::DataLevelIteratorType::ITERATOR},
        normalConstIterator{PointerTypeNameComposer::composeName(secondLevelName, "const_iterator"),
                            iteratorType,
                            utils::DataLevelIteratorType::ITERATOR,
                            utils::DataLevelIteratorType::ITERATOR} {

        normal = new IC[N];
        normalIterator = new typename IC::iterator[N];
        normalConstIterator = new typename IC::const_iterator[N];
    }

    PointerSubContainer(const PointerSubContainer &) = delete;
    PointerSubContainer(PointerSubContainer &&) = delete;
    PointerSubContainer &operator= (const PointerSubContainer &) = delete;
    PointerSubContainer &operator= (PointerSubContainer &&) = delete;

    ~PointerSubContainer() {

        delete[] normal;
        delete[] normalIterator;
        delete[] normalConstIterator;
    }

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field[index] = element;
    }

    void initIterators() {

        for (std::size_t i = 0; i < N; i++) {
            normalIterator.field[i] = normal.field[i].begin();
            normalConstIterator.field[i] = normal.field[i].cbegin();
        }
    }

    std::tuple<CollectionInfo<IC *> &,
               CollectionInfo<typename IC::iterator *> &,
               CollectionInfo<typename IC::const_iterator *> &>
    getCollectionsInfo() {

        return std::tuple<CollectionInfo<IC *> &,
                          CollectionInfo<typename IC::iterator *> &,
                          CollectionInfo<typename IC::const_iterator *> &>(

                normal, normalIterator, normalConstIterator);
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    CollectionInfo<IC *> normal;
    CollectionInfo<typename IC::iterator *> normalIterator;
    CollectionInfo<typename IC::const_iterator *> normalConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::pointer

#endif  // FINAL_PROJECT_HPC_POINTERSUBCONTAINER_H
