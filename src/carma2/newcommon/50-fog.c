#include "50-fog.h"

#include "33-depth.h"
#include "41-utility.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006a6d38
int gMaterials_to_adapt_count;

// GLOBAL: CARMA2_HW 0x006a8298
int gAllow_material_adapt;

// GLOBAL: CARMA2_HW 0x006a3340
br_material* gMaterials_to_adapt[200];

// FUNCTION: CARMA2_HW 0x004ea760
void C2_HOOK_FASTCALL InitFogificateMaterials(void) {

    gMaterials_to_adapt_count = 0;
}

// FUNCTION: CARMA2_HW 0x004ea770
void C2_HOOK_FASTCALL FogificateMaterials(tRendererShadingType pShading_type) {
    int i;

    for (i = 0; i < gMaterials_to_adapt_count; i++) {
        if (gMaterials_to_adapt[i] != NULL) {
            GlorifyMaterial(&gMaterials_to_adapt[i], 1, pShading_type);
            FogAccordingToGPSCDE(gMaterials_to_adapt[i]);
        }
    }
    InitFogificateMaterials();
}

// FUNCTION: CARMA2_HW 0x004ea7c0
void C2_HOOK_FASTCALL AutoFogificateMaterials(tRendererShadingType pShading_type) {

    if (gAllow_material_adapt) {
        FogificateMaterials(pShading_type);
    }
}

// FUNCTION: CARMA2_HW 0x004ea820
void C2_HOOK_FASTCALL AddFogificateMaterial(br_material* pMaterial) {
    int count = gMaterials_to_adapt_count;

    if (count < (int)REC2_ASIZE(gMaterials_to_adapt)) {
        gMaterials_to_adapt[count] = pMaterial;
        gMaterials_to_adapt_count = count + 1;
    }
}

// FUNCTION: CARMA2_HW 0x004ea840
void C2_HOOK_FASTCALL EnableAutoFogification(void) {
    gAllow_material_adapt = 1;
}

// FUNCTION: CARMA2_HW 0x004ea850
void C2_HOOK_FASTCALL DisableAutoFogification(void) {
    gAllow_material_adapt = 0;
}

// STUB: CARMA2_HW 0x004ea860
void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial) {
    NOT_IMPLEMENTED();
}

