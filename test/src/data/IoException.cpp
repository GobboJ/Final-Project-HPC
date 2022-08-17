/*
 * IOException implementation.
 *
 * @author DeB
 * @author Jonathan
 * @version 1.0 2022-08-06
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

}  // namespace cluster::test::data
