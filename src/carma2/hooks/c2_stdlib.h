#ifndef C2_STDLIB_H
#define C2_STDLIB_H

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

void C2_HOOK_CDECL c2_srand(int seed);

void C2_HOOK_CDECL c2_abort(void);

void* C2_HOOK_CDECL c2_malloc(size_t size);

void* C2_HOOK_CDECL c2_calloc (size_t num, size_t size);

void C2_HOOK_CDECL c2_free(void* ptr);

char* C2_HOOK_CDECL c2_getenv(const char* name);

void C2_HOOK_CDECL c2_qsort(void *base, size_t nitems, size_t size, int (C2_HOOK_CDECL*compar)(const void *, const void*));

float C2_HOOK_CDECL c2_strtof(const char* str, char** endptr);

double C2_HOOK_CDECL c2_strtod(const char* str, char** endptr);

long int C2_HOOK_CDECL c2_strtol(const char* str, char** endptr, int base);

unsigned long int C2_HOOK_CDECL c2_strtoul(const char* str, char** endptr, int base);

void C2_HOOK_CDECL c2_exit(int status);

#else

#include <stdlib.h>

#define c2_srand srand

#define c2_abort abort

#define c2_malloc malloc

#define c2_calloc calloc

#define c2_free free

#define c2_getenv getenv

#define c2_strtof strtof

#define c2_strtod strtod

#define c2_strtol strtol

#define c2_strtoul strtoul

#define c2_exit exit

#endif

#endif // C2_STDLIB_H
