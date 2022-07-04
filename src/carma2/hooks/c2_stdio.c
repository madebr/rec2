#include "c2_stdio.h"

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#include <stdarg.h>
#include <stdio.h>

C2_HOOK_VARIABLE_IMPLEMENT(FILE, c2_stdout_value, 0x006737f0);

static FILE* (C2_HOOK_CDECL * fopen_original)(const char*, const char*);
FILE*  c2_fopen(const char* path, const char* mode) {
    c2_printf("c2_fopen(%s,%s)\n", path, mode);
    c2_fflush(c2_stdout);
    return fopen_original(path, mode);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577170, c2_fopen, fopen_original)

static int(C2_HOOK_CDECL * fflush_original)(FILE* file);
int C2_HOOK_CDECL c2_fflush(FILE* file) {
    return fflush_original(file);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577700, c2_fflush, fflush_original)

int (C2_HOOK_CDECL * ftell_original)(FILE*);
int c2_ftell(FILE* file) {
    return ftell_original(file);
}
C2_HOOK_FUNCTION_ORIGINAL(0x005772f0, c2_ftell, ftell_original)

static int (C2_HOOK_CDECL * fputc_original)(int character, FILE* file);
int c2_fputc(int character, FILE* file) {
    return fputc_original(character, file);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577c40, c2_fputc, fputc_original);

static int (C2_HOOK_CDECL * fputs_original)(const char*, FILE*);
int c2_fputs(const char* text, FILE* file) {
    return fputs_original(text, file);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577ef0, c2_fputs, fputs_original);

static int (C2_HOOK_CDECL * printf_original)(const char* format, ...);
int c2_printf(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    return c2_vfprintf(c2_stdout, format, ap);
    va_end(ap);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577930, c2_printf, printf_original);

static int (C2_HOOK_CDECL * fprintf_original)(FILE*, const char* format, ...);
int c2_fprintf(FILE* file, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    return c2_vfprintf(file, format, ap);
    va_end(ap);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576d00, c2_fprintf, fprintf_original);

int c2_sprintf(char* str, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    c2_printf("c2_sprintf(%s, format)\n", str, format);
    return vsprintf(str, format, ap);
    va_end(ap);
}
C2_HOOK_FUNCTION(0x00575de0, c2_sprintf)

int (C2_HOOK_CDECL * vfprintf_original)(FILE*, const char*, va_list ap);
int c2_vfprintf(FILE* file, const char* format, va_list ap) {
    return vfprintf_original(file, format, ap);
}
C2_HOOK_FUNCTION_ORIGINAL(0x0057b800, c2_vfprintf, vfprintf_original);

size_t (C2_HOOK_CDECL * fwrite_original)(const void* ptr, size_t size, size_t count, FILE* stream);
size_t c2_fwrite(const void* ptr, size_t size, size_t count, FILE* stream) {
    return fwrite_original(ptr, size, count, stream);
}
C2_HOOK_FUNCTION_ORIGINAL(0x005774a0, c2_fwrite, fwrite_original)

#endif
