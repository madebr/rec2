#ifndef C2_STDLIB_H
#define C2_STDLIB_H

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

void C2_HOOK_CDECL c2_srand(int seed);

void C2_HOOK_CDECL c2_abort(void);

#else

#include <stdlib.h>

#define c2_srand srand

#define c2_abort abort

#endif

#endif // C2_STDLIB_H
