/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-09
 * @since version date
 */
#ifndef FINAL_PROJECT_HPC_PILAMBDAITERATORUTILS_H
#define FINAL_PROJECT_HPC_PILAMBDAITERATORUTILS_H

#include "Types.h"
#include <type_traits>

namespace cluster::utils {

class PiLambdaIteratorUtils {
public:
    // cluster::utils::RandomIterator<I, T>
    template <typename T, typename I, std::enable_if_t<ContiguousIterator<I, T>, bool> = true>
    static inline T *createEfficientIterator(I &dataStructure, const char *name) {

        printSummary(name, "Using direct pointers");

        return &(dataStructure[0]);
    }

    template <typename T, typename I, std::enable_if_t<ContiguousIterable<I, T>, bool> = true>
    static inline T *createEfficientIterator(I &dataStructure, const char *name) {

        printSummary(name, "Using direct pointers from normal iterator");
        return &((dataStructure.begin())[0]);
    }

    template <typename T,
              typename I,
              std::enable_if_t<ContiguousIterator<I, T> || ContiguousIterable<I, T>, bool> = true>
    static inline T &getCurrentElement(T *const currentElement) {

        return *currentElement;
    }

    template <typename T,
              typename I,
              std::enable_if_t<ContiguousIterator<I, T> || ContiguousIterable<I, T>, bool> = true>
    static inline T &getElementAt(T *const startElement, const std::size_t index) {

        return startElement[index];
    }

    template <typename T,
              typename I,
              std::enable_if_t<ContiguousIterator<I, T> || ContiguousIterable<I, T>, bool> = true>
    static inline void moveNext(T *&currentElement) {

        ++currentElement;
    }

    /**********************************************************************************************/
    // cluster::utils::RandomIterator<I, T>
    template <typename T,
              typename I,
              std::enable_if_t<RandomIterator<I, T> && !ContiguousIterator<I, T>, bool> = true>
    static inline I createEfficientIterator(I &dataStructureIterator, const char *name) {

        printSummary(name, "Using direct pointers");

        return dataStructureIterator;
    }

    template <typename T,
              typename D,
              std::enable_if_t<RandomIterable<D, T> && !ContiguousIterable<D, T>, bool> = true>
    static inline auto createEfficientIterator(D &dataStructure, const char *name) {

        printSummary(name, "Using direct pointers from normal iterator");
        return dataStructure.begin();
    }

    template <typename T,
              typename I,
              typename EI,
              std::enable_if_t<RandomIterator<I, T> || RandomIterable<I, T>, bool> = true>
    static inline T &getCurrentElement(EI &currentElement) {

        return *currentElement;
    }

    template <typename T,
              typename I,
              typename EI,
              std::enable_if_t<RandomIterator<I, T> || RandomIterable<I, T>, bool> = true>
    static inline T &getElementAt(EI &iteratorAtStartElement, const std::size_t index) {

        return iteratorAtStartElement[index];
    }

    template <typename T,
              typename I,
              typename EI,
              std::enable_if_t<RandomIterator<I, T> || RandomIterable<I, T>, bool> = true>
    static inline void moveNext(EI &currentElement) {

        ++currentElement;
    }

private:
    static inline void printSummary(const char *name, const char *summary) {

#ifdef ITERATORS_SUMMARY
        std::cout << name << ": Using" << ' ' << summary << std::endl;
#endif
    }
};

}  // namespace cluster::utils

#endif  // FINAL_PROJECT_HPC_PILAMBDAITERATORUTILS_H
