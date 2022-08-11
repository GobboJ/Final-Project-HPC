#ifndef FINAL_PROJECT_HPC_COLLECTIONCREATOR_H
#define FINAL_PROJECT_HPC_COLLECTIONCREATOR_H

#include "AlignedArray.h"
#include <array>
#include <deque>
#include <list>
#include <vector>
#include "CollectionContainer.h"
#include "PointerCollectionContainer.h"
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
    static void createIndirectCArrays(const std::vector<std::array<double, D>> &indirectParsedData,
                                      PointerCollectionContainer<N, D> &container) {

        initCArrayContainers<N, D>(indirectParsedData, container);
    }

    template <std::size_t N, std::size_t D>
    static void createIndirectArrays(const std::vector<std::array<double, D>> &indirectParsedData,
                                     ArrayCollectionContainer<std::array, N, D> &container) {

        initArrayContainers<N, D>(indirectParsedData, container);
    }

    template <std::size_t D>
    static void createIndirectVectors(const std::vector<std::array<double, D>> &indirectParsedData,
                                      CollectionContainer<std::vector, D> &container) {

        return initContainers<D, std::vector>(indirectParsedData, container);
    }

    template <std::size_t D>
    static void createIndirectLists(const std::vector<std::array<double, D>> &indirectParsedData,
                                    CollectionContainer<std::list, D> &container) {

        return initContainers<D, std::list>(indirectParsedData, container);
    }

    template <std::size_t D>
    static void createIndirectDeques(const std::vector<std::array<double, D>> &indirectParsedData,
                                     CollectionContainer<std::deque, D> &container) {

        return initContainers<D, std::deque>(indirectParsedData, container);
    }

private:
    template <std::size_t N, std::size_t D>
    static void initCArrayContainers(const std::vector<std::array<double, D>> &indirectParsedData,
                                     PointerCollectionContainer<N, D> &container) {

        for (std::size_t i = 0; i < indirectParsedData.size(); i++) {
            const std::array<double, D> &point = indirectParsedData[i];

            addPointToPointerContainer<N, D>(point, i, container);
        }
    }

    template <std::size_t N, std::size_t D, template <typename, std::size_t> typename C>
    static void initArrayContainers(const std::vector<std::array<double, D>> &indirectParsedData,
                                    ArrayCollectionContainer<C, N, D> &container) {

        for (std::size_t i = 0; i < indirectParsedData.size(); i++) {
            const std::array<double, D> &point = indirectParsedData[i];

            addPointToArrayContainer<N, D>(point, i, container);
        }
        initIterators(container);
    }

    template <std::size_t D, template <typename> typename C>
    static void initContainers(const std::vector<std::array<double, D>> &indirectParsedData,
                               CollectionContainer<C, D> &container) {

        for (std::size_t i = 0; i < indirectParsedData.size(); i++) {
            const std::array<double, D> &point = indirectParsedData[i];

            addPointToContainer<D>(point, i, container);
        }

        for (auto &addedArrayElement : container.array) {
            container.iteratorsArray.emplace_back(addedArrayElement.begin());
            container.constIteratorsArray.emplace_back(addedArrayElement.cbegin());
        }

        for (auto &addedVectorElement : container.vector) {
            container.iteratorsVector.emplace_back(addedVectorElement.begin());
            container.constIteratorsVector.emplace_back(addedVectorElement.cbegin());
        }

        for (auto &addedListElement : container.list) {
            container.iteratorsList.emplace_back(addedListElement.begin());
            container.constIteratorsList.emplace_back(addedListElement.cbegin());
        }

        for (auto &addedDequeElement : container.deque) {
            container.iteratorsDeque.emplace_back(addedDequeElement.begin());
            container.constIteratorsDeque.emplace_back(addedDequeElement.cbegin());
        }

        initIterators(container);
    }

    template <typename C>
    static void initIterators(C &container) {
        // Set the iterators
        container.cArrayIterator = container.cArray.begin();
        container.arraysIterator = container.array.begin();
        container.vectorsIterator = container.vector.begin();
        container.listsIterator = container.list.begin();
        container.dequesIterator = container.deque.begin();

        container.cArrayConstIterator = container.cArray.cbegin();
        container.arraysConstIterator = container.array.cbegin();
        container.vectorsConstIterator = container.vector.cbegin();
        container.listsConstIterator = container.list.cbegin();
        container.dequesConstIterator = container.deque.cbegin();
    }

    template <std::size_t N, std::size_t D>
    static void addPointToPointerContainer(const std::array<double, D> &point,
                                           const std::size_t index,
                                           PointerCollectionContainer<N, D> &container) {

        double *cArrayPoint = new double[D];
        std::array<double, D> arrayPoint{};
        std::vector<double> vectorPoint{};
        std::list<double> listPoint{};
        std::deque<double> dequePoint{};
        AlignedArray<double, D, Alignments::SSE_ALIGNMENT> sseAlignedArray{};
        AlignedArray<double, D, Alignments::AVX_ALIGNMENT> avxAlignedArray{};

        for (std::size_t j = 0; j < D; j++) {
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

    template <std::size_t N, std::size_t D, template <typename, std::size_t> typename C>
    static void addPointToArrayContainer(const std::array<double, D> &point,
                                         const std::size_t index,
                                         ArrayCollectionContainer<C, N, D> &container) {

        double *cArrayPoint = new double[D];
        std::array<double, D> arrayPoint{};
        std::vector<double> vectorPoint{};
        std::list<double> listPoint{};
        std::deque<double> dequePoint{};
        AlignedArray<double, D, Alignments::SSE_ALIGNMENT> sseAlignedArray{};
        AlignedArray<double, D, Alignments::AVX_ALIGNMENT> avxAlignedArray{};

        for (std::size_t j = 0; j < D; j++) {
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

    template <std::size_t D, template <typename> typename C>
    static void addPointToContainer(const std::array<double, D> &point,
                                    const std::size_t index,
                                    CollectionContainer<C, D> &container) {

        double *cArrayPoint = new double[D];
        std::array<double, D> arrayPoint{};
        std::vector<double> vectorPoint{};
        std::list<double> listPoint{};
        std::deque<double> dequePoint{};
        AlignedArray<double, D, Alignments::SSE_ALIGNMENT> sseAlignedArray{};
        AlignedArray<double, D, Alignments::AVX_ALIGNMENT> avxAlignedArray{};

        for (std::size_t j = 0; j < D; j++) {
            cArrayPoint[j] = point[j];
            arrayPoint[j] = point[j];
            vectorPoint.emplace_back(point[j]);
            listPoint.emplace_back(point[j]);
            dequePoint.emplace_back(point[j]);
            sseAlignedArray[j] = point[j];
            avxAlignedArray[j] = point[j];
        }

        container.cArray.emplace_back(cArrayPoint);

        container.array.emplace_back(arrayPoint);
        container.vector.emplace_back(vectorPoint);
        container.list.emplace_back(listPoint);
        container.deque.emplace_back(dequePoint);

        container.sseAlignedArray.emplace_back(sseAlignedArray);
        container.avxAlignedArray.emplace_back(avxAlignedArray);
    }
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_COLLECTIONCREATOR_H
