#ifndef SOFTREND_DEVICE_H
#define SOFTREND_DEVICE_H

#include "types.h"

#include "c2_hooks.h"

br_device* C2_HOOK_STDCALL DeviceSoftAllocate(const char* identifier);

#endif /* SOFTREND_DEVICE_H */