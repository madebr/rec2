#include "67-lighting.h"

#include "08-loading1.h"
#include "28-world3.h"
#include "41-utility.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
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

// LoadTrackMaterials

// LoadCarMaterials

// SmoothlySetCarMaterialFlags

// SmoothlySetWorldMaterialFlags

// SmoothificatePowerups

// SmoothificatePowerupMaterial

// SmoothificateWorldMaterial

// SmoothificateCarMaterial

// FUNCTION: CARMA2_HW 0x00486d60
void C2_HOOK_FASTCALL MungeLightingEffects(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486d70
void C2_HOOK_FASTCALL KillLightingEffects(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486d80
void C2_HOOK_FASTCALL InitLightingEffects(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486db0
void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* pRace_lighting_path) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00486dc0
void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF) {
    GetThreeInts(pF, &gLighting_data.directional.red, &gLighting_data.directional.green, &gLighting_data.directional.blue);
    GetPairOfScalars(pF, &gLighting_data.ambient_0, &gLighting_data.diffuse_0);
    GetPairOfScalars(pF, &gLighting_data.ambient_1, &gLighting_data.diffuse_1);
    GetPairOfScalars(pF, &gLighting_data.ambient_else, &gLighting_data.diffuse_else);
}

