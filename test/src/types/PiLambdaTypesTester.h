#ifndef FINAL_PROJECT_HPC_PILAMBDATYPESTESTER_H
#define FINAL_PROJECT_HPC_PILAMBDATYPESTESTER_H

#include <cstddef>
#include <tuple>
#include <string>
#include <iostream>
#include <iomanip>
#include "TypesPrinter.h"
#include "PiLambdaIteratorUtils.h"
#include "ParallelClustering.h"
#include "DistanceComputers.h"
#include "../ResultsChecker.h"

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-10
 * @since version date
 */
template <typename D>
class PiLambdaTypesTester {

public:
    enum class IteratorType { NONE, NORMAL_ITERATOR, CONST_ITERATOR };

    PiLambdaTypesTester(const D &data,
                        size_t dataElementsCount,
                        size_t dimension,
                        const int maxPiNameLength,
                        const int maxLambdaNameLength,
                        const std::vector<std::size_t> &expectedPi,
                        const std::vector<double> &expectedLambda) :
        data(data),
        dataElementsCount(dataElementsCount),
        dimension(dimension),
        maxPiNameLength(maxPiNameLength),
        maxLambdaNameLength(maxLambdaNameLength),
        expectedPi(expectedPi),
        expectedLambda(expectedLambda) {
    }

    template <typename... Ps, typename... NPs, typename... Ls, typename... NLs>
    void testAllPermutations(std::tuple<Ps...> &compilablePis,
                             std::tuple<NPs...> &notCompilablePis,
                             std::tuple<Ls...> &compilableLambdas,
                             std::tuple<NLs...> &notCompilableLambdas) {

        auto concatenatedPi = std::tuple_cat(compilablePis, notCompilablePis);
        auto concatenatedLambda = std::tuple_cat(compilableLambdas, notCompilableLambdas);

        if constexpr (sizeof...(Ps) + sizeof...(NPs) > 0 && sizeof...(Ls) + sizeof...(NLs) > 0) {
            testPermutations<0, sizeof...(Ps), sizeof...(Ls)>(concatenatedPi, concatenatedLambda);
        }
    }

private:
    template <std::size_t PI, std::size_t CP, std::size_t CL, typename... Ps, typename... Ls>
    void testPermutations(std::tuple<Ps...> pis, std::tuple<Ls...> &lambdas) {

        auto pi = std::get<PI>(pis);

        testLambdaPermutations < PI<CP, 0, CL>(pi, lambdas);

        if constexpr (PI + 1 < sizeof...(Ps)) {
            testPermutations<PI + 1, CP, CL>(pis, lambdas);
        }
    }

    template <bool PC, std::size_t LI, std::size_t CL, typename P, typename... Ls>
    void testLambdaPermutations(P &pi, std::tuple<Ls...> &lambdas) {

        std::cout << std::setfill(' ') << std::setw(maxPiNameLength)
                  << TypesPrinter::getTypeName<P>();

        std::cout << " | ";
        std::cout << std::setfill(' ') << std::setw(maxLambdaNameLength)
                  << TypesPrinter::getTypeName<
                             typename std::tuple_element<LI, std::tuple<Ls...>>::type>();
        std::cout << " | ";
        std::cout.flush();

        constexpr bool piCompiles = cluster::utils::PiIterator<P>;

        constexpr bool lambdaCompiles = cluster::utils::LambdaIterator<
                typename std::tuple_element<LI, std::tuple<Ls...>>::type>;

        if constexpr (LI < CL) {
            auto &lambda = std::get<LI>(lambdas);

            static_assert(cluster::utils::LambdaIterator<decltype(lambda)>, "NO");
            if constexpr (!lambdaCompiles) {
                std::cout << "\033[31mError (lambda should compile)\033[0m";
            } else if constexpr (PC && !piCompiles) {
                std::cout << "\033[31mError (pi should compile)\033[0m";
            } else if constexpr (PC) {
                // Perform the test
                parallel::ParallelClustering<true, true, true>::cluster<
                        parallel::DistanceComputers::CLASSICAL>(
                        this->data, this->dataElementsCount, this->dimension, pi, lambda, 6, 6, 6);
                bool result;

                if constexpr (cluster::utils::ConstIterable<P, std::size_t>) {
                    if constexpr (cluster::utils::ConstIterable<decltype(lambda), double>) {
                        result = ResultsChecker::checkResults(pi.cbegin(),
                                                              pi.cend(),
                                                              lambda.cbegin(),
                                                              lambda.cend(),
                                                              this->expectedPi.cbegin(),
                                                              this->expectedPi.cend(),
                                                              this->expectedLambda.cbegin(),
                                                              this->expectedLambda.cend());
                    } else {
                        result = ResultsChecker::checkResults(pi.cbegin(),
                                                              pi.cend(),
                                                              lambda,
                                                              lambda + this->dataElementsCount,
                                                              this->expectedPi.cbegin(),
                                                              this->expectedPi.cend(),
                                                              this->expectedLambda.cbegin(),
                                                              this->expectedLambda.cend());
                    }
                } else {
                    if constexpr (cluster::utils::ConstIterable<decltype(lambda), double>) {
                        result = ResultsChecker::checkResults(pi,
                                                              pi + this->dataElementsCount,
                                                              lambda.cbegin(),
                                                              lambda.cend(),
                                                              this->expectedPi.cbegin(),
                                                              this->expectedPi.cend(),
                                                              this->expectedLambda.cbegin(),
                                                              this->expectedLambda.cend());
                    } else {
                        result = ResultsChecker::checkResults(pi,
                                                              pi + this->dataElementsCount,
                                                              lambda,
                                                              lambda + this->dataElementsCount,
                                                              this->expectedPi.cbegin(),
                                                              this->expectedPi.cend(),
                                                              this->expectedLambda.cbegin(),
                                                              this->expectedLambda.cend());
                    }
                }

                std::cout << ((result) ? "\033[32mOK" : "\033[31mError") << "\033[0m";
            } else {
                std::cout << "\033[34mOK\033[0m (pi)";
            }
        } else {
            if constexpr (lambdaCompiles) {
                std::cout << "\033[31mError (lambda should not compile)\033[0m";
            } else if constexpr (!PC && piCompiles) {
                std::cout << "\033[31mError (pi should not compile)\033[0m";
            } else {
                std::cout << "\033[34mOK\033[0m (" << ((!piCompiles) ? "pi and " : "") << "lambda)";
            }
        }
        std::cout << std::endl;
        if constexpr (LI + 1 < sizeof...(Ls)) {
            testLambdaPermutations<PC, LI + 1, CL>(pi, lambdas);
        }
    }

    const D &data;
    std::size_t dataElementsCount;
    const std::size_t dimension;
    const int maxPiNameLength;
    const int maxLambdaNameLength;
    const std::vector<std::size_t> &expectedPi;
    const std::vector<double> &expectedLambda;
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_PILAMBDATYPESTESTER_H
