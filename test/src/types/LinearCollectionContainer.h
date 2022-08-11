#ifndef FINAL_PROJECT_HPC_LINEARCOLLECTIONCONTAINER_H
#define FINAL_PROJECT_HPC_LINEARCOLLECTIONCONTAINER_H

#include "AlignedArray.h"
#include <array>
#include <cstddef>
#include <deque>
#include <list>
#include <string>
#include <tuple>
#include <vector>

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-09
 * @since version date
 */
template <std::size_t ND>
class LinearCollectionContainer {

private:
    static const constexpr std::size_t SSE_ALIGNMENT = 16;
    static const constexpr std::size_t AVX_ALIGNMENT = 32;

public:
    LinearCollectionContainer() :
        cArray{new double[ND]},
        array{},
        sseAlignedArray{},
        avxAlignedArray{} {
    }

    LinearCollectionContainer(const LinearCollectionContainer& other) :
        cArray{new double[ND]},
        array{other.array},
        arrayIterator{other.arrayIterator},
        arrayConstIterator{other.arrayConstIterator},
        vector{other.vector},
        vectorIterator{other.vectorIterator},
        vectorConstIterator{other.vectorConstIterator},
        list{other.list},
        listIterator{other.listIterator},
        listConstIterator{other.listConstIterator},
        deque{other.deque},
        dequeIterator{other.dequeIterator},
        dequeConstIterator{other.dequeConstIterator},
        sseAlignedArray{other.sseAlignedArray},
        avxAlignedArray{other.avxAlignedArray} {

        memcpy(this->cArray, other.cArray, ND * sizeof(double));
    }

    LinearCollectionContainer& operator= (const LinearCollectionContainer& other) {
        if (this != &other) {
            delete[] this->cArray;
            cArray = new double[ND];
            memcpy(this->cArray, other.cArray, ND * sizeof(double));

            array = other.array;
            arrayIterator = other.arrayIterator;
            arrayConstIterator = other.arrayConstIterator;
            vector = other.vector;
            vectorIterator = other.vectorIterator;
            vectorConstIterator = other.vectorConstIterator;
            list = other.list;
            listIterator = other.listIterator;
            listConstIterator = other.listConstIterator;
            deque = other.deque;
            dequeIterator = other.dequeIterator;
            dequeConstIterator = other.dequeConstIterator;
            sseAlignedArray = other.sseAlignedArray;
            avxAlignedArray = other.avxAlignedArray;
        }
    }

    ~LinearCollectionContainer() {
        delete[] this->cArray;
    }

    double* cArray;

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

    AlignedArray<double, ND, SSE_ALIGNMENT> sseAlignedArray;
    AlignedArray<double, ND, AVX_ALIGNMENT> avxAlignedArray;
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_LINEARCOLLECTIONCONTAINER_H
