#ifndef REC2_POLYFONT_H
#define REC2_POLYFONT_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gInitial_count_font_texture_pages);
C2_HOOK_VARIABLE_DECLARE(int, gSize_font_texture_pages);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gString_root_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gCar_icons_model_actor);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tCar_icon, gCar_icons, 128);
C2_HOOK_VARIABLE_DECLARE(int, gRender_poly_text);
C2_HOOK_VARIABLE_DECLARE(int, gInterface_polyfont_texture_pages);
C2_HOOK_VARIABLE_DECLARE(int, gInterface_fonts_loaded);

int C2_HOOK_FASTCALL PolyFontHeight(int pIndex);

int C2_HOOK_FASTCALL CharacterWidth(int pIndex, tU8 pCharacter);

int C2_HOOK_FASTCALL GetPolyFontInterCharacterSpacing(int pIndex);

void C2_HOOK_FASTCALL RenderInterfaceBlendedPolyText(int pFont, const char* pText, int pX, int pY, int pWidth, int pHeight, tJustification pJustification, undefined4 pParam_8);

void C2_HOOK_FASTCALL RenderPolyText(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, int pRender);

void C2_HOOK_FASTCALL TransparentPolyFontTextInABox(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, undefined4 pUnknown, double pBlend);

int C2_HOOK_FASTCALL CalculatePolyFontMapWidth(br_pixelmap* pMap);

void C2_HOOK_FASTCALL LoadPolyFont(const char* pName, int pSize, int pIndex);

void C2_HOOK_FASTCALL InitCarIcons(br_pixelmap* pMap);

void C2_HOOK_FASTCALL UpdateMapAndSaveOptions(void);

void C2_HOOK_FASTCALL InitPolyFonts(void);

void C2_HOOK_FASTCALL LoadPolyFontWithTimerFix(int pFont, const char* pName, float pFactor, int pSize);

void C2_HOOK_FASTCALL LoadInterfacePolyFonts(void);

void C2_HOOK_FASTCALL CheckLoadFrontendPolyFonts(int pFont);

void C2_HOOK_FASTCALL ClearInterfacePolyFonts(void);

void C2_HOOK_FASTCALL ClearPolyFontGlyphs(int pFont);

int C2_HOOK_FASTCALL GetPolyFontTextWidth(int pFont, const char* pText);

void C2_HOOK_FASTCALL RenderPolyTextLine(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender);

void C2_HOOK_FASTCALL TransparentPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender, double pBlend_factor);

#endif //REC2_POLYFONT_H
