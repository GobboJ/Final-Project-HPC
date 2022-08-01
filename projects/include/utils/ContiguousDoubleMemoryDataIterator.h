/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-01
 * @since version date
 */
#ifndef FINAL_PROJECT_HPC_CONTIGUOUSDOUBLEMEMORYDATAITERATOR_H
#define FINAL_PROJECT_HPC_CONTIGUOUSDOUBLEMEMORYDATAITERATOR_H

#include <array>
#include <cstddef>

class ContiguousDoubleMemoryDataIterator {

public:
    using value_type = const double *const;
    using reference_type = const value_type &;
    using pointer = value_type *const;
    using difference_type = std::ptrdiff_t;

    ContiguousDoubleMemoryDataIterator();

    ContiguousDoubleMemoryDataIterator(const double *startElement, size_t stride);

    template <std::size_t N>
    ContiguousDoubleMemoryDataIterator(const std::array<double, N> array, size_t stride) :
        ContiguousDoubleMemoryDataIterator(array.data(), stride) {
    }

    ContiguousDoubleMemoryDataIterator(const ContiguousDoubleMemoryDataIterator &other) = default;

    ContiguousDoubleMemoryDataIterator(ContiguousDoubleMemoryDataIterator &&other) noexcept :
        startElement(other.startElement),
        currentElement(other.currentElement),
        stride(other.stride) {
    }

    ContiguousDoubleMemoryDataIterator &operator= (const ContiguousDoubleMemoryDataIterator &other);

    ContiguousDoubleMemoryDataIterator &operator= (
            ContiguousDoubleMemoryDataIterator &&other) noexcept;

    reference_type operator* () const;

    pointer operator->() const;

    const double *operator[] (difference_type index) const;

    bool operator== (const ContiguousDoubleMemoryDataIterator &other) const;

    bool operator!= (const ContiguousDoubleMemoryDataIterator &other) const;

    ContiguousDoubleMemoryDataIterator &operator++ ();

    ContiguousDoubleMemoryDataIterator operator++ (int);

    static std::size_t computeSseStride(std::size_t dimension);
    
    static std::size_t computeAvxStride(std::size_t dimension);

private:
    const double *startElement;
    const double *currentElement;
    std::size_t stride;
};

#endif  // FINAL_PROJECT_HPC_CONTIGUOUSDOUBLEMEMORYDATAITERATOR_H
