#ifndef FINAL_PROJECT_HPC_COLLECTIONCREATOR_H
#define FINAL_PROJECT_HPC_COLLECTIONCREATOR_H

#include "AlignedArray.h"
#include <array>
#include <deque>
#include <list>
#include <vector>
#include "CollectionContainer.h"
#include "PointerWrapper.h"
#include "LinearCollectionContainer.h"

namespace cluster::test::types {

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-08
 * @since version date
 */
class CollectionCreator {

private:
    static const constexpr std::size_t SSE_ALIGNMENT = 16;
    static const constexpr std::size_t AVX_ALIGNMENT = 32;

public:
    template <std::size_t ND>
    static void createLinearContainers(std::vector<double> parsedData,
                                       LinearCollectionContainer<ND> &container) {

        // Fill the linear containers
        for (std::size_t i = 0; i < parsedData.size(); i++) {
            double value = parsedData[i];

            container.cArray[i] = value;
            container.array[i] = value;
            container.vector.push_back(value);
            container.list.push_back(value);
            container.deque.push_back(value);
            container.sseAlignedArray[i] = value;
            container.avxAlignedArray[i] = value;
        }

        // Initialize the iterators
        container.arrayIterator = container.array.begin();
        container.arrayConstIterator = container.array.cbegin();
        container.vectorIterator = container.vector.begin();
        container.vectorConstIterator = container.vector.cbegin();
        container.listIterator = container.list.begin();
        container.listConstIterator = container.list.cbegin();
        container.dequeIterator = container.deque.begin();
        container.dequeConstIterator = container.deque.cbegin();
    }

    template <std::size_t N, std::size_t D>
    static void createIndirectCArrays(std::vector<std::array<double, N>> indirectParsedData,
                                      ArrayCollectionContainer<PointerWrapper, N, D> &container) {

        initArrayContainers(indirectParsedData, container, "", " *");
    }

    template <std::size_t N, std::size_t D>
    static void createIndirectArrays(std::vector<std::array<double, N>> indirectParsedData,
                                     ArrayCollectionContainer<std::array, N, D> &container) {

        initArrayContainers(indirectParsedData, container, "std::array<", ", N>");
    }

    template <std::size_t N>
    static void createIndirectVectors(std::vector<std::array<double, N>> indirectParsedData,
                                      CollectionContainer<std::vector, N> &container) {

        return initContainers<N, std::vector>(indirectParsedData, container, "std::vector<", ">");
    }

    template <std::size_t N>
    static void createIndirectLists(std::vector<std::array<double, N>> indirectParsedData,
                                    CollectionContainer<std::list, N> &container) {

        return initContainers<N, std::list>(indirectParsedData, container, "std::list<", ">");
    }

    template <std::size_t N>
    static void createIndirectDeques(std::vector<std::array<double, N>> indirectParsedData,
                                     CollectionContainer<std::deque, N> &container) {

        return initContainers<N, std::deque>(indirectParsedData, container, "std::deque<", ">");
    }

private:
    template <std::size_t N, template <typename> typename C>
    static void initContainers(std::vector<std::array<double, N>> indirectParsedData,
                               CollectionContainer<C, N> &container,
                               const std::string &nameBegin,
                               const std::string &nameEnd) {

        for (std::size_t i = 0; i < indirectParsedData.size(); i++) {
            const std::array<double, N> &point = indirectParsedData[i];

            addPoint<N, 0, C>(point, i, container);
        }
        initIterators<N>(container, nameBegin, nameEnd);
    }

    template <std::size_t N, std::size_t D, template <typename, std::size_t> typename C>
    static void initArrayContainers(std::vector<std::array<double, N>> indirectParsedData,
                                    ArrayCollectionContainer<C, D, N> &container,
                                    const std::string &nameBegin,
                                    const std::string &nameEnd) {

        for (std::size_t i = 0; i < indirectParsedData.size(); i++) {
            const std::array<double, N> &point = indirectParsedData[i];

            addPoint<N, D, C>(point, container);
        }
        initIterators<N>(container, nameBegin, nameEnd);
    }

    template <std::size_t N, typename C>
    static void initIterators(C &container) {
        // Set the iterators
        container.cArrayIterator = container.cArray.begin();
        container.arraysIterator = container.array.begin();
        container.vectorsIterator = container.vector.begin();
        container.listsIterator = container.list.begin();
        container.dequesIterator = container.deque.begin();

        container.cArrayConstIterator = container.cArray.cbegin();
        container.arraysConstIterator = container.array.cbegin();
        container.vectorsConstIterator = container.vectorcbegin();
        container.listsConstIterator = container.list.cbegin();
        container.dequesConstIterator = container.deque.cbegin();
    }

    template <std::size_t N, std::size_t D, template <typename> typename C>
    static void addPoint(const std::array<double, N> &point,
                         const std::size_t index,
                         CollectionContainer<C, N> &container) {

        double *cArrayPoint = new double[N];
        std::array<double, N> arrayPoint{};
        std::vector<double> vectorPoint{};
        std::list<double> listPoint{};
        std::deque<double> dequePoint{};
        AlignedArray<double, N, SSE_ALIGNMENT> sseAlignedArray{};
        AlignedArray<double, N, AVX_ALIGNMENT> avxAlignedArray{};

        for (std::size_t j = 0; j < N; j++) {
            cArrayPoint[j] = point[j];
            arrayPoint[j] = point[j];
            vectorPoint.emplace_back(point[j]);
            listPoint.emplace_back(point[j]);
            dequePoint.emplace_back(point[j]);
            sseAlignedArray[j] = point[j];
            avxAlignedArray[j] = point[j];
        }

        container.cArray.emplace_back(cArrayPoint);

        const auto &addedArrayElement = container.array.emplace_back(arrayPoint);
        container.iteratorsArray.emplace_back(addedArrayElement.begin());
        container.constIteratorsArray.emplace_back(addedArrayElement.cbegin());

        const auto &addedVectorElement = container.vector.emplace_back(vectorPoint);
        container.iteratorsVector.emplace_back(addedVectorElement.begin());
        container.constIteratorsVector.emplace_back(addedVectorElement.cbegin());

        const auto &addedListElement = container.list.emplace_back(listPoint);
        container.iteratorsList.emplace_back(addedListElement.begin());
        container.constIteratorsList.emplace_back(addedListElement.cbegin());

        const auto &addedDequeElement = container.deque.emplace_back(dequePoint);
        container.iteratorsDeque.emplace_back(addedDequeElement.begin());
        container.constIteratorsDeque.emplace_back(addedDequeElement.cbegin());

        container.sseAlignedArray.emplace_back(sseAlignedArray);
        container.avxAlignedArray.emplace_back(avxAlignedArray);
    }

    template <std::size_t N, std::size_t D, template <typename, std::size_t> typename C>
    static void addPoint(const std::array<double, N> &point,
                         const std::size_t index,
                         ArrayCollectionContainer<C, N, D> &container) {

        double *cArrayPoint = new double[N];
        std::array<double, N> arrayPoint{};
        std::vector<double> vectorPoint{};
        std::list<double> listPoint{};
        std::deque<double> dequePoint{};
        AlignedArray<double, N, SSE_ALIGNMENT> sseAlignedArray{};
        AlignedArray<double, N, AVX_ALIGNMENT> avxAlignedArray{};

        for (std::size_t j = 0; j < N; j++) {
            cArrayPoint[j] = point[j];
            arrayPoint[j] = point[j];
            vectorPoint.emplace_back(point[j]);
            listPoint.emplace_back(point[j]);
            dequePoint.emplace_back(point[j]);
            sseAlignedArray[j] = point[j];
            avxAlignedArray[j] = point[j];
        }

        container.cArray[index] = cArrayPoint;

        container.array[index] = arrayPoint;
        container.iteratorsArray[index] = container.array[index].begin();
        container.constIteratorsArray[index] = container.array[index].cbegin();

        container.vector[index] = vectorPoint;
        container.iteratorsVector[index] = container.vector[index].begin();
        container.constIteratorsVector[index] = container.vector[index].cbegin();

        container.list[index] = listPoint;
        container.iteratorsList[index] = container.list[index].begin();
        container.constIteratorsList[index] = container.list[index].cbegin();

        container.deque[index] = dequePoint;
        container.iteratorsDeque[index] = container.deque[index].begin();
        container.constIteratorsDeque[index] = container.deque[index].cbegin();

        container.sseAlignedArray[index] = sseAlignedArray;
        container.avxAlignedArray[index] = avxAlignedArray;
    }
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_COLLECTIONCREATOR_H
