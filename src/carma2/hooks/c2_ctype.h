#ifndef C2_CTYPE_H
#define C2_CTYPE_H

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

int C2_HOOK_CDECL c2_tolower(int character);

int C2_HOOK_CDECL c2_toupper(int character);

int C2_HOOK_CDECL c2_isalpha(int c);

int C2_HOOK_CDECL c2_isdigit(int c);

int C2_HOOK_CDECL c2_isspace(int c);

int C2_HOOK_CDECL c2_isprint(int c);

#else

#include <ctype.h>

#define c2_tolower tolower

#define c2_toupper toupper

#define c2_isalpha isalpha

#define c2_isdigit isdigit

#define c2_isspace isspace

#define c2_isprint isprint

#endif

#endif // C2_CTYPE_H
