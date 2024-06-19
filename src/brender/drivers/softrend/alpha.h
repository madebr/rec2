#ifndef SOFTREND_ALPHA_H
#define SOFTREND_ALPHA_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL SurfaceAlpha(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar *comp);

#endif /* SOFTREND_ALPHA_H */
