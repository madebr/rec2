/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "67-lighting.h"

#include "08-loading1.h"
#include "28-world3.h"
#include "41-utility.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include <string.h>

// FUNCTION: CARMA2_HW 0x004f6520
// FUNCTION: CARMA2_SW 0x004f2180
void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage* pStorage, const char* pPath) {
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, SmoothlyLoadIfItsAModel);
}

// FUNCTION: CARMA2_HW 0x004f6540
void C2_HOOK_FASTCALL SmoothlyLoadIfItsAModel(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".DAT") != NULL) {
        AddSmoothModels(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x004f6580
int C2_HOOK_FASTCALL AddSmoothModels(tBrender_storage* pStorage, const char* pPath) {
    br_model* temp_array[2000];
    int count;
    int new_ones;
    int i;

    new_ones = 0;
    count = BrModelLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    WhitenVertexRGB(temp_array, count);
    if (count == 0) {
        FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < count; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddModelToStorage(pStorage, temp_array[i])) {
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientModelSlots);
            break;
        case eStorage_duplicate:
            BrModelFree(temp_array[i]);
            break;
        case eStorage_allocated:
            temp_array[i]->flags |= BR_MODF_UPDATEABLE;
            temp_array[i]->flags &= ~(BR_MODF_DONT_WELD | BR_MODF_CUSTOM_NORMALS);
            BrModelAdd(temp_array[i]);
            new_ones++;
            break;
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x004f6640
// FUNCTION: CARMA2_SW 0x004f2290
void C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath) {

    gMaterial_shading_for_callback = kRendererShadingType_Default;
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
    SmoothlySetWorldMaterialFlags(pStorage);
}

// FUNCTION: CARMA2_HW 0x004f6740
// FUNCTION: CARMA2_SW 0x004f22d0
void C2_HOOK_FASTCALL LoadCarMaterials(tBrender_storage* pStorage, const char* pPath, tCar_spec* pCar_spec) {

    gMaterial_shading_for_callback = kRendererShadingType_Specular;
    gStorage_for_callbacks = pStorage;
    pCar_spec->old_material_count = pStorage->materials_count;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    pCar_spec->new_material_count = pStorage->materials_count;
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
    SmoothlySetCarMaterialFlags(pStorage, pCar_spec);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004f67a0
// FUNCTION: CARMA2_SW 0x004f2330
void C2_HOOK_FASTCALL SmoothlySetCarMaterialFlags(tBrender_storage* pStorage, tCar_spec* pCar_spec) {
    int i;
    br_material* material;

    for (i = pCar_spec->old_material_count; i < pCar_spec->new_material_count; i++) {
        material = pStorage->materials[i];
#ifdef REC2_HW
        SmoothificateCarMaterial(material);
        material->user = material->colour_map;
#endif
#ifdef REC2_SW
        PossibleService();
        material->user = material->colour_map;
        material->flags &= ~(BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH);
        if (material != NULL && material->identifier != NULL) {
            if (material->identifier[1] == '\\') {
                switch (material->identifier[2]) {
                case '2':
                    BlendifyMaterial(material, 25);
                    break;
                case '7':
                    BlendifyMaterial(material, 75);
                    break;
                default:
                    BlendifyMaterial(material, 50);
                    break;
                }
                material->flags |= BR_MATF_TWO_SIDED;
            }
        }
        BrMaterialUpdate(material, BR_MATU_ALL);
#endif
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004f6820
// STUB: CARMA2_SW 0x004f23d0
void C2_HOOK_FASTCALL SmoothlySetWorldMaterialFlags(tBrender_storage* pStorage) {
#ifdef REC2_HW
    int i;
    for (i = 0; i < pStorage->materials_count; i++) {
        br_material* material = pStorage->materials[i];

        SmoothificateWorldMaterial(material);
    }
#endif
#ifdef REC2_SW
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x004f6900
void C2_HOOK_FASTCALL SmoothificatePowerups(br_actor* pActor) {
    br_model* model;
    int i;

    if (gNbPixelBits != 16) {
        return;
    }
    model = pActor->model;
    if (model == NULL) {
        return;
    }
    if (pActor->material != NULL) {
        SmoothificatePowerupMaterial(pActor->material);
    }
    for (i = 0; i < model->nfaces; i++) {
        if (model->faces[i].material != NULL) {
            SmoothificatePowerupMaterial(model->faces[i].material);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f6a20
void C2_HOOK_FASTCALL SmoothificatePowerupMaterial(br_material* pMaterial) {
    int need_update;

    need_update = 0;
    pMaterial->user = (void*)(uintptr_t)0x5ba0;
    if (pMaterial->ka <= 0.9999) {
        need_update = 1;
    }
    pMaterial->ka = 1.0f;
    pMaterial->kd = 1.0f;
    if (!(pMaterial->flags & BR_MATF_SMOOTH)) {
        need_update = 1;
    }
    pMaterial->flags |= BR_MATF_SMOOTH;
    if (!(pMaterial->flags & BR_MATF_LIGHT)) {
        need_update = 1;
    }
    pMaterial->flags |= BR_MATF_LIGHT;
    if (pMaterial->flags & BR_MATF_PRELIT) {
        need_update = 1;
    }
    pMaterial->flags &= ~BR_MATF_PRELIT;
    if (need_update) {
        BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
    }
}

// FUNCTION: CARMA2_HW 0x004f6a90
void C2_HOOK_FASTCALL SmoothificateWorldMaterial(br_material* pMaterial) {
#ifdef REC2_HW
    if (gNbPixelBits != 16) {
        return;
    }
    if ((uintptr_t)pMaterial->user == 0x5ba0) {
        SmoothificatePowerupMaterial(pMaterial);
    } else {
        pMaterial->user = NULL;
        pMaterial->ka = gLighting_data.ambient_else;
        pMaterial->kd = gLighting_data.diffuse_else;
        pMaterial->ks = 0.0f;
        pMaterial->flags &= ~BR_MATF_PRELIT;
        pMaterial->flags |= BR_MATF_LIGHT | BR_MATF_SMOOTH;
        BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
    }
#endif
#ifdef REC2_SW
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x004f6b50
void C2_HOOK_FASTCALL SmoothificateCarMaterial(br_material* pMaterial) {

  if (gNbPixelBits == 16) {
      pMaterial->ka = 0.6f;
      pMaterial->flags &= ~BR_MATF_PRELIT;
      pMaterial->flags |= BR_MATF_SMOOTH | BR_MATF_LIGHT;
      pMaterial->kd = 0.2f;
      BrMaterialUpdate(pMaterial,BR_MATU_ALL);
  }
}

// FUNCTION: CARMA2_HW 0x00486d60
// FUNCTION: CARMA2_SW 0x00483e90
void C2_HOOK_FASTCALL MungeLightingEffects(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486d70
// FUNCTION: CARMA2_SW 0x00483ea0
void C2_HOOK_FASTCALL KillLightingEffects(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486d80
void C2_HOOK_FASTCALL InitLightingEffects(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486db0
// FUNCTION: CARMA2_SW 0x00483ee0
void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* pRace_lighting_path) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486dc0
// FUNCTION: CARMA2_SW 0x00483ef0
void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF) {
#ifdef REC2_HW
    GetThreeInts(pF, &gLighting_data.directional.red, &gLighting_data.directional.green, &gLighting_data.directional.blue);
    GetPairOfScalars(pF, &gLighting_data.ambient_0, &gLighting_data.diffuse_0);
    GetPairOfScalars(pF, &gLighting_data.ambient_1, &gLighting_data.diffuse_1);
    GetPairOfScalars(pF, &gLighting_data.ambient_else, &gLighting_data.diffuse_else);
#endif
#ifdef REC2_SW
    GetThreeInts(pF, &gLighting_data_ambient.directional.red, &gLighting_data_ambient.directional.green, &gLighting_data_ambient.directional.blue);
    GetPairOfScalars(pF, &gLighting_data_ambient.ambient_0, &gLighting_data_diffuse.diffuse_0);
    GetPairOfScalars(pF, &gLighting_data_ambient.ambient_1, &gLighting_data_diffuse.diffuse_1);
    GetPairOfScalars(pF, &gLighting_data_ambient.ambient_else, &gLighting_data_diffuse.diffuse_else);
#endif
}

