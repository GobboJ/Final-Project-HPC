/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-07
 * @since version date
 */
#include "src/data/DataReader.h"
#include "ParallelClustering.h"
#include "SequentialClustering.h"
#include <deque>
#include <list>

// #define COMPILE_WRONG_SOLUTIONS
// #define COMPILE_SOLUTIONS

using cluster::parallel::ParallelClustering;
using DistanceComputers = cluster::parallel::ParallelClustering::DistanceComputers;
using cluster::sequential::SequentialClustering;
using cluster::test::data::DataReader;

const std::size_t ELEMENTS = 1000;
const std::size_t COORDINATES = 2;

#ifdef COMPILE_SOLUTIONS
template <typename D>
concept SolutionIterator =
        requires(const D data, std::vector<std::size_t> pi, std::vector<double> lambda) {
            {
                ParallelClustering::cluster<DistanceComputers::CLASSICAL>(
                        data, ELEMENTS, COORDINATES, pi.begin(), lambda.begin())
                } -> std::convertible_to<void>;
        };

#else
template <typename D>
concept SolutionIterator = requires(int a) { a = 1; };
#endif

template <SolutionIterator D>
void performParallelTest(const D data) {

#ifdef COMPILE_SOLUTIONS
    std::vector<std::size_t> pi{};
    std::vector<double> lambda{};

    ParallelClustering::cluster<DistanceComputers::CLASSICAL>(
            data, ELEMENTS, COORDINATES, pi.begin(), lambda.begin());

    // TODO: Check results
#endif;
}

template <typename D>
void performNotCompilableParallelTest(const D data) {

#ifdef COMPILE_WRONG_SOLUTIONS
    std::vector<std::size_t> pi{};
    std::vector<double> lambda{};

    ParallelClustering::cluster<DistanceComputers::CLASSICAL>(
            data, ELEMENTS, COORDINATES, pi.begin(), lambda.begin());
#endif
}

int main() {

    double *linearCArray = new double[ELEMENTS * COORDINATES];
    std::array<double, ELEMENTS> linearArray{};
    std::vector<double> linearVector{};
    std::list<double> linearList{};
    std::deque<double> linearDeque{};

    double **indirectCArray = new double *[ELEMENTS];
    std::array<double *, ELEMENTS> indirectArrayCArray{};
    std::vector<double *> indirectVectorCArray{};
    std::list<double *> indirectListCArray{};
    std::deque<double *> indirectDequeCArray{};

    std::array<double, COORDINATES> *indirectArray = new std::array<double, COORDINATES>[ELEMENTS];
    std::array<std::array<double, COORDINATES>, ELEMENTS> indirectArrayArray{};
    std::array<std::vector<double>, ELEMENTS> indirectArrayVector{};
    std::array<std::list<double>, ELEMENTS> indirectArrayList{};
    std::array<std::deque<double>, ELEMENTS> indirectArrayDeque{};

    std::vector<double> *indirectCArrayVector = new std::vector<double>[ELEMENTS];
    std::vector<std::array<double, COORDINATES>> indirectVectorArray;
    std::vector<std::vector<double>> indirectVectorVector;
    std::vector<std::list<double>> indirectVectorList;
    std::vector<std::deque<double>> indirectVectorDeque;

    std::list<double> *indirectCArrayList = new std::list<double>[ELEMENTS];
    std::list<std::array<double, COORDINATES>> indirectListArray{};
    std::list<std::vector<double>> indirectListVector{};
    std::list<std::list<double>> indirectListList{};
    std::list<std::deque<double>> indirectListDeque{};

    std::deque<double> *indirectCArrayDeque = new std::deque<double>[ELEMENTS];
    std::deque<std::array<double, COORDINATES>> indirectDequeArray{};
    std::deque<std::vector<double>> indirectDequeVector{};
    std::deque<std::list<double>> indirectDequeList{};
    std::deque<std::deque<double>> indirectDequeDeque{};

    // Read the data
    std::vector<double> parsedData;
    DataReader::readAndParseData(
            "../../test/resources/Parking Birmingham.data", parsedData, 0, 0, 1, 2);

    // Organized the data in indirect vectors
    std::vector<std::array<double, COORDINATES>> indirectParsedData;
    for (std::size_t i = 0; i < parsedData.size(); i += COORDINATES) {
        std::array<double, COORDINATES> point{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            point[j] = parsedData[i + j];
        }
        indirectParsedData.emplace_back(point);
    }

    // Fill the linear containers
    for (std::size_t i = 0; i < parsedData.size(); i++) {
        double value = parsedData[i];
        linearCArray[i] = value;
        linearArray[i] = value;
        linearVector.push_back(value);
        linearList.push_back(value);
        linearDeque.push_back(value);
    }

    // Fill indirect C arrays
    for (std::size_t i = 0; i < indirectParsedData.size(); i++) {
        std::array<double, COORDINATES> value = indirectParsedData[i];

        double *cArray = new double[COORDINATES];
        memcpy(cArray, value.data(), COORDINATES);

        indirectCArray[i] = cArray;
        indirectArrayCArray[i] = cArray;
        indirectVectorCArray.push_back(cArray);
        indirectListCArray.push_back(cArray);
        indirectDequeCArray.push_back(cArray);
    }

    // Fill indirect arrays
    for (std::size_t i = 0; i < indirectParsedData.size(); i++) {

        std::array<double, COORDINATES> value = indirectParsedData[i];

        indirectArray[i] = value;
        indirectArrayArray[i] = value;

        std::vector<double> vector{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            vector.push_back(value[j]);
        }
        indirectArrayVector[i] = vector;

        std::list<double> list{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            list.push_back(value[j]);
        }
        indirectArrayList[i] = list;

        std::deque<double> deque{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            deque.push_back(value[j]);
        }
        indirectArrayDeque[i] = deque;
    }

    // Fill indirect vectors
    for (std::size_t i = 0; i < indirectParsedData.size(); i++) {

        std::array<double, COORDINATES> value = indirectParsedData[i];

        std::vector<double> vector{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            vector.push_back(value[j]);
        }

        indirectCArrayVector[i] = vector;
        indirectVectorArray.emplace_back(value);
        indirectVectorVector.emplace_back(vector);

        std::list<double> list{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            list.push_back(value[j]);
        }
        indirectVectorList.emplace_back(list);

        std::deque<double> deque{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            deque.push_back(value[j]);
        }
        indirectVectorDeque.emplace_back(deque);
    }

    // Fill indirect lists
    for (std::size_t i = 0; i < indirectParsedData.size(); i++) {

        std::array<double, COORDINATES> value = indirectParsedData[i];

        std::list<double> list{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            list.push_back(value[j]);
        }

        indirectCArrayList[i] = list;
        indirectListArray.emplace_back(value);

        std::vector<double> vector{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            vector.push_back(value[j]);
        }
        indirectListVector.emplace_back(vector);

        indirectListList.emplace_back(list);

        std::deque<double> deque{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            deque.push_back(value[j]);
        }
        indirectListDeque.emplace_back(deque);
    }

    // Fill indirect deques
    for (std::size_t i = 0; i < indirectParsedData.size(); i++) {

        std::array<double, COORDINATES> value = indirectParsedData[i];

        std::deque<double> deque{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            deque.push_back(value[j]);
        }

        indirectCArrayDeque[i] = deque;
        indirectDequeArray.emplace_back(value);

        std::vector<double> vector{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            vector.push_back(value[j]);
        }
        indirectDequeVector.emplace_back(vector);

        std::list<double> list{};
        for (std::size_t j = 0; j < COORDINATES; j++) {
            list.push_back(value[j]);
        }
        indirectDequeList.emplace_back(list);

        indirectDequeDeque.emplace_back(deque);
    }

    // Perform linear tests
    performParallelTest(linearCArray);
    performParallelTest(linearArray.begin());
    performParallelTest(linearVector.begin());
    performNotCompilableParallelTest(linearList.begin());
    performNotCompilableParallelTest(linearDeque.begin());

    // Perform indirect tests
    performParallelTest(indirectCArray);
    performParallelTest(indirectArray);
    performParallelTest(indirectCArrayVector);
    performNotCompilableParallelTest(indirectCArrayList);
    performNotCompilableParallelTest(indirectCArrayDeque);

    // Perform indirect arrays test
    performParallelTest(indirectArrayCArray.begin());
    performParallelTest(indirectArrayArray.begin());
    performParallelTest(indirectArrayVector.begin());
    performNotCompilableParallelTest(indirectArrayList.begin());
    performNotCompilableParallelTest(indirectArrayDeque.begin());

    // Perform indirect vectors tests
    performParallelTest(indirectVectorCArray.begin());
    performParallelTest(indirectVectorArray.begin());
    performParallelTest(indirectVectorVector.begin());
    performNotCompilableParallelTest(indirectVectorList);
    performNotCompilableParallelTest(indirectVectorDeque);

    // Perform indirect lists tests
    performNotCompilableParallelTest(indirectListCArray.begin());
    performNotCompilableParallelTest(indirectListArray.begin());
    performNotCompilableParallelTest(indirectListVector.begin());
    performNotCompilableParallelTest(indirectListList.begin());
    performNotCompilableParallelTest(indirectListDeque.begin());

    // Perform indirect deques tests
    performParallelTest(indirectDequeCArray.begin());
    performParallelTest(indirectDequeArray.begin());
    performParallelTest(indirectDequeVector.begin());
    performNotCompilableParallelTest(indirectDequeList.begin());
    performNotCompilableParallelTest(indirectDequeDeque.begin());

    return 0;
}
