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

            container.cArray.second[i] = value;
            container.array.second[i] = value;
            container.vector.second.push_back(value);
            container.list.second.push_back(value);
            container.deque.second.push_back(value);
            container.sseAlignedArray.second[i] = value;
            container.avxAlignedArray.second[i] = value;
        }

        // Initialize the iterators
        container.arrayIterator.second = container.array.second.cbegin();
        container.vectorIterator.second = container.vector.second.cbegin();
        container.listIterator.second = container.list.second.cbegin();
        container.dequeIterator.second = container.deque.second.cbegin();

        // Set the names
        container.cArray.first = "double *";
        container.array.first = "std::array<double, ND>";
        container.arrayIterator.first = "std::array<double, ND>::const_iterator";
        container.vector.first = "std::vector<double>";
        container.vectorIterator.first = "std::vector<double>::const_iterator";
        container.list.first = "std::list<double>";
        container.listIterator.first = "std::list<double>::const_iterator";
        container.deque.first = "std::deque<double>";
        container.dequeIterator.first = "std::deque<double>::const_iterator";
        container.sseAlignedArray.first = "AlignedArray<double, ND, SSE_ALIGNMENT>";
        container.avxAlignedArray.first = "AlignedArray<double, ND, AVX_ALIGNMENT>";
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
    static void initIterators(
            C &container, const std::string &nameBegin, const std::string &nameEnd) {
        // Set the iterators
        container.cArrayIterator.second = container.cArray.second.cbegin();
        container.arrayIterator.second = container.array.second.cbegin();
        container.vectorIterator.second = container.vector.secondcbegin();
        container.listIterator.second = container.list.second.cbegin();
        container.dequeIterator.second = container.deque.second.cbegin();

        setNames(container, nameBegin, nameEnd);
    }

    template <std::size_t N, typename C>
    static void setNames(C &container, const std::string &nameBegin, const std::string &nameEnd) {
        // Set the names
        container.cArray.first = nameBegin + "double *" + nameEnd;
        container.cArrayIterator.first = nameBegin + "double *" + nameEnd + "::const_iterator";

        container.array.first = nameBegin + "std::array<double, N>";
        container.arrayIterator.first = nameBegin + "std::array<double, N>" + "::const_iterator";
        container.iteratorArray.first = nameBegin + "std::array<double, N>::iterator>" + nameEnd;

        container.vector.first = nameBegin + "std::vector<double>" + nameEnd;
        container.vectorIterator.first =
                nameBegin + "std::vector<double>" + nameEnd + "::const_iterator";
        container.iteratorVector.first = nameBegin + "std::vector<double>::iterator" + nameEnd;

        container.list.first = nameBegin + "std::list<double>" + nameEnd;
        container.listIterator.first =
                nameBegin + "std::list<double>" + nameEnd + "::const_iterator";
        container.iteratorList.first = nameBegin + "std::list<double>::iterator" + nameEnd;

        container.deque.first = nameBegin + "std::deque<double>" + nameEnd;
        container.dequeIterator.first =
                nameBegin + "std::deque<double>" + nameEnd + "::const_iterator";
        container.iteratorDeque.first =
                nameBegin + "double*>std::deque<double>::iterator" + nameEnd;

        container.sseAlignedArray.first =
                nameBegin + "AlignedArray<double, N, SSE_ALIGNMENT>" + nameEnd;
        container.avxAlignedArray.first =
                nameBegin + "AlignedArray<double, N, AVX_ALIGNMENT>" + nameEnd;
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

        container.cArray.second.emplace_back(cArrayPoint);

        const auto &addedArrayElement = container.array.second.emplace_back(arrayPoint);
        container.iteratorArray.second.emplace_back(addedArrayElement.cbegin());

        const auto &addedVectorElement = container.vector.second.emplace_back(vectorPoint);
        container.iteratorVector.second.emplace_back(addedVectorElement.cbegin());

        const auto &addedListElement = container.list.second.emplace_back(listPoint);
        container.iteratorList.second.emplace_back(addedListElement.cbegin());

        const auto &addedDequeElement = container.deque.second.emplace_back(dequePoint);
        container.iteratorDeque.second.emplace_back(addedDequeElement.cbegin());

        container.sseAlignedArray.second.emplace_back(sseAlignedArray);
        container.avxAlignedArray.second.emplace_back(avxAlignedArray);
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

        container.cArray.second[index] = cArrayPoint;

        container.array.second[index] = arrayPoint;
        container.iteratorArray.second[index] = container.array.second[index].cbegin();

        container.vector.second[index] = vectorPoint;
        container.iteratorVector.second[index] = container.vector.second[index].cbegin();

        container.list.second[index] = listPoint;
        container.iteratorList.second[index] = container.list.second[index].cbegin();

        container.deque.second[index] = dequePoint;
        container.iteratorDeque.second[index] = container.deque.second[index].cbegin();

        container.sseAlignedArray.second[index] = sseAlignedArray;
        container.avxAlignedArray.second[index] = avxAlignedArray;
    }
};

}  // namespace cluster::test::types

#endif  // FINAL_PROJECT_HPC_COLLECTIONCREATOR_H
