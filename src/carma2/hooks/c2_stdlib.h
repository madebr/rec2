#ifndef C2_STDLIB_H
#define C2_STDLIB_H

#if defined(C2_HOOKS_ENABLED)

#ifndef NULL
#define NULL ((void*)0)
#endif

#else

#include <stdlib.h>

#endif

#endif // C2_STDLIB_H
