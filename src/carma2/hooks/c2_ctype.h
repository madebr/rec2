#ifndef C2_CTYPE_H
#define C2_CTYPE_H

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

int C2_HOOK_CDECL c2_tolower(int character);

int C2_HOOK_CDECL c2_toupper(int character);

#else

#include <ctype.h>

#define c2_tolower tolower

#define c2_toupper toupper

#endif

#endif // C2_CTYPE_H
