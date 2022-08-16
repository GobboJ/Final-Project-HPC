#ifndef FINAL_PROJECT_HPC_ARRAYSUBCONTAINER_H
#define FINAL_PROJECT_HPC_ARRAYSUBCONTAINER_H

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
class ArraySubContainer {

public:
    ArraySubContainer(const std::string &firstLevelName,
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
        iterators{ArrayTypeNameComposer::composeName(firstLevelName, secondLevelName, "iterator"),
                  iteratorType,
                  firstLevelIteratorType,
                  utils::DataLevelIteratorType::ITERATOR},
        iteratorsIterator{ArrayTypeNameComposer::composeName(
                                  firstLevelName, secondLevelName, "iterator", "iterator"),
                          iteratorType,
                          utils::DataLevelIteratorType::ITERATOR,
                          utils::DataLevelIteratorType::ITERATOR},
        iteratorsConstIterator{
                ArrayTypeNameComposer::composeName(
                        firstLevelName, secondLevelName, "iterator", "const_iterator"),
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                utils::DataLevelIteratorType::ITERATOR},
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

    ArraySubContainer(const ArraySubContainer &) = delete;
    ArraySubContainer(ArraySubContainer &&) = delete;
    ArraySubContainer &operator= (const ArraySubContainer &) = delete;
    ArraySubContainer &operator= (ArraySubContainer &&) = delete;
    ~ArraySubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field[index] = element;
    }

    void initIterators() {
        
        std::size_t index = 0;
        for (auto &element : normal.field) {
            iterators.field[index] = element.begin();
            constIterators.field[index] = element.cbegin();
            index++;
        }
        normalIterator.field = normal.field.begin();
        normalConstIterator.field = normal.field.cbegin();
        iteratorsIterator.field = iterators.field.begin();
        iteratorsConstIterator.field = iterators.field.cbegin();
        constIteratorsIterator.field = constIterators.field.begin();
        constIteratorsConstIterator.field = constIterators.field.cbegin();
    }

    std::tuple<CollectionInfo<C<IC, N>> &,
               CollectionInfo<typename C<IC, N>::iterator> &,
               CollectionInfo<typename C<IC, N>::const_iterator> &,
               CollectionInfo<C<typename IC::iterator, N>> &,
               CollectionInfo<typename C<typename IC::iterator, N>::iterator> &,
               CollectionInfo<typename C<typename IC::iterator, N>::const_iterator> &,
               CollectionInfo<C<typename IC::const_iterator, N>> &,
               CollectionInfo<typename C<typename IC::const_iterator, N>::iterator> &,
               CollectionInfo<typename C<typename IC::const_iterator, N>::const_iterator> &>
    getCollectionsInfo() {

        return std::tuple<
                CollectionInfo<C<IC, N>> &,
                CollectionInfo<typename C<IC, N>::iterator> &,
                CollectionInfo<typename C<IC, N>::const_iterator> &,
                CollectionInfo<C<typename IC::iterator, N>> &,
                CollectionInfo<typename C<typename IC::iterator, N>::iterator> &,
                CollectionInfo<typename C<typename IC::iterator, N>::const_iterator> &,
                CollectionInfo<C<typename IC::const_iterator, N>> &,
                CollectionInfo<typename C<typename IC::const_iterator, N>::iterator> &,
                CollectionInfo<typename C<typename IC::const_iterator, N>::const_iterator> &>(

                normal,
                normalIterator,
                normalConstIterator,
                iterators,
                iteratorsIterator,
                iteratorsConstIterator,
                constIterators,
                constIteratorsIterator,
                constIteratorsConstIterator);
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    CollectionInfo<C<IC, N>> normal;
    CollectionInfo<typename C<IC, N>::iterator> normalIterator;
    CollectionInfo<typename C<IC, N>::const_iterator> normalConstIterator;
    CollectionInfo<C<typename IC::iterator, N>> iterators;
    CollectionInfo<typename C<typename IC::iterator, N>::iterator> iteratorsIterator;
    CollectionInfo<typename C<typename IC::iterator, N>::const_iterator> iteratorsConstIterator;
    CollectionInfo<C<typename IC::const_iterator, N>> constIterators;
    CollectionInfo<typename C<typename IC::const_iterator, N>::iterator> constIteratorsIterator;
    CollectionInfo<typename C<typename IC::const_iterator, N>::const_iterator>
            constIteratorsConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::array

#endif  // FINAL_PROJECT_HPC_ARRAYSUBCONTAINER_H
