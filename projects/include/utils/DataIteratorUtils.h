#ifndef FINAL_PROJECT_HPC_DATAITERATORUTILS_H
#define FINAL_PROJECT_HPC_DATAITERATORUTILS_H

#include "Types.h"
#include <type_traits>

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
    template <typename D, std::enable_if_t<ContiguousIterator<D, const double>, bool> = true>
    static inline const double *createEfficientIterator(const D &iterator, const char *name) {

        printSummary(name, "Contiguous iterator");
        return &(iterator[0]);
    }

    template <typename D,
              std::enable_if_t<ContiguousIterable<D, const double> ||
                                       ContiguousConstIterable<D, const double>,
                               bool> = true>
    static inline const double *createEfficientIterator(const D &data, const char *name) {

        if constexpr (
                ContiguousIterable<D, const double> && !ContiguousConstIterable<D, const double>) {
            printSummary(name, "Contiguous iterable");
            return &(data.begin()[0]);
        } else {
            printSummary(name, "Contiguous const iterable");
            return &(data.cbegin()[0]);
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

        printSummary(name, "Contiguous iterator of iterators");
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

        if constexpr (ContiguousIteratorOfIterables<D, const double> &&
                      !ContiguousIteratorOfConstIterables<D, const double>) {
            printSummary(name, "Contiguous iterator of iterables");
        } else {
            printSummary(name, "Contiguous iterator of const iterables");
        }
        return &(data[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> &&
                                       !ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getCurrentElement(const I &currentElement) {

        return &(((*currentElement).begin())[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfConstIterables<D, const double>, bool> = true>
    static inline const double *getCurrentElement(const I &currentElement) {

        return &(((*currentElement).cbegin())[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfIterables<D, const double> &&
                                       !ContiguousIteratorOfConstIterables<D, const double>,
                               bool> = true>
    static inline const double *getElementAt(
            const I &startElement, const std::size_t index, std::size_t stride) {

        return &((startElement[index].begin())[0]);
    }

    template <typename D,
              typename I,
              std::enable_if_t<ContiguousIteratorOfConstIterables<D, const double>, bool> = true>
    static inline const double *getElementAt(
            const I &startElement, const std::size_t index, std::size_t stride) {

        return &((startElement[index].cbegin())[0]);
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

        if constexpr (ContiguousIterableOfIterators<D, const double> &&
                      !ContiguousConstIterableOfIterators<D, const double>) {
            printSummary(name, "Contiguous iterable of iterators");
            return &((data.begin())[0]);
        } else {
            printSummary(name, "Contiguous const iterable of iterators");
            return &((data.cbegin())[0]);
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

        if constexpr ((ContiguousIterableOfIterables<D, const double> &&
                       !ContiguousConstIterableOfIterables<D, const double>) ||
                      (ContiguousIterableOfConstIterables<D, const double> &&
                       !ContiguousConstIterableOfConstIterables<D, const double>) ) {
            if constexpr (ContiguousIterableOfIterables<D, const double>) {
                printSummary(name, "Contiguous iterable of iterables");
            } else {
                printSummary(name, "Contiguous iterable of const iterables");
            }

            return &((data.begin())[0]);
        } else {
            if (ContiguousConstIterableOfIterables<D, const double> &&
                !ContiguousConstIterableOfConstIterables<D, const double>) {
                printSummary(name, "Contiguous const iterable of iterables");
            } else {
                printSummary(name, "Contiguous const iterable of const iterables");
            }
            return &((data.cbegin())[0]);
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

        if constexpr (ContiguousIterableOfIterables<D, const double> ||
                      ContiguousConstIterableOfIterables<D, const double>) {
            return &(((*currentElementIterator).begin())[0]);
        } else {
            return &(((*currentElementIterator).cbegin())[0]);
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

        if constexpr (ContiguousIterableOfIterables<D, const double> ||
                      ContiguousConstIterableOfIterables<D, const double>) {
            return &((startElementIterator[index].begin())[0]);
        } else {
            return &((startElementIterator[index].cbegin())[0]);
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

    /*template <typename D, std::enable_if_t<RandomIterator<D, const double*>, bool> = true>
    static inline D createEfficientIterator(const D &iterator, const char *name) {

        printSummary(name, "Using direct pointers");
        return iterator;
    }

    template <typename I, std::enable_if_t<RandomIterator<I, const double>, bool> = true>
    static inline const double *getCurrentElement(const I &currentElementIterator) {

        return *currentElementIterator;
    }

    template <typename I, std::enable_if_t<RandomIterator<I, const double>, bool> = true>
    static inline const double *getElementAt(
            const I &startElementIterator, const std::size_t index, std::size_t stride) {

        return &(startElementIterator[index * stride]);
    }*/

private:
    static void printSummary(const char *name, const char *summary) {

#ifdef ITERATORS_SUMMARY
        std::cout << name << ": Using" << ' ' << summary << std::endl;
#endif
#ifdef ITERATORS_SUMMARY_TEST
        if (strcmp("Current data", name) == 0) {
            std::size_t summaryLength = strlen(summary);

            std::cout << summary << "\033[" << summaryLength << "D";
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

}  // namespace cluster::utils

#endif  // FINAL_PROJECT_HPC_DATAITERATORUTILS_H
