/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-07
 * @since version date
 */
#include "../src/data/DataReader.h"
#include "../src/data/DataWriter.h"
#include "../src/data/ResultsChecker.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"
#include "types/CollectionCreator.h"
#include "types/DataTypesTester.h"
#include "types/PiLambdaTypesTester.h"
#include "types/TypesPrinter.h"
//#include "types/containers/collection/CollectionContainer.h"
#include "types/containers/PiLambdaContainer.h"
#include "types/containers/array/ArrayContainer.h"
#include <deque>
#include <list>

using cluster::parallel::DistanceComputers;
using cluster::parallel::ParallelClustering;
using cluster::sequential::SequentialClustering;
using cluster::test::ResultsChecker;
using cluster::test::data::DataReader;
using cluster::test::data::DataWriter;
using cluster::test::types::CollectionCreator;
using cluster::test::types::DataTypesTester;
using cluster::test::types::PiLambdaTypesTester;
using cluster::test::types::TypesPrinter;
using cluster::test::types::containers::array::ArrayContainer;
using cluster::test::types::collections::AlignedArray;
using cluster::test::types::collections::OnlyConstIterableVector;
using cluster::test::types::collections::OnlyIterableVector;
//using cluster::test::types::containers::LinearCollectionContainer;
using cluster::test::types::containers::PiLambdaContainer;
using cluster::utils::ConstIterable;
using cluster::utils::ContiguousConstIterable;
using cluster::utils::ContiguousIterable;
using cluster::utils::ContiguousIterator;
using cluster::utils::DataIteratorUtils;
using cluster::utils::DataIteratorUtils;
using cluster::utils::InputIterator;
using cluster::utils::Iterable;
using cluster::utils::OutputIterator;
using cluster::utils::ParallelDataIterator;
using cluster::utils::PiLambdaIteratorUtils;
using cluster::utils::RandomIterable;
using cluster::utils::RandomIterator;
using IteratorType = PiLambdaTypesTester<std::vector<double>>::IteratorType;

const std::size_t ELEMENTS = 35717;
const std::size_t DIMENSION = 2;

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
                data.second, ELEMENTS, DIMENSION, piIterator, lambdaIterator, 6, 6, 6);

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
void testParallelDataStructureTypes(
        const std::vector<double> &parsedData,
        const std::vector<std::array<double, DIMENSION>> &indirectParsedData,
        const std::vector<size_t> &expectedPi,
        const std::vector<double> &expectedLambda);

int main() {

    // Read the data
    std::vector<double> parsedData;
    DataReader::readAndParseData(
            "../../test/resources/Parking Birmingham.data", parsedData, 0, 0, 2, 3);
    //"../../test/resources/slides.data", parsedData, 0, 0, 1, 2);

    // Organize the data in indirect vectors
    std::vector<std::array<double, DIMENSION>> indirectParsedData{};
    for (std::size_t i = 0; i < parsedData.size(); i += DIMENSION) {
        std::array<double, DIMENSION> point{};
        for (std::size_t j = 0; j < DIMENSION; j++) {
            point[j] = parsedData[i + j];
        }
        indirectParsedData.emplace_back(point);
    }

    std::vector<std::size_t> expectedPi{};
    expectedPi.resize(parsedData.size() / DIMENSION);
    std::vector<double> expectedLambda{};
    expectedLambda.resize(parsedData.size() / DIMENSION);

    std::cout << "Running sequential implementation to check the results" << std::endl;
    /*SequentialClustering::cluster(
            ContiguousDoubleMemoryDataIterator(&(parsedData.cbegin()[0]), COORDINATES),
            parsedData.size() / COORDINATES,
            COORDINATES,
            expectedPi.begin(),
            expectedLambda.begin());*/
    DataReader::readPiLambda("../../out/birm-p-11-results.txt", expectedPi, expectedLambda);
    std::cout << "End" << std::endl;

    // testPiLambdaTypes(parsedData, expectedPi, expectedLambda);
    testParallelDataStructureTypes(parsedData, indirectParsedData, expectedPi, expectedLambda);

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

template <typename... Ts>
std::tuple<Ts &...> wrapReferences(Ts &...arguments) {

    return std::tuple<Ts &...>(arguments...);
}

template <typename T>
std::pair<T &, DataType> wrapDataStructure(T &dataStructure, DataType iteratorType) {

    return std::pair<T &, DataType>(dataStructure, iteratorType);
}

template <typename C>
auto wrapCompilableCollections(C &container) {

    return std::tuple_cat(container.cArrays.getCollectionsInfo(),
                          container.constCArrays.getCollectionsInfo(),
                          container.arrays.getCollectionsInfo(),
                          container.vectors.getCollectionsInfo(),
                          container.sseAlignedArrays.getCollectionsInfo(),
                          container.avxAlignedArrays.getCollectionsInfo(),
                          container.onlyIterables.getCollectionsInfo(),
                          container.onlyConstIterables.getCollectionsInfo());
}

template <typename C>
auto wrapNotCompilableCollections(C &container) {

    return std::tuple_cat(container.lists.getCollectionsInfo(),
                          container.deques.getCollectionsInfo(),
                          container.integers.getCollectionsInfo(),
                          container.paths.getCollectionsInfo(),
                          container.pathsVector.getCollectionsInfo(),
                          container.pairs.getCollectionsInfo(),
                          container.strings.getCollectionsInfo());
}

void testParallelDataStructureTypes(
        const std::vector<double> &parsedData,
        const std::vector<std::array<double, DIMENSION>> &indirectParsedData,
        const std::vector<size_t> &expectedPi,
        const std::vector<double> &expectedLambda) {

    // Longest type:
    // std::vector<std::vector<std::filesystem::path::const_iterator>::const_iterator>::const_iterator
    const constexpr std::size_t maxTypeLength = 95;
    // Longest summary: Contiguous const iterable of const iterables
    const constexpr std::size_t maxSummaryLength = 44;
    // Longest Result: OK (Contiguous const iterable of const iterables)
    const constexpr std::size_t maxResultLength = 49;
    std::cout << std::endl;
    std::cout << std::left << std::setfill(' ') << std::setw(maxTypeLength) << "Data structure";
    std::cout << " | ";
    std::cout << std::left << std::setfill(' ') << std::setw(maxResultLength) << "Result";
    std::cout << " | ";
    std::cout << std::left << std::setfill(' ') << std::setw(maxSummaryLength) << "Overload";
    std::cout << std::endl;

    // Max result string: Error (should compile)
    const constexpr std::size_t separatorLength =
            maxTypeLength + maxSummaryLength + 6 + maxResultLength;

    for (std::size_t i = 0; i < separatorLength; i++) {
        std::cout << '-';
    }
    std::cout << std::endl;

    DataIteratorUtils::printSummaries = true;
    PiLambdaIteratorUtils::printSummaries = false;

    const std::size_t dataElementsCount = parsedData.size() / DIMENSION;
    DataTypesTester dataTypesTester{dataElementsCount,
                                    DIMENSION,
                                    expectedPi,
                                    expectedLambda,
                                    maxTypeLength,
                                    maxResultLength};
    /*
        // Fill the linear structures
        auto *const linearContainer = new LinearCollectionContainer<ELEMENTS * DIMENSION>();
        CollectionCreator::createLinearContainers<ELEMENTS * DIMENSION>(parsedData,
       *linearContainer);

        // Fill indirect C arrays
        auto *const indirectCArrays = new CollectionContainer<ArrayWrapper, DIMENSION>();
        CollectionCreator::createIndirectCArrays<DIMENSION>(indirectParsedData, *indirectCArrays);
*/
    // Fill indirect arrays
    auto *const arraysContainer = new ArrayContainer<std::array, ELEMENTS, DIMENSION>(
            "std::array", DataType::CONTIGUOUS);
    CollectionCreator::createIndirectArrays<decltype(*arraysContainer), DIMENSION>(indirectParsedData, *arraysContainer);
    /*
    // Fill indirect vectors
    auto *const vectorsContainer = new CollectionContainer<std::vector, DIMENSION>(
            "std::vector", DataIteratorType::CONTIGUOUS);
    CollectionCreator::createIndirectVectors<DIMENSION>(indirectParsedData, *vectorsContainer);
    
    // Fill indirect lists
    auto *const listsContainer =
            new CollectionContainer<std::list, DIMENSION>("std::list", DataIteratorType::INPUT);
    CollectionCreator::createIndirectLists<DIMENSION>(indirectParsedData, *listsContainer);

    // Fill indirect deques
    auto *const dequesContainer =
            new CollectionContainer<std::deque, DIMENSION>("std::deque", DataIteratorType::RANDOM);
    CollectionCreator::createIndirectDeques<DIMENSION>(indirectParsedData, *dequesContainer);

    // Fill indirect only iterables
    auto *const onlyIterablesContainer = new CollectionContainer<OnlyIterableVector,
   DIMENSION>(); CollectionCreator::createIndirectOnlyIterables<DIMENSION>(indirectParsedData,
                                                              *onlyIterablesContainer);

    // Fill indirect only const iterables
    auto *const onlyConstIterablesContainer =
            new CollectionContainer<OnlyConstIterableVector, DIMENSION>();
    CollectionCreator::createIndirectOnlyConstIterables<DIMENSION>(indirectParsedData,
                                                                   *onlyConstIterablesContainer);
*/
    auto compilableDataStructures = std::tuple_cat(std::make_tuple(
            // Linear types
            /*wrapDataStructure(linearContainer->cArray, DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->cConstArray, DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->array, DataIteratorType::CONTIGUOUS_CONST_ITERABLE),
            wrapDataStructure(linearContainer->arrayIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->arrayConstIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->vector, DataIteratorType::CONTIGUOUS_CONST_ITERABLE),
            wrapDataStructure(linearContainer->vectorIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->vectorConstIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->sseAlignedArray,
                              DataIteratorType::CONTIGUOUS_CONST_ITERABLE),
            wrapDataStructure(linearContainer->sseAlignedArrayIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->sseAlignedArrayConstIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->avxAlignedArray,
                              DataIteratorType::CONTIGUOUS_CONST_ITERABLE),
            wrapDataStructure(linearContainer->avxAlignedArrayIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->avxAlignedArrayConstIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->onlyIterable,
                              DataIteratorType::CONTIGUOUS_CONST_ITERABLE),
            wrapDataStructure(linearContainer->onlyIterableIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR),
            wrapDataStructure(linearContainer->onlyConstIterable,
                              DataIteratorType::CONTIGUOUS_CONST_ITERABLE),
            wrapDataStructure(linearContainer->onlyConstIterableIterator,
                              DataIteratorType::CONTIGUOUS_ITERATOR)*/),
            // Indirect C arrays
            
            // Indirect std::array
            wrapCompilableCollections(*arraysContainer)
            // Indirect std::vector
         //   wrapCompilableCollections(*vectorsContainer)
           //TODO: wrapCompilableCollections(*dequesContainer)
    );

    // Indirect std::array

    // Indirect std::list
    // Indirect std::deque
    // Indirect OnlyIterableVector
    // Indirect OnlyConstIterableVector

    auto notCompilableDataStructures = std::tuple_cat(
            wrapNotCompilableCollections(*arraysContainer)
            //wrapNotCompilableCollections(*vectorsContainer)
            //TODO:wrapNotCompilableCollections(*dequesContainer),
            //TODO:wrapCompilableCollections(*listsContainer),
            //TODO:wrapNotCompilableCollections(*listsContainer)
            // Linear types
            /*linearContainer->list,
            linearContainer->listIterator,
            linearContainer->listConstIterator,
            linearContainer->deque,
            linearContainer->dequeIterator,
            linearContainer->dequeConstIterator,
            linearContainer->integers,
            linearContainer->integersIterator,
            linearContainer->integersConstIterator,
            linearContainer->path,
            linearContainer->pathConstIterator,
            linearContainer->pathsVector,
            linearContainer->pathsVectorIterator,
            linearContainer->pathsVectorConstIterator,
            linearContainer->pair,
            linearContainer->pairsArray,
            linearContainer->pairsArrayIterator,
            linearContainer->pairsArrayConstIterator,
            linearContainer->string,
            linearContainer->stringIterator,
            linearContainer->stringConstIterator/*,
            // Indirect C arrays
            indirectCArrays->list,
            indirectCArrays->iteratorsList,
            indirectCArrays->constIteratorsList,
            indirectCArrays->deque,
            indirectCArrays->iteratorsDeque,
            indirectCArrays->constIteratorsDeque,
            indirectCArrays->integers,
            indirectCArrays->integersIterator,
            indirectCArrays->integersConstIterator,
            indirectCArrays->path,
            indirectCArrays->pathConstIterator,
            indirectCArrays->pathsVector,
            indirectCArrays->pairsArray,
            indirectCArrays->string,
            indirectCArrays->stringIterator,
            indirectCArrays->stringConstIterator,
            // Indirect std::array
            arraysContainer->list,
            arraysContainer->listsIterator,
            arraysContainer->listsConstIterator,
            arraysContainer->iteratorsList,
            arraysContainer->constIteratorsList,
            arraysContainer->deque,
            arraysContainer->dequesIterator,
            arraysContainer->dequesConstIterator,
            arraysContainer->iteratorsDeque,
            arraysContainer->constIteratorsDeque,
            arraysContainer->integers,
            arraysContainer->iteratorIntegers,
            arraysContainer->constIteratorIntegers,
            arraysContainer->integersIterator,
            arraysContainer->integersConstIterator,
            arraysContainer->path,
            arraysContainer->pathConstIterator,
            arraysContainer->pathsVector,
            arraysContainer->pairsArray,
            arraysContainer->string,
            arraysContainer->stringIterator,
            arraysContainer->stringConstIterator,
            // Indirect std::vector
            vectorsContainer->list,
            vectorsContainer->listsIterator,
            vectorsContainer->listsConstIterator,
            vectorsContainer->iteratorsList,
            vectorsContainer->constIteratorsList,
            vectorsContainer->deque,
            vectorsContainer->dequesIterator,
            vectorsContainer->dequesConstIterator,
            vectorsContainer->iteratorsDeque,
            vectorsContainer->constIteratorsDeque,
            vectorsContainer->integers,
            vectorsContainer->integersIterator,
            vectorsContainer->integersConstIterator,
            vectorsContainer->iteratorIntegers,
            vectorsContainer->constIteratorIntegers,
            vectorsContainer->path,
            vectorsContainer->pathConstIterator,
            vectorsContainer->pathsVector,
            vectorsContainer->pairsArray,
            vectorsContainer->string,
            vectorsContainer->stringIterator,
            vectorsContainer->stringConstIterator,
            // Indirect std::list
            listsContainer->cArray,
            listsContainer->cArrayIterator,
            listsContainer->cArrayConstIterator,
            listsContainer->array,
            listsContainer->arraysIterator,
            listsContainer->arraysConstIterator,
            listsContainer->iteratorsArray,
            listsContainer->constIteratorsArray,
            listsContainer->vector,
            listsContainer->vectorsIterator,
            listsContainer->vectorsConstIterator,
            listsContainer->iteratorsVector,
            listsContainer->constIteratorsVector,
            listsContainer->list,
            listsContainer->listsIterator,
            listsContainer->listsConstIterator,
            listsContainer->iteratorsList,
            listsContainer->constIteratorsList,
            listsContainer->deque,
            listsContainer->dequesIterator,
            listsContainer->dequesConstIterator,
            listsContainer->iteratorsDeque,
            listsContainer->constIteratorsDeque,
            listsContainer->sseAlignedArray,
            listsContainer->avxAlignedArray,
            listsContainer->integers,
            listsContainer->integersIterator,
            listsContainer->integersConstIterator,
            listsContainer->iteratorIntegers,
            listsContainer->constIteratorIntegers,
            listsContainer->path,
            listsContainer->pathConstIterator,
            listsContainer->pathsVector,
            listsContainer->pairsArray,
            listsContainer->string,
            listsContainer->stringIterator,
            listsContainer->stringConstIterator,
            // Indirect std::deque
            dequesContainer->list,
            dequesContainer->listsIterator,
            dequesContainer->listsConstIterator,
            dequesContainer->iteratorsList,
            dequesContainer->constIteratorsList,
            dequesContainer->deque,
            dequesContainer->dequesIterator,
            dequesContainer->dequesConstIterator,
            dequesContainer->iteratorsDeque,
            dequesContainer->constIteratorsDeque,
            dequesContainer->integers,
            dequesContainer->integersIterator,
            dequesContainer->integersConstIterator,
            dequesContainer->iteratorIntegers,
            dequesContainer->constIteratorIntegers,
            dequesContainer->path,
            dequesContainer->pathConstIterator,
            dequesContainer->pathsVector,
            dequesContainer->pairsArray,
            dequesContainer->string,
            dequesContainer->stringIterator,
            dequesContainer->stringConstIterator,
            // Indirect OnlyIterableVector
            onlyIterablesContainer->list,
            onlyIterablesContainer->listsIterator,
            onlyIterablesContainer->listsConstIterator,
            onlyIterablesContainer->iteratorsList,
            onlyIterablesContainer->constIteratorsList,
            onlyIterablesContainer->deque,
            onlyIterablesContainer->dequesIterator,
            onlyIterablesContainer->dequesConstIterator,
            onlyIterablesContainer->iteratorsDeque,
            onlyIterablesContainer->constIteratorsDeque,
            onlyIterablesContainer->integers,
            onlyIterablesContainer->integersIterator,
            onlyIterablesContainer->integersConstIterator,
            onlyIterablesContainer->iteratorIntegers,
            onlyIterablesContainer->constIteratorIntegers,
            onlyIterablesContainer->path,
            onlyIterablesContainer->pathConstIterator,
            onlyIterablesContainer->pathsVector,
            onlyIterablesContainer->pairsArray,
            onlyIterablesContainer->string,
            onlyIterablesContainer->stringIterator,
            onlyIterablesContainer->stringConstIterator,*/
            // Indirect OnlyConstIterableVector
            /*onlyConstIterablesContainer->list,
            onlyConstIterablesContainer->listsIterator,
            onlyConstIterablesContainer->listsConstIterator,
            onlyConstIterablesContainer->iteratorsList,
            onlyConstIterablesContainer->constIteratorsList,
            onlyConstIterablesContainer->deque,
            onlyConstIterablesContainer->dequesIterator,
            onlyConstIterablesContainer->dequesConstIterator,
            onlyConstIterablesContainer->iteratorsDeque,
            onlyConstIterablesContainer->constIteratorsDeque,
            onlyConstIterablesContainer->integers,
            onlyConstIterablesContainer->integersIterator,
            onlyConstIterablesContainer->integersConstIterator,
            onlyConstIterablesContainer->iteratorIntegers,
            onlyConstIterablesContainer->constIteratorIntegers,
            onlyConstIterablesContainer->path,
            onlyConstIterablesContainer->pathConstIterator,
            onlyConstIterablesContainer->pathsVector,
            onlyConstIterablesContainer->pairsArray,
            onlyConstIterablesContainer->string,
            onlyConstIterablesContainer->stringIterator,
            onlyConstIterablesContainer->stringConstIterator*/);

    dataTypesTester.testParallelTypes(compilableDataStructures, notCompilableDataStructures);

    //  delete linearContainer;
    /*delete indirectCArrays;
    delete arraysContainer;
    delete vectorsContainer;
    delete listsContainer;
    delete dequesContainer;
    delete onlyIterablesContainer;
    delete onlyConstIterablesContainer;*/
}

void testPiLambdaTypes(const std::vector<double> &parsedData,
                       const std::vector<size_t> &expectedPi,
                       const std::vector<double> &expectedLambda) {
    // Longest type: std::vector<std::filesystem::path::const_iterator>
    const constexpr int maxPiLength = 50;
    // Longest type: std::vector<std::filesystem::path::const_iterator>
    const constexpr int maxLambdaLength = 50;

    std::cout << std::endl;
    std::cout << "Type | ";
    std::cout << std::left << std::setfill(' ') << std::setw(maxPiLength) << "pi";
    std::cout << " | ";
    std::cout << std::left << std::setfill(' ') << std::setw(maxLambdaLength) << "lambda";
    std::cout << " | ";
    std::cout << std::left << std::setfill(' ') << std::setw(19) << "pi iterator";
    std::cout << " | ";
    std::cout << std::left << std::setfill(' ') << std::setw(19) << "lambda iterator";
    std::cout << " | Result" << std::endl;

    // Max result string: Error (lambda should not compile)
    const constexpr std::size_t separatorLength = maxPiLength + maxLambdaLength + 6 + 33 + (19 * 2);

    for (std::size_t i = 0; i < separatorLength; i++) {
        std::cout << '-';
    }
    std::cout << std::endl;

    DataIteratorUtils::printSummaries = false;
    PiLambdaIteratorUtils::printSummaries = true;

    const std::size_t dataElementsCount = parsedData.size() / DIMENSION;
    PiLambdaTypesTester<std::vector<double>> piLambdaTypesTester{parsedData,
                                                                 dataElementsCount,
                                                                 DIMENSION,
                                                                 maxPiLength,
                                                                 maxLambdaLength,
                                                                 expectedPi,
                                                                 expectedLambda};

    PiLambdaContainer<std::size_t, ELEMENTS> piContainer{};
    PiLambdaContainer<double, ELEMENTS> lambdaContainer{};

    auto contiguousIteratorPis = wrapReferences(piContainer.cArray,
                                                piContainer.arrayIterator,
                                                piContainer.vectorIterator,
                                                piContainer.onlyIterableIterator);
    auto contiguousIterablePis = wrapReferences(piContainer.array,
                                                piContainer.vector,
                                                piContainer.sseArray,
                                                piContainer.avxArray,
                                                piContainer.onlyIterable);
    auto randomIteratorPis = wrapReferences(piContainer.dequeIterator);
    auto randomIterablePis = wrapReferences(piContainer.deque);
    auto notCompilablePis = wrapReferences(piContainer.arrayConstIterator,
                                           piContainer.vectorConstIterator,
                                           piContainer.list,
                                           piContainer.listIterator,
                                           piContainer.listConstIterator,
                                           piContainer.dequeConstIterator,
                                           piContainer.onlyConstIterable,
                                           piContainer.onlyConstIterableConstIterator,
                                           piContainer.integers,
                                           piContainer.integersIterator,
                                           piContainer.integersConstIterator,
                                           piContainer.twoLevelsArray,
                                           piContainer.twoLevelsArrayIterator,
                                           piContainer.twoLevelsArrayConstIterator,
                                           piContainer.path,
                                           piContainer.pathIterator,
                                           piContainer.pathsVector,
                                           piContainer.pairsArray,
                                           piContainer.string,
                                           piContainer.stringIterator,
                                           piContainer.stringConstIterator);

    auto contiguousIteratorLambdas = wrapReferences(lambdaContainer.cArray,
                                                    lambdaContainer.arrayIterator,
                                                    lambdaContainer.vectorIterator,
                                                    lambdaContainer.onlyIterableIterator);
    auto contiguousIterableLambdas = wrapReferences(lambdaContainer.array,
                                                    lambdaContainer.vector,
                                                    lambdaContainer.sseArray,
                                                    lambdaContainer.avxArray,
                                                    lambdaContainer.onlyIterable);
    auto randomIteratorLambdas = wrapReferences(lambdaContainer.dequeIterator);
    auto randomIterableLambdas = wrapReferences(lambdaContainer.deque);
    auto notCompilableLambdas = wrapReferences(lambdaContainer.arrayConstIterator,
                                               lambdaContainer.vectorConstIterator,
                                               lambdaContainer.list,
                                               lambdaContainer.listIterator,
                                               lambdaContainer.listConstIterator,
                                               lambdaContainer.dequeConstIterator,
                                               lambdaContainer.onlyConstIterable,
                                               lambdaContainer.onlyConstIterableConstIterator,
                                               lambdaContainer.integers,
                                               lambdaContainer.integersIterator,
                                               lambdaContainer.integersConstIterator,
                                               lambdaContainer.twoLevelsArray,
                                               lambdaContainer.twoLevelsArrayIterator,
                                               lambdaContainer.twoLevelsArrayConstIterator,
                                               lambdaContainer.path,
                                               lambdaContainer.pathIterator,
                                               lambdaContainer.pathsVector,
                                               lambdaContainer.pairsArray,
                                               lambdaContainer.string,
                                               lambdaContainer.stringIterator,
                                               lambdaContainer.stringConstIterator);

    /*
    auto *const compilablePis = wrapReferences(piCArray,
                                         *piArray,
                                         piArray->begin(),
                                         piVector,
                                         piVector.begin(),
                                         piDeque,
                                         piDeque.begin(),
                                         piSseArray,
                                         piAvxArray,
                                         piOnlyIterable,
                                         piOnlyIterable.begin());

    auto * const notCompilablePis = wrapReferences(piArray->cbegin(),
                                            piVector.cbegin(),
                                            piList,
                                            piList.begin(),
                                            piList.cbegin(),
                                            piDeque.cbegin(),
                                            piOnlyConstIterable,
                                            piOnlyConstIterable.cbegin(),
                                            piIntegers,
                                            piIntegers.begin(),
                                            piIntegers.cbegin(),
                                            piPath,
                                            piPath.begin(),
                                            piPathsVector,
                                            piPathsVector.begin(),
                                            piPathsVector.cbegin(),
                                            piPairsArray,
                                            piString,
                                            piString.begin(),
                                            piString.cbegin());

    auto * const compilableLambdas = wrapReferences(lambdaCArray,
                                             *lambdaArray,
                                             lambdaArray->begin(),
                                             lambdaVector,
                                             lambdaVector.begin(),
                                             lambdaDeque,
                                             lambdaDeque.begin(),
                                             lambdaSseArray,
                                             lambdaAvxArray,
                                             lambdaOnlyIterable,
                                             lambdaOnlyIterable.begin());
    auto * const notCompilableLambdas = wrapReferences(lambdaList,
                                                lambdaList.begin(),
                                                lambdaArray->cbegin(),
                                                lambdaVector.cbegin(),
                                                lambdaList.cbegin(),
                                                lambdaDeque.cbegin(),
                                                lambdaOnlyConstIterable,
                                                lambdaOnlyConstIterable.cbegin(),
                                                lambdaIntegers,
                                                lambdaIntegers.begin(),
                                                lambdaIntegers.cbegin(),
                                                lambdaPath,
                                                lambdaPath.begin(),
                                                lambdaPathsVector,
                                                lambdaPathsVector.begin(),
                                                lambdaPathsVector.cbegin(),
                                                lambdaPairsArray,
                                                lambdaString,
                                                lambdaString.begin(),
                                                lambdaString.cbegin());*/

    piLambdaTypesTester.testAllPermutations(contiguousIteratorPis,
                                            contiguousIterablePis,
                                            randomIteratorPis,
                                            randomIterablePis,
                                            notCompilablePis,
                                            contiguousIteratorLambdas,
                                            contiguousIterableLambdas,
                                            randomIteratorLambdas,
                                            randomIterableLambdas,
                                            notCompilableLambdas);

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
}
