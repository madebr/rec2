#ifndef GUARD_67_LIGHTING_H
#define GUARD_67_LIGHTING_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include <stdio.h>

extern void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage* pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL SmoothlyLoadIfItsAModel(const char* pPath);

extern int C2_HOOK_FASTCALL AddSmoothModels(tBrender_storage* pStorage, const char* pPath);

// LoadTrackMaterials

// LoadCarMaterials

// SmoothlySetCarMaterialFlags

// SmoothlySetWorldMaterialFlags

// SmoothificatePowerups

extern void C2_HOOK_FASTCALL SmoothificatePowerupMaterial(br_material* pMaterial);

// SmoothificateWorldMaterial

// SmoothificateCarMaterial

extern void C2_HOOK_FASTCALL MungeLightingEffects(void);

extern void C2_HOOK_FASTCALL KillLightingEffects(void);

extern void C2_HOOK_FASTCALL InitLightingEffects(void);

extern void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* pRace_lighting_path);

extern void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF);

#endif // GUARD_67_LIGHTING_H
