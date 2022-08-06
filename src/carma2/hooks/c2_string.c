#include "c2_string.h"

#if defined(C2_HOOKS_ENABLED)

#include "hooks.h"

#include <string.h>


int C2_HOOK_CDECL c2_memcmp(const void* s1, const void* s2, size_t n) {
    return memcmp(s1, s2, n);
}

int C2_HOOK_CDECL c2_memicmp(const char* buffer1, const char* buffer2, size_t count) {
    return _memicmp(buffer1, buffer2, count);
}
C2_HOOK_FUNCTION(0x00577d20, c2_memicmp)

void* C2_HOOK_CDECL c2_memcpy(void* s1, const void* s2, size_t n) {
    return memcpy(s1, s2, n);
}

void* C2_HOOK_CDECL c2_memmove(void* s1, const void* s2, size_t n) {
    return memmove(s1, s2, n);
}
C2_HOOK_FUNCTION(0x00576080, c2_memmove)

void* C2_HOOK_CDECL c2_memset(void* s1, int c, size_t n) {
    return memset(s1, c, n);
}

char* C2_HOOK_CDECL c2_strcat(char* dest, const char *src) {
    return strcat(dest, src);
}
C2_HOOK_FUNCTION(0x00579a40, c2_strcat)

int C2_HOOK_CDECL c2_strcmp(const char* s1, const char* s2) {
    return strcmp(s1, s2);
}

int C2_HOOK_CDECL c2_strcasecmp(const char* s1, const char* s2) {
#if defined(_WIN32)
    return _stricmp(s1, s2);
#else
    return strcasecmp(s1, s2);
#endif
}

int C2_HOOK_CDECL c2_strncasecmp(const char* s1, const char* s2, size_t n) {
#if defined(_WIN32)
    return _strnicmp(s1, s2, n);
#else
    return strncasecmp(s1, s2, n);
#endif
}

char* C2_HOOK_CDECL c2_strchr(const char* str, int c) {
    return strchr(str, c);
}
C2_HOOK_FUNCTION(0x00575bf0, c2_strchr)

char* C2_HOOK_CDECL c2_strrchr(const char *str, int c) {
    return strrchr(str, c);
}
C2_HOOK_FUNCTION(0x00577970, c2_strrchr)

char* C2_HOOK_CDECL c2_strcpy(char* dest, const char* src) {
    return strcpy(dest, src);
}
C2_HOOK_FUNCTION(0x00579a30, c2_strcpy)

char* C2_HOOK_CDECL c2_strncpy(char *dest, const char *src, size_t n) {
    return strncpy(dest, src, n);
}

char* C2_HOOK_CDECL c2_strstr(const char* str, const char* delim) {
    return strstr(str, delim);
}
C2_HOOK_FUNCTION(0x005767d0, c2_strstr)

char* C2_HOOK_CDECL c2_strtok(char* str, const char* delim) {
    return strtok(str, delim);
}
C2_HOOK_FUNCTION(0x00575d00, c2_strtok)
#endif

size_t C2_HOOK_CDECL c2_strlen(const char* str) {
    return strlen(str);
}

char* C2_HOOK_CDECL c2_strpbrk(const char* dest, const char* breakset) {
    return strpbrk(dest, breakset);
}
C2_HOOK_FUNCTION(0x00581110, c2_strpbrk)
