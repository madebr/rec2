#ifndef SOFTREND_DEVICE_H
#define SOFTREND_DEVICE_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL _M_br_softrend_device_free(br_soft_device* self);

br_device* C2_HOOK_STDCALL DeviceSoftAllocate(const char* identifier);

#endif /* SOFTREND_DEVICE_H */
