#ifndef GUARD_52_ERRORS_H
#define GUARD_52_ERRORS_H

#include "c2_hooks.h"

extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);

extern void C2_HOOK_FASTCALL CloseDiagnostics(void);

extern void C2_HOOK_FASTCALL OpenDiagnostics(void);

extern void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);

#endif // GUARD_52_ERRORS_H
