#ifndef FINAL_PROJECT_HPC_COLLECTIONCREATOR_H
#define FINAL_PROJECT_HPC_COLLECTIONCREATOR_H

#include "collections/AlignedArray.h"
#include "old/PointerWrapper.h"
#include "collections/ArrayWrapper.h"
#include "collections/CollectionContainer.h"
#include "collections/LinearCollectionContainer.h"
#include <array>
#include <deque>
#include <list>
#include <vector>

namespace cluster::test::types::collections {

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
            container.onlyIterable.push_back(value);
            container.onlyConstIterable.push_back(value);
            container.integers.push_back(static_cast<int>(value));
            container.pairsArray[i] = {static_cast<int>(i) + 0.5, static_cast<int>(i) - 0.5};
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
        container.sseAlignedArrayIterator = container.sseAlignedArray.begin();
        container.sseAlignedArrayConstIterator = container.sseAlignedArray.cbegin();
        container.avxAlignedArrayIterator = container.avxAlignedArray.begin();
        container.avxAlignedArrayConstIterator = container.avxAlignedArray.cbegin();
        container.onlyIterableIterator = container.onlyIterable.begin();
        container.onlyConstIterableIterator = container.onlyConstIterable.cbegin();

        // Initialize fake types
        container.integersIterator = container.integers.begin();
        container.integersConstIterator = container.integers.cbegin();
        container.path = "/a/b";
        container.pathConstIterator = container.path.begin();
        container.pathsVector.push_back(container.pathConstIterator);
        container.pathsVector.push_back(container.path.begin());
        container.pathsVector.template emplace_back(container.path.root_path().begin());
        container.pathsVectorIterator = container.pathsVector.begin();
        container.pathsVectorConstIterator = container.pathsVector.cbegin();
        container.pair = {3.5, 97};
        container.pairsArrayIterator = container.pairsArray.begin();
        container.pairsArrayConstIterator = container.pairsArray.cbegin();
        container.string = "Fake type";
        container.stringIterator = container.string.begin();
        container.stringConstIterator = container.string.cbegin();
    }

    /*template <std::size_t N, std::size_t D>
    static void createIndirectCArrays(const std::vector<std::array<double, D>> &indirectParsedData,
                                      PointerWrapper<N, D> &container) {

        initCArrayContainers<N, D>(indirectParsedData, container);
    }

    template <std::size_t N, std::size_t D>
    static void createIndirectArrays(const std::vector<std::array<double, D>> &indirectParsedData,
                                     ArrayCollectionContainer<std::array, N, D> &container) {

        initArrayContainers<N, D>(indirectParsedData, container);
    }*/

    template <std::size_t D>
    static void createIndirectCArrays(const std::vector<std::array<double, D>> &indirectParsedData,
                                      CollectionContainer<ArrayWrapper, D> &container) {

        initContainers<D, ArrayWrapper>(indirectParsedData, container);
    }

    template <std::size_t D>
    static void createIndirectArrays(const std::vector<std::array<double, D>> &indirectParsedData,
                                     CollectionContainer<ArrayWrapper, D> &container) {

        initContainers<D, ArrayWrapper>(indirectParsedData, container);
    }

    template <std::size_t D>
    static void createIndirectVectors(const std::vector<std::array<double, D>> &indirectParsedData,
                                      CollectionContainer<std::vector, D> &container) {

        initContainers<D, std::vector>(indirectParsedData, container);
    }

    template <std::size_t D>
    static void createIndirectLists(const std::vector<std::array<double, D>> &indirectParsedData,
                                    CollectionContainer<std::list, D> &container) {

        initContainers<D, std::list>(indirectParsedData, container);
    }

    template <std::size_t D>
    static void createIndirectDeques(const std::vector<std::array<double, D>> &indirectParsedData,
                                     CollectionContainer<std::deque, D> &container) {

        initContainers<D, std::deque>(indirectParsedData, container);
    }

    /*

     template <std::size_t D>
     static void createIndirectOnlyIterables(
             const std::vector<std::array<double, D>> &indirectParsedData,
             CollectionContainer<OnlyIterableVector, D> &container) {

         for (std::size_t i = 0; i < indirectParsedData.size(); i++) {
             const std::array<double, D> &point = indirectParsedData[i];

             addPointToContainer<D>(point, i, container);
         }

         for (auto &addedArrayElement : container.array) {
             container.constIteratorsArray.emplace_back(addedArrayElement.cbegin());
         }

         for (auto &addedVectorElement : container.vector) {
             container.constIteratorsVector.emplace_back(addedVectorElement.cbegin());
         }

         for (auto &addedListElement : container.list) {
             container.constIteratorsList.emplace_back(addedListElement.cbegin());
         }

         for (auto &addedDequeElement : container.deque) {
             container.constIteratorsDeque.emplace_back(addedDequeElement.cbegin());
         }

         for (auto &addedOnlyIterable : container.onlyIterable) {
             container.iteratorsOnlyIterable.emplace_back(addedOnlyIterable.begin());
         }

         for (auto &addedOnlyConstIterable : container.onlyConstIterable) {
             container.iteratorsOnlyConstIterable.emplace_back(addedOnlyConstIterable.cbegin());
         }

         for (auto &addedIntegers : container.integers) {
             container.integersConstIterator.emplace_back(addedIntegers.cbegin());
         }

         for (auto &addedPath : container.path) {
             auto &addedPathIterator = container.pathConstIterator.emplace_back(addedPath.begin());
             std::vector<std::filesystem::path::const_iterator> iterators{};
             iterators.template emplace_back(addedPath.begin());
             iterators.template emplace_back(addedPathIterator);
             container.pathsVector.emplace_back(iterators);
         }

         for (auto &addedString : container.string) {
             container.stringConstIterator.emplace_back(addedString.cbegin());
         }

         // Set the iterators
         container.cArrayConstIterator = container.cArray.begin();
         container.arraysConstIterator = container.array.begin();
         container.vectorsConstIterator = container.vector.begin();
         container.listsConstIterator = container.list.begin();
         container.dequesConstIterator = container.deque.begin();

         container.constIteratorIntegers = container.integers.begin();
     }

     template <std::size_t D>
     static void createIndirectOnlyConstIterables(
             const std::vector<std::array<double, D>> &indirectParsedData,
             CollectionContainer<OnlyConstIterableVector, D> &container) {

         for (std::size_t i = 0; i < indirectParsedData.size(); i++) {
             const std::array<double, D> &point = indirectParsedData[i];

             addPointToContainer<D>(point, i, container);
         }

         {
             auto iterator = container.array.cbegin();
             while (iterator != container.array.cend()) {
                 auto &addedArrayElement = *iterator;
                 container.constIteratorsArray.emplace_back(addedArrayElement.cbegin());
                 ++iterator;
             }
         }
         {
             auto iterator = container.vector.cbegin();
             while (iterator != container.vector.cend()) {
                 auto &addedVectorElement = *iterator;
                 container.constIteratorsVector.emplace_back(addedVectorElement.cbegin());
                 ++iterator;
             }
         }
         {
             auto iterator = container.list.cbegin();
             while (iterator != container.list.cend()) {
                 auto &addedListElement = *iterator;
                 container.constIteratorsList.emplace_back(addedListElement.cbegin());
                 ++iterator;
             }
         }
         {
             auto iterator = container.deque.cbegin();
             while (iterator != container.deque.cend()) {
                 auto &addedDequeElement = *iterator;
                 container.constIteratorsDeque.emplace_back(addedDequeElement.cbegin());
                 ++iterator;
             }
         }

         {
             auto iterator = container.onlyConstIterable.cbegin();
             while (iterator != container.onlyConstIterable.cend()) {
                 auto &addedOnlyConstIterable = *iterator;
                 container.iteratorsOnlyConstIterable.emplace_back(addedOnlyConstIterable.cbegin());
                 ++iterator;
             }
         }

         {
             auto iterator = container.integers.cbegin();
             while (iterator != container.integers.cend()) {
                 auto &addedIntegers = *iterator;
                 container.integersConstIterator.emplace_back(addedIntegers.cbegin());
                 ++iterator;
             }
         }

         {
             auto iterator = container.path.cbegin();
             while (iterator != container.path.cend()) {
                 auto &addedPath = *iterator;
                 auto &addedPathIterator =
                         container.pathConstIterator.emplace_back(addedPath.begin());
                 std::vector<std::filesystem::path::const_iterator> iterators{};
                 iterators.template emplace_back(addedPath.begin());
                 iterators.template emplace_back(addedPathIterator);
                 container.pathsVector.emplace_back(iterators);
                 ++iterator;
             }
         }

         {
             auto iterator = container.string.cbegin();
             while (iterator != container.string.cend()) {
                 auto &addedString = *iterator;
                 container.stringConstIterator.emplace_back(addedString.cbegin());
                 ++iterator;
             }
         }

         // Set the iterators
         container.cArrayConstIterator = container.cArray.cbegin();
         container.arraysConstIterator = container.array.cbegin();
         container.vectorsConstIterator = container.vector.cbegin();
         container.listsConstIterator = container.list.cbegin();
         container.dequesConstIterator = container.deque.cbegin();

         container.constIteratorIntegers = container.integers.cbegin();
     }
 */

private:
    /*template <std::size_t N, std::size_t D>
    static void initCArrayContainers(const std::vector<std::array<double, D>> &indirectParsedData,
                                     PointerWrapper<N, D> &container) {

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
    }*/

    template <std::size_t D, template <typename> typename C>
    static void initContainers(const std::vector<std::array<double, D>> &indirectParsedData,
                               CollectionContainer<C, D> &container) {

        for (std::size_t i = 0; i < indirectParsedData.size(); i++) {
            const std::array<double, D> &point = indirectParsedData[i];

            addPointToContainer<D>(point, i, container);
        }

        for (auto &addedPath : container.paths.normal) {
            std::vector<std::filesystem::path::const_iterator> pathsConstIterators{};
            pathsConstIterators.push_back(addedPath.begin());

            container.pathsVector.template emplaceElement(pathsConstIterators);
        }

        // Set the iterators
        container.cArrays.initIterators();
        container.constCArrays.initIterators();

        container.arrays.initIterators();
        container.vectors.initIterators();
        container.lists.initIterators();
        container.deques.initIterators();
        container.sseAlignedArrays.initIterators();
        container.avxAlignedArrays.initIterators();

        container.onlyIterables.initIterators();
        container.onlyConstIterables.initIterators();

        container.integers.initIterators();
        //container.paths.initIterators();
        container.pathsVector.initIterators();
        container.pairs.initIterators();
        container.strings.initIterators();
    }
    
    /*
    template <typename C, typename CI, typename CCI>
    static void setSubContainerIterators(
            C &subContainer, CI &subContainerIterator, CCI &subContainerConstIterator) {

        for (auto &addedElement : subContainer) {
            subContainerIterator.emplace_back(addedElement.begin());
            subContainerConstIterator.emplace_back(addedElement.cbegin());
        }
    }

    

template <std::size_t N, std::size_t D>
static void addPointToPointerContainer(const std::array<double, D> &point,
                                      const std::size_t index,
                                      PointerWrapper<N, D> &container) {

   double *cArrayPoint = new double[D];
   std::array<double, D> arrayPoint{};
   std::vector<double> vectorPoint{};
   std::list<double> listPoint{};
   std::deque<double> dequePoint{};
   AlignedArray<double, D, Alignments::SSE_ALIGNMENT> sseAlignedArray{};
   AlignedArray<double, D, Alignments::AVX_ALIGNMENT> avxAlignedArray{};
   OnlyIterableVector<double> onlyIterableVector{};
   OnlyConstIterableVector<double> constIterableVector{};
   std::vector<int> integerVector{};

   for (std::size_t j = 0; j < D; j++) {
       cArrayPoint[j] = point[j];
       arrayPoint[j] = point[j];
       vectorPoint.emplace_back(point[j]);
       listPoint.emplace_back(point[j]);
       dequePoint.emplace_back(point[j]);
       sseAlignedArray[j] = point[j];
       avxAlignedArray[j] = point[j];
       onlyIterableVector.push_back(point[j]);
       constIterableVector.push_back(point[j]);
       integerVector.template emplace_back(static_cast<int>(j));
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

   container.onlyIterable[index] = onlyIterableVector;
   container.iteratorsOnlyIterable[index] = container.onlyIterable[index].begin();
   container.onlyConstIterable[index] = constIterableVector;
   container.iteratorsOnlyConstIterable[index] = container.onlyConstIterable[index].cbegin();

   container.integers[index] = integerVector;
   container.integersIterator[index] = container.integers[index].begin();
   container.integersConstIterator[index] = container.integers[index].cbegin();

   container.path[index] = "/b/c/d";
   container.pathConstIterator[index] = container.path[index].begin();
   container.pathsVector[index].push_back(container.path[index].begin());
   container.pathsVector[index].push_back(container.pathConstIterator[index]);

   container.pairsArray[index] = {static_cast<int>(index) + 1.5,
                                  static_cast<int>(index) - 8.9};

   container.string[index] = "Fake type again";
   container.stringIterator[index] = container.string[index].begin();
   container.stringConstIterator[index] = container.string[index].cbegin();
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
   OnlyIterableVector<double> onlyIterableVector{};
   OnlyConstIterableVector<double> constIterableVector{};
   std::vector<int> integerVector{};

   for (std::size_t j = 0; j < D; j++) {
       cArrayPoint[j] = point[j];
       arrayPoint[j] = point[j];
       vectorPoint.emplace_back(point[j]);
       listPoint.emplace_back(point[j]);
       dequePoint.emplace_back(point[j]);
       sseAlignedArray[j] = point[j];
       avxAlignedArray[j] = point[j];
       onlyIterableVector.push_back(point[j]);
       constIterableVector.push_back(point[j]);
       integerVector.template emplace_back(static_cast<int>(j));
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
   container.iteratorsSseAlignedArray[index] = container.sseAlignedArray[index].begin();
   container.constIteratorsSseAlignedArray[index] = container.sseAlignedArray[index].cbegin();

   container.avxAlignedArray[index] = avxAlignedArray;
   container.iteratorsAvxAlignedArray[index] = container.avxAlignedArray[index].begin();
   container.constIteratorsAvxAlignedArray[index] = container.avxAlignedArray[index].cbegin();

   container.onlyIterable[index] = onlyIterableVector;
   container.iteratorsOnlyIterable[index] = container.onlyIterable[index].begin();
   container.onlyConstIterable[index] = constIterableVector;
   container.iteratorsOnlyConstIterable[index] = container.onlyConstIterable[index].cbegin();

   container.integers[index] = integerVector;
   container.integersIterator[index] = container.integers[index].begin();
   container.integersConstIterator[index] = container.integers[index].cbegin();

   container.path[index] = "/e/f/g/h/a.txt";
   container.pathConstIterator[index] = container.path[index].begin();
   container.pathsVector[index].push_back(container.path[index].begin());
   container.pathsVector[index].push_back(container.pathConstIterator[index]);

   container.pairsArray[index] = {static_cast<int>(index) + 14.5,
                                  static_cast<int>(index) - 88.09};

   container.string[index] = "Fake type again 2";
   container.stringIterator[index] = container.string[index].begin();
   container.stringConstIterator[index] = container.string[index].cbegin();




   C<OnlyIterableVector<double>, N> onlyIterable;
   C<OnlyIterableVector<double>::iterator, N> iteratorsOnlyIterable;
   C<OnlyConstIterableVector<double>, N> onlyConstIterable;
   C<OnlyConstIterableVector<double>::const_iterator, N> iteratorsOnlyConstIterable;

   C<std::vector<int>, N> integers;

   C<std::vector<int>::iterator, N> integersIterator;
   C<std::vector<int>::const_iterator, N> integersConstIterator;
   C<std::filesystem::path, N> path;
   C<std::filesystem::path::const_iterator, N> pathConstIterator;
   C<std::vector<std::filesystem::path::const_iterator>, N> pathsVector;
   C<std::pair<double, double>, N> pairsArray;

   C<std::string, N> string;
   C<std::string::iterator, N> stringIterator;
   C<std::string::const_iterator, N> stringConstIterator;



}*/

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
        OnlyIterableVector<double> onlyIterableVector{};
        OnlyConstIterableVector<double> constIterableVector{};
        std::vector<int> integerVector{};

        for (std::size_t j = 0; j < D; j++) {
            cArrayPoint[j] = point[j];
            arrayPoint[j] = point[j];
            vectorPoint.emplace_back(point[j]);
            listPoint.emplace_back(point[j]);
            dequePoint.emplace_back(point[j]);
            sseAlignedArray[j] = point[j];
            avxAlignedArray[j] = point[j];
            onlyIterableVector.emplace_back(point[j]);
            constIterableVector.emplace_back(point[j]);
            integerVector.template emplace_back(static_cast<int>(j));
        }

        container.cArrays.emplaceElement(cArrayPoint);
        container.constCArrays.emplaceElement(cArrayPoint);

        container.arrays.emplaceElement(arrayPoint);
        container.vectors.emplaceElement(vectorPoint);
        container.lists.emplaceElement(listPoint);
        container.deques.emplaceElement(dequePoint);

        container.sseAlignedArrays.emplaceElement(sseAlignedArray);
        container.avxAlignedArrays.emplaceElement(avxAlignedArray);

        container.onlyIterables.emplaceElement(onlyIterableVector);
        container.onlyConstIterables.emplaceElement(constIterableVector);

        container.integers.emplaceElement(integerVector);
        container.paths.emplaceElement("/e/f/g/h/a.txt");
        container.pairs.emplaceElement(
                std::pair{static_cast<int>(index) + 4.5, static_cast<int>(index) - 8.09});
        container.strings.emplaceElement(std::string{"Fake type again"});
    }
};

}  // namespace cluster::test::types::collections

#endif  // FINAL_PROJECT_HPC_COLLECTIONCREATOR_H
