#ifndef C2_DIRECT_H
#define C2_DIRECT_H

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

int C2_HOOK_CDECL c2_mkdir(const char* path);

#else

#include <direct.h>

#define c2_mkdir _mkdir

#endif

#endif // C2_DIRECT_H
