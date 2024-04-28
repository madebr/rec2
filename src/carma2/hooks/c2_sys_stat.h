#ifndef REC2_SYS_STAT_H_
#define REC2_SYS_STAT_H_

#include <sys/types.h>

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

int C2_HOOK_CDECL c2_fstat32(int fd, struct _stat32 *buffer);

#else

#include <sys/stat.h>

#define c2_fstat32 _fstat32

#endif

#endif //REC2_SYS_STAT_H_
