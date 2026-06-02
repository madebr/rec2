#ifndef GUARD_18_GRAPHICS2_H
#define GUARD_18_GRAPHICS2_H

#include "c2_hooks.h"

extern int gNoTransients;

// DRSetPaletteEntries

// DRSetPalette3

// DRSetPalette2

// DRSetPalette

// InitializePalettes

// InitPaletteAnimate

// RevertPalette

// MungePalette

// ResetPalette

// Darken

// SetFadedPalette

// FadePaletteDown

// FadePaletteUp

// EnsurePaletteUp

// EnsureRenderPalette

// InitTransientBitmaps

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
