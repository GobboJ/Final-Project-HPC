#ifndef FINAL_PROJECT_HPC_COLLECTIONINFO_H
#define FINAL_PROJECT_HPC_COLLECTIONINFO_H

#include "IteratorType.h"

namespace cluster::test::types::containers {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-13
 * @since version date
 */
template <typename T>
class CollectionInfo {

public:
    CollectionInfo(std::string name,
                   utils::DataIteratorType iteratorType,
                   utils::DataLevelIteratorType firstLevelIteratorType,
                   utils::DataLevelIteratorType secondLevelIteratorType) :
        name(name),
        iteratorType(iteratorType),
        firstLevelIteratorType(firstLevelIteratorType),
        secondLevelIteratorType(secondLevelIteratorType),
        field{} {
    }

    std::string name;
    utils::DataIteratorType iteratorType;
    utils::DataLevelIteratorType firstLevelIteratorType;
    utils::DataLevelIteratorType secondLevelIteratorType;
    T field;
};

}  // namespace cluster::test::types::containers

#endif  // FINAL_PROJECT_HPC_COLLECTIONINFO_H
