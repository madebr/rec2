/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_50_FOG_H
#define GUARD_50_FOG_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include <brender/brender.h>

extern int gAllow_material_adapt;
extern br_material* gMaterials_to_adapt[200];
extern int gShrapnel_ghost_actor_count;

extern void C2_HOOK_FASTCALL InitFogificateMaterials(void);

extern void C2_HOOK_FASTCALL FogificateMaterials(tRendererShadingType pShading_type);

extern void C2_HOOK_FASTCALL AutoFogificateMaterials(tRendererShadingType pShading_type);

extern void C2_HOOK_FASTCALL AddFogificateMaterial(br_material* pMaterial);

extern void C2_HOOK_FASTCALL EnableAutoFogification(void);

extern void C2_HOOK_FASTCALL DisableAutoFogification(void);

extern void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial);

#endif // GUARD_50_FOG_H
