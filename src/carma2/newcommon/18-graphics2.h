#ifndef GUARD_18_GRAPHICS2_H
#define GUARD_18_GRAPHICS2_H

#include "c2_hooks.h"
#include "brender/brender.h"

extern int gNoTransients;
extern br_pixelmap* gRender_palette;
extern int gDim_amount;

// DRSetPaletteEntries

// DRSetPalette3

// DRSetPalette2

// DRSetPalette

extern void C2_HOOK_FASTCALL InitializePalettes(void);

extern void C2_HOOK_FASTCALL InitPaletteAnimate(void);

// RevertPalette

// MungePalette

// ResetPalette

// Darken

// SetFadedPalette

// FadePaletteDown

// FadePaletteUp

// EnsurePaletteUp

// EnsureRenderPalette

extern void C2_HOOK_FASTCALL InitTransientBitmaps(void);

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

extern void C2_HOOK_FASTCALL ClearEntireScreen(void);

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

#endif // GUARD_18_GRAPHICS2_H
