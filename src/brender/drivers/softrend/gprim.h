#ifndef SOFTREND_GPRIM_H
#define SOFTREND_GPRIM_H

#include "types.h"

#include "c2_hooks.h"

br_geometry_primitives* C2_HOOK_STDCALL GeometryPrimitivesAllocate(br_soft_renderer_facility* type, const char* id);

#endif /* SOFTREND_GPRIM_H */
