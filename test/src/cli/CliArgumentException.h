#ifndef FINAL_PROJECT_HPC_CLIARGUMENTEXCEPTION_H
#define FINAL_PROJECT_HPC_CLIARGUMENTEXCEPTION_H

#include <stdexcept>

namespace cluster::test::cli {
/**
 * Exception thrown when one of the arguments specified via command line is incorrect.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-08-05
 * @since 1.0
 */
class CliArgumentException : public std::runtime_error {

public:
    /**
     * Creates a new exception, with the specified message, thrown when one of the arguments
     * specified via command line is incorrect.
     *
     * @param message Message of the exception.
     */
    explicit CliArgumentException(const char* message);

    /**
     * Creates a new exception, with the specified message, thrown when one of the arguments
     * specified via command line is incorrect.
     *
     * @param message Message of the exception.
     */
    explicit CliArgumentException(const std::string& message);
};
}  // namespace cluster::test::cli
#endif  // FINAL_PROJECT_HPC_CLIARGUMENTEXCEPTION_H
