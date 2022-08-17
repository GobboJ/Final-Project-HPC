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
#include "../../src/data/ResultsChecker.h"

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

    using PiLambdaIteratorType = cluster::utils::PiLambdaType;
    using PiLambdaIteratorTypeUtils = cluster::utils::PiLambdaIteratorTypeUtils;
    using PiLambdaIteratorUtils = cluster::utils::PiLambdaIteratorUtils;

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

    template <typename... Ps1,
              typename... Ps2,
              typename... Ps3,
              typename... Ps4,
              typename... NPs,
              typename... Ls1,
              typename... Ls2,
              typename... Ls3,
              typename... Ls4,
              typename... NLs>
    void testAllPermutations(std::tuple<Ps1 &...> &contiguousIteratorPis,
                             std::tuple<Ps2 &...> &contiguousIterablePis,
                             std::tuple<Ps3 &...> &randomIteratorPis,
                             std::tuple<Ps4 &...> &randomIterablePis,
                             std::tuple<NPs &...> &notCompilablePis,
                             std::tuple<Ls1 &...> &contiguousIteratorLambdas,
                             std::tuple<Ls2 &...> &contiguousIterableLambdas,
                             std::tuple<Ls3 &...> &randomIteratorLambdas,
                             std::tuple<Ls4 &...> &randomIterableLambdas,
                             std::tuple<NLs &...> &notCompilableLambdas) {

        testAllPermutationsWithOneAlgorithm<false>(contiguousIteratorPis,
                                                   contiguousIterablePis,
                                                   randomIteratorPis,
                                                   randomIterablePis,
                                                   notCompilablePis,
                                                   contiguousIteratorLambdas,
                                                   contiguousIterableLambdas,
                                                   randomIteratorLambdas,
                                                   randomIterableLambdas,
                                                   notCompilableLambdas);
    }

private:
    template <bool S,
              typename... Ps1,
              typename... Ps2,
              typename... Ps3,
              typename... Ps4,
              typename... NPs,
              typename... Ls1,
              typename... Ls2,
              typename... Ls3,
              typename... Ls4,
              typename... NLs>
    void testAllPermutationsWithOneAlgorithm(std::tuple<Ps1 &...> &contiguousIteratorPis,
                                             std::tuple<Ps2 &...> &contiguousIterablePis,
                                             std::tuple<Ps3 &...> &randomIteratorPis,
                                             std::tuple<Ps4 &...> &randomIterablePis,
                                             std::tuple<NPs &...> &notCompilablePis,
                                             std::tuple<Ls1 &...> &contiguousIteratorLambdas,
                                             std::tuple<Ls2 &...> &contiguousIterableLambdas,
                                             std::tuple<Ls3 &...> &randomIteratorLambdas,
                                             std::tuple<Ls4 &...> &randomIterableLambdas,
                                             std::tuple<NLs &...> &notCompilableLambdas) {

        testAllLambdaPermutations<S, true>(contiguousIteratorPis,
                                           PiLambdaIteratorType::CONTIGUOUS_ITERATOR,
                                           contiguousIteratorLambdas,
                                           contiguousIterableLambdas,
                                           randomIteratorLambdas,
                                           randomIterableLambdas,
                                           notCompilableLambdas);
        testAllLambdaPermutations<S, true>(contiguousIterablePis,
                                           PiLambdaIteratorType::CONTIGUOUS_ITERABLE,
                                           contiguousIteratorLambdas,
                                           contiguousIterableLambdas,
                                           randomIteratorLambdas,
                                           randomIterableLambdas,
                                           notCompilableLambdas);
        testAllLambdaPermutations<S, true>(randomIteratorPis,
                                           PiLambdaIteratorType::RANDOM_ITERATOR,
                                           contiguousIteratorLambdas,
                                           contiguousIterableLambdas,
                                           randomIteratorLambdas,
                                           randomIterableLambdas,
                                           notCompilableLambdas);
        testAllLambdaPermutations<S, true>(randomIterablePis,
                                           PiLambdaIteratorType::RANDOM_ITERABLE,
                                           contiguousIteratorLambdas,
                                           contiguousIterableLambdas,
                                           randomIteratorLambdas,
                                           randomIterableLambdas,
                                           notCompilableLambdas);
        testAllLambdaPermutations<S, false>(notCompilablePis,
                                            PiLambdaIteratorType::NONE,
                                            contiguousIteratorLambdas,
                                            contiguousIterableLambdas,
                                            randomIteratorLambdas,
                                            randomIterableLambdas,
                                            notCompilableLambdas);
    }

    template <bool S,
              bool PC,
              typename... Ps,
              typename... Ls1,
              typename... Ls2,
              typename... Ls3,
              typename... Ls4,
              typename... NLs>
    void testAllLambdaPermutations(std::tuple<Ps &...> &pis,
                                   PiLambdaIteratorType piIteratorType,
                                   std::tuple<Ls1 &...> &contiguousIteratorLambdas,
                                   std::tuple<Ls2 &...> &contiguousIterableLambdas,
                                   std::tuple<Ls3 &...> &randomIteratorLambdas,
                                   std::tuple<Ls4 &...> &randomIterableLambdas,
                                   std::tuple<NLs &...> &notCompilableLambdas) {

        testPermutations<S, PC, true, 0>(pis,
                                         piIteratorType,
                                         contiguousIteratorLambdas,
                                         PiLambdaIteratorType::CONTIGUOUS_ITERATOR);
        testPermutations<S, PC, true, 0>(pis,
                                         piIteratorType,
                                         contiguousIterableLambdas,
                                         PiLambdaIteratorType::CONTIGUOUS_ITERABLE);
        testPermutations<S, PC, true, 0>(
                pis, piIteratorType, randomIteratorLambdas, PiLambdaIteratorType::RANDOM_ITERATOR);
        testPermutations<S, PC, true, 0>(
                pis, piIteratorType, randomIterableLambdas, PiLambdaIteratorType::RANDOM_ITERABLE);
        testPermutations<S, PC, false, 0>(
                pis, piIteratorType, notCompilableLambdas, PiLambdaIteratorType::NONE);
    }

    template <bool S, bool PC, bool LC, std::size_t PI, typename... Ps, typename... Ls>
    void testPermutations(std::tuple<Ps &...> &pis,
                          PiLambdaIteratorType piIteratorType,
                          std::tuple<Ls &...> &lambdas,
                          PiLambdaIteratorType lambdaIteratorsType) {

        if constexpr (sizeof...(Ps) > 0 && sizeof...(Ls) > 0) {
            auto &pi = std::get<PI>(pis);
            auto *piCopy = new std::remove_reference_t<decltype(pi)>(pi);

            testLambdaPermutations<S, PC, LC, 0>(
                    piCopy, piIteratorType, lambdas, lambdaIteratorsType);
            delete piCopy;
            if constexpr (PI + 1 < sizeof...(Ps)) {
                testPermutations<S, PC, LC, PI + 1>(
                        pis, piIteratorType, lambdas, lambdaIteratorsType);
            }
        }
    }

    template <bool S, bool PC, bool LC, std::size_t LI, typename P, typename... Ls>
    void testLambdaPermutations(P *pi,
                                PiLambdaIteratorType piIteratorType,
                                std::tuple<Ls &...> &lambdas,
                                PiLambdaIteratorType lambdaIteratorsType) {

        if constexpr (!S) {
            std::cout << "Par  | ";
        } else {
            // TODO: Seq
        }
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

        if constexpr (LC) {
            // Make a copy
            auto &lambdaReference = std::get<LI>(lambdas);
            auto *lambda = new std::remove_reference_t<decltype(lambdaReference)>(lambdaReference);

            if constexpr (!lambdaCompiles) {
                std::cout << "\033[31mError (lambda should compile)\033[0m";
            } else if constexpr (PC && !piCompiles) {
                std::cout << "\033[31mError (pi should compile)\033[0m";
            } else if constexpr (PC) {
                // Perform the test
                if constexpr (!S) {
                    parallel::ParallelClustering<true, true, true>::cluster<
                            parallel::DistanceComputers::CLASSICAL>(this->data,
                                                                    this->dataElementsCount,
                                                                    this->dimension,
                                                                    *pi,
                                                                    *lambda,
                                                                    6,
                                                                    6,
                                                                    6);
                } else {
                    // TODO: Test sequential
                }
                bool result = ResultsChecker::checkResults(getBegin<std::size_t>(*pi),
                                                           getEnd<std::size_t>(*pi),
                                                           getBegin<double>(*lambda),
                                                           getEnd<double>(*lambda),
                                                           this->expectedPi.cbegin(),
                                                           this->expectedPi.cend(),
                                                           this->expectedLambda.cbegin(),
                                                           this->expectedLambda.cend());

                std::cout << ((result) ? "\033[32mOK" : "\033[31mError") << "\033[0m";
                bool correctPiIteratorType =
                        PiLambdaIteratorUtils::lastPiType == piIteratorType;
                bool correctLambdaIteratorType =
                        PiLambdaIteratorUtils::lastLambdaType == lambdaIteratorsType;

                if (!correctPiIteratorType || !correctLambdaIteratorType) {
                    std::cout << " \033[31m(\033[0m";
                } else {
                    std::cout << " \033[32m(\033[0m";
                }

                if (correctPiIteratorType) {
                    std::cout << "\033[32mOK\033[0m";
                } else {
                    std::cout << "\033[31m"
                              << PiLambdaIteratorTypeUtils::getDescription(piIteratorType)
                              << "\033[0m";
                }
                std::cout << ", ";
                if (correctLambdaIteratorType) {
                    std::cout << " \033[32mOK\033[0m";
                } else {
                    std::cout << " \033[31m"
                              << PiLambdaIteratorTypeUtils::getDescription(lambdaIteratorsType)
                              << "\033[0m";
                }
                if (!correctPiIteratorType || !correctLambdaIteratorType) {
                    std::cout << "\033[31m)\033[0m";
                } else {
                    std::cout << "\033[32m)\033[0m";
                }
            } else {
                for (std::size_t i = 0; i < 19; i++) {
                    std::cout << ' ';
                }
                std::cout << " | ";
                for (std::size_t i = 0; i < 19; i++) {
                    std::cout << ' ';
                }
                std::cout << " | \033[34mOK\033[0m (pi)";
            }
            delete lambda;
        } else {
            for (std::size_t i = 0; i < 19; i++) {
                std::cout << ' ';
            }
            std::cout << " | ";
            for (std::size_t i = 0; i < 19; i++) {
                std::cout << ' ';
            }
            std::cout << " | ";
            if constexpr (lambdaCompiles) {
                std::cout << "\033[31mError (lambda should not compile)\033[0m";
            } else if constexpr (!PC && piCompiles) {
                std::cout << "\033[31mError (pi should not compile)\033[0m";
            } else {
                std::cout << "\033[34mOK\033[0m (" << ((!piCompiles) ? "pi and " : "") << "lambda)";
            }
        }
        std::cout << "\033[K" << std::endl;

        if constexpr (LI + 1 < sizeof...(Ls)) {
            auto *const piCopy = new std::remove_reference_t<decltype(*pi)>(*pi);
            testLambdaPermutations<S, PC, LC, LI + 1>(
                    piCopy, piIteratorType, lambdas, lambdaIteratorsType);
            delete piCopy;
        }
    }

    template <typename T, typename I>
    auto getBegin(I &dataStructure) {

        if constexpr (utils::ConstIterable<I, T>) {
            return dataStructure.cbegin();
        } else if constexpr (utils::Iterable<I, T>) {
            return dataStructure.begin();
        } else {
            return dataStructure;
        }
    }

    template <typename T, typename I>
    auto getEnd(I &dataStructure) {

        if constexpr (utils::ConstIterable<I, T>) {
            return dataStructure.cend();
        } else if constexpr (utils::Iterable<I, T>) {
            return dataStructure.end();
        } else {
            return dataStructure + this->dataElementsCount;
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
