#ifndef FINAL_PROJECT_HPC_ARRAYWRAPPER_H
#define FINAL_PROJECT_HPC_ARRAYWRAPPER_H

#include <cstddef>
#include <array>
#include <memory>
#include <vector>

namespace cluster::test::types::collections {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-13
 * @since version date
 */
template <typename T>
class ArrayWrapper {

public:
    using value_type = T;
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    template <class... Args>
    T& emplace_back(Args&&... args) {
        return this->data.template emplace_back(std::forward<Args...>(args...));
    }

    void push_back(const T& value) {
        this->data.push_back(value);
    }

    template <std::size_t N, typename NT = T>
    std::array<NT, N>* toArray() {
        auto *result = new std::array<NT, N>();
        std::copy(this->data.cbegin(), this->data.cend(), result->begin());
        
        return result;
    }
    
    template <typename NT = T>
    NT* toPointer() {
        return data.data();
    }

    iterator begin() {
        return this->data.begin();
    }

    iterator end() {
        return this->data.end();
    }

    const_iterator cbegin() const {
        return this->data.cbegin();
    }

    const_iterator cend() const {
        return this->data.cend();
    }

private:
    std::vector<T> data;
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_ARRAYWRAPPER_H
