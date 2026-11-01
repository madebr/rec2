/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "18-graphics2.h"

#include "16-graphics1.h"
#include "25-grafdata.h"
#include "33-depth.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "66-tintedpoly.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include <limits.h>
#include <string.h>

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
tTransient_bm gTransient_bitmaps[50];

// GLOBAL: CARMA2_HW 0x006923a4
// GLOBAL: CARMA2_SW 0x0068c3fc
tU32 gLast_palette_change;

// GLOBAL: CARMA2_HW 0x006923b4
// GLOBAL: CARMA2_SW 0x0068c40c
int gPalette_index;

// GLOBAL: CARMA2_HW 0x0074ca28
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

// GLOBAL: CARMA2_HW 0x0074a670
// GLOBAL: CARMA2_SW 0x00741130
br_pixelmap* gPalette_0074a670;

// GLOBAL: CARMA2_HW 0x0074a660
// GLOBAL: CARMA2_SW 0x00741120
br_pixelmap* gBlend_palettes[4];

// GLOBAL: CARMA2_HW 0x0068be20
// GLOBAL: CARMA2_SW 0x00685e64
br_pixelmap* gCurrent_splash;

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
    gScratch_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
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

// Darken

// SetFadedPalette

// FUNCTION: CARMA2_HW 0x004b5460
// FUNCTION: CARMA2_SW 0x004b1ba0
void C2_HOOK_FASTCALL FadePaletteDown(void) {
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b5470
// STUB: CARMA2_SW 0x004b1bb0
void C2_HOOK_FASTCALL FadePaletteUp(void) {
    NOT_IMPLEMENTED();
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(on)
#endif

// EnsurePaletteUp

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

// DeallocateTransientBitmap

// DeallocateAllTransientBitmaps

// STUB: CARMA2_HW 0x0043e010
// STUB: CARMA2_SW 0x0043e060
void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them) {
    NOT_IMPLEMENTED();
}

// SaveTransient

// STUB: CARMA2_HW 0x0043e0a0
int C2_HOOK_FASTCALL DoMouseCursor(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0043e3f0
void C2_HOOK_FASTCALL AllocateCursorTransient(void) {
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

// InitMouseTargetLines

// STUB: CARMA2_HW 0x0043e6c0
void C2_HOOK_FASTCALL StartMouseCursor(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0043e710
// STUB: CARMA2_SW 0x0043e820
void C2_HOOK_FASTCALL EndMouseCursor(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0047b450
void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDestn, int pX1, int pY1, int pX2, int pY2, int pColour) {

    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
    BrPixelmapLine(pDestn, pX1, pY1, pX2, pY2, pColour);
}

// FUNCTION: CARMA2_HW 0x0047b880
// FUNCTION: CARMA2_SW 0x0047aba0
void C2_HOOK_FASTCALL ClearEntireScreen(void) {

    if (gScreen != NULL) {
        BrPixelmapFill(gScreen, gGraf_specs[gGraf_spec_index].black_value);
    }
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
    BrPixelmapFill(gDepth_buffer, 0xffffffff);
    PDScreenBufferSwap(0);
#ifdef REC2_HW
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
#endif
}

// DistanceFromPlane

// FUNCTION: CARMA2_HW 0x0047b960
// FUNCTION: CARMA2_SW 0x0047ac50
void C2_HOOK_FASTCALL KillSplashScreen(void) {

    if (gCurrent_splash != NULL) {
        BrMapRemove(gCurrent_splash);
        BrPixelmapFree(gCurrent_splash);
        gCurrent_splash = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x0047b990
// FUNCTION: CARMA2_SW 0x0047ac80
void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name) {
    br_pixelmap* map;
    tPixelFlags flags;

    map = BrMapFind(pPixmap_name);
    if (gCurrent_splash != NULL && map == gCurrent_splash) {
        return;
    }
    if (gCurrent_splash != NULL) {
        KillSplashScreen();
    }
    flags = gPixelFlags;
#ifdef REC2_SW
    gPixelFlags |= 0x8;
#endif
    gCurrent_splash = map;
    if (map == NULL) {
        gCurrent_splash = LoadPixelmap(pPixmap_name);
        BRPM_convert(gCurrent_splash, gBack_screen->type);
        if (gCurrent_splash != NULL) {
            BrMapAdd(gCurrent_splash);
        }
    }
    gPixelFlags = flags;
#ifdef REC2_SW
    if (gCurrent_splash == NULL || gCurrent_splash->map == NULL) {
        EnsureRenderPalette();
    } else {
        if (gScreen != NULL) {
            BrPixelmapFill(gScreen, gGraf_specs[gGraf_spec_index].black_value);
        }
        BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
        BrPixelmapFill(gDepth_buffer, 0xffffffff);
        PDScreenBufferSwap(0);
        FadePaletteUp();
        PDSetPalette(gCurrent_splash->map);
    }
#endif
    if (gCurrent_splash != NULL) {
        PixelmapSwapByteOrder(gCurrent_splash);
        DrPixelmapRectangleCopyPossibleLock(gBack_screen, 0, 0, gCurrent_splash, 0, 0,
            (br_int_16)gCurrent_splash->width, (br_int_16)gCurrent_splash->height);
        PDScreenBufferSwap(0);
        BrMapRemove(gCurrent_splash);
        BrPixelmapFree(gCurrent_splash);
        gCurrent_splash = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x0047ba80
// FUNCTION: CARMA2_SW 0x0047ae00
void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight) {
#ifdef REC2_SW
    int y_count;
    int x_count;
    int dest_row_wrap;
    int source_row_wrap;
    int x_delta;
    tU8 the_byte;
    tU8* source_ptr;
    tU8* dest_ptr;

    source_ptr = (tU8*)pSource->pixels + pSource->row_bytes * pSource_y + pSource_x;
    dest_ptr = (tU8*)pDest->pixels + pDest->row_bytes * pDest_y + pDest_x;
    source_row_wrap = pSource->row_bytes - pWidth;
    dest_row_wrap = pDest->row_bytes - pWidth;

    if (pDest_y < 0) {
        pHeight += pDest_y;
        if (pHeight <= 0) {
            return;
        }
        source_ptr -= pDest_y * pSource->row_bytes;
        dest_ptr -= pDest_y * pDest->row_bytes;
        pDest_y = 0;
    }
    if (pDest_y >= pDest->height) {
        return;
    }
    if (pDest_y + pHeight > pDest->height) {
        pHeight = pDest->height - pDest_y;
    }
    if (pDest_x < 0) {
        pWidth += pDest_x;
        if (pWidth <= 0) {
            return;
        }
        source_ptr -= pDest_x;
        dest_ptr -= pDest_x;
        source_row_wrap -= pDest_x;
        dest_row_wrap -= pDest_x;
        pDest_x = 0;
    }
    if (pDest_x >= pDest->width) {
        return;
    }
    if (pDest_x + pWidth > pDest->width) {
        x_delta = pDest_x + pWidth - pDest->width;
        pWidth -= x_delta;
        source_row_wrap += x_delta;
        dest_row_wrap += x_delta;
    }
    for (y_count = 0; y_count < pHeight; y_count++) {
        for (x_count = 0; x_count < pWidth; x_count++) {
            the_byte = *source_ptr;
            if (the_byte != 0) {
                *dest_ptr = the_byte;
            }
            source_ptr++;
            dest_ptr++;
        }
        source_ptr += source_row_wrap;
        dest_ptr += dest_row_wrap;
    }
#else
    int y;
    int x;
    int dest_row_wrap;
    int source_row_wrap;
    tU16 the_byte;
    tU16* source_ptr;
    tU16* dest_ptr;
    int dest_x;
    int dest_y;

    source_ptr = (tU16*)pSource->pixels + pSource->row_bytes * pSource_y / sizeof(tU16) + pSource_x;
    dest_x = pDest_x;
    dest_y = pDest_y;
    dest_ptr = (tU16*)pDest->pixels + dest_y * pDest->row_bytes / sizeof(tU16) + pDest->base_x / sizeof(tU16) + dest_x;
    source_row_wrap = pSource->row_bytes / sizeof(tU16) - pWidth;
    dest_row_wrap = pDest->row_bytes / sizeof(tU16) - pWidth;

    if (dest_y < 0) {
        pHeight += dest_y;
        if (pHeight <= 0) {
            return;
        }
        source_ptr -= dest_y * pSource->row_bytes / sizeof(tU16);
        dest_ptr -= dest_y * pDest->row_bytes / sizeof(tU16);
        dest_y = 0;
    }
    if (dest_y < pDest->height) {
        if (pDest->height < dest_y + pHeight) {
            pHeight = pDest->height - dest_y;
        }
        if (dest_x < 0) {
            pWidth += dest_x;
            if (pWidth <= 0) {
                return;
            }
            dest_x = 0;
            source_ptr -= dest_x;
            dest_ptr -= dest_x;
            source_row_wrap -= dest_x;
            dest_row_wrap -= dest_x;
        }
        if (dest_x < pDest->width) {
            if (dest_x + pWidth > pDest->width) {
                pWidth -= pWidth + dest_x - pDest->width;
                source_row_wrap += pWidth + dest_x - pDest->width;
                dest_row_wrap += pWidth + dest_x - pDest->width;
            }
            for (y = 0; y < pHeight; y++) {
                for (x = 0; x < pWidth; x++) {
                    the_byte = *source_ptr;
                    if (the_byte != 0) {
                        *dest_ptr = the_byte;
                    }
                    source_ptr++;
                    dest_ptr++;
                }
                source_ptr += source_row_wrap;
                dest_ptr += dest_row_wrap;
            }
        }
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047bc40
// FUNCTION: CARMA2_SW 0x0047af60
void C2_HOOK_FASTCALL DRMaskedStamp(br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource) {

    DRPixelmapRectangleMaskedCopy(gBack_screen, pDest_x, pDest_y, pSource, 0, 0, pSource->width, pSource->height);
}
