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
    template <typename D, std::enable_if_t<ContiguousIterator<D, double>, bool> = true>
    static inline const double *createEfficientIterator(const D &data, const char *name) {

        printSummary(name, "Using direct pointers");
        return &(data[0]);
    }

    template <typename D,
              std::enable_if_t<ContiguousIterable<D, double> && !ContiguousConstIterable<D, double>,
                               bool> = true>
    static inline const double *createEfficientIterator(const D &data, const char *name) {

        printSummary(name, "Using direct pointers from normal iterator");
        return &(data.begin()[0]);
    }

    template <typename D, std::enable_if_t<ContiguousConstIterable<D, double>, bool> = true>
    static inline const double *createEfficientIterator(const D &data, const char *name) {

        printSummary(name, "Using direct pointers from const iterator");
        return &(data.cbegin()[0]);
    }

    template <typename I,
              std::enable_if_t<ContiguousIterator<I, double> || ContiguousIterable<I, double> ||
                                       ContiguousConstIterable<I, double>,
                               bool> = true>
    static inline const double *getCurrentElement(const double *const currentElement) {

        return currentElement;
    }

    template <typename I,
              std::enable_if_t<ContiguousIterator<I, double> || ContiguousIterable<I, double> ||
                                       ContiguousConstIterable<I, double>,
                               bool> = true>
    static inline const double *getElementAt(
            const double *const startElement, const std::size_t index, std::size_t stride) {

        return &(startElement[index * stride]);
    }

    template <typename I,
              std::enable_if_t<ContiguousIterator<I, double> || ContiguousIterable<I, double> ||
                                       ContiguousConstIterable<I, double>,
                               bool> = true>
    static inline void moveNext(const double *&currentElement, std::size_t stride) {

        currentElement += stride;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

private:
    static inline void printSummary(const char *name, const char *summary) {

#ifdef ITERATORS_SUMMARY
        std::cout << name << ": Using" << ' ' << summary << std::endl;
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
