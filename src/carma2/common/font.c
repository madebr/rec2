#include "font.h"

#include "errors.h"
#include "loading.h"
#include "utility.h"

#include "rec2_macros.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gTextureMaps, 1000, 0x0076c960);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPolyFontBorderColours, gPolyFontBorderColours, 27, 0x00765ec0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gPolyFontMaterials, 80, 0x00686340);
C2_HOOK_VARIABLE_IMPLEMENT(int, polyFontMaterialCounter, 0x00686498); // FIXME: unknown purpose
C2_HOOK_VARIABLE_IMPLEMENT(int, currentPolyFontMaterialIdx, 0x00686480);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPolyFont, gPolyFonts, 27, 0x0076d960);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gPixelmapBuffer, 1000, 0x00764f00);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPixelmapBufferSize, 0x007663d0);

#define POLYFONT_MATERIAL_STORE_PROPS(MATERIAL, CHARACTER, FONTIDX, COUNTER)  do { (MATERIAL)->user = (void*)(uintptr_t)((CHARACTER) | ((FONTIDX) << 8) | ((COUNTER) << 16)); } while (0)
#define POLYFONT_MATERIAL_GET_CHARACTER(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 0) & 0xff)
#define POLYFONT_MATERIAL_GET_FONTIDX(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 8) & 0xff)
#define POLYFONT_MATERIAL_GET_COUNTER(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 16) & 0xffff)

br_pixelmap* C2_HOOK_FASTCALL LoadPolyFontPixiesP16(const char* path, const char* glyphName, int loadFromDisk) {
    tPath_name pathBuffer;
    FILE* f;
    char* str;
    int i;

    PathCat(pathBuffer, path, "PIXIES.P16");
    f = TWT_fopen(pathBuffer, "rb");
    if (f == NULL) {
        PathCat(pathBuffer, path, glyphName);
        return DRLoadUpdatePixelmapFromTif(pathBuffer);
    }
    DRfclose(f);
    if (loadFromDisk) {
        C2V(gPixelmapBufferSize) = BrPixelmapLoadMany(pathBuffer, C2V(gPixelmapBuffer), REC2_ASIZE(C2V(gPixelmapBuffer)));
    }
    c2_strcpy(pathBuffer, glyphName);

    str = c2_strchr(pathBuffer, '.');
    *str = '\0';

    for (i = 0; i < C2V(gPixelmapBufferSize); i++) {
        if (C2V(gPixelmapBuffer)[i] != NULL && DRStricmp(C2V(gPixelmapBuffer)[i]->identifier, pathBuffer) == 0) {
            return C2V(gPixelmapBuffer)[i];
        }
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x00464a70, LoadPolyFontPixiesP16)

br_model* C2_HOOK_FASTCALL CreateStringModel(int width, int height, int textureIdX, int textureIdY, const char* pageName) {
    br_model* pModel;

    pModel = BrModelAllocate("String model", 4, 2);
    if (pModel == NULL) {
        FatalError(kFatalError_CouldNotCreateTexturesPages_S, pageName);
    }
    pModel->faces[0].vertices[0] = 0;
    pModel->faces[0].vertices[1] = 1;
    pModel->faces[0].vertices[2] = 2;
    pModel->faces[1].vertices[0] = 1;
    pModel->faces[1].vertices[1] = 3;
    pModel->faces[1].vertices[2] = 2;
    BrVector3Set(&pModel->vertices[0].p, 0.f, 0.f, -1.2f);
    BrVector3Set(&pModel->vertices[1].p, (float)width, 0.f, -1.2f);
    BrVector3Set(&pModel->vertices[2].p, 0.f, -(float)height, -1.2f);
    BrVector3Set(&pModel->vertices[3].p, (float)width, (float)-height, -1.2f);
    BrVector2Set(&pModel->vertices[0].map, (float)textureIdX / 64.f, (float)textureIdY / 64.f);
    BrVector2Set(&pModel->vertices[1].map, (float)(textureIdX + width) / 64.f, (float)textureIdY / 64.f);
    BrVector2Set(&pModel->vertices[2].map, (float)textureIdX / 64.f,  (float)(textureIdY + height) / 64.f);
    BrVector2Set(&pModel->vertices[3].map, (float)(textureIdX + width) / 64.f, (float)(textureIdY + height) / 64.f);
    pModel->flags |= BR_STATE_STATS;
    BrModelAdd(pModel);
    return pModel;
}
C2_HOOK_FUNCTION(0x00464b80, CreateStringModel)

br_material* C2_HOOK_FASTCALL CreateFontCharacterMaterial(int textureIdx) {
    br_material* material;

    material = BrMaterialAllocate("Character Material");
    if (material == NULL) {
        BrFailure("Could not create font material");
    }
    material->flags = BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    material->colour_map = C2V(gTextureMaps)[textureIdx];
    BrMaterialAdd(material);
    return material;
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
