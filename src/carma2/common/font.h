#ifndef REC2_FONT_H
#define REC2_FONT_H

#include "c2_hooks.h"

#include "brender/brender.h"
#include "rec2_types.h"

extern br_pixelmap* gTextureMaps[1024];
extern tPolyFontBorderColours gPolyFontBorderColours[27];
extern br_material* gPolyFontMaterials[80];
extern int polyFontMaterialCounter;
extern int currentPolyFontMaterialIdx;
extern tPolyFont gPolyFonts[27];
extern br_pixelmap* gPixelmapBuffer[1000];
extern int gPixelmapBufferSize;

br_pixelmap* C2_HOOK_FASTCALL LoadPolyFontPixiesP16(const char* path, const char* glyphName, int loadFromDisk);

br_material* C2_HOOK_FASTCALL CreateFontCharacterMaterial(int textureIdx);

br_model* C2_HOOK_FASTCALL CreateStringModel(int width, int height, int textureIdX, int textureIdY, const char* pageName);

void C2_HOOK_FASTCALL SetPolyFontBorderColours(br_model* pModel, int fontIdx);

br_material* C2_HOOK_FASTCALL GetPolyFontMaterial(int fontIdx, char character);

#endif //REC2_FONT_H