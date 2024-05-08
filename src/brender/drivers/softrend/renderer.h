#ifndef SOFTREND_RENDERER_H
#define SOFTREND_RENDERER_H

#include "types.h"

#include "c2_hooks.h"

br_renderer* C2_HOOK_STDCALL RendererSoftAllocate(br_device *dev, br_soft_renderer_facility *type, struct br_primitive_library *prims);

#endif /* SOFTREND_RENDERER_H */
