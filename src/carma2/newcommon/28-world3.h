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

// AddPixelmapToStorage

// AddShadeTableToStorage

// AddMaterialToStorage

// AddModelToStorage

tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id);

// AddPixelmaps

// LoadSinglePixelmap

// LoadSingleShadeTable

// LoadSingleMaterial

extern tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id);

// AddShadeTables

// AddModels

// AddMaterials

// DodgyModelUpdate

// LoadIfItsAPixelmap

// LoadAllPixelmapsInDirectory

extern int C2_HOOK_FASTCALL GetFileName(const char *path, tName_list *pList);

extern int C2_HOOK_FASTCALL GetAdditionalFileName(const char* path, tName_list* pList);

// LoadAllImagesInDirectory

// LoadIfItsAMaterial

// LoadAllMaterialsInDirectory

// LoadIfItsAModel

// LoadAllModelsInDirectory

// LoadIfItsAShadeTable

// LoadAllShadeTablesInDirectory

// LoadAllStuffInDirectory

// DisallowDuplicates

// AllowDuplicates

#endif // GUARD_28_WORLD3_H
