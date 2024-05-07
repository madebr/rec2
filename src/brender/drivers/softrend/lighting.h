#ifndef SOFTREND_LIGHTING_H
#define SOFTREND_LIGHTING_H

#include "types.h"

#include "c2_hooks.h"

br_geometry_lighting* C2_HOOK_STDCALL GeometryLightingAllocate(br_soft_renderer_facility* type, const char* id);

#endif /* SOFTREND_LIGHTING_H */
