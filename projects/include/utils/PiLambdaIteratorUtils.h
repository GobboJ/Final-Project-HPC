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
    static PiLambdaIteratorType lastPiIteratorType;
    static PiLambdaIteratorType lastLambdaIteratorType;
    static bool printSummaries;

    // cluster::utils::RandomIterator<I, T>
    template <typename T, typename I, std::enable_if_t<ContiguousIterator<I, T>, bool> = true>
    static inline T *createEfficientIterator(I &dataStructure, const char *name) {

        printSummary(name, PiLambdaIteratorType::CONTIGUOUS_ITERATOR);

        return &(dataStructure[0]);
    }

    template <typename T, typename I, std::enable_if_t<ContiguousIterable<I, T>, bool> = true>
    static inline T *createEfficientIterator(I &dataStructure, const char *name) {

        printSummary(name, PiLambdaIteratorType::CONTIGUOUS_ITERABLE);
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

        printSummary(name, PiLambdaIteratorType::RANDOM_ITERATOR);

        return dataStructureIterator;
    }

    template <typename T,
              typename D,
              std::enable_if_t<RandomIterable<D, T> && !ContiguousIterable<D, T>, bool> = true>
    static inline auto createEfficientIterator(D &dataStructure, const char *name) {

        printSummary(name, PiLambdaIteratorType::RANDOM_ITERABLE);
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
    static inline void printSummary(const char *name, PiLambdaIteratorType iteratorType) {

#ifdef ITERATORS_SUMMARY
        std::cout << name << ": Using" << ' ' << PiLambdaIteratorTypeUtils::getDescription(iteratorType)
                  << std::endl;
#endif
#ifdef ITERATORS_SUMMARY_TEST
        if (printSummaries) {
            bool isPi = strcmp("First element of pi", name) == 0;
            bool isLambda = strcmp("First element of lambda", name) == 0;
            if (isPi || isLambda) {
                if (isPi) {
                    lastPiIteratorType = iteratorType;
                } else {
                    lastLambdaIteratorType = iteratorType;
                }
                std::string summary = PiLambdaIteratorTypeUtils::getDescription(iteratorType);
                //    lastIteratorType = iteratorType;

                // Longest string: Contiguous iterator
                std::cout << std::setfill(' ') << std::setw(19) << summary << " | ";
                std::cout.flush();
            }
        }
#endif
    }
};

PiLambdaIteratorType PiLambdaIteratorUtils::lastPiIteratorType = PiLambdaIteratorType::NONE;
PiLambdaIteratorType PiLambdaIteratorUtils::lastLambdaIteratorType = PiLambdaIteratorType::NONE;
bool PiLambdaIteratorUtils::printSummaries = false;
}  // namespace cluster::utils

#endif  // FINAL_PROJECT_HPC_PILAMBDAITERATORUTILS_H
