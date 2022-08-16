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

//     U                T
// double         double                OK
// const double   double                No
// double         const double          OK
// const double   const double          OK
template <typename U, typename T>
concept IsSame = requires(std::remove_reference_t<U> *normalU,
                          const std::remove_reference_t<U> *constU,
                          std::remove_reference_t<T> *normalT) {
                     normalT = normalU;
                     constU = normalT;
                 };

template <typename I, typename T>
concept InputIterator = requires(I iterator) {
                            { *iterator } -> IsSame<T>;
                        };

template <typename I, typename T>
concept OutputIterator = requires(I iterator, T value) { *iterator = value; };

template <typename I, typename T>
concept OutputIterable = requires(I iterator, T value) { *(iterator.begin()) = value; };

template <typename I, typename T>
concept Iterable = requires(I iterator) {
                       iterator.begin();
                       { *(iterator.begin()) } -> IsSame<T>;
                   };

template <typename I, typename T>
concept ConstIterable = requires(I iterator) {
                            iterator.cbegin();
                            { *(iterator.cbegin()) } -> IsSame<const T>;
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

////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename I>
concept InputOnlyIterator = std::input_iterator<I> && !
std::random_access_iterator<I>;

////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename I, typename T>
concept InputIteratorOfIterators = InputOnlyIterator<I> && requires(I iterator) {
                                                               {
                                                                   *iterator
                                                                   } -> ContiguousIterator<T>;
                                                           };

template <typename I, typename T>
concept InputIteratorOfIterables = InputOnlyIterator<I> && requires(I iterator) {
                                                               {
                                                                   *iterator
                                                                   } -> ContiguousIterable<T>;
                                                           };

template <typename I, typename T>
concept InputIteratorOfConstIterables =
        InputOnlyIterator<I> && requires(I iterator) {
                                    { *iterator } -> ContiguousConstIterable<T>;
                                };

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename I, typename T>
concept InputIterableOfIterators = requires(I iterator) {
                                       { iterator.begin() } -> InputOnlyIterator<>;
                                       { *(iterator.begin()) } -> ContiguousIterator<T>;
                                   };

template <typename I, typename T>
concept InputIterableOfIterables = requires(I iterator) {
                                       { iterator.begin() } -> InputOnlyIterator<>;
                                       { *(iterator.begin()) } -> ContiguousIterable<T>;
                                   };

template <typename I, typename T>
concept InputIterableOfConstIterables = requires(I iterator) {
                                            { iterator.begin() } -> InputOnlyIterator<>;
                                            { *(iterator.begin()) } -> ContiguousConstIterable<T>;
                                        };

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename I, typename T>
concept InputConstIterableOfIterators = requires(I iterator) {
                                            { iterator.cbegin() } -> InputOnlyIterator<>;
                                            { *(iterator.cbegin()) } -> ContiguousIterator<T>;
                                        };

template <typename I, typename T>
concept InputConstIterableOfIterables = requires(I iterator) {
                                            { iterator.cbegin() } -> InputOnlyIterator<>;
                                            { *(iterator.cbegin()) } -> ContiguousIterable<T>;
                                        };

template <typename I, typename T>
concept InputConstIterableOfConstIterables = requires(I iterator) {
                                                 { iterator.cbegin() } -> InputOnlyIterator<>;
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
        ContiguousConstIterableOfIterators<std::remove_cvref_t<I>, const double> ||
        ContiguousConstIterableOfIterables<std::remove_cvref_t<I>, const double> ||
        ContiguousConstIterableOfConstIterables<std::remove_cvref_t<I>, const double> ||
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
concept SequentialDataIterator =
        ParallelDataIterator<I> || InputIteratorOfIterators<std::remove_cvref_t<I>, const double> ||
        InputIteratorOfIterables<std::remove_cvref_t<I>, const double> ||
        InputIteratorOfConstIterables<std::remove_cvref_t<I>, const double> ||
        InputIterableOfIterators<std::remove_cvref_t<I>, const double> ||
        InputIterableOfIterables<std::remove_cvref_t<I>, const double> ||
        InputIterableOfConstIterables<std::remove_cvref_t<I>, const double> ||
        InputConstIterableOfIterators<std::remove_cvref_t<I>, const double> ||
        InputConstIterableOfIterables<std::remove_cvref_t<I>, const double> ||
        InputConstIterableOfConstIterables<std::remove_cvref_t<I>, const double>;

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

}  // namespace cluster::utils
#endif  // FINAL_PROJECT_HPC_TYPES_H
