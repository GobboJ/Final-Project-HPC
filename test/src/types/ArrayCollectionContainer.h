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
    std::pair<std::string, C<double *, N>> cArray;
    std::pair<std::string, typename C<double *, N>::const_iterator> cArrayIterator;
    std::pair<std::string, typename C<double *, N>::const_iterator> cArrayConstIterator;

    std::pair<std::string, C<std::array<double, D>, N>> array;
    std::pair<std::string, typename C<std::array<double, D>, N>::iterator> arraysIterator;
    std::pair<std::string, typename C<std::array<double, D>, N>::const_iterator>
            arraysConstIterator;
    std::pair<std::string, C<typename std::array<double, D>::iterator, N>> iteratorsArray;
    std::pair<std::string, C<typename std::array<double, D>::const_iterator, N>>
            constIteratorsArray;

    std::pair<std::string, C<std::vector<double>, N>> vector;
    std::pair<std::string, typename C<std::vector<double>, N>::iterator> vectorsIterator;
    std::pair<std::string, typename C<std::vector<double>, N>::const_iterator> vectorsConstIterator;
    std::pair<std::string, C<std::vector<double>::iterator, N>> iteratorsVector;
    std::pair<std::string, C<std::vector<double>::const_iterator, N>> constIteratorsVector;

    std::pair<std::string, C<std::list<double>, N>> list;
    std::pair<std::string, typename C<std::list<double>, N>::iterator> listsIterator;
    std::pair<std::string, typename C<std::list<double>, N>::const_iterator> listsConstIterator;
    std::pair<std::string, C<std::list<double>::iterator, N>> iteratorsList;
    std::pair<std::string, C<std::list<double>::const_iterator, N>> constIteratorsList;

    std::pair<std::string, C<std::deque<double>, N>> deque;
    std::pair<std::string, typename C<std::deque<double>, N>::iterator> dequesIterator;
    std::pair<std::string, typename C<std::deque<double>, N>::const_iterator> dequeConstIterator;
    std::pair<std::string, C<std::deque<double>::iterator, N>> iteratorsDeque;
    std::pair<std::string, C<std::deque<double>::const_iterator, N>> constIteratorsDeque;

    std::pair<std::string, C<AlignedArray<double, D, SSE_ALIGNMENT>, N>> sseAlignedArray;
    std::pair<std::string, C<AlignedArray<double, D, AVX_ALIGNMENT>, N>> avxAlignedArray;
};

}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_ARRAYCOLLECTIONCONTAINER_H
