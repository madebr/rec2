#ifndef REC2_STDIO_H
#define REC2_STDIO_H

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#include <stdarg.h>

#ifndef FILE
#define FILE void*
#endif

FILE* C2_HOOK_CDECL c2_fopen(const char* path, const char* mode);

int C2_HOOK_CDECL c2_ftell(FILE* file);

int c2_fputs(const char* text, FILE* file);

int C2_HOOK_CDECL c2_fprintf(FILE* file, const char* format, ...);
int C2_HOOK_CDECL c2_sprintf(char* str, const char* format, ...);
int C2_HOOK_CDECL c2_vfprintf(FILE* file, const char* format, va_list ap);

#else

#include <stdio.h>

#define c2_sprintf sprintf

#endif

#endif //REC2_STDIO_H
