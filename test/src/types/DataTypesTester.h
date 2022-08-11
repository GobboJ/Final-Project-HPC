#ifndef FINAL_PROJECT_HPC_DATATYPESTESTER_H
#define FINAL_PROJECT_HPC_DATATYPESTESTER_H

#include "TypesPrinter.h"
#include "ParallelClustering.h"
#include "../ResultsChecker.h"
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-10
 * @since version date
 */
class DataTypesTester {

public:
    DataTypesTester(const size_t dataElementsCount,
                    const size_t dimension,
                    const std::vector<std::size_t> &expectedPi,
                    const std::vector<double> &expectedLambda,
                    const int maxTypeNameLength) :
        dataElementsCount(dataElementsCount),
        dimension(dimension),
        expectedPi(expectedPi),
        expectedLambda(expectedLambda),
        maxTypeNameLength(maxTypeNameLength) {
    }

    template <typename... CDs, typename... NCDs>
    void testParallelTypes(std::tuple<CDs...> &compilableDataStructures,
                           std::tuple<NCDs...> &notCompilableDataStructures) {

        std::vector<std::size_t> pi{};
        pi.resize(this->dataElementsCount);
        std::vector<double> lambda{};
        lambda.resize(this->dataElementsCount);

        if constexpr (sizeof...(CDs) > 0) {
            testCompilableTypes<false, 0>(compilableDataStructures, pi, lambda);
        }
        if constexpr (sizeof...(NCDs) > 0) {
            testNotCompilableTypes<0>(notCompilableDataStructures);
        }
    }

private:
    template <bool S, std::size_t DI, typename... CDs>
    void testCompilableTypes(std::tuple<CDs...> &compilableDataStructures,
                             std::vector<std::size_t> &pi,
                             std::vector<double> &lambda) {

        const auto &data = std::get<DI>(compilableDataStructures);

        std::cout << std::setfill(' ') << std::setw(this->maxTypeNameLength)
                  << TypesPrinter::getTypeName<decltype(data)>();
        std::cout << " | ";
        std::cout.flush();

        
        if constexpr (!utils::ParallelDataIterator<decltype(data)>) {
            std::cout << "\033[31mError (should compile)\033[0m";
        } else {
            parallel::ParallelClustering<true, true, true>::cluster<
                    parallel::DistanceComputers::CLASSICAL>(
                    data, this->dataElementsCount, this->dimension, pi, lambda, 6, 6, 6);

            bool result = ResultsChecker::checkResults(pi.cbegin(),
                                                       pi.cend(),
                                                       lambda.cbegin(),
                                                       lambda.cend(),
                                                       this->expectedPi.cbegin(),
                                                       this->expectedPi.cend(),
                                                       this->expectedLambda.cbegin(),
                                                       this->expectedLambda.cend());
            std::cout << ((result) ? "\033[32mOK" : "\033[31mError") << "\033[0m";
        }
        std::cout << std::endl;
        if constexpr (DI + 1 < sizeof...(CDs)) {
            testCompilableTypes<S, DI + 1>(compilableDataStructures, pi, lambda);
        }
    }

    template <std::size_t NDI, typename... NCDs>
    void testNotCompilableTypes(std::tuple<NCDs...> &notCompilableDataStructures) {

        const auto &data = std::get<NDI>(notCompilableDataStructures);

        std::cout << std::setfill(' ') << std::setw(this->maxTypeNameLength)
                  << TypesPrinter::getTypeName<decltype(data)>();
        std::cout << " | ";
        std::cout.flush();

        if constexpr (utils::ParallelDataIterator<decltype(data)>) {
            std::cout << "\033[31mError (should not compile)\033[0m";
        } else {
            std::cout << "\033[34mOK\033[0m";
        }
        std::cout << std::endl;
        if constexpr (NDI + 1 < sizeof...(NCDs)) {
            testNotCompilableTypes<NDI + 1>(notCompilableDataStructures);
        }
    }

    const std::size_t dataElementsCount;
    const std::size_t dimension;
    const std::vector<std::size_t> &expectedPi;
    const std::vector<double> &expectedLambda;
    const int maxTypeNameLength;
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_DATATYPESTESTER_H
