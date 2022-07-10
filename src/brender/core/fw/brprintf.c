#include "brprintf.h"

#include "scratch.h"

#include "core/std/brstdlib.h"
#include "core/std/logwrite.h"

#include "c2_stdio.h"

#include <stdarg.h>

br_int_32 C2_HOOK_CDECL BrSprintf(char* buf, char* fmt, ...) {
    int n;
    va_list args;

    va_start(args, fmt);
    n = BrVSprintf(buf, fmt, args);
    va_end(args);
    return n;
}
C2_HOOK_FUNCTION(0x0052cfc0, BrSprintf)

br_int_32 C2_HOOK_CDECL BrSprintfN(char* buf, br_size_t buf_size, char* fmt, ...) {
    int n;
    va_list args;

    va_start(args, fmt);
    n = vsnprintf(buf, buf_size, fmt, args);
    va_end(args);
    return n;
}
C2_HOOK_FUNCTION(0x0052cfe0, BrSprintfN)

int C2_HOOK_CDECL BrLogPrintf(char* fmt, ...) {
    int n;
    va_list args;

    va_start(args, fmt);
    n = BrVSprintf(BrScratchString(), fmt, args);
    va_end(args);
    BrLogWrite(BrScratchString(), 1, n);

    return n;
}
C2_HOOK_FUNCTION(0x0052d000, BrLogPrintf)

br_int_32 C2_HOOK_CDECL BrSScanf(char* str, char* fmt, ...) {
    int n;
    va_list args;

    va_start(args, fmt);
    n = vsscanf(str, fmt, args);
    va_end(args);
    return n;
}
C2_HOOK_FUNCTION(0x0052d030, BrSScanf)
