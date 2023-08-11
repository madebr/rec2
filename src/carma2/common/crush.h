#ifndef REC2_CRUSH_H
#define REC2_CRUSH_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tU8, gCrush_data_entry_counter);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gSoftness_names, 6);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gCrush_type_names, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gEase_of_detachment_names, 5);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gCar_crush_shape_names, 2);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gDetach_type_names, 4);

void C2_HOOK_FASTCALL InitCrush(void);

void C2_HOOK_FASTCALL LoadGeneralCrushSettings(FILE* file);

br_scalar C2_HOOK_FASTCALL SquaredDistanceFromLineSegment(br_vector3* pP, br_vector3* pA, br_vector3* pB);

void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds);

void C2_HOOK_FASTCALL LoadNoncarActivation(FILE* pF, tNoncar_activation** pNoncar_activations, int* pCount_noncar_activations);

void C2_HOOK_FASTCALL LoadAward(FILE* pF, tAward_info* pAward_info);

void C2_HOOK_FASTCALL LoadSmashableLevels(FILE* pF, tSmashable_level** pSmashable_levels, int* pCount_smashable_levels, int pIs_texture_change, tBrender_storage* pBrender_storage);

void C2_HOOK_FASTCALL LoadCarCrushSmashDataEntries(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage);

void C2_HOOK_FASTCALL LoadCarCrushDataEntry(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry,tBrender_storage *pBrender_storage);

int C2_HOOK_CDECL LinkCrushData(br_actor* pActor, void* pData);

int C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, tCar_crush_buffer* pCar_crush_buffer);

int C2_HOOK_CDECL AllocateUserDetailLevel(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL PrepareCarForCrushing(tCar_spec * pCar_spec);

#endif // REC2_CRUSH_H
