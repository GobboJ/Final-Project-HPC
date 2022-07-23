#ifndef FINAL_PROJECT_HPC_MALFORMEDFILEEXCEPTION_H
#define FINAL_PROJECT_HPC_MALFORMEDFILEEXCEPTION_H

#include <stdexcept>

/**
 * Exception thrown when the input data file does not follow the format.
 *
 * @author DeB, Jonathan
 * @version 1.0 2022-07-18
 * @since 1.0
 */
class MalformedFileException : public std::runtime_error {

public:
    /**
     * Creates the exception with the specified error message.
     *
     * @param string The error message.
     */
    MalformedFileException(const char *string) noexcept;

    /**
     * Creates the exception with the specified error message.
     *
     * @param string The error message.
     */
    MalformedFileException(const std::string &string) noexcept;
};

#endif  // FINAL_PROJECT_HPC_MALFORMEDFILEEXCEPTION_H
