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
    std::pair<std::string, typename C<double *>::const_iterator> cArrayIterator;

    std::pair<std::string, C<std::array<double, D>>> array;
    std::pair<std::string, typename C<std::array<double, D>>::const_iterator> arrayIterator;
    std::pair<std::string, C<typename std::array<double, D>::const_iterator>> iteratorArray;

    std::pair<std::string, C<std::vector<double>>> vector;
    std::pair<std::string, typename C<std::vector<double>>::const_iterator> vectorIterator;
    std::pair<std::string, C<std::vector<double>::const_iterator>> iteratorVector;

    std::pair<std::string, C<std::list<double>>> list;
    std::pair<std::string, typename C<std::list<double>>::const_iterator> listIterator;
    std::pair<std::string, C<std::list<double>::const_iterator>> iteratorList;

    std::pair<std::string, C<std::deque<double>>> deque;
    std::pair<std::string, typename C<std::deque<double>>::const_iterator> dequeIterator;
    std::pair<std::string, C<std::deque<double>::const_iterator>> iteratorDeque;

    std::pair<std::string, C<AlignedArray<double, D, SSE_ALIGNMENT>>> sseAlignedArray;
    std::pair<std::string, C<AlignedArray<double, D, AVX_ALIGNMENT>>> avxAlignedArray;
};

}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H
