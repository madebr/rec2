#include "brstdlib.h"

#include "c2_hooks.h"

#include "brender/br_types.h"

#include "c2_ctype.h"
#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

int C2_HOOK_CDECL BrMemCmp(void* s1, void* s2, size_t n) {
    return c2_memcmp(s1, s2, n);
}
C2_HOOK_FUNCTION(0x0053f030, BrMemCmp)

void* C2_HOOK_CDECL BrMemCpy(void* s1, void* s2, size_t n) {
    return c2_memcpy(s1, s2, n);
}
C2_HOOK_FUNCTION(0x0053f050, BrMemCpy)

void* C2_HOOK_CDECL BrMemSet(void* s, int c, size_t n) {
    if (s == NULL) {
        // Passing a null target pointer is undefined behavior
        return NULL;
    }
    return c2_memset(s, c, n);
}
C2_HOOK_FUNCTION(0x0053f080, BrMemSet)

char* C2_HOOK_CDECL BrStrCat(char* s1, char* s2) {
    return c2_strcat(s1, s2);
}
C2_HOOK_FUNCTION(0x0053f0c0, BrStrCat)

int C2_HOOK_CDECL BrStrCmp(char* s1, char* s2) {
    return c2_strcmp(s1, s2);
}
C2_HOOK_FUNCTION(0x0053f100, BrStrCmp)

int C2_HOOK_CDECL BrStrICmp(char* s1, char* s2) {
    return c2_strcasecmp(s1, s2);
}
C2_HOOK_FUNCTION(0x0053f140, BrStrICmp)

char* C2_HOOK_CDECL BrStrCpy(char* s1, const char* s2) {
    return c2_strcpy(s1, s2);
}
C2_HOOK_FUNCTION(0x0053f160, BrStrCpy)

br_size_t C2_HOOK_CDECL BrStrLen(char* s) {
    return c2_strlen(s);
}
C2_HOOK_FUNCTION(0x0053f190, BrStrLen)

int C2_HOOK_CDECL BrStrNCmp(char* s1, char* s2, size_t n) {
    return c2_strncasecmp(s1, s2, n);
}
C2_HOOK_FUNCTION(0x0053f1b0, BrStrNCmp)

int C2_HOOK_CDECL BrStrNICmp(char* s1, char* s2, size_t n) {
    return c2_strncasecmp(s1, s2, n);
}
C2_HOOK_FUNCTION(0x0053f1d0, BrStrNICmp)

char* C2_HOOK_CDECL BrStrNCpy(char* s1, char* s2, size_t n) {
    return c2_strncpy(s1, s2, n);
}
C2_HOOK_FUNCTION(0x0053f1f0, BrStrNCpy)

char* C2_HOOK_CDECL BrStrRChr(char* s1, char c) {
    return c2_strrchr(s1, c);
}
C2_HOOK_FUNCTION(0x0053f210, BrStrRChr)

void C2_HOOK_CDECL BrAbort(void) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x0053f230, BrAbort);

char* C2_HOOK_CDECL BrGetEnv(char* name) {
    return c2_getenv(name);
}
C2_HOOK_FUNCTION(0x0053f240, BrGetEnv)

float C2_HOOK_CDECL BrStrToF(char* nptr, char** endptr) {
    return c2_strtof(nptr, endptr);
}
C2_HOOK_FUNCTION(0x0053f250, BrStrToF)

double C2_HOOK_CDECL BrStrToD(char* nptr, char** endptr) {
    return c2_strtod(nptr, endptr);
}
C2_HOOK_FUNCTION(0x0053f270, BrStrToD)

long C2_HOOK_CDECL BrStrToL(char* nptr, char** endptr, int base) {
    return c2_strtol(nptr, endptr, base);
}
C2_HOOK_FUNCTION(0x0053f290, BrStrToL)

unsigned long C2_HOOK_CDECL BrStrToUL(char* nptr, char** endptr, int base) {
    return c2_strtoul(nptr, endptr, base);
}
C2_HOOK_FUNCTION(0x0053f2b0, BrStrToUL)

br_boolean C2_HOOK_CDECL BrIsAlpha(int c) {
    return c2_isalpha(c);
}
C2_HOOK_FUNCTION(0x0053f2d0, BrIsAlpha)

br_boolean C2_HOOK_CDECL BrIsDigit(int c) {
    return c2_isdigit(c);
}
C2_HOOK_FUNCTION(0x0053f300, BrIsDigit)

br_boolean C2_HOOK_CDECL BrIsSpace(int c) {
    return c2_isspace(c);
}
C2_HOOK_FUNCTION(0x0053f330, BrIsSpace)

br_boolean C2_HOOK_CDECL BrIsPrint(int c) {
    return c2_isprint(c);
}
C2_HOOK_FUNCTION(0x0053f360, BrIsPrint)

int C2_HOOK_CDECL BrVSprintf(char* buf, const char* fmt, va_list args) {
    return c2_vsprintf(buf, fmt, args);
}
C2_HOOK_FUNCTION(0x0053f390, BrVSprintf)

br_int_32 C2_HOOK_CDECL BrVSprintfN(char* buf, br_size_t buf_size, const char* fmt, va_list args) {
    // Most C libs have vsnprintf these days that can replace this.
    unsigned int n;
    char tmp[512];

    n = c2_vsprintf(tmp, fmt, args);

    if (buf_size - 1 < n) {
        n = buf_size - 1;
    }

    c2_strncpy(buf, tmp, n);
    buf[n] = '\0';
    return n;
}
C2_HOOK_FUNCTION(0x0053f3b0, BrVSprintfN)

br_int_32 C2_HOOK_CDECL BrVSScanf(char* buf, const char* fmt, va_list args) {
    return c2_vsscanf(buf, fmt, args);
}
C2_HOOK_FUNCTION(0x0053f410, BrVSScanf)
