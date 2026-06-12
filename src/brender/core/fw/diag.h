#ifndef REC2_DIAG_H
#define REC2_DIAG_H

#include "c2_hooks.h"

extern C2_NORETURN void C2_HOOK_CDECL BrFailure(const char* format, ...);

extern void C2_HOOK_CDECL BrWarning(const char* s, ...);

extern void C2_HOOK_CDECL BrFatal(const char* name, int line, const char* s, ...);

extern void C2_HOOK_CDECL _BrAssert(const char* condition, const char* file, unsigned int line);

extern void C2_HOOK_CDECL _BrUAssert(const char* condition, const char* file, unsigned int line);

#endif // REC2_DIAG_H
