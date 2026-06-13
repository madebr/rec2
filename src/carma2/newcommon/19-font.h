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

// GetThisFuckingPixelmap

// KillThePixies

// CheckAvailabilityOfThisFont

// DisposeInterfaceFonts

// RemovePolyFont

// LoadInterfaceFonts

// LoadPolyFont

// CreateFontCharacterModel

extern br_model* C2_HOOK_FASTCALL CreateCharacterModel(int width, int height, int textureIdX, int textureIdY, const char* pageName);

// CreatePolyMaterial

// ColourVertices

// SolidPolyFontText

// InitPolyFontMaterials

// GetPolyFontMaterial

// PolyFontText

// TransparentPolyFontText

// DrawDigitAt

// DrawNumberAt

// LoadFont

// DisposeFont

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
