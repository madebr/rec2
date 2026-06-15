#ifndef REC2_POLYFONT_H
#define REC2_POLYFONT_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_hooks.h"

extern int gInitial_count_font_texture_pages;
extern int gSize_font_texture_pages;
extern br_actor* gString_root_actor;
extern br_actor* gCar_icons_model_actor;
extern tCar_icon gCar_icons[128];
extern int gRender_poly_text;
extern int gInterface_polyfont_texture_pages;
extern int gInterface_fonts_loaded;

void C2_HOOK_FASTCALL CheckAvailabilityOfThisFont(int pIndex);

int C2_HOOK_FASTCALL PolyFontHeight(int pIndex);

int C2_HOOK_FASTCALL CharacterWidth(int pIndex, tU8 pCharacter);

int C2_HOOK_FASTCALL GetSpacing(int pIndex);

void C2_HOOK_FASTCALL SolidPolyFontTextInABox(int pFont, const char* pText, int pX, int pY, int pWidth, int pHeight, tJustification pJustification, undefined4 pParam_8);

void C2_HOOK_FASTCALL PolyFontTextInABox(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, int pRender);

void C2_HOOK_FASTCALL TransparentPolyFontTextInABox(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, undefined4 pUnknown, double pBlend);

int C2_HOOK_FASTCALL FindCharacterWidth(br_pixelmap* pMap);

void C2_HOOK_FASTCALL CreatePolyFont(const char* pName, int pSize, int pIndex);

void C2_HOOK_FASTCALL ConvertCarIcons(br_pixelmap* pMap);

void C2_HOOK_FASTCALL UpdateMapAndSaveOptions(void);

void C2_HOOK_FASTCALL InitPolyFonts(void);

void C2_HOOK_FASTCALL CreatePolyFontWithTimerFix(int pFont, const char* pName, float pFactor, int pSize);

void C2_HOOK_FASTCALL LoadInterfaceFonts(void);

void C2_HOOK_FASTCALL CheckAvailabilityOfThisFont(int pFont);

void C2_HOOK_FASTCALL DisposeInterfaceFonts(void);

void C2_HOOK_FASTCALL RemovePolyFont(int pFont);

int C2_HOOK_FASTCALL PolyFontTextWidth(int pFont, const char* pText);

void C2_HOOK_FASTCALL RenderPolyTextLine(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender);

void C2_HOOK_FASTCALL TransparentPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender, double pOpacity_factor);

void C2_HOOK_FASTCALL PolyClipName(char *pText, int pFont, int pWidth);

#endif //REC2_POLYFONT_H
