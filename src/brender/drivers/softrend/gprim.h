#ifndef SOFTREND_GPRIM_H
#define SOFTREND_GPRIM_H

#include "types.h"

#include "c2_hooks.h"

br_geometry_primitives* C2_HOOK_STDCALL GeometryPrimitivesAllocate(br_soft_renderer_facility* type, const char* id);

void C2_HOOK_CDECL _M_br_geometry_primitives_soft_free(br_geometry_primitives_soft* self);

br_token C2_HOOK_CDECL _M_br_geometry_primitives_soft_type(br_geometry_primitives_soft* self);

br_boolean C2_HOOK_CDECL _M_br_geometry_primitives_soft_isType(br_geometry_primitives_soft* self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_geometry_primitives_soft_space(br_geometry_primitives_soft* self);

br_tv_template* C2_HOOK_CDECL _M_br_geometry_primitives_soft_templateQuery(br_geometry_primitives_soft* self);

#endif /* SOFTREND_GPRIM_H */
