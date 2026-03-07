#ifndef C2_STDLIB_H
#define C2_STDLIB_H

#include <stdlib.h>

#define c2_srand srand

#define c2_rand rand

#define c2_rand rand

#define c2_abort abort

#define c2_malloc malloc

#define c2_calloc calloc

#define c2_free free

#define c2_getenv getenv

#define c2_qsort qsort

#if defined(_MSC_VER) && _MSC_VER < 1300
#define c2_strtof(V,X) (float)strtod(V,X)
#else
#define c2_strtof strtof
#endif

#define c2_strtod strtod

#define c2_strtol strtol

#define c2_strtoul strtoul

#define c2_exit exit

#define c2_atoi atoi

#define c2_atof atof

#endif // C2_STDLIB_H
