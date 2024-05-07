#ifndef SOFTREND_GV1MODEL_H
#define SOFTREND_GV1MODEL_H

#include "types.h"

#include "c2_hooks.h"

br_geometry_v1_model* C2_HOOK_STDCALL GeometryV1ModelAllocate(br_soft_renderer_facility *type, const char *id);

#endif /* SOFTREND_GV1MODEL_H */
