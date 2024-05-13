#ifndef SOFTREND_LIGHTING_H
#define SOFTREND_LIGHTING_H

#include "types.h"

#include "c2_hooks.h"

br_geometry_lighting* C2_HOOK_STDCALL GeometryLightingAllocate(br_soft_renderer_facility* type, const char* id);

void C2_HOOK_CDECL _M_br_geometry_lighting_soft_free(br_geometry_lighting_soft* self);

br_token C2_HOOK_CDECL _M_br_geometry_lighting_soft_type(br_geometry_lighting_soft* self);

br_boolean C2_HOOK_CDECL _M_br_geometry_lighting_soft_isType(br_geometry_lighting_soft* self, br_token t);

#endif /* SOFTREND_LIGHTING_H */
