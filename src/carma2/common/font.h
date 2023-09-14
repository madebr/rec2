#ifndef REC2_FONT_H
#define REC2_FONT_H

#include "c2_hooks.h"

#include "brender/brender.h"
#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(br_pixelmap*, gTextureMaps, 1024);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tPolyFontBorderColours, gPolyFontBorderColours, 27);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_material*, gPolyFontMaterials, 80);
C2_HOOK_VARIABLE_DECLARE(int, polyFontMaterialCounter);
C2_HOOK_VARIABLE_DECLARE(int, currentPolyFontMaterialIdx);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tPolyFont, gPolyFonts, 27);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_pixelmap*, gPixelmapBuffer, 1000);
C2_HOOK_VARIABLE_DECLARE(int, gPixelmapBufferSize);

br_material* C2_HOOK_FASTCALL CreateFontCharacterMaterial(int textureIdx);

#endif //REC2_FONT_H
