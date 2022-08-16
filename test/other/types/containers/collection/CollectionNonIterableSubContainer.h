#ifndef FINAL_PROJECT_HPC_COLLECTIONNONITERABLESUBCONTAINER_H
#define FINAL_PROJECT_HPC_COLLECTIONNONITERABLESUBCONTAINER_H

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
class CollectionNonIterableSubContainer {

public:
    CollectionNonIterableSubContainer(const std::string &firstLevelName,
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
                            secondLevelIteratorType} {
    }

    CollectionNonIterableSubContainer(const CollectionNonIterableSubContainer &) = delete;
    CollectionNonIterableSubContainer(CollectionNonIterableSubContainer &&) = delete;
    CollectionNonIterableSubContainer &operator= (
            const CollectionNonIterableSubContainer &) = delete;
    CollectionNonIterableSubContainer &operator= (CollectionNonIterableSubContainer &&) = delete;
    ~CollectionNonIterableSubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element, std::size_t index) {

        normal.field.emplace_back(std::forward<E>(element));
    }

    void initIterators() {

        normalIterator.field = normal.field.begin();
        normalConstIterator.field = normal.field.cbegin();
    }

    std::tuple<CollectionInfo<C<IC>> &,
               CollectionInfo<typename C<IC>::iterator> &,
               CollectionInfo<typename C<IC>::const_iterator> &>
    getCollectionsInfo() {

        return std::tuple<CollectionInfo<C<IC>> &,
                          CollectionInfo<typename C<IC>::iterator> &,
                          CollectionInfo<typename C<IC>::const_iterator> &>(

                normal, normalIterator, normalConstIterator);
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    CollectionInfo<C<IC>> normal;
    CollectionInfo<typename C<IC>::iterator> normalIterator;
    CollectionInfo<typename C<IC>::const_iterator> normalConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::collection

#endif  // FINAL_PROJECT_HPC_COLLECTIONNONITERABLESUBCONTAINER_H
