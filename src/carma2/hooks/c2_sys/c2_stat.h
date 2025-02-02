#ifndef C2_SYS_STAT_H
#define C2_SYS_STAT_H

#include <sys/stat.h>

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

#define c2_stat _stat32

int C2_HOOK_CDECL c2_stat32(const char* path, struct c2_stat* buffer);

#else

#define c2_stat _stat

#define c2_stat32 _stat

#endif

#endif // C2_SYS_STAT_H
