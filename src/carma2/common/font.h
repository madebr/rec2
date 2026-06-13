#ifndef REC2_FONT_H
#define REC2_FONT_H

#include "c2_hooks.h"

#include "brender/brender.h"
#include "rec2_types.h"

extern br_pixelmap* gTexture_maps[1024];
extern tPolyFontBorderColours gPoly_font_border_colours[27];
extern br_material* gPolyFontMaterials[80];
extern int polyFontMaterialCounter;
extern int currentPolyFontMaterialIdx;
extern tPolyFont gPoly_fonts[27];
extern br_pixelmap* gPixelmapBuffer[1000];
extern int gPixelmapBufferSize;

br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmap(const char* path, const char* glyph_name, int loadFromDisk);

br_material* C2_HOOK_FASTCALL CreateFontCharacterMaterial(int textureIdx);

br_model* C2_HOOK_FASTCALL CreateCharacterModel(int width, int height, int textureIdX, int textureIdY, const char* pageName);

void C2_HOOK_FASTCALL ColourVertices(br_model* pModel, int fontIdx);

br_material* C2_HOOK_FASTCALL GetPolyFontMaterial(int fontIdx, char character);

#endif //REC2_FONT_H
