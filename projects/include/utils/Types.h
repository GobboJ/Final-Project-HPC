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
                            iterator.begin();
                            { *(iterator.cbegin()) } -> std::convertible_to<const T>;
                        };

template <typename I, typename T>
concept RandomIterator = std::random_access_iterator<I> && InputIterator<I, T>;

template <typename I, typename T>
concept RandomIterable = Iterable<I, T> && requires(I iterator) {
                                               {
                                                   iterator.begin()
                                                   } -> std::random_access_iterator<>;
                                           };

template <typename I, typename T>
concept ContiguousIterator = std::contiguous_iterator<I> && InputIterator<I, T>;

template <typename I, typename T>
concept ContiguousIterable = Iterable<I, T> && requires(I iterator) {
                                                   {
                                                       iterator.begin()
                                                       } -> std::contiguous_iterator<>;
                                               };

template <typename I, typename T>
concept ContiguousConstIterable = ConstIterable<I, T> && requires(I iterator) {
                                                             {
                                                                 iterator.cbegin()
                                                                 } -> std::contiguous_iterator<>;
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
concept ParallelDataIterator = ContiguousIterator<I, double> || ContiguousIterable<I, double> ||
                               ContiguousConstIterable<I, double>;

template <typename I>
concept PiIterator = (RandomIterator<I, std::size_t> && OutputIterator<I, std::size_t>) ||
                     (RandomIterable<I, std::size_t> && OutputIterable<I, std::size_t>);

template <typename I>
concept LambdaIterator = (RandomIterator<I, double> && OutputIterator<I, double>) ||
                         (RandomIterable<I, double> && OutputIterable<I, double>);

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
// Iterator of iterator double **
template <typename I, typename T>
concept IndirectContiguousAccessIterator = std::contiguous_iterator<I> &&

                                           requires(I iterator, T *value) {
                                               //  value = *iterator;
                                               std::is_same_v<decltype(**iterator), double>;
                                               ContiguousIterator<decltype(*iterator), double *>;
                                           };

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
