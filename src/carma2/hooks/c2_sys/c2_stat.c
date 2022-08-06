#include "c2_stat.h"

int C2_HOOK_CDECL c2_stat32(const char* path, struct c2_stat* buffer) {
    return _stat32(path, buffer);
}
C2_HOOK_FUNCTION(0x00578010, c2_stat32)
