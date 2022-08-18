#include "font.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gTextureMaps, 1000, 0x0076c960);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPolyFontBorderColours, gPolyFontBorderColours, 27, 0x00765ec0);

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

void C2_HOOK_FASTCALL SetPolyFontBorderColours(br_model* pModel, int fontIdx) {
    if (pModel == NULL) {
        return;
    }
    pModel->vertices[0].red = C2V(gPolyFontBorderColours)[fontIdx].tl.r;
    pModel->vertices[0].grn = C2V(gPolyFontBorderColours)[fontIdx].tl.g;
    pModel->vertices[0].blu = C2V(gPolyFontBorderColours)[fontIdx].tl.b;
    pModel->vertices[1].red = C2V(gPolyFontBorderColours)[fontIdx].tr.r;
    pModel->vertices[1].grn = C2V(gPolyFontBorderColours)[fontIdx].tr.g;
    pModel->vertices[1].blu = C2V(gPolyFontBorderColours)[fontIdx].tr.b;
    pModel->vertices[2].red = C2V(gPolyFontBorderColours)[fontIdx].bl.r;
    pModel->vertices[2].grn = C2V(gPolyFontBorderColours)[fontIdx].bl.g;
    pModel->vertices[2].blu = C2V(gPolyFontBorderColours)[fontIdx].bl.b;
    pModel->vertices[3].red = C2V(gPolyFontBorderColours)[fontIdx].br.r;
    pModel->vertices[3].grn = C2V(gPolyFontBorderColours)[fontIdx].br.g;
    pModel->vertices[3].blu = C2V(gPolyFontBorderColours)[fontIdx].br.b;
}
C2_HOOK_FUNCTION(0x00464d40, SetPolyFontBorderColours)
