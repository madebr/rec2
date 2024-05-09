#ifndef SOFTREND_RENDERER_H
#define SOFTREND_RENDERER_H

#include "types.h"

#include "c2_hooks.h"

br_renderer* C2_HOOK_STDCALL RendererSoftAllocate(br_device *dev, br_soft_renderer_facility *type, struct br_primitive_library *prims);

void C2_HOOK_CDECL _M_br_soft_renderer_free(br_soft_renderer* self);

br_token C2_HOOK_CDECL _M_br_soft_renderer_type(br_soft_renderer* self);

int C2_HOOK_CDECL _M_br_soft_renderer_isType(br_soft_renderer* self, br_token t);

#endif /* SOFTREND_RENDERER_H */
