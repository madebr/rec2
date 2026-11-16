/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "18-graphics2.h"

#include "08-loading1.h"
#include "16-graphics1.h"
#include "25-grafdata.h"
#include "33-depth.h"
#include "35-intrface.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "66-tintedpoly.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include <limits.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x00656998
// GLOBAL: CARMA2_SW 0x006517a8
int gFancy_line_center_palette_indexes[5] = { 1, 2, 3, 2, 1 };

// GLOBAL: CARMA2_HW 0x006569b0
// GLOBAL: CARMA2_SW 0x006517c0
int gFancy_line_side_palette_indexes[5] = { 1, 2, 3, 2, 1 };

// GLOBAL: CARMA2_HW 0x0058fdc8
int gCursor_tinted_top = -1;

// GLOBAL: CARMA2_HW 0x0058fdd0
int gCursor_tinted_left = -1;

// GLOBAL: CARMA2_HW 0x0058fdcc
int gCursor_tinted_bottom = -1;

// GLOBAL: CARMA2_HW 0x0058fdd4
int gCursor_tinted_right = -1;

// GLOBAL: CARMA2_HW 0x0058fdd8
int gCursor_tinted_center = -1;

// GLOBAL: CARMA2_HW 0x0058fde0
int gCursor_line_width = 5;

// GLOBAL: CARMA2_HW 0x0067be98
// GLOBAL: CARMA2_SW 0x00676608
tTransient_bm gTransient_bitmaps[50] = { 0 };

// GLOBAL: CARMA2_HW 0x0067c348
// GLOBAL: CARMA2_SW 0x00676ab8
int gNext_transient = 0;

#ifdef REC2_MATCHING
// GLOBAL: CARMA2_HW 0x0067c34c
// GLOBAL: CARMA2_SW 0x00676abc
int gINT_0067c34c = 0;

// GLOBAL: CARMA2_HW 0x0067c350
// GLOBAL: CARMA2_SW 0x00676ac0
int gINT_0067c350 = 0;

// GLOBAL: CARMA2_HW 0x0067c354
// GLOBAL: CARMA2_SW 0x00676ac4
int gINT_0067c354 = 0;

// GLOBAL: CARMA2_HW 0x0067c358
// GLOBAL: CARMA2_SW 0x00676ac8
int gINT_0067c358 = 0;
#endif

// GLOBAL: CARMA2_HW 0x0067c390
// GLOBAL: CARMA2_SW 0x00676b04
int gMouse_started;

// GLOBAL: CARMA2_HW 0x006923a4
// GLOBAL: CARMA2_SW 0x0068c3fc
tU32 gLast_palette_change;

// GLOBAL: CARMA2_HW 0x006923b4
// GLOBAL: CARMA2_SW 0x0068c40c
int gPalette_index;

// GLOBAL: CARMA2_HW 0x0074ca28
// GLOBAL: CARMA2_SW 0x007430bc
int gNoTransients;

// GLOBAL: CARMA2_HW 0x0074a674
// GLOBAL: CARMA2_SW 0x00741134
br_pixelmap* gRender_palette;

// GLOBAL: CARMA2_HW 0x0079ec14
// GLOBAL: CARMA2_SW 0x00795174
int gDim_amount;

// GLOBAL: CARMA2_HW 0x0074a680
// GLOBAL: CARMA2_SW 0x00741140
char* gCurrent_palette_pixels;

#ifdef REC2_HW
// GLOBAL: CARMA2_HW 0x0074cf04
int gPalette_changed;
#endif

// GLOBAL: CARMA2_HW 0x0074a678
// GLOBAL: CARMA2_SW 0x00741138
br_pixelmap* gCurrent_palette;

// GLOBAL: CARMA2_HW 0x006923ac
// GLOBAL: CARMA2_SW 0x0068c404
br_pixelmap* gOrig_render_palette;

// GLOBAL: CARMA2_HW 0x0074a67c
// GLOBAL: CARMA2_SW 0x0074113c
br_pixelmap* gFlic_palette;

// GLOBAL: CARMA2_HW 0x006923c8
// GLOBAL: CARMA2_SW 0x0068c420
int gFaded_palette;

// GLOBAL: CARMA2_HW 0x006923c0
// GLOBAL: CARMA2_SW 0x0068c418
int gPalette_munged;

// GLOBAL: CARMA2_HW 0x006923b8
// GLOBAL: CARMA2_SW 0x0068c410
br_colour* gScratch_pixels;

// GLOBAL: CARMA2_HW 0x006923a8
// GLOBAL: CARMA2_SW 0x0068c400
br_pixelmap* gScratch_palette;

// GLOBAL: CARMA2_HW 0x0074a604
// GLOBAL: CARMA2_SW 0x007410c4
br_pixelmap* gBlend25_table;

// GLOBAL: CARMA2_HW 0x0074a600
// GLOBAL: CARMA2_SW 0x007410c0
br_pixelmap* gBlend50_table;

// GLOBAL: CARMA2_HW 0x0074a5fc
// GLOBAL: CARMA2_SW 0x007410bc
br_pixelmap* gBlend75_table;

// GLOBAL: CARMA2_HW 0x0074a5f8
// GLOBAL: CARMA2_SW 0x007410b8
int gPHIL_doing_physics;

// GLOBAL: CARMA2_HW 0x0074a670
// GLOBAL: CARMA2_SW 0x00741130
br_pixelmap* gPalette_0074a670;

// GLOBAL: CARMA2_HW 0x0074a660
// GLOBAL: CARMA2_SW 0x00741120
br_pixelmap* gBlend_palettes[4];

// GLOBAL: CARMA2_HW 0x0068be20
// GLOBAL: CARMA2_SW 0x00685e64
br_pixelmap* gCurrent_splash;

// GLOBAL: CARMA2_HW 0x0067be90
// GLOBAL: CARMA2_SW 0x00676600
int gCurrent_cursor_index;

// GLOBAL: CARMA2_HW 0x0079ecb0
// GLOBAL: CARMA2_SW 0x00795210
int gMouse_last_x_coord;

// GLOBAL: CARMA2_HW 0x0079ecb4
// GLOBAL: CARMA2_SW 0x00795240
int gMouse_last_y_coord;

// GLOBAL: CARMA2_HW 0x0067be94
// GLOBAL: CARMA2_SW 0x00676604
int gCursor_transient_index;

// FUNCTION: CARMA2_HW 0x0047d470
// FUNCTION: CARMA2_SW 0x0047d7a0
void C2_HOOK_FASTCALL DRPixelmapShadedHRectFill(br_pixelmap* pMap, int pX, int pY, int pWidth, int pHeight, br_uint_32 pColour) {

#ifdef REC2_HW
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
#endif
    BrPixelmapRectangleFill(pMap, pX, pY + 1, pWidth, pHeight - 2, pColour);
    BrPixelmapLine(pMap, pX, pY, pX + pWidth - 1, pY, pColour + 2);
    BrPixelmapLine(pMap, pX, pY + pHeight - 1, pX + pWidth - 1, pY + pHeight - 1, pColour - 2);
}

// FUNCTION: CARMA2_HW 0x0047d510
// FUNCTION: CARMA2_SW 0x0047d810
void C2_HOOK_FASTCALL DRPixelmapShadedVRectFill(br_pixelmap* pMap, int pX, int pY, int pWidth, int pHeight, br_colour pColour) {

#ifdef REC2_HW
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
#endif
    BrPixelmapRectangleFill(pMap, pX + 1, pY, pWidth - 2, pHeight, pColour);
    BrPixelmapLine(pMap, pX, pY, pX, pY + pHeight - 1, pColour + 2);
    BrPixelmapLine(pMap, pX + pWidth - 1, pY, pX + pWidth - 1, pY + pHeight - 1, pColour - 2);
}

// FUNCTION: CARMA2_HW 0x004b4fd0
// FUNCTION: CARMA2_SW 0x004b1140
void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pThe_palette, int pFirst_colour, int pCount) {

    if (!pFirst_colour) {
        ((br_int_32*)pThe_palette->pixels)[0] = 0;
    }
    memcpy(gCurrent_palette_pixels + 4 * pFirst_colour, (char*)pThe_palette->pixels + 4 * pFirst_colour, 4 * pCount);
#ifdef REC2_HW
    gPalette_changed = 0;
#endif
    if (!gFaded_palette) {
        PDSetPaletteEntries(pThe_palette, pFirst_colour, pCount);
    }
    gPalette_munged = 1;
}

void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette) {

    if (pSet_current_palette) {
        memcpy(gCurrent_palette_pixels, pThe_palette->pixels, 256 * sizeof(br_colour));
    }
#ifdef REC2_HW
    gPalette_changed = 0;
#endif
    if (!gFaded_palette) {
        PDSetPalette(pThe_palette);
    }
    if (pThe_palette != gRender_palette) {
        gPalette_munged |= 0x1;
    }
}

void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette) {

    ((br_int_32*)pThe_palette->pixels)[0] = 0;
    if (pSet_current_palette) {
        memcpy(gCurrent_palette_pixels, pThe_palette->pixels, 256 * sizeof(br_colour));
    }
#ifdef REC2_HW
    gPalette_changed = 0;
#endif
    if (!gFaded_palette) {
        PDSetPalette(pThe_palette);
    }
    gPalette_munged |= pThe_palette != gRender_palette;
}

// FUNCTION: CARMA2_HW 0x004b5030
// FUNCTION: CARMA2_SW 0x004b1190
void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette) {

    DRSetPalette2(pThe_palette, 1);
}

// FUNCTION: CARMA2_SW 0x004b1720
br_pixelmap* C2_HOOK_FASTCALL CreateBlendTable(const br_pixelmap* pPalette, const char* pName, float pF) {
    float inv_f = 1.0f - pF;
    br_pixelmap* blend_table;
    const br_colour* palette_pixels;
    br_uint_8* blend_pixels;
    tPath_name path;
    int i;
    int j;
    int k;

    blend_table = BrPixelmapAllocate(BR_PMT_INDEX_8, 256, 256, NULL, 0);
    if (blend_table->identifier != NULL) {
        BrResFree(blend_table->identifier);
    }
    blend_table->identifier = BrResStrDup(blend_table, pName);
    BrTableAdd(blend_table);
    blend_pixels = blend_table->pixels;
    palette_pixels = pPalette->pixels;
    for (i = 0; i < 256; i++) {
        for (j = 0; j < 256; j++) {
            int r = (int)(pF * BR_COLOUR_RED(palette_pixels[i]) + inv_f * BR_COLOUR_RED(palette_pixels[j]));
            int g = (int)(pF * BR_COLOUR_GRN(palette_pixels[i]) + inv_f * BR_COLOUR_GRN(palette_pixels[j]));
            int b = (int)(pF * BR_COLOUR_BLU(palette_pixels[i]) + inv_f * BR_COLOUR_BLU(palette_pixels[j]));
            int best_index = -1;
            int min_error = INT_MAX;
            for (k = 0; k < 256; k++) {
                int dr = BR_COLOUR_RED(palette_pixels[k]) - r;
                int dg = BR_COLOUR_GRN(palette_pixels[k]) - g;
                int db = BR_COLOUR_BLU(palette_pixels[k]) - b;
                int error = dr * dr + dg * dg + db * db;
                if (error < min_error) {
                    best_index = k;
                    min_error = error;
                }
            }
            *blend_pixels++ = best_index;
        }
    }
    PathCat(path, gApplication_path, "SHADETAB");
    PathCat(path, path, pName);
    BrPixelmapSave(path, blend_table);
    return blend_table;
}

// FUNCTION: CARMA2_HW 0x004b5090
// FUNCTION: CARMA2_SW 0x004b11e0
void C2_HOOK_FASTCALL InitializePalettes(void) {
    br_pixelmap* render_palette;
#ifdef REC2_SW
    tPath_name path;
#endif

    gCurrent_palette_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
#ifdef REC2_HW
    gPalette_changed = 0;
#endif
    gCurrent_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gCurrent_palette_pixels, 0);
    gRender_palette = BrTableFind("DRRENDER.PAL");
    if (gRender_palette == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
#ifdef REC2_HW
    NobbleNonzeroBlacks(gRender_palette);
#endif
    gOrig_render_palette = BrPixelmapAllocateSub(gRender_palette, 0, 0, gRender_palette->width, gRender_palette->height);
    gOrig_render_palette->pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    memcpy(gOrig_render_palette->pixels, gRender_palette->pixels, 256 * sizeof(br_uint_32));
    gFlic_palette = BrTableFind("DRACEFLC.PAL");
    if (gFlic_palette == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    render_palette = gRender_palette;
    ((br_uint_32*)render_palette->pixels)[0] = 0;
    memcpy(gCurrent_palette_pixels, render_palette->pixels, 256 * sizeof(br_uint_32));
#ifdef REC2_HW
    gPalette_changed = 0;
#endif
    if (!gFaded_palette) {
        PDSetPalette(render_palette);
    }
    gPalette_munged |= render_palette != gRender_palette;
    gScratch_pixels = BrMemAllocate(256 * sizeof(br_colour), kMem_misc);
    gScratch_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gScratch_pixels, 0);
#ifdef REC2_SW
    gBlend25_table = BrTableFind("BLEND25.TAB");
    if (gBlend25_table == NULL) {
        PathCat(path, gApplication_path,"SHADETAB");
        PathCat(path, path, "BLEND25.TAB");
        gBlend25_table = BrPixelmapLoad(path);
        if (gBlend25_table != NULL) {
          BrTableAdd(gBlend25_table);
        } else {
            gBlend25_table = CreateBlendTable(gRender_palette, "BLEND25.TAB", 0.25f);
        }
    }
    gBlend50_table = BrTableFind("BLEND50.TAB");
    if (gBlend50_table == NULL) {
        PathCat(path, gApplication_path,"SHADETAB");
        PathCat(path, path, "BLEND50.TAB");
        gBlend50_table = BrPixelmapLoad(path);
        if (gBlend50_table != NULL) {
            BrTableAdd(gBlend50_table);
        } else {
            gBlend50_table = CreateBlendTable(gRender_palette, "BLEND50.TAB", 0.50f);
        }
    }
    gBlend75_table = BrTableFind("BLEND75.TAB");
    if (gBlend75_table == NULL) {
        PathCat(path, gApplication_path,"SHADETAB");
        PathCat(path, path, "BLEND75.TAB");
        gBlend75_table = BrPixelmapLoad(path);
        if (gBlend75_table != NULL) {
            BrTableAdd(gBlend75_table);
        } else {
            gBlend75_table = CreateBlendTable(gRender_palette, "BLEND75.TAB", 0.75f);
        }
    }
#endif
    gBlend_palettes[0] = (br_pixelmap*)(intptr_t)-1; /* FIXME: invalid pointer! */
    gBlend_palettes[1] = gBlend25_table;
    gBlend_palettes[2] = gBlend50_table;
    gBlend_palettes[3] = gBlend75_table;
    gPalette_0074a670 = NULL;
}

// FUNCTION: CARMA2_HW 0x004b5230
void C2_HOOK_FASTCALL SwitchToPalette(char* pPal_name) {
    br_pixelmap* the_palette;

    the_palette = BrTableFind(pPal_name);
    if (the_palette != NULL) {
        DRSetPalette(the_palette);
    }
}

// FUNCTION: CARMA2_HW 0x004b52a0
// FUNCTION: CARMA2_SW 0x004b1a00
void C2_HOOK_FASTCALL InitPaletteAnimate(void) {

    gLast_palette_change = 0;
    gPalette_index = 0;
}

// RevertPalette

// FUNCTION: CARMA2_HW 0x004b5320
// FUNCTION: CARMA2_SW 0x004b1a70
void C2_HOOK_FASTCALL MungePalette(void) {
}

// FUNCTION: CARMA2_HW 0x004b5330
// FUNCTION: CARMA2_SW 0x004b1a80
void C2_HOOK_FASTCALL ResetPalette(void) {

    InitPaletteAnimate();
    DRSetPalette(gRender_palette);
}

void C2_HOOK_FASTCALL Darken(tU8 *pPtr, tU32 pDarken_amount) {
    unsigned int value;

    value = *pPtr;
    *pPtr = (value * pDarken_amount) / 256;
}

// FUNCTION: CARMA2_HW 0x004b5390
// FUNCTION: CARMA2_SW 0x004b1ae0
void C2_HOOK_FASTCALL SetFadedPalette(tU32 pDegree) {
    int i;

    memcpy(gScratch_pixels, gCurrent_palette->pixels, 256 * sizeof(br_colour));
    for (i = 0; i < 256; i++) {
        Darken(&((tU8*)gScratch_pixels)[4 * i + 0], pDegree);
        Darken(&((tU8*)gScratch_pixels)[4 * i + 1], pDegree);
        Darken(&((tU8*)gScratch_pixels)[4 * i + 2], pDegree);
        Darken(&((tU8*)gScratch_pixels)[4 * i + 3], pDegree);
    }
    DRSetPalette2(gScratch_palette, 0);
}

// FUNCTION: CARMA2_HW 0x004b5460
// FUNCTION: CARMA2_SW 0x004b1ba0
void C2_HOOK_FASTCALL FadePaletteDown(void) {
}

// FUNCTION: CARMA2_HW 0x004b5470
// FUNCTION: CARMA2_SW 0x004b1bb0
void C2_HOOK_FASTCALL FadePaletteUp(void) {
    int start_time;
    int the_time;

    if (!gFaded_palette) {
        return;
    }

    gFaded_palette = 0;
    start_time = PDGetTotalTime();
    while ((the_time = PDGetTotalTime() - start_time) < 500) {
        SetFadedPalette((the_time * 256) / 500);
    }
    DRSetPalette(gCurrent_palette);
}

// FUNCTION: CARMA2_HW 0x004b55f0
// FUNCTION: CARMA2_SW 0x004b1d20
void C2_HOOK_FASTCALL EnsurePaletteUp(void) {

    if (gFaded_palette) {
        FadePaletteUp();
    }
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004b5770
// FUNCTION: CARMA2_SW 0x004b1e90
void C2_HOOK_FASTCALL EnsureRenderPalette(void) {

    if (gPalette_munged) {
        ResetPalette();
        gPalette_munged = 0;
    }
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0043dff0
// FUNCTION: CARMA2_SW 0x0043e040
void C2_HOOK_FASTCALL InitTransientBitmaps(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gTransient_bitmaps); i++) {
        gTransient_bitmaps[i].pixmap = NULL;
        gTransient_bitmaps[i].in_use = 0;
    }
}

#ifdef REC2_SW
int C2_HOOK_FASTCALL AllocateTransientBitmap(int pWidth, int pHeight, int pUser_data) {
    int bm_index;

    for (bm_index = 0; bm_index < (int)REC2_ASIZE(gTransient_bitmaps); bm_index++) {
        if (gTransient_bitmaps[bm_index].pixmap == NULL) {
            gTransient_bitmaps[bm_index].pixmap = DRPixelmapAllocate(gBack_screen->type, pWidth + 8, pHeight, NULL, 0);
            gTransient_bitmaps[bm_index].in_use = 0;
            gTransient_bitmaps[bm_index].user_data = pUser_data;
            return bm_index;
        }
    }
    FatalError(kFatalError_CouldNotFindASpareTransientBitmap);
    return -1;
}
#endif

void C2_HOOK_FASTCALL DeallocateTransientBitmap(int pIndex) {

    if (gTransient_bitmaps[pIndex].pixmap != NULL) {
        BrPixelmapFree(gTransient_bitmaps[pIndex].pixmap);
        gTransient_bitmaps[pIndex].pixmap = NULL;
        gTransient_bitmaps[pIndex].in_use = 0;
    }
}

void C2_HOOK_FASTCALL DeallocateAllTransientBitmaps(void) {
    int i;
    for (i = 0; i < (int)REC2_ASIZE(gTransient_bitmaps); i++) {
        DeallocateTransientBitmap(i);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0043e010
// FUNCTION: CARMA2_SW 0x0043e060
void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them) {
    int i;
    int order_number;

    if (!gNoTransients && pGraphically_remove_them) {
        for (order_number = gNext_transient - 1; order_number >= 0; order_number--) {
            for (i = 0; i < (int)REC2_ASIZE(gTransient_bitmaps); i++) {
                if (gTransient_bitmaps[i].pixmap != NULL && order_number == gTransient_bitmaps[i].order_number) {
                    if (gTransient_bitmaps[i].in_use) {
                        BrPixelmapRectangleCopy(gBack_screen,
                            gTransient_bitmaps[i].x_coord,
                            gTransient_bitmaps[i].y_coord,
                            gTransient_bitmaps[i].pixmap,
                            0,
                            0,
                            gTransient_bitmaps[i].pixmap->width,
                            gTransient_bitmaps[i].pixmap->height);
                    }
                    break;
                }
            }
        }
    }
    gNext_transient = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SaveTransient

// STUB: CARMA2_HW 0x0043e0a0
int C2_HOOK_FASTCALL DoMouseCursor(void) {
    NOT_IMPLEMENTED();
}

int C2_HOOK_FASTCALL AllocateCursorTransient(void) {
#ifdef REC2_HW
    return 0;
#endif
#ifdef REC2_SW
    int i;
    int largest_width;
    int largest_height;

    largest_width = 0;
    largest_height = 0;
    for (i = 0; i < (int)REC2_ASIZE(gCursors); i++) {
        if (gCursors[i]->width > largest_width) {
            largest_width = gCursors[i]->width;
        }
        if (gCursors[i]->height > largest_height) {
            largest_height = gCursors[i]->height;
        }
    }
    return AllocateTransientBitmap(largest_width, largest_height, 0);
#endif
}

// FUNCTION: CARMA2_HW 0x0043e3f0
void C2_HOOK_FASTCALL InitMouseTargetLines(void) {
    br_pixelmap* cross;
    br_pixelmap* line;

    BrMapFind("mse_corn");
    cross = BrMapFind("mse_cros");
    line = BrMapFind("mse_line");
    gCursor_tinted_top = CreateTintedPoly(0, 0, gCursor_line_width, gCurrent_graf_data->height, 1, 127, 192, 0);
    gCursor_tinted_left = CreateTintedPoly(0, 0, gCurrent_graf_data->width, gCursor_line_width, 1, 127, 192, 0);
    gCursor_tinted_bottom = CreateTintedPoly(0, 0, gCursor_line_width, gCurrent_graf_data->height, 1, 127, 192, 0);
    gCursor_tinted_right = CreateTintedPoly(0, 0, gCurrent_graf_data->width, gCursor_line_width, 1, 127, 192, 0);
    gCursor_tinted_center = CreateTintedPoly(0, 0, 16, 16, 1, 127, 192, 0);
    if (line != NULL) {
        gTintedPolys[gCursor_tinted_left].material->colour_map = line;
        gTintedPolys[gCursor_tinted_left].material->ka = 1.0f;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_left].material, BR_MATU_ALL);
        gTintedPolys[gCursor_tinted_right].material->colour_map = line;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_right].material, BR_MATU_ALL);
        gTintedPolys[gCursor_tinted_top].material->colour_map = line;
        BrMatrix23Rotate(&gTintedPolys[gCursor_tinted_top].material->map_transform, BR_ANGLE_DEG(90));
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_top].material, BR_MATU_ALL);
        gTintedPolys[gCursor_tinted_bottom].material->colour_map = line;
        BrMatrix23Rotate(&gTintedPolys[gCursor_tinted_bottom].material->map_transform, BR_ANGLE_DEG(90));
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_bottom].material, BR_MATU_ALL);
    }
    if (cross != NULL) {
        gTintedPolys[gCursor_tinted_center].material->colour_map = cross;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_center].material, BR_MATU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x0043e6c0
// FUNCTION: CARMA2_SW 0x0043e750
void C2_HOOK_FASTCALL StartMouseCursor(void) {
#ifdef REC2_HW
    if (gCursor_tinted_top == -1) {
        InitMouseTargetLines();
    }
#endif
    gNext_transient = 0;
    gCursor_transient_index = AllocateCursorTransient();
    GetMousePosition(&gMouse_last_x_coord, &gMouse_last_y_coord);
    gMouse_in_use = 0;
    gCurrent_cursor_index = 2;
    gMouse_started = 1;
}

// FUNCTION: CARMA2_HW 0x0043e710
// FUNCTION: CARMA2_SW 0x0043e820
void C2_HOOK_FASTCALL EndMouseCursor(void) {

    RemoveTransientBitmaps(1);
    DeallocateAllTransientBitmaps();
    gMouse_started = 0;
}
