#ifndef C2_CTYPE_H
#define C2_CTYPE_H

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

int C2_HOOK_CDECL c2_toupper(int character);

#else

#include <ctype.h>

#endif

#endif // C2_CTYPE_H
