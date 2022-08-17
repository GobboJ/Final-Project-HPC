#ifndef FINAL_PROJECT_HPC_ARRAYONLYITERABLESUBCONTAINER_H
#define FINAL_PROJECT_HPC_ARRAYONLYITERABLESUBCONTAINER_H

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
class ArrayOnlyIterableSubContainer {

public:
    ArrayOnlyIterableSubContainer(const std::string &firstLevelName,
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
                            secondLevelIteratorType},
        iterators{ArrayTypeNameComposer::composeName(firstLevelName, secondLevelName, "iterator"),
                  iteratorType,
                  firstLevelIteratorType,
                  utils::DataLevelType::ITERATOR},
        iteratorsIterator{ArrayTypeNameComposer::composeName(
                                  firstLevelName, secondLevelName, "iterator", "iterator"),
                          iteratorType,
                          utils::DataLevelType::ITERATOR,
                          utils::DataLevelType::ITERATOR},
        iteratorsConstIterator{
                ArrayTypeNameComposer::composeName(
                        firstLevelName, secondLevelName, "iterator", "const_iterator"),
                iteratorType,
                utils::DataLevelType::ITERATOR,
                utils::DataLevelType::ITERATOR} {
    }

    ArrayOnlyIterableSubContainer(const ArrayOnlyIterableSubContainer &) = delete;
    ArrayOnlyIterableSubContainer(ArrayOnlyIterableSubContainer &&) = delete;
    ArrayOnlyIterableSubContainer &operator= (const ArrayOnlyIterableSubContainer &) = delete;
    ArrayOnlyIterableSubContainer &operator= (ArrayOnlyIterableSubContainer &&) = delete;
    ~ArrayOnlyIterableSubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field[index] = element;
    }

    void initIterators() {

        std::size_t index = 0;
        for (auto &element : normal.field) {
            iterators.field[index] = element.begin();
            index++;
        }
        normalIterator.field = normal.field.begin();
        normalConstIterator.field = normal.field.cbegin();
        iteratorsIterator.field = iterators.field.begin();
        iteratorsConstIterator.field = iterators.field.cbegin();
    }

    std::tuple<CollectionInfo<C<IC, N>> &,
               CollectionInfo<typename C<IC, N>::iterator> &,
               CollectionInfo<typename C<IC, N>::const_iterator> &,
               CollectionInfo<C<typename IC::iterator, N>> &,
               CollectionInfo<typename C<typename IC::iterator, N>::iterator> &,
               CollectionInfo<typename C<typename IC::iterator, N>::const_iterator> &>
    getCollectionsInfo() {

        return std::tuple<CollectionInfo<C<IC, N>> &,
                          CollectionInfo<typename C<IC, N>::iterator> &,
                          CollectionInfo<typename C<IC, N>::const_iterator> &,
                          CollectionInfo<C<typename IC::iterator, N>> &,
                          CollectionInfo<typename C<typename IC::iterator, N>::iterator> &,
                          CollectionInfo<typename C<typename IC::iterator, N>::const_iterator> &>(

                normal,
                normalIterator,
                normalConstIterator,
                iterators,
                iteratorsIterator,
                iteratorsConstIterator);
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    CollectionInfo<C<IC, N>> normal;
    CollectionInfo<typename C<IC, N>::iterator> normalIterator;
    CollectionInfo<typename C<IC, N>::const_iterator> normalConstIterator;
    CollectionInfo<C<typename IC::iterator, N>> iterators;
    CollectionInfo<typename C<typename IC::iterator, N>::iterator> iteratorsIterator;
    CollectionInfo<typename C<typename IC::iterator, N>::const_iterator> iteratorsConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::array

#endif  // FINAL_PROJECT_HPC_ARRAYONLYITERABLESUBCONTAINER_H
