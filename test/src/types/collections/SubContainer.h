#ifndef FINAL_PROJECT_HPC_SUBCONTAINER_H
#define FINAL_PROJECT_HPC_SUBCONTAINER_H

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
class SubContainer {

public:
    SubContainer() = default;
    SubContainer(const SubContainer &) = delete;
    SubContainer(SubContainer &&) = delete;
    SubContainer &operator= (const SubContainer &) = delete;
    SubContainer &operator= (SubContainer &&) = delete;
    ~SubContainer() = default;

    template <typename E>
    void emplaceElement(E &&element) {

        normal.emplace_back(std::forward<E>(element));
    }

    void initIterators() {

        for (auto &element : normal) {
            iterators.emplace_back(element.begin());
            constIterators.emplace_back(element.cbegin());
        }
        normalIterator = normal.begin();
        normalConstIterator = normal.cbegin();
        iteratorsIterator = iterators.begin();
        iteratorsConstIterator = iterators.cbegin();
        constIteratorsIterator = constIterators.begin();
        constIteratorsConstIterator = constIterators.cbegin();
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    C<IC> normal;
    typename C<IC>::iterator normalIterator;
    typename C<IC>::const_iterator normalConstIterator;
    C<typename IC::iterator> iterators;
    typename C<typename IC::iterator>::iterator iteratorsIterator;
    typename C<typename IC::iterator>::const_iterator iteratorsConstIterator;
    C<typename IC::const_iterator> constIterators;
    typename C<typename IC::const_iterator>::iterator constIteratorsIterator;
    typename C<typename IC::const_iterator>::const_iterator constIteratorsConstIterator;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

}  // namespace cluster::test::types::collections

#endif  // FINAL_PROJECT_HPC_SUBCONTAINER_H
