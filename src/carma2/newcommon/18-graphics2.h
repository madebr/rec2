#ifndef GUARD_18_GRAPHICS2_H
#define GUARD_18_GRAPHICS2_H

#include "c2_hooks.h"
#include "brender/brender.h"

extern int gNoTransients;
extern br_pixelmap* gRender_palette;
extern int gDim_amount;

extern void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pThe_palette, int pFirst_colour, int pCount);

extern void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette);

extern void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette);

extern void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette);

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

extern void C2_HOOK_FASTCALL EnableLights(void);

#endif // GUARD_18_GRAPHICS2_H
