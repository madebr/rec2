/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "28-world3.h"

#include "08-loading1.h"
#include "33-depth.h"
#include "37-brucetrk.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "51-smash.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x006aaf40
tU8* gTrack_flic_buffer;

// GLOBAL: CARMA2_HW 0x006aaf48
tU32 gTrack_flic_buffer_size;

// GLOBAL: CARMA2_HW 0x006aaf50
tFlic_descriptor gTrack_flic_descriptor;

// GLOBAL: CARMA2_HW 0x006aafc0
static tU32 prev_sky_animation;

// GLOBAL: CARMA2_HW 0x006b75c0
// GLOBAL: CARMA2_SW 0x006ae2a0
br_actor* gAdditional_actors;

// GLOBAL: CARMA2_HW 0x006b7820
// GLOBAL: CARMA2_SW 0x006ae500
tBrender_storage* gStorage_for_callbacks;

// GLOBAL: CARMA2_HW 0x006aaa2c
// GLOBAL: CARMA2_SW 0x006a447c
int gDisallow_duplicates;

// GLOBAL: CARMA2_HW 0x006aaa20
// GLOBAL: CARMA2_SW 0x006a4470
br_pixelmap* gDuplicate_pixelmap;

// GLOBAL: CARMA2_HW 0x006aaa28
// GLOBAL: CARMA2_SW 0x006a4478
br_material* gDuplicate_material;

// GLOBAL: CARMA2_HW 0x006aaa24
br_model* gDuplicate_model;

// GLOBAL: CARMA2_HW 0x00660cb8
// GLOBAL: CARMA2_SW 0x0065b62c
tRendererShadingType gMaterial_shading_for_callback = kRendererShadingType_Undefined;

// FUNCTION: CARMA2_HW 0x00504b30
// FUNCTION: CARMA2_SW 0x005002f0
void C2_HOOK_FASTCALL InitTreeSurgery(void) {
    tPath_name the_path;
    FILE* file;
    int i;

    PathCat(the_path, gApplication_path, "TreeSurgery.TXT");
    file = PFfopen(the_path, "rt");
    if (file != NULL) {
        gTree_surgery_pass1_count = GetAnInt(file);
        for (i = 0; i < gTree_surgery_pass1_count; i++) {
            GetAString(file, gTree_surgery_pass1[i].name);
        }
        gTree_surgery_pass2_count = GetAnInt(file);
        for (i = 0; i < gTree_surgery_pass2_count; i++) {
            GetAString(file, gTree_surgery_pass2[i].original);
            GetAString(file, gTree_surgery_pass2[i].replacement);
        }
#ifdef REC2_FIX_BUGS
        PFfclose(file);
#endif
    } else {
        gTree_surgery_pass1_count = 0;
        gTree_surgery_pass2_count = 0;
    }
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

// FUNCTION: CARMA2_HW 0x00506e20
intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg) {

    if (pActor->type == BR_ACTOR_BOUNDS || pActor->type == BR_ACTOR_BOUNDS_CORRECT) {
        BrResFree(pActor->type_data);
        pActor->type_data = NULL;
    }
    return 0;
}

// RemoveBoundsStructures

// STUB: CARMA2_HW 0x00506c20
// STUB: CARMA2_SW 0x00502070
void C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00506e50
void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {

    RenderTrack(pWorld, pTrack_spec, pCamera, pCamera_to_world_transform);
    if (gAdditional_actors) {
        BrZbsSceneRenderAdd(gAdditional_actors);
    }
}

// FUNCTION: CARMA2_HW 0x00506e80
void C2_HOOK_FASTCALL AnimateSky(void) {
    int now;
    int now2;
    int frame_increment;

    now2 = GetTotalTime();
    if (gTrack_flic_buffer == NULL) {
        return;
    }
    now = now2;
    if (now < (int)prev_sky_animation) {
        now2 = 2 * prev_sky_animation - now;
    }
    if (prev_sky_animation != 0) {
        frame_increment = ((now2 - prev_sky_animation) / gTrack_flic_descriptor.frame_period) % (gTrack_flic_descriptor.frames_left + gTrack_flic_descriptor.current_frame);
    } else {
        frame_increment = 1;
    }
    for (; frame_increment > 0; frame_increment--) {
        if (PlayNextFlicFrame(&gTrack_flic_descriptor)) {
            EndFlic(&gTrack_flic_descriptor);
            StartFlic(gTrack_flic_descriptor.file_name, -1, &gTrack_flic_descriptor, gTrack_flic_buffer_size, (tS8*)gTrack_flic_buffer, NULL, 0, 0, 0);
            AssertFlicPixelmap(&gTrack_flic_descriptor, gProgram_state.default_depth_effect.sky_texture);
        }
        prev_sky_animation = now;
        SkyTextureChanged();
    }
}

// CombineBounds

// CheckNonCarModelBounds

// FUNCTION: CARMA2_HW 0x00500d50
// FUNCTION: CARMA2_SW 0x004fc6b0
void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds) {

    pStorage_space->pixelmaps_count = 0;
    pStorage_space->shade_tables_count = 0;
    pStorage_space->materials_count = 0;
    pStorage_space->models_count = 0;
    pStorage_space->sounds_count = 0;
    pStorage_space->flags = pUnknown;
    pStorage_space->max_pixelmaps = pMax_pixelmaps;
    pStorage_space->max_shade_tables = pMax_shade_tables;
    pStorage_space->max_materials = pMax_materials;
    pStorage_space->max_models = pMax_models;
    pStorage_space->max_sounds = pMax_sounds;
    pStorage_space->pixelmaps = BrMemCalloc(pMax_pixelmaps, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->shade_tables = BrMemCalloc(pMax_shade_tables, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->materials = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
    pStorage_space->models = BrMemCalloc(pMax_models, sizeof(br_model*), kMem_stor_space_table);
    pStorage_space->sounds = BrMemCalloc(pMax_sounds, sizeof(int), kMem_stor_space_table);
    pStorage_space->materialProps = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
}

// FUNCTION: CARMA2_HW 0x00500e10
// FUNCTION: CARMA2_SW 0x004fc770
void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage) {

    BrMemFree(pStorage->pixelmaps);
    BrMemFree(pStorage->shade_tables);
    BrMemFree(pStorage->materials);
    BrMemFree(pStorage->models);
    BrMemFree(pStorage->sounds);
    BrMemFree(pStorage->materialProps);
}

// FUNCTION: CARMA2_HW 0x00500e60
// FUNCTION: CARMA2_SW 0x004fc7c0
void C2_HOOK_FASTCALL ClearMatertrialSetFromStorageSpace(tBrender_storage* pStorage_space, int pStart, int pEnd) {
    int i;
    int move_to;

    for (i = pStart; i < pEnd && i < pStorage_space->materials_count; i++) {
        BrMaterialRemove(pStorage_space->materials[i]);
        BrMaterialFree(pStorage_space->materials[i]);
        RemoveMaterialFromFogification(pStorage_space->materials[i]);
    }
    for (move_to = pStart, i = pEnd; i < pStorage_space->materials_count; ) {
        pStorage_space->materials[move_to++] = pStorage_space->materials[i++];
    }
    pStorage_space->materials_count = move_to;
    if (pStorage_space == &gNet_cars_storage_space) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            tCar_spec* car = gNet_players[i].car;
            if (car != NULL && car->old_material_count >= pEnd) {
                car->old_material_count += pStart - pEnd;
                car->new_material_count += pStart - pEnd;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00500f30
// FUNCTION: CARMA2_SW 0x004fc890
void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space) {
    int i;

    DRS3StopAllOutletSoundsExceptCDA();
    for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
        if (pStorage_space->pixelmaps[i] != NULL) {
            BrMapRemove(pStorage_space->pixelmaps[i]);
            BrPixelmapFree(pStorage_space->pixelmaps[i]);
        }
    }
    pStorage_space->pixelmaps_count = 0;
    for (i = 0; i < pStorage_space->shade_tables_count; i++) {
        if (pStorage_space->shade_tables[i] != NULL) {
            BrTableRemove(pStorage_space->shade_tables[i]);
            BrPixelmapFree(pStorage_space->shade_tables[i]);
        }
    }
    pStorage_space->shade_tables_count = 0;
    for (i = 0; i < pStorage_space->materials_count; i++) {
        if (pStorage_space->materials[i] != NULL) {
            BrMaterialRemove(pStorage_space->materials[i]);
            BrMaterialFree(pStorage_space->materials[i]);
        }
    }
    pStorage_space->materials_count = 0;
    for (i = 0; i < pStorage_space->models_count; i++) {
        if (pStorage_space->models[i] != NULL) {
            BrModelRemove(pStorage_space->models[i]);
            BrModelFree(pStorage_space->models[i]);
        }
    }
    pStorage_space->models_count = 0;
}

// FUNCTION: CARMA2_HW 0x00501020
// FUNCTION: CARMA2_SW 0x004fc980
tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm) {
    int i;

    gDuplicate_pixelmap = NULL;
    if (pStorage_space->pixelmaps_count < pStorage_space->max_pixelmaps) {
        for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
            if (pStorage_space->pixelmaps[i]->identifier != NULL
                    && pThe_pm->identifier != NULL
                    && strcmp(pStorage_space->pixelmaps[i]->identifier, pThe_pm->identifier) == 0) {
                gDuplicate_pixelmap = pStorage_space->pixelmaps[i];
                return eStorage_duplicate;
            }
        }
        pStorage_space->pixelmaps[pStorage_space->pixelmaps_count] = pThe_pm;
        pStorage_space->pixelmaps_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x005010e0
// FUNCTION: CARMA2_SW 0x004fca40
tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st) {
    int i;

    if (pStorage_space->shade_tables_count < pStorage_space->max_shade_tables) {
        for (i = 0; i < pStorage_space->shade_tables_count; i++) {
            if (pStorage_space->shade_tables[i]->identifier != NULL
                && pThe_st->identifier != NULL
                && strcmp(pStorage_space->shade_tables[i]->identifier, pThe_st->identifier) == 0) {
                return eStorage_duplicate;
            }
        }
        pStorage_space->shade_tables[pStorage_space->shade_tables_count] = pThe_st;
        pStorage_space->shade_tables_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501190
// FUNCTION: CARMA2_SW 0x004fcaf0
tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat) {
    int i;

    gDuplicate_material = NULL;
    if (pStorage_space->materials_count < pStorage_space->max_materials) {
        for (i = 0; i < pStorage_space->materials_count; i++) {
            if (pStorage_space->materials[i]->identifier != NULL
                    && pThe_mat->identifier != NULL
                    && strcmp(pStorage_space->materials[i]->identifier, pThe_mat->identifier) == 0) {
                gDuplicate_material = pStorage_space->materials[i];
                return eStorage_duplicate;
            }
        }
        pStorage_space->materialProps[pStorage_space->materials_count] = 0; /* FIXME */
        pStorage_space->materials[pStorage_space->materials_count] = pThe_mat;
        pStorage_space->materials_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501260
// FUNCTION: CARMA2_SW 0x004fcbc0
tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod) {
    int i;

    gDuplicate_model = NULL;
    if (pStorage_space->models_count < pStorage_space->max_models) {
        if (pStorage_space->flags & 0x1) { /* FIXME: add enum (0x1 -> eStorage_space_flags_No_duplicates*/
            for (i = 0; i < pStorage_space->models_count; i++) {
                if (pStorage_space->models[i] != NULL
                    && pStorage_space->models[i]->identifier != NULL
                        && pThe_mod->identifier != NULL
                        && strcmp(pStorage_space->models[i]->identifier, pThe_mod->identifier) == 0) {
                    gDuplicate_model = pStorage_space->models[i];
                    return eStorage_duplicate;
                }
            }
        }
        pStorage_space->models[pStorage_space->models_count] = pThe_mod;
        pStorage_space->models_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501330
// FUNCTION: CARMA2_SW 0x004fcc90
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

// FUNCTION: CARMA2_HW 0x005024f0
// FUNCTION: CARMA2_SW 0x004fde40
int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path) {
    int i;
    int new_ones;
    int total;
    tPath_name path_dirname;
    char path_stem[32];
    br_pixelmap* temp_array[500];

    total = 0;
    if (gDisableTiffConversion) {
        SepDirAndFilename(path, path_dirname, path_stem);
        new_ones = LoadBunchOfPixies(path_dirname, path_stem, temp_array, REC2_ASIZE(temp_array));
    } else {
        new_ones = DRPixelmapLoadMany(path, temp_array, REC2_ASIZE(temp_array));
    }
    if (new_ones == 0) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, path);
    }
    for (i = 0; i < new_ones; i++) {
        if (temp_array[i] != NULL) {
            EnsurePixelmapAllowed(temp_array[i], 1);
            switch (AddPixelmapToStorage(pStorage_space, temp_array[i])) {
            case eStorage_allocated:
                BrMapAdd(temp_array[i]);
                total += 1;
                break;
            case eStorage_duplicate:
                if (gDisallow_duplicates) {
                    FatalError(kFatalError_DuplicatePixelmap_S, temp_array[i]->identifier);
                } else {
                    BrPixelmapFree(temp_array[i]);
                }
                break;
            case eStorage_not_enough_room:
                FatalError(kFatalError_InsufficientPixelmapSlots);
                break;
            }
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x00501560
// FUNCTION: CARMA2_SW 0x004fcec0
br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName) {
    br_pixelmap* map;
    tAdd_to_storage_result addResult;

    map = LoadPixelmap(pName);
    if (map == NULL) {
        return BrMapFind(pName);
    }

    addResult = AddPixelmapToStorage(pStorage, map);
    switch (addResult) {
    case eStorage_allocated:
        BrMapAdd(map);
        return map;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicatePixelmap_S, map->identifier);
        } else {
            BrPixelmapFree(map);
        }
        return gDuplicate_pixelmap;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientPixelmapSlots);
        break;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x005016a0
// FUNCTION: CARMA2_SW 0x004fd000
br_pixelmap* C2_HOOK_FASTCALL LoadSingleShadeTable(tBrender_storage* pStorage_space, const char* pName) {
    br_pixelmap* temp;

    temp = LoadShadeTable(pName);
    if (temp == NULL) {
        return BrTableFind(pName);
    }

    switch (AddShadeTableToStorage(pStorage_space, temp)) {
    case eStorage_allocated:
        BrTableAdd(temp);
        return temp;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicatePixelmap_S, temp->identifier);
        } else {
            BrPixelmapFree(temp);
        }
        return BrTableFind(pName);
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientShadeTableSlots);
        break;
    }

    return NULL;
}

// FUNCTION: CARMA2_HW 0x005017e0
// FUNCTION: CARMA2_SW 0x004fd140
br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName) {
    br_material* material;

    material = LoadMaterial(pName);
    if (material == NULL) {
        return BrMaterialFind(pName);
    }

    switch (AddMaterialToStorage(pStorage_space, material)) {
    case eStorage_allocated:
        BrMaterialAdd(material);
        return material;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicateMaterial_S, material->identifier);
        } else {
            BrMaterialFree(material);
        }
        return gDuplicate_material;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientMaterialSlots);
        return NULL;
    }
#ifdef REC2_FIX_BUGS
    return NULL;
#endif
}

// FUNCTION: CARMA2_HW 0x00501930
// FUNCTION: CARMA2_SW 0x004fd290
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

int C2_HOOK_FASTCALL AddShadeTables(tBrender_storage* pStorage_space, const char* pPath) {
    br_pixelmap* temp_array[50];
    int total;
    int i;

    total = BrPixelmapLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadShadeTableFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddShadeTableToStorage(pStorage_space, temp_array[i])) {
        case eStorage_allocated:
            BrTableAdd(temp_array[i]);
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicatePixelmap_S, temp_array[i]->identifier);
            } else {
                BrPixelmapFree(temp_array[i]);
            }
            break;
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientShadeTableSlots);
            break;
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x00501e40
// FUNCTION: CARMA2_SW 0x004fd7a0
int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath) {
    int i;
    int new_ones;
    int total;
    br_model* temp_array[2000];

    new_ones = 0;
    total = BrModelLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    WhitenVertexRGB(temp_array, total);
    if (total == 0) {
        FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddModelToStorage(pStorage_space, temp_array[i])) {
        case eStorage_allocated:
            temp_array[i]->flags |= BR_MODF_UPDATEABLE;
            BrModelAdd(temp_array[i]);
            new_ones++;
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicateModel_S, temp_array[i]->identifier);
            } else {
                BrModelFree(temp_array[i]);
            }
            break;
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientModelSlots);
            break;
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x00502060
// FUNCTION: CARMA2_SW 0x004fd9b0
int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading) {
    int i;
    int new_ones;
    int total;
    br_material* temp_array[500];

    new_ones = 0;
    total = BrMaterialLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadMaterialFileOrItIsEmpty_S, pPath);
    }
    GlorifyMaterial(temp_array, total, pShading);
    for (i = 0; i < total; ++i) {
        if (temp_array[i] != NULL) {
            switch (AddMaterialToStorage(pStorage_space, temp_array[i])) {
            case eStorage_allocated:
                BrMaterialAdd(temp_array[i]);
                new_ones += 1;
                break;
            case eStorage_duplicate:
                if (gDisallow_duplicates) {
                    FatalError(kFatalError_DuplicateMaterial_S, temp_array[i]->identifier);
                } else {
                    BrMaterialFree(temp_array[i]);
                }
                break;
            case eStorage_not_enough_room:
                FatalError(kFatalError_InsufficientMaterialSlots);
                break;
            }
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x00502210
// FUNCTION: CARMA2_SW 0x004fdb60
void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM) {

    BrResFree(pM->faces);
    BrResFree(pM->vertices);
    pM->nfaces = 0;
    pM->nvertices = 0;
    pM->faces = NULL;
    pM->vertices = NULL;
}

// FUNCTION: CARMA2_HW 0x005024b0
// FUNCTION: CARMA2_SW 0x004fde00
void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".PIX") != NULL) {
        AddPixelmaps(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502490
// FUNCTION: CARMA2_SW 0x004fdde0
void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAPixelmap);
}

// FUNCTION: CARMA2_HW 0x005026b0
// FUNCTION: CARMA2_SW 0x004fe000
int C2_HOOK_FASTCALL GetFileName(const char *path, tName_list *pList) {
    tPath_name pathCopy;
    tPath_name pathUpper;
    tPath_name dir_path;
    tPath_name stem_path;

    strcpy(pathCopy, path);
    Uppercaseificate(pathUpper, pathCopy);
    if (strstr(pathUpper, ".PIX") == NULL
            && strstr(pathUpper, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(pathUpper, dir_path, stem_path);
    strcpy(pList->items[pList->size], stem_path);
    if (pList->size < (int)REC2_ASIZE(pList->items)) {
        pList->size += 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00502780
// FUNCTION: CARMA2_SW 0x004fe0d0
int C2_HOOK_FASTCALL GetAdditionalFileName(const char* path, tName_list* pList) {
    tPath_name pathCopy;
    tPath_name upperPath;
    tPath_name dir_path;
    tPath_name stem_path;
    int alreadyInList;
    int i;

    strcpy(pathCopy, path);
    Uppercaseificate(upperPath, pathCopy);

    if (strstr(upperPath, ".PIX") == NULL
            && strstr(upperPath, ".P16") == NULL
            && strstr(upperPath, ".P08") == NULL
            && strstr(upperPath, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(upperPath, dir_path, stem_path);

    for (i = 0, alreadyInList = 0; i < pList->size; i++) {
        if (strcmp(pList->items[i], stem_path) == 0) {
            alreadyInList = 1;
            break;
        }
    }
    if (!alreadyInList) {
        strcpy(pList->items[pList->size], stem_path);
        if (pList->size < (int)REC2_ASIZE(pList->items)) {
            pList->size += 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005028f0
// FUNCTION: CARMA2_SW 0x004fe240
void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path) {
    tPath_name pathCopy;
    tPath_name pixPath;
    tPath_name tifPath;
    tName_list list;
    int i;

    C2_HOOK_BUG_ON(sizeof(tBrender_storage) != 68);

    gStorage_for_callbacks = pStorage_space;
    // TwatPIX16(path);
    list.size = 0;
    strcpy(pathCopy, path);
    if (gDisableTiffConversion) {
        PFForEveryFile2(pathCopy, (tEnumPathCallback)GetAdditionalFileName, &list);
    }
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFX");
        PFForEveryFile2(tifPath, (tEnumPathCallback)GetFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX8");
    PFForEveryFile2(pixPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFRGB");
        PFForEveryFile2(tifPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX16");
    PFForEveryFile2(pixPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    for (i = 0; i < list.size; i++) {
        PathCat(pathCopy, path, list.items[i]);
        AddPixelmaps(gStorage_for_callbacks, pathCopy);
    }
}

// FUNCTION: CARMA2_HW 0x00502a70
// FUNCTION: CARMA2_SW 0x004fe3c0
void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath) {
    char path[256];
    tRendererShadingType shading;

    if (gMaterial_shading_for_callback == kRendererShadingType_Undefined) {
        shading = kRendererShadingType_Default;
    } else {
        shading = gMaterial_shading_for_callback;
    }
    Uppercaseificate(path, pPath);
    if (strstr(path, ".MAT") != NULL) {
        AddMaterials(gStorage_for_callbacks, pPath, shading);
    }
}

// FUNCTION: CARMA2_HW 0x00502ad0
// FUNCTION: CARMA2_SW 0x004fe420
void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    gMaterial_shading_for_callback = pShading;
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
}

// FUNCTION: CARMA2_HW 0x00502b20
// FUNCTION: CARMA2_SW 0x004fe470
void C2_HOOK_FASTCALL LoadIfItsAModel(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".DAT") != NULL) {
        AddModels(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b00
// FUNCTION: CARMA2_SW 0x004fe450
void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAModel);
}

// FUNCTION: CARMA2_HW 0x00502b80
// FUNCTION: CARMA2_SW 0x004fe4d0
void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath) {
    char path[256];

    Uppercaseificate(path, pPath);
    if (strstr(path, ".TAB") != NULL) {
        AddShadeTables(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b60
// FUNCTION: CARMA2_SW 0x004fe4b0
void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAShadeTable);
}

// FUNCTION: CARMA2_HW 0x00502cf0
// FUNCTION: CARMA2_SW 0x004fe640
void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    LoadAllShadeTablesInDirectory(pStorage, pPath);
    LoadAllPixelmapsInDirectory(pStorage, pPath);
    LoadAllImagesInDirectory(pStorage, pPath);
    LoadAllMaterialsInDirectory(pStorage, pPath, pShading);
    LoadAllModelsInDirectory(pStorage, pPath);
}

// FUNCTION: CARMA2_HW 0x00502d60
// FUNCTION: CARMA2_SW 0x004fe6b0
void C2_HOOK_FASTCALL DisallowDuplicates(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00502d70
// FUNCTION: CARMA2_SW 0x004fe6c0
void C2_HOOK_FASTCALL AllowDuplicates(void) {

    gDisallow_duplicates = 0;
}

// FUNCTION: CARMA2_HW 0x00502d80
br_pixelmap* C2_HOOK_FASTCALL SwapPixelmapInStorage(br_pixelmap* pNew, br_pixelmap* pOriginal, tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->pixelmaps_count; i++) {
        if (pOriginal == pStorage->pixelmaps[i]) {
            BrMapRemove(pOriginal);
            BrPixelmapFree(pOriginal);
            pStorage->pixelmaps[i] = pNew;
            BrMapAdd(pNew);
            return pNew;
        }
    }
    PDFatalError("Can't swap in pixelmap");
    return NULL;
}

extern FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name);
extern tPed_remap* gPed_remaps[10];
extern tPed_move* gPed_moves[250];
extern tPed_move* C2_HOOK_FASTCALL ReadMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags);
extern br_actor* gMr_blendy;
extern void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face* pFace, br_model* pModel);

// GLOBAL: CARMA2_HW 0x0058f290
const char* gPed_remap_axis_choices[6] = {
    "+X",
    "+Y",
    "+Z",
    "-X",
    "-Y",
    "-Z",
};

// FUNCTION: CARMA2_HW 0x00404410
tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char* pFile_name) {
    FILE* f;
    tPed_remap* remap;
    char s[256];
    int i;
    int j;

    if (strcmp(pFile_name, "NONE") == 0) {
        return NULL;
    }
    f = BonerOpenRemaps(pFile_name);
    if (f == NULL) {
        FatalError(kFatalError_BonerError_UnableToOpenFile_S, pFile_name);
    }
    remap = BrMemAllocate(sizeof(tPed_remap), kBoner_mem_type_remap);
    if (remap == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory, 0);
    }
    remap->count_bones = 0;
    GetALineAndDontArgue(f, s);
    if (strcmp("START OF BONES", s) != 0) {
        FatalError(kFatalError_BonerError_SyntaxErrorInFormFileExpected_S, "START OF BONES");
    }
    while (!PFfeof(f)) {
        GetALineAndDontArgue(f, s);
        if ((remap->count_bones == 0 && strcmp("END OF BONES", s) != 0) || strcmp("NEXT BONE", s) == 0) {
            remap->count_bones++;
        }
    }
    remap->bones = BrMemAllocate(sizeof(tPed_remap_bone) * remap->count_bones, kBoner_mem_type_remap);
    PFrewind(f);
    GetALineAndDontArgue(f, s);
    for (i = 0; i < remap->count_bones; i++) {
        GetALineAndDontArgue(f, remap->bones[i].name);
        for (j = 0; j < 3; j++) {
            remap->bones[i].general_remap[j] = GetALineAndInterpretCommand(f, gPed_remap_axis_choices, REC2_ASIZE(gPed_remap_axis_choices));
        }
        for (j = 0; j < 3; j++) {
            remap->bones[i].powerup_axis[j] = GetALineAndInterpretCommand(f, gPed_remap_axis_choices, REC2_ASIZE(gPed_remap_axis_choices));
        }
        GetALineAndDontArgue(f, s);
    }
    strcpy(remap->name, pFile_name);
    for (i = 0; i < (int)REC2_ASIZE(gPed_remaps); i++) {
        if (gPed_remaps[i] == NULL) {
            gPed_remaps[i] = remap;
            return remap;
        }
    }
    FatalError(kFatalError_BonerError_TooManyRemapsLoaded, 0);
}

// FUNCTION: CARMA2_HW 0x004047b0
tPed_move* C2_HOOK_FASTCALL FindOrOpenMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags) {
    int i;

    for (i = 0; i < 250; i++) {
        if (gPed_moves[i] != NULL && strcmp(pName, gPed_moves[i]->name) == 0) {
            return gPed_moves[i];
        }
    }
    return ReadMove(pName, pForm, pLooping_reset_flags, pMovement_direction, pFlags);
}

// FUNCTION: CARMA2_HW 0x0040d530
void C2_HOOK_FASTCALL MungeFaces(br_actor* pActor, br_model* pModel) {
    int i;
    int j;
    int k;
    br_face* face;
    br_face* blendy_face;
    br_vertex* bv;
    int changed_one;

    changed_one = 0;

    for (i = 0, face = pModel->faces; i < pModel->nfaces; i++, face++) {
        SpawnPedsOnFace(face, pModel);
#ifdef REC2_MATCHING
        if (((int (C2_HOOK_FASTCALL *)(br_material*, br_model*))SmashFaceMustBeUpdateable)(face->material, pModel)) {
#else
        if (SmashFaceMustBeUpdateable(face->material)) {
#endif
            if (gMr_blendy == NULL) {
                gMr_blendy = BrActorAllocate(BR_ACTOR_MODEL, NULL);
                gMr_blendy->render_style = BR_RSTYLE_FACES;
                gMr_blendy->model = BrModelAllocate(NULL, 2000, 2000);
                gMr_blendy->model->nfaces = 0;
                gMr_blendy->model->nvertices = 0;
                gMr_blendy->model->flags |= BR_MODF_UPDATEABLE;
            }
            blendy_face = &gMr_blendy->model->faces[gMr_blendy->model->nfaces];
            gMr_blendy->model->nfaces += 1;
            memcpy(blendy_face, face, sizeof(br_face));
            for (j = 0; j < 3; j++) {
                br_vertex* vertex_j = &pModel->vertices[face->vertices[j]];
                for (k = 0, bv = gMr_blendy->model->vertices; k < gMr_blendy->model->nvertices; k++, bv++) {
                    if (Vector3Equals(&bv->p, &vertex_j->p)
                            && Vector2Equals(&bv->map, &vertex_j->map)) {
                        blendy_face->vertices[j] = k;
                        vertex_j = NULL;
                        break;
                    }
                }
                if (vertex_j != NULL) {
                    blendy_face->vertices[j] = gMr_blendy->model->nvertices;
                    memcpy(&gMr_blendy->model->vertices[gMr_blendy->model->nvertices], vertex_j, sizeof(br_vertex));
                    gMr_blendy->model->nvertices += 1;
                }
            }
            if (i < pModel->nfaces - 1) {
                memmove(&pModel->faces[i], &pModel->faces[i + 1], (pModel->nfaces - (i + 1)) * sizeof(br_vertex));
            }
            pModel->nfaces -= 1;
            i -= 1;
            face = &pModel->faces[i];
            changed_one = 1;
        }
    }
    if (changed_one) {
        if (pModel->nfaces != 0) {
            BrModelUpdate(pModel, BR_MODU_ALL);
        } else {
            pActor->model = NULL;
            pActor->render_style = BR_RSTYLE_NONE;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d3520
void C2_HOOK_FASTCALL FinishUpLoadingPeds(void) {
    extern int gPed_count;
    extern tPedestrian* gPedestrian_array;
    tPedestrian* new_pedestrians;
    int i;

    dr_dprintf("%d pedestrians, %d bytes each, total = %d bytes\n", gPed_count, sizeof(tRace_pedestrian), gPed_count * sizeof(tRace_pedestrian));
    new_pedestrians = BrMemAllocate(gPed_count * sizeof(tPedestrian), kMem_ped_array);
    memcpy(new_pedestrians, gPedestrian_array, gPed_count * sizeof(tPedestrian));
    BrMemFree(gPedestrian_array);
    gPedestrian_array = new_pedestrians;
    for (i = 0; i < gPed_count; i++) {
        gPedestrian_array[i].character->ped = &gPedestrian_array[i];
    }
}

// OrientationChanged

// MungeSmashMaterialNames

// STUB: CARMA2_HW 0x00406ab0
tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00404f60
tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004d2cc0
void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face* pFace, br_model* pModel) { NOT_IMPLEMENTED(); }
