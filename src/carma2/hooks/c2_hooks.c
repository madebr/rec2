#include "c2_hooks.h"

#include <stdarg.h>
#include <stdio.h>

static FILE* gHookFile;

void c2_hook_init(void) {
    gHookFile = fopen("hooks.txt", "w");
}

void c2_hook_deinit(void) {
    fclose(gHookFile);
    gHookFile = NULL;
}

void c2_hook_debugf(const char* function, const char* format, ...) {
    va_list ap;

    va_start(ap, format);
    c2_hook_vdebugf(function, format, ap);
    va_end(ap);
}

void c2_hook_vdebugf(const char* function, const char* format, va_list ap) {
    fputs(function, gHookFile);
    fputs(": ", gHookFile);
    vfprintf(gHookFile, format, ap);
    fputc('\n', gHookFile);
    fflush(gHookFile);
}
