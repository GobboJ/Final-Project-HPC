#ifndef FINAL_PROJECT_HPC_SUBCONTAINERWRAPPER_H
#define FINAL_PROJECT_HPC_SUBCONTAINERWRAPPER_H

#include "collections/SubContainer.h"
#include "IteratorType.h"
#include "collections/ArrayWrapper.h"
#include <tuple>

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-13
 * @since version date
 */
class SubContainerWrapper {
    using DataIteratorType = utils::DataIteratorType;
private:
    template <typename T>
    struct IsArrayWrapper : std::false_type {};
    
    template <typename T>
    struct IsArrayWrapper<collections::ArrayWrapper<T>> : std::true_type {};
public:
    enum class WrapperType {
        ARRAY_WRAPPER, NORMAL
    };
    
    template <WrapperType WT, template <typename> typename C, typename IC >
    static auto wrapSubContainer(collections::SubContainer<C, IC> &subContainer) {

        /*,
                std::pair<typename C<IC>::iterator &, DataIteratorType>,
                std::pair<typename C<IC>::const_iterator &, DataIteratorType>,
                std::pair<C<typename IC::iterator> &, DataIteratorType>,
                std::pair<typename C<typename IC::iterator>::iterator &, DataIteratorType>,
                std::pair<typename C<typename IC::iterator>::const_iterator &, DataIteratorType>,
                std::pair<C<typename IC::const_iterator> &, DataIteratorType>,
                std::pair<typename C<typename IC::const_iterator>::iterator &, DataIteratorType>,
                std::pair<typename C<typename IC::const_iterator>::const_iterator &,
                          DataIteratorType>*/

        return std::make_tuple(
                wrapReferences<WT>(subContainer.normal,
                               DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_CONST_ITERABLES),
                wrapReferences<WT>(subContainer.normalIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_CONST_ITERABLES),
                wrapReferences<WT>(subContainer.normalConstIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_CONST_ITERABLES),
                wrapReferences<WT>(subContainer.iterators,
                               DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_ITERATORS),
                wrapReferences<WT>(subContainer.iteratorsIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS),
                wrapReferences<WT>(subContainer.iteratorsConstIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS),
                wrapReferences<WT>(subContainer.constIterators,
                               DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_ITERATORS),
                wrapReferences<WT>(subContainer.constIteratorsIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS),
                wrapReferences<WT>(subContainer.constIteratorsConstIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS));
    }

    template <WrapperType WT,template <typename> typename C, typename IC>
    static auto wrapSubContainer(collections::NonIterableContentSubContainer<C, IC> &subContainer) {

        return std::make_tuple(
                wrapReferences<WT>(subContainer.normal,
                               DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_ITERATORS),
                wrapReferences<WT>(subContainer.normalIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS),
                wrapReferences<WT>(subContainer.normalConstIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS));
    }

    template <WrapperType WT,template <typename> typename C, typename IC>
    static auto wrapSubContainer(collections::OnlyConstIterableSubContainer<C, IC> &subContainer) {

        return std::make_tuple(
                wrapReferences<WT>(subContainer.normal,
                               DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_CONST_ITERABLES),
                wrapReferences<WT>(subContainer.normalIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_CONST_ITERABLES),
                wrapReferences<WT>(subContainer.normalConstIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_CONST_ITERABLES),
                wrapReferences<WT>(subContainer.constIterators,
                               DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_ITERATORS),
                wrapReferences<WT>(subContainer.constIteratorsIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS),
                wrapReferences<WT>(subContainer.constIteratorsConstIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS));
    }

    template <WrapperType WT,template <typename> typename C, typename IC>
    static auto wrapSubContainer(collections::OnlyIterableSubContainer<C, IC> &subContainer) {

        return std::make_tuple(
                wrapReferences<WT>(subContainer.normal,
                               DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_CONST_ITERABLES),
                wrapReferences<WT>(subContainer.normalIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_CONST_ITERABLES),
                wrapReferences<WT>(subContainer.normalConstIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_CONST_ITERABLES),
                wrapReferences<WT>(subContainer.iterators,
                               DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_ITERATORS),
                wrapReferences<WT>(subContainer.iteratorsIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS),
                wrapReferences<WT>(subContainer.iteratorsConstIterator,
                               DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS));
    }
    
private:
    template <WrapperType WT, typename T>
    static std::pair<T &, DataIteratorType> wrapReferences(T &arguments, DataIteratorType types) {
        
        if constexpr (IsArrayWrapper<std::remove_reference_t<T>(decltype(arguments))>::value) {
            return std::pair<T &, DataIteratorType>(*(arguments.toArray()), types);
        } else {
            return std::pair<T &, DataIteratorType>(arguments, types);
        }
    }
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_SUBCONTAINERWRAPPER_H
