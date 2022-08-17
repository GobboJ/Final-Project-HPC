#ifndef FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H
#define FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H

#include "../../collections/AlignedArray.h"
#include "../../collections/OnlyIterableVector.h"
#include "../../collections/OnlyConstIterableVector.h"
#include "../../collections/Alignments.h"
#include "CollectionSubContainer.h"
#include "CollectionNonIterableSubContainer.h"
#include "CollectionOnlyConstIterableSubContainer.h"
#include "CollectionOnlyIterableSubContainer.h"
#include <array>
#include <cstddef>
#include <deque>
#include <list>
#include <vector>
#include <filesystem>

namespace cluster::test::types::containers::collection {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-09
 * @since version date
 */
template <template <typename> typename C, std::size_t D>
class CollectionContainer {

public:
    CollectionContainer(const std::string &collectionName, utils::DataType iteratorType) :
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

    CollectionContainer(const CollectionContainer &) = delete;
    CollectionContainer(CollectionContainer &&) = delete;
    CollectionContainer &operator= (const CollectionContainer &) = delete;
    CollectionContainer &operator= (CollectionContainer &&) = delete;

    ~CollectionContainer() {

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
    CollectionNonIterableSubContainer<C, double *> cArrays;
    CollectionNonIterableSubContainer<C, const double *> constCArrays;

    CollectionSubContainer<C, std::array<double, D>> arrays;
    CollectionSubContainer<C, std::vector<double>> vectors;
    CollectionSubContainer<C, std::list<double>> lists;
    CollectionSubContainer<C, std::deque<double>> deques;
    CollectionSubContainer<
            C,
            collections::AlignedArray<double, D, collections::Alignments::SSE_ALIGNMENT>>
            sseAlignedArrays;
    CollectionSubContainer<
            C,
            collections::AlignedArray<double, D, collections::Alignments::AVX_ALIGNMENT>>
            avxAlignedArrays;

    CollectionOnlyIterableSubContainer<C, collections::OnlyIterableVector<double>> onlyIterables;
    CollectionOnlyConstIterableSubContainer<C, collections::OnlyConstIterableVector<double>>
            onlyConstIterables;

    CollectionSubContainer<C, std::vector<int>> integers;
    CollectionOnlyConstIterableSubContainer<C, std::filesystem::path> paths;
    CollectionSubContainer<C, std::vector<std::filesystem::path::const_iterator>> pathsVector;
    CollectionNonIterableSubContainer<C, std::pair<double, double>> pairs;
    CollectionSubContainer<C, std::string> strings;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::containers::collection
#endif  // FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H
