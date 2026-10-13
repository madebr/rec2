#ifndef GUARD_18_GRAPHICS2_H
#define GUARD_18_GRAPHICS2_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern tTransient_bm gTransient_bitmaps[50];
extern tU32 gLast_palette_change;
extern int gPalette_index;
extern br_pixelmap* gPalette_0074a604;
extern br_pixelmap* gPalette_0074a600;
extern br_pixelmap* gPalette_0074a5fc;

extern int gNoTransients;
extern br_pixelmap* gRender_palette;
extern int gDim_amount;
extern int gPalette_changed;
extern br_pixelmap* gCurrent_palette;
extern int gCursor_tinted_top;
extern int gCursor_tinted_left;
extern int gCursor_tinted_bottom;
extern int gCursor_tinted_right;
extern int gCursor_tinted_center;
extern int gCursor_line_width;
extern int gTransient_bitmap_index;
extern tMouse_coord gMouse_last_pos;
extern int gMouse_in_use;
extern int gCurrent_cursor_index;
extern int gMouse_started;
extern int gNext_transient;
extern br_pixelmap* gCurrent_splash;

extern void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pThe_palette, int pFirst_colour, int pCount);

extern void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette);

extern void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette);

extern void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette);

extern void C2_HOOK_FASTCALL InitializePalettes(void);

extern void C2_HOOK_FASTCALL InitPaletteAnimate(void);

extern void C2_HOOK_FASTCALL RevertPalette(void);

extern void C2_HOOK_FASTCALL MungePalette(void);

extern void C2_HOOK_FASTCALL ResetPalette(void);

// Darken is inlined in SetFadedPalette.

extern void C2_HOOK_FASTCALL SetFadedPalette(int pDegree);

extern void C2_HOOK_FASTCALL FadePaletteDown(void);

extern void C2_HOOK_FASTCALL FadePaletteUp(void);

extern void C2_HOOK_FASTCALL EnsurePaletteUp(void);

extern void C2_HOOK_FASTCALL EnsureRenderPalette(void);

extern void C2_HOOK_FASTCALL InitTransientBitmaps(void);

// DeallocateTransientBitmap is inlined in EndMouseCursor.

// DeallocateAllTransientBitmaps is inlined in EndMouseCursor.

extern void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);

// SaveTransient

extern int C2_HOOK_FASTCALL DoMouseCursor(void);

extern void C2_HOOK_FASTCALL AllocateCursorActors(void);

// InitMouseTargetLines

extern void C2_HOOK_FASTCALL StartMouseCursor(void);

extern void C2_HOOK_FASTCALL EndMouseCursor(void);

extern void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDest, int pX1, int pY1, int pX2, int pY2, int pColour);

extern void C2_HOOK_FASTCALL ClearEntireScreen(void);

extern double C2_HOOK_FASTCALL DistanceFromPlane(const br_vector3* pPos, br_scalar pA, br_scalar pB, br_scalar pC, br_scalar pD);

extern void C2_HOOK_FASTCALL KillSplashScreen(void);

extern void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name);

extern void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

// DRMaskedStamp

// SwitchToHiresMode

// SwitchToLoresMode

extern void C2_HOOK_FASTCALL PoshDrawLine(br_pixelmap* pDest, int pX1, br_scalar pAngle, int pY1, int pX2, int pY2, int pColour);

extern void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_CDECL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_FASTCALL DimRectangleClipped(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_FASTCALL DR8BitPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans);

extern void C2_HOOK_FASTCALL DRPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans);

extern void C2_HOOK_FASTCALL DR16BitPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans);

// DRPixelmapBlendedLine

// DR8BitFancyDrawLine

// FancyDrawLine

extern void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack8Bit(br_pixelmap* pDest, br_pixelmap* pSrc);

extern void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack(br_pixelmap* pDest, br_pixelmap* pSrc);

extern void C2_HOOK_FASTCALL DisableLights(void);

extern void C2_HOOK_FASTCALL EnableLights(void);

#endif // GUARD_18_GRAPHICS2_H
