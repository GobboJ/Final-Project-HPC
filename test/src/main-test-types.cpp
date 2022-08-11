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
#include "types/DataTypesTester.h"
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
using cluster::test::types::DataTypesTester;
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

private:
    template <NotParallelDataIterator D>
    static void staticCheck(const std::string &name, const D &data, int maxTypeNameLength) {

        std::cout << std::setfill(' ') << std::setw(maxTypeNameLength) << name;
        std::cout << " | ";
        std::cout << ((!NotParallelDataIterator<D>) ? "\033[34mOK" : "\033[31mError") << "\033[0m"
                  << std::endl;
    }

    const std::vector<std::size_t> &expectedPi;
    const std::vector<double> &expectedLambda;
    const int maxTypeNameLength;
};

void testPiLambdaTypes(const std::vector<double> &parsedData,
                       const std::vector<size_t> &expectedPi,
                       const std::vector<double> &expectedLambda);
void testParallelDataStructureTypes(const std::vector<double> &parsedData,
                                    const std::vector<size_t> &expectedPi,
                                    const std::vector<double> &expectedLambda);

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
    expectedPi.resize(parsedData.size() / COORDINATES);
    std::vector<double> expectedLambda{};
    expectedLambda.resize(parsedData.size() / COORDINATES);

    std::cout << "Running sequential implementation to check the results" << std::endl;
    /*SequentialClustering::cluster(
            ContiguousDoubleMemoryDataIterator(&(parsedData.cbegin()[0]), COORDINATES),
            parsedData.size() / COORDINATES,
            COORDINATES,
            expectedPi.begin(),
            expectedLambda.begin());*/
    DataReader::readPiLambda("../../out/birm-p-11-results.txt", expectedPi, expectedLambda);

    // testPiLambdaTypes(parsedData, expectedPi, expectedLambda);
    testParallelDataStructureTypes(parsedData, expectedPi, expectedLambda);

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

void testParallelDataStructureTypes(const std::vector<double> &parsedData,
                                    const std::vector<size_t> &expectedPi,
                                    const std::vector<double> &expectedLambda) {

    // Longest type: std::vector<AlignedArray<double, N, AVX_ALIGNMENT>>
    const constexpr std::size_t maxTypeLength = 51;
    std::cout << std::endl;
    std::cout << std::left << std::setfill(' ') << std::setw(maxTypeLength) << "Data structure";
    std::cout << " | Result" << std::endl;

    // Max result string: Error (should compile)
    const constexpr std::size_t separatorLength = maxTypeLength + 3 + 22;

    for (std::size_t i = 0; i < separatorLength; i++) {
        std::cout << '-';
    }
    std::cout << std::endl;
    
    const std::size_t dataElementsCount = parsedData.size() / COORDINATES;
    DataTypesTester dataTypesTester{
            dataElementsCount, COORDINATES, expectedPi, expectedLambda, maxTypeLength};

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

    auto compilableDataStructures = std::make_tuple(linearContainer.cArray,
                                                    linearContainer.array,
                                                    linearContainer.arrayIterator,
                                                    linearContainer.arrayConstIterator,
                                                    linearContainer.vector,
                                                    linearContainer.vectorIterator,
                                                    linearContainer.vectorConstIterator,
                                                    /*TODO: linearContainer.deque,
                                                    linearContainer.dequeIterator,
                                                    linearContainer.dequeConstIterator,*/
                                                    linearContainer.sseAlignedArray,
                                                    linearContainer.avxAlignedArray);
    auto notCompilableDataStructures = std::make_tuple(
            linearContainer.list, linearContainer.listIterator, linearContainer.listConstIterator);

    dataTypesTester.testParallelTypes(compilableDataStructures, notCompilableDataStructures);

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

void testPiLambdaTypes(const std::vector<double> &parsedData,
                       const std::vector<size_t> &expectedPi,
                       const std::vector<double> &expectedLambda) {
    // Longest type: std::array<std::size_t, ELEMENTS>::const_iterator
    const constexpr int maxPiLength = 49;
    // Longest type: std::array<double, ELEMENTS>::const_iterator
    const constexpr int maxLambdaLength = 44;

    std::cout << std::endl;
    std::cout << std::left << std::setfill(' ') << std::setw(maxPiLength) << "pi";
    std::cout << " | ";
    std::cout << std::left << std::setfill(' ') << std::setw(maxLambdaLength) << "lambda";
    std::cout << " | Result" << std::endl;

    // Max result string: Error (lambda should not compile)
    const constexpr std::size_t separatorLength = maxPiLength + maxLambdaLength + 6 + 33;

    for (std::size_t i = 0; i < separatorLength; i++) {
        std::cout << '-';
    }
    std::cout << std::endl;

    const std::size_t dataElementsCount = parsedData.size() / COORDINATES;
    PiLambdaTypesTester<std::vector<double>> piLambdaTypesTester{parsedData,
                                                                 dataElementsCount,
                                                                 COORDINATES,
                                                                 maxPiLength,
                                                                 maxLambdaLength,
                                                                 expectedPi,
                                                                 expectedLambda};

    auto *piCArray = new std::size_t[dataElementsCount];
    std::array<std::size_t, ELEMENTS> piArray{};
    std::vector<std::size_t> piVector{};
    piVector.resize(dataElementsCount);
    std::list<std::size_t> piList{};
    piList.resize(dataElementsCount);
    std::deque<std::size_t> piDeque{};
    piDeque.resize(dataElementsCount);
    AlignedArray<std::size_t, ELEMENTS, 16> piSseArray{};
    AlignedArray<std::size_t, ELEMENTS, 32> piAvxArray{};

    auto *lambdaCArray = new double[dataElementsCount];
    std::array<double, ELEMENTS> lambdaArray{};
    std::vector<double> lambdaVector{};
    lambdaVector.resize(dataElementsCount);
    std::list<double> lambdaList{};
    lambdaList.resize(dataElementsCount);
    std::deque<double> lambdaDeque{};
    lambdaDeque.resize(dataElementsCount);
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

    piLambdaTypesTester.testAllPermutations(
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
