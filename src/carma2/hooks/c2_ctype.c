#include "c2_ctype.h"

#include <ctype.h>

int c2_toupper(int character) {
    return toupper(character);
}
C2_HOOK_FUNCTION(0x005771a0, c2_toupper);
