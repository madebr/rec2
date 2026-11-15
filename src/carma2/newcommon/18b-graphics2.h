/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_18B_GRAPHICS2_H
#define GUARD_18B_GRAPHICS2_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDestn, int pX1, int pY1, int pX2, int pY2, int pColour);

extern void C2_HOOK_FASTCALL ClearEntireScreen(void);

// DistanceFromPlane

extern void C2_HOOK_FASTCALL KillSplashScreen(void);

extern void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name);

extern void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

extern void C2_HOOK_FASTCALL DRMaskedStamp(br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource);

extern void C2_HOOK_FASTCALL DRPixelmapRectangleOnscreenCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

extern void C2_HOOK_FASTCALL DRPixelmapRectangleShearedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight, int pShear);

extern void C2_HOOK_FASTCALL DRPixelmapRectangleVScaledCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

extern void C2_HOOK_FASTCALL DropInImageFromTop(br_pixelmap* pImage, int pLeft, int pTop, int pTop_clip, int pBottom_clip);

extern void C2_HOOK_FASTCALL DropInImageFromBottom(br_pixelmap* pImage, int pLeft, int pTop, int pTop_clip, int pBottom_clip);

extern void C2_HOOK_FASTCALL DropOutImageThruBottom(br_pixelmap* pImage, int pLeft, int pTop, int pTop_clip, int pBottom_clip);

extern void C2_HOOK_FASTCALL DropOutImageThruTop(br_pixelmap* pImage, int pLeft, int pTop, int pTop_clip, int pBottom_clip);

extern int C2_HOOK_FASTCALL SwitchToHiresMode(void);

extern int C2_HOOK_FASTCALL SwitchToLoresMode(void);

#ifdef REC2_SW
extern void DRPixelmapDoubledCopy(br_pixelmap* pDestn, br_pixelmap* pSource, int pSource_width, int pSource_height, int pX_offset, int pY_offset);
#endif

extern void C2_HOOK_FASTCALL PoshDrawLine(br_pixelmap* pDestn, int pX1, float pAngle, int pY1, int pX2, int pY2, int pColour);

extern void C2_HOOK_FASTCALL DrawRectangle(br_pixelmap* pMap, int pLeft, int pTop, int pRight, int pBottom, br_uint_32 pColour);

extern void C2_HOOK_FASTCALL DrawRRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, br_uint_32 pColour);

extern void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_CDECL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

extern void C2_HOOK_FASTCALL DimRectangleClipped(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);

// DR8BitPixelmapRotatedAndFeatheredCopy

// DR16BitPixelmapRotatedAndFeatheredCopy

// DRPixelmapRotatedAndFeatheredCopy

// DRPixelmapBlendedLine

extern void C2_HOOK_FASTCALL DR8BitPixelmapBlendedLine(br_pixelmap* pDest, int pX1, int pY1, int pX2, int pY2, int pColour, br_pixelmap *pPalette);

extern void C2_HOOK_FASTCALL DRPixelmapBlendedLine(br_pixelmap* pDest, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, br_pixelmap* pPalette);

// DR8BitFancyDrawLine

extern void C2_HOOK_FASTCALL FancyDrawLine(br_pixelmap* pDest, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period);

// DRPixelmapCopyMapBlack8Bit

// DRPixelmapCopyMapBlack

extern void C2_HOOK_FASTCALL DisableLights(void);

extern void C2_HOOK_FASTCALL EnableLights(void);

extern void C2_HOOK_FASTCALL DRPixelmap16Copy(br_pixelmap* pDest, br_pixelmap* pSource, br_uint_16 pDest_x, br_uint_16 pDest_y, br_uint_16 pWidth, br_uint_16 pHeight);

extern void C2_HOOK_FASTCALL DRPixelmapBlendyCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource);

#endif // GUARD_18B_GRAPHICS2_H
