#ifndef FINAL_PROJECT_HPC_DATAITERATORUTILS_H
#define FINAL_PROJECT_HPC_DATAITERATORUTILS_H

#include "IteratorType.h"
#include "Types.h"
#include <cstring>
#include <type_traits>

namespace cluster::utils {

/**
 * Utility class providing several methods that allow to iterate over the data structure holding
 * data samples to cluster using pointers as much as possible.<br>
 * For test purposes, this class prints a summary allowing to better identify the selected overload.
 * To enable this feature, just define the <code>ITERATORS_SUMMARY</code> macro, or define the
 * <code>ITERATORS_SUMMARY_TEST</code> one to have a more aligned output. In both cases, you can set
 * <code>DataIteratorUtils::printSummaries</code> to <code>true</code> to enable or disable the
 * printing of the summary at run-time.<br>
 * Moreover, if <code>ITERATORS_SUMMARY_TEST</code> is defined, you can use access
 * <code>DataIteratorUtils::lastDataType</code>,
 * <code>DataIteratorUtils::lastFirstLevelDataType</code> and
 * <code>DataIteratorUtils::lastSecondLevelDataType</code> to retrieve the type of the argument
 * supplied to the <code>DataIteratorUtils::createEfficientIterator</code> method. This is again
 * useful for test purposes.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-08-16
 * @since 1.0
 */
class DataIteratorUtils {

public:
    /**
     * Type of the last specified iterator or iterable holding the first level of indirection of the
     * data samples to cluster. This field is used for test purposes.
     */
    static DataType lastDataType;

    /**
     * Type of the last specified data structure or iterator holding the first level of indirection
     * of the data samples to cluster. This field is used for test purposes.
     */
    static DataLevelType lastFirstLevelDataType;

    /**
     * Type of the last specified data structure or iterator holding the second level of indirection
     * of the data samples to cluster. This field is used for test purposes.
     */
    static DataLevelType lastSecondLevelDataType;

    /**
     * Flag allowing to enable or disable the printing of the summary. This field is used for test
     * purposes.
     */
    static bool printSummaries;

    /***********************************************************************************************
     *
     * Contiguous iterators, contiguous iterables and contiguous const iterables.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the specified one.
     *
     * @tparam D Type of the iterator that iterates over the data.
     * @param iterator Iterator used to iterate over the data.
     * @param name String printed to the console before the type of the supplied iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D, std::enable_if_t<ContiguousIterator<D, const double>, bool> = true>
    static inline const double *createEfficientIterator(const D &iterator, const char *const name) {

        printSummary(name, DataType::CONTIGUOUS, DataLevelType::ITERATOR, DataLevelType::NONE);
        return &(iterator[0]);
    }

    /**
     * Creates a more efficient iterator from the iterator returned by the <code>begin()</code>
     * method, or the <code>cbegin()</code> one if present, called on the specified data structure.
     *
     * @tparam D Type of the data structure to iterate over efficiently.
     * @param data Data structure holding the data to iterate efficiently.
     * @param name String printed to the console before the type of the supplied data structure, so
     * to better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<ContiguousIterable<D, const double> ||
                                       ContiguousConstIterable<D, const double>,
                               bool> = true>
    static inline const double *createEfficientIterator(const D &data, const char *const name) {

        if constexpr (ContiguousConstIterable<D, const double>) {
            printSummary(
                    name, DataType::CONTIGUOUS, DataLevelType::CONST_ITERABLE, DataLevelType::NONE);
            return &(data.cbegin()[0]);
        } else {
            printSummary(name, DataType::CONTIGUOUS, DataLevelType::ITERABLE, DataLevelType::NONE);
            return &(data.begin()[0]);
        }
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I This type parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @param currentElement Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I = const double *,
              std::enable_if_t<ContiguousIterator<D, const double> ||
                                       ContiguousIterable<D, const double> ||
                                       ContiguousConstIterable<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const double *const currentElement) {

        return currentElement;
    }

    /**
     * Returns a pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I This type parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @param startElement Efficient iterator pointing to the first element of the data structure it
     * is iterating over.
     * @param index Index of the element to return.
     * @param stride Number of <code>double</code> values between the first attribute of a data
     * sample and the first attribute of the following one. This is, therefore, the dimension of the
     * data samples, taking also into account possible paddings.
     * @return A pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     */
    template <typename D,
              typename I = const double *,
              std::enable_if_t<ContiguousIterator<D, const double> ||
                                       ContiguousIterable<D, const double> ||
                                       ContiguousConstIterable<D, const double>,
                               bool> = true>
    static inline const double *getSampleAt(
            const double *const startElement, const std::size_t index, const std::size_t stride) {

        return &(startElement[index * stride]);
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I This type parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @param currentElement Efficient iterator pointing to the current element.
     * @param stride Number of <code>double</code> values between the first attribute of a data
     * sample and the first attribute of the following one. This is, therefore, the dimension of the
     * data samples, taking also into account possible paddings.
     */
    template <typename D,
              typename I = const double *,
              std::enable_if_t<ContiguousIterator<D, const double> ||
                                       ContiguousIterable<D, const double> ||
                                       ContiguousConstIterable<D, const double>,
                               bool> = true>
    static inline void moveNext(const double *&currentElement, const std::size_t stride) {

        currentElement += stride;
    }

    /***********************************************************************************************
     *
     * Contiguous iterators of contiguous iterators.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the specified one.
     *
     * @tparam D Type of the iterator that iterates over the data.
     * @param iterator Iterator used to iterate over the data.
     * @param name String printed to the console before the type of the supplied iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<ContiguousIteratorOfIterators<D, const double>, bool> = true>
    static inline auto *createEfficientIterator(const D &iterator, const char *const name) {

        printSummary(name, DataType::CONTIGUOUS, DataLevelType::ITERATOR, DataLevelType::ITERATOR);
        return &(iterator[0]);
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElement Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterators<D, const double>, bool> = true>
    static inline const double *getCurrentSample(const I &currentElement) {

        return &((*currentElement)[0]);
    }

    /**
     * Returns a pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param startElement Efficient iterator pointing to the first element of the data structure it
     * is iterating over.
     * @param index Index of the element to return.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @return A pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterators<D, const double>, bool> = true>
    static inline const double *getSampleAt(const I &startElement,
                                            const std::size_t index,
                                            [[maybe_unused]] const std::size_t stride) {

        return &(startElement[index][0]);
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElement Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterators<D, const double>, bool> = true>
    static inline void moveNext(I &currentElement, [[maybe_unused]] const std::size_t stride) {

        ++currentElement;
    }

    /***********************************************************************************************
     *
     * Contiguous iterators of contiguous iterables and contiguous iterators of contiguous const
     * iterables.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the specified one.
     *
     * @tparam D Type of the iterator that iterates over the data.
     * @param iterator Iterator used to iterate over the data.
     * @param name String printed to the console before the type of the supplied iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> ||
                                       ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline auto *createEfficientIterator(const D &iterator, const char *const name) {

        if constexpr (ContiguousIteratorOfConstIterables<D, const double>) {
            printSummary(name,
                         DataType::CONTIGUOUS,
                         DataLevelType::ITERATOR,
                         DataLevelType::CONST_ITERABLE);
        } else {
            printSummary(
                    name, DataType::CONTIGUOUS, DataLevelType::ITERATOR, DataLevelType::ITERABLE);
        }
        return &(iterator[0]);
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> ||
                                       ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        if constexpr (ContiguousIteratorOfConstIterables<D, const double>) {
            return &(((*currentElementIterator).cbegin())[0]);
        } else {
            return &(((*currentElementIterator).begin())[0]);
        }
    }

    /**
     * Returns a pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param startElementIterator Efficient iterator pointing to the first element of the data
     * structure it is iterating over.
     * @param index Index of the element to return.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @return A pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> ||
                                       ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getSampleAt(const I &startElementIterator,
                                            const std::size_t index,
                                            [[maybe_unused]] const std::size_t stride) {

        if constexpr (ContiguousIteratorOfConstIterables<D, const double>) {
            return &((startElementIterator[index].cbegin())[0]);
        } else {
            return &((startElementIterator[index].begin())[0]);
        }
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> ||
                                       ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Contiguous iterables of contiguous iterators and contiguous const iterables of contiguous
     * iterators.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the iterator returned by the <code>begin()</code>
     * method, or the <code>cbegin()</code> one if present, called on the specified data structure.
     *
     * @tparam D Type of the data structure to iterate over efficiently.
     * @param data Data structure holding the data to iterate efficiently.
     * @param name String printed to the console before the type of the supplied data structure, so
     * to better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<ContiguousIterableOfIterators<D, const double> ||
                                       ContiguousConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline auto *createEfficientIterator(const D &data, const char *const name) {

        if constexpr (ContiguousConstIterableOfIterators<D, const double>) {
            printSummary(name,
                         DataType::CONTIGUOUS,
                         DataLevelType::CONST_ITERABLE,
                         DataLevelType::ITERATOR);
            return &((data.cbegin())[0]);
        } else {
            printSummary(
                    name, DataType::CONTIGUOUS, DataLevelType::ITERABLE, DataLevelType::ITERATOR);
            return &((data.begin())[0]);
        }
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterators<D, const double> ||
                                       ContiguousConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        return &((*currentElementIterator)[0]);
    }

    /**
     * Returns a pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param startElementIterator Efficient iterator pointing to the first element of the data
     * structure it is iterating over.
     * @param index Index of the element to return.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @return A pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterators<D, const double> ||
                                       ContiguousConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline const double *getSampleAt(const I &startElementIterator,
                                            const std::size_t index,
                                            [[maybe_unused]] const std::size_t stride) {

        return &(startElementIterator[index][0]);
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterators<D, const double> ||
                                       ContiguousConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Contiguous iterables of contiguous iterables, contiguous iterables of contiguous const
     * iterables, contiguous const iterables of contiguous iterables and contiguous const iterables
     * of contiguous const iterables.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the iterator returned by the <code>begin()</code>
     * method, or the <code>cbegin()</code> one if present, called on the specified data structure.
     *
     * @tparam D Type of the data structure to iterate over efficiently.
     * @param data Data structure holding the data to iterate efficiently.
     * @param name String printed to the console before the type of the supplied data structure, so
     * to better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<ContiguousIterableOfIterables<D, const double> ||
                                       ContiguousIterableOfConstIterables<D, const double> ||
                                       ContiguousConstIterableOfIterables<D, const double> ||
                                       ContiguousConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline auto *createEfficientIterator(const D &data, const char *const name) {

        if constexpr (ContiguousConstIterableOfIterables<D, const double> ||
                      ContiguousConstIterableOfConstIterables<D, const double>) {

            if constexpr (ContiguousConstIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataType::CONTIGUOUS,
                             DataLevelType::CONST_ITERABLE,
                             DataLevelType::CONST_ITERABLE);
            } else {
                printSummary(name,
                             DataType::CONTIGUOUS,
                             DataLevelType::CONST_ITERABLE,
                             DataLevelType::ITERABLE);
            }
            return &((data.cbegin())[0]);
        } else {
            if constexpr (ContiguousIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataType::CONTIGUOUS,
                             DataLevelType::ITERABLE,
                             DataLevelType::CONST_ITERABLE);
            } else {
                printSummary(name,
                             DataType::CONTIGUOUS,
                             DataLevelType::ITERABLE,
                             DataLevelType::ITERABLE);
            }

            return &((data.begin())[0]);
        }
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterables<D, const double> ||
                                       ContiguousIterableOfConstIterables<D, const double> ||
                                       ContiguousConstIterableOfIterables<D, const double> ||
                                       ContiguousConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        if constexpr (ContiguousIterableOfConstIterables<D, const double> ||
                      ContiguousConstIterableOfConstIterables<D, const double>) {
            return &(((*currentElementIterator).cbegin())[0]);
        } else {
            return &(((*currentElementIterator).begin())[0]);
        }
    }

    /**
     * Returns a pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param startElementIterator Efficient iterator pointing to the first element of the data
     * structure it is iterating over.
     * @param index Index of the element to return.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @return A pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterables<D, const double> ||
                                       ContiguousIterableOfConstIterables<D, const double> ||
                                       ContiguousConstIterableOfIterables<D, const double> ||
                                       ContiguousConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getSampleAt(const I &startElementIterator,
                                            const std::size_t index,
                                            [[maybe_unused]] const std::size_t stride) {

        if constexpr (ContiguousIterableOfConstIterables<D, const double> ||
                      ContiguousConstIterableOfConstIterables<D, const double>) {
            return &((startElementIterator[index].cbegin())[0]);
        } else {
            return &((startElementIterator[index].begin())[0]);
        }
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterables<D, const double> ||
                                       ContiguousIterableOfConstIterables<D, const double> ||
                                       ContiguousConstIterableOfIterables<D, const double> ||
                                       ContiguousConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Random iterators of contiguous iterators.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the specified one.
     *
     * @tparam D Type of the iterator that iterates over the data.
     * @param iterator Iterator used to iterate over the data.
     * @param name String printed to the console before the type of the supplied iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D, std::enable_if_t<RandomIteratorOfIterators<D, const double>, bool> = true>
    static inline D createEfficientIterator(const D &iterator, const char *const name) {

        printSummary(name, DataType::RANDOM, DataLevelType::ITERATOR, DataLevelType::ITERATOR);
        return iterator;
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterators<D, const double>, bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        return &((*currentElementIterator)[0]);
    }

    /**
     * Returns a pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param startElementIterator Efficient iterator pointing to the first element of the data
     * structure it is iterating over.
     * @param index Index of the element to return.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @return A pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterators<D, const double>, bool> = true>
    static inline const double *getSampleAt(const I &startElementIterator,
                                            const std::size_t index,
                                            [[maybe_unused]] const std::size_t stride) {

        return &(startElementIterator[index][0]);
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterators<D, const double>, bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Random iterators of contiguous iterables and random iterators of contiguous const iterables.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the specified one.
     *
     * @tparam D Type of the iterator that iterates over the data.
     * @param iterator Iterator used to iterate over the data.
     * @param name String printed to the console before the type of the supplied iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<RandomIteratorOfIterables<D, const double> ||
                                       RandomIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline D createEfficientIterator(const D &iterator, const char *const name) {

        if constexpr (RandomIteratorOfConstIterables<D, const double>) {
            printSummary(
                    name, DataType::RANDOM, DataLevelType::ITERATOR, DataLevelType::CONST_ITERABLE);
        } else {
            printSummary(name, DataType::RANDOM, DataLevelType::ITERATOR, DataLevelType::ITERABLE);
        }
        return iterator;
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterables<D, const double> ||
                                       RandomIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        if constexpr (RandomIteratorOfConstIterables<D, const double>) {
            return &(((*currentElementIterator).cbegin())[0]);
        } else {
            return &(((*currentElementIterator).begin())[0]);
        }
    }

    /**
     * Returns a pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param startElementIterator Efficient iterator pointing to the first element of the data
     * structure it is iterating over.
     * @param index Index of the element to return.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @return A pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterables<D, const double> ||
                                       RandomIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getSampleAt(const I &startElementIterator,
                                            const std::size_t index,
                                            [[maybe_unused]] const std::size_t stride) {

        if constexpr (RandomIteratorOfConstIterables<D, const double>) {
            return &((startElementIterator[index].cbegin())[0]);
        } else {
            return &((startElementIterator[index].begin())[0]);
        }
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterables<D, const double> ||
                                       RandomIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Random iterables of contiguous iterators and random const iterables of contiguous iterators.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the iterator returned by the <code>begin()</code>
     * method, or the <code>cbegin()</code> one if present, called on the specified data structure.
     *
     * @tparam D Type of the data structure to iterate over efficiently.
     * @param data Data structure holding the data to iterate efficiently.
     * @param name String printed to the console before the type of the supplied data structure, so
     * to better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<RandomIterableOfIterators<D, const double> ||
                                       RandomConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline auto createEfficientIterator(const D &data, const char *const name) {

        if constexpr (RandomConstIterableOfIterators<D, const double>) {
            printSummary(
                    name, DataType::RANDOM, DataLevelType::CONST_ITERABLE, DataLevelType::ITERATOR);
            return data.cbegin();
        } else {
            printSummary(name, DataType::RANDOM, DataLevelType::ITERABLE, DataLevelType::ITERATOR);
            return data.begin();
        }
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterators<D, const double> ||
                                       RandomConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        return &((*currentElementIterator)[0]);
    }

    /**
     * Returns a pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param startElementIterator Efficient iterator pointing to the first element of the data
     * structure it is iterating over.
     * @param index Index of the element to return.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @return A pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterators<D, const double> ||
                                       RandomConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline const double *getSampleAt(const I &startElementIterator,
                                            const std::size_t index,
                                            [[maybe_unused]] const std::size_t stride) {

        return &(startElementIterator[index][0]);
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterators<D, const double> ||
                                       RandomConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Random iterables of contiguous iterables, random iterables of contiguous const iterables,
     * random const iterables of contiguous iterables and random const iterables of contiguous const
     * iterables.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the iterator returned by the <code>begin()</code>
     * method, or the <code>cbegin()</code> one if present, called on the specified data structure.
     *
     * @tparam D Type of the data structure to iterate over efficiently.
     * @param data Data structure holding the data to iterate efficiently.
     * @param name String printed to the console before the type of the supplied data structure, so
     * to better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<RandomIterableOfIterables<D, const double> ||
                                       RandomIterableOfConstIterables<D, const double> ||
                                       RandomConstIterableOfIterables<D, const double> ||
                                       RandomConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline auto createEfficientIterator(const D &data, const char *const name) {

        if constexpr (RandomConstIterableOfIterables<D, const double> ||
                      RandomConstIterableOfConstIterables<D, const double>) {
            if constexpr (RandomConstIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataType::RANDOM,
                             DataLevelType::CONST_ITERABLE,
                             DataLevelType::CONST_ITERABLE);
            } else {
                printSummary(name,
                             DataType::RANDOM,
                             DataLevelType::CONST_ITERABLE,
                             DataLevelType::ITERABLE);
            }
            return data.cbegin();
        } else {
            if constexpr (RandomIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataType::RANDOM,
                             DataLevelType::ITERABLE,
                             DataLevelType::CONST_ITERABLE);
            } else {
                printSummary(
                        name, DataType::RANDOM, DataLevelType::ITERABLE, DataLevelType::ITERABLE);
            }
            return data.begin();
        }
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterables<D, const double> ||
                                       RandomIterableOfConstIterables<D, const double> ||
                                       RandomConstIterableOfIterables<D, const double> ||
                                       RandomConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        if constexpr (RandomIterableOfConstIterables<D, const double> ||
                      RandomConstIterableOfConstIterables<D, const double>) {
            return &(((*currentElementIterator).cbegin())[0]);
        } else {
            return &(((*currentElementIterator).begin())[0]);
        }
    }

    /**
     * Returns a pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param startElementIterator Efficient iterator pointing to the first element of the data
     * structure it is iterating over.
     * @param index Index of the element to return.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     * @return A pointer to the first attribute of the <code>index</code>-th data sample of the data
     * structure the specified efficient iterator is iterating over.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterables<D, const double> ||
                                       RandomIterableOfConstIterables<D, const double> ||
                                       RandomConstIterableOfIterables<D, const double> ||
                                       RandomConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getSampleAt(const I &startElementIterator,
                                            const std::size_t index,
                                            [[maybe_unused]] const std::size_t stride) {

        if constexpr (RandomIterableOfConstIterables<D, const double> ||
                      RandomConstIterableOfConstIterables<D, const double>) {
            return &((startElementIterator[index].cbegin())[0]);
        } else {
            return &((startElementIterator[index].begin())[0]);
        }
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterables<D, const double> ||
                                       RandomIterableOfConstIterables<D, const double> ||
                                       RandomConstIterableOfIterables<D, const double> ||
                                       RandomConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Input iterators of contiguous iterators.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the specified one.
     *
     * @tparam D Type of the iterator that iterates over the data.
     * @param iterator Iterator used to iterate over the data.
     * @param name String printed to the console before the type of the supplied iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D, std::enable_if_t<InputIteratorOfIterators<D, const double>, bool> = true>
    static inline D createEfficientIterator(const D &iterator, const char *const name) {

        printSummary(name, DataType::INPUT, DataLevelType::ITERATOR, DataLevelType::ITERATOR);
        return iterator;
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<InputIteratorOfIterators<D, const double>, bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        return &((*currentElementIterator)[0]);
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<InputIteratorOfIterators<D, const double>, bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Input iterators of contiguous iterables and input iterators of contiguous const iterables.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the specified one.
     *
     * @tparam D Type of the iterator that iterates over the data.
     * @param iterator Iterator used to iterate over the data.
     * @param name String printed to the console before the type of the supplied iterator, so to
     * better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<InputIteratorOfIterables<D, const double> ||
                                       InputIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline D createEfficientIterator(const D &iterator, const char *const name) {

        if constexpr (InputIteratorOfConstIterables<D, const double>) {
            printSummary(
                    name, DataType::INPUT, DataLevelType::ITERATOR, DataLevelType::CONST_ITERABLE);
        } else {
            printSummary(name, DataType::INPUT, DataLevelType::ITERATOR, DataLevelType::ITERABLE);
        }
        return iterator;
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<InputIteratorOfIterables<D, const double> ||
                                       InputIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        if constexpr (InputIteratorOfConstIterables<D, const double>) {
            return &(((*currentElementIterator).cbegin())[0]);
        } else {
            return &(((*currentElementIterator).begin())[0]);
        }
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<InputIteratorOfIterables<D, const double> ||
                                       InputIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Input iterables of contiguous iterators and input const iterables of contiguous iterators.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the iterator returned by the <code>begin()</code>
     * method, or the <code>cbegin()</code> one if present, called on the specified data structure.
     *
     * @tparam D Type of the data structure to iterate over efficiently.
     * @param data Data structure holding the data to iterate efficiently.
     * @param name String printed to the console before the type of the supplied data structure, so
     * to better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<InputIterableOfIterators<D, const double> ||
                                       InputConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline auto createEfficientIterator(const D &data, const char *const name) {

        if constexpr (InputConstIterableOfIterators<D, const double>) {
            printSummary(
                    name, DataType::INPUT, DataLevelType::CONST_ITERABLE, DataLevelType::ITERATOR);
            return data.cbegin();
        } else {
            printSummary(name, DataType::INPUT, DataLevelType::ITERABLE, DataLevelType::ITERATOR);
            return data.begin();
        }
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<InputIterableOfIterators<D, const double> ||
                                       InputConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        return &((*currentElementIterator)[0]);
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<InputIterableOfIterators<D, const double> ||
                                       InputConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

    /***********************************************************************************************
     *
     * Input iterables of contiguous iterables, input iterables of contiguous const iterables, input
     * const iterables of contiguous iterables and input const iterables of contiguous const
     * iterables.
     *
     **********************************************************************************************/

    /**
     * Creates a more efficient iterator from the iterator returned by the <code>begin()</code>
     * method, or the <code>cbegin()</code> one if present, called on the specified data structure.
     *
     * @tparam D Type of the data structure to iterate over efficiently.
     * @param data Data structure holding the data to iterate efficiently.
     * @param name String printed to the console before the type of the supplied data structure, so
     * to better distinguish it among the messages printed to the console. This is useful for test
     * purposes.
     * @return The created iterator.
     */
    template <typename D,
              std::enable_if_t<InputIterableOfIterables<D, const double> ||
                                       InputIterableOfConstIterables<D, const double> ||
                                       InputConstIterableOfIterables<D, const double> ||
                                       InputConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline auto createEfficientIterator(const D &data, const char *const name) {

        if constexpr (InputConstIterableOfIterables<D, const double> ||
                      InputConstIterableOfConstIterables<D, const double>) {
            if constexpr (InputConstIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataType::INPUT,
                             DataLevelType::CONST_ITERABLE,
                             DataLevelType::CONST_ITERABLE);
            } else {
                printSummary(name,
                             DataType::INPUT,
                             DataLevelType::CONST_ITERABLE,
                             DataLevelType::ITERABLE);
            }
            return data.cbegin();
        } else {
            if constexpr (InputIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataType::INPUT,
                             DataLevelType::ITERABLE,
                             DataLevelType::CONST_ITERABLE);
            } else {
                printSummary(
                        name, DataType::INPUT, DataLevelType::ITERABLE, DataLevelType::ITERABLE);
            }
            return data.begin();
        }
    }

    /**
     * Returns a pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the element to return.
     * @return A pointer to the first attribute of the data sample pointed by the specified
     * efficient iterator.
     */
    template <typename D,
              typename I,
              std::enable_if_t<InputIterableOfIterables<D, const double> ||
                                       InputIterableOfConstIterables<D, const double> ||
                                       InputConstIterableOfIterables<D, const double> ||
                                       InputConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentSample(const I &currentElementIterator) {

        if constexpr (InputIterableOfConstIterables<D, const double> ||
                      InputConstIterableOfConstIterables<D, const double>) {
            return &(((*currentElementIterator).cbegin())[0]);
        } else {
            return &(((*currentElementIterator).begin())[0]);
        }
    }

    /**
     * Moves the specified efficient iterator one step forward, so that it points to the next data
     * sample in the data structure the iterator is iterating over.
     *
     * @tparam D Type of the data structure/iterator supplied to the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @tparam I Type of the efficient iterator created invoking the
     * <code>DataIteratorUtils::createEfficientIterator</code> method.
     * @param currentElementIterator Efficient iterator pointing to the current element.
     * @param stride This parameter is ignored. It is defined just to make all the overloads
     * uniform.
     */
    template <typename D,
              typename I,
              std::enable_if_t<InputIterableOfIterables<D, const double> ||
                                       InputIterableOfConstIterables<D, const double> ||
                                       InputConstIterableOfIterables<D, const double> ||
                                       InputConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator,
                                [[maybe_unused]] const std::size_t stride) {

        ++currentElementIterator;
    }

private:
    /**
     *Utility function that prints to the console the type of the data structure or iterator
     * supplied to the <code>DataIteratorUtils::createEfficientIterator</code> method.
     *
     * @param name Name used to distinguish the printed string.
     * @param iteratorType Type of iterator/iterable that holds the first level of indirection of
     * the data structure holding the samples to cluster.
     * @param firstLevel Type of the data structure/iterator that holds the first level of
     * indirection of the data structure holding the samples to cluster.
     * @param secondLevel Type of the data structure/iterator that holds the second level of
     * indirection of the data structure holding the samples to cluster.
     */
    static void printSummary(const char *const name,
                             const DataType iteratorType,
                             const DataLevelType firstLevel,
                             const DataLevelType secondLevel) {

#ifdef ITERATORS_SUMMARY
        // Print a brief summary, if requested
        if (printSummaries) {
            std::cout << name << ": Using" << ' '
                      << DataIteratorTypeUtils::getDescription(
                                 iteratorType, firstLevel, secondLevel)
                      << std::endl;
        }
#endif
#ifdef ITERATORS_SUMMARY_TEST
        // Print an aligned summary, if requested
        if (printSummaries && strcmp("Current data", name) == 0) {
            // Update the type of the data structure
            lastIteratorType = iteratorType;
            lastFirstLevelIteratorType = firstLevel;
            lastSecondLevelIteratorType = secondLevel;
            // Print the summary aligned
            std::string summary =
                    DataIteratorTypeUtils::getDescription(iteratorType, firstLevel, secondLevel);
            std::cout << summary << "\033[" << summary.length() << "D";
            std::cout.flush();
        }

#endif
    }
};

}  // namespace cluster::utils

#endif  // FINAL_PROJECT_HPC_DATAITERATORUTILS_H
