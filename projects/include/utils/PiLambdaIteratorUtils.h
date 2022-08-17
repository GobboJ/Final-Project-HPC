#ifndef FINAL_PROJECT_HPC_PILAMBDAITERATORUTILS_H
#define FINAL_PROJECT_HPC_PILAMBDAITERATORUTILS_H

#include "IteratorType.h"
#include "Types.h"
#include <type_traits>

namespace cluster::utils {

/**
 * Utility class providing several methods that allow to iterate over the data structures holding pi
 * and lambda using pointers as much as possible.<br>
 * For test purposes, this class prints a summary allowing to better identify the selected overload.
 * To enable this feature, just define the <code>ITERATORS_SUMMARY</code> macro, or define the
 * <code>ITERATORS_SUMMARY_TEST</code> one to have a more aligned output. In both cases, you can set
 * <code>PiLambdaIteratorUtils::printSummaries</code> to <code>true</code> to enable or disable the
 * printing of the summary at run-time.<br>
 * Moreover, if <code>ITERATORS_SUMMARY_TEST</code> is defined, you can use access
 * <code>PiLambdaIteratorUtils::lastPiIteratorType</code> and
 * <code>PiLambdaIteratorUtils::lastLambdaIteratorType</code> to retrieve the type of the argument
 * supplied to the <code>PiLambdaIteratorUtils::createEfficientIterator</code> method. This is again
 * useful for test purposes.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-16
 * @since 1.0
 */
class PiLambdaIteratorUtils {
public:
    /**
     * Type of the last specified data structure or iterator holding <code>pi</code>. This field is
     * used for test purposes.
     */
    static PiLambdaType lastPiType;
    /**
     * Type of the last specified data structure or iterator holding <code>lambda</code>. This field
     * is used for test purposes.
     */
    static PiLambdaType lastLambdaType;

    /**
     * Flag allowing to disable printing the summary. This field is used for test purposes.
     */
    static bool printSummaries;

    /**
     * Using the specified iterator, creates an iterator that iterates over the specified data
     * structure in the most efficient way.
     *
     * @tparam T Type of the elements in the data structure.
     * @tparam I Type of the iterator iterating data structure.
     * @param dataStructureIterator Iterator iterating over the data structure.
     * @param name String printed to the console before the type of the supplied iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename T, typename I, std::enable_if_t<ContiguousIterator<I, T>, bool> = true>
    static inline T *createEfficientIterator(I &dataStructureIterator, const char *const name) {

        printSummary(name, PiLambdaType::CONTIGUOUS_ITERATOR);

        return &(dataStructureIterator[0]);
    }

    /**
     * Creates an iterator that iterates over the specified data structure in the most efficient
     * way.
     *
     * @tparam T Type of the elements in the data structure.
     * @tparam D Type of the data structure.
     * @param dataStructure Data structure the efficient iterator will iterate over.
     * @param name String printed to the console before the type of the supplied data structure, so
     * to better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename T, typename D, std::enable_if_t<ContiguousIterable<D, T>, bool> = true>
    static inline T *createEfficientIterator(D &dataStructure, const char *const name) {

        printSummary(name, PiLambdaType::CONTIGUOUS_ITERABLE);

        return &((dataStructure.begin())[0]);
    }

    /**
     * Returns the element the specified efficient iterator is pointing to.
     *
     * @tparam T Type of the elements in the data structure the efficient iterator is iterating
     * over.
     * @tparam I Type of the argument supplied to the
     * <code>PiLambdaIteratorUtils::createEfficientIterator</code> method.
     * @param currentElement Efficient iterator pointing to the element to return.
     * @return The element the specified efficient iterator is pointing to.
     */
    template <typename T,
              typename I,
              std::enable_if_t<ContiguousIterator<I, T> || ContiguousIterable<I, T>, bool> = true>
    static inline T &getCurrentElement(T *const currentElement) {

        return *currentElement;
    }

    /**
     * Returns the element at the specified index of the data structure the efficient iterator is
     * iterating over.
     *
     * @tparam T Type of the elements in the data structure the efficient iterator is iterating
     * over.
     * @tparam I Type of the argument supplied to the
     * <code>PiLambdaIteratorUtils::createEfficientIterator</code> method.
     * @param startElement Efficient iterator pointing at the first element of the data structure it
     * is iterating over.
     * @param index Index of the element to return.
     * @return The element at the specified index.
     */
    template <typename T,
              typename I,
              std::enable_if_t<ContiguousIterator<I, T> || ContiguousIterable<I, T>, bool> = true>
    static inline T &getElementAt(T *const startElement, const std::size_t index) {

        return startElement[index];
    }

    /**
     * Moves the efficient iterator one step forward, so that it points to the next element of the
     * data structure it is iterating over.
     * @tparam T Type of the elements in the data structure the efficient iterator is iterating
     * over.
     * @tparam I Type of the argument supplied to the
     * <code>PiLambdaIteratorUtils::createEfficientIterator</code> method.
     * @param currentElement Efficient iterator pointing to the current element.
     */
    template <typename T,
              typename I,
              std::enable_if_t<ContiguousIterator<I, T> || ContiguousIterable<I, T>, bool> = true>
    static inline void moveNext(T *&currentElement) {

        ++currentElement;
    }

    /**********************************************************************************************/

    /**
     * Using the specified iterator, creates an iterator that iterates over the specified data
     * structure in the most efficient way.
     *
     * @tparam T Type of the elements in the data structure.
     * @tparam I Type of the iterator iterating data structure.
     * @param dataStructureIterator Iterator iterating over the data structure.
     * @param name String printed to the console before the type of the supplied iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename T,
              typename I,
              std::enable_if_t<RandomIterator<I, T> && !ContiguousIterator<I, T>, bool> = true>
    static inline I createEfficientIterator(I &dataStructureIterator, const char *const name) {

        printSummary(name, PiLambdaType::RANDOM_ITERATOR);

        return dataStructureIterator;
    }

    /**
     * Creates an iterator that iterates over the specified data structure in the most efficient
     * way.
     *
     * @tparam T Type of the elements in the data structure.
     * @tparam D Type of the data structure.
     * @param dataStructure Data structure the efficient iterator will iterate over.
     * @param name String printed to the console before the type of the created iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename T,
              typename D,
              std::enable_if_t<RandomIterable<D, T> && !ContiguousIterable<D, T>, bool> = true>
    static inline auto createEfficientIterator(D &dataStructure, const char *const name) {

        printSummary(name, PiLambdaType::RANDOM_ITERABLE);

        return dataStructure.begin();
    }

    /**
     * Returns the element the specified efficient iterator is pointing to.
     *
     * @tparam T Type of the elements in the data structure the efficient iterator is iterating
     * over.
     * @tparam I Type of the argument supplied to the
     * <code>PiLambdaIteratorUtils::createEfficientIterator</code> method.
     * @tparam EI Type of the specified efficient iterator.
     * @param currentElement Efficient iterator pointing to the element to return.
     * @return The element the specified efficient iterator is pointing to.
     */
    template <typename T,
              typename I,
              typename EI,
              std::enable_if_t<(RandomIterator<I, T> && !ContiguousIterator<I, T>) ||
                                       (RandomIterable<I, T> && !ContiguousIterable<T, T>),
                               bool> = true>
    static inline T &getCurrentElement(EI &currentElement) {

        return *currentElement;
    }

    /**
     * Returns the element at the specified index of the data structure the efficient iterator is
     * iterating over.
     *
     * @tparam T Type of the elements in the data structure the efficient iterator is iterating
     * over.
     * @tparam I Type of the argument supplied to the
     * <code>PiLambdaIteratorUtils::createEfficientIterator</code> method.
     * @tparam EI Type of the specified efficient iterator.
     * @param iteratorAtStartElement Efficient iterator pointing at the first element of the data
     * structure it is iterating over.
     * @param index Index of the element to return.
     * @return The element at the specified index.
     */
    template <typename T,
              typename I,
              typename EI,
              std::enable_if_t<(RandomIterator<I, T> && !ContiguousIterator<I, T>) ||
                                       (RandomIterable<I, T> && !ContiguousIterable<T, T>),
                               bool> = true>
    static inline T &getElementAt(EI &iteratorAtStartElement, const std::size_t index) {

        return iteratorAtStartElement[index];
    }

    /**
     * Moves the efficient iterator one step forward, so that it points to the next element of the
     * data structure it is iterating over.
     * @tparam T Type of the elements in the data structure the efficient iterator is iterating
     * over.
     * @tparam I Type of the argument supplied to the
     * <code>PiLambdaIteratorUtils::createEfficientIterator</code> method.
     * @tparam EI Type of the specified efficient iterator.
     * @param currentElement Efficient iterator pointing to the current element.
     */
    template <typename T,
              typename I,
              typename EI,
              std::enable_if_t<(RandomIterator<I, T> && !ContiguousIterator<I, T>) ||
                                       (RandomIterable<I, T> && !ContiguousIterable<T, T>),
                               bool> = true>
    static inline void moveNext(EI &currentElement) {

        ++currentElement;
    }

private:
    /**
     * Utility function that prints to the console the type of the data structure or iterator
     * supplied to the <code>PiLambdaIteratorUtils::createEfficientIterator</code> method.
     *
     * @param name Name used to distinguish the printed string.
     * @param structureType Type of the data structure of the iterator supplied.
     */
    static inline void printSummary(const char *const name, const PiLambdaType structureType) {

#ifdef ITERATORS_SUMMARY
        // Print a brief summary, if requested
        if (printSummaries) {
            std::cout << name << ": Using" << ' '
                      << PiLambdaIteratorTypeUtils::getDescription(iteratorType) << std::endl;
        }
#endif
#ifdef ITERATORS_SUMMARY_TEST
        // Print an aligned summary, if requested
        if (printSummaries) {
            // Print the summary only the first time the createEfficientIterator is called
            bool isPi = strcmp("First element of pi", name) == 0;
            bool isLambda = strcmp("First element of lambda", name) == 0;
            if (isPi || isLambda) {
                // Update the type of the iterator used
                if (isPi) {
                    lastPiIteratorType = iteratorType;
                } else {
                    lastLambdaIteratorType = iteratorType;
                }
                // Print the summary aligned
                std::string summary = PiLambdaIteratorTypeUtils::getDescription(iteratorType);

                // Longest string: Contiguous iterator
                std::cout << std::setfill(' ') << std::setw(19) << summary << " | ";
                std::cout.flush();
            }
        }
#endif
    }
};
}  // namespace cluster::utils

#endif  // FINAL_PROJECT_HPC_PILAMBDAITERATORUTILS_H
