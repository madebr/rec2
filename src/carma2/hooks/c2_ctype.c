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
