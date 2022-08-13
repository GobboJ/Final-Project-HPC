#ifndef FINAL_PROJECT_HPC_ONLYCONSTITERABLESUBCONTAINER_H
#define FINAL_PROJECT_HPC_ONLYCONSTITERABLESUBCONTAINER_H

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
class OnlyConstIterableSubContainer {
public:
    OnlyConstIterableSubContainer() = default;
    OnlyConstIterableSubContainer(const OnlyConstIterableSubContainer &) = delete;
    OnlyConstIterableSubContainer(OnlyConstIterableSubContainer &&) = delete;
    OnlyConstIterableSubContainer &operator= (const OnlyConstIterableSubContainer &) = delete;
    OnlyConstIterableSubContainer &operator= (OnlyConstIterableSubContainer &&) = delete;
    ~OnlyConstIterableSubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element) {

        normal.emplace_back(std::forward<E>(element));
    }

    void initIterators() {

        for (auto &element : normal) {
            constIterators.emplace_back(element.cbegin());
        }
        normalIterator = normal.begin();
        normalConstIterator = normal.cbegin();
        constIteratorsIterator = constIterators.begin();
        constIteratorsConstIterator = constIterators.cbegin();
    }

    C<IC> normal;
    typename C<IC>::iterator normalIterator;
    typename C<IC>::const_iterator normalConstIterator;
    C<typename IC::const_iterator> constIterators;
    typename C<typename IC::const_iterator>::iterator constIteratorsIterator;
    typename C<typename IC::const_iterator>::const_iterator constIteratorsConstIterator;
};

}  // namespace cluster::test::types::collections

#endif  // FINAL_PROJECT_HPC_ONLYCONSTITERABLESUBCONTAINER_H
