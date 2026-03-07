#include "temp.h"

#include "depth.h"
#include "52-errors.h"
#include "graphics.h"

#include "rec2_macros.h"
#include "rec2_types.h"


// GLOBAL: CARMA2_HW 0x006a6d38
int gMaterials_to_adapt_count;

// GLOBAL: CARMA2_HW 0x006a3340
br_material* gMaterials_to_adapt[200];

// GLOBAL: CARMA2_HW 0x006a8298
int gAllow_material_adapt;

// FUNCTION: CARMA2_HW 0x004ea760
void C2_HOOK_FASTCALL InitFogificateMaterials(void) {

    gMaterials_to_adapt_count = 0;
}

br_material* C2_HOOK_FASTCALL LoadTemporaryMaterial(const char* pName) {
    br_material* material;

    material  = BrMaterialFind(pName);
    if (material == NULL) {
        FatalError(kFatalError_CannotFindSmashMaterial_S, pName);
    }
    if (gMaterials_to_adapt_count < REC2_ASIZE(gMaterials_to_adapt)) {
        gMaterials_to_adapt[gMaterials_to_adapt_count] = material;
        gMaterials_to_adapt_count++;
    }
    return material;
}

// FUNCTION: CARMA2_HW 0x004ea820
void C2_HOOK_FASTCALL TemporaryMaterialStore(br_material* pMaterial) {

    if (gMaterials_to_adapt_count < REC2_ASIZE(gMaterials_to_adapt)) {
        gMaterials_to_adapt[gMaterials_to_adapt_count] = pMaterial;
        gMaterials_to_adapt_count += 1;
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

// FUNCTION: CARMA2_HW 0x004ea7c0
void C2_HOOK_FASTCALL AdaptCachedMaterials(tRendererShadingType pShading_type) {
    int i;

    if (!gAllow_material_adapt) {
        return;
    }
    for (i = 0; i < gMaterials_to_adapt_count; i++) {
        br_material* material;

        material = gMaterials_to_adapt[i];
        if (material == NULL) {
            continue;
        }
        GlorifyMaterial(&material, 1, pShading_type);
        FogAccordingToGPSCDE(material);
    }
    gMaterials_to_adapt_count = 0;
}

// FUNCTION: CARMA2_HW 0x004ea770
void C2_HOOK_FASTCALL FogificateMaterials(tRendererShadingType pShading_type) {
    int i;
    br_material* mat;

    for (i = 0; i < gMaterials_to_adapt_count; i++) {

        mat = gMaterials_to_adapt[i];
        if (mat != NULL) {
            GlorifyMaterial(&mat, 1, pShading_type);
            FogAccordingToGPSCDE(mat);
        }
    }
    gMaterials_to_adapt_count = 0;
}
