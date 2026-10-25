/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef SOFTREND_LIGHT8_H
#define SOFTREND_LIGHT8_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL SurfaceIndexZero(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceIndexUnlit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_CDECL SurfaceIndexLit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexDirect(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexPoint(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexLocal1(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexLocal2(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexPointAttn(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexSpot(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexSpotAttn(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL ActiveLightAccumulateIndexSet(active_light* alp);

#endif /* SOFTREND_LIGHT8_H */