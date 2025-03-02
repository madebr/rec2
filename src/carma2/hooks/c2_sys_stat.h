#ifndef REC2_SYS_STAT_H_
#define REC2_SYS_STAT_H_

#include <sys/types.h>
#include <sys/stat.h>

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

int C2_HOOK_CDECL c2_fstat32(int fd, struct _stat32 *buffer);

#else

#include <sys/stat.h>

#ifdef _WIN32
#define c2_fstat32 _fstat32
#else
#define c2_fstat32 fstat
#define _stat32 stat
#endif

#endif

#endif //REC2_SYS_STAT_H_
