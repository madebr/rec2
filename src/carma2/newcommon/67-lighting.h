#ifndef GUARD_67_LIGHTING_H
#define GUARD_67_LIGHTING_H

#include "c2_hooks.h"

#include <stdio.h>

// LoadTrackModels

// SmoothlyLoadIfItsAModel

// AddSmoothModels

// LoadTrackMaterials

// LoadCarMaterials

// SmoothlySetCarMaterialFlags

// SmoothlySetWorldMaterialFlags

// SmoothificatePowerups

// SmoothificatePowerupMaterial

// SmoothificateWorldMaterial

// SmoothificateCarMaterial

extern void C2_HOOK_FASTCALL MungeLightingEffects(void);

extern void C2_HOOK_FASTCALL KillLightingEffects(void);

extern void C2_HOOK_FASTCALL InitLightingEffects(void);

extern void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* pRace_lighting_path);

extern void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF);

#endif // GUARD_67_LIGHTING_H
