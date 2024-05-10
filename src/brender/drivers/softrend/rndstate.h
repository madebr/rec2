#ifndef SOFTREND_RNDSTATE_H
#define SOFTREND_RNDSTATE_H

#include "types.h"

#include "c2_hooks.h"

br_error C2_HOOK_STDCALL CheckPrimitiveState(br_soft_renderer* self);

br_error C2_HOOK_CDECL _M_br_soft_renderer_partSet(br_soft_renderer* self, br_token part, br_int_32 index, br_token t, br_uint_32 value);

br_error C2_HOOK_CDECL _M_br_soft_renderer_partSetMany(br_soft_renderer* self, br_token part, br_int_32 index, br_token_value* tv, br_int_32* pcount);

#endif /* SOFTREND_RNDSTATE_H */
