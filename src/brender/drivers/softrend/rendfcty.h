#ifndef SOFTREND_FACILITY_H
#define SOFTREND_FACILITY_H

#include <brender/brender.h>

#include "c2_hooks.h"

br_renderer_facility* C2_HOOK_STDCALL RendererFacilitySoftAllocate(br_device* dev, const char* identifier);

#endif //SOFTREND_FACILITY_H
