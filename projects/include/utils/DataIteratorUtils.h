#ifndef FINAL_PROJECT_HPC_DATAITERATORUTILS_H
#define FINAL_PROJECT_HPC_DATAITERATORUTILS_H

#include "Types.h"
#include "IteratorType.h"
#include <type_traits>
#include <cstring>

namespace cluster::utils {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-08
 * @since version date
 */
class DataIteratorUtils {

public:
    static DataIteratorType lastIteratorType;
    static DataLevelIteratorType lastFirstLevelIteratorType;
    static DataLevelIteratorType lastSecondLevelIteratorType;
    static bool printSummaries;

    template <typename D, std::enable_if_t<ContiguousIterator<D, const double>, bool> = true>
    static inline const double *createEfficientIterator(const D &iterator, const char *name) {

        printSummary(name,
                     DataIteratorType::CONTIGUOUS,
                     DataLevelIteratorType::ITERATOR,
                     DataLevelIteratorType::NONE);
        return &(iterator[0]);
    }

    template <typename D,
              std::enable_if_t<ContiguousIterable<D, const double> ||
                                       ContiguousConstIterable<D, const double>,
                               bool> = true>
    static inline const double *createEfficientIterator(const D &data, const char *name) {

        if constexpr (ContiguousConstIterable<D, const double>) {
            printSummary(name,
                         DataIteratorType::CONTIGUOUS,
                         DataLevelIteratorType::CONST_ITERABLE,
                         DataLevelIteratorType::NONE);
            return &(data.cbegin()[0]);
        } else {
            printSummary(name,
                         DataIteratorType::CONTIGUOUS,
                         DataLevelIteratorType::ITERABLE,
                         DataLevelIteratorType::NONE);
            return &(data.begin()[0]);
        }
    }

    template <typename I,
              std::enable_if_t<ContiguousIterator<I, const double> ||
                                       ContiguousIterable<I, const double> ||
                                       ContiguousConstIterable<I, const double>,
                               bool> = true>
    static inline const double *getCurrentElement(const double *const currentElement) {

        return currentElement;
    }

    template <typename I,
              std::enable_if_t<ContiguousIterator<I, const double> ||
                                       ContiguousIterable<I, const double> ||
                                       ContiguousConstIterable<I, const double>,
                               bool> = true>
    static inline const double *getElementAt(
            const double *const startElement, const std::size_t index, std::size_t stride) {

        return &(startElement[index * stride]);
    }

    template <typename I,
              std::enable_if_t<ContiguousIterator<I, const double> ||
                                       ContiguousIterable<I, const double> ||
                                       ContiguousConstIterable<I, const double>,
                               bool> = true>
    static inline void moveNext(const double *&currentElement, std::size_t stride) {

        currentElement += stride;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename D,
              std::enable_if_t<ContiguousIteratorOfIterators<D, const double>, bool> = true>
    static inline auto *createEfficientIterator(const D &iterator, const char *name) {

        printSummary(name,
                     DataIteratorType::CONTIGUOUS,
                     DataLevelIteratorType::ITERATOR,
                     DataLevelIteratorType::ITERATOR);
        return &(iterator[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterators<D, const double>, bool> = true>
    static inline const double *getCurrentElement(const I &currentElement) {

        return &((*currentElement)[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterators<D, const double>, bool> = true>
    static inline const double *getElementAt(
            const I &startElement, const std::size_t index, std::size_t stride) {

        return &(startElement[index][0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterators<D, const double>, bool> = true>
    static inline void moveNext(I &currentElement, std::size_t stride) {

        ++currentElement;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename D,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> ||
                                       ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline auto *createEfficientIterator(const D &data, const char *name) {

        if constexpr (ContiguousIteratorOfConstIterables<D, const double>) {
            printSummary(name,
                         DataIteratorType::CONTIGUOUS,
                         DataLevelIteratorType::ITERATOR,
                         DataLevelIteratorType::CONST_ITERABLE);
        } else {
            printSummary(name,
                         DataIteratorType::CONTIGUOUS,
                         DataLevelIteratorType::ITERATOR,
                         DataLevelIteratorType::ITERABLE);
        }
        return &(data[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> ||
                                       ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentElement(const I &currentElement) {

        if constexpr (ContiguousIteratorOfConstIterables<D, const double>) {
            return &(((*currentElement).cbegin())[0]);
        } else {
            return &(((*currentElement).begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> ||
                                       ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getElementAt(
            const I &startElement, const std::size_t index, std::size_t stride) {

        if constexpr (ContiguousIteratorOfConstIterables<D, const double>) {
            return &((startElement[index].cbegin())[0]);
        } else {
            return &((startElement[index].begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> ||
                                       ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElement, std::size_t stride) {

        ++currentElement;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename D,
              std::enable_if_t<ContiguousIterableOfIterators<D, const double> ||
                                       ContiguousConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline auto *createEfficientIterator(const D &data, const char *name) {

        if constexpr (ContiguousConstIterableOfIterators<D, const double>) {
            printSummary(name,
                         DataIteratorType::CONTIGUOUS,
                         DataLevelIteratorType::CONST_ITERABLE,
                         DataLevelIteratorType::ITERATOR);
            return &((data.cbegin())[0]);
        } else {
            printSummary(name,
                         DataIteratorType::CONTIGUOUS,
                         DataLevelIteratorType::ITERABLE,
                         DataLevelIteratorType::ITERATOR);
            return &((data.begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterators<D, const double> ||
                                       ContiguousConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline const double *getCurrentElement(const I &currentElementIterator) {

        return &((*currentElementIterator)[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterators<D, const double> ||
                                       ContiguousConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline const double *getElementAt(
            const I &startElementIterator, const std::size_t index, std::size_t stride) {

        return &(startElementIterator[index][0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterators<D, const double> ||
                                       ContiguousConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator, std::size_t stride) {

        ++currentElementIterator;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename D,
              std::enable_if_t<ContiguousIterableOfIterables<D, const double> ||
                                       ContiguousIterableOfConstIterables<D, const double> ||
                                       ContiguousConstIterableOfIterables<D, const double> ||
                                       ContiguousConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline auto *createEfficientIterator(const D &data, const char *name) {

        if constexpr (ContiguousConstIterableOfIterables<D, const double> ||
                      ContiguousConstIterableOfConstIterables<D, const double>) {

            if constexpr (ContiguousConstIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataIteratorType::CONTIGUOUS,
                             DataLevelIteratorType::CONST_ITERABLE,
                             DataLevelIteratorType::CONST_ITERABLE);
            } else {
                printSummary(name,
                             DataIteratorType::CONTIGUOUS,
                             DataLevelIteratorType::CONST_ITERABLE,
                             DataLevelIteratorType::ITERABLE);
            }
            return &((data.cbegin())[0]);
        } else {
            if constexpr (ContiguousIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataIteratorType::CONTIGUOUS,
                             DataLevelIteratorType::ITERABLE,
                             DataLevelIteratorType::CONST_ITERABLE);
            } else {
                printSummary(name,
                             DataIteratorType::CONTIGUOUS,
                             DataLevelIteratorType::ITERABLE,
                             DataLevelIteratorType::ITERABLE);
            }

            return &((data.begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterables<D, const double> ||
                                       ContiguousIterableOfConstIterables<D, const double> ||
                                       ContiguousConstIterableOfIterables<D, const double> ||
                                       ContiguousConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentElement(const I &currentElementIterator) {

        if constexpr (ContiguousIterableOfConstIterables<D, const double> ||
                      ContiguousConstIterableOfConstIterables<D, const double>) {
            return &(((*currentElementIterator).cbegin())[0]);
        } else {
            return &(((*currentElementIterator).begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterables<D, const double> ||
                                       ContiguousIterableOfConstIterables<D, const double> ||
                                       ContiguousConstIterableOfIterables<D, const double> ||
                                       ContiguousConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getElementAt(
            const I &startElementIterator, const std::size_t index, std::size_t stride) {

        if constexpr (ContiguousIterableOfConstIterables<D, const double> ||
                      ContiguousConstIterableOfConstIterables<D, const double>) {
            return &((startElementIterator[index].cbegin())[0]);
        } else {
            return &((startElementIterator[index].begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIterableOfIterables<D, const double> ||
                                       ContiguousIterableOfConstIterables<D, const double> ||
                                       ContiguousConstIterableOfIterables<D, const double> ||
                                       ContiguousConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator, std::size_t stride) {

        ++currentElementIterator;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename D, std::enable_if_t<RandomIteratorOfIterators<D, const double>, bool> = true>
    static inline D createEfficientIterator(const D &iterator, const char *name) {

        printSummary(name,
                     DataIteratorType::RANDOM,
                     DataLevelIteratorType::ITERATOR,
                     DataLevelIteratorType::ITERATOR);
        return iterator;
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterators<D, const double>, bool> = true>
    static inline const double *getCurrentElement(const I &currentElementIterator) {

        return &((*currentElementIterator)[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterators<D, const double>, bool> = true>
    static inline const double *getElementAt(
            const I &startElementIterator, const std::size_t index, std::size_t stride) {

        return &(startElementIterator[index][0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterators<D, const double>, bool> = true>
    static inline void moveNext(I &currentElementIterator, std::size_t stride) {

        ++currentElementIterator;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename D,
              std::enable_if_t<RandomIteratorOfIterables<D, const double> ||
                                       RandomIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline D createEfficientIterator(const D &iterator, const char *name) {

        if constexpr (RandomIteratorOfConstIterables<D, const double>) {
            printSummary(name,
                         DataIteratorType::RANDOM,
                         DataLevelIteratorType::ITERATOR,
                         DataLevelIteratorType::CONST_ITERABLE);
        } else {
            printSummary(name,
                         DataIteratorType::RANDOM,
                         DataLevelIteratorType::ITERATOR,
                         DataLevelIteratorType::ITERABLE);
        }
        return iterator;
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterables<D, const double> ||
                                       RandomIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentElement(const I &currentElementIterator) {

        if constexpr (RandomIteratorOfConstIterables<D, const double>) {
            return &(((*currentElementIterator).cbegin())[0]);
        } else {
            return &(((*currentElementIterator).begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterables<D, const double> ||
                                       RandomIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getElementAt(
            const I &startElementIterator, const std::size_t index, std::size_t stride) {

        if constexpr (RandomIteratorOfConstIterables<D, const double>) {
            return &((startElementIterator[index].cbegin())[0]);
        } else {
            return &((startElementIterator[index].begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIteratorOfIterables<D, const double> ||
                                       RandomIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElementIterator, std::size_t stride) {

        ++currentElementIterator;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename D,
              std::enable_if_t<RandomIterableOfIterators<D, const double> ||
                                       RandomConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline auto createEfficientIterator(const D &data, const char *name) {

        if constexpr (RandomConstIterableOfIterators<D, const double>) {
            printSummary(name,
                         DataIteratorType::RANDOM,
                         DataLevelIteratorType::CONST_ITERABLE,
                         DataLevelIteratorType::ITERATOR);
            return data.cbegin();
        } else {
            printSummary(name,
                         DataIteratorType::RANDOM,
                         DataLevelIteratorType::ITERABLE,
                         DataLevelIteratorType::ITERATOR);
            return data.begin();
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterators<D, const double> ||
                                       RandomConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline const double *getCurrentElement(const I &currentElement) {

        return &((*currentElement)[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterators<D, const double> ||
                                       RandomConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline const double *getElementAt(
            const I &startElement, const std::size_t index, std::size_t stride) {

        return &(startElement[index][0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterators<D, const double> ||
                                       RandomConstIterableOfIterators<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElement, std::size_t stride) {

        ++currentElement;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename D,
              std::enable_if_t<RandomIterableOfIterables<D, const double> ||
                                       RandomIterableOfConstIterables<D, const double> ||
                                       RandomConstIterableOfIterables<D, const double> ||
                                       RandomConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline auto createEfficientIterator(const D &data, const char *name) {

        if constexpr (RandomConstIterableOfIterables<D, const double> ||
                      RandomConstIterableOfConstIterables<D, const double>) {
            if constexpr (RandomConstIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataIteratorType::RANDOM,
                             DataLevelIteratorType::CONST_ITERABLE,
                             DataLevelIteratorType::CONST_ITERABLE);
            } else {
                printSummary(name,
                             DataIteratorType::RANDOM,
                             DataLevelIteratorType::CONST_ITERABLE,
                             DataLevelIteratorType::ITERABLE);
            }
            return data.cbegin();
        } else {
            if constexpr (RandomIterableOfConstIterables<D, const double>) {
                printSummary(name,
                             DataIteratorType::RANDOM,
                             DataLevelIteratorType::ITERABLE,
                             DataLevelIteratorType::CONST_ITERABLE);
            } else {
                printSummary(name,
                             DataIteratorType::RANDOM,
                             DataLevelIteratorType::ITERABLE,
                             DataLevelIteratorType::ITERABLE);
            }
            return data.begin();
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterables<D, const double> ||
                                       RandomIterableOfConstIterables<D, const double> ||
                                       RandomConstIterableOfIterables<D, const double> ||
                                       RandomConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentElement(const I &currentElement) {

        if constexpr (RandomIterableOfConstIterables<D, const double> ||
                      RandomConstIterableOfConstIterables<D, const double>) {
            return &(((*currentElement).cbegin())[0]);
        } else {
            return &(((*currentElement).begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterables<D, const double> ||
                                       RandomIterableOfConstIterables<D, const double> ||
                                       RandomConstIterableOfIterables<D, const double> ||
                                       RandomConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getElementAt(
            const I &startElement, const std::size_t index, std::size_t stride) {

        if constexpr (RandomIterableOfConstIterables<D, const double> ||
                      RandomConstIterableOfConstIterables<D, const double>) {
            return &((startElement[index].cbegin())[0]);
        } else {
            return &((startElement[index].begin())[0]);
        }
    }

    template <typename D,
              typename I,
              std::enable_if_t<RandomIterableOfIterables<D, const double> ||
                                       RandomIterableOfConstIterables<D, const double> ||
                                       RandomConstIterableOfIterables<D, const double> ||
                                       RandomConstIterableOfConstIterables<D, const double>,
                               bool> = true>
    static inline void moveNext(I &currentElement, std::size_t stride) {

        ++currentElement;
    }

private:
    static void printSummary(const char *name,
                             DataIteratorType iteratorType,
                             DataLevelIteratorType firstLevel,
                             DataLevelIteratorType secondLevel) {

#ifdef ITERATORS_SUMMARY
        std::cout << name << ": Using" << ' ' << DataIteratorTypeUtils::getDescription(iteratorType)
                  << std::endl;
#endif
#ifdef ITERATORS_SUMMARY_TEST
        if (printSummaries && strcmp("Current data", name) == 0) {
            std::string summary =
                    DataIteratorTypeUtils::getDescription(iteratorType, firstLevel, secondLevel);
            lastIteratorType = iteratorType;
            lastFirstLevelIteratorType = firstLevel;
            lastSecondLevelIteratorType = secondLevel;
            std::cout << summary << "\033[" << summary.length() << "D";
            std::cout.flush();
        }

#endif
    }

    /*


    template <typename I, std::enable_if_t<IndirectRandomAccessIterator<I, T>, bool> = true>
    static inline I createEfficientIterator(const I &iterator, const char *name) {

#ifdef ITERATORS_SUMMARY
        std::cout << name << ": Using random access iterator over iterators" << std::endl;
#endif

        return iterator;
    }

    template <typename I, std::enable_if_t<IndirectRandomAccessIterable<I, T>, bool> = true>
    static inline I createEfficientIterator(const I &iterator, const char *name) {

#ifdef ITERATORS_SUMMARY
        std::cout << name << ": Using random access iterator over iterables" << std::endl;
#endif

        return iterator;
    }

    template <typename I, std::enable_if_t<IndirectContiguousAccessIterator<I, T>, bool> = true>
    static inline const T * createEfficientIterator(const I &iterator, const char *name) {

#ifdef ITERATORS_SUMMARY
        std::cout << name << ": Using indirect pointers" << std::endl;
#endif

        return &((*iterator)[0]);
    }

    template <typename I, std::enable_if_t<IndirectContiguousAccessIterable<I, T>, bool> = true>
    static inline T *createEfficientIterator(const I &iterator, const char *name) {

#ifdef ITERATORS_SUMMARY
        std::cout << name << ": Using element iterator as indirect pointers" << std::endl;
#endif

        return &(((*iterator).begin())[0]);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename I, typename EI, std::enable_if_t<ContiguousIterator<I, T>, bool> = true>
    static inline T *getCurrent(const EI &currentElement) {

        return currentElement;
    }


    template <typename I,
              typename EI,
              std::enable_if_t<IndirectRandomAccessIterator<I, T>, bool> = true>
    static inline T *getCurrent(const EI &currentElement) {

        return *currentElement;
    }

    template <typename I,
              typename EI,
              std::enable_if_t<IndirectRandomAccessIterable<I, T>, bool> = true>
    static inline T *getCurrent(const EI &currentElement) {

        return &(((*currentElement).begin())[0]);
    }

    template <typename I,
              typename EI,
              std::enable_if_t<IndirectContiguousAccessIterator<I, T>, bool> = true>
    static inline const T *getCurrent(const EI &currentElement) {

        return currentElement;
    }

    template <typename I,
              typename EI,
              std::enable_if_t<IndirectContiguousAccessIterable<I, T>, bool> = true>
    static inline T *getCurrent(const EI &currentElement) {

        return currentElement;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename I, typename EI, std::enable_if_t<ContiguousIterator<I, T>, bool> = true>
    static inline T *getElement(const EI &startElement, std::size_t i, std::size_t stride) {

        return &(startElement[i * stride]);
    }


        template <typename I,
                  typename EI,
                  std::enable_if_t<IndirectRandomAccessIterator<I, T>, bool> = true>
        static inline T *getElement(const EI &efficientIterator, std::size_t i, std::size_t stride)
       {

            return efficientIterator[i];
        }

        template <typename I,
                  typename EI,
                  std::enable_if_t<IndirectRandomAccessIterable<I, T>, bool> = true>
        static inline T *getElement(const EI &efficientIterator, std::size_t i, std::size_t stride)
       {

            return &((efficientIterator[i].begin())[0]);
        }

        template <typename I,
                  typename EI,
                  std::enable_if_t<IndirectContiguousAccessIterator<I, T>, bool> = true>
        static inline const T *getElement(const EI &startElement, std::size_t i, std::size_t stride)
       {

            return &(startElement[i]);
        }

        template <typename I,
                  typename EI,
                  std::enable_if_t<IndirectContiguousAccessIterable<I, T>, bool> = true>
        static inline T *getElement(const EI &startElement, std::size_t i, std::size_t stride) {

            return &(startElement[i]);
        }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename I, typename EI, std::enable_if_t<ContiguousIterator<I, T>, bool> = true>
    static inline void moveNext(EI &currentElement, const std::size_t stride) {

        currentElement += stride;
    }


        template <typename I,
                  typename EI,
                  std::enable_if_t<IndirectRandomAccessIterator<I, T>, bool> = true>
        static inline void moveNext(EI &iterator, const std::size_t stride) {

            ++iterator;
        }

        template <typename I,
                  typename EI,
                  std::enable_if_t<IndirectRandomAccessIterable<I, T>, bool> = true>
        static inline void moveNext(EI &iterator, const std::size_t stride) {

            ++iterator;
        }

        template <typename I,
                  typename EI,
                  std::enable_if_t<IndirectContiguousAccessIterator<I, T>, bool> = true>
        static inline void moveNext(EI &currentElement, const std::size_t stride) {

            ++currentElement;
        }

        template <typename I,
                  typename EI,
                  std::enable_if_t<IndirectContiguousAccessIterable<I, T>, bool> = true>
        static inline void moveNext(EI &currentElement, const std::size_t stride) {

            ++currentElement;
        }*/
};

DataIteratorType DataIteratorUtils::lastIteratorType = DataIteratorType::NONE;
DataLevelIteratorType DataIteratorUtils::lastFirstLevelIteratorType = DataLevelIteratorType::NONE;
DataLevelIteratorType DataIteratorUtils::lastSecondLevelIteratorType = DataLevelIteratorType::NONE;
bool DataIteratorUtils::printSummaries = false;
}  // namespace cluster::utils

#endif  // FINAL_PROJECT_HPC_DATAITERATORUTILS_H
