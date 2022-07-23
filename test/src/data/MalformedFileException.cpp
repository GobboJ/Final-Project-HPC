/*
 * MalformedException implementation.
 *
 * @author DeB, Jonathan
 * @version 1.0 2022-07-18
 * @since 1.0
 */
#include "MalformedFileException.h"

/**
 * Creates the exception with the specified error message.
 *
 * @param string The error message.
 */
MalformedFileException::MalformedFileException(const char *const string) noexcept :
    runtime_error(string) {
}

/**
 * Creates the exception with the specified error message.
 *
 * @param string The error message.
 */
MalformedFileException::MalformedFileException(const std::string &string) noexcept :
    runtime_error(string) {
}
