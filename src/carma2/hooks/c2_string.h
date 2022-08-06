#ifndef REC2_STRING_H_
#define REC2_STRING_H_

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#include "c2_stdlib.h"

int C2_HOOK_CDECL c2_memcmp(const void* s1, const void* s2, size_t n);
void* C2_HOOK_CDECL c2_memcpy(void* s1, const void* s2, size_t n);
void* C2_HOOK_CDECL c2_memmove(void* s1, const void* s2, size_t n);
void* C2_HOOK_CDECL c2_memset(void* s1, int c, size_t n);

char* C2_HOOK_CDECL c2_strcat(char *dest, const char *src);

int C2_HOOK_CDECL c2_strcmp(const char* s1, const char* s2);
int C2_HOOK_CDECL c2_strcasecmp(const char* s1, const char* s2);
int C2_HOOK_CDECL c2_strncasecmp(const char* s1, const char* s2, size_t n);

char* C2_HOOK_CDECL c2_strcpy(char *dest, const char *src);
char* C2_HOOK_CDECL c2_strncpy(char *dest, const char *src, size_t n);
size_t C2_HOOK_CDECL c2_strlen(const char *dest);

char* C2_HOOK_CDECL c2_strchr(const char *str, int c);
char* C2_HOOK_CDECL c2_strrchr(const char *str, int c);
char* C2_HOOK_CDECL c2_strstr(const char *str, const char *delim);
char* C2_HOOK_CDECL c2_strtok(char* str, const char *delim);
char* C2_HOOK_CDECL c2_strpbrk(const char* dest, const char* breakset);

#else

#include <string.h>

#define c2_memcmp memcmp
#define c2_memcpy memcpy
#define c2_memmove memmove
#define c2_memset memset

#define c2_strcat strcat

#define c2_strcmp strcmp

#if defined(_WIN32)
#define c2_strcasecmp stricmp
#define c2_strncasecmp strnicmp
#else
#define c2_strcasecmp strcasecmp
#define c2_strncasecmp strncasecmp
#endif

#define c2_strchr strchr
#define c2_strrchr strrchr
#define c2_strstr strstr
#define c2_strtok strtok
#define c2_strpbrk strpbrk

#endif

#endif //REC2_STRING_H_
