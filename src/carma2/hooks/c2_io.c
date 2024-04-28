#include "c2_io.h"

#include "c2_hooks.h"


#if defined(C2_HOOKS_ENABLED)

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int (C2_HOOK_CDECL * c2_open_original)(const char *filename, int oflag);
int C2_HOOK_CDECL c2_open(const char *filename, int oflag) {

#if defined(C2_HOOKS_ENABLED)
    return c2_open_original(filename, oflag);
#else
    return _open(filename, oflag, _O_RDONLY);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00584a00, c2_open, c2_open_original)

int (C2_HOOK_CDECL * c2_close_original)(int fd);
int C2_HOOK_CDECL c2_close(int fd) {

#if defined(C2_HOOKS_ENABLED)
    return c2_close_original(fd);
#else
    return _close(fd);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005805e0, c2_close, c2_close_original)

#endif
