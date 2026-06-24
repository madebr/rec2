#ifndef GUARD_18_GRAPHICS2_H
#define GUARD_18_GRAPHICS2_H

#include "c2_hooks.h"
#include "brender/brender.h"

extern int gNoTransients;
extern br_pixelmap* gRender_palette;
extern int gDim_amount;
extern int gPalette_changed;
extern br_pixelmap* gCurrent_palette;

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

extern void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);

// SaveTransient

extern int C2_HOOK_FASTCALL DoMouseCursor(void);

// AllocateCursorTransient

// InitMouseTargetLines

// StartMouseCursor

extern void C2_HOOK_FASTCALL EndMouseCursor(void);

// DRDrawLine

extern void C2_HOOK_FASTCALL ClearEntireScreen(void);

// DistanceFromPlane

extern void C2_HOOK_FASTCALL KillSplashScreen(void);

// SplashScreenWith

extern void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

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
