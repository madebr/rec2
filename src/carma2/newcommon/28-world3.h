#ifndef GUARD_28_WORLD3_H
#define GUARD_28_WORLD3_H

#include "brender/brender.h"
#include "rec2_types.h"

extern br_actor* gAdditional_actors;

// TurnOnCloaking

// RemoveFromCloakingList

// IsCarCloaked

// TurnOffCloaking

// PeriodicCloaking

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

// RemoveBoundsStructures

// FreeTrack

// ProcessTrack

// AnimateSky

// CombineBounds

// CheckNonCarModelBounds

extern void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds);

// DisposeStorageSpace

// ClearMatertrialSetFromStorageSpace

// ClearOutStorageSpace

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id);

extern int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path);

// LoadSinglePixelmap

// LoadSingleShadeTable

// LoadSingleMaterial

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

// LoadIfItsAShadeTable

// LoadAllShadeTablesInDirectory

extern void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath);

extern void C2_HOOK_FASTCALL DisallowDuplicates(void);

extern void C2_HOOK_FASTCALL AllowDuplicates(void);

#endif // GUARD_28_WORLD3_H
