#ifndef REC2_DIAG_H
#define REC2_DIAG_H

#include "c2_hooks.h"

void C2_HOOK_CDECL BrFailure (const char* format, ...);

void C2_HOOK_CDECL BrWarning(const char* s, ...);

void C2_HOOK_CDECL BrFatal(const char* name, int line, const char* s, ...);

void C2_HOOK_CDECL _BrAssert(const char* condition, const char* file, unsigned int line);

void C2_HOOK_CDECL _BrUAssert(const char* condition, const char* file, unsigned int line);

#endif // REC2_DIAG_H
