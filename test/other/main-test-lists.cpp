/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-16
 * @since version date
 */
#include <forward_list>
#include "ParallelClustering.h"
#include "../src/data/ResultsChecker.h"
#include "../src/data/DataReader.h"
#include "../src/data/DataWriter.h"
#include "types/CollectionCreator.h"
#include "types/DataTypesTest.h"
#include "types/DataTypesTester.h"
#include "types/containers/array/ArrayContainer.h"
#include "SequentialClustering.h"

using cluster::sequential::SequentialClustering;
using cluster::test::ResultsChecker;
using cluster::test::data::DataReader;
using cluster::test::types::collections::OnlyIterableVector;
using cluster::utils::DataIteratorUtils;
using cluster::utils::PiLambdaIteratorUtils;

static const constexpr std::size_t ELEMENTS = 35717;
static const constexpr std::size_t DIMENSION = 2;

template <typename D>
void testDataType(const D &data,
                  const std::vector<std::size_t> &expectedPi,
                  const std::vector<double> &expectedLambda) {

    std::vector<std::size_t> pi{};
    pi.resize(ELEMENTS);
    std::vector<double> lambda{};
    lambda.resize(ELEMENTS);

    SequentialClustering::cluster(data, ELEMENTS, DIMENSION, pi, lambda);

    bool result = ResultsChecker::checkResults(pi.cbegin(),
                                               pi.cend(),
                                               lambda.cbegin(),
                                               lambda.cend(),
                                               expectedPi.cbegin(),
                                               expectedPi.cend(),
                                               expectedLambda.cbegin(),
                                               expectedLambda.cend());
    std::cout << ((result) ? "\033[32mOK" : "\033[31mError") << "\033[0m" << std::endl << std::endl;
}

int main() {

    std::vector<double> parsedData;
    DataReader::readAndParseData(
            "../../../test/resources/Parking Birmingham.data", parsedData, 0, 0, 2, 3);

    std::vector<std::size_t> expectedPi{};
    expectedPi.resize(parsedData.size() / DIMENSION);
    std::vector<double> expectedLambda{};
    expectedLambda.resize(parsedData.size() / DIMENSION);

    std::cout << "Running sequential implementation to check the results" << std::endl;
    DataReader::readPiLambda("../../../out/birm-p-11-results.txt", expectedPi, expectedLambda);
    std::cout << "End" << std::endl;

    DataIteratorUtils::printSummaries = true;
    PiLambdaIteratorUtils::printSummaries = false;

    std::list<double *> list{};
    std::list<std::vector<double>> vectorsList{};
    std::vector<std::array<double, DIMENSION>> arrays{};
    std::forward_list<std::array<double, DIMENSION>::const_iterator> constIteratorsForwardList{};
    std::forward_list<OnlyIterableVector<double>> onlyIterableForwardList{};

    // Populate the data structures
    for (std::size_t i = 0; i < parsedData.size(); i += DIMENSION) {
        double *cArrayPoint = new double[DIMENSION];
        std::vector<double> vectorPoint{};
        std::array<double, DIMENSION> arrayPoint{};
        

        for (std::size_t j = 0; j < DIMENSION; j++) {
            cArrayPoint[j] = parsedData[i + j];
            vectorPoint.push_back(parsedData[i + j]);
            arrayPoint[j] = parsedData[i + j];
        }
        // Add the elements
        list.emplace_back(cArrayPoint);
        vectorsList.emplace_back(vectorPoint);
        arrays.emplace_back(arrayPoint);
        
    }
    // Initialize the const iterators
    auto reverseIterator = arrays.crbegin();
    while (reverseIterator != arrays.crend()) {
        // Extract the point
        const std::array<double, DIMENSION> &array = *reverseIterator;
        // Fill the new point
        OnlyIterableVector<double> onlyIterablePoint{};
        for (std::size_t j = 0; j < DIMENSION; j++) {
            onlyIterablePoint.push_back(array[j]);
        }
        // Emplace the points
        onlyIterableForwardList.emplace_front(onlyIterablePoint);
        constIteratorsForwardList.emplace_front(array.cbegin());
        ++reverseIterator;
    }

    //testDataType(list, expectedPi, expectedLambda);
    //testDataType(vectorsList.cbegin(), expectedPi, expectedLambda);
    testDataType(constIteratorsForwardList, expectedPi, expectedLambda);
    testDataType(onlyIterableForwardList.cbegin(), expectedPi, expectedLambda);

    // Deallocate the allocated memory
    for (double *point : list) {
        delete[] point;
    }
}
