/*
 * IOException implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.1 2022-08-22
 * @since 1.0
 */
#include "IoException.h"

namespace cluster::test::data {

/**
 * Creates a new exception with the specified message.
 *
 * @param message Message of the exception.
 */
IOException::IOException(const char *const message) : runtime_error(message) {
}

/**
 * Creates a new exception with the specified message.
 *
 * @param message Message of the exception.
 */
IOException::IOException(const std::string &message) : runtime_error(message) {
}

}  // namespace cluster::test::data
