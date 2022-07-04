#ifndef REC2_STRING_H_
#define REC2_STRING_H_

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

char* C2_HOOK_CDECL c2_strchr(const char *str, int c);
char* C2_HOOK_CDECL c2_strcat(char *dest, const char *src);
char* C2_HOOK_CDECL c2_strcpy(char *dest, const char *src);
const char* C2_HOOK_CDECL c2_strstr(const char *str, const char *delim);
char* C2_HOOK_CDECL c2_strtok(char *str, const char *delim);

#else

#include <string.h>

#define c2_strchr strchr
#define c2_strtok strtok
#define c2_strstr strstr

#endif

#endif //REC2_STRING_H_
