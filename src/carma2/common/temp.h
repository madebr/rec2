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
