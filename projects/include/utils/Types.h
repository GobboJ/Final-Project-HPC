/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-31
 * @since version date
 */
#ifndef FINAL_PROJECT_HPC_TYPES_H
#define FINAL_PROJECT_HPC_TYPES_H

template <typename I, typename T>
concept InputIterator = requires(I iterator, T value) { value = *iterator; };

template <typename I, typename T>
concept OutputIterator = requires(I iterator, T value) { *iterator = value; };

template <typename I>
concept DataIterator = std::input_iterator<I> && InputIterator<I, double*>;

template <typename I>
concept PiIterator = std::random_access_iterator<I> && InputIterator<I, std::size_t> &&
                     OutputIterator<I, std::size_t>;

template <typename I>
concept LambdaIterator =
        std::random_access_iterator<I> && InputIterator<I, double> && OutputIterator<I, double>;

#endif  // FINAL_PROJECT_HPC_TYPES_H
