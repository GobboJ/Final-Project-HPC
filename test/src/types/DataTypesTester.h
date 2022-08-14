#ifndef FINAL_PROJECT_HPC_DATATYPESTESTER_H
#define FINAL_PROJECT_HPC_DATATYPESTESTER_H

#include "TypesPrinter.h"
#include "../ResultsChecker.h"
#include "ParallelClustering.h"
#include "containers/CollectionInfo.h"
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
    using DataIteratorType = cluster::utils::DataIteratorType;
    using DataIteratorTypeUtils = cluster::utils::DataIteratorTypeUtils;
    using DataIteratorUtils = cluster::utils::DataIteratorUtils;

public:
    DataTypesTester(const size_t dataElementsCount,
                    const size_t dimension,
                    const std::vector<std::size_t> &expectedPi,
                    const std::vector<double> &expectedLambda,
                    const int maxTypeNameLength,
                    const int maxResultLength) :
        dataElementsCount(dataElementsCount),
        dimension(dimension),
        expectedPi(expectedPi),
        expectedLambda(expectedLambda),
        maxTypeNameLength(maxTypeNameLength),
        maxResultLength(maxResultLength) {
    }

    template <typename... CDs, typename... NCDs>
    void testParallelTypes(
            const std::tuple<containers::CollectionInfo<CDs> &...> &compilableDataStructures,
            const std::tuple<containers::CollectionInfo<NCDs> &...> &notCompilableDataStructures) {

        std::vector<std::size_t> pi{};
        pi.resize(this->dataElementsCount);
        std::vector<double> lambda{};
        lambda.resize(this->dataElementsCount);
        bool testPassed = true;

        if constexpr (sizeof...(CDs) > 0) {
            testPassed &= testCompilableTypes<false, 0>(compilableDataStructures, pi, lambda);
        }
        if constexpr (sizeof...(NCDs) > 0) {
            testPassed &= testNotCompilableTypes<0>(notCompilableDataStructures);
        }
        std::cout << std::endl
                  << "Test ended with" << ' '
                  << ((testPassed) ? "\033[32msuccess" : "\033[31msome errors") << "\033[0m"
                  << std::endl;
    }

private:
    template <bool S, std::size_t DI, typename... CDs>
    bool testCompilableTypes(
            const std::tuple<containers::CollectionInfo<CDs> &...> &compilableDataStructures,
            std::vector<std::size_t> &pi,
            std::vector<double> &lambda) {

        const auto &dataInfo = std::get<DI>(compilableDataStructures);
        const auto &data = dataInfo.field;

        std::cout << std::setfill(' ') << std::setw(this->maxTypeNameLength)
                  << dataInfo.name;  // TypesPrinter::getTypeName<decltype(data)>();
        std::cout << " | ";
        std::cout << std::setfill(' ') << std::setw(this->maxResultLength) << ' ';
        std::cout << " | ";
        std::cout.flush();

        bool result = true;

        if constexpr (!utils::ParallelDataIterator<decltype(data)>) {
            std::cout << "\033[" << this->maxResultLength + 3 << "D"
                      << "\033[31m";
            std::cout << std::setfill(' ') << std::setw(this->maxResultLength)
                      << "Error (should compile)";
            std::cout << "\033[0m";

            result = false;
        } else {
            if constexpr (!S) {
                parallel::ParallelClustering<true, true, true>::cluster<
                        parallel::DistanceComputers::CLASSICAL>(
                        data, this->dataElementsCount, this->dimension, pi, lambda, 6, 6, 6);
            } else {
                // TODO: Sequential
            }
            result = ResultsChecker::checkResults(pi.cbegin(),
                                                  pi.cend(),
                                                  lambda.cbegin(),
                                                  lambda.cend(),
                                                  this->expectedPi.cbegin(),
                                                  this->expectedPi.cend(),
                                                  this->expectedLambda.cbegin(),
                                                  this->expectedLambda.cend());
            std::cout << "\033[" << this->maxResultLength + 3 << "D"
                      << ((result) ? "\033[32mOK" : "\033[31mError") << "\033[0m";

            if (DataIteratorUtils::lastIteratorType != dataInfo.iteratorType ||
                DataIteratorUtils::lastFirstLevelIteratorType != dataInfo.firstLevelIteratorType ||
                DataIteratorUtils::lastSecondLevelIteratorType !=
                        dataInfo.secondLevelIteratorType) {
                std::cout << " \033[31m("
                          << DataIteratorTypeUtils::getDescription(dataInfo.iteratorType,
                                                                   dataInfo.firstLevelIteratorType,
                                                                   dataInfo.secondLevelIteratorType)
                          << ")\033[0m";
                result = false;
            } else {
                std::cout << " \033[32m(OK)\033[0m";
            }
        }
        std::cout << std::endl;
        if constexpr (DI + 1 < sizeof...(CDs)) {
            result &= testCompilableTypes<S, DI + 1>(compilableDataStructures, pi, lambda);
        }
        return result;
    }

    template <std::size_t NDI, typename... NCDs>
    bool testNotCompilableTypes(
            const std::tuple<containers::CollectionInfo<NCDs> &...> &notCompilableDataStructures) {

        const auto &dataPair = std::get<NDI>(notCompilableDataStructures);
        const auto &data = dataPair.field;

        std::cout << std::setfill(' ') << std::setw(this->maxTypeNameLength)
                  << dataPair.name;  // TypesPrinter::getTypeName<decltype(data)>();
        std::cout << " | ";

        bool result = true;

        if constexpr (utils::ParallelDataIterator<decltype(data)>) {
            std::cout << "\033[31m";
            std::cout << std::setfill(' ') << std::setw(this->maxResultLength)
                      << "Error (should not compile)";
            std::cout << "\033[0m |";
            result = false;
        } else {
            std::cout << "\033[34m";
            std::cout << std::setfill(' ') << std::setw(this->maxResultLength) << "OK";
            std::cout << "\033[0m |";
        }
        std::cout << std::endl;
        if constexpr (NDI + 1 < sizeof...(NCDs)) {
            result &= testNotCompilableTypes<NDI + 1>(notCompilableDataStructures);
        }
        return result;
    }

    const std::size_t dataElementsCount;
    const std::size_t dimension;
    const std::vector<std::size_t> &expectedPi;
    const std::vector<double> &expectedLambda;
    const int maxTypeNameLength;
    const int maxResultLength;
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_DATATYPESTESTER_H
