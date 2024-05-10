#ifndef SOFTREND_RNDSTATE_H
#define SOFTREND_RNDSTATE_H

#include "types.h"

#include "c2_hooks.h"

br_error C2_HOOK_STDCALL CheckPrimitiveState(br_soft_renderer* self);

br_error C2_HOOK_CDECL _M_br_soft_renderer_partSet(br_soft_renderer* self, br_token part, br_int_32 index, br_token t, br_uint_32 value);

br_error C2_HOOK_CDECL _M_br_soft_renderer_partSetMany(br_soft_renderer* self, br_token part, br_int_32 index, br_token_value* tv, br_int_32* pcount);

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQuery(br_soft_renderer* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_token t);

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQueryBuffer(br_soft_renderer* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_uint_32 *buffer, br_size_t buffer_size, br_token t);

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQueryMany(br_soft_renderer* self, br_token part, br_int_32 index, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount);

#endif /* SOFTREND_RNDSTATE_H */
