#ifndef REC2_STDIO_H
#define REC2_STDIO_H

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#include <stdio.h>

C2_HOOK_VARIABLE_DECLARE(FILE, c2_stdin_value);
C2_HOOK_VARIABLE_DECLARE(FILE, c2_stdout_value);
C2_HOOK_VARIABLE_DECLARE(FILE, c2_stderr_value);

#define c2_stdin (&C2V(c2_stdin_value))
#define c2_stdout (&C2V(c2_stdout_value))
#define c2_stderr (&C2V(c2_stderr_value))

FILE* C2_HOOK_CDECL c2_fopen(const char* path, const char* mode);
int C2_HOOK_CDECL c2_fflush(FILE* file);

int C2_HOOK_CDECL c2_ftell(FILE* file);

int C2_HOOK_CDECL c2_fputc(int character, FILE* file);
int C2_HOOK_CDECL c2_fputs(const char* text, FILE* file);

int C2_HOOK_CDECL c2_printf(const char* format, ...);
int C2_HOOK_CDECL c2_fprintf(FILE* file, const char* format, ...);
int C2_HOOK_CDECL c2_sprintf(char* str, const char* format, ...);
int C2_HOOK_CDECL c2_vfprintf(FILE* file, const char* format, va_list ap);

size_t C2_HOOK_CDECL c2_fwrite(const void* ptr, size_t size, size_t count, FILE* stream);

#else

#include <stdio.h>

#define c2_sprintf sprintf

#endif

#endif //REC2_STDIO_H
