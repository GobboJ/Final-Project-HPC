#ifndef FINAL_PROJECT_HPC_LINEARCOLLECTIONCONTAINER_H
#define FINAL_PROJECT_HPC_LINEARCOLLECTIONCONTAINER_H

#include "AlignedArray.h"
#include "Alignments.h"
#include <array>
#include <cstddef>
#include <deque>
#include <list>
#include <string>
#include <tuple>
#include <vector>

namespace cluster::test::types::collections {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-09
 * @since version date
 */
template <std::size_t ND>
class LinearCollectionContainer {

public:
    LinearCollectionContainer() :
        cArray{new double[ND]},
        cConstArray{this->cArray},
        array{},
        sseAlignedArray{},
        avxAlignedArray{} {
    }

    LinearCollectionContainer(const LinearCollectionContainer& other) = delete;

    LinearCollectionContainer(LinearCollectionContainer&& other) = delete;

    LinearCollectionContainer& operator= (const LinearCollectionContainer& other) = delete;

    LinearCollectionContainer& operator= (LinearCollectionContainer&& other) = delete;

    ~LinearCollectionContainer() {
        delete[] this->cArray;
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    double* cArray;
    const double* cConstArray;

    std::array<double, ND> array;
    typename std::array<double, ND>::iterator arrayIterator;
    typename std::array<double, ND>::const_iterator arrayConstIterator;

    std::vector<double> vector;
    std::vector<double>::iterator vectorIterator;
    std::vector<double>::const_iterator vectorConstIterator;

    std::list<double> list;
    std::list<double>::iterator listIterator;
    std::list<double>::const_iterator listConstIterator;

    std::deque<double> deque;
    std::deque<double>::iterator dequeIterator;
    std::deque<double>::const_iterator dequeConstIterator;

    AlignedArray<double, ND, Alignments::SSE_ALIGNMENT> sseAlignedArray;
    typename AlignedArray<double, ND, Alignments::SSE_ALIGNMENT>::iterator sseAlignedArrayIterator;
    typename AlignedArray<double, ND, Alignments::SSE_ALIGNMENT>::const_iterator
            sseAlignedArrayConstIterator;
    AlignedArray<double, ND, Alignments::AVX_ALIGNMENT> avxAlignedArray;
    typename AlignedArray<double, ND, Alignments::AVX_ALIGNMENT>::iterator avxAlignedArrayIterator;
    typename AlignedArray<double, ND, Alignments::AVX_ALIGNMENT>::const_iterator
            avxAlignedArrayConstIterator;

    OnlyIterableVector<double> onlyIterable;
    OnlyIterableVector<double>::iterator onlyIterableIterator;

    OnlyConstIterableVector<double> onlyConstIterable;
    OnlyConstIterableVector<double>::const_iterator onlyConstIterableIterator;

    std::vector<int> integers;
    std::vector<int>::iterator integersIterator;
    std::vector<int>::const_iterator integersConstIterator;
    std::filesystem::path path;
    std::filesystem::path::const_iterator pathConstIterator;
    std::vector<std::filesystem::path::const_iterator> pathsVector;
    std::vector<std::filesystem::path::const_iterator>::iterator pathsVectorIterator;
    std::vector<std::filesystem::path::const_iterator>::const_iterator pathsVectorConstIterator;
    std::pair<double, double> pair;
    std::array<std::pair<double, double>, ND> pairsArray;
    typename std::array<std::pair<double, double>, ND>::iterator pairsArrayIterator;
    typename std::array<std::pair<double, double>, ND>::const_iterator pairsArrayConstIterator;
    std::string string;
    std::string::iterator stringIterator;
    std::string::const_iterator stringConstIterator;

    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::collections

#endif  // FINAL_PROJECT_HPC_LINEARCOLLECTIONCONTAINER_H
