#include "18-graphics2.h"

// GLOBAL: CARMA2_HW 0x0074ca28
int gNoTransients;

// GLOBAL: CARMA2_HW 0x0074a674
br_pixelmap* gRender_palette;

// DRSetPaletteEntries

// DRSetPalette3

// DRSetPalette2

// DRSetPalette

// InitializePalettes

// STUB: CARMA2_HW 0x004b52a0
void C2_HOOK_FASTCALL InitPaletteAnimate(void) {
    NOT_IMPLEMENTED();
}

// RevertPalette

// MungePalette

// ResetPalette

// Darken

// SetFadedPalette

// FadePaletteDown

// FadePaletteUp

// EnsurePaletteUp

// EnsureRenderPalette

// STUB: CARMA2_HW 0x0043dff0
void C2_HOOK_FASTCALL InitTransientBitmaps(void) {
    NOT_IMPLEMENTED();
}

// DeallocateTransientBitmap

// DeallocateAllTransientBitmaps

// RemoveTransientBitmaps

// SaveTransient

// DoMouseCursor

// AllocateCursorTransient

// InitMouseTargetLines

// StartMouseCursor

// EndMouseCursor

// DRDrawLine

// STUB: CARMA2_HW 0x0047b880
void C2_HOOK_FASTCALL ClearEntireScreen(void) {
    NOT_IMPLEMENTED();
}

// DistanceFromPlane

// KillSplashScreen

// SplashScreenWith

// DRPixelmapRectangleMaskedCopy

// DRMaskedStamp

// SwitchToHiresMode

// SwitchToLoresMode

// PoshDrawLine

// DeviouslyDimRectangle

// DimRectangle

// DimRectangleClipped

// DR8BitPixelmapRotatedAndFeatheredCopy

// DR16BitPixelmapRotatedAndFeatheredCopy

// DRPixelmapRotatedAndFeatheredCopy

// DRPixelmapBlendedLine

// DR8BitFancyDrawLine

// FancyDrawLine

// DRPixelmapCopyMapBlack8Bit

// DRPixelmapCopyMapBlack

// DisableLights

// EnableLights

