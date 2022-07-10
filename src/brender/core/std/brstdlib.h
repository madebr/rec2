#ifndef REC2_BRSTDLIB_H
#define REC2_BRSTDLIB_H

#include "c2_hooks.h"

#include "brender/br_types.h"

#include <stdarg.h>

int C2_HOOK_CDECL BrMemCmp(void* s1, void* s2, size_t n);

void* C2_HOOK_CDECL BrMemCpy(void* s1, void* s2, size_t n);

void* C2_HOOK_CDECL BrMemSet(void* s, int c, size_t n);

char* C2_HOOK_CDECL BrStrCat(char* s1, const char* s2);

int C2_HOOK_CDECL BrStrCmp(const char* s1, const char* s2);

int C2_HOOK_CDECL BrStrICmp(const char* s1, const char* s2);

char* C2_HOOK_CDECL BrStrCpy(char* s1, const char* s2);

br_size_t C2_HOOK_CDECL BrStrLen(const char* s);

int C2_HOOK_CDECL BrStrNCmp(const char* s1, const char* s2, size_t n);

int C2_HOOK_CDECL BrStrNICmp(const char* s1, const char* s2, size_t n);

char* C2_HOOK_CDECL BrStrNCpy(char* s1, const char* s2, size_t n);

char* C2_HOOK_CDECL BrStrRChr(const char* s1, char c);

void C2_HOOK_CDECL BrAbort(void);

char* C2_HOOK_CDECL BrGetEnv(char* name);

float C2_HOOK_CDECL BrStrToF(char* nptr, char** endptr);

double C2_HOOK_CDECL BrStrToD(char* nptr, char** endptr);

long C2_HOOK_CDECL BrStrToL(char* nptr, char** endptr, int base);

unsigned long C2_HOOK_CDECL BrStrToUL(char* nptr, char** endptr, int base);

br_boolean C2_HOOK_CDECL BrIsAlpha(int c);

br_boolean C2_HOOK_CDECL BrIsDigit(int c);

br_boolean C2_HOOK_CDECL BrIsSpace(int c);

br_boolean C2_HOOK_CDECL BrIsPrint(int c);

br_int_32 C2_HOOK_CDECL BrVSprintf(char* buf, const char* fmt, va_list args);

br_int_32 C2_HOOK_CDECL BrVSprintfN(char* buf, br_size_t buf_size, const char* fmt, va_list args);

br_int_32 C2_HOOK_CDECL BrVSScanf(char* buf, const char* fmt, va_list args);

#endif // REC2_BRSTDLIB_H
