#pragma once

#include <assert.h>
/* --------------------------------- START ---------------------------------- */
/* OR [BEST], for **any version of C OR C++**: */

/* See: https://stackoverflow.com/a/71899854/4561887 */
#define CONCAT_(prefix, suffix) prefix##suffix
/* Concatenate `prefix, suffix` into `prefixsuffix` */
#define CONCAT(prefix, suffix) CONCAT_(prefix, suffix)
/* Make a unique variable name containing the line number at the end of the */
/* name. Ex: `uint64_t MAKE_UNIQUE_VARIABLE_NAME(counter) = 0;` would */
/* produce `uint64_t counter_7 = 0` if the call is on line 7! */
#define MAKE_UNIQUE_VARIABLE_NAME(prefix) CONCAT(prefix##_, __LINE__)

/* Static assert hack required for **pre-C++11**, such as C++98, C++03, etc. */
/* - It works only with C++, NOT with C! */
/* See: */
/* 1. [my ans with this] https://stackoverflow.com/a/54993033/4561887 */
/* 1. Info. on `_Pragma()`: https://stackoverflow.com/a/47518775/4561887 */
/* 1. The inspiration for this `typedef char` array hack as a struct  */
/*    definition: https://stackoverflow.com/a/3385694/4561887 */
/* Discard the `message` portion entirely. */
#define _Static_assert_hack(expression, message) \
    struct MAKE_UNIQUE_VARIABLE_NAME(static_assertion_failed) \
    { \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wunused-local-typedefs\"") \
        typedef char static_assertion_failed[(expression) ? 1 : -1]; \
        _Pragma("GCC diagnostic pop") \
    }

/* For C++ only: */
/* See: https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html */
#ifdef __cplusplus
    #if __cplusplus < 201103L
        /* for pre-C++11 */
        #ifndef _Static_assert
            #define _Static_assert _Static_assert_hack
        #endif
    #else
        /* for C++11 or later */
        #ifndef _Static_assert
            #define _Static_assert static_assert
        #endif
    #endif
#endif

/* For C **and** C++: */
#define STATIC_ASSERT(test_for_true) \
    _Static_assert((test_for_true), "(" #test_for_true ") failed")
/* ---------------------------------- END ----------------------------------- */
