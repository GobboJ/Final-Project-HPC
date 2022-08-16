#ifndef FINAL_PROJECT_HPC_POINTERCONTAINER_H
#define FINAL_PROJECT_HPC_POINTERCONTAINER_H

#include "../../collections/Alignments.h"
#include "../../collections/AlignedArray.h"
#include "../../collections/OnlyConstIterableVector.h"
#include "../../collections/OnlyIterableVector.h"
#include "PointerNonIterableSubContainer.h"
#include "PointerOnlyConstIterableSubContainer.h"
#include "PointerOnlyIterableSubContainer.h"
#include "PointerSubContainer.h"
#include <cstring>
#include <filesystem>
#include <string>
#include <list>
#include <deque>
#include <vector>

namespace cluster::test::types::containers::pointer {
/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-08
 * @since version date
 */
template <std::size_t N, std::size_t D>
class PointerContainer {

public:
    PointerContainer(const std::string &collectionName, utils::DataIteratorType iteratorType) :
        cArrays{collectionName,
                "double *",
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                utils::DataLevelIteratorType::ITERATOR},
        constCArrays{collectionName,
                     "const double *",
                     iteratorType,
                     utils::DataLevelIteratorType::ITERATOR,
                     utils::DataLevelIteratorType::ITERATOR},
        arrays{collectionName,
               "std::array<double, D>",
               iteratorType,
               utils::DataLevelIteratorType::ITERATOR,
               utils::DataLevelIteratorType::CONST_ITERABLE},
        vectors{collectionName,
                "std::vector<double>",
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                utils::DataLevelIteratorType::CONST_ITERABLE},
        lists{collectionName,
              "std::list<double>",
              iteratorType,
              utils::DataLevelIteratorType::ITERATOR,
              utils::DataLevelIteratorType::CONST_ITERABLE},
        deques{collectionName,
               "std::deque<double>",
               iteratorType,
               utils::DataLevelIteratorType::ITERATOR,
               utils::DataLevelIteratorType::CONST_ITERABLE},
        sseAlignedArrays{collectionName,
                         "AlignedArray<double, D, SSE_ALIGNMENT>",
                         iteratorType,
                         utils::DataLevelIteratorType::ITERATOR,
                         utils::DataLevelIteratorType::CONST_ITERABLE},
        avxAlignedArrays{collectionName,
                         "AlignedArray<double, D, AVX_ALIGNMENT>",
                         iteratorType,
                         utils::DataLevelIteratorType::ITERATOR,
                         utils::DataLevelIteratorType::CONST_ITERABLE},
        onlyIterables{collectionName,
                      "OnlyIterableVector<double>",
                      iteratorType,
                      utils::DataLevelIteratorType::ITERATOR,
                      utils::DataLevelIteratorType::ITERABLE},
        onlyConstIterables{collectionName,
                           "OnlyConstIterableVector<double>",
                           iteratorType,
                           utils::DataLevelIteratorType::ITERATOR,
                           utils::DataLevelIteratorType::CONST_ITERABLE},
        integers{collectionName,
                 "std::vector<int>",
                 iteratorType,
                 utils::DataLevelIteratorType::ITERATOR,
                 utils::DataLevelIteratorType::CONST_ITERABLE},
        paths{collectionName,
              "std::filesystem::path",
              iteratorType,
              utils::DataLevelIteratorType::ITERATOR,
              utils::DataLevelIteratorType::CONST_ITERABLE},
        pathsVector{collectionName,
                    "std::vector<std::filesystem::path::const_iterator>",
                    iteratorType,
                    utils::DataLevelIteratorType::ITERATOR,
                    utils::DataLevelIteratorType::CONST_ITERABLE},
        pairs{collectionName,
              "std::pair<double, double>",
              iteratorType,
              utils::DataLevelIteratorType::ITERATOR,
              utils::DataLevelIteratorType::NONE},
        strings{collectionName,
                "std::string",
                iteratorType,
                utils::DataLevelIteratorType::ITERATOR,
                utils::DataLevelIteratorType::CONST_ITERABLE} {
    }

    PointerContainer(const PointerContainer &) = delete;
    PointerContainer(PointerContainer &&) = delete;
    PointerContainer &operator= (const PointerContainer &) = delete;
    PointerContainer &operator= (PointerContainer &&) = delete;

    ~PointerContainer() {

        for (double *cArray : cArrays.normal) {
            delete[] cArray;
        }
    }

    void addPoint(std::size_t index,
                  double *cArrayPoint,
                  std::array<double, D> &&arrayPoint,
                  std::vector<double> &&vectorPoint,
                  std::list<double> &&listPoint,
                  std::deque<double> &&dequePoint,
                  collections::AlignedArray<double, D, collections::Alignments::SSE_ALIGNMENT>
                          &&sseAlignedArray,
                  collections::AlignedArray<double, D, collections::Alignments::AVX_ALIGNMENT>
                          &&avxAlignedArray,
                  collections::OnlyIterableVector<double> &&onlyIterableVector,
                  collections::OnlyConstIterableVector<double> &&constIterableVector,
                  std::vector<int> &&integerVector) {

        cArrays.emplaceElement(cArrayPoint, index);
        constCArrays.emplaceElement(cArrayPoint, index);

        arrays.emplaceElement(arrayPoint, index);
        vectors.emplaceElement(vectorPoint, index);
        lists.emplaceElement(listPoint, index);
        deques.emplaceElement(dequePoint, index);

        sseAlignedArrays.emplaceElement(sseAlignedArray, index);
        avxAlignedArrays.emplaceElement(avxAlignedArray, index);

        onlyIterables.emplaceElement(onlyIterableVector, index);
        onlyConstIterables.emplaceElement(constIterableVector, index);

        integers.emplaceElement(integerVector, index);
        paths.emplaceElement("/e/f/g/h/a.txt", index);
        pairs.emplaceElement(
                std::pair{static_cast<int>(index) + 4.5, static_cast<int>(index) - 8.09}, index);
        strings.emplaceElement(std::string{"Fake type"}, index);
    }

    void initIterators() {
        std::size_t index = 0;
        for (auto &addedPath : paths.normal.field) {
            std::vector<std::filesystem::path::const_iterator> pathsConstIterators{};
            pathsConstIterators.push_back(addedPath.begin());

            pathsVector.template emplaceElement(pathsConstIterators, index);
        }

        // Set the iterators
        cArrays.initIterators();
        constCArrays.initIterators();

        arrays.initIterators();
        vectors.initIterators();
        lists.initIterators();
        deques.initIterators();
        sseAlignedArrays.initIterators();
        avxAlignedArrays.initIterators();

        onlyIterables.initIterators();
        onlyConstIterables.initIterators();

        integers.initIterators();
        // container.paths.initIterators();
        pathsVector.initIterators();
        pairs.initIterators();
        strings.initIterators();
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    PointerNonIterableSubContainer<double *, N> cArrays;
    PointerNonIterableSubContainer<const double *, N> constCArrays;

    PointerSubContainer<std::array<double, D>, N> arrays;
    PointerSubContainer<std::vector<double>, N> vectors;
    PointerSubContainer<std::list<double>, N> lists;
    PointerSubContainer<std::deque<double>, N> deques;
    PointerSubContainer<
            collections::AlignedArray<double, D, collections::Alignments::SSE_ALIGNMENT>,
            N>
            sseAlignedArrays;
    PointerSubContainer<
            collections::AlignedArray<double, D, collections::Alignments::AVX_ALIGNMENT>,
            N>
            avxAlignedArrays;

    PointerOnlyIterableSubContainer<collections::OnlyIterableVector<double>, N> onlyIterables;
    PointerOnlyConstIterableSubContainer<collections::OnlyConstIterableVector<double>, N>
            onlyConstIterables;

    PointerSubContainer<std::vector<int>, N> integers;
    PointerOnlyConstIterableSubContainer<std::filesystem::path, N> paths;
    PointerSubContainer<std::vector<std::filesystem::path::const_iterator>, N> pathsVector;
    PointerNonIterableSubContainer<std::pair<double, double>, N> pairs;
    PointerSubContainer<std::string, N> strings;

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
}  // namespace cluster::test::types::containers::pointer
#endif  // FINAL_PROJECT_HPC_POINTERCONTAINER_H
