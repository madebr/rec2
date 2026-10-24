#ifndef GUARD_18_GRAPHICS2_H
#define GUARD_18_GRAPHICS2_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern int gCursor_tinted_top;
extern int gCursor_tinted_left;
extern int gCursor_tinted_bottom;
extern int gCursor_tinted_right;
extern int gCursor_tinted_center;
extern int gCursor_line_width;

extern tTransient_bm gTransient_bitmaps[50];
extern tU32 gLast_palette_change;
extern int gPalette_index;

extern int gNoTransients;
extern br_pixelmap* gRender_palette;
extern int gDim_amount;
extern int gPalette_changed;
extern br_pixelmap* gCurrent_palette;
extern br_pixelmap* gCurrent_splash;

extern void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pThe_palette, int pFirst_colour, int pCount);

extern void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette);

extern void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette);

extern void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette);

extern void C2_HOOK_FASTCALL InitializePalettes(void);

extern void C2_HOOK_FASTCALL InitPaletteAnimate(void);

// RevertPalette

extern void C2_HOOK_FASTCALL MungePalette(void);

extern void C2_HOOK_FASTCALL ResetPalette(void);

// Darken

// SetFadedPalette

extern void C2_HOOK_FASTCALL FadePaletteDown(void);

// FadePaletteUp

// EnsurePaletteUp

extern void C2_HOOK_FASTCALL EnsureRenderPalette(void);

extern void C2_HOOK_FASTCALL InitTransientBitmaps(void);

// DeallocateTransientBitmap

// DeallocateAllTransientBitmaps

extern void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);

// SaveTransient

extern int C2_HOOK_FASTCALL DoMouseCursor(void);

extern void C2_HOOK_FASTCALL AllocateCursorTransient(void);

// InitMouseTargetLines

extern void C2_HOOK_FASTCALL StartMouseCursor(void);

extern void C2_HOOK_FASTCALL EndMouseCursor(void);

extern void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDestn, int pX1, int pY1, int pX2, int pY2, int pColour);

extern void C2_HOOK_FASTCALL ClearEntireScreen(void);

// DistanceFromPlane

extern void C2_HOOK_FASTCALL KillSplashScreen(void);

// SplashScreenWith

extern void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

extern void C2_HOOK_FASTCALL DRMaskedStamp(br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource);

extern int C2_HOOK_FASTCALL SwitchToHiresMode(void);

extern int C2_HOOK_FASTCALL SwitchToLoresMode(void);

// PoshDrawLine

extern void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_CDECL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_FASTCALL DimRectangleClipped(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

// DR8BitPixelmapRotatedAndFeatheredCopy

// DR16BitPixelmapRotatedAndFeatheredCopy

// DRPixelmapRotatedAndFeatheredCopy

extern void C2_HOOK_FASTCALL DRPixelmapBlendedLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, br_pixelmap* pPalette);

// DR8BitFancyDrawLine

extern void C2_HOOK_FASTCALL FancyDrawLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period);

// DRPixelmapCopyMapBlack8Bit

// DRPixelmapCopyMapBlack

extern void C2_HOOK_FASTCALL DisableLights(void);

extern void C2_HOOK_FASTCALL EnableLights(void);

extern void C2_HOOK_FASTCALL DRPixelmapBlend75Copy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource);

#endif // GUARD_18_GRAPHICS2_H
