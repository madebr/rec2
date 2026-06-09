#include "19-font.h"

#include "rec2_macros.h"
#include "rec2_types.h"

// GLOBAL: CARMA2_HW 0x0076c960
br_pixelmap* gTextureMaps[1024];

// GLOBAL: CARMA2_HW 0x007663e0
tDR_font gFonts[24];

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

// CreateCharacterModel

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

// FUNCTION: CARMA2_HW 0x00465a40
void C2_HOOK_FASTCALL InitDRFonts(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gFonts); i++) {
        gFonts[i].images = NULL;
        gFonts[i].file_read_once = 0;
        gFonts[i].id = 11;
    }
}

// DRPixelmapText

// DRPixelmapCleverText

// PolyFontTextWidth

// DRTextWidth

// DRTextCleverWidth

// DRPixelmapCentredText

// OoerrIveGotTextInMeBoxMissus

// STUB: CARMA2_HW 0x00466050
br_font* C2_HOOK_FASTCALL LoadBRFont(const char* pName) {
    NOT_IMPLEMENTED();
}

// GetPolyFontIndexToReplaceDRfontWith

