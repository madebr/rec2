#ifndef GUARD_19_FONT_H
#define GUARD_19_FONT_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern br_pixelmap* gTexture_maps[1024];
extern int gSize_font_texture_pages;
extern br_actor* gPolyfont_glyph_actors[256];
extern int gCount_polyfont_glyph_actors;

extern int C2_HOOK_FASTCALL PolyFontHeight(int pFont_index);

extern int C2_HOOK_FASTCALL FindCharacterWidth(br_pixelmap* pMap);

extern int C2_HOOK_FASTCALL CharacterWidth(int pFont_index, tU8 pCharacter);

extern int C2_HOOK_FASTCALL GetSpacing(int pFont_index);

// SolidPolyFontTextInABox

// PolyFontTextInABox

// TransparentPolyFontTextInABox

extern void C2_HOOK_FASTCALL InitPolyFonts(void);

extern void C2_HOOK_FASTCALL CreatePolyFont(int pFont, const char* pName, float pFactor, int pSize);

extern br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmap(const char* path, const char* glyph_name, int loadFromDisk);

extern void C2_HOOK_FASTCALL KillThePixies(void);

extern void C2_HOOK_FASTCALL CheckAvailabilityOfThisFont(int pFont);

extern void C2_HOOK_FASTCALL DisposeInterfaceFonts(void);

extern void C2_HOOK_FASTCALL RemovePolyFont(int pFont);

extern void C2_HOOK_FASTCALL LoadInterfaceFonts(void);

extern void C2_HOOK_FASTCALL LoadPolyFont(const char* pName, int pSize, int pIndex);

extern br_model* C2_HOOK_FASTCALL CreateFontCharacterModel(int pIndex, int pAscii, int pWidth, int pHeight, const char* pName);

extern br_model* C2_HOOK_FASTCALL CreateCharacterModel(int width, int height, int texture_id_x, int texture_id_y, const char* pageName);

extern br_material* C2_HOOK_FASTCALL CreatePolyMaterial(int pTexture_id);

extern void C2_HOOK_FASTCALL ColourVertices(br_model* pModel, int pFont_index);

extern void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);

extern void C2_HOOK_FASTCALL InitPolyFontMaterials(void);

extern br_material* C2_HOOK_FASTCALL GetPolyFontMaterial(int pFont_index, char pCharacter);

extern void C2_HOOK_FASTCALL PolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender);

extern void C2_HOOK_FASTCALL TransparentPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender, double pOpacity_factor);

extern void C2_HOOK_FASTCALL DrawDigitAt(const br_pixelmap* pSource, br_pixelmap* pDest, br_uint_16 pX, br_uint_16 pY, br_uint_16 pDigit_height, int pDigit);

extern void C2_HOOK_FASTCALL DrawNumberAt(const br_pixelmap* pSrc, br_pixelmap* pDest, int pX, int pY, int pX_pitch, int pY_pitch, int pValue ,int pDigit_count, int pLeading_zeroes);

extern void C2_HOOK_FASTCALL LoadFont(int pFont_ID);

extern void C2_HOOK_FASTCALL DisposeFont(int pFont_ID);

extern void C2_HOOK_FASTCALL InitDRFonts(void);

extern void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);

// DRPixelmapCleverText

extern int C2_HOOK_FASTCALL PolyFontTextWidth(int pFont, const char* pText);

extern int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText);

extern int C2_HOOK_FASTCALL DRTextCleverWidth(const tDR_font* pFont, const char* pText);

// DRPixelmapCentredText

// OoerrIveGotTextInMeBoxMissus

extern br_font* C2_HOOK_FASTCALL LoadBRFont(const char* pName);

extern int C2_HOOK_FASTCALL GetPolyFontIndexToReplaceDRfontWith(const tDR_font* pFont);

#endif // GUARD_19_FONT_H
