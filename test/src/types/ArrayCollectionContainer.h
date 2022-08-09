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

    std::pair<std::string, C<std::array<double, D>, N>> array;
    std::pair<std::string, typename C<std::array<double, D>, N>::const_iterator> arrayIterator;
    std::pair<std::string, C<typename std::array<double, D>::const_iterator, N>> iteratorArray;

    std::pair<std::string, C<std::vector<double>, N>> vector;
    std::pair<std::string, typename C<std::vector<double>, N>::const_iterator> vectorIterator;
    std::pair<std::string, C<std::vector<double>::const_iterator, N>> iteratorVector;

    std::pair<std::string, C<std::list<double>, N>> list;
    std::pair<std::string, typename C<std::list<double>, N>::const_iterator> listIterator;
    std::pair<std::string, C<std::list<double>::const_iterator, N>> iteratorList;

    std::pair<std::string, C<std::deque<double>, N>> deque;
    std::pair<std::string, typename C<std::deque<double>, N>::const_iterator> dequeIterator;
    std::pair<std::string, C<std::deque<double>::const_iterator, N>> iteratorDeque;

    std::pair<std::string, C<AlignedArray<double, D, SSE_ALIGNMENT>, N>> sseAlignedArray;
    std::pair<std::string, C<AlignedArray<double, D, AVX_ALIGNMENT>, N>> avxAlignedArray;
};

}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_ARRAYCOLLECTIONCONTAINER_H
