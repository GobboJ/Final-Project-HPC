#ifndef FINAL_PROJECT_HPC_ARRAYCOLLECTIONCONTAINER_H
#define FINAL_PROJECT_HPC_ARRAYCOLLECTIONCONTAINER_H

#include <cstddef>
#include <array>
#include <vector>
#include <list>
#include <deque>
#include "AlignedArray.h"

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

private:
    static const constexpr std::size_t SSE_ALIGNMENT = 16;
    static const constexpr std::size_t AVX_ALIGNMENT = 32;

public:
    C<double *, N> cArray;
    typename C<double *, N>::iterator cArrayIterator;
    typename C<double *, N>::const_iterator cArrayConstIterator;

    C<std::array<double, D>, N> array;
    typename C<std::array<double, D>, N>::iterator arraysIterator;
    typename C<std::array<double, D>, N>::const_iterator arraysConstIterator;
    C<typename std::array<double, D>::iterator, N> iteratorsArray;
    C<typename std::array<double, D>::const_iterator, N> constIteratorsArray;

    C<std::vector<double>, N> vector;
    typename C<std::vector<double>, N>::iterator vectorsIterator;
    typename C<std::vector<double>, N>::const_iterator vectorsConstIterator;
    C<std::vector<double>::iterator, N> iteratorsVector;
    C<std::vector<double>::const_iterator, N> constIteratorsVector;

    C<std::list<double>, N> list;
    typename C<std::list<double>, N>::iterator listsIterator;
    typename C<std::list<double>, N>::const_iterator listsConstIterator;
    C<std::list<double>::iterator, N> iteratorsList;
    C<std::list<double>::const_iterator, N> constIteratorsList;

    C<std::deque<double>, N> deque;
    typename C<std::deque<double>, N>::iterator dequesIterator;
    typename C<std::deque<double>, N>::const_iterator dequeConstIterator;
    C<std::deque<double>::iterator, N> iteratorsDeque;
    C<std::deque<double>::const_iterator, N> constIteratorsDeque;

    C<AlignedArray<double, D, SSE_ALIGNMENT>, N> sseAlignedArray;
    C<AlignedArray<double, D, AVX_ALIGNMENT>, N> avxAlignedArray;
};

}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_ARRAYCOLLECTIONCONTAINER_H
