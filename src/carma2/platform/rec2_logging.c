#include "rec2_logging.h"

#include <stdio.h>
#include <stdarg.h>

void C2_HOOK_CDECL rec2_log_warn(const char* fmt, ...) {
    va_list va;

    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}
