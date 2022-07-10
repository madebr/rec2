#ifndef REC2_BRPRINTF_H
#define REC2_BRPRINTF_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_int_32 C2_HOOK_CDECL BrSprintf(char* buf, char* fmt, ...);

br_int_32 C2_HOOK_CDECL BrSprintfN(char* buf, br_size_t buf_size, char* fmt, ...);

int C2_HOOK_CDECL BrLogPrintf(char* fmt, ...);

br_int_32 C2_HOOK_CDECL BrSScanf(char* str, char* fmt, ...);

#endif // REC2_BRPRINTF_H
