#ifndef FINAL_PROJECT_HPC_ARRAYONLYCONSTITERABLESUBCONTAINER_H
#define FINAL_PROJECT_HPC_ARRAYONLYCONSTITERABLESUBCONTAINER_H

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
class ArrayOnlyConstIterableSubContainer {

public:
    ArrayOnlyConstIterableSubContainer(const std::string &firstLevelName,
                                      const std::string &secondLevelName,
                                      utils::DataIteratorType iteratorType,
                                      utils::DataLevelIteratorType firstLevelIteratorType,
                                      utils::DataLevelIteratorType secondLevelIteratorType) :
        normal{ArrayTypeNameComposer::composeName(firstLevelName, secondLevelName),
               iteratorType,
               firstLevelIteratorType,
               secondLevelIteratorType},
        normalIterator{
                ArrayTypeNameComposer::composeName(firstLevelName, secondLevelName, "", "iterator"),
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                secondLevelIteratorType},
        normalConstIterator{ArrayTypeNameComposer::composeName(
                                    firstLevelName, secondLevelName, "", "const_iterator"),
                            iteratorType,
                            utils::DataLevelIteratorType::ITERATOR,
                            secondLevelIteratorType},
        constIterators{ArrayTypeNameComposer::composeName(
                               firstLevelName, secondLevelName, "const_iterator"),
                       iteratorType,
                       firstLevelIteratorType,
                       utils::DataLevelIteratorType::ITERATOR},
        constIteratorsIterator{
                ArrayTypeNameComposer::composeName(
                        firstLevelName, secondLevelName, "const_iterator", "iterator"),
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                utils::DataLevelIteratorType::ITERATOR},
        constIteratorsConstIterator{
                ArrayTypeNameComposer::composeName(
                        firstLevelName, secondLevelName, "const_iterator", "const_iterator"),
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                utils::DataLevelIteratorType::ITERATOR}

    {
    }

    ArrayOnlyConstIterableSubContainer(const ArrayOnlyConstIterableSubContainer &) = delete;
    ArrayOnlyConstIterableSubContainer(ArrayOnlyConstIterableSubContainer &&) = delete;
    ArrayOnlyConstIterableSubContainer &operator= (
            const ArrayOnlyConstIterableSubContainer &) = delete;
    ArrayOnlyConstIterableSubContainer &operator= (ArrayOnlyConstIterableSubContainer &&) = delete;
    ~ArrayOnlyConstIterableSubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field[index] = element;
    }

    void initIterators() {

        std::size_t index = 0;
        for (auto &element : normal.field) {
            constIterators.field[index] = element.cbegin();
            index++;
        }
        normalIterator.field = normal.field.begin();
        normalConstIterator.field = normal.field.cbegin();
        constIteratorsIterator.field = constIterators.field.begin();
        constIteratorsConstIterator.field = constIterators.field.cbegin();
    }

    std::tuple<CollectionInfo<C<IC, N>> &,
               CollectionInfo<typename C<IC, N>::iterator> &,
               CollectionInfo<typename C<IC, N>::const_iterator> &,
               CollectionInfo<C<typename IC::const_iterator, N>> &,
               CollectionInfo<typename C<typename IC::const_iterator, N>::iterator> &,
               CollectionInfo<typename C<typename IC::const_iterator, N>::const_iterator> &>
    getCollectionsInfo() {

        return std::tuple<
                CollectionInfo<C<IC, N>> &,
                CollectionInfo<typename C<IC, N>::iterator> &,
                CollectionInfo<typename C<IC, N>::const_iterator> &,
                CollectionInfo<C<typename IC::const_iterator, N>> &,
                CollectionInfo<typename C<typename IC::const_iterator, N>::iterator> &,
                CollectionInfo<typename C<typename IC::const_iterator, N>::const_iterator> &>(

                normal,
                normalIterator,
                normalConstIterator,
                constIterators,
                constIteratorsIterator,
                constIteratorsConstIterator);
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    CollectionInfo<C<IC, N>> normal;
    CollectionInfo<typename C<IC, N>::iterator> normalIterator;
    CollectionInfo<typename C<IC, N>::const_iterator> normalConstIterator;
    CollectionInfo<C<typename IC::const_iterator, N>> constIterators;
    CollectionInfo<typename C<typename IC::const_iterator, N>::iterator> constIteratorsIterator;
    CollectionInfo<typename C<typename IC::const_iterator, N>::const_iterator>
            constIteratorsConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::array

#endif  // FINAL_PROJECT_HPC_ARRAYONLYCONSTITERABLESUBCONTAINER_H
