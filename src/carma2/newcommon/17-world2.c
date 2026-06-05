#include "17-world2.h"

#include "globvars.h"

// GLOBAL: CARMA2_HW 0x00591374
tCar_texturing_level gCar_texturing_level = eCTL_full;

// InitialiseExtraRenders

// AddExtraRender

// PointOutOfSight

// CancelificateClipulatingPlaneyThings

// FixificateClipulatingPlaneyThings

// ProcessModelFaceMaterials

// ProcessModelFaceMaterials2

// ProcessFaceMaterials

// FUNCTION: CARMA2_HW 0x004475c0
int C2_HOOK_FASTCALL DRPixelmapHasZeros(br_pixelmap* pm) {
    int x;
    int y;
    tU8* row_ptr;
    tU8* pp;

    if (pm->flags & BR_PMF_NO_ACCESS) {
        return 1;
    }
    row_ptr = (tU8*)pm->pixels + pm->base_x + pm->base_y * pm->row_bytes;
    for (y = 0; y < pm->height; y++) {
        pp = row_ptr;
        for (x = 0; x < pm->width; x++, pp++) {
            if (*pp == '\0') {
                return 1;
            }
        }
        row_ptr += pm->row_bytes;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004479c0
int C2_HOOK_FASTCALL StorageContainsPixelmap(tBrender_storage* pStorage, br_pixelmap* pMap) {
    int i;

    for (i = 0; i < pStorage->pixelmaps_count; i++) {

        if (pStorage->pixelmaps[i] == pMap) {
            break;
        }
    }
    return i != pStorage->pixelmaps_count;
}

void C2_HOOK_FASTCALL HideStoredOpaqueTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materials[i]->colour_map;

        if (colour_map != NULL && StorageContainsPixelmap(pStorage, colour_map) && !DRPixelmapHasZeros(colour_map)) {
            pStorage->materialProps[i] = colour_map;
            pStorage->materials[i]->colour_map = NULL;
            pStorage->materials[i]->flags &= ~BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00447b00
void C2_HOOK_FASTCALL RevealStoredTransparentTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materialProps[i];

        if (colour_map != NULL && DRPixelmapHasZeros(colour_map)) {
            pStorage->materials[i]->colour_map = colour_map;
            pStorage->materialProps[i] = NULL;
            pStorage->materials[i]->flags |= BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

void C2_HOOK_FASTCALL HideStoredTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materials[i]->colour_map;

        if (colour_map != NULL && StorageContainsPixelmap(pStorage, colour_map)) {
            pStorage->materialProps[i] = colour_map;
            pStorage->materials[i]->colour_map = NULL;
            pStorage->materials[i]->flags &= ~BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

void C2_HOOK_FASTCALL RevealStoredTextures(tBrender_storage* pStorage) {
    int i;
    br_pixelmap* colour_map;

    for (i = 0; i < pStorage->materials_count; i++) {

        colour_map = pStorage->materialProps[i];

        if (colour_map != NULL) {
            pStorage->materials[i]->colour_map = colour_map;
            pStorage->materialProps[i] = NULL;
            pStorage->materials[i]->flags |= BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00447350
void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld) {

    switch (pNew) {
    case eCTL_full:
        RevealStoredTextures(pStorage);
        break;
    case eCTL_transparent:
        switch (pOld) {
        case eCTL_full:
            HideStoredOpaqueTextures(pStorage);
            break;
        case eCTL_none:
            RevealStoredTransparentTextures(pStorage);
            break;
        default:
            break;
        }
        break;
    case eCTL_none:
        HideStoredTextures(pStorage);
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00447640
tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void) {

    return gCar_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00447650
    void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel) {

    if (gCar_texturing_level != pLevel) {
        if (gOur_car_storage_space.models_count != 0) {
            SetCarStorageTexturingLevel(&gOur_car_storage_space, pLevel, gCar_texturing_level);
        }
        if (gTheir_cars_storage_space.models_count != 0) {
            SetCarStorageTexturingLevel(&gTheir_cars_storage_space, pLevel, gCar_texturing_level);
        }
        if (gNet_cars_storage_space.models_count != 0) {
            // FIXME: wrong BRender storage?
            SetCarStorageTexturingLevel(&gTheir_cars_storage_space, pLevel, gCar_texturing_level);
        }
    }
    gCar_texturing_level = pLevel;
}

// HasThisSuffix

// UnsuffixedMaterial

// RoadUntexToPersp

// WallUntexToPersp

// WallLinearToPersp

// GetRoadTexturingLevel

// STUB: CARMA2_HW 0x00448780
void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel) {
    NOT_IMPLEMENTED();
}

// GetWallTexturingLevel

// STUB: CARMA2_HW 0x00448900
void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel) {
    NOT_IMPLEMENTED();
}

// DisposeSuffixedMaterials

// DisposeTexturingMaterials

// SetAccessoryRenderingCB

// STUB: CARMA2_HW 0x00448ec0
void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn) {
    NOT_IMPLEMENTED();
}

// GetAccessoryRendering

// STUB: CARMA2_HW 0x00448f10
void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel) {
    NOT_IMPLEMENTED();
}

// GetCarSimplificationLevel

// ProcessMaterials

// BuildColourTable

