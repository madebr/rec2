#include "67-lighting.h"

#include "28-world3.h"
#include "70-packfile.h"
#include "platform.h"

// LoadTrackModels

// SmoothlyLoadIfItsAModel

// AddSmoothModels

// LoadTrackMaterials

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

// SmoothlySetWorldMaterialFlags

// SmoothificatePowerups

// SmoothificatePowerupMaterial

// SmoothificateWorldMaterial

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

// ReadGlobalLightingInfo

