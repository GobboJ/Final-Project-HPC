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
#include "../src/data/ResultsChecker.h"
#include "../src/data/DataReader.h"
#include "../src/data/DataWriter.h"
#include "types/CollectionCreator.h"
#include "types/DataTypesTest.h"
#include "types/DataTypesTester.h"
#include "types/containers/array/ArrayContainer.h"

using cluster::parallel::DistanceComputers;
using cluster::parallel::ParallelClustering;
using cluster::test::ResultsChecker;
using cluster::test::data::DataReader;
using cluster::test::data::DataWriter;
using cluster::test::types::CollectionCreator;
using cluster::test::types::DataTypesTest;
using cluster::test::types::DataTypesTester;
using cluster::test::types::containers::array::ArrayContainer;
using cluster::utils::DataIteratorUtils;
using cluster::utils::DataIteratorUtils;
using cluster::utils::PiLambdaIteratorUtils;

int main() {

    std::vector<double> parsedData = DataTypesTest::readData();

    // Fill indirect arrays
    /*auto *const arraysContainer =
            new ArrayContainer<std::array, DataTypesTest::ELEMENTS, DataTypesTest::DIMENSION>(
                    "std::array", DataIteratorType::CONTIGUOUS);
    /*CollectionCreator::createIndirectArrays<decltype(*arraysContainer), DIMENSION>(
            readData, *arraysContainer);
    */
    auto compilableDataStructures =
            std::make_tuple();  // wrapCompilableCollections(*arraysContainer);
    // Indirect std::array

    auto notCompilableDataStructures =
            std::make_tuple();  // wrapNotCompilableCollections(*arraysContainer);

    // testPiLambdaTypes(parsedData, expectedPi, expectedLambda);
    DataTypesTest::performTest(parsedData, compilableDataStructures, notCompilableDataStructures);

    return 0;
}
