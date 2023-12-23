#include "temp.h"

#include "depth.h"
#include "errors.h"
#include "graphics.h"

#include "rec2_macros.h"
#include "rec2_types.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gMaterials_to_adapt_count, 0x006a6d38);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gMaterials_to_adapt, 200, 0x006a3340);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAllow_material_adapt, 0x006a8298);

void C2_HOOK_FASTCALL TemporaryMaterialStorageInit(void) {

    C2V(gMaterials_to_adapt_count) = 0;
}
C2_HOOK_FUNCTION(0x004ea760, TemporaryMaterialStorageInit)

br_material* C2_HOOK_FASTCALL LoadTemporaryMaterial(const char* pName) {
    br_material* material;

    material  = BrMaterialFind(pName);
    if (material == NULL) {
        FatalError(kFatalError_CannotFindSmashMaterial_S, pName);
    }
    if (C2V(gMaterials_to_adapt_count) < REC2_ASIZE(C2V(gMaterials_to_adapt))) {
        C2V(gMaterials_to_adapt)[C2V(gMaterials_to_adapt_count)] = material;
        C2V(gMaterials_to_adapt_count)++;
    }
    return material;
}

void C2_HOOK_FASTCALL TemporaryMaterialStore(br_material* pMaterial) {

    if (C2V(gMaterials_to_adapt_count) < REC2_ASIZE(C2V(gMaterials_to_adapt))) {
        C2V(gMaterials_to_adapt)[C2V(gMaterials_to_adapt_count)] = pMaterial;
        C2V(gMaterials_to_adapt_count) += 1;
    }
}
C2_HOOK_FUNCTION(0x004ea820, TemporaryMaterialStore)

void C2_HOOK_FASTCALL EnableMaterialAdapt(void) {
    C2V(gAllow_material_adapt) = 1;
}
C2_HOOK_FUNCTION(0x004ea840, EnableMaterialAdapt)

void C2_HOOK_FASTCALL DisableMaterialAdapt(void) {
    C2V(gAllow_material_adapt) = 0;
}
C2_HOOK_FUNCTION(0x004ea850, DisableMaterialAdapt)

void C2_HOOK_FASTCALL AdaptCachedMaterials(tRendererShadingType pShading_type) {
    int i;

    if (!C2V(gAllow_material_adapt)) {
        return;
    }
    for (i = 0; i < C2V(gMaterials_to_adapt_count); i++) {
        br_material* material;

        material = C2V(gMaterials_to_adapt)[i];
        if (material == NULL) {
            continue;
        }
        AdaptMaterialsForRenderer(&material, 1, pShading_type);
        FogAccordingToGPSCDE(material);
    }
    C2V(gMaterials_to_adapt_count) = 0;
}
C2_HOOK_FUNCTION(0x004ea7c0, AdaptCachedMaterials)
