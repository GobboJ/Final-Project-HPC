#ifndef FINAL_PROJECT_HPC_POINTERWRAPPER_H
#define FINAL_PROJECT_HPC_POINTERWRAPPER_H

#include <vector>

namespace cluster::test::types {
/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-08
 * @since version date
 */
template <typename T, std::size_t N>
class PointerWrapper {

public:
    using const_iterator = const T*;

    explicit PointerWrapper() : data{new T[N]} {
    }

    ~PointerWrapper() {
        delete[] data;
    }

    T* getPointer() {

        return data;
    }

    T& operator[] (std::size_t index) {

        return data[index];
    }

    const_iterator cbegin() {

        return data;
    }

private:
    T* data;
};
}  // namespace cluster::test::types
#endif  // FINAL_PROJECT_HPC_POINTERWRAPPER_H
