#include "brstdlib.h"

#include "c2_hooks.h"

#include "brender/br_types.h"

#include "c2_ctype.h"
#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

// FUNCTION: CARMA2_HW 0x0053f030
int C2_HOOK_CDECL BrMemCmp(const void* s1, const void* s2, size_t n) {
    return c2_memcmp(s1, s2, n);
}

// FUNCTION: CARMA2_HW 0x0053f050
void* C2_HOOK_CDECL BrMemCpy(void* s1, const void* s2, size_t n) {
    return c2_memcpy(s1, s2, n);
}

// FUNCTION: CARMA2_HW 0x0053f080
void* C2_HOOK_CDECL BrMemSet(void* s, int c, size_t n) {
    if (s == NULL) {
        // Passing a null target pointer is undefined behavior
        return NULL;
    }
    return c2_memset(s, c, n);
}

// FUNCTION: CARMA2_HW 0x0053f0c0
char* C2_HOOK_CDECL BrStrCat(char* s1, const char* s2) {
    return c2_strcat(s1, s2);
}

// FUNCTION: CARMA2_HW 0x0053f100
int C2_HOOK_CDECL BrStrCmp(const char* s1, const char* s2) {
    return c2_strcmp(s1, s2);
}

// FUNCTION: CARMA2_HW 0x0053f140
int C2_HOOK_CDECL BrStrICmp(const char* s1, const char* s2) {
    return c2_strcasecmp(s1, s2);
}

// FUNCTION: CARMA2_HW 0x0053f160
char* C2_HOOK_CDECL BrStrCpy(char* s1, const char* s2) {
    return c2_strcpy(s1, s2);
}

// FUNCTION: CARMA2_HW 0x0053f190
br_size_t C2_HOOK_CDECL BrStrLen(const char* s) {
    return c2_strlen(s);
}

// FUNCTION: CARMA2_HW 0x0053f1b0
int C2_HOOK_CDECL BrStrNCmp(const char* s1, const char* s2, size_t n) {
    return c2_strncasecmp(s1, s2, n);
}

// FUNCTION: CARMA2_HW 0x0053f1d0
int C2_HOOK_CDECL BrStrNICmp(const char* s1, const char* s2, size_t n) {
    return c2_strncasecmp(s1, s2, n);
}

// FUNCTION: CARMA2_HW 0x0053f1f0
char* C2_HOOK_CDECL BrStrNCpy(char* s1, const char* s2, size_t n) {
    return c2_strncpy(s1, s2, n);
}

// FUNCTION: CARMA2_HW 0x0053f210
char* C2_HOOK_CDECL BrStrRChr(const char* s1, char c) {
    return c2_strrchr(s1, c);
}

// FUNCTION: CARMA2_HW 0x0053f230
void C2_HOOK_CDECL BrAbort(void) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x0053f240
char* C2_HOOK_CDECL BrGetEnv(const char* name) {
    return c2_getenv(name);
}

// FUNCTION: CARMA2_HW 0x0053f250
float C2_HOOK_CDECL BrStrToF(const char* nptr, char** endptr) {
    return c2_strtof(nptr, endptr);
}

// FUNCTION: CARMA2_HW 0x0053f270
double C2_HOOK_CDECL BrStrToD(const char* nptr, char** endptr) {
    return c2_strtod(nptr, endptr);
}

// FUNCTION: CARMA2_HW 0x0053f290
long C2_HOOK_CDECL BrStrToL(const char* nptr, char** endptr, int base) {
    return c2_strtol(nptr, endptr, base);
}

// FUNCTION: CARMA2_HW 0x0053f2b0
unsigned long C2_HOOK_CDECL BrStrToUL(const char* nptr, char** endptr, int base) {
    return c2_strtoul(nptr, endptr, base);
}

// FUNCTION: CARMA2_HW 0x0053f2d0
br_boolean C2_HOOK_CDECL BrIsAlpha(int c) {
    return c2_isalpha(c);
}

// FUNCTION: CARMA2_HW 0x0053f300
br_boolean C2_HOOK_CDECL BrIsDigit(int c) {
    return c2_isdigit(c);
}

// FUNCTION: CARMA2_HW 0x0053f330
br_boolean C2_HOOK_CDECL BrIsSpace(int c) {
    return c2_isspace(c);
}

// FUNCTION: CARMA2_HW 0x0053f360
br_boolean C2_HOOK_CDECL BrIsPrint(int c) {
    return c2_isprint(c);
}

// FUNCTION: CARMA2_HW 0x0053f390
int C2_HOOK_CDECL BrVSprintf(char* buf, const char* fmt, va_list args) {
    return c2_vsprintf(buf, fmt, args);
}

// FUNCTION: CARMA2_HW 0x0053f3b0
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

// FUNCTION: CARMA2_HW 0x0053f410
br_int_32 C2_HOOK_CDECL BrVSScanf(char* buf, const char* fmt, va_list args) {

#ifdef REC2_MATCHING
    int i;
    unsigned long arg_block[16];

    for(i=0; i < 16; i++)
        arg_block[i] = va_arg(args,unsigned long);

    return c2_sscanf(buf, fmt,
        arg_block[0],
        arg_block[1],
        arg_block[2],
        arg_block[3],
        arg_block[4],
        arg_block[5],
        arg_block[6],
        arg_block[7],
        arg_block[8],
        arg_block[9],
        arg_block[10],
        arg_block[11],
        arg_block[12],
        arg_block[13],
        arg_block[14],
        arg_block[15]);
#else
    return c2_vsscanf(buf, fmt, args);
#endif
}
