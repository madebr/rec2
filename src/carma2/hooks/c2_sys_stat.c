#include "c2_sys_stat.h"

#if defined(C2_HOOKS_ENABLED)

#include <sys/stat.h>

int (C2_HOOK_CDECL * c2_fstat32_original)(int fd, struct _stat32 *buffer);
int C2_HOOK_CDECL c2_fstat32(int fd, struct _stat32 *buffer) {

#if defined(C2_HOOKS_ENABLED)
    return c2_fstat32_original(fd, buffer);
#else
    return _fstat32(fd, buffer);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00587d00, c2_fstat32, c2_fstat32_original)

#endif
