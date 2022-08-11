#ifndef FINAL_PROJECT_HPC_POINTERCOLLECTIONCONTAINER_H
#define FINAL_PROJECT_HPC_POINTERCOLLECTIONCONTAINER_H

#include "Alignments.h"
#include <cstring>
#include <vector>

namespace cluster::test::types {
/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-08
 * @since version date
 */
template <std::size_t N, std::size_t D>
class PointerCollectionContainer {

public:
    PointerCollectionContainer() :
        cArray{new double *[N]},
        array{new std::array<double, D>[N]},
        iteratorsArray{new typename std::array<double, D>::iterator[N]},
        constIteratorsArray{new typename std::array<double, D>::const_iterator[N]},
        vector{new std::vector<double>[N]},
        iteratorsVector{new std::vector<double>::iterator[N]},
        constIteratorsVector{new std::vector<double>::const_iterator[N]},
        list{new std::list<double>[N]},
        iteratorsList{new std::list<double>::iterator[N]},
        constIteratorsList{new std::list<double>::const_iterator[N]},
        deque{new std::deque<double>[N]},
        iteratorsDeque{new std::deque<double>::iterator[N]},
        constIteratorsDeque{new std::deque<double>::const_iterator[N]},
        sseAlignedArray{new AlignedArray<double, D, Alignments::SSE_ALIGNMENT>[N]},
        avxAlignedArray{new AlignedArray<double, D, Alignments::AVX_ALIGNMENT>[N]} {
    }

    PointerCollectionContainer(const PointerCollectionContainer &other) :
        cArray{new double *[N]},
        array{new std::array<double, D>[N]},
        iteratorsArray{new typename std::array<double, D>::iterator[N]},
        constIteratorsArray{new typename std::array<double, D>::const_iterator[N]},
        vector{new std::vector<double>[N]},
        iteratorsVector{new std::vector<double>::iterator[N]},
        constIteratorsVector{new std::vector<double>::const_iterator[N]},
        list{new std::list<double>[N]},
        iteratorsList{new std::list<double>::iterator[N]},
        constIteratorsList{new std::list<double>::const_iterator[N]},
        deque{new std::deque<double>[N]},
        iteratorsDeque{new std::deque<double>::iterator[N]},
        constIteratorsDeque{new std::deque<double>::const_iterator[N]},
        sseAlignedArray{new AlignedArray<double, D, Alignments::SSE_ALIGNMENT>[N]},
        avxAlignedArray{new AlignedArray<double, D, Alignments::AVX_ALIGNMENT>[N]} {

        memcpy(this->cArray, other->carray, N * sizeof(double *));

        memcpy(this->array, other->array, N * sizeof(std::array<double, D>));
        memcpy(this->iteratorsArray,
               other->iteratorsArray,
               N * sizeof(typename std::array<double, D>::iterator));
        memcpy(this->constIteratorsArray,
               other->constIteratorsArray,
               N * sizeof(typename std::array<double, D>::const_iterator));

        memcpy(this->vector, other->vector, N * sizeof(std::vector<double>));
        memcpy(this->iteratorsVector,
               other->iteratorsVector,
               N * sizeof(std::vector<double>::iterator));
        memcpy(this->constIteratorsVector,
               other->constIteratorsVector,
               N * sizeof(std::vector<double>::const_iterator));

        memcpy(this->list, other->list, N * sizeof(std::list<double>));
        memcpy(this->iteratorsList, other->iteratorsList, N * sizeof(std::list<double>::iterator));
        memcpy(this->constIteratorsList,
               other->constIteratorsList,
               N * sizeof(std::list<double>::const_iterator));

        memcpy(this->deque, other->deque, N * sizeof(std::deque<double>));
        memcpy(this->iteratorsDeque,
               other->iteratorsDeque,
               N * sizeof(std::deque<double>::iterator));
        memcpy(this->constIteratorsDeque,
               other->constIteratorsDeque,
               N * sizeof(std::deque<double>::const_iterator));

        memcpy(this->sseAlignedArray,
               other->sseAlignedArray,
               N * sizeof(AlignedArray<double, D, Alignments::SSE_ALIGNMENT>));
        memcpy(this->avxAlignedArray,
               other->avxAlignedArray,
               N * sizeof(AlignedArray<double, D, Alignments::AVX_ALIGNMENT>));
    }

    PointerCollectionContainer &operator= (const PointerCollectionContainer &other) {
        if (this != &other) {
            memcpy(this->cArray, other->carray, N * sizeof(double *));

            memcpy(this->array, other->array, N * sizeof(std::array<double, D>));
            memcpy(this->iteratorsArray,
                   other->iteratorsArray,
                   N * sizeof(typename std::array<double, D>::iterator));
            memcpy(this->constIteratorsArray,
                   other->constIteratorsArray,
                   N * sizeof(typename std::array<double, D>::const_iterator));

            memcpy(this->vector, other->vector, N * sizeof(std::vector<double>));
            memcpy(this->iteratorsVector,
                   other->iteratorsVector,
                   N * sizeof(std::vector<double>::iterator));
            memcpy(this->constIteratorsVector,
                   other->constIteratorsVector,
                   N * sizeof(std::vector<double>::const_iterator));

            memcpy(this->list, other->list, N * sizeof(std::list<double>));
            memcpy(this->iteratorsList,
                   other->iteratorsList,
                   N * sizeof(std::list<double>::iterator));
            memcpy(this->constIteratorsList,
                   other->constIteratorsList,
                   N * sizeof(std::list<double>::const_iterator));

            memcpy(this->deque, other->deque, N * sizeof(std::deque<double>));
            memcpy(this->iteratorsDeque,
                   other->iteratorsDeque,
                   N * sizeof(std::deque<double>::iterator));
            memcpy(this->constIteratorsDeque,
                   other->constIteratorsDeque,
                   N * sizeof(std::deque<double>::const_iterator));

            memcpy(this->sseAlignedArray,
                   other->sseAlignedArray,
                   N * sizeof(AlignedArray<double, D, Alignments::SSE_ALIGNMENT>));
            memcpy(this->avxAlignedArray,
                   other->avxAlignedArray,
                   N * sizeof(AlignedArray<double, D, Alignments::AVX_ALIGNMENT>));
        }

        return *this;
    }

    ~PointerCollectionContainer() {

        for (std::size_t i = 0; i < N; i++) {
            delete[] this->cArray[i];
        }
        delete[] this->cArray;
        delete[] this->array;
        delete[] this->iteratorsArray;
        delete[] this->constIteratorsArray;
        delete[] this->vector;
        delete[] this->iteratorsVector;
        delete[] this->constIteratorsVector;
        delete[] this->list;
        delete[] this->iteratorsList;
        delete[] this->constIteratorsList;
        delete[] this->deque;
        delete[] this->iteratorsDeque;
        delete[] this->constIteratorsDeque;
        delete[] this->sseAlignedArray;
        delete[] this->avxAlignedArray;
    }

    double **cArray;

    std::array<double, D> *array;
    typename std::array<double, D>::iterator *iteratorsArray;
    typename std::array<double, D>::const_iterator *constIteratorsArray;

    std::vector<double> *vector;
    std::vector<double>::iterator *iteratorsVector;
    std::vector<double>::const_iterator *constIteratorsVector;

    std::list<double> *list;
    std::list<double>::iterator *iteratorsList;
    std::list<double>::const_iterator *constIteratorsList;

    std::deque<double> *deque;
    std::deque<double>::iterator *iteratorsDeque;
    std::deque<double>::const_iterator *constIteratorsDeque;

    AlignedArray<double, D, Alignments::SSE_ALIGNMENT> *sseAlignedArray;
    AlignedArray<double, D, Alignments::AVX_ALIGNMENT> *avxAlignedArray;
};
}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_POINTERCOLLECTIONCONTAINER_H
