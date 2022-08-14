/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-14
 * @since version date
 */
/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-07
 * @since version date
 */
#include "ParallelClustering.h"
#include "ResultsChecker.h"
#include "data/DataReader.h"
#include "data/DataWriter.h"
#include "types/CollectionCreator.h"
#include "types/DataTypesTester.h"
#include "types/containers/array/ArrayContainer.h"

using cluster::parallel::DistanceComputers;
using cluster::parallel::ParallelClustering;
using cluster::test::ResultsChecker;
using cluster::test::data::DataReader;
using cluster::test::data::DataWriter;
using cluster::test::types::CollectionCreator;
using cluster::test::types::DataTypesTester;
using cluster::test::types::containers::array::ArrayContainer;
using cluster::utils::DataIteratorType;
using cluster::utils::DataIteratorUtils;
using cluster::utils::PiLambdaIteratorUtils;

const std::size_t ELEMENTS = 35717;
const std::size_t DIMENSION = 2;

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

    return 0;
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

    // Fill indirect arrays
    auto *const arraysContainer = new ArrayContainer<std::array, ELEMENTS, DIMENSION>(
            "std::array", DataIteratorType::CONTIGUOUS);
    CollectionCreator::createIndirectArrays<decltype(*arraysContainer), DIMENSION>(
            indirectParsedData, *arraysContainer);

    auto compilableDataStructures = wrapCompilableCollections(*arraysContainer);
    // Indirect std::array

    auto notCompilableDataStructures = wrapNotCompilableCollections(*arraysContainer);

    dataTypesTester.testParallelTypes(compilableDataStructures, notCompilableDataStructures);
}
