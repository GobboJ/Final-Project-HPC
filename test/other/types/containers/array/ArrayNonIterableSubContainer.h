#ifndef FINAL_PROJECT_HPC_ARRAYNONITERABLESUBCONTAINER_H
#define FINAL_PROJECT_HPC_ARRAYNONITERABLESUBCONTAINER_H

#include <cstddef>
#include <string>
#include "ArrayTypeNameComposer.h"
#include "IteratorType.h"
#include "../CollectionInfo.h"

namespace cluster::test::types::containers::array {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-14
 * @since version date
 */
template <template <typename, std::size_t> typename C, typename IC, std::size_t N>
class ArrayNonIterableSubContainer {

public:
    ArrayNonIterableSubContainer(const std::string &firstLevelName,
                                 const std::string &secondLevelName,
                                 utils::DataType iteratorType,
                                 utils::DataLevelType firstLevelIteratorType,
                                 utils::DataLevelType secondLevelIteratorType) :
        normal{ArrayTypeNameComposer::composeName(firstLevelName, secondLevelName),
               iteratorType,
               firstLevelIteratorType,
               secondLevelIteratorType},
        normalIterator{
                ArrayTypeNameComposer::composeName(firstLevelName, secondLevelName, "", "iterator"),
                iteratorType,
                utils::DataLevelType::ITERATOR,
                secondLevelIteratorType},
        normalConstIterator{ArrayTypeNameComposer::composeName(
                                    firstLevelName, secondLevelName, "", "const_iterator"),
                            iteratorType,
                            utils::DataLevelType::ITERATOR,
                            secondLevelIteratorType}

    {
    }

    ArrayNonIterableSubContainer(const ArrayNonIterableSubContainer &) = delete;
    ArrayNonIterableSubContainer(ArrayNonIterableSubContainer &&) = delete;
    ArrayNonIterableSubContainer &operator= (const ArrayNonIterableSubContainer &) = delete;
    ArrayNonIterableSubContainer &operator= (ArrayNonIterableSubContainer &&) = delete;
    ~ArrayNonIterableSubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field[index] = element;
    }

    void initIterators() {

        normalIterator.field = normal.field.begin();
        normalConstIterator.field = normal.field.cbegin();
    }

    std::tuple<CollectionInfo<C<IC, N>> &,
               CollectionInfo<typename C<IC, N>::iterator> &,
               CollectionInfo<typename C<IC, N>::const_iterator> &>
    getCollectionsInfo() {

        return std::tuple<CollectionInfo<C<IC, N>> &,
                          CollectionInfo<typename C<IC, N>::iterator> &,
                          CollectionInfo<typename C<IC, N>::const_iterator> &>(

                normal, normalIterator, normalConstIterator);
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    CollectionInfo<C<IC, N>> normal;
    CollectionInfo<typename C<IC, N>::iterator> normalIterator;
    CollectionInfo<typename C<IC, N>::const_iterator> normalConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::array

#endif  // FINAL_PROJECT_HPC_ARRAYNONITERABLESUBCONTAINER_H
