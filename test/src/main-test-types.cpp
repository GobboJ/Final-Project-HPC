/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-07
 * @since version date
 */
#include "ContiguousDoubleMemoryDataIterator.h"
#include "ParallelClustering.h"
#include "ResultsChecker.h"
#include "SequentialClustering.h"
#include "data/DataReader.h"
#include "data/DataWriter.h"
#include "types/ArrayCollectionContainer.h"
#include "types/CollectionContainer.h"
#include "types/CollectionCreator.h"
#include "types/TypesPrinter.h"
#include "types/PiLambdaTypesTester.h"
#include <deque>
#include <list>

using cluster::parallel::DistanceComputers;
using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;
using cluster::test::ResultsChecker;
using cluster::test::data::DataReader;
using cluster::test::data::DataWriter;
using cluster::test::types::AlignedArray;
using cluster::test::types::ArrayCollectionContainer;
using cluster::test::types::CollectionContainer;
using cluster::test::types::CollectionCreator;
using cluster::test::types::LinearCollectionContainer;
using cluster::test::types::PiLambdaTypesTester;
using cluster::test::types::TypesPrinter;
using cluster::utils::ConstIterable;
using cluster::utils::ContiguousConstIterable;
using cluster::utils::ContiguousIterable;
using cluster::utils::ContiguousIterator;
using cluster::utils::InputIterator;
using cluster::utils::Iterable;
using cluster::utils::OutputIterator;
using cluster::utils::ParallelDataIterator;
using cluster::utils::RandomIterable;
using cluster::utils::RandomIterator;
using IteratorType = PiLambdaTypesTester<std::vector<double>>::IteratorType;

const std::size_t ELEMENTS = 35717;
const std::size_t COORDINATES = 2;

template <typename D>
concept NotParallelDataIterator = !
ParallelDataIterator<D>;

class Tester {
public:
    Tester(const std::vector<std::size_t> &expectedPi,
           const std::vector<double> &expectedLambda,
           const int maxTypeNameLength) :
        expectedPi(expectedPi),
        expectedLambda(expectedLambda),
        maxTypeNameLength(maxTypeNameLength) {
    }

    template <ParallelDataIterator D>
    void performParallelTest(std::pair<std::string, D> data) {

        std::vector<std::size_t> pi{};
        pi.resize(ELEMENTS);
        std::vector<double> lambda{};
        lambda.resize(ELEMENTS);

        std::vector<std::size_t>::iterator piIterator = pi.begin();
        std::vector<double>::iterator lambdaIterator = lambda.begin();

        std::cout << std::setfill(' ') << std::setw(this->maxTypeNameLength) << data.first;
        std::cout << " | ";
        std::cout.flush();

        ParallelClustering<true, true, true>::cluster<DistanceComputers::CLASSICAL>(
                data.second, ELEMENTS, COORDINATES, piIterator, lambdaIterator, 6, 6, 6);

        bool result = ResultsChecker::checkResults(pi.cbegin(),
                                                   pi.cend(),
                                                   lambda.cbegin(),
                                                   lambda.cend(),
                                                   this->expectedPi.cbegin(),
                                                   this->expectedPi.cend(),
                                                   this->expectedLambda.cbegin(),
                                                   this->expectedLambda.cend());
        std::cout << ((result) ? "\033[32mOK" : "\033[31mError") << "\033[0m" << std::endl;
    }

    template <typename D>
    void performNotCompilableParallelTest(std::pair<std::string, D> &data) {

        staticCheck(data.first, data.second, this->maxTypeNameLength);
    }

    template <typename D, typename... Ps, typename... NPs, typename... Ls, typename... NLs>
    void performAllPermutationTests(
            const D &data,
            std::size_t dataElementsCount,
            const int maxPiNameLength,
            const int maxLambdaNameLength,
            std::tuple<std::pair<std::string, Ps>...> compilablePis,
            std::tuple<std::pair<std::string, NPs>...> notCompilablePis,
            std::tuple<std::pair<std::string, Ls>...> compilableLambdas,
            std::tuple<std::pair<std::string, NLs>...> notCompilableLambdas) {

        performAllCompilablePermutationTests<D, 0>(data,
                                                   dataElementsCount,
                                                   maxPiNameLength,
                                                   maxLambdaNameLength,
                                                   compilablePis,
                                                   compilableLambdas,
                                                   notCompilableLambdas);

        performAllNotCompilablePermutationTests<D, 0>(data,
                                                      dataElementsCount,
                                                      maxPiNameLength,
                                                      maxLambdaNameLength,
                                                      notCompilablePis,
                                                      compilableLambdas,
                                                      notCompilableLambdas);
    }

private:
    template <typename D, std::size_t PI, typename... Ps, typename... Ls, typename... NLs>
    void performAllCompilablePermutationTests(
            const D &data,
            std::size_t dataElementsCount,
            const int maxPiNameLength,
            const int maxLambdaNameLength,
            std::tuple<std::pair<std::string, Ps>...> compilablePis,
            std::tuple<std::pair<std::string, Ls>...> compilableLambdas,
            std::tuple<std::pair<std::string, NLs>...> notCompilableLambdas) {

        if constexpr (PI < sizeof...(Ps)) {
            auto &current = std::get<PI>(compilablePis);
            auto &pi = current.second;
            performPiLambdaPermutationTest<true, D, decltype(pi)>(data,
                                                                  dataElementsCount,
                                                                  current.first,
                                                                  maxPiNameLength,
                                                                  maxLambdaNameLength,
                                                                  pi,
                                                                  compilableLambdas,
                                                                  notCompilableLambdas);

            performAllCompilablePermutationTests<D, PI + 1>(data,
                                                            dataElementsCount,
                                                            maxPiNameLength,
                                                            maxLambdaNameLength,
                                                            compilablePis,
                                                            compilableLambdas,
                                                            notCompilableLambdas);
        }
    }

    template <typename D, std::size_t PI, typename... NPs, typename... Ls, typename... NLs>
    void performAllNotCompilablePermutationTests(
            const D &data,
            std::size_t dataElementsCount,
            const int maxPiNameLength,
            const int maxLambdaNameLength,
            std::tuple<std::pair<std::string, NPs>...> notCompilablePis,
            std::tuple<std::pair<std::string, Ls>...> compilableLambdas,
            std::tuple<std::pair<std::string, NLs>...> notCompilableLambdas) {

        if constexpr (PI < sizeof...(NPs)) {
            auto &current = std::get<PI>(notCompilablePis);
            auto &pi = current.second;
            performPiLambdaPermutationTest<false, D, decltype(pi)>(data,
                                                                   dataElementsCount,
                                                                   current.first,
                                                                   maxPiNameLength,
                                                                   maxLambdaNameLength,
                                                                   pi,
                                                                   compilableLambdas,
                                                                   notCompilableLambdas);

            performAllNotCompilablePermutationTests<D, PI + 1>(data,
                                                               dataElementsCount,
                                                               maxPiNameLength,
                                                               maxLambdaNameLength,
                                                               notCompilablePis,
                                                               compilableLambdas,
                                                               notCompilableLambdas);
        }
    }

    template <bool C, typename D, typename P, typename... Ls, typename... NLs>
    void performPiLambdaPermutationTest(
            const D &data,
            std::size_t dataElementsCount,
            const std::string &name,
            const int maxPiNameLength,
            const int maxLambdaNameLength,
            P &pi,
            std::tuple<std::pair<std::string, Ls>...> compilableLambdas,
            std::tuple<std::pair<std::string, NLs>...> notCompilableLambdas) {

        performCompilablePiLambdaPermutationTest<C, D, P, 0>(data,
                                                             dataElementsCount,
                                                             name,
                                                             maxPiNameLength,
                                                             maxLambdaNameLength,
                                                             pi,
                                                             compilableLambdas);

        performNotCompilablePiLambdaPermutationTest<C, D, P, 0>(
                data, name, maxPiNameLength, maxLambdaNameLength, pi, notCompilableLambdas);
    }

    template <NotParallelDataIterator D>
    static void staticCheck(const std::string &name, const D &data, int maxTypeNameLength) {

        std::cout << std::setfill(' ') << std::setw(maxTypeNameLength) << name;
        std::cout << " | ";
        std::cout << ((!NotParallelDataIterator<D>) ? "\033[34mOK" : "\033[31mError") << "\033[0m"
                  << std::endl;
    }

    template <bool C, typename D, typename P, std::size_t LI, typename... Ls>
    void performCompilablePiLambdaPermutationTest(
            const D &data,
            std::size_t dataElementsCount,
            const std::string &name,
            const int maxPiNameLength,
            const int maxLambdaNameLength,
            P &pi,
            std::tuple<std::pair<std::string, Ls>...> &compilableLambdas) {

        if constexpr (LI < sizeof...(Ls)) {
            auto &currentElement = std::get<LI>(compilableLambdas);
            auto &lambda = currentElement.second;
            std::cout << std::setfill(' ') << std::setw(maxPiNameLength) << name;
            std::cout << " | ";
            std::cout << std::setfill(' ') << std::setw(maxLambdaNameLength)
                      << currentElement.first;
            std::cout << " | ";
            std::cout.flush();

            if constexpr (C) {
                ParallelClustering<true, true, true>::cluster<DistanceComputers::CLASSICAL>(
                        data, ELEMENTS, COORDINATES, pi, lambda, 6, 6, 6);

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
                                                              lambda + dataElementsCount,
                                                              this->expectedPi.cbegin(),
                                                              this->expectedPi.cend(),
                                                              this->expectedLambda.cbegin(),
                                                              this->expectedLambda.cend());
                    }
                } else {
                    if constexpr (cluster::utils::ConstIterable<decltype(lambda), double>) {
                        result = ResultsChecker::checkResults(pi,
                                                              pi + dataElementsCount,
                                                              lambda.cbegin(),
                                                              lambda.cend(),
                                                              this->expectedPi.cbegin(),
                                                              this->expectedPi.cend(),
                                                              this->expectedLambda.cbegin(),
                                                              this->expectedLambda.cend());
                    } else {
                        result = ResultsChecker::checkResults(pi,
                                                              pi + dataElementsCount,
                                                              lambda,
                                                              lambda + dataElementsCount,
                                                              this->expectedPi.cbegin(),
                                                              this->expectedPi.cend(),
                                                              this->expectedLambda.cbegin(),
                                                              this->expectedLambda.cend());
                    }
                }

                std::cout << ((result) ? "\033[32mOK" : "\033[31mError") << "\033[0m" << std::endl;
            } else {
                static_assert(!cluster::utils::PiIterator<P>,
                              "Pi seems to have a correct type, but it has not");

                std::cout << ((!C) ? "\033[34mOK" : "\033[31mError") << "\033[0m" << std::endl;
            }
            performCompilablePiLambdaPermutationTest<C, D, P, LI + 1, Ls...>(data,
                                                                             dataElementsCount,
                                                                             name,
                                                                             maxPiNameLength,
                                                                             maxLambdaNameLength,
                                                                             pi,
                                                                             compilableLambdas);
        }
    }

    template <bool C, typename D, typename P, std::size_t NLI, typename... NLs>
    void performNotCompilablePiLambdaPermutationTest(
            const D &data,
            const std::string &name,
            const int maxPiNameLength,
            const int maxLambdaNameLength,
            P &pi,
            std::tuple<std::pair<std::string, NLs>...> &notCompilableLambdas) {

        if constexpr (NLI < sizeof...(NLs)) {
            static_assert(
                    !cluster::utils::LambdaIterator<std::tuple_element<NLI, std::tuple<NLs...>>>,
                    "Lambda seems to have a correct type, but it has not");

            std::cout << std::setfill(' ') << std::setw(maxPiNameLength) << name;
            std::cout << " | ";
            std::cout << std::setfill(' ') << std::setw(maxLambdaNameLength)
                      << std::get<NLI>(notCompilableLambdas).first;
            std::cout << " | "
                      << ((!cluster::utils::LambdaIterator<
                                  std::tuple_element<NLI, std::tuple<NLs...>>>)
                                  ? "\033[34mOK"
                                  : "\033[31mError")
                      << "\033[0m" << std::endl;

            performNotCompilablePiLambdaPermutationTest<C, D, P, NLI + 1, NLs...>(
                    data, name, maxPiNameLength, maxLambdaNameLength, pi, notCompilableLambdas);
        }
    }

    const std::vector<std::size_t> &expectedPi;
    const std::vector<double> &expectedLambda;
    const int maxTypeNameLength;
};

void performPiLambdaTest(PiLambdaTypesTester<std::vector<double>> &tester);
void performContainersTest(const std::vector<double> &parsedData, Tester &tester);

int main() {

    // Read the data
    std::vector<double> parsedData;
    DataReader::readAndParseData(
            "../../test/resources/Parking Birmingham.data", parsedData, 0, 0, 2, 3);

    // Organize the data in indirect vectors
    std::vector<std::array<double, COORDINATES>> indirectParsedData;
    for (std::size_t i = 0; i < parsedData.size(); i += COORDINATES) {
        std::array<double, COORDINATES> point{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            point[j] = parsedData[i + j];
        }
        indirectParsedData.emplace_back(point);
    }

    std::vector<size_t> expectedPi{};
    expectedPi.resize(ELEMENTS);
    std::vector<double> expectedLambda{};
    expectedLambda.resize(ELEMENTS);

    std::cout << "Running sequential implementation to check the results" << std::endl;
    /*SequentialClustering::cluster(
            ContiguousDoubleMemoryDataIterator(&(parsedData.cbegin()[0]), COORDINATES),
            ELEMENTS,
            COORDINATES,
            expectedPi.begin(),
            expectedLambda.begin());*/
    DataReader::readPiLambda("../../out/birm-p-11-results.txt", expectedPi, expectedLambda);

    // Longest type: std::array<std::size_t, ELEMENTS>::const_iterator
    const constexpr int maxPiLength = 49;
    // Longest type: std::array<double, ELEMENTS>::const_iterator
    const constexpr int maxLambdaLength = 44;

    std::cout << std::endl;
    std::cout << std::left << std::setfill(' ') << std::setw(maxPiLength) << "pi";
    std::cout << " | ";
    std::cout << std::left << std::setfill(' ') << std::setw(maxLambdaLength) << "lambda";
    std::cout << " | Result" << std::endl;

    PiLambdaTypesTester<std::vector<double>> piLambdaTypesTester{parsedData,
                                                                 ELEMENTS,
                                                                 COORDINATES,
                                                                 maxPiLength,
                                                                 maxLambdaLength,
                                                                 expectedPi,
                                                                 expectedLambda};

    // performContainersTest(parsedData, tester);
    performPiLambdaTest(piLambdaTypesTester);

    // Longest type: std::vector<AlignedArray<double, N, AVX_ALIGNMENT>>

    /* std::cout << TypesPrinter::getTypeName<double>() << std::endl;
     std::cout << TypesPrinter::getTypeName<std::size_t>() << std::endl;
     std::cout << TypesPrinter::getTypeName<double *>() << std::endl;
     std::cout << TypesPrinter::getTypeName<std::vector<double>>() << std::endl;
     std::cout << TypesPrinter::getTypeName<std::list<double>>() << std::endl;
     std::cout << TypesPrinter::getTypeName<std::deque<double>>() << std::endl;
     std::cout << TypesPrinter::getTypeName<std::vector<std::list<double>>>() << std::endl;
     std::cout << TypesPrinter::getTypeName<std::array<double, 1000>>() << std::endl;
     std::cout << TypesPrinter::getTypeName<
                          std::array<std::vector<std::array<std::size_t, 10>>, 1000>>()
               << std::endl;
     std::cout << TypesPrinter::getTypeName<
                          std::array<std::vector<std::array<std::size_t, 10> *>, 1000> *>()
               << std::endl;
     std::cout << TypesPrinter::getTypeName<std::vector<double>::const_iterator>() << std::endl;
     std::cout << TypesPrinter::getTypeName<
                          std::vector<std::array<std::list<double>::const_iterator *,
                                                 1000>::const_iterator>::const_iterator>()
               << std::endl;
     std::cout << TypesPrinter::getTypeName<std::array<const double, 1000>>() << std::endl;
     std::cout << TypesPrinter::getTypeName<AlignedArray<double, 1000, 32>>() << std::endl;
     std::cout << TypesPrinter::getTypeName<AlignedArray<double, 1000, 16>>() << std::endl;
 */

    return 0;
}

void performContainersTest(const std::vector<double> &parsedData, Tester &tester) {
    // Fill the linear structures
    LinearCollectionContainer<ELEMENTS * COORDINATES> linearContainer{};
    CollectionCreator::createLinearContainers<ELEMENTS * COORDINATES>(parsedData, linearContainer);

    /*// Fill indirect C arrays
    auto cArrayContainers =
            CollectionCreator::createIndirectCArrays<COORDINATES>(indirectParsedData);
    // Fill indirect arrays
    auto arrayContainers = CollectionCreator::createIndirectArrays<COORDINATES>(indirectParsedData);
    // Fill indirect vectors
    auto vectorContainers =
            CollectionCreator::createIndirectVectors<COORDINATES>(indirectParsedData);
    // Fill indirect lists
    auto listContainers = CollectionCreator::createIndirectLists<COORDINATES>(indirectParsedData);
    // Fill indirect deques
    auto dequeueContainers =
            CollectionCreator::createIndirectDeques<COORDINATES>(indirectParsedData);
*/
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    // Perform linear tests
    tester.performParallelTest(linearContainer.cArray);
    tester.performParallelTest(linearContainer.array);
    tester.performParallelTest(linearContainer.arrayIterator);
    tester.performParallelTest(linearContainer.vector);
    tester.performParallelTest(linearContainer.vectorIterator);
    tester.performNotCompilableParallelTest(linearContainer.list);
    tester.performNotCompilableParallelTest(linearContainer.listIterator);
    tester.performNotCompilableParallelTest(linearContainer.deque);
    tester.performNotCompilableParallelTest(linearContainer.dequeIterator);

    /*

    performNotCompilableParallelTest(std::get<3>(linearContainer));
    performNotCompilableParallelTest(std::get<3>(linearContainer).begin());
    performNotCompilableParallelTest(std::get<4>(linearContainer));
    performNotCompilableParallelTest(std::get<5>(linearContainer).begin());
    tester.performParallelTest(std::get<5>(linearContainer).begin());
    tester.performParallelTest(std::get<6>(linearContainer).begin());
    /*
        // Perform indirect C arrays tests
        performParallelTest(std::get<0>(cArrayContainers));
        performParallelTest(std::get<1>(cArrayContainers));
        performParallelTest(std::get<2>(cArrayContainers));
        performParallelTest(std::get<3>(cArrayContainers));
        performParallelTest(std::get<4>(cArrayContainers));
        performNotCompilableParallelTest(std::get<5>(cArrayContainers));
        performNotCompilableParallelTest(std::get<6>(cArrayContainers));
        performParallelTest(std::get<7>(cArrayContainers));
        performParallelTest(std::get<8>(cArrayContainers));
        performParallelTest(std::get<9>(cArrayContainers));
        performParallelTest(std::get<10>(cArrayContainers));

        // Perform indirect arrays test
        performParallelTest(std::get<0>(arrayContainers).begin());
        performParallelTest(std::get<1>(arrayContainers).begin());
        performParallelTest(std::get<2>(arrayContainers).begin());
        performParallelTest(std::get<3>(arrayContainers).begin());
        performParallelTest(std::get<4>(arrayContainers).begin());
        performNotCompilableParallelTest(std::get<5>(arrayContainers).begin());
        performNotCompilableParallelTest(std::get<6>(arrayContainers).begin());
        performNotCompilableParallelTest(std::get<7>(arrayContainers).begin());
        performNotCompilableParallelTest(std::get<8>(arrayContainers).begin());
        performParallelTest(std::get<9>(arrayContainers).begin());
        performParallelTest(std::get<10>(arrayContainers).begin());

        // Perform indirect vectors tests
        performParallelTest(std::get<0>(vectorContainers).begin());
        performParallelTest(std::get<1>(vectorContainers).begin());
        performParallelTest(std::get<2>(vectorContainers).begin());
        performParallelTest(std::get<3>(vectorContainers).begin());
        performParallelTest(std::get<4>(vectorContainers).begin());
        performNotCompilableParallelTest(std::get<5>(vectorContainers).begin());
        performNotCompilableParallelTest(std::get<6>(vectorContainers).begin());
        performNotCompilableParallelTest(std::get<7>(vectorContainers).begin());
        performNotCompilableParallelTest(std::get<8>(vectorContainers).begin());
        performParallelTest(std::get<9>(vectorContainers).begin());
        performParallelTest(std::get<10>(vectorContainers).begin());

        // Perform indirect lists tests
        performNotCompilableParallelTest(std::get<0>(listContainers).begin());
        performNotCompilableParallelTest(std::get<1>(listContainers).begin());
        performNotCompilableParallelTest(std::get<2>(listContainers).begin());
        performNotCompilableParallelTest(std::get<3>(listContainers).begin());
        performNotCompilableParallelTest(std::get<4>(listContainers).begin());
        performNotCompilableParallelTest(std::get<5>(listContainers).begin());
        performNotCompilableParallelTest(std::get<6>(listContainers).begin());
        performNotCompilableParallelTest(std::get<7>(listContainers).begin());
        performNotCompilableParallelTest(std::get<8>(listContainers).begin());
        performNotCompilableParallelTest(std::get<9>(listContainers).begin());
        performNotCompilableParallelTest(std::get<10>(listContainers).begin());

        // Perform indirect deques tests
        performParallelTest(std::get<0>(vectorContainers).begin());
        performParallelTest(std::get<1>(vectorContainers).begin());
        performParallelTest(std::get<2>(vectorContainers).begin());
        performParallelTest(std::get<3>(vectorContainers).begin());
        performParallelTest(std::get<4>(vectorContainers).begin());
        performNotCompilableParallelTest(std::get<5>(vectorContainers).begin());
        performNotCompilableParallelTest(std::get<6>(vectorContainers).begin());
        performNotCompilableParallelTest(std::get<7>(vectorContainers).begin());
        performNotCompilableParallelTest(std::get<8>(vectorContainers).begin());
        performParallelTest(std::get<9>(vectorContainers).begin());
        performParallelTest(std::get<10>(dequeueContainers).begin());
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    */
}

void performPiLambdaTest(PiLambdaTypesTester<std::vector<double>> &tester) {

    auto *piCArray = new std::size_t[ELEMENTS];
    std::array<std::size_t, ELEMENTS> piArray{};
    std::vector<std::size_t> piVector{};
    piVector.resize(ELEMENTS);
    std::list<std::size_t> piList{};
    piList.resize(ELEMENTS);
    std::deque<std::size_t> piDeque{};
    piDeque.resize(ELEMENTS);
    AlignedArray<std::size_t, ELEMENTS, 16> piSseArray{};
    AlignedArray<std::size_t, ELEMENTS, 32> piAvxArray{};

    auto *lambdaCArray = new double[ELEMENTS];
    std::array<double, ELEMENTS> lambdaArray{};
    std::vector<double> lambdaVector{};
    lambdaVector.resize(ELEMENTS);
    std::list<double> lambdaList{};
    lambdaList.resize(ELEMENTS);
    std::deque<double> lambdaDeque{};
    lambdaDeque.resize(ELEMENTS);
    AlignedArray<double, ELEMENTS, 16> lambdaSseArray{};
    AlignedArray<double, ELEMENTS, 32> lambdaAvxArray{};

    auto compilablePis = std::make_tuple(piCArray,
                                         piArray,
                                         piArray.begin(),
                                         piVector,
                                         piVector.begin(),
                                         piDeque,
                                         piDeque.begin(),
                                         piSseArray,
                                         piAvxArray);
    auto notCompilablePis = std::make_tuple(piArray.cbegin(),
                                            piVector.cbegin(),
                                            piList,
                                            piList.begin(),
                                            piList.cbegin(),
                                            piDeque.cbegin());

    auto compilableLambdas = std::make_tuple(lambdaCArray,
                                             lambdaArray,
                                             lambdaArray.begin(),
                                             lambdaVector,
                                             lambdaVector.begin(),
                                             lambdaDeque,
                                             lambdaDeque.begin(),
                                             lambdaSseArray,
                                             lambdaAvxArray);
    auto notCompilableLambdas = std::make_tuple(lambdaList,
                                                lambdaList.begin(),
                                                lambdaArray.cbegin(),
                                                lambdaVector.cbegin(),
                                                lambdaList.cbegin(),
                                                lambdaDeque.cbegin());

    tester.testAllPermutations(
            compilablePis, notCompilablePis, compilableLambdas, notCompilableLambdas);

    std::cout << "Test terminated" << std::endl;

    /*tester.performAllPermutationTests(data,
                                      ELEMENTS,
                                      maxPiLength,
                                      maxLambdaLength,
                                      std::make_tuple(/piCArrayPair,
                                                      piArrayPair,
                                                      piArrayIteratorPair,
                                                      piVectorPair,
                                                      piVectorIteratorPair,
                                                      piDequePair,
                                                      piDequeIteratorPair,
                                                      piSseArrayPair,
                                                      piAvxArrayPair/
                                                      piArrayConstIteratorPair),
                                      std::make_tuple(piListPair,
                                                      piListIteratorPair,
                                                      piArrayConstIteratorPair,
                                                      piVectorConstIteratorPair,
                                                      piDequeConstIteratorPair),
                                      std::make_tuple(lambdaCArrayPair,
                                                      lambdaArrayPair,
                                                      lambdaArrayIteratorPair,
                                                      lambdaVectorPair,
                                                      lambdaVectorIteratorPair,
                                                      lambdaDequePair,
                                                      lambdaDequeIteratorPair,
                                                      lambdaSseArrayPair,
                                                      lambdaAvxArrayPair),
                                      std::make_tuple(lambdaListPair, lambdaListIteratorPair));*/

    delete[] piCArray;
    delete[] lambdaCArray;
}
