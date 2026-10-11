#include "18-graphics2.h"

#include "25-grafdata.h"
#include "33-depth.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "66-tintedpoly.h"
#include "globvars.h"
#include "globvrbm.h"
#include "platform.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include <string.h>
#include <math.h>

// GLOBAL: CARMA2_HW 0x0067be98
tTransient_bm gTransient_bitmaps[50];

// GLOBAL: CARMA2_HW 0x006923a4
tU32 gLast_palette_change;

// GLOBAL: CARMA2_HW 0x006923b4
int gPalette_index;

// GLOBAL: CARMA2_HW 0x0074ca28
int gNoTransients;

// GLOBAL: CARMA2_HW 0x0074a674
br_pixelmap* gRender_palette;

// GLOBAL: CARMA2_HW 0x0079ec14
int gDim_amount;

// GLOBAL: CARMA2_HW 0x0074a680
char* gCurrent_palette_pixels;

// GLOBAL: CARMA2_HW 0x0074cf04
int gPalette_changed;

// GLOBAL: CARMA2_HW 0x0074a678
br_pixelmap* gCurrent_palette;

// GLOBAL: CARMA2_HW 0x006923ac
br_pixelmap* gOrig_render_palette;

// GLOBAL: CARMA2_HW 0x0074a67c
br_pixelmap* gFlic_palette;

// GLOBAL: CARMA2_HW 0x006923c8
int gFaded_palette;

// GLOBAL: CARMA2_HW 0x006923c0
int gPalette_munged;

// GLOBAL: CARMA2_HW 0x006923b8
br_colour* gScratch_pixels;

// GLOBAL: CARMA2_HW 0x006923a8
br_pixelmap* gScratch_palette;

// GLOBAL: CARMA2_HW 0x0074a604
br_pixelmap* gPalette_0074a604;

// GLOBAL: CARMA2_HW 0x0074a600
br_pixelmap* gPalette_0074a600;

// GLOBAL: CARMA2_HW 0x0074a66c
br_pixelmap* gPalette_0074a66c;

// GLOBAL: CARMA2_HW 0x0074a5fc
br_pixelmap* gPalette_0074a5fc;

// GLOBAL: CARMA2_HW 0x0074a670
br_pixelmap* gPalette_0074a670;

// GLOBAL: CARMA2_HW 0x0074a660
br_pixelmap* gMini_map_glowing_line_palettes[3];

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

// GLOBAL: CARMA2_HW 0x0067be94
int gTransient_bitmap_index;

// GLOBAL: CARMA2_HW 0x0079ecb0
tMouse_coord gMouse_last_pos;

// GLOBAL: CARMA2_HW 0x0067be90
int gCurrent_cursor_index;

// GLOBAL: CARMA2_HW 0x0067c390
int gMouse_started;

// GLOBAL: CARMA2_HW 0x0067c348
int gNext_transient;

// GLOBAL: CARMA2_HW 0x0067c398
static tU32 gCursor_last_call_time;

// GLOBAL: CARMA2_HW 0x0067c39c
static tU32 gCursor_last_required_change;

// GLOBAL: CARMA2_HW 0x0067c3a0
static int gCursor_delta_x;

// GLOBAL: CARMA2_HW 0x0067c3a4
static int gCursor_required_cursor;

// GLOBAL: CARMA2_HW 0x0067c3a8
static int gCursor_zero_count;

// GLOBAL: CARMA2_HW 0x0067c3ac
static int gCursor_button_was_down;

// GLOBAL: CARMA2_HW 0x0058fddc
static int gDraw_cursor = 1;

// GLOBAL: CARMA2_HW 0x0068be20
br_pixelmap* gCurrent_splash;

// FUNCTION: CARMA2_HW 0x004b4fd0
void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pThe_palette, int pFirst_colour, int pCount) {

    if (!pFirst_colour) {
        ((br_int_32*)pThe_palette->pixels)[0] = 0;
    }
    memcpy(gCurrent_palette_pixels + 4 * pFirst_colour, (char*)pThe_palette->pixels + 4 * pFirst_colour, 4 * pCount);
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPaletteEntries(pThe_palette, pFirst_colour, pCount);
    }
    gPalette_munged = 1;
}

void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette) {

    if (pSet_current_palette) {
        memcpy(gCurrent_palette_pixels, pThe_palette->pixels, 256 * sizeof(br_colour));
    }
    gPalette_changed = 0;
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
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(pThe_palette);
    }
    gPalette_munged |= pThe_palette != gRender_palette;
}

// FUNCTION: CARMA2_HW 0x004b5030
void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette) {

    DRSetPalette2(pThe_palette, 1);
}

// FUNCTION: CARMA2_HW 0x004b5090
void C2_HOOK_FASTCALL InitializePalettes(void) {
    br_pixelmap* render_palette;

    gCurrent_palette_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    gPalette_changed = 0;
    gCurrent_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gCurrent_palette_pixels, 0);
    gRender_palette = BrTableFind("DRRENDER.PAL");
    if (gRender_palette == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    NobbleNonzeroBlacks(gRender_palette);
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
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(render_palette);
    }
    gPalette_munged |= render_palette != gRender_palette;
    gScratch_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    gScratch_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gScratch_pixels, 0);
    gMini_map_glowing_line_palettes[0] = (br_pixelmap*)(intptr_t)-1; /* FIXME: invalid pointer! */
    gMini_map_glowing_line_palettes[1] = gPalette_0074a604;
    gMini_map_glowing_line_palettes[2] = gPalette_0074a600;
    gPalette_0074a66c = gPalette_0074a5fc;
    gPalette_0074a670 = NULL;
}

// FUNCTION: CARMA2_HW 0x004b52a0
void C2_HOOK_FASTCALL InitPaletteAnimate(void) {

    gLast_palette_change = 0;
    gPalette_index = 0;
}

// FUNCTION: CARMA2_HW 0x004b52b0
void C2_HOOK_FASTCALL RevertPalette(void) {

    memcpy(gRender_palette->pixels, gOrig_render_palette->pixels, 256 * sizeof(br_colour));
    DRSetPalette3(gRender_palette, 1);
}

// FUNCTION: CARMA2_HW 0x004b5320
void C2_HOOK_FASTCALL MungePalette(void) {
}

// FUNCTION: CARMA2_HW 0x004b5330
void C2_HOOK_FASTCALL ResetPalette(void) {

    InitPaletteAnimate();
    DRSetPalette(gRender_palette);
}

// Darken is inlined in SetFadedPalette.

// FUNCTION: CARMA2_HW 0x004b5390
void C2_HOOK_FASTCALL SetFadedPalette(int pDegree) {
    int i;
    unsigned char* bytes;

    memcpy(gScratch_pixels, gCurrent_palette->pixels, 256 * sizeof(br_colour));
    bytes = (unsigned char*)gScratch_pixels;
    for (i = 0; i < 256 * (int)sizeof(br_colour); i++) {
        bytes[i] = ((br_uint_32)bytes[i] * (br_uint_32)pDegree) >> 8;
    }
    DRSetPalette2(gScratch_palette, 0);
}

// FUNCTION: CARMA2_HW 0x004b5460
void C2_HOOK_FASTCALL FadePaletteDown(void) {
}

// FUNCTION: CARMA2_HW 0x004b5470
void C2_HOOK_FASTCALL FadePaletteUp(void) {

    if (gFaded_palette) {
        tU32 start_time;
        int elapsed;

        gFaded_palette = 0;
        start_time = PDGetTotalTime();
        elapsed = (br_int_32)(PDGetTotalTime() - start_time);
        while (elapsed < 500) {
            int degree = (br_int_32)((br_uint_32)elapsed << 8) / 500;

            SetFadedPalette(degree);
            elapsed = (br_int_32)(PDGetTotalTime() - start_time);
        }
        DRSetPalette(gCurrent_palette);
    }
}

// FUNCTION: CARMA2_HW 0x004b55f0
void C2_HOOK_FASTCALL EnsurePaletteUp(void) {

    if (gFaded_palette) {
        FadePaletteUp();
    }
}

// FUNCTION: CARMA2_HW 0x004b5770
void C2_HOOK_FASTCALL EnsureRenderPalette(void) {

    if (gPalette_munged) {
        ResetPalette();
        gPalette_munged = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0043dff0
void C2_HOOK_FASTCALL InitTransientBitmaps(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gTransient_bitmaps); i++) {
        gTransient_bitmaps[i].pixmap = NULL;
        gTransient_bitmaps[i].in_use = 0;
    }
}

// DeallocateTransientBitmap is inlined in EndMouseCursor.

// DeallocateAllTransientBitmaps is inlined in EndMouseCursor.

// FUNCTION: CARMA2_HW 0x0043e010
void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them) {
    int order;
    int i;

    if (!gNoTransients && pGraphically_remove_them) {
        for (order = (br_int_32)((tU32)gNext_transient - 1); order >= 0; order--) {
            for (i = 0; i < (int)REC2_ASIZE(gTransient_bitmaps); i++) {
                if (gTransient_bitmaps[i].pixmap != NULL && gTransient_bitmaps[i].order_number == order) {
                    if (gTransient_bitmaps[i].in_use) {
                        BrPixelmapRectangleCopy(gBack_screen,
                            gTransient_bitmaps[i].x_coord, gTransient_bitmaps[i].y_coord,
                            gTransient_bitmaps[i].pixmap, 0, 0,
                            gTransient_bitmaps[i].pixmap->width, gTransient_bitmaps[i].pixmap->height);
                    }
                    break;
                }
            }
        }
    }
    gNext_transient = 0;
}

// SaveTransient

// FUNCTION: CARMA2_HW 0x0043e0a0
int C2_HOOK_FASTCALL DoMouseCursor(void) {
    tU32 time_now;
    int delta_time;
    int pos_x;
    int pos_y;
    int mouse_moved;
    int button_down;
    int required;

    do {
        time_now = PDGetTotalTime();
        delta_time = gCursor_last_call_time == 0 ? 1000 : (br_int_32)(time_now - gCursor_last_call_time);
    } while (delta_time <= 20);
    GetMousePosition(&pos_x, &pos_y);
    mouse_moved = pos_x != gMouse_last_pos.x || pos_y != gMouse_last_pos.y;
    button_down = EitherMouseButtonDown();
    if (gMouse_in_use || mouse_moved) {
        gMouse_in_use = 1;
        if (pos_x == gMouse_last_pos.x) {
            int old_count = gCursor_zero_count;

            gCursor_zero_count = (br_int_32)((tU32)gCursor_zero_count + 1);
            if (old_count >= 5) {
                gCursor_delta_x = 0;
            }
        } else {
            gCursor_zero_count = 0;
            gCursor_delta_x = (br_int_32)(((tU32)pos_x - (tU32)gMouse_last_pos.x) * 1000u) / delta_time;
        }
        required = gCursor_delta_x < -10 ? 0 : gCursor_delta_x > 10 ? 3 : 2;
        if (required != gCursor_required_cursor && time_now - gCursor_last_required_change >= 200) {
            gCursor_last_required_change = time_now;
            gCursor_required_cursor = required;
        }
        gCurrent_cursor_index = 2;
        if (!gNoTransients) {
            int index = gTransient_bitmap_index;
            br_pixelmap* map;

            gTransient_bitmaps[index].x_coord = (br_int_32)(((tU32)pos_x - 7) & ~3u);
            gTransient_bitmaps[index].y_coord = (br_int_32)((tU32)pos_y - 7);
            gTransient_bitmaps[index].in_use = 1;
            gTransient_bitmaps[index].order_number = gNext_transient;
            gNext_transient = (br_int_32)((tU32)gNext_transient + 1);
            map = gTransient_bitmaps[index].pixmap;
            BrPixelmapRectangleCopy(map, 0, 0, gBack_screen,
                gTransient_bitmaps[index].x_coord, gTransient_bitmaps[index].y_coord, map->width, map->height);
        }
        if (gDraw_cursor == 1) {
            PossibleUnlock(1);
            SetTintedPolySize(gCursor_tinted_top, pos_x - 1, 0, gCursor_line_width, pos_y - 9);
            SetTintedPolySize(gCursor_tinted_left, 0, pos_y - 1, pos_x - 9, gCursor_line_width);
            SetTintedPolySize(gCursor_tinted_bottom, pos_x - 1, pos_y + 5 + gCursor_line_width,
                gCursor_line_width, gCurrent_graf_data->height - gCursor_line_width - pos_y + 9);
            SetTintedPolySize(gCursor_tinted_right, pos_x + 5 + gCursor_line_width, pos_y - 1,
                gCurrent_graf_data->width - gCursor_line_width - pos_x + 9, gCursor_line_width);
            SetTintedPolySize(gCursor_tinted_center, pos_x - 7, pos_y - 7, 16, 16);
            TurnTintedPolyOn(gCursor_tinted_top);
            TurnTintedPolyOn(gCursor_tinted_left);
            TurnTintedPolyOn(gCursor_tinted_bottom);
            TurnTintedPolyOn(gCursor_tinted_right);
            TurnTintedPolyOn(gCursor_tinted_center);
            ProcessTintedPoly(gCursor_tinted_top);
            ProcessTintedPoly(gCursor_tinted_left);
            ProcessTintedPoly(gCursor_tinted_bottom);
            ProcessTintedPoly(gCursor_tinted_right);
            ProcessTintedPoly(gCursor_tinted_center);
            RenderTintedPolys();
            TurnTintedPolyOff(gCursor_tinted_top);
            TurnTintedPolyOff(gCursor_tinted_left);
            TurnTintedPolyOff(gCursor_tinted_bottom);
            TurnTintedPolyOff(gCursor_tinted_right);
            TurnTintedPolyOff(gCursor_tinted_center);
        }
    }
    gCursor_last_call_time = time_now;
    gMouse_last_pos.y = pos_y;
    gMouse_last_pos.x = pos_x;
    gCursor_button_was_down = button_down;
    return mouse_moved;
}

// FUNCTION: CARMA2_HW 0x0043e3f0
void C2_HOOK_FASTCALL AllocateCursorActors(void) {
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
        /* Preserve the original encoded angle (0x3ffc), just below a quarter turn. */
        BrMatrix23Rotate(&gTintedPolys[gCursor_tinted_top].material->map_transform, 0x3ffc);
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_top].material, BR_MATU_ALL);
        gTintedPolys[gCursor_tinted_bottom].material->colour_map = line;
        BrMatrix23Rotate(&gTintedPolys[gCursor_tinted_bottom].material->map_transform, 0x3ffc);
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_bottom].material, BR_MATU_ALL);
    }
    if (cross != NULL) {
        gTintedPolys[gCursor_tinted_center].material->colour_map = cross;
        BrMaterialUpdate(gTintedPolys[gCursor_tinted_center].material, BR_MATU_ALL);
    }
}

// InitMouseTargetLines

// FUNCTION: CARMA2_HW 0x0043e6c0
void C2_HOOK_FASTCALL StartMouseCursor(void) {
    if (gCursor_tinted_top == -1) {
        AllocateCursorActors();
    }
    gNext_transient = 0;
    gTransient_bitmap_index = 0;
    GetMousePosition(&gMouse_last_pos.x, &gMouse_last_pos.y);
    gMouse_in_use = 0;
    gCurrent_cursor_index = 2;
    gMouse_started = 1;
}

// FUNCTION: CARMA2_HW 0x0043e710
void C2_HOOK_FASTCALL EndMouseCursor(void) {
    int i;

    RemoveTransientBitmaps(1);
    for (i = 0; i < (int)REC2_ASIZE(gTransient_bitmaps); i++) {
        if (gTransient_bitmaps[i].pixmap != NULL) {
            BrPixelmapFree(gTransient_bitmaps[i].pixmap);
            gTransient_bitmaps[i].pixmap = NULL;
            gTransient_bitmaps[i].in_use = 0;
        }
    }
    gMouse_started = 0;
}

// FUNCTION: CARMA2_HW 0x0047b450
void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDest, int pX1, int pY1, int pX2, int pY2, int pColour) {

    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
    BrPixelmapLine(pDest, pX1, pY1, pX2, pY2, pColour);
}

// FUNCTION: CARMA2_HW 0x0047b880
void C2_HOOK_FASTCALL ClearEntireScreen(void) {

    if (gScreen != NULL) {
        BrPixelmapFill(gScreen, gGraf_specs[gGraf_spec_index].black_value);
    }
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
    BrPixelmapFill(gDepth_buffer, 0xffffffff);
    PDScreenBufferSwap(0);
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
}

// FUNCTION: CARMA2_HW 0x0047b910
double C2_HOOK_FASTCALL DistanceFromPlane(const br_vector3* pPos, br_scalar pA, br_scalar pB, br_scalar pC, br_scalar pD) {

    /* The original caller uses unspilled PC53 ST0 for its first component. */
    return fabs(((double)pPos->v[0] * pA + (double)pPos->v[1] * pB + (double)pPos->v[2] * pC + pD)
        / ((double)pA * pA + (double)pB * pB + (double)pC * pC));
}

// FUNCTION: CARMA2_HW 0x0047b960
void C2_HOOK_FASTCALL KillSplashScreen(void) {

    if (gCurrent_splash != NULL) {
        BrMapRemove(gCurrent_splash);
        BrPixelmapFree(gCurrent_splash);
        gCurrent_splash = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x0047b990
void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name) {
    br_pixelmap* map;
    tPixelFlags flags;

    map = BrMapFind(pPixmap_name);
    if (gCurrent_splash != NULL) {
        if (map == gCurrent_splash) {
            return;
        }
        if (gCurrent_splash != NULL) {
            BrMapRemove(gCurrent_splash);
            BrPixelmapFree(gCurrent_splash);
        }
    }
    flags = gPixelFlags;
    gCurrent_splash = map;
    if (map == NULL) {
        gCurrent_splash = LoadPixelmap(pPixmap_name);
        BRPM_convert(gCurrent_splash, gBack_screen->type);
        if (gCurrent_splash != NULL) {
            BrMapAdd(gCurrent_splash);
        }
    }
    gPixelFlags = flags;
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
void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight) {
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
}

// DRMaskedStamp

// SwitchToHiresMode

// SwitchToLoresMode

// FUNCTION: CARMA2_HW 0x0047c740
void C2_HOOK_FASTCALL PoshDrawLine(br_pixelmap* pDest, int pX1, br_scalar pAngle, int pY1, int pX2, int pY2, int pColour) {

    if (pColour < 0) {
        /* NEG and coordinate offsets wrap as original 32-bit integer operations. */
        if (pAngle >= 0.785 && pAngle <= 5.498 && (pAngle <= 2.356 || pAngle >= 3.926)) {
            if ((pAngle <= 0.785 || pAngle >= 1.57) && (pAngle <= 3.926 || pAngle >= 4.712)) {
                DRDrawLine(pDest, (int)((tU32)pX1 - 1u), pY1, (int)((tU32)pX2 - 1u), pY2, (int)(0u - (tU32)pColour - 1u));
                DRDrawLine(pDest, (int)((tU32)pX1 + 1u), pY1, (int)((tU32)pX2 + 1u), pY2, (int)(1u - (tU32)pColour));
            } else {
                DRDrawLine(pDest, (int)((tU32)pX1 - 1u), pY1, (int)((tU32)pX2 - 1u), pY2, (int)(1u - (tU32)pColour));
                DRDrawLine(pDest, (int)((tU32)pX1 + 1u), pY1, (int)((tU32)pX2 + 1u), pY2, (int)(0u - (tU32)pColour - 1u));
            }
        } else {
            DRDrawLine(pDest, pX1, (int)((tU32)pY1 + 1u), pX2, (int)((tU32)pY2 + 1u), (int)(0u - (tU32)pColour - 1u));
            DRDrawLine(pDest, pX1, (int)((tU32)pY1 - 1u), pX2, (int)((tU32)pY2 - 1u), (int)(1u - (tU32)pColour));
        }
        DRDrawLine(pDest, pX1, pY1, pX2, pY2, (int)(0u - (tU32)pColour));
    } else {
        DRDrawLine(pDest, pX1, pY1, pX2, pY2, pColour);
    }
}

// FUNCTION: CARMA2_HW 0x0047cad0
void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {

    (void)pPixelmap;
    (void)pKnock_out_corners;
    gDim_model->vertices[1].p.v[0] = (br_scalar)pLeft;
    gDim_model->vertices[0].p.v[0] = gDim_model->vertices[1].p.v[0];
    gDim_model->vertices[3].p.v[0] = (br_scalar)pRight;
    gDim_model->vertices[2].p.v[0] = gDim_model->vertices[3].p.v[0];
    gDim_model->vertices[3].p.v[1] = -(br_scalar)pTop;
    gDim_model->vertices[0].p.v[1] = gDim_model->vertices[3].p.v[1];
    gDim_model->vertices[2].p.v[1] = -(br_scalar)pBottom;
    gDim_model->vertices[1].p.v[1] = gDim_model->vertices[2].p.v[1];
    BrModelUpdate(gDim_model, BR_MODU_VERTEX_POSITIONS);
    gDim_actor->render_style = BR_RSTYLE_FACES;
    if (gDim_actor->prev == NULL) {
        BrActorAdd(g2d_camera, gDim_actor);
    }
    BrZbsSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
    if (gDim_actor->parent != NULL) {
        BrActorRemove(gDim_actor);
    }
    gDim_actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x0047cbd0
void C2_HOOK_CDECL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {
    tU8* ptr;
    tU8* depth_table_ptr;
    tU8* right_ptr;
    int y;
    int line_skip;
    int width;

    if (gDevious_2d) {
        DeviouslyDimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
        return;
    }
    ptr = (tU8*)pPixelmap->pixels + pLeft + pPixelmap->row_bytes * pTop;
    line_skip = pPixelmap->row_bytes - pRight + pLeft;
    depth_table_ptr = (tU8*)gDepth_shade_table->pixels + gDepth_shade_table->row_bytes * gDim_amount;
    width = pRight - pLeft;
    if (pKnock_out_corners) {
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr];
        }
        ptr += line_skip + 1;
        for (y = pTop + 1; y < pBottom - 1; y++, ptr += line_skip) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr];
            }
        }
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr];
        }
    } else {
        for (y = pTop; y < pBottom; y++) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr];
            }
            ptr += line_skip;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047cce0
void C2_HOOK_FASTCALL DimRectangleClipped(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {

    if (pLeft < 0) {
        pLeft = 0;
    }
    if (pTop < 0) {
        pTop = 0;
    }
    if (pRight > pPixelmap->width) {
        pRight = pPixelmap->width;
    }
    if (pBottom > pPixelmap->height) {
        pBottom = pPixelmap->height;
    }
    if (pLeft < pRight && pTop < pBottom) {
        DimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
    }
}

// DR8BitPixelmapRotatedAndFeatheredCopy

// DR16BitPixelmapRotatedAndFeatheredCopy

// DRPixelmapRotatedAndFeatheredCopy

// DRPixelmapBlendedLine

// DR8BitFancyDrawLine

// FancyDrawLine

// DRPixelmapCopyMapBlack8Bit

// DRPixelmapCopyMapBlack

// FUNCTION: CARMA2_HW 0x0047d6a0
void C2_HOOK_FASTCALL DisableLights(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        BrLightDisable(gLight_array[i]);
    }
}

// FUNCTION: CARMA2_HW 0x0047d6d0
void C2_HOOK_FASTCALL EnableLights(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        BrLightEnable(gLight_array[i]);
    }
}
