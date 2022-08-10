#ifndef FINAL_PROJECT_HPC_LINEARCOLLECTIONCONTAINER_H
#define FINAL_PROJECT_HPC_LINEARCOLLECTIONCONTAINER_H

#include "AlignedArray.h"
#include <array>
#include <cstddef>
#include <deque>
#include <list>
#include <string>
#include <tuple>
#include <vector>

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-09
 * @since version date
 */
template <std::size_t ND>
class LinearCollectionContainer {

private:
    static const constexpr std::size_t SSE_ALIGNMENT = 16;
    static const constexpr std::size_t AVX_ALIGNMENT = 32;

public:
    LinearCollectionContainer() :
        cArray{std::pair{"", new double[ND]}},
        array{},
        sseAlignedArray{},
        avxAlignedArray{} {
    }

    std::pair<std::string, double *> cArray;

    std::pair<std::string, std::array<double, ND>> array;
    std::pair<std::string, typename std::array<double, ND>::iterator> arrayIterator;
    std::pair<std::string, typename std::array<double, ND>::const_iterator> arrayConstIterator;

    std::pair<std::string, std::vector<double>> vector;
    std::pair<std::string, std::vector<double>::iterator> vectorIterator;
    std::pair<std::string, std::vector<double>::const_iterator> vectorConstIterator;

    std::pair<std::string, std::list<double>> list;
    std::pair<std::string, std::list<double>::iterator> listIterator;
    std::pair<std::string, std::list<double>::const_iterator> listConstIterator;

    std::pair<std::string, std::deque<double>> deque;
    std::pair<std::string, std::deque<double>::iterator> dequeIterator;
    std::pair<std::string, std::deque<double>::const_iterator> dequeConstIterator;

    std::pair<std::string, AlignedArray<double, ND, SSE_ALIGNMENT>> sseAlignedArray;
    std::pair<std::string, AlignedArray<double, ND, AVX_ALIGNMENT>> avxAlignedArray;
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_LINEARCOLLECTIONCONTAINER_H
