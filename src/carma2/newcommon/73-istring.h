#ifndef GUARD_72_INTERFACE_H
#define GUARD_72_INTERFACE_H

#include "c2_hooks.h"

extern void C2_HOOK_FASTCALL IString_Load(void);

extern void C2_HOOK_FASTCALL IString_Free(void);

extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);

#endif // GUARD_73_ISTRING
