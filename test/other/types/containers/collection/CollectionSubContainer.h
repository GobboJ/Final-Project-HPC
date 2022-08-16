#ifndef FINAL_PROJECT_HPC_COLLECTIONSUBCONTAINER_H
#define FINAL_PROJECT_HPC_COLLECTIONSUBCONTAINER_H

#include "../CollectionInfo.h"
#include "CollectionTypeNameComposer.h"
#include <utility>

namespace cluster::test::types::containers::collection {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-13
 * @since version date
 */
template <template <typename> typename C, typename IC>
class CollectionSubContainer {

public:
    CollectionSubContainer(const std::string &firstLevelName,
                           const std::string &secondLevelName,
                           utils::DataIteratorType iteratorType,
                           utils::DataLevelIteratorType firstLevelIteratorType,
                           utils::DataLevelIteratorType secondLevelIteratorType) :
        normal{CollectionTypeNameComposer::composeName(firstLevelName, secondLevelName),
               iteratorType,
               firstLevelIteratorType,
               secondLevelIteratorType},
        normalIterator{CollectionTypeNameComposer::composeName(
                               firstLevelName, secondLevelName, "", "iterator"),
                       iteratorType,
                       utils::DataLevelIteratorType::ITERATOR,
                       secondLevelIteratorType},
        normalConstIterator{CollectionTypeNameComposer::composeName(
                                    firstLevelName, secondLevelName, "", "const_iterator"),
                            iteratorType,
                            utils::DataLevelIteratorType::ITERATOR,
                            secondLevelIteratorType},
        iterators{CollectionTypeNameComposer::composeName(
                          firstLevelName, secondLevelName, "iterator"),
                  iteratorType,
                  firstLevelIteratorType,
                  utils::DataLevelIteratorType::ITERATOR},
        iteratorsIterator{CollectionTypeNameComposer::composeName(
                                  firstLevelName, secondLevelName, "iterator", "iterator"),
                          iteratorType,
                          utils::DataLevelIteratorType::ITERATOR,
                          utils::DataLevelIteratorType::ITERATOR},
        iteratorsConstIterator{
                CollectionTypeNameComposer::composeName(
                        firstLevelName, secondLevelName, "iterator", "const_iterator"),
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                utils::DataLevelIteratorType::ITERATOR},
        constIterators{CollectionTypeNameComposer::composeName(
                               firstLevelName, secondLevelName, "const_iterator"),
                       iteratorType,
                       firstLevelIteratorType,
                       utils::DataLevelIteratorType::ITERATOR},
        constIteratorsIterator{
                CollectionTypeNameComposer::composeName(
                        firstLevelName, secondLevelName, "const_iterator", "iterator"),
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                utils::DataLevelIteratorType::ITERATOR},
        constIteratorsConstIterator{
                CollectionTypeNameComposer::composeName(
                        firstLevelName, secondLevelName, "const_iterator", "const_iterator"),
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                utils::DataLevelIteratorType::ITERATOR}

    {
    }

    CollectionSubContainer(const CollectionSubContainer &) = delete;
    CollectionSubContainer(CollectionSubContainer &&) = delete;
    CollectionSubContainer &operator= (const CollectionSubContainer &) = delete;
    CollectionSubContainer &operator= (CollectionSubContainer &&) = delete;
    ~CollectionSubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field.emplace_back(std::forward<E>(element));
    }

    void initIterators() {

        for (auto &element : normal.field) {
            iterators.field.emplace_back(element.begin());
            constIterators.field.emplace_back(element.cbegin());
        }
        normalIterator.field = normal.field.begin();
        normalConstIterator.field = normal.field.cbegin();
        iteratorsIterator.field = iterators.field.begin();
        iteratorsConstIterator.field = iterators.field.cbegin();
        constIteratorsIterator.field = constIterators.field.begin();
        constIteratorsConstIterator.field = constIterators.field.cbegin();
    }

    std::tuple<CollectionInfo<C<IC>> &,
               CollectionInfo<typename C<IC>::iterator> &,
               CollectionInfo<typename C<IC>::const_iterator> &,
               CollectionInfo<C<typename IC::iterator>> &,
               CollectionInfo<typename C<typename IC::iterator>::iterator> &,
               CollectionInfo<typename C<typename IC::iterator>::const_iterator> &,
               CollectionInfo<C<typename IC::const_iterator>> &,
               CollectionInfo<typename C<typename IC::const_iterator>::iterator> &,
               CollectionInfo<typename C<typename IC::const_iterator>::const_iterator> &>
    getCollectionsInfo() {

        return std::tuple<
                CollectionInfo<C<IC>> &,
                CollectionInfo<typename C<IC>::iterator> &,
                CollectionInfo<typename C<IC>::const_iterator> &,
                CollectionInfo<C<typename IC::iterator>> &,
                CollectionInfo<typename C<typename IC::iterator>::iterator> &,
                CollectionInfo<typename C<typename IC::iterator>::const_iterator> &,
                CollectionInfo<C<typename IC::const_iterator>> &,
                CollectionInfo<typename C<typename IC::const_iterator>::iterator> &,
                CollectionInfo<typename C<typename IC::const_iterator>::const_iterator> &>(

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
    CollectionInfo<C<IC>> normal;
    CollectionInfo<typename C<IC>::iterator> normalIterator;
    CollectionInfo<typename C<IC>::const_iterator> normalConstIterator;
    CollectionInfo<C<typename IC::iterator>> iterators;
    CollectionInfo<typename C<typename IC::iterator>::iterator> iteratorsIterator;
    CollectionInfo<typename C<typename IC::iterator>::const_iterator> iteratorsConstIterator;
    CollectionInfo<C<typename IC::const_iterator>> constIterators;
    CollectionInfo<typename C<typename IC::const_iterator>::iterator> constIteratorsIterator;
    CollectionInfo<typename C<typename IC::const_iterator>::const_iterator>
            constIteratorsConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::collection

#endif  // FINAL_PROJECT_HPC_COLLECTIONSUBCONTAINER_H
