/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_28_WORLD3_H
#define GUARD_28_WORLD3_H

#include "brender/brender.h"
#include "rec2_types.h"

extern br_actor* gAdditional_actors;

extern tU8* gTrack_flic_buffer;

extern tU32 gTrack_flic_buffer_size;

extern tFlic_descriptor gTrack_flic_descriptor;

extern tBrender_storage* gStorage_for_callbacks;

extern tRendererShadingType gMaterial_shading_for_callback;

extern void C2_HOOK_FASTCALL InitTreeSurgery(void);

// ModelIsATree

// PerformTreeSurgery

// PossibleTreeSurgery

// AddExceptionToList

// LoadExceptionsFile

// LoadExceptionsFileForTrack

// FreeExceptions

// GetFaceFlag

// MungeTrackModel

// LoadTrack

// RemoveBounds

extern intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg);

// RemoveBoundsStructures

extern void C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec);

// ProcessTrack

extern void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform);

// AnimateSky

extern void C2_HOOK_FASTCALL AnimateSky(void);

// CombineBounds

// CheckNonCarModelBounds

extern void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds);

extern void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL ClearMatertrialSetFromStorageSpace(tBrender_storage* pStorage_space, int pStart, int pEnd);

extern void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id);

extern int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path);

extern br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName);

extern br_pixelmap* C2_HOOK_FASTCALL LoadSingleShadeTable(tBrender_storage* pStorage_space, const char* pName);

extern br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName);

extern tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id);

extern int C2_HOOK_FASTCALL AddShadeTables(tBrender_storage* pStorage_space, const char* pPath);

extern int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath);

extern int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading);

extern void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM);

extern void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath);

extern int C2_HOOK_FASTCALL GetFileName(const char *path, tName_list *pList);

extern int C2_HOOK_FASTCALL GetAdditionalFileName(const char* path, tName_list* pList);

extern void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);

extern void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

extern void C2_HOOK_FASTCALL LoadIfItsAModel(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

extern void C2_HOOK_FASTCALL DisallowDuplicates(void);

extern void C2_HOOK_FASTCALL AllowDuplicates(void);

extern br_pixelmap* C2_HOOK_FASTCALL SwapPixelmapInStorage(br_pixelmap* pNew, br_pixelmap* pOriginal, tBrender_storage* pStorage);

extern tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char* pFile_name);

extern tPed_move* C2_HOOK_FASTCALL FindOrOpenMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags);

extern void C2_HOOK_FASTCALL MungeFaces(br_actor* pActor, br_model* pModel);

extern void C2_HOOK_FASTCALL FinishUpLoadingPeds(void);

extern int C2_HOOK_FASTCALL OrientationChanged(const br_matrix34* pOrientation_1, const br_matrix34* pOrientation_2, float pCos_angle);

extern void C2_HOOK_FASTCALL MungeSmashMaterialNames(void);

#endif // GUARD_28_WORLD3_H
