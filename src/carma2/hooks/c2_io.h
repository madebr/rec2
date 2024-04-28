#ifndef REC2_IO_H_
#define REC2_IO_H_

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

int C2_HOOK_CDECL c2_open(const char *filename, int oflag);

int C2_HOOK_CDECL c2_close(int fd);

int C2_HOOK_CDECL c2_read(int fd, void * const buffer, unsigned const buffer_size);

#else

#include <io.h>

#define c2_open _open

#define c2_close _close

#define c2_read _read

#endif

#endif //REC2_IO_H_
