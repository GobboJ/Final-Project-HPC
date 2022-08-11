#ifndef FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H
#define FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H

#include <array>
#include <cstddef>
#include <deque>
#include <list>
#include <vector>
#include "AlignedArray.h"

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-09
 * @since version date
 */
template <template <typename> typename C, std::size_t D>
class CollectionContainer {

private:
    static const constexpr std::size_t SSE_ALIGNMENT = 16;
    static const constexpr std::size_t AVX_ALIGNMENT = 32;

public:
    C<double *> cArray;
    typename C<double *>::iterator cArrayIterator;
    typename C<double *>::const_iterator cArrayConstIterator;

    C<std::array<double, D>> array;
    typename C<std::array<double, D>>::iterator arraysIterator;
    typename C<std::array<double, D>>::const_iterator arraysConstIterator;
    C<typename std::array<double, D>::iterator> iteratorsArray;
    C<typename std::array<double, D>::const_iterator> constIteratorsArray;

    C<std::vector<double>> vector;
    typename C<std::vector<double>>::iterator vectorsIterator;
    typename C<std::vector<double>>::const_iterator vectorsConstIterator;
    C<std::vector<double>::iterator> iteratorsVector;
    C<std::vector<double>::const_iterator> constIteratorsVector;

    C<std::list<double>> list;
    typename C<std::list<double>>::iterator listsIterator;
    typename C<std::list<double>>::const_iterator listsConstIterator;
    C<std::list<double>::iterator> iteratorsList;
    C<std::list<double>::const_iterator> constIteratorsList;

    C<std::deque<double>> deque;
    typename C<std::deque<double>>::iterator dequesIterator;
    typename C<std::deque<double>>::const_iterator dequesConstIterator;
    C<std::deque<double>::iterator> iteratorsDeque;
    C<std::deque<double>::const_iterator> constIteratorsDeque;

    C<AlignedArray<double, D, SSE_ALIGNMENT>> sseAlignedArray;
    C<AlignedArray<double, D, AVX_ALIGNMENT>> avxAlignedArray;
};

}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H
