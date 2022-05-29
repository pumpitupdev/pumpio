#ifndef PUMPIO_RESULT_H
#define PUMPIO_RESULT_H

#include <errno.h>

#define RESULT_SUCCESS 0
#define RESULT_IS_SUCCESS(x) (x == RESULT_SUCCESS)
#define RESULT_IS_ERROR(x) (x > 0)

/**
 * Reflects a result/an error value.
 *
 * 0 = success
 * > 0 = Error as defined by errno.h
 *
 * Any utility function, e.g. strerror, that is comptaible with errno is
 * compatible with this "type".
 */
typedef uint32_t result_t;

#endif