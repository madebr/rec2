#include "c2_ctype.h"

#include <ctype.h>

int C2_HOOK_CDECL c2_tolower(int character) {
    return tolower(character);
}
C2_HOOK_FUNCTION(0x00576860, c2_tolower);

int C2_HOOK_CDECL c2_toupper(int character) {
    return toupper(character);
}
C2_HOOK_FUNCTION(0x005771a0, c2_toupper);

int C2_HOOK_CDECL c2_isalpha(int c) {
    return isalpha(c);
}
C2_HOOK_FUNCTION(0x005843e0, c2_isalpha)

int C2_HOOK_CDECL c2_isdigit(int c) {
    return isdigit(c);
}
C2_HOOK_FUNCTION(0x00584240, c2_isdigit)

int C2_HOOK_CDECL c2_isspace(int c) {
    return isspace(c);
}
C2_HOOK_FUNCTION(0x005842a0, c2_isspace)

int C2_HOOK_CDECL c2_isprint(int c) {
    return isprint(c);
}
C2_HOOK_FUNCTION(0x00584330, c2_isprint)
