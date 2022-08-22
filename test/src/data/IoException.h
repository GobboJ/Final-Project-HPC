#ifndef FINAL_PROJECT_HPC_IOEXCEPTION_H
#define FINAL_PROJECT_HPC_IOEXCEPTION_H

#include <stdexcept>

namespace cluster::test::data {

/**
 * Exception thrown when an error occurs while performing an I/O operation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-22
 * @since 1.0
 */
class IOException : public std::runtime_error {

public:
    /**
     * Creates a new exception, with the specified message, thrown when an error occurs while
     * performing an I/O operation.
     *
     * @param message Message of the exception.
     */
    explicit IOException(const char *message);

    /**
     * Creates a new exception, with the specified message, thrown when an error occurs while
     * performing an I/O operation.
     *
     * @param message Message of the exception.
     */
    explicit IOException(const std::string &message);
};

}  // namespace cluster::test::data

#endif  // FINAL_PROJECT_HPC_IOEXCEPTION_H
