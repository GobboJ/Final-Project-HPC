#ifndef FINAL_PROJECT_HPC_POINTERWRAPPER_H
#define FINAL_PROJECT_HPC_POINTERWRAPPER_H

#include "../collections/Alignments.h"
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
template <std::size_t D>
class PointerWrapper {

public:
   /* PointerWrapper() :

    ~PointerCollectionContainer() {

        for (std::size_t i = 0; i < N; i++) {
            delete[] this->cArray[i];
        }
        delete[] this->cArray;
 
    }
private:
    std::vector<T> data;
    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)

    /*double **cArray;
    double *const *cConstArray;
    const double *const *constCConstArray;

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

    OnlyIterableVector<double> *onlyIterable;
    OnlyIterableVector<double>::iterator *iteratorsOnlyIterable;
    OnlyConstIterableVector<double> *onlyConstIterable;
    OnlyConstIterableVector<double>::const_iterator *iteratorsOnlyConstIterable;

    std::vector<int> *integers;
    std::vector<int>::iterator *integersIterator;
    std::vector<int>::const_iterator *integersConstIterator;
    std::filesystem::path *path;
    std::filesystem::path::const_iterator *pathConstIterator;
    std::vector<std::filesystem::path::const_iterator> *pathsVector;
    std::pair<double, double> *pairsArray;
    std::string *string;
    std::string::iterator *stringIterator;
    std::string::const_iterator *stringConstIterator;*/

    // NOLINTEND(misc-non-private-member-variables-in-classes)
};
}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_POINTERWRAPPER_H
