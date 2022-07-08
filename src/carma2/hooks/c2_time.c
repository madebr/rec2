#include "c2_time.h"


c2_time_t (C2_HOOK_CDECL * time_original)(c2_time_t *time);
c2_time_t C2_HOOK_CDECL c2_time(c2_time_t *time) {
    return time_original(time);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00577dc0, c2_time, time_original)
