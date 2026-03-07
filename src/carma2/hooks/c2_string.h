#ifndef REC2_STRING_H_
#define REC2_STRING_H_

#include <string.h>

#define c2_memcmp memcmp
#ifdef _WIN32
#define c2_memicmp _memicmp
#else
#define c2_memicmp memcmp
#endif
#define c2_memcpy memcpy
#define c2_memmove memmove
#define c2_memset memset

#define c2_memchr memchr

#define c2_strcat strcat
#define c2_strncat strncat

#define c2_strcmp strcmp
#define c2_strncmp strncmp

#if defined(_WIN32)
#define c2_strcasecmp _stricmp
#define c2_strncasecmp _strnicmp
#else
#define c2_strcasecmp strcasecmp
#define c2_strncasecmp strncasecmp
#endif

#define c2_strcpy strcpy
#define c2_strncpy strncpy
#define c2_strlen strlen

#define c2_strchr strchr
#define c2_strspn strspn
#define c2_strrchr strrchr
#define c2_strstr strstr
#define c2_strtok strtok
#define c2_strpbrk strpbrk

#endif //REC2_STRING_H_
