#ifndef __LIB_DEBUG_H
#define __LIB_DEBUG_H

#include <assert.h>

/* GCC lets us add "attributes" to functions, function
   parameters, etc. to indicate their properties.
   See the GCC manual for details. */
#define UNUSED __attribute__ ((unused))

#define ASSERT(...) assert(__VA_ARGS__)
#define PANIC(...) assert(false)

#endif
