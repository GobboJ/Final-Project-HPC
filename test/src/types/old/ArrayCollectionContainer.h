#ifndef FINAL_PROJECT_HPC_ARRAYCOLLECTIONCONTAINER_H
#define FINAL_PROJECT_HPC_ARRAYCOLLECTIONCONTAINER_H

#include "../collections/AlignedArray.h"
#include "../collections/Alignments.h"
#include "../collections/OnlyConstIterableVector.h"
#include <array>
#include <cstddef>
#include <deque>
#include <list>
#include <vector>
#include "../collections/OnlyIterableVector.h"

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-09
 * @since version date
 */
template <template <typename, std::size_t> typename C, std::size_t N, std::size_t D>
class ArrayCollectionContainer {

public:
    /*
    ArrayCollectionContainer() = default;
    ArrayCollectionContainer(const ArrayCollectionContainer &) = delete;
    ArrayCollectionContainer(ArrayCollectionContainer &&) = delete;
    ArrayCollectionContainer &operator= (const ArrayCollectionContainer &) = delete;
    ArrayCollectionContainer &operator= (ArrayCollectionContainer &&) = delete;
    ~ArrayCollectionContainer() = default;

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    C<double *, N> cArrays;
    typename C<double *, N>::iterator cArraysIterator;
    typename C<double *, N>::const_iterator cArraysConstIterator;

    

    C<std::vector<double>, N> vectors;
    typename C<std::vector<double>, N>::iterator vectorsIterator;
    typename C<std::vector<double>, N>::const_iterator vectorsConstIterator;
    C<std::vector<double>::iterator, N> iteratorsVector;
    typename C<std::vector<double>::iterator, N>::iterator iteratorsVectorIterator;
    typename C<std::vector<double>::iterator, N>::const_iterator iteratorsVectorCostIterator;
    C<std::vector<double>::const_iterator, N> constIteratorsVector;
    typename C<std::vector<double>::const_iterator, N>::iterator iteratorConstIteratorsVector;
    typename C<std::vector<double>::const_iterator, N>::const_iterator constIteratorConstIteratorsVector;

    C<std::list<double>, N> list;
    typename C<std::list<double>, N>::iterator listsIterator;
    typename C<std::list<double>, N>::const_iterator listsConstIterator;
    C<std::list<double>::iterator, N> iteratorsList;
    C<std::list<double>::const_iterator, N> constIteratorsList;

    C<std::deque<double>, N> deque;
    typename C<std::deque<double>, N>::iterator dequesIterator;
    typename C<std::deque<double>, N>::const_iterator dequesConstIterator;
    C<std::deque<double>::iterator, N> iteratorsDeque;
    C<std::deque<double>::const_iterator, N> constIteratorsDeque;

    C<AlignedArray<double, D, Alignments::SSE_ALIGNMENT>, N> sseAlignedArray;
    typename C<AlignedArray<double, D, Alignments::SSE_ALIGNMENT>, N>::iterator
            sseAlignedArrayIterator;
    typename C<AlignedArray<double, D, Alignments::SSE_ALIGNMENT>, N>::const_iterator
            sseAlignedArrayConstIterator;
    C<typename AlignedArray<double, D, Alignments::SSE_ALIGNMENT>::iterator, N>
            iteratorsSseAlignedArray;
    C<typename AlignedArray<double, D, Alignments::SSE_ALIGNMENT>::const_iterator, N>
            constIteratorsSseAlignedArray;

    C<AlignedArray<double, D, Alignments::AVX_ALIGNMENT>, N> avxAlignedArray;
    typename C<AlignedArray<double, D, Alignments::AVX_ALIGNMENT>, N>::iterator
            avxAlignedArrayIterator;
    typename C<AlignedArray<double, D, Alignments::AVX_ALIGNMENT>, N>::const_iterator
            avxAlignedArrayConstIterator;
    C<typename AlignedArray<double, D, Alignments::AVX_ALIGNMENT>::iterator, N>
            iteratorsAvxAlignedArray;
    C<typename AlignedArray<double, D, Alignments::AVX_ALIGNMENT>::const_iterator, N>
            constIteratorsAvxAlignedArray;

    C<OnlyIterableVector<double>, N> onlyIterable;
    typename C<OnlyIterableVector<double>, N>::iterator onlyIterableIterator;
    typename C<OnlyIterableVector<double>, N>::const_iterator onlyIterableConstIterator;
    C<OnlyIterableVector<double>::iterator, N> iteratorsOnlyIterable;
    
    
    C<OnlyConstIterableVector<double>, N> onlyConstIterable;
    C<OnlyConstIterableVector<double>::const_iterator, N> iteratorsOnlyConstIterable;

    C<std::vector<int>, N> integers;
    typename C<std::vector<int>, N>::iterator iteratorIntegers;
    typename C<std::vector<int>, N>::const_iterator constIteratorIntegers;
    C<std::vector<int>::iterator, N> integersIterator;
    C<std::vector<int>::const_iterator, N> integersConstIterator;
    C<std::filesystem::path, N> path;
    C<std::filesystem::path::const_iterator, N> pathConstIterator;
    C<std::vector<std::filesystem::path::const_iterator>, N> pathsVector;
    C<std::pair<double, double>, N> pairsArray;
    typename C<std::pair<double, double>, N>::iterator pairsArrayIterator;
    typename C<std::pair<double, double>, N>::const_iterator pairsArrayConstIterator;
    C<std::string, N> string;
    C<std::string::iterator, N> stringIterator;
    C<std::string::const_iterator, N> stringConstIterator;
*/
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_ARRAYCOLLECTIONCONTAINER_H
