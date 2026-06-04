#include "28-world3.h"

#include "52-errors.h"

// GLOBAL: CARMA2_HW 0x006b75c0
br_actor* gAdditional_actors;

// TurnOnCloaking

// RemoveFromCloakingList

// IsCarCloaked

// TurnOffCloaking

// PeriodicCloaking

// STUB: CARMA2_HW 0x00504b30
void C2_HOOK_FASTCALL InitTreeSurgery(void) {
    NOT_IMPLEMENTED();
}

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

// STUB: CARMA2_HW 0x00500d50
void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds) {
    NOT_IMPLEMENTED();
}

// DisposeStorageSpace

// ClearMatertrialSetFromStorageSpace

// ClearOutStorageSpace

// AddPixelmapToStorage

// AddShadeTableToStorage

// AddMaterialToStorage

// AddModelToStorage

tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id) {
    int i;

    if (pStorage_space->sounds_count < pStorage_space->max_sounds) {
        for (i = 0; i < pStorage_space->sounds_count; i++) {
            if (pStorage_space->sounds[i] != 0 && pStorage_space->sounds[i] == pSound_id) {
                return eStorage_duplicate;
            }
        }
        pStorage_space->sounds[pStorage_space->sounds_count] = pSound_id;
        pStorage_space->sounds_count++;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// AddPixelmaps

// LoadSinglePixelmap

// LoadSingleShadeTable

// LoadSingleMaterial

// FUNCTION: CARMA2_HW 0x00501930
tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id) {

    if (S3GetBufferDescription(pSound_id) != NULL) {
        return pSound_id;
    }

    switch (AddSoundToStorage(pStorage_space, pSound_id)) {

    case eStorage_duplicate:
        return pSound_id;

    case eStorage_allocated:
        S3LoadSample(pSound_id);
        return pSound_id;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientSoundSlotsInStorageArea);
        break;
    }
    return 0;
}

// AddShadeTables

// AddModels

// AddMaterials

// DodgyModelUpdate

// LoadIfItsAPixelmap

// LoadAllPixelmapsInDirectory

// GetFileName

// GetAdditionalFileName

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

