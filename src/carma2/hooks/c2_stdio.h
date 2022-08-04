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
void C2_HOOK_CDECL c2_fclose(FILE* file);
int C2_HOOK_CDECL c2_feof(FILE* file);

int C2_HOOK_CDECL c2_fflush(FILE* file);

int C2_HOOK_CDECL c2_ftell(FILE* file);
int C2_HOOK_CDECL c2_fseek(FILE *stream, int offset, int whence);
void C2_HOOK_CDECL c2_rewind(FILE *stream);

int C2_HOOK_CDECL c2_fgetc(FILE * stream);
char* C2_HOOK_CDECL c2_fgets(char* str, int num, FILE* stream);
int C2_HOOK_CDECL c2_fputc(int character, FILE* file);
int C2_HOOK_CDECL c2_fputs(const char* text, FILE* file);

int C2_HOOK_CDECL c2_printf(const char* format, ...);
int C2_HOOK_CDECL c2_fprintf(FILE* file, const char* format, ...);
int C2_HOOK_CDECL c2_vfprintf(FILE* file, const char* format, va_list ap);

int C2_HOOK_CDECL c2_sprintf(char* str, const char* format, ...);
int C2_HOOK_CDECL c2_vsprintf(char* str, const char* format, va_list ap);

size_t C2_HOOK_CDECL c2_fwrite(const void* ptr, size_t size, size_t count, FILE* stream);
size_t C2_HOOK_CDECL c2_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

int C2_HOOK_CDECL c2_fscanf(FILE* file, const char* format, ...);
int C2_HOOK_CDECL c2_vfscanf(FILE* file, const char* format, va_list ap);

int C2_HOOK_CDECL c2_sscanf(const char* str, const char* format, ...);
int C2_HOOK_CDECL c2_vsscanf(const char* str, const char* format, va_list ap);

#else

#include <stdio.h>

#define c2_fopen fopen
#define c2_fclose fclose
#define c2_feof feof

#define c2_fflush fflush

#define c2_ftell ftell
#define c2_fseek fseek
#define c2_rewind rewind

#define c2_fgetc fgetc
#define c2_fgets fgets
#define c2_fputc fpuc
#define c2_fputs fputs

#define c2_printf printf
#define c2_fprintf fprintf
#define c2_vfprintf vfprintf

#define c2_sprintf sprintf
#define c2_vsprintf vsprintf

#define c2_fwrite fwrite
#define c2_fread fread

#define c2_sscanf sscanf
#define c2_vsscanf vsscanf

#endif

#endif //REC2_STDIO_H
