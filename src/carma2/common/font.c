#include "font.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gTextureMaps, 1000, 0x0076c960);

br_material* C2_HOOK_FASTCALL CreateFontCharacterMaterial(int textureIdx) {
    br_material* pMaterial;

    pMaterial = BrMaterialAllocate("Character Material");
    if (pMaterial == NULL) {
        BrFailure("Could not create font material");
    }
    pMaterial->flags = BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    pMaterial->colour_map = C2V(gTextureMaps)[textureIdx];
    BrMaterialAdd(pMaterial);
    return pMaterial;
}
C2_HOOK_FUNCTION(0x00464cf0, CreateFontCharacterMaterial)
