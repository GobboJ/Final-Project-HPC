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
    std::pair<std::string, C<double *>> cArray;
    std::pair<std::string, typename C<double *>::iterator> cArrayIterator;
    std::pair<std::string, typename C<double *>::const_iterator> cArrayConstIterator;

    std::pair<std::string, C<std::array<double, D>>> array;
    std::pair<std::string, typename C<std::array<double, D>>::iterator> arraysIterator;
    std::pair<std::string, typename C<std::array<double, D>>::const_iterator> arraysConstIterator;
    std::pair<std::string, C<typename std::array<double, D>::iterator>> iteratorsArray;
    std::pair<std::string, C<typename std::array<double, D>::const_iterator>> constIteratorsArray;

    std::pair<std::string, C<std::vector<double>>> vector;
    std::pair<std::string, typename C<std::vector<double>>::iterator> vectorsIterator;
    std::pair<std::string, typename C<std::vector<double>>::const_iterator> vectorsConstIterator;
    std::pair<std::string, C<std::vector<double>::iterator>> iteratorsVector;
    std::pair<std::string, C<std::vector<double>::const_iterator>> constIteratorsVector;

    std::pair<std::string, C<std::list<double>>> list;
    std::pair<std::string, typename C<std::list<double>>::iterator> listsIterator;
    std::pair<std::string, typename C<std::list<double>>::const_iterator> listsConstIterator;
    std::pair<std::string, C<std::list<double>::iterator>> iteratorsList;
    std::pair<std::string, C<std::list<double>::const_iterator>> constIteratorsList;

    std::pair<std::string, C<std::deque<double>>> deque;
    std::pair<std::string, typename C<std::deque<double>>::iterator> dequesIterator;
    std::pair<std::string, typename C<std::deque<double>>::const_iterator> dequesConstIterator;
    std::pair<std::string, C<std::deque<double>::iterator>> iteratorsDeque;
    std::pair<std::string, C<std::deque<double>::const_iterator>> constIteratorsDeque;

    std::pair<std::string, C<AlignedArray<double, D, SSE_ALIGNMENT>>> sseAlignedArray;
    std::pair<std::string, C<AlignedArray<double, D, AVX_ALIGNMENT>>> avxAlignedArray;
};

}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H
