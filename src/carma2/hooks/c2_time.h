#ifndef REC2_C2_TIME_H
#define REC2_C2_TIME_H

#if defined(C2_HOOKS_ENABLED)

#include "c2_hooks.h"

typedef long c2_time_t;

c2_time_t C2_HOOK_CDECL c2_time(c2_time_t *time);

#else

#include "time.h"

#define c2_time_t time_t
#define c2_time time

#endif

#endif //REC2_C2_TIME_H
