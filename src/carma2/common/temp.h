/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef TEMP_H
#define TEMP_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_hooks.h"

void C2_HOOK_FASTCALL InitFogificateMaterials(void);

br_material* C2_HOOK_FASTCALL LoadTemporaryMaterial(const char* pName);

void C2_HOOK_FASTCALL TemporaryMaterialStore(br_material* pMaterial);

void C2_HOOK_FASTCALL EnableAutoFogification(void);

void C2_HOOK_FASTCALL DisableAutoFogification(void);

void C2_HOOK_FASTCALL AdaptCachedMaterials(tRendererShadingType pShading_type);

void C2_HOOK_FASTCALL FogificateMaterials(tRendererShadingType pShading_type);

#endif /* TEMP_H */