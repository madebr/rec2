#ifndef SOFTREND_DEVICE_H
#define SOFTREND_DEVICE_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL _M_br_softrend_device_free(br_soft_device* self);

br_token C2_HOOK_CDECL _M_br_softrend_device_type(br_soft_device* self);

int C2_HOOK_CDECL _M_br_softrend_device_isType(br_soft_device* self, br_token t);

br_size_t C2_HOOK_CDECL _M_br_softrend_device_space(br_soft_device* self);

br_tv_template* C2_HOOK_CDECL _M_br_soft_device_templateQuery(br_soft_device* self);

void* C2_HOOK_CDECL _M_br_softrend_device_listQuery(br_soft_device* self);

br_device* C2_HOOK_STDCALL DeviceSoftAllocate(const char* identifier);

#endif /* SOFTREND_DEVICE_H */
