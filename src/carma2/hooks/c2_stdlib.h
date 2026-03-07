#ifndef C2_STDLIB_H
#define C2_STDLIB_H

#include <stdlib.h>

#if defined(_MSC_VER) && _MSC_VER < 1300
#define c2_strtof(V,X) (float)strtod(V,X)
#else
#define c2_strtof strtof
#endif

#endif // C2_STDLIB_H
