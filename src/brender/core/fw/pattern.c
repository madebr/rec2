#include "pattern.h"

#include "c2_ctype.h"

br_boolean C2_HOOK_CDECL BrNamePatternMatch(const char* p, const char* s) {
    const char* cp;

    if (p == NULL) {
        return 1;
    }
    if (s == NULL) {
        return 0;
    }

    for (;; s++, p++) {

        if (*p == '\0') {
            // if pattern string ends at the same point as search string, we match
            return *s == 0;
        }

        if (*p == '*') {
            cp = s;
            do {
                if (BrNamePatternMatch(p + 1, cp)) {
                    return 1;
                }
            } while (*cp++);
            return 0;
        }
        if (*p == '/') {
            return *s == 0;
        }
        if (*p == '?') {
            if (*s == '\0') {
                return 0;
            }
            continue;
        }

        if (c2_toupper(*p) != c2_toupper(*s)) {
            return 0;
        }
    }
    return 1;
}
C2_HOOK_FUNCTION(0x00529860, BrNamePatternMatch)
