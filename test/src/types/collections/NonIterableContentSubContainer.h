#ifndef FINAL_PROJECT_HPC_NONITERABLECONTENTSUBCONTAINER_H
#define FINAL_PROJECT_HPC_NONITERABLECONTENTSUBCONTAINER_H

#include <utility>

namespace cluster::test::types::collections {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-13
 * @since version date
 */
template <template <typename> typename C, typename IC>
class NonIterableContentSubContainer {
public:
    NonIterableContentSubContainer() = default;
    NonIterableContentSubContainer(const NonIterableContentSubContainer &) = delete;
    NonIterableContentSubContainer(NonIterableContentSubContainer &&) = delete;
    NonIterableContentSubContainer &operator= (const NonIterableContentSubContainer &) = delete;
    NonIterableContentSubContainer &operator= (NonIterableContentSubContainer &&) = delete;
    ~NonIterableContentSubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element) {

        normal.emplace_back(std::forward<E>(element));
    }

    void initIterators() {

        normalIterator = normal.begin();
        normalConstIterator = normal.cbegin();
    }

    C<IC> normal;
    typename C<IC>::iterator normalIterator;
    typename C<IC>::const_iterator normalConstIterator;
};

}  // namespace cluster::test::types::collections

#endif  // FINAL_PROJECT_HPC_NONITERABLECONTENTSUBCONTAINER_H
