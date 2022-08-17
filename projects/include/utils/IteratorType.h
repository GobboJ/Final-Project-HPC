#ifndef FINAL_PROJECT_HPC_ITERATORTYPE_H
#define FINAL_PROJECT_HPC_ITERATORTYPE_H

#include <string>

namespace cluster::utils {

/**
 * Enumeration of the possible types of iterator or iterable holding the first level of indirection
 * of the data samples to cluster.<br>
 * Note that the second level is always a contiguous iterator, or a contiguous iterable data
 * structure.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-08-16
 * @since 1.0
 */
enum class DataType {
    /**
     * No type.
     */
    NONE,

    /**
     * Contiguous iterator or iterable.
     */
    CONTIGUOUS,

    /**
     * Random iterator or iterable that is not contiguous.
     */
    RANDOM,

    /**
     * Input iterator or iterable that is not random.
     */
    INPUT
};

/**
 * Enumeration of the possible types of data structures that can be found on both levels of
 * indirection of the data samples to cluster.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-13
 * @since 1.0
 */
enum class DataLevelType {

    /**
     * No type.
     */
    NONE,

    /**
     * The data structure is an iterator.
     */
    ITERATOR,

    /**
     * The data structure is iterable by using the iterator returned by the <code>begin()</code>
     * method.
     */
    ITERABLE,

    /**
     * The data structure is const iterable by using the iterator returned by the
     * <code>cbegin()</code> method.
     */
    CONST_ITERABLE
};

/**
 * Utility class allowing to compute a textual description of the type of data structure/iterator
 * used to hold the data samples to cluster.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.2 2022-08-16
 * @since 1.0
 */
class DataIteratorTypeUtils {

public:
    /**
     * Returns a textual description of the data structure/iterator used to hold the data samples to
     * cluster.
     *
     * @param iteratorType Type of the data iterator/iterable holding the first level of
     * indirection of the data samples to cluster.
     * @param firstLevelIteratorType Type of the data structure/iterator holding the first level of
     * indirection of the data samples to cluster.
     * @param secondLevelIteratorType Type of the data structure/iterator holding the second level
     * of indirection of the data samples to cluster.
     * @return The computed textual description.
     */
    static std::string getDescription(DataType iteratorType,
                                      DataLevelType firstLevelIteratorType,
                                      DataLevelType secondLevelIteratorType) {

        // Clear the result
        std::string result{};

        // Append the type of iterator/iterable at the first level of indirection
        switch (iteratorType) {
            case DataType::CONTIGUOUS:
                result += "Contiguous";
                break;
            case DataType::RANDOM:
                result += "Random";
                break;
            case DataType::INPUT:
                result += "Input";
                break;
            case DataType::NONE:
            default:
                result += "Unknown";
                break;
        }

        // Append the type of data structure at the first level of indirection
        switch (firstLevelIteratorType) {
            case DataLevelType::ITERATOR:
                result += " iterator";
                break;
            case DataLevelType::ITERABLE:
                result += " iterable";
                break;
            case DataLevelType::CONST_ITERABLE:
                result += " const iterable";
                break;
            case DataLevelType::NONE:
            default:
                result += " unknown";
                break;
        }

        // Append the type of data structure at the second level of indirection
        switch (secondLevelIteratorType) {
            case DataLevelType::ITERATOR:
                result += " of iterators";
                break;
            case DataLevelType::ITERABLE:
                result += " of iterables";
                break;
            case DataLevelType::CONST_ITERABLE:
                result += " of const iterables";
                break;
            case DataLevelType::NONE:
                result += "";
                break;
            default:
                result += " of unknown";
                break;
        }

        // Return the computed description
        return result;
    }
};

/**
 * Enumeration of the possible types of data structures that can hold the values of <code>pi</code>
 * and <code>lambda</code>;
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-13
 * @since 1.0
 */
enum class PiLambdaType {

    /**
     * No type.
     */
    NONE,

    /**
     * The data structure is a contiguous iterator.
     */
    CONTIGUOUS_ITERATOR,

    /**
     * The data structure is iterable by using the contiguous iterator returned by the
     * <code>begin()</code> method.
     */
    CONTIGUOUS_ITERABLE,

    /**
     * The data structure is a random iterator that is not contiguous.
     */
    RANDOM_ITERATOR,

    /**
     * The data structure is iterable by using the random iterator returned by the
     * <code>begin()</code> method.
     */
    RANDOM_ITERABLE
};

/**
 * Utility class allowing to compute a textual description of the type of data structure/iterator
 * used to hold the values of <code>pi</code> and <code>lambda</code>.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-13
 * @since 1.0
 */
class PiLambdaIteratorTypeUtils {

public:
    /**
     * Returns a textual description of the data structure/iterator used to hold the values of
     * <code>pi</code> and <code>lambda</code>.
     *
     * @param iteratorType Type of the iterator/iterable holding the values of <code>pi</code> and
     * <code>lambda</code>.
     * @return The computed textual description.
     */
    static std::string getDescription(PiLambdaType iteratorType) {

        // Compose the textual description based on the type of data structure
        switch (iteratorType) {

            case PiLambdaType::CONTIGUOUS_ITERATOR:
                return "Contiguous iterator";
            case PiLambdaType::CONTIGUOUS_ITERABLE:
                return "Contiguous iterable";
            case PiLambdaType::RANDOM_ITERATOR:
                return "Random iterator";
            case PiLambdaType::RANDOM_ITERABLE:
                return "Random iterable";
            case PiLambdaType::NONE:
            default:
                return "Invalid iterator";
        }
    }
};

}  // namespace cluster::utils
#endif  // FINAL_PROJECT_HPC_ITERATORTYPE_H
