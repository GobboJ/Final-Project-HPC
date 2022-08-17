/*
 * CliArgumentsException implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-08-05
 * @since 1.0
 */
#include "CliArgumentException.h"

namespace cluster::test::cli {

/**
 * Creates a new exception with the specified message.
 *
 * @param message Message of the exception.
 */
CliArgumentException::CliArgumentException(const char* message) : runtime_error(message) {
}

/**
 * Creates a new exception with the specified message.
 *
 * @param message Message of the exception.
 */
CliArgumentException::CliArgumentException(const std::string& message) : runtime_error(message) {
}

}  // namespace cluster::test::cli
