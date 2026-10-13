#ifndef GUARD_50_FOG_H
#define GUARD_50_FOG_H

#include "c2_hooks.h"
#include <brender/brender.h>

extern int gMaterials_to_adapt_count;
extern br_material* gMaterials_to_adapt[200];
extern int gAllow_material_adapt;

extern void C2_HOOK_FASTCALL InitFogificateMaterials(void);

// FogificateMaterials

// AutoFogificateMaterials

extern void C2_HOOK_FASTCALL AddFogificateMaterial(br_material* pMaterial);

extern void C2_HOOK_FASTCALL EnableAutoFogification(void);

extern void C2_HOOK_FASTCALL DisableAutoFogification(void);

extern void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial);

#endif // GUARD_50_FOG_H
