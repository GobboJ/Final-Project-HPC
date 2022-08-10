#ifndef FINAL_PROJECT_HPC_ALIGNEDARRAY_H
#define FINAL_PROJECT_HPC_ALIGNEDARRAY_H

#include <cstddef>
#include <memory>
#include <cstring>
#include <cassert>

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-08
 * @since version date
 */
template <typename T, std::size_t N, std::size_t A>
class AlignedArray {

public:
    using value_type = T;
    
private:
    template <std::size_t M>
    static constexpr bool powerOfTwo() {

        if constexpr (M == 1) {
            return true;
        }

        return M % 2 == 0 && powerOfTwo<M / 2>();
    }

public:
    AlignedArray() : fullData{new T[N + A]}, data{nullptr} {

        static_assert(powerOfTwo<A>(), "The specified alignment is not a power of 2");

        memset(this->fullData, 0, (N + A) * sizeof(T));
        void *fullDataPointer = this->fullData;

        std::size_t size = (N + A) * sizeof(T);

        this->data = static_cast<T *>(std::align(A, N * sizeof(T), fullDataPointer, size));
        
        assert(size >= N);
    }

    AlignedArray(const AlignedArray<T, N, A> &other) : fullData{new T[N + A]}, data{nullptr} {

        memcpy(this->fullData, other.fullData, (N + A) * sizeof(T));

        void *fullDataPointer = this->fullData;

        std::size_t size = (N + A) * sizeof(T);

        this->data = static_cast<T *>(std::align(A, N * sizeof(T), fullDataPointer, size));
        
        assert(size >= N);
    }

    AlignedArray &operator= (const AlignedArray<T, N, A> &other) {
        if (this != &other) {
            memcpy(this->fullData, other.fullData, (N + A) * sizeof(T));

            void *fullDataPointer = this->fullData;

            std::size_t size = (N + A) * sizeof(T);

            this->data =
                    static_cast<T *>(std::align(A, N * sizeof(T), fullDataPointer, size));
            
            assert(size >= N);
        }

        return *this;
    }

    ~AlignedArray() {
        delete[] fullData;
    }

    T &operator[] (std::size_t index) {

        assert(index < N);
        return data[index];
    }

    T *begin() {
        return data;
    }

    T *end() {
        return data + N;
    }

    T *cbegin() {
        return data;
    }

    T *cend() {
        return data + N;
    }

private:
    T *fullData;
    T *data;
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_ALIGNEDARRAY_H
