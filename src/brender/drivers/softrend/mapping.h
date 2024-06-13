#ifndef SOFTREND_MAPPING_H
#define SOFTREND_MAPPING_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL SurfaceMapEnvironmentInfinite(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* normal, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapEnvironmentLocal(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* normal, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapGeometryX(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

#endif /* SOFTREND_MAPPING_H */
