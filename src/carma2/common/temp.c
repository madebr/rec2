#include "temp.h"

#include "errors.h"

#include "rec2_macros.h"
#include "rec2_types.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gMaterials_to_adapt_count, 0x006a6d38);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gMaterials_to_adapt, 200, 0x006a3340);

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
