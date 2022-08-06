/*
 * MalformedException implementation.
 *
 * @author DeB, Jonathan
 * @version 1.1 2022-08-06
 * @since 1.0
 */
#include "MalformedFileException.h"

namespace cluster::test::data {
/**
 * Creates a new exception with the specified message.
 *
 * @param message The error message.
 */
MalformedFileException::MalformedFileException(const char *const message) noexcept :
    runtime_error(message) {
}

/**
 * Creates a new exception with the specified message.
 *
 * @param message The error message.
 */
MalformedFileException::MalformedFileException(const std::string &message) noexcept :
    runtime_error(message) {
}
}  // namespace cluster::test::data
