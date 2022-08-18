#include "font.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gTextureMaps, 1000, 0x0076c960);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPolyFontBorderColours, gPolyFontBorderColours, 27, 0x00765ec0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gPolyFontMaterials, 80, 0x00686340);
C2_HOOK_VARIABLE_IMPLEMENT(int, polyFontMaterialCounter, 0x00686498); // FIXME: unknown purpose
C2_HOOK_VARIABLE_IMPLEMENT(int, currentPolyFontMaterialIdx, 0x00686480);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPolyFont, gPolyFonts, 27, 0x0076d960);

#define POLYFONT_MATERIAL_STORE_PROPS(MATERIAL, CHARACTER, FONTIDX, COUNTER)  do { (MATERIAL)->user = (void*)(uintptr_t)((CHARACTER) | ((FONTIDX) << 8) | ((COUNTER) << 16)); } while (0)
#define POLYFONT_MATERIAL_GET_CHARACTER(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 0) & 0xff)
#define POLYFONT_MATERIAL_GET_FONTIDX(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 8) & 0xff)
#define POLYFONT_MATERIAL_GET_COUNTER(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 16) & 0xffff)

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

br_material* C2_HOOK_FASTCALL GetPolyFontMaterial(int fontIdx, char character) {
    br_material* pMaterial;
    br_scalar factor;
    unsigned char uchar;

    C2_HOOK_BUG_ON(sizeof(tPolyFont) != 0x1c68);
    C2_HOOK_BUG_ON(sizeof(tPolyFontGlyph) != 0x1c);

    uchar = character;
    pMaterial = C2V(gPolyFonts)[fontIdx].glyphs[uchar].material;
    if (pMaterial != NULL && POLYFONT_MATERIAL_GET_CHARACTER(pMaterial) == uchar && POLYFONT_MATERIAL_GET_FONTIDX(pMaterial) == fontIdx) {
        POLYFONT_MATERIAL_STORE_PROPS(pMaterial, uchar, fontIdx, C2V(polyFontMaterialCounter));
        return pMaterial;
    }
    // Find "oldest" material, and re-use
    while (1) {
        C2V(currentPolyFontMaterialIdx) += 1;
        if (C2V(currentPolyFontMaterialIdx) >= REC2_ASIZE(C2V(gPolyFontMaterials))) {
            C2V(currentPolyFontMaterialIdx) = 0;
        }
        C2V(polyFontMaterialCounter) = (C2V(polyFontMaterialCounter) + 1) & 0xffff;
        pMaterial = C2V(gPolyFontMaterials)[C2V(currentPolyFontMaterialIdx)];
        if ((C2V(polyFontMaterialCounter) - POLYFONT_MATERIAL_GET_COUNTER(pMaterial)) >= REC2_ASIZE(C2V(gPolyFontMaterials))) {
            break;
        }
    }
    C2V(gPolyFonts)[fontIdx].glyphs[uchar].material = pMaterial;
    pMaterial->colour_map = C2V(gTextureMaps)[C2V(gPolyFonts)[fontIdx].glyphs[uchar].index];
    POLYFONT_MATERIAL_STORE_PROPS(pMaterial, uchar, fontIdx, C2V(polyFontMaterialCounter));
    factor = C2V(gPolyFonts)[fontIdx].fontSize / 64.f;
    BrMatrix23Scale(&pMaterial->map_transform, factor, factor);
    pMaterial->map_transform.m[2][0] = C2V(gPolyFonts)[fontIdx].glyphs[uchar].texCoord.v[0];
    pMaterial->map_transform.m[2][1] = C2V(gPolyFonts)[fontIdx].glyphs[uchar].texCoord.v[1];
    pMaterial->extra_prim = NULL;
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
    return pMaterial;
}
C2_HOOK_FUNCTION(0x00465250, GetPolyFontMaterial)
