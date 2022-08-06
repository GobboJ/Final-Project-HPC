#ifndef FINAL_PROJECT_HPC_MALFORMEDFILEEXCEPTION_H
#define FINAL_PROJECT_HPC_MALFORMEDFILEEXCEPTION_H

#include <stdexcept>

namespace cluster::test::data {
/**
 * Exception thrown when the input data file does not follow the correct format.
 *
 * @author DeB, Jonathan
 * @version 1.1 2022-08-06
 * @since 1.0
 */
class MalformedFileException : public std::runtime_error {

public:
    /**
     * Creates a new exception, with the specified message, thrown when the input data file does not
     * follow the correct format.
     *
     * @param message Message of the exception.
     */
    explicit MalformedFileException(const char *message) noexcept;

    /**
     * Creates a new exception, with the specified message, thrown when the input data file does
     * not follow the correct format.
     *
     * @param message Message of the exception.
     */
    explicit MalformedFileException(const std::string &message) noexcept;
};
}  // namespace cluster::test::data

#endif  // FINAL_PROJECT_HPC_MALFORMEDFILEEXCEPTION_H
