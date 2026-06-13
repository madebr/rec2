#include "font.h"

#include "52-errors.h"
#include "loading.h"
#include "utility.h"

#include "rec2_macros.h"

#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x0076c960
br_pixelmap* gTexture_maps[1024];

// GLOBAL: CARMA2_HW 0x00765ec0
tPolyFontBorderColours gPolyFontBorderColours[27];

// GLOBAL: CARMA2_HW 0x00686340
br_material* gPolyFontMaterials[80];

// GLOBAL: CARMA2_HW 0x00686498
int polyFontMaterialCounter; // FIXME: unknown purpose

// GLOBAL: CARMA2_HW 0x00686480
int currentPolyFontMaterialIdx;

// GLOBAL: CARMA2_HW 0x0076d960
tPolyFont gPolyFonts[27];

// GLOBAL: CARMA2_HW 0x00764f00
br_pixelmap* gPixelmapBuffer[1000];

// GLOBAL: CARMA2_HW 0x007663d0
int gPixelmapBufferSize;

#define POLYFONT_MATERIAL_STORE_PROPS(MATERIAL, CHARACTER, FONTIDX, COUNTER)  do { (MATERIAL)->user = (void*)(uintptr_t)((CHARACTER) | ((FONTIDX) << 8) | ((COUNTER) << 16)); } while (0)
#define POLYFONT_MATERIAL_GET_CHARACTER(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 0) & 0xff)
#define POLYFONT_MATERIAL_GET_FONTIDX(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 8) & 0xff)
#define POLYFONT_MATERIAL_GET_COUNTER(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 16) & 0xffff)

// FUNCTION: CARMA2_HW 0x00464a70
br_pixelmap* C2_HOOK_FASTCALL LoadPolyFontPixiesP16(const char* path, const char* glyphName, int loadFromDisk) {
    tPath_name pathBuffer;
    FILE* f;
    char* str;
    int i;

    PathCat(pathBuffer, path, "PIXIES.P16");
    f = PFfopen(pathBuffer, "rb");
    if (f == NULL) {
        PathCat(pathBuffer, path, glyphName);
        return DRLoadUpdatePixelmapFromTif(pathBuffer);
    }
    PFfclose(f);
    if (loadFromDisk) {
        gPixelmapBufferSize = BrPixelmapLoadMany(pathBuffer, gPixelmapBuffer, REC2_ASIZE(gPixelmapBuffer));
    }
    strcpy(pathBuffer, glyphName);

    str = strchr(pathBuffer, '.');
    *str = '\0';

    for (i = 0; i < gPixelmapBufferSize; i++) {
        if (gPixelmapBuffer[i] != NULL && DRStricmp(gPixelmapBuffer[i]->identifier, pathBuffer) == 0) {
            return gPixelmapBuffer[i];
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00464b80
br_model* C2_HOOK_FASTCALL CreateCharacterModel(int width, int height, int textureIdX, int textureIdY, const char* pageName) {
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

// FUNCTION: CARMA2_HW 0x00464cf0
br_material* C2_HOOK_FASTCALL CreateFontCharacterMaterial(int textureIdx) {
    br_material* material;

    material = BrMaterialAllocate("Character Material");
    if (material == NULL) {
        BrFailure("Could not create font material");
    }
    material->flags = BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    material->colour_map = gTexture_maps[textureIdx];
    BrMaterialAdd(material);
    return material;
}

// FUNCTION: CARMA2_HW 0x00464d40
void C2_HOOK_FASTCALL SetPolyFontBorderColours(br_model* pModel, int fontIdx) {
    if (pModel == NULL) {
        return;
    }
    pModel->vertices[0].red = gPolyFontBorderColours[fontIdx].tl.r;
    pModel->vertices[0].grn = gPolyFontBorderColours[fontIdx].tl.g;
    pModel->vertices[0].blu = gPolyFontBorderColours[fontIdx].tl.b;
    pModel->vertices[1].red = gPolyFontBorderColours[fontIdx].tr.r;
    pModel->vertices[1].grn = gPolyFontBorderColours[fontIdx].tr.g;
    pModel->vertices[1].blu = gPolyFontBorderColours[fontIdx].tr.b;
    pModel->vertices[2].red = gPolyFontBorderColours[fontIdx].bl.r;
    pModel->vertices[2].grn = gPolyFontBorderColours[fontIdx].bl.g;
    pModel->vertices[2].blu = gPolyFontBorderColours[fontIdx].bl.b;
    pModel->vertices[3].red = gPolyFontBorderColours[fontIdx].br.r;
    pModel->vertices[3].grn = gPolyFontBorderColours[fontIdx].br.g;
    pModel->vertices[3].blu = gPolyFontBorderColours[fontIdx].br.b;
}

// FUNCTION: CARMA2_HW 0x00465250
br_material* C2_HOOK_FASTCALL GetPolyFontMaterial(int fontIdx, char character) {
    br_material* pMaterial;
    br_scalar factor;
    unsigned char uchar;

    C2_HOOK_BUG_ON(sizeof(tPolyFont) != 0x1c68);
    C2_HOOK_BUG_ON(sizeof(tPolyFontGlyph) != 0x1c);

    uchar = character;
    pMaterial = gPolyFonts[fontIdx].glyphs[uchar].material;
    if (pMaterial != NULL && POLYFONT_MATERIAL_GET_CHARACTER(pMaterial) == uchar && POLYFONT_MATERIAL_GET_FONTIDX(pMaterial) == fontIdx) {
        POLYFONT_MATERIAL_STORE_PROPS(pMaterial, uchar, fontIdx, polyFontMaterialCounter);
        return pMaterial;
    }
    // Find "oldest" material, and re-use
    while (1) {
        currentPolyFontMaterialIdx += 1;
        if (currentPolyFontMaterialIdx >= REC2_ASIZE(gPolyFontMaterials)) {
            currentPolyFontMaterialIdx = 0;
        }
        polyFontMaterialCounter = (polyFontMaterialCounter + 1) & 0xffff;
        pMaterial = gPolyFontMaterials[currentPolyFontMaterialIdx];
        if ((polyFontMaterialCounter - POLYFONT_MATERIAL_GET_COUNTER(pMaterial)) >= REC2_ASIZE(gPolyFontMaterials)) {
            break;
        }
    }
    gPolyFonts[fontIdx].glyphs[uchar].material = pMaterial;
    pMaterial->colour_map = gTexture_maps[gPolyFonts[fontIdx].glyphs[uchar].index];
    POLYFONT_MATERIAL_STORE_PROPS(pMaterial, uchar, fontIdx, polyFontMaterialCounter);
    factor = gPolyFonts[fontIdx].fontSize / 64.f;
    BrMatrix23Scale(&pMaterial->map_transform, factor, factor);
    pMaterial->map_transform.m[2][0] = gPolyFonts[fontIdx].glyphs[uchar].texCoord.v[0];
    pMaterial->map_transform.m[2][1] = gPolyFonts[fontIdx].glyphs[uchar].texCoord.v[1];
    pMaterial->extra_prim = NULL;
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
    return pMaterial;
}
