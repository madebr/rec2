#include "c2_direct.h"

#if defined(C2_HOOKS_ENABLED)

#include <direct.h>

int C2_HOOK_CDECL c2_mkdir(const char* path) {
    return _mkdir(path);
}
C2_HOOK_FUNCTION(0x00578400, c2_mkdir)

#endif
