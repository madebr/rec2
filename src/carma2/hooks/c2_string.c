#include "c2_string.h"

#if defined(C2_HOOKS_ENABLED)

#include "hooks.h"

#include <string.h>

char *c2_strchr(const char *str, int c) {
    return strchr(str, c);
}
C2_HOOK_FUNCTION(0x00575bf0, c2_strchr)

char* c2_strcat(char *dest, const char *src) {
    return strcat(dest, src);
}
C2_HOOK_FUNCTION(0x00579a40, c2_strcat)

char* c2_strcpy(char *dest, const char *src) {
    return strcpy(dest, src);
}
C2_HOOK_FUNCTION(0x00579a30, c2_strcpy)

const char *c2_strstr(const char *str, const char *delim) {
    return strstr(str, delim);
}
C2_HOOK_FUNCTION(0x005767d0, c2_strstr)

char *c2_strtok(char *str, const char *delim) {
    return strtok(str, delim);
}
C2_HOOK_FUNCTION(0x00575d00, c2_strtok)

#endif
