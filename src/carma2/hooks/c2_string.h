#ifndef C2_STRING_H
#define C2_STRING_H

#include <string.h>

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#ifdef _WIN32
#define c2_memicmp _memicmp
#else
#define c2_memicmp memcmp
#endif

#endif /* C2_STRING_H */
