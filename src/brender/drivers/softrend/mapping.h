/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef SOFTREND_MAPPING_H
#define SOFTREND_MAPPING_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL SurfaceMapEnvironmentInfinite(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* normal, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapEnvironmentLocal(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* normal, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapGeometryX(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapGeometryY(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapGeometryZ(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapGeometryMap(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapGeometryMapScale(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapGeometryMapScaleTranslate(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceMapGeometryMapCopy(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

#endif /* SOFTREND_MAPPING_H */