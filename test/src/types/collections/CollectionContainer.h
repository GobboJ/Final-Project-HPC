#ifndef FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H
#define FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H

#include "AlignedArray.h"
#include "OnlyIterableVector.h"
#include "OnlyConstIterableVector.h"
#include "Alignments.h"
#include "SubContainer.h"
#include "NonIterableContentSubContainer.h"
#include "OnlyConstIterableSubContainer.h"
#include "OnlyIterableSubContainer.h"
#include <array>
#include <cstddef>
#include <deque>
#include <list>
#include <vector>
#include <filesystem>

namespace cluster::test::types::collections {

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
    CollectionContainer() = default;
    CollectionContainer(const CollectionContainer &) = delete;
    CollectionContainer(CollectionContainer &&) = delete;
    CollectionContainer &operator= (const CollectionContainer &) = delete;
    CollectionContainer &operator= (CollectionContainer &&) = delete;

    ~CollectionContainer() {

        for (double *cArray : cArrays) {
            delete[] cArray;
        }
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    NonIterableContentSubContainer<C, double *> cArrays;
    NonIterableContentSubContainer<C, const double *> constCArrays;

    SubContainer<C, std::array<double, D>> arrays;
    SubContainer<C, std::vector<double>> vectors;
    SubContainer<C, std::list<double>> lists;
    SubContainer<C, std::deque<double>> deques;
    SubContainer<C, AlignedArray<double, D, Alignments::SSE_ALIGNMENT>> sseAlignedArrays;
    SubContainer<C, AlignedArray<double, D, Alignments::AVX_ALIGNMENT>> avxAlignedArrays;

    OnlyIterableSubContainer<C, OnlyIterableVector<double>> onlyIterables;
    OnlyConstIterableSubContainer<C, OnlyConstIterableVector<double>> onlyConstIterables;
    
    SubContainer<C, std::vector<int>> integers;
    OnlyConstIterableSubContainer<C, std::filesystem::path> paths;
    SubContainer<C, std::vector<std::filesystem::path::const_iterator>> pathsVector;
    NonIterableContentSubContainer<C, std::pair<double, double>> pairs;
    SubContainer<C, std::string> strings;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::collections
#endif  // FINAL_PROJECT_HPC_COLLECTIONCONTAINER_H
