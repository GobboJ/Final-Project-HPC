/*
 * File containing several utility concepts using to select the appropriate overloads when dealing
 * with the data structure holding the samples to cluster, or the data structures holding pi and
 * lambda.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-16
 * @since 1.0
 */
#ifndef FINAL_PROJECT_HPC_TYPES_H
#define FINAL_PROJECT_HPC_TYPES_H

#include <iterator>
#include <type_traits>

namespace cluster::utils {

/**
 * Utility concept requiring the two specified types to be the same non-const-qualified type, and to
 * be <code>const</code>-correct.
 * For instance:
 * <table>
 *  <thead>
 *      <tr>
 *          <td><code>U</code></td>
 *          <td><code>T</code></td>
 *          <td>Compiles?</td>
 *      </tr>
 *  </thead>
 *  <tbody>
 *      <tr>
 *          <td><code>double</code></td>
 *          <td><code>double</code></td>
 *          <td>Yes</td>
 *      </tr>
 *      <tr>
 *          <td><code>const double</code></td>
 *          <td><code>double</code></td>
 *          <td>No</td>
 *      </tr>
 *      <tr>
 *          <td><code>double</code></td>
 *          <td><code>const double</code></td>
 *          <td>Yes</td>
 *      </tr>
 *      <tr>
 *          <td><code>const double</code></td>
 *          <td><code>const double</code></td>
 *          <td>Yes</td>
 *      </tr>
 *  </tbody>
 * </table>
 * @tparam U Type to check against <code>T</code>.
 * @tparam T Type <code>U</code> is required to be equal to.
 */
template <typename U, typename T>
concept IsSame = requires(std::remove_reference_t<U> *normalU,
                          const std::remove_reference_t<U> *constU,
                          std::remove_reference_t<T> *normalT) {
                     normalT = normalU;
                     constU = normalT;
                 };

/**
 * Requires the specified type <code>I</code> to be an input iterator that provides elements of a
 * type that is <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects elements by the iterator.
 */
template <typename I, typename T>
concept InputIterator = requires(I iterator) {
                            { *iterator } -> IsSame<T>;
                        };

/**
 * Requires the specified type <code>I</code> to be an output iterator that accepts elements of type
 * <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects elements accepted by the iterator.
 */
template <typename I, typename T>
concept OutputIterator = requires(I iterator, T value) { *iterator = value; };

/**
 * Requires objects of the specified type <code>D</code> to provide an output iterator through the
 * <code>begin</code> method, and requires the returned iterator to accept elements of type
 * <code>T</code>.
 *
 * @tparam D Type of the objects that are required to be iterable.
 * @tparam T Type of the objects elements accepted by the iterator.
 */
template <typename D, typename T>
concept OutputIterable = requires(D object, T value) { *(object.begin()) = value; };

/**
 * Requires objects of the specified type <code>D</code> to provide an iterator through the
 * <code>begin</code> method, and requires the returned iterator to provide elements of a type that
 * is <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects that are required to be iterable.
 * @tparam T Type of the objects the iterator must return.
 */
template <typename D, typename T>
concept Iterable = requires(D object) {
                       object.begin();
                       { *(object.begin()) } -> IsSame<T>;
                   };

/**
 * Requires objects of the specified type <code>D</code> to provide a const iterator through the
 * <code>cbegin</code> method, and requires the returned const iterator to provide elements of a
 * type that is <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects that are required to be const iterable.
 * @tparam T Type of the objects the iterator must return.
 */
template <typename D, typename T>
concept ConstIterable = requires(D object) {
                            object.cbegin();
                            { *(object.cbegin()) } -> IsSame<const T>;
                        };

/**
 * Requires the iterator of the specified type <code>I</code> to be a random access iterator that
 * provides elements of a type that is <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the iterator must return.
 */
template <typename I, typename T>
concept RandomIterator = std::random_access_iterator<I> && InputIterator<I, T>;

/**
 * Requires objects of the specified type <code>D</code> to provide a random access iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements of a
 * type that is <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects that are required to be random iterable.
 * @tparam T Type of the objects the iterator must return.
 */
template <typename D, typename T>
concept RandomIterable =
        Iterable<std::remove_reference_t<D>, T> && requires(D object) {
                                                       {
                                                           object.begin()
                                                           } -> std::random_access_iterator<>;
                                                   };

/**
 * Requires the iterator of the specified type <code>I</code> to be a contiguous iterator that
 * provides elements of a type that is <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the iterator must return.
 */
template <typename I, typename T>
concept ContiguousIterator = std::contiguous_iterator<std::remove_cvref_t<I>> &&
                             InputIterator<std::remove_reference_t<I>, T>;

/**
 * Requires objects of the specified type <code>D</code> to provide a contiguous iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements of a
 * type that is <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects that are required to be contiguous iterable.
 * @tparam T Type of the objects the iterator must return.
 */
template <typename D, typename T>
concept ContiguousIterable =
        Iterable<std::remove_reference_t<D>, T> && requires(D object) {
                                                       {
                                                           object.begin()
                                                           } -> std::contiguous_iterator<>;
                                                   };

/**
 * Requires objects of the specified type <code>D</code> to provide a contiguous iterator through
 * the <code>cbegin</code> method, and requires the returned iterator to provide elements of a
 * type that is <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects that are required to be contiguous iterable.
 * @tparam T Type of the objects the iterator must return.
 */
template <typename D, typename T>
concept ContiguousConstIterable =
        ConstIterable<std::remove_reference_t<D>, T> && requires(D object) {
                                                            {
                                                                object.cbegin()
                                                                } -> std::contiguous_iterator<>;
                                                        };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Requires the iterator of the specified type <code>I</code> to be a contiguous iterator providing
 * elements that are, in turn, contiguous iterators providing elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename I, typename T>
concept ContiguousIteratorOfIterators =
        std::contiguous_iterator<I> && requires(I iterator) {
                                           { *iterator } -> ContiguousIterator<T>;
                                       };

/**
 * Requires the iterator of the specified type <code>I</code> to be a contiguous iterator returning
 * elements that provide a contiguous iterator through the <code>begin</code> method, and requires
 * the returned iterator to provide elements of a type that is <code>const</code>-compatible to
 * <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename I, typename T>
concept ContiguousIteratorOfIterables =
        std::contiguous_iterator<I> && requires(I iterator) {
                                           { *iterator } -> ContiguousIterable<T>;
                                       };

/**
 * Requires the iterator of the specified type <code>I</code> to be a contiguous iterator returning
 * elements that provide a contiguous iterator through the <code>cbegin</code> method, and requires
 * the returned iterator to provide elements of a type that is <code>const</code>-compatible to
 * <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename I, typename T>
concept ContiguousIteratorOfConstIterables =
        std::contiguous_iterator<I> && requires(I iterator) {
                                           { *iterator } -> ContiguousConstIterable<T>;
                                       };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a contiguous iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements that, in
 * turn, are contiguous iterators providing elements of a type that is <code>const</code>-compatible
 * to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept ContiguousIterableOfIterators = requires(D object) {
                                            { object.begin() } -> std::contiguous_iterator<>;
                                            { *(object.begin()) } -> ContiguousIterator<T>;
                                        };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a contiguous iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements that, in
 * turn, provide a contiguous iterator through the <code>begin</code> method. Moreover, it requires
 * the second-level iterators to provide elements of a type that is <code>const</code>-compatible to
 * <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept ContiguousIterableOfIterables = requires(D object) {
                                            { object.begin() } -> std::contiguous_iterator<>;
                                            { *(object.begin()) } -> ContiguousIterable<T>;
                                        };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a contiguous iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements that, in
 * turn, provide a contiguous const iterator through the <code>cbegin</code> method. Moreover, it
 * requires the second-level const iterators to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level const iterators must return.
 */
template <typename D, typename T>
concept ContiguousIterableOfConstIterables = requires(D object) {
                                                 { object.begin() } -> std::contiguous_iterator<>;
                                                 {
                                                     *(object.begin())
                                                     } -> ContiguousConstIterable<T>;
                                             };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a contiguous const iterator
 * through the <code>cbegin</code> method, and requires the returned iterator to provide elements
 * that, in turn, are contiguous iterators providing elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept ContiguousConstIterableOfIterators = requires(D object) {
                                                 { object.cbegin() } -> std::contiguous_iterator<>;
                                                 { *(object.cbegin()) } -> ContiguousIterator<T>;
                                             };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a contiguous const iterator
 * through the <code>cbegin</code> method, and requires the returned iterator to provide elements
 * that, in turn, provide a contiguous iterator through the <code>begin</code> method. Moreover, it
 * requires the second-level iterators to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept ContiguousConstIterableOfIterables = requires(D object) {
                                                 { object.cbegin() } -> std::contiguous_iterator<>;
                                                 { *(object.cbegin()) } -> ContiguousIterable<T>;
                                             };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a contiguous const iterator
 * through the <code>cbegin</code> method, and requires the returned iterator to provide elements
 * that, in turn, provide a contiguous const iterator through the <code>cbegin</code> method.
 * Moreover, it requires the second-level const iterators to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level const iterators must return.
 */
template <typename D, typename T>
concept ContiguousConstIterableOfConstIterables = requires(D object) {
                                                      {
                                                          object.cbegin()
                                                          } -> std::contiguous_iterator<>;
                                                      {
                                                          *(object.cbegin())
                                                          } -> ContiguousConstIterable<T>;
                                                  };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Requires the specified type to be a random access iterator, but not a contiguous one.
 *
 * @tparam I Type of the iterator to check.
 */
template <typename I>
concept RandomOnlyIterator = std::random_access_iterator<I> && !
std::contiguous_iterator<I>;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Requires the iterator of the specified type <code>I</code> to be a random access iterator
 * providing elements that are, in turn, contiguous iterators providing elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename I, typename T>
concept RandomIteratorOfIterators = RandomOnlyIterator<I> && requires(I iterator) {
                                                                 {
                                                                     *iterator
                                                                     } -> ContiguousIterator<T>;
                                                             };

/**
 * Requires the iterator of the specified type <code>I</code> to be a random access iterator
 * returning elements that provide a contiguous iterator through the <code>begin</code> method, and
 * requires the returned iterator to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename I, typename T>
concept RandomIteratorOfIterables = RandomOnlyIterator<I> && requires(I iterator) {
                                                                 {
                                                                     *iterator
                                                                     } -> ContiguousIterable<T>;
                                                             };

/**
 * Requires the iterator of the specified type <code>I</code> to be a random access iterator
 * returning elements that provide a contiguous iterator through the <code>cbegin</code> method, and
 * requires the returned iterator to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename I, typename T>
concept RandomIteratorOfConstIterables =
        RandomOnlyIterator<I> && requires(I iterator) {
                                     { *iterator } -> ContiguousConstIterable<T>;
                                 };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a random access iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements that, in
 * turn, are contiguous iterators providing elements of a type that is <code>const</code>-compatible
 * to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept RandomIterableOfIterators = requires(D object) {
                                        { object.begin() } -> RandomOnlyIterator<>;
                                        { *(object.begin()) } -> ContiguousIterator<T>;
                                    };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a random access iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements that, in
 * turn, provide a contiguous iterator through the <code>begin</code> method. Moreover, it requires
 * the second-level iterators to provide elements of a type that is <code>const</code>-compatible to
 * <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept RandomIterableOfIterables = requires(D object) {
                                        { object.begin() } -> RandomOnlyIterator<>;
                                        { *(object.begin()) } -> ContiguousIterable<T>;
                                    };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a random access iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements that, in
 * turn, provide a contiguous const iterator through the <code>cbegin</code> method. Moreover, it
 * requires the second-level const iterators to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level const iterators must return.
 */
template <typename D, typename T>
concept RandomIterableOfConstIterables = requires(D object) {
                                             { object.begin() } -> RandomOnlyIterator<>;
                                             { *(object.begin()) } -> ContiguousConstIterable<T>;
                                         };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a random access const iterator
 * through the <code>cbegin</code> method, and requires the returned iterator to provide elements
 * that, in turn, are contiguous iterators providing elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept RandomConstIterableOfIterators = requires(D object) {
                                             { object.cbegin() } -> RandomOnlyIterator<>;
                                             { *(object.cbegin()) } -> ContiguousIterator<T>;
                                         };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a random access const iterator
 * through the <code>cbegin</code> method, and requires the returned iterator to provide elements
 * that, in turn, provide a contiguous iterator through the <code>begin</code> method. Moreover, it
 * requires the second-level iterators to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept RandomConstIterableOfIterables = requires(D object) {
                                             { object.cbegin() } -> RandomOnlyIterator<>;
                                             { *(object.cbegin()) } -> ContiguousIterable<T>;
                                         };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide a random access const iterator
 * through the <code>cbegin</code> method, and requires the returned iterator to provide elements
 * that, in turn, provide a contiguous const iterator through the <code>cbegin</code> method.
 * Moreover, it requires the second-level const iterators to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level const iterators must return.
 */
template <typename D, typename T>
concept RandomConstIterableOfConstIterables = requires(D object) {
                                                  { object.cbegin() } -> RandomOnlyIterator<>;
                                                  {
                                                      *(object.cbegin())
                                                      } -> ContiguousConstIterable<T>;
                                              };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Requires the specified type to be an input iterator, but not a random access one.
 *
 * @tparam I Type of the iterator to check.
 */
template <typename I>
concept InputOnlyIterator = std::input_iterator<I> && !
std::random_access_iterator<I>;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Requires the iterator of the specified type <code>I</code> to be an input iterator providing
 * elements that are, in turn, contiguous iterators providing elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename I, typename T>
concept InputIteratorOfIterators = InputOnlyIterator<I> && requires(I iterator) {
                                                               {
                                                                   *iterator
                                                                   } -> ContiguousIterator<T>;
                                                           };

/**
 * Requires the iterator of the specified type <code>I</code> to be an input iterator returning
 * elements that provide a contiguous iterator through the <code>begin</code> method, and requires
 * the returned iterator to provide elements of a type that is <code>const</code>-compatible to
 * <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename I, typename T>
concept InputIteratorOfIterables = InputOnlyIterator<I> && requires(I iterator) {
                                                               {
                                                                   *iterator
                                                                   } -> ContiguousIterable<T>;
                                                           };

/**
 * Requires the iterator of the specified type <code>I</code> to be an input iterator returning
 * elements that provide a contiguous iterator through the <code>cbegin</code> method, and requires
 * the returned iterator to provide elements of a type that is <code>const</code>-compatible to
 * <code>T</code>.
 *
 * @tparam I Type of the iterator.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename I, typename T>
concept InputIteratorOfConstIterables =
        InputOnlyIterator<I> && requires(I iterator) {
                                    { *iterator } -> ContiguousConstIterable<T>;
                                };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 * Requires objects of the specified type <code>D</code> to provide an input iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements that, in
 * turn, are contiguous iterators providing elements of a type that is <code>const</code>-compatible
 * to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept InputIterableOfIterators = requires(D object) {
                                       { object.begin() } -> InputOnlyIterator<>;
                                       { *(object.begin()) } -> ContiguousIterator<T>;
                                   };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide an input iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements that, in
 * turn, provide a contiguous iterator through the <code>begin</code> method. Moreover, it requires
 * the second-level iterators to provide elements of a type that is <code>const</code>-compatible to
 * <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept InputIterableOfIterables = requires(D object) {
                                       { object.begin() } -> InputOnlyIterator<>;
                                       { *(object.begin()) } -> ContiguousIterable<T>;
                                   };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide an input iterator through
 * the <code>begin</code> method, and requires the returned iterator to provide elements that, in
 * turn, provide a contiguous const iterator through the <code>cbegin</code> method. Moreover, it
 * requires the second-level const iterators to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level const iterators must return.
 */
template <typename D, typename T>
concept InputIterableOfConstIterables = requires(D object) {
                                            { object.begin() } -> InputOnlyIterator<>;
                                            { *(object.begin()) } -> ContiguousConstIterable<T>;
                                        };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 * Requires objects of the specified type <code>D</code> to provide an input const iterator
 * through the <code>cbegin</code> method, and requires the returned iterator to provide elements
 * that, in turn, are contiguous iterators providing elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept InputConstIterableOfIterators = requires(D object) {
                                            { object.cbegin() } -> InputOnlyIterator<>;
                                            { *(object.cbegin()) } -> ContiguousIterator<T>;
                                        };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide an input const iterator
 * through the <code>cbegin</code> method, and requires the returned iterator to provide elements
 * that, in turn, provide a contiguous iterator through the <code>begin</code> method. Moreover, it
 * requires the second-level iterators to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level iterators must return.
 */
template <typename D, typename T>
concept InputConstIterableOfIterables = requires(D object) {
                                            { object.cbegin() } -> InputOnlyIterator<>;
                                            { *(object.cbegin()) } -> ContiguousIterable<T>;
                                        };

/**
 *
 * Requires objects of the specified type <code>D</code> to provide an input const iterator
 * through the <code>cbegin</code> method, and requires the returned iterator to provide elements
 * that, in turn, provide a contiguous const iterator through the <code>cbegin</code> method.
 * Moreover, it requires the second-level const iterators to provide elements of a type that is
 * <code>const</code>-compatible to <code>T</code>.
 *
 * @tparam D Type of the objects to check against this concept.
 * @tparam T Type of the objects the second-level const iterators must return.
 */
template <typename D, typename T>
concept InputConstIterableOfConstIterables = requires(D object) {
                                                 { object.cbegin() } -> InputOnlyIterator<>;
                                                 {
                                                     *(object.cbegin())
                                                     } -> ContiguousConstIterable<T>;
                                             };

/**
 * Concept satisfied only by the data structures holding the samples to cluster that are accepted by
 * the parallel implementation of the clustering algorithm.
 *
 * @tparam D Type of the data structure.
 */
template <typename D>
concept ParallelDataIterator =
        ContiguousIterator<std::remove_cvref_t<D>, const double> ||
        ContiguousIterable<std::remove_cvref_t<D>, const double> ||
        ContiguousConstIterable<std::remove_cvref_t<D>, const double> ||
        ContiguousIteratorOfIterators<std::remove_cvref_t<D>, const double> ||
        ContiguousIteratorOfIterables<std::remove_cvref_t<D>, const double> ||
        ContiguousIteratorOfConstIterables<std::remove_cvref_t<D>, const double> ||
        ContiguousIterableOfIterators<std::remove_cvref_t<D>, const double> ||
        ContiguousIterableOfIterables<std::remove_cvref_t<D>, const double> ||
        ContiguousIterableOfConstIterables<std::remove_cvref_t<D>, const double> ||
        ContiguousConstIterableOfIterators<std::remove_cvref_t<D>, const double> ||
        ContiguousConstIterableOfIterables<std::remove_cvref_t<D>, const double> ||
        ContiguousConstIterableOfConstIterables<std::remove_cvref_t<D>, const double> ||
        RandomIteratorOfIterators<std::remove_cvref_t<D>, const double> ||
        RandomIteratorOfIterables<std::remove_cvref_t<D>, const double> ||
        RandomIteratorOfConstIterables<std::remove_cvref_t<D>, const double> ||
        RandomIterableOfIterators<std::remove_cvref_t<D>, const double> ||
        RandomIterableOfIterables<std::remove_cvref_t<D>, const double> ||
        RandomIterableOfConstIterables<std::remove_cvref_t<D>, const double> ||
        RandomConstIterableOfIterators<std::remove_cvref_t<D>, const double> ||
        RandomConstIterableOfIterables<std::remove_cvref_t<D>, const double> ||
        RandomConstIterableOfConstIterables<std::remove_cvref_t<D>, const double>;

/**
 * Concept satisfied only by the data structures holding the samples to cluster that are accepted by
 * the sequential implementation of the clustering algorithm.
 *
 * @tparam D Type of the data structure.
 */
template <typename D>
concept SequentialDataIterator =
        ParallelDataIterator<D> || InputIteratorOfIterators<std::remove_cvref_t<D>, const double> ||
        InputIteratorOfIterables<std::remove_cvref_t<D>, const double> ||
        InputIteratorOfConstIterables<std::remove_cvref_t<D>, const double> ||
        InputIterableOfIterators<std::remove_cvref_t<D>, const double> ||
        InputIterableOfIterables<std::remove_cvref_t<D>, const double> ||
        InputIterableOfConstIterables<std::remove_cvref_t<D>, const double> ||
        InputConstIterableOfIterators<std::remove_cvref_t<D>, const double> ||
        InputConstIterableOfIterables<std::remove_cvref_t<D>, const double> ||
        InputConstIterableOfConstIterables<std::remove_cvref_t<D>, const double>;

/**
 * Concept satisfied only by the data structures holding pi accepted by the two implementations of
 * the clustering algorithm.
 *
 * @tparam D Type of the data structure.
 */
template <typename D>
concept PiIterator = (RandomIterator<std::remove_reference_t<D>, std::size_t> &&
                      OutputIterator<std::remove_reference_t<D>, std::size_t>) ||
                     (RandomIterable<std::remove_reference_t<D>, std::size_t> &&
                      OutputIterable<std::remove_reference_t<D>, std::size_t>);

/**
 * Concept satisfied only by the data structures holding lambda accepted by the two implementations
 * of the clustering algorithm.
 *
 * @tparam D Type of the data structure.
 */
template <typename D>
concept LambdaIterator = (RandomIterator<std::remove_reference_t<D>, double> &&
                          OutputIterator<std::remove_reference_t<D>, double>) ||
                         (RandomIterable<std::remove_reference_t<D>, double> &&
                          OutputIterable<std::remove_reference_t<D>, double>);

}  // namespace cluster::utils
#endif  // FINAL_PROJECT_HPC_TYPES_H
