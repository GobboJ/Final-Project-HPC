#ifndef FINAL_PROJECT_HPC_ONLYCONSTITERABLEVECTOR_H
#define FINAL_PROJECT_HPC_ONLYCONSTITERABLEVECTOR_H

#include <vector>

namespace cluster::test::types::collections {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-12
 * @since version date
 */
template <typename T>
class OnlyConstIterableVector {

public:
    using value_type = T;
    using iterator = typename std::vector<T>::const_iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    template <class... Args>
    T& emplace_back(Args&&... args) {
        return this->data.template emplace_back(std::forward<Args...>(args...));
    }

    void push_back(const T& value) {
        this->data.push_back(value);
    }

    void resize(std::size_t newSize) {
        this->data.resize(newSize);
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

}  // namespace cluster::test::types::collections

#endif  // FINAL_PROJECT_HPC_ONLYCONSTITERABLEVECTOR_H
