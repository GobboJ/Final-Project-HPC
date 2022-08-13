#ifndef FINAL_PROJECT_HPC_ONLYITERABLESUBCONTAINER_H
#define FINAL_PROJECT_HPC_ONLYITERABLESUBCONTAINER_H

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
class OnlyIterableSubContainer {

public:
    OnlyIterableSubContainer() = default;
    OnlyIterableSubContainer(const OnlyIterableSubContainer &) = delete;
    OnlyIterableSubContainer(OnlyIterableSubContainer &&) = delete;
    OnlyIterableSubContainer &operator= (const OnlyIterableSubContainer &) = delete;
    OnlyIterableSubContainer &operator= (OnlyIterableSubContainer &&) = delete;
    ~OnlyIterableSubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element) {

        normal.emplace_back(std::forward<E>(element));
    }

    void initIterators() {

        for (auto &element : normal) {
            iterators.emplace_back(element.begin());
        }
        normalIterator = normal.begin();
        normalConstIterator = normal.cbegin();
        iteratorsIterator = iterators.begin();
        iteratorsConstIterator = iterators.cbegin();
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    C<IC> normal;
    typename C<IC>::iterator normalIterator;
    typename C<IC>::const_iterator normalConstIterator;
    C<typename IC::iterator> iterators;
    typename C<typename IC::iterator>::iterator iteratorsIterator;
    typename C<typename IC::iterator>::const_iterator iteratorsConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::collections

#endif  // FINAL_PROJECT_HPC_ONLYITERABLESUBCONTAINER_H
