/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-08-12
 * @since version date
 */
#ifndef FINAL_PROJECT_HPC_ITERATORTYPE_H
#define FINAL_PROJECT_HPC_ITERATORTYPE_H

namespace cluster::utils {

enum class DataIteratorType { NONE, CONTIGUOUS, RANDOM, INPUT };

enum class DataLevelIteratorType { NONE, ITERATOR, ITERABLE, CONST_ITERABLE };

class DataIteratorTypeUtils {

public:
    static std::string getDescription(DataIteratorType iteratorType,
                                      DataLevelIteratorType firstLevelIteratorType,
                                      DataLevelIteratorType secondLevelIteratorType) {

        std::string result{};

        switch (iteratorType) {
            case DataIteratorType::CONTIGUOUS:
                result += "Contiguous";
                break;
            case DataIteratorType::RANDOM:
                result += "Random";
                break;
            case DataIteratorType::INPUT:
                result += "Input";
                break;
            case DataIteratorType::NONE:
            default:
                result += "Unknown";
                break;
        }

        switch (firstLevelIteratorType) {
            case DataLevelIteratorType::ITERATOR:
                result += " iterator";
                break;
            case DataLevelIteratorType::ITERABLE:
                result += " iterable";
                break;
            case DataLevelIteratorType::CONST_ITERABLE:
                result += " const iterable";
                break;
            case DataLevelIteratorType::NONE:
            default:
                result += " unknown";
                break;
        }
        
        switch (secondLevelIteratorType) {
            case DataLevelIteratorType::ITERATOR:
                result += " of iterators";
                break;
            case DataLevelIteratorType::ITERABLE:
                result += " of iterables";
                break;
            case DataLevelIteratorType::CONST_ITERABLE:
                result += " of const iterables";
                break;
            case DataLevelIteratorType::NONE:
            default:
                result += " of unknown";
                break;
        }

        return result;
        /*TODO: switch (iteratorType) {

            case DataIteratorType::CONTIGUOUS_ITERATOR:
                return "Contiguous iterator";

            case DataIteratorType::CONTIGUOUS_ITERABLE:
                return "Contiguous iterable";

            case DataIteratorType::CONTIGUOUS_CONST_ITERABLE:
                return "Contiguous const iterable";

            case DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERATORS:
                return "Contiguous iterator of iterators";

            case DataIteratorType::CONTIGUOUS_ITERATOR_OF_ITERABLES:
                return "Contiguous iterator of iterables";

            case DataIteratorType::CONTIGUOUS_ITERATOR_OF_CONST_ITERABLES:
                return "Contiguous iterator of const iterables";

            case DataIteratorType::CONTIGUOUS_ITERABLE_OF_ITERATORS:
                return "Contiguous iterable of iterators";

            case DataIteratorType::CONTIGUOUS_ITERABLE_OF_ITERABLES:
                return "Contiguous iterable of iterables";

            case DataIteratorType::CONTIGUOUS_ITERABLE_OF_CONST_ITERABLES:
                return "Contiguous iterable of const iterables";

            case DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_ITERATORS:
                return "Contiguous const iterable of iterators";

            case DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_ITERABLES:
                return "Contiguous const iterable of iterables";

            case DataIteratorType::CONTIGUOUS_CONST_ITERABLE_OF_CONST_ITERABLES:
                return "Contiguous const iterable of const iterables";

            case DataIteratorType::RANDOM_ITERATOR_OF_ITERATORS:
                return "Random iterator of iterators";

            case DataIteratorType::RANDOM_ITERATOR_OF_ITERABLES:
                return "Random iterator of iterables";

            case DataIteratorType::RANDOM_ITERATOR_OF_CONST_ITERABLES:
                return "Random iterator of const iterables";

            case DataIteratorType::RANDOM_ITERABLE_OF_ITERATORS:
                return "Random iterable of iterators";

            case DataIteratorType::RANDOM_ITERABLE_OF_ITERABLES:
                return "Random iterable of iterables";

            case DataIteratorType::RANDOM_ITERABLE_OF_CONST_ITERABLES:
                return "Random iterable of const iterables";

            case DataIteratorType::RANDOM_CONST_ITERABLE_OF_ITERATORS:
                return "Random const iterable of iterators";

            case DataIteratorType::RANDOM_CONST_ITERABLE_OF_ITERABLES:
                return "Random const iterable of iterables";

            case DataIteratorType::RANDOM_CONST_ITERABLE_OF_CONST_ITERABLES:
                return "Random const iterable of const iterables";

            case DataIteratorType::NONE:
            default:
                return "Invalid iterator";
        }*/
    }
};

enum class PiLambdaIteratorType {
    NONE,
    CONTIGUOUS_ITERATOR,
    CONTIGUOUS_ITERABLE,
    RANDOM_ITERATOR,
    RANDOM_ITERABLE
};

class PiLambdaIteratorTypeUtils {

public:
    static std::string getDescription(PiLambdaIteratorType iteratorType) {
        switch (iteratorType) {

            case PiLambdaIteratorType::CONTIGUOUS_ITERATOR:
                return "Contiguous iterator";

            case PiLambdaIteratorType::CONTIGUOUS_ITERABLE:
                return "Contiguous iterable";

            case PiLambdaIteratorType::RANDOM_ITERATOR:
                return "Random iterator";

            case PiLambdaIteratorType::RANDOM_ITERABLE:
                return "Random iterable";

            case PiLambdaIteratorType::NONE:
            default:
                return "Invalid iterator";
        }
    }
};

}  // namespace cluster::utils
#endif  // FINAL_PROJECT_HPC_ITERATORTYPE_H
