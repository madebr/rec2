#include "c2_stdio.h"

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#include <stdio.h>

static FILE* (* C2_HOOK_CDECL fopen_original)(const char*, const char*);
FILE*  c2_fopen(const char* path, const char* mode) {
    return fopen_original(path, mode);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577170, c2_fopen, fopen_original)

int (* C2_HOOK_CDECL ftell_original)(FILE*);
int c2_ftell(FILE* file) {
    return ftell_original(file);
}
C2_HOOK_FUNCTION_ORIGINAL(0x005772f0, c2_fprintf, ftell_original)

static int (* C2_HOOK_CDECL fputs_original)(const char*, FILE*);
int c2_fputs(const char* text, FILE* file) {
    return fputs_original(file, text);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577ef0, c2_fputs, fputs_original);

static int (* C2_HOOK_CDECL fputs_original)(const char*, FILE*);
int c2_fputs(const char* text, FILE* file) {
    return fputs_original(file, text);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577ef0, c2_fputs, fputs_original);

static int (* C2_HOOK_CDECL fprintf_original)(FILE*, const char* format, ...);
int c2_fprintf(FILE* file, const char* format, ...) {
    va_list ap;
    va_begin(format, ap);
    return c2_vfprintf(file, format, ap);
    va_end(ap);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576d00, c2_fprintf, fprintf_original);

int c2_sprintf(char* str, char* format, ...) {
    va_list ap;
    va_begin(format, ap);
    return vsprintf(str, format, ap);
    va_end(ap);
}
C2_HOOK_FUNCTION(0x00575de0, c2_sprintf)

int (* C2_HOOK_CDECL vfprintf_original)(FILE*, const char*, va_list ap);
int c2_vfprintf(FILE* file, const char* format, va_list ap) {
    return vfprintf_original(file, format, ap);
}
C2_HOOK_FUNCTION_ORIGINAL(0x0057b800, c2_vfprintf, vfprintf_original);

#endif
