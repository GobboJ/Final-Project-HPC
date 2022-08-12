/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-31
 * @since version date
 */
#ifndef FINAL_PROJECT_HPC_TYPES_H
#define FINAL_PROJECT_HPC_TYPES_H

namespace cluster::utils {

template <typename I, typename T>
concept InputIterator = requires(I iterator) {
                            { *iterator } -> std::convertible_to<T>;
                        };

template <typename I, typename T>
concept OutputIterator = requires(I iterator, T value) { *iterator = value; };

template <typename I, typename T>
concept OutputIterable = requires(I iterator, T value) { *(iterator.begin()) = value; };

template <typename I, typename T>
concept Iterable = requires(I iterator) {
                       iterator.begin();
                       { *(iterator.begin()) } -> std::convertible_to<T>;
                   };

template <typename I, typename T>
concept ConstIterable = requires(I iterator) {
                            iterator.cbegin();
                            { *(iterator.cbegin()) } -> std::convertible_to<const T>;
                        };

template <typename I, typename T>
concept RandomIterator = std::random_access_iterator<I> && InputIterator<I, T>;

template <typename I, typename T>
concept RandomIterable =
        Iterable<std::remove_reference_t<I>, T> && requires(I iterator) {
                                                       {
                                                           iterator.begin()
                                                           } -> std::random_access_iterator<>;
                                                   };

template <typename I, typename T>
concept ContiguousIterator = std::contiguous_iterator<std::remove_cvref_t<I>> &&
                             InputIterator<std::remove_reference_t<I>, T>;

template <typename I, typename T>
concept ContiguousIterable =
        Iterable<std::remove_reference_t<I>, T> && requires(I iterator) {
                                                       {
                                                           iterator.begin()
                                                           } -> std::contiguous_iterator<>;
                                                   };

template <typename I, typename T>
concept ContiguousConstIterable =
        ConstIterable<std::remove_reference_t<I>, T> && requires(I iterator) {
                                                            {
                                                                iterator.cbegin()
                                                                } -> std::contiguous_iterator<>;
                                                        };

////////////////////////////////////////////////////////////////////////////////////////////////////
// Iterator of iterator double **
template <typename I, typename T>
concept ContiguousIteratorOfIterators =
        std::contiguous_iterator<I> && requires(I iterator) {
                                           { *iterator } -> ContiguousIterator<T>;
                                       };

template <typename I, typename T>
concept ContiguousIteratorOfIterables =
        std::contiguous_iterator<I> && requires(I iterator) {
                                           { *iterator } -> ContiguousIterable<T>;
                                       };

template <typename I, typename T>
concept ContiguousIteratorOfConstIterables =
        std::contiguous_iterator<I> && requires(I iterator) {
                                           { *iterator } -> ContiguousConstIterable<T>;
                                       };

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename I, typename T>
concept ContiguousIterableOfIterators = requires(I iterator) {
                                            { iterator.begin() } -> std::contiguous_iterator<>;
                                            { *(iterator.begin()) } -> ContiguousIterator<T>;
                                        };

template <typename I, typename T>
concept ContiguousIterableOfIterables = requires(I iterator) {
                                            { iterator.begin() } -> std::contiguous_iterator<>;
                                            { *(iterator.begin()) } -> ContiguousIterable<T>;
                                        };

template <typename I, typename T>
concept ContiguousIterableOfConstIterables = requires(I iterator) {
                                                 { iterator.begin() } -> std::contiguous_iterator<>;
                                                 {
                                                     *(iterator.begin())
                                                     } -> ContiguousConstIterable<T>;
                                             };

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename I, typename T>
concept ContiguousConstIterableOfIterators = requires(I iterator) {
                                                 {
                                                     iterator.cbegin()
                                                     } -> std::contiguous_iterator<>;
                                                 { *(iterator.cbegin()) } -> ContiguousIterator<T>;
                                             };

template <typename I, typename T>
concept ContiguousConstIterableOfIterables = requires(I iterator) {
                                                 {
                                                     iterator.cbegin()
                                                     } -> std::contiguous_iterator<>;
                                                 { *(iterator.cbegin()) } -> ContiguousIterable<T>;
                                             };

template <typename I, typename T>
concept ContiguousConstIterableOfConstIterables = requires(I iterator) {
                                                      {
                                                          iterator.cbegin()
                                                          } -> std::contiguous_iterator<>;
                                                      {
                                                          *(iterator.cbegin())
                                                          } -> ContiguousConstIterable<T>;
                                                  };

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename I>
concept RandomOnlyIterator = std::random_access_iterator<I> && !
std::contiguous_iterator<I>;

////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename I, typename T>
concept RandomIteratorOfIterators = RandomOnlyIterator<I> && requires(I iterator) {
                                                                 {
                                                                     *iterator
                                                                     } -> ContiguousIterator<T>;
                                                             };

template <typename I, typename T>
concept RandomIteratorOfIterables = RandomOnlyIterator<I> && requires(I iterator) {
                                                                 {
                                                                     *iterator
                                                                     } -> ContiguousIterable<T>;
                                                             };

template <typename I, typename T>
concept RandomIteratorOfConstIterables =
        RandomOnlyIterator<I> && requires(I iterator) {
                                     { *iterator } -> ContiguousConstIterable<T>;
                                 };

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename I, typename T>
concept RandomIterableOfIterators = requires(I iterator) {
                                        { iterator.begin() } -> RandomOnlyIterator<>;
                                        { *(iterator.begin()) } -> ContiguousIterator<T>;
                                    };

template <typename I, typename T>
concept RandomIterableOfIterables = requires(I iterator) {
                                        { iterator.begin() } -> RandomOnlyIterator<>;
                                        { *(iterator.begin()) } -> ContiguousIterable<T>;
                                    };

template <typename I, typename T>
concept RandomIterableOfConstIterables = requires(I iterator) {
                                             { iterator.begin() } -> RandomOnlyIterator<>;
                                             { *(iterator.begin()) } -> ContiguousConstIterable<T>;
                                         };

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename I, typename T>
concept RandomConstIterableOfIterators = requires(I iterator) {
                                             { iterator.cbegin() } -> RandomOnlyIterator<>;
                                             { *(iterator.cbegin()) } -> ContiguousIterator<T>;
                                         };

template <typename I, typename T>
concept RandomConstIterableOfIterables = requires(I iterator) {
                                             { iterator.cbegin() } -> RandomOnlyIterator<>;
                                             { *(iterator.cbegin()) } -> ContiguousIterable<T>;
                                         };

template <typename I, typename T>
concept RandomConstIterableOfConstIterables = requires(I iterator) {
                                                  { iterator.cbegin() } -> RandomOnlyIterator<>;
                                                  {
                                                      *(iterator.cbegin())
                                                      } -> ContiguousConstIterable<T>;
                                              };

/*
 *
 *
 *
 *
 *
 *
 */

template <typename I>
concept ParallelDataIterator =
        ContiguousIterator<std::remove_cvref_t<I>, const double> ||
        ContiguousIterable<std::remove_cvref_t<I>, const double> ||
        ContiguousConstIterable<std::remove_cvref_t<I>, const double> ||
        ContiguousIteratorOfIterators<std::remove_cvref_t<I>, const double> ||
        ContiguousIteratorOfIterables<std::remove_cvref_t<I>, const double> ||
        ContiguousIteratorOfConstIterables<std::remove_cvref_t<I>, const double> ||
        ContiguousIterableOfIterators<std::remove_cvref_t<I>, const double> ||
        ContiguousIterableOfIterables<std::remove_cvref_t<I>, const double> ||
        ContiguousIterableOfConstIterables<std::remove_cvref_t<I>, const double> ||
        RandomIteratorOfIterators<std::remove_cvref_t<I>, const double> ||
        RandomIteratorOfIterables<std::remove_cvref_t<I>, const double> ||
        RandomIteratorOfConstIterables<std::remove_cvref_t<I>, const double> ||
        RandomIterableOfIterators<std::remove_cvref_t<I>, const double> ||
        RandomIterableOfIterables<std::remove_cvref_t<I>, const double> ||
        RandomIterableOfConstIterables<std::remove_cvref_t<I>, const double> ||
        RandomConstIterableOfIterators<std::remove_cvref_t<I>, const double> ||
        RandomConstIterableOfIterables<std::remove_cvref_t<I>, const double> ||
        RandomConstIterableOfConstIterables<std::remove_cvref_t<I>, const double>;

template <typename I>
concept PiIterator = (RandomIterator<std::remove_reference_t<I>, std::size_t> &&
                      OutputIterator<std::remove_reference_t<I>, std::size_t>) ||
                     (RandomIterable<std::remove_reference_t<I>, std::size_t> &&
                      OutputIterable<std::remove_reference_t<I>, std::size_t>);

template <typename I>
concept LambdaIterator = (RandomIterator<std::remove_reference_t<I>, double> &&
                          OutputIterator<std::remove_reference_t<I>, double>) ||
                         (RandomIterable<std::remove_reference_t<I>, double> &&
                          OutputIterable<std::remove_reference_t<I>, double>);

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

template <typename I, typename T>
concept IndirectRandomAccessIterator = std::random_access_iterator<I> && !
std::contiguous_iterator<I> &&

        requires(I iterator, T *value) {
            //  value = *iterator;
            std::is_same_v<decltype(**iterator), double>;
            ContiguousIterator<decltype(*iterator), double *>;
        };

template <typename I, typename T>
concept IndirectContiguousAccessIterable =
        std::contiguous_iterator<I> &&
        requires(I iterator, T *value) {
            //  value = *iterator;
            std::is_same_v<decltype(*((*iterator).begin())), double>;
            ContiguousIterator<decltype(((*iterator).begin())), double *>;
        };

template <typename I, typename T>
concept IndirectRandomAccessIterable = std::random_access_iterator<I> && !
std::contiguous_iterator<I> &&requires(I iterator, T *value) {
                                  //  value = *iterator;
                                  std::is_same_v<decltype(*((*iterator).begin())), double>;
                                  ContiguousIterator<decltype(((*iterator).begin())), double *>;
                              };

template <typename I>
concept DataIterator = std::input_iterator<I> && InputIterator<I, const double *>;

}  // namespace cluster::utils
#endif  // FINAL_PROJECT_HPC_TYPES_H
