#ifndef FINAL_PROJECT_HPC_PILAMBDACONTAINER_H
#define FINAL_PROJECT_HPC_PILAMBDACONTAINER_H

#include "AlignedArray.h"
#include "OnlyConstIterableVector.h"
#include "Alignments.h"
#include "OnlyIterableVector.h"
#include <array>
#include <deque>
#include <list>
#include <vector>
#include <filesystem>

namespace cluster::test::types::collections {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-12
 * @since version date
 */
template <typename T, std::size_t N>
class PiLambdaContainer {

public:
    PiLambdaContainer() :
        cArray{new T[N]},
        array{},
        arrayIterator{this->array.begin()},
        arrayConstIterator{this->array.cbegin()},
        twoLevelsArrayIterator(twoLevelsArray.begin()),
        twoLevelsArrayConstIterator(twoLevelsArray.cbegin()),
        path{"/t/u/v/z.txt"},
        pathIterator{path.begin()},
        string{"This is a fake type for both pi and lambda"},
        stringIterator{string.begin()},
        stringConstIterator{string.cbegin()} {

        vector.resize(N);
        vectorIterator = vector.begin();
        vectorConstIterator = vector.cbegin();

        list.resize(N);
        listIterator = list.begin();
        listConstIterator = list.cbegin();

        deque.resize(N);
        dequeIterator = deque.begin();
        dequeConstIterator = deque.cbegin();

        onlyIterable.resize(N);
        onlyIterableIterator = onlyIterable.begin();
        onlyConstIterable.resize(N);
        onlyConstIterableConstIterator = onlyConstIterable.cbegin();

        integers.resize(N);
        integersIterator = integers.begin();
        integersConstIterator = integers.cbegin();

        pathsVector.push_back(path.begin());
        pathsVector.push_back(pathIterator);
    }

    PiLambdaContainer(const PiLambdaContainer&) = delete;
    PiLambdaContainer(PiLambdaContainer&&) = delete;
    PiLambdaContainer& operator= (const PiLambdaContainer&) = delete;
    PiLambdaContainer& operator= (PiLambdaContainer&&) = delete;

    virtual ~PiLambdaContainer() {
        delete[] cArray;
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    T* cArray;
    std::array<T, N> array;
    typename std::array<T, N>::iterator arrayIterator;
    typename std::array<T, N>::const_iterator arrayConstIterator;
    std::vector<T> vector;
    typename std::vector<T>::iterator vectorIterator;
    typename std::vector<T>::const_iterator vectorConstIterator;
    std::list<T> list;
    typename std::list<T>::iterator listIterator;
    typename std::list<T>::const_iterator listConstIterator;
    std::deque<T> deque;
    typename std::deque<T>::iterator dequeIterator;
    typename std::deque<T>::const_iterator dequeConstIterator;

    AlignedArray<T, N, Alignments::SSE_ALIGNMENT> sseArray;
    AlignedArray<T, N, Alignments::AVX_ALIGNMENT> avxArray;

    OnlyIterableVector<T> onlyIterable;
    typename OnlyIterableVector<T>::iterator onlyIterableIterator;
    OnlyConstIterableVector<T> onlyConstIterable;
    typename OnlyConstIterableVector<T>::const_iterator onlyConstIterableConstIterator;

    std::vector<int> integers;
    std::vector<int>::iterator integersIterator;
    std::vector<int>::const_iterator integersConstIterator;
    std::array<std::array<std::size_t, 2>, N> twoLevelsArray;
    typename std::array<std::array<std::size_t, 2>, N>::iterator twoLevelsArrayIterator;
    typename std::array<std::array<std::size_t, 2>, N>::const_iterator twoLevelsArrayConstIterator;

    std::filesystem::path path;
    std::filesystem::path::const_iterator pathIterator;
    std::vector<std::filesystem::path::const_iterator> pathsVector;
    std::pair<T, T> pairsArray;
    std::string string;
    std::string::iterator stringIterator;
    std::string::const_iterator stringConstIterator;

    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::collections

#endif  // FINAL_PROJECT_HPC_PILAMBDACONTAINER_H
