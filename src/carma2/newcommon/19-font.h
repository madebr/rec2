#ifndef GUARD_19_FONT_H
#define GUARD_19_FONT_H

#include "brender/brender.h"

extern br_pixelmap* gTexture_maps[1024];

// PolyFontHeight

// FindCharacterWidth

// CharacterWidth

// GetSpacing

// SolidPolyFontTextInABox

// PolyFontTextInABox

// TransparentPolyFontTextInABox

// InitPolyFonts

// CreatePolyFont

extern br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmap(const char* path, const char* glyph_name, int loadFromDisk);

// KillThePixies

// CheckAvailabilityOfThisFont

// DisposeInterfaceFonts

// RemovePolyFont

// LoadInterfaceFonts

// LoadPolyFont

// CreateFontCharacterModel

extern br_model* C2_HOOK_FASTCALL CreateCharacterModel(int width, int height, int texture_id_x, int texture_id_y, const char* pageName);

// CreatePolyMaterial

// ColourVertices

// SolidPolyFontText

// InitPolyFontMaterials

// GetPolyFontMaterial

// PolyFontText

// TransparentPolyFontText

// DrawDigitAt

// DrawNumberAt

extern void C2_HOOK_FASTCALL LoadFont(int pFont_ID);

extern void C2_HOOK_FASTCALL DisposeFont(int pFont_ID);

extern void C2_HOOK_FASTCALL InitDRFonts(void);

// DRPixelmapText

// DRPixelmapCleverText

// PolyFontTextWidth

// DRTextWidth

// DRTextCleverWidth

// DRPixelmapCentredText

// OoerrIveGotTextInMeBoxMissus

extern br_font* C2_HOOK_FASTCALL LoadBRFont(const char* pName);

// GetPolyFontIndexToReplaceDRfontWith

#endif // GUARD_19_FONT_H
