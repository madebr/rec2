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
void C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath) {
    LoadAllMaterialsInDirectory(pStorage, pPath, kRendererShadingType_Default);
    SmoothlySetWorldMaterialFlags(pStorage);
}

// FUNCTION: CARMA2_HW 0x004f6740
void C2_HOOK_FASTCALL LoadCarMaterials(tBrender_storage* pStorage, const char* pPath, tCar_spec* pCar_spec) {
    gMaterial_shading_for_callback = kRendererShadingType_Specular;
    gStorage_for_callbacks = pStorage;
    pCar_spec->old_material_count = (tU16)pStorage->materials_count;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    pCar_spec->new_material_count = (tU16)pStorage->materials_count;
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
    SmoothlySetCarMaterialFlags(pStorage, pCar_spec);
}

// FUNCTION: CARMA2_HW 0x004f67a0
void C2_HOOK_FASTCALL SmoothlySetCarMaterialFlags(tBrender_storage* pStorage, tCar_spec* pCar_spec) {
    int i;
    br_material* material;

    for (i = pCar_spec->old_material_count; i < pCar_spec->new_material_count; i++) {
        material = pStorage->materials[i];
        SmoothificateCarMaterial(material);
        material->user = material->colour_map;
    }
}

// FUNCTION: CARMA2_HW 0x004f6820
void C2_HOOK_FASTCALL SmoothlySetWorldMaterialFlags(tBrender_storage* pStorage) {
    int i;
    br_material* material;

    for (i = 0; i < pStorage->materials_count; ++i) {
        material = pStorage->materials[i];
        SmoothificateWorldMaterial(material);
    }
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

    need_update = !(pMaterial->ka > 0.9999);
    pMaterial->user = (void*)(uintptr_t)0x5ba0;
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
}

void C2_HOOK_FASTCALL SmoothificateCarMaterial(br_material* pMaterial) {
    if (gNbPixelBits == 16) {
        pMaterial->ka = 0.6f;
        pMaterial->flags = (pMaterial->flags & ~BR_MATF_PRELIT) | BR_MATF_LIGHT | BR_MATF_SMOOTH;
        pMaterial->kd = 0.2f;
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
    }
}

// MungeLightingEffects

// FUNCTION: CARMA2_HW 0x00486d70
void C2_HOOK_FASTCALL KillLightingEffects(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486d80
void C2_HOOK_FASTCALL InitLightingEffects(void) {

    // empty
}

// LoadStaticLightingForRace

// FUNCTION: CARMA2_HW 0x00486dc0
void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF) {
    GetThreeInts(pF, &gLighting_data.directional.red, &gLighting_data.directional.green, &gLighting_data.directional.blue);
    GetPairOfScalars(pF, &gLighting_data.ambient_0, &gLighting_data.diffuse_0);
    GetPairOfScalars(pF, &gLighting_data.ambient_1, &gLighting_data.diffuse_1);
    GetPairOfScalars(pF, &gLighting_data.ambient_else, &gLighting_data.diffuse_else);
}

