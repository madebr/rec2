#ifndef GUARD_67_LIGHTING_H
#define GUARD_67_LIGHTING_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage* pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL SmoothlyLoadIfItsAModel(const char* pPath);

extern int C2_HOOK_FASTCALL AddSmoothModels(tBrender_storage* pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL LoadCarMaterials(tBrender_storage* pStorage, const char* pPath, tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL SmoothlySetCarMaterialFlags(tBrender_storage* pStorage, tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL SmoothlySetWorldMaterialFlags(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL SmoothificatePowerups(br_actor* pActor);

extern void C2_HOOK_FASTCALL SmoothificatePowerupMaterial(br_material* pMaterial);

extern void C2_HOOK_FASTCALL SmoothificateWorldMaterial(br_material* pMaterial);

extern void C2_HOOK_FASTCALL SmoothificateCarMaterial(br_material* pMaterial);

extern void C2_HOOK_FASTCALL MungeLightingEffects(void);

extern void C2_HOOK_FASTCALL KillLightingEffects(void);

extern void C2_HOOK_FASTCALL InitLightingEffects(void);

extern void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* race_lighting_path);

extern void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF);

#endif // GUARD_67_LIGHTING_H
