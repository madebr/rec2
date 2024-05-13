#ifndef SOFTREND_LIGHTING_H
#define SOFTREND_LIGHTING_H

#include "types.h"

#include "c2_hooks.h"

br_geometry_lighting* C2_HOOK_STDCALL GeometryLightingAllocate(br_soft_renderer_facility* type, const char* id);

void C2_HOOK_CDECL _M_br_geometry_lighting_soft_free(br_geometry_lighting_soft* self);

br_token C2_HOOK_CDECL _M_br_geometry_lighting_soft_type(br_geometry_lighting_soft* self);

br_boolean C2_HOOK_CDECL _M_br_geometry_lighting_soft_isType(br_geometry_lighting_soft* self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_geometry_lighting_soft_space(br_geometry_lighting_soft* self);

br_tv_template* C2_HOOK_CDECL _M_br_geometry_lighting_soft_templateQuery(br_geometry_lighting_soft* self);

br_error C2_HOOK_CDECL _M_br_geometry_lighting_soft_render(br_geometry_lighting_soft* self, br_soft_renderer* renderer, br_vector3* points, br_vector3* normals, br_colour* colour_in, br_colour* colour_out, br_uint_16* redirect, int pstride, int nstride, int cinstride, int coutstride, int nvertices);

#endif /* SOFTREND_LIGHTING_H */
