#include "17-world2.h"

#include "41-utility.h"
#include "globvars.h"
#include "rec2_macros.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00591374
tCar_texturing_level gCar_texturing_level = eCTL_full;

// GLOBAL: CARMA2_HW 0x0059136c
tRoad_texturing_level gRoad_texturing_level = eRTL_full;

// GLOBAL: CARMA2_HW 0x00591370
tWall_texturing_level gWall_texturing_level = eWTL_full;

// GLOBAL: CARMA2_HW 0x006793d8
int gCar_simplification_level;

// GLOBAL: CARMA2_HW 0x00591368
int gRendering_accessories = 1;

// GLOBAL: CARMA2_HW 0x006a22c0
int gCount_extra_renders;

// GLOBAL: CARMA2_HW 0x006a22c8
tExtra_render gExtra_renders[6];

// GLOBAL: CARMA2_HW 0x00704e40
int gMirror_on__graphics;

// FUNCTION: CARMA2_HW 0x004e5cb0
void C2_HOOK_FASTCALL InitialiseExtraRenders(void) {

    gCount_extra_renders = 0;
}

// FUNCTION: CARMA2_HW 0x004e5cc0
void C2_HOOK_FASTCALL AddExtraRender(br_actor* pActor, br_material* pMaterial) {

    gExtra_renders[gCount_extra_renders].actor = pActor;
    gExtra_renders[gCount_extra_renders].material = pMaterial;
    gCount_extra_renders += 1;
}

// FUNCTION: CARMA2_HW 0x004e5ce0
int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, undefined4 pArg2, br_scalar pMax_distance) {
    br_vector3 distance_vector;
    int i;

#define CAMERA_MAX_DISTANCE(A) ((pMax_distance != 0.f) ? pMax_distance : REC2_SQR(((br_camera*)(A)->type_data)->yon_z))

    if (gMirror_on__graphics) {
        BrVector3Sub(&distance_vector, pPoint, (br_vector3*)gRearview_camera_to_world.m[3]);
        if (BrVector3LengthSquared(&distance_vector) < CAMERA_MAX_DISTANCE(gRearview_camera)
                && BrVector3Dot(&distance_vector, (br_vector3*)gRearview_camera_to_world.m[2]) < 0.f) {

            return 0;
        }
    }

    for (i = 0; i < gCount_extra_renders; i++) {
        br_actor* a = gExtra_renders[i].actor;

        BrVector3Sub(&distance_vector, pPoint, &a->t.t.translate.t);
        if (BrVector3LengthSquared(&distance_vector) < CAMERA_MAX_DISTANCE(a)) {

            return 0;
        }
    }

    BrVector3Sub(&distance_vector, pPoint, (br_vector3*)gCamera_to_world.m[3]);
    if (BrVector3LengthSquared(&distance_vector) >= CAMERA_MAX_DISTANCE(gCamera)) {
        return 1;
    }
    if (BrVector3Dot(&distance_vector, (br_vector3*)gCamera_to_world.m[2]) >= 0.f) {

        return 1;
    }
#undef CAMERA_MAX_DISTANCE
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e5ef0
void C2_HOOK_FASTCALL CancelificateClipulatingPlaneyThings(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x004e5f00
void C2_HOOK_FASTCALL FixificateClipulatingPlaneyThings(void) {

    // empty
}

void C2_HOOK_FASTCALL ProcessModelFaceMaterials(br_model* pModel, tPMFMCB* pCallback) {
    tU16 f;
    br_material* possible_mat;
    br_material* new_mat = NULL;

    for (f = 0; f < pModel->nfaces; f++) {

        if (pModel->faces[f].material == NULL) {
            continue;
        }
        possible_mat = pCallback(pModel, f);
        if (possible_mat == NULL) {
            continue;
        }
        pModel->faces[f].material = possible_mat;
        new_mat = possible_mat;
    }
    if (new_mat != NULL) {
        BrModelUpdate(pModel, BR_MODU_ALL);
    }
}

// ProcessModelFaceMaterials2

// FUNCTION: CARMA2_HW 0x00448850
intptr_t C2_HOOK_CDECL ProcessFaceMaterials(br_actor* pActor, void* pData) {
    tPMFMCB* callback = pData;

    if (pActor->identifier != NULL && pActor->identifier[0] == '&') {
        return 0;
    }
    if (pActor->type == BR_ACTOR_MODEL && pActor->model != NULL) {
        ProcessModelFaceMaterials(pActor->model, callback);
    }
    return BrActorEnum(pActor, ProcessFaceMaterials, pData);
}

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

        if (pStorage->materialProps[i] != NULL && DRPixelmapHasZeros(pStorage->materialProps[i])) {
            pStorage->materials[i]->colour_map = pStorage->materialProps[i];
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

// FUNCTION: CARMA2_HW 0x00448ce0
int C2_HOOK_FASTCALL HasThisSuffix(const char* pIdent, const char* pSuffix) {
    size_t len_ident;
    size_t len_suffix;

    len_ident = strlen(pIdent);
    len_suffix = strlen(pSuffix);
    if (pIdent == NULL) {
        return 0;
    }
    if (len_ident < len_suffix) {
        return 0;
    }
    return strcmp(pIdent + len_ident - len_suffix, pSuffix) == 0;
}

// FUNCTION: CARMA2_HW 0x00448d70
br_material* C2_HOOK_FASTCALL UnsuffixedMaterial(const char* pOld_ident, const char* pSuffix) {
    br_material* result;
    int unsuffixed_len;
    char* new_id;

    unsuffixed_len = strlen(pOld_ident) - strlen(pSuffix);
    new_id = BrMemAllocate(unsuffixed_len + 1, kMem_new_mat_id);
    sprintf(new_id, "%.*s", unsuffixed_len, pOld_ident);
    result = BrMaterialFind(new_id);
    BrMemFree(new_id);
    return result;
}

// FUNCTION: CARMA2_HW 0x00447c60
br_material* C2_HOOK_FASTCALL RoadUntexToPersp(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".road")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".road");
    } else {
        new_mat = NULL;
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x004481d0
br_material* C2_HOOK_FASTCALL WallUntexToPersp(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".lwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".lwall");
    } else if (HasThisSuffix(old_mat->identifier, ".pwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".pwall");
    } else {
        new_mat = NULL;
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x00448390
br_material* C2_HOOK_FASTCALL WallLinearToPersp(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".pwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".pwall");
    } else {
        new_mat = NULL;
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x00448770
tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void) {

    return gRoad_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00448780
void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel) {

    gRoad_texturing_level = pLevel;
}

// GetWallTexturingLevel

// FUNCTION: CARMA2_HW 0x00448900
void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel) {

    gWall_texturing_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00448dd0
br_material* C2_HOOK_FASTCALL DisposeSuffixedMaterials(br_model* pModel, tU16 pFace) {
    size_t max_suffix_len;
    br_material* mat;
    br_material* victim;
    // GLOBAL: CARMA2_HW 0x005913a0
    static const char* suffixes[3] = {
        ".road",
        ".pwall",
        ".lwall",
    };
    int s;
    char* id;

    mat = pModel->faces[pFace].material;
    if (mat->identifier != NULL) {
        max_suffix_len = 0;
        for (s = 0; s < REC2_ASIZE(suffixes); s++) {
            if (max_suffix_len < strlen(suffixes[s])) {
                max_suffix_len = strlen(suffixes[s]);
            }
        }
        id = BrMemAllocate(strlen(mat->identifier) + max_suffix_len + 1, kMem_new_mat_id);
        for (s = 0; s < REC2_ASIZE(suffixes); s++) {
            sprintf(id, "%s%s", mat->identifier, suffixes[s]);
            victim = BrMaterialFind(id);
            if (victim != NULL) {
                BrMaterialRemove(victim);
                BrMaterialFree(victim);
            }
        }
#ifdef REC2_FIX_BUGS
        BrMemFree(id);
#endif
    }
    return NULL;
}

// DisposeTexturingMaterials

// FUNCTION: CARMA2_HW 0x00448ea0
intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB(br_actor* pActor, void* pFlag) {

    if (pActor->identifier != NULL && pActor->identifier[0] == '&') {
        pActor->render_style = *(br_uint_8*)pFlag;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00448ec0
void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn) {
    int style;

    if (gTrack_actor != NULL)  {
        if (pOn) {
            style = BR_RSTYLE_FACES;
        } else {
            style = BR_RSTYLE_NONE;
        }
        DRActorEnumRecurse(gTrack_actor, SetAccessoryRenderingCB, &style);
    }
    gRendering_accessories = pOn;
}

// GetAccessoryRendering

// FUNCTION: CARMA2_HW 0x00448f10
void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel) {

    gCar_simplification_level = pLevel;
}

// GetCarSimplificationLevel

// STUB: CARMA2_HW 0x00448f90
intptr_t C2_HOOK_CDECL ProcessMaterials(br_actor* pActor, material_cbfn* pCallback) {
    NOT_IMPLEMENTED();
}

// BuildColourTable

