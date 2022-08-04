#include "graphics.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "platform.h"

#include "brender/brender.h"
#include "brender/br_types.h"

#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

#include <math.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tWobble_spec, gWobble_array, 5, 0x006a22f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gCosine_array, 64, 0x00705080);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gEvalu, 0x006a22b8);
C2_HOOK_VARIABLE_IMPLEMENT(int, g16bitPaltte_munged, 0x0074cf04);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPalette_munged, 0x006923c0);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gCurrent_palette_pixels, 0x0074a680);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFaded_palette, 0x006923c8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gRender_palette, 0x0074a674);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gCurrent_palette, 0x0074a678);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gCurrent_splash, 0x0068be20);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTransient_bm, gTransient_bitmaps, 50, 0x0067be98);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMouse_started, 0x0067c390);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNoTransients, 0x0074ca28);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNext_transient, 0x0067c348);

C2_HOOK_VARIABLE_DECLARE_ARRAY(tTransient_bm, gTransient_bitmaps, 50);
C2_HOOK_VARIABLE_DECLARE(int, gMouse_started);
C2_HOOK_VARIABLE_DECLARE(int, gNoTransients);
C2_HOOK_VARIABLE_DECLARE(int, gNext_transient);

void C2_HOOK_FASTCALL ClearWobbles(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gWobble_array)); ++i) {
        C2V(gWobble_array)[i].time_started = 0;
    }
}
C2_HOOK_FUNCTION(0x004e4d30, ClearWobbles)

void C2_HOOK_FASTCALL InitWobbleStuff(void) {
    int i;

    ClearWobbles();
    for (i = 0; i < REC2_ASIZE(C2V(gCosine_array)); i++) {
        C2V(gCosine_array)[i] = cosf(i / 64.0f * 3.141592653589793f / 2.0f);
    }

    C2V(gEvalu) = DRLoadPixelmap("Evalu01.PIX");
}
C2_HOOK_FUNCTION(0x004e4d50, InitWobbleStuff)

void C2_HOOK_FASTCALL FadePaletteDown(void) {
}
C2_HOOK_FUNCTION(0x004b5460, FadePaletteDown)

void C2_HOOK_FASTCALL ClearEntireScreen(void) {
    if (C2V(gScreen) != NULL) {
        BrPixelmapFill(C2V(gScreen), C2V(gGraf_specs)[C2V(gGraf_spec_index)].black_value);
    }
    BrPixelmapFill(C2V(gBack_screen), C2V(gGraf_specs)[C2V(gGraf_spec_index)].black_value);
    BrPixelmapFill(C2V(gDepth_buffer), 0xffffffff);
    PDScreenBufferSwap(0);
    BrPixelmapFill(C2V(gBack_screen), C2V(gGraf_specs)[C2V(gGraf_spec_index)].black_value);
}
C2_HOOK_FUNCTION(0x0047b880, ClearEntireScreen)

void (C2_HOOK_FASTCALL * BuildColourTable_original)(br_pixelmap* pPalette);
void C2_HOOK_FASTCALL BuildColourTable(br_pixelmap* pPalette) {
#if defined(C2_HOOKS_ENABLED)
    BuildColourTable_original(pPalette);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4ed0, BuildColourTable, BuildColourTable_original)

void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount) {

    if (!pFirst_colour) {
        ((br_int_32*)pPalette->pixels)[0] = 0;
    }
    c2_memcpy(C2V(gCurrent_palette_pixels) + 4 * pFirst_colour, (char*)pPalette->pixels + 4 * pFirst_colour, 4 * pCount);
    C2V(g16bitPaltte_munged) = 0;
    if (!C2V(gFaded_palette)) {
        PDSetPaletteEntries(pPalette, pFirst_colour, pCount);
    }
    C2V(gPalette_munged) = 1;
}
C2_HOOK_FUNCTION(0x004b4fd0, DRSetPaletteEntries)

void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette) {
    if (!pSet_current_palette) {
        ((br_int_32*)pThe_palette->pixels)[0] = 0;
    }
    c2_memcpy(C2V(gCurrent_palette_pixels), pThe_palette->pixels, 0x400u);
    C2V(g16bitPaltte_munged) = 0;
    if (!C2V(gFaded_palette)) {
        PDSetPalette(pThe_palette);
    }
    if (pThe_palette != C2V(gRender_palette)) {
        C2V(gPalette_munged) |= 0x1;
    }
}

void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette) {
    DRSetPalette2(pThe_palette, 1);
}
C2_HOOK_FUNCTION(0x004b5030, DRSetPalette)

void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name) {
    br_pixelmap* the_map;
    int tiffFlags;

    the_map = BrMapFind(pPixmap_name);
    if (C2V(gCurrent_splash) != NULL) {
        if (the_map == C2V(gCurrent_splash)) {
            return;
        }
        if (C2V(gCurrent_splash) != NULL) {
            BrMapRemove(C2V(gCurrent_splash));
            BrPixelmapFree(C2V(gCurrent_splash));
        }
    }
    tiffFlags = C2V(gTiffFlags);
    C2V(gCurrent_splash) = the_map;
    if (the_map == NULL) {
        C2V(gCurrent_splash) = DRLoadPixelmap(pPixmap_name);
        DRPixelmapConvertRGB565ToRGB555IfNeeded(C2V(gCurrent_splash), C2V(gBack_screen)->type);
        if (C2V(gCurrent_splash) != NULL) {
            BrMapAdd(C2V(gCurrent_splash));
        }
    }
    C2V(gTiffFlags) = tiffFlags;
    if (C2V(gCurrent_splash) != NULL) {
        // FUN005191f();
        DRPixelmapRectangleCopy(C2V(gBack_screen),
                0, 0, C2V(gCurrent_splash),
                0, 0,
                C2V(gCurrent_splash)->width, C2V(gCurrent_splash)->height);
        PDScreenBufferSwap(0);
        BrMapRemove(C2V(gCurrent_splash));
        BrPixelmapFree(C2V(gCurrent_splash));
        C2V(gCurrent_splash) = NULL;
    }
}
C2_HOOK_FUNCTION(0x0047b990, SplashScreenWith)

void (C2_HOOK_FASTCALL * DRPixelmapConvertRGB565ToRGB555IfNeeded_original)(br_pixelmap* pixelmap, int pixelType);
void C2_HOOK_FASTCALL DRPixelmapConvertRGB565ToRGB555IfNeeded(br_pixelmap* pixelmap, int pixelType) {
    br_uint_8* pixel_row_start;
    br_uint_16* pixel;
    int i;
    int j;

    if (pixelmap == NULL || pixelmap->type != BR_PMT_RGB_565 || pixelType != BR_PMT_RGB_555 || pixelmap->pixels == NULL) {
        return;
    }
    pixel_row_start = pixelmap->pixels;

    for (i = 0; i < pixelmap->height; i++) {
        pixel = (br_uint_16*)pixel_row_start;
        for (j = 0; j < pixelmap->width; j++) {
            // Remove least significant bit of blue (6 bits -> 5 bits)
            *pixel = ((*pixel & 0xffc0)>>1) | (*pixel & 0x001f);
            pixel++;
        }
        pixel_row_start += pixelmap->row_bytes;
    }
    pixelmap->type = BR_PMT_RGB_555;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00518700, DRPixelmapConvertRGB565ToRGB555IfNeeded, DRPixelmapConvertRGB565ToRGB555IfNeeded_original)

br_model* (C2_HOOK_FASTCALL * CreateInterpolatedQuadModel_original)(int x, int y, int width, int height, int nbX, int nbY);
br_model* C2_HOOK_FASTCALL CreateInterpolatedQuadModel(int x, int y, int width, int height, int nbX, int nbY) {
#if defined(C2_HOOKS_ENABLED)
    return CreateInterpolatedQuadModel_original(x, y, width, height, nbX, nbY);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d7ad0, CreateInterpolatedQuadModel, CreateInterpolatedQuadModel_original)

void (C2_HOOK_FASTCALL * TintedPolyActor_SetColour_original)(int index, int red, int green, int blue);
void C2_HOOK_FASTCALL TintedPolyActor_SetColour(int index, int red, int green, int blue) {
#if defined(C2_HOOKS_ENABLED)
    TintedPolyActor_SetColour_original(index, red, green, blue);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d82b0, TintedPolyActor_SetColour, TintedPolyActor_SetColour_original)

int (C2_HOOK_FASTCALL * CreateTintedPolyActor_original)(int x, int y, int width, int height, int type, int arg1, int arg2, int arg3);
int C2_HOOK_FASTCALL CreateTintedPolyActor(int x, int y, int width, int height, int type, int arg1, int arg2, int arg3) {
#if defined(C2_HOOKS_ENABLED)
    return CreateTintedPolyActor_original(x, y, width, height, type, arg1, arg2, arg3);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d70c0, CreateTintedPolyActor, CreateTintedPolyActor_original)

void (C2_HOOK_FASTCALL * AllocateCursorActors_original)(void);
void C2_HOOK_FASTCALL AllocateCursorActors(void) {
#if defined(C2_HOOKS_ENABLED)
    AllocateCursorActors_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043e3f0, AllocateCursorActors, AllocateCursorActors_original)

void (C2_HOOK_FASTCALL * StartMouseCursor_original)(void);
void C2_HOOK_FASTCALL StartMouseCursor(void) {
#if defined(C2_HOOKS_ENABLED)
    StartMouseCursor_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043e6c0, StartMouseCursor, StartMouseCursor_original)

void C2_HOOK_FASTCALL EndMouseCursor(void) {

    RemoveTransientBitmaps(1);
    DeallocateAllTransientBitmaps();
    C2V(gMouse_started) = 0;
}
C2_HOOK_FUNCTION(0x0043e710, EndMouseCursor)

void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them) {
    int i;
    int order_number;

    if (C2V(gNoTransients) && pGraphically_remove_them) {
        for (order_number = C2V(gNext_transient) - 1; order_number >= 0; order_number--) {
            for (i = 0; i < REC2_ASIZE(C2V(gTransient_bitmaps)); i++) {
                if (C2V(gTransient_bitmaps)[i].pixmap != NULL && C2V(gTransient_bitmaps)[i].order_number == order_number) {
                    if (C2V(gTransient_bitmaps)[i].in_use) {
                        BrPixelmapRectangleCopy(C2V(gBack_screen),
                                                C2V(gTransient_bitmaps)[i].x_coord,
                                                C2V(gTransient_bitmaps)[i].y_coord,
                                                C2V(gTransient_bitmaps)[i].pixmap,
                                                0,
                                                0,
                                                C2V(gTransient_bitmaps)[i].pixmap->width,
                                                C2V(gTransient_bitmaps)[i].pixmap->height);
                    }
                    break;
                }
            }
        }
    }
    C2V(gNext_transient) = 0;
}
C2_HOOK_FUNCTION(0x0043e010, RemoveTransientBitmaps)

void C2_HOOK_FASTCALL DeallocateTransientBitmap(int pIndex) {

    if (C2V(gTransient_bitmaps)[pIndex].pixmap != NULL) {
        BrPixelmapFree(C2V(gTransient_bitmaps)[pIndex].pixmap);
        C2V(gTransient_bitmaps)[pIndex].pixmap = NULL;
        C2V(gTransient_bitmaps)[pIndex].in_use = 0;
    }
}

void C2_HOOK_FASTCALL DeallocateAllTransientBitmaps(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gTransient_bitmaps)); i++) {
        DeallocateTransientBitmap(i);
    }
}
