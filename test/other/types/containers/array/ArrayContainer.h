#ifndef FINAL_PROJECT_HPC_ARRAYCONTAINER_H
#define FINAL_PROJECT_HPC_ARRAYCONTAINER_H

#include "../../collections/AlignedArray.h"
#include "../../collections/Alignments.h"
#include "../../collections/OnlyConstIterableVector.h"
#include "../../collections/OnlyIterableVector.h"
#include "ArrayNonIterableSubContainer.h"
#include "ArrayOnlyConstIterableSubContainer.h"
#include "ArrayOnlyIterableSubContainer.h"
#include "ArraySubContainer.h"
#include <array>
#include <cstddef>
#include <deque>
#include <list>
#include <vector>

namespace cluster::test::types::containers::array {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-09
 * @since version date
 */
template <template <typename, std::size_t> typename C, std::size_t N, std::size_t D>
class ArrayContainer {

public:
    ArrayContainer(const std::string &collectionName, utils::DataType iteratorType) :
        cArrays{collectionName,
                "double *",
                iteratorType,
                utils::DataLevelType::CONST_ITERABLE,
                utils::DataLevelType::ITERATOR},
        constCArrays{collectionName,
                     "const double *",
                     iteratorType,
                     utils::DataLevelType::CONST_ITERABLE,
                     utils::DataLevelType::ITERATOR},
        arrays{collectionName,
               "std::array<double, D>",
               iteratorType,
               utils::DataLevelType::CONST_ITERABLE,
               utils::DataLevelType::CONST_ITERABLE},
        vectors{collectionName,
                "std::vector<double>",
                iteratorType,
                utils::DataLevelType::CONST_ITERABLE,
                utils::DataLevelType::CONST_ITERABLE},
        lists{collectionName,
              "std::list<double>",
              iteratorType,
              utils::DataLevelType::CONST_ITERABLE,
              utils::DataLevelType::CONST_ITERABLE},
        deques{collectionName,
               "std::deque<double>",
               iteratorType,
               utils::DataLevelType::CONST_ITERABLE,
               utils::DataLevelType::CONST_ITERABLE},
        sseAlignedArrays{collectionName,
                         "AlignedArray<double, D, SSE_ALIGNMENT>",
                         iteratorType,
                         utils::DataLevelType::CONST_ITERABLE,
                         utils::DataLevelType::CONST_ITERABLE},
        avxAlignedArrays{collectionName,
                         "AlignedArray<double, D, AVX_ALIGNMENT>",
                         iteratorType,
                         utils::DataLevelType::CONST_ITERABLE,
                         utils::DataLevelType::CONST_ITERABLE},
        onlyIterables{collectionName,
                      "OnlyIterableVector<double>",
                      iteratorType,
                      utils::DataLevelType::CONST_ITERABLE,
                      utils::DataLevelType::ITERABLE},
        onlyConstIterables{collectionName,
                           "OnlyConstIterableVector<double>",
                           iteratorType,
                           utils::DataLevelType::CONST_ITERABLE,
                           utils::DataLevelType::CONST_ITERABLE},
        integers{collectionName,
                 "std::vector<int>",
                 iteratorType,
                 utils::DataLevelType::CONST_ITERABLE,
                 utils::DataLevelType::CONST_ITERABLE},
        paths{collectionName,
              "std::filesystem::path",
              iteratorType,
              utils::DataLevelType::CONST_ITERABLE,
              utils::DataLevelType::CONST_ITERABLE},
        pathsVector{collectionName,
                    "std::vector<std::filesystem::path::const_iterator>",
                    iteratorType,
                    utils::DataLevelType::CONST_ITERABLE,
                    utils::DataLevelType::CONST_ITERABLE},
        pairs{collectionName,
              "std::pair<double, double>",
              iteratorType,
              utils::DataLevelType::CONST_ITERABLE,
              utils::DataLevelType::NONE},
        strings{collectionName,
                "std::string",
                iteratorType,
                utils::DataLevelType::CONST_ITERABLE,
                utils::DataLevelType::CONST_ITERABLE} {
    }

    ArrayContainer(const ArrayContainer &) = delete;
    ArrayContainer(ArrayContainer &&) = delete;
    ArrayContainer &operator= (const ArrayContainer &) = delete;
    ArrayContainer &operator= (ArrayContainer &&) = delete;

    ~ArrayContainer() {

        for (double *cArray : cArrays) {
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
    ArrayNonIterableSubContainer<C, double *, N> cArrays;
    ArrayNonIterableSubContainer<C, const double *, N> constCArrays;

    ArraySubContainer<C, std::array<double, D>, N> arrays;
    ArraySubContainer<C, std::vector<double>, N> vectors;
    ArraySubContainer<C, std::list<double>, N> lists;
    ArraySubContainer<C, std::deque<double>, N> deques;
    ArraySubContainer<C,
                      collections::AlignedArray<double, D, collections::Alignments::SSE_ALIGNMENT>,
                      N>
            sseAlignedArrays;
    ArraySubContainer<C,
                      collections::AlignedArray<double, D, collections::Alignments::AVX_ALIGNMENT>,
                      N>
            avxAlignedArrays;

    ArrayOnlyIterableSubContainer<C, collections::OnlyIterableVector<double>, N> onlyIterables;
    ArrayOnlyConstIterableSubContainer<C, collections::OnlyConstIterableVector<double>, N>
            onlyConstIterables;

    ArraySubContainer<C, std::vector<int>, N> integers;
    ArrayOnlyConstIterableSubContainer<C, std::filesystem::path, N> paths;
    ArraySubContainer<C, std::vector<std::filesystem::path::const_iterator>, N> pathsVector;
    ArrayNonIterableSubContainer<C, std::pair<double, double>, N> pairs;
    ArraySubContainer<C, std::string, N> strings;

    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::array
#endif  // FINAL_PROJECT_HPC_ARRAYCONTAINER_H
