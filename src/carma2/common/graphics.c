#include "graphics.h"

#include "errors.h"
#include "globvars.h"
#include "init.h"
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

C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gMap_render_x, 0x00659b2c, 80.f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gMap_render_y, 0x00659b30, 6.f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gMap_render_width, 0x00659b34, 128.f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gMap_render_height, 0x00659b38, 80.f);

C2_HOOK_VARIABLE_IMPLEMENT(int, gHeadup_map_x, 0x0074abac);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHeadup_map_y, 0x0074abb0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHeadup_map_w, 0x0074aba8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHeadup_map_h, 0x0074abb4);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHeadup_detail_level, 0x00655e54, 5);

C2_HOOK_VARIABLE_IMPLEMENT(int, gMap_trans, 0x0068d8d4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCamera_type, 0x00679308);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gAR_camera_type, 0x0058f620, 2);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gMini_map_visible, 0x00655dfc, 2);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gColours, 9, 0x0074a620);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gRGB_colours, 9, 0x0065cf30, {
    0x000000,   0xffffff,   0xff0000,   0x00ff00,
    0x0000ff,   0xffff00,   0x00ffff,   0xff00ff,
    0xd04702,
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tShadow_level, gShadow_level, 0x0065fdc8, kMiscString_ShadowUsOnly);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDR_font, gFonts, 24, 0x007663e0);

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gRear_pixelmap, 0x006a22bc);

C2_HOOK_VARIABLE_IMPLEMENT(int, gWidth, 0x00703e24);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHeight, 0x00703e20);
C2_HOOK_VARIABLE_IMPLEMENT(int, gX_offset, 0x00705060);
C2_HOOK_VARIABLE_IMPLEMENT(int, gY_offset, 0x006baa2c);

C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_palette_change, 0x006923a4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPalette_index, 0x006923b4);

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
#if 0 // defined(C2_HOOKS_ENABLED)
    BuildColourTable_original(pPalette);
#else
    int i;
    int j;
    int nearest_index = 0;
    int red;
    int green;
    int blue;
    float nearest_distance;
    float distance;

    for (i = 0; i < REC2_ASIZE(C2V(gRGB_colours)); i++) {
        nearest_distance = 196608.f;
        red = (C2V(gRGB_colours)[i] >> 16) & 0xFF;
        green = (C2V(gRGB_colours)[i] >> 8) & 0xFF;
        blue = C2V(gRGB_colours)[i] & 0xFF;
        for (j = 0; j < 256; j++) {
            distance = (float)(sqr((double)(signed int)(*((br_uint_8*)pPalette->pixels + 4 * j + 2) - red))
                + sqr((double)(signed int)(*((br_uint_8*)pPalette->pixels + 4 * j) - blue))
                + sqr((double)(signed int)(*((br_uint_8*)pPalette->pixels + 4 * j + 1) - green)));
            if (distance < nearest_distance) {
                nearest_index = j;
                nearest_distance = distance;
            }
        }
        C2V(gColours)[i] = nearest_index;
    }
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
    ((br_int_32*)pThe_palette->pixels)[0] = 0;
    if (pSet_current_palette) {
        c2_memcpy(C2V(gCurrent_palette_pixels), pThe_palette->pixels, 256 * sizeof(br_colour));
    }
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
    tPixelFlags pixelFlags;

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
    pixelFlags = C2V(gPixelFlags);
    C2V(gCurrent_splash) = the_map;
    if (the_map == NULL) {
        C2V(gCurrent_splash) = DRLoadPixelmap(pPixmap_name);
        DRPixelmapConvertRGB565ToRGB555IfNeeded(C2V(gCurrent_splash), C2V(gBack_screen)->type);
        if (C2V(gCurrent_splash) != NULL) {
            BrMapAdd(C2V(gCurrent_splash));
        }
    }
    C2V(gPixelFlags) = pixelFlags;
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

void (C2_HOOK_FASTCALL * InitTransientBitmaps_original)(void);
void C2_HOOK_FASTCALL InitTransientBitmaps(void) {
#if defined(C2_HOOKS_ENABLED)
    InitTransientBitmaps_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043dff0, InitTransientBitmaps, InitTransientBitmaps_original)

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

void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel) {

    C2V(gShadow_level) = pLevel;
}
C2_HOOK_FUNCTION(0x004e9940, SetShadowLevel)

tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void) {

    return C2V(gShadow_level);
}
C2_HOOK_FUNCTION(0x004e9950, GetShadowLevel)

void (C2_HOOK_FASTCALL * ToggleShadow_original)(void);
void C2_HOOK_FASTCALL ToggleShadow(void) {

#if defined(C2_HOOKS_ENABLED)
    ToggleShadow_original();
#else
    gShadow_level++;
    if (C2V(gShadow_level) == eShadow_everyone) {
        C2V(gShadow_level) = eShadow_none;
    }
    switch (C2V(gShadow_level)) {
        case eShadow_none:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowNone));
            break;
        case eShadow_us_only:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowUsOnly));
            break;
        case eShadow_us_and_opponents:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowUsAndOpponents));
            break;
        case eShadow_everyone:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowEveryone));
            break;
        default:
            return;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e9960, ToggleShadow, ToggleShadow_original)

void (C2_HOOK_FASTCALL * EnsurePaletteUp_original)(void);
void C2_HOOK_FASTCALL EnsurePaletteUp(void) {

#if defined(C2_HOOKS_ENABLED)
    EnsurePaletteUp_original();
#else
    if (C2V(gFaded_palette)) {
        FadePaletteUp();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b55f0, EnsurePaletteUp, EnsurePaletteUp_original)

int (C2_HOOK_FASTCALL * DoMouseCursor_original)(void);
int C2_HOOK_FASTCALL DoMouseCursor(void) {

#if defined(C2_HOOKS_ENABLED)
    return DoMouseCursor_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043e0a0, DoMouseCursor, DoMouseCursor_original)

void (C2_HOOK_FASTCALL * LoadFont_original)(int pFont_ID);
void C2_HOOK_FASTCALL LoadFont(int pFont_ID) {

#if defined(C2_HOOKS_ENABLED)
    LoadFont_original(pFont_ID);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00465850, LoadFont, LoadFont_original)

void (C2_HOOK_FASTCALL * InitDRFonts_original)(void);
void C2_HOOK_FASTCALL InitDRFonts(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitDRFonts_original();
#else
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gFonts)); i++) {
        C2V(gFonts)[i].file_read_once = 0;
        C2V(gFonts)[i].images = NULL;
        C2V(gFonts)[i].id = 11;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00465a40, InitDRFonts, InitDRFonts_original)

void (C2_HOOK_FASTCALL * UpdateMap_original)(void);
void C2_HOOK_FASTCALL UpdateMap(void) {

#if defined(C2_HOOKS_ENABLED)
    UpdateMap_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00496e30, UpdateMap, UpdateMap_original)

void (C2_HOOK_FASTCALL * InitHUDActor_original)(void);
void C2_HOOK_FASTCALL InitHUDActor(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitHUDActor_original();
#else
    br_camera* camera_data;

    C2V(gHUD_root) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    C2V(gHUD_camera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    if (C2V(gHUD_root) == NULL || C2V(gHUD_camera) == NULL) {
        FatalError(kFatalError_OOM_S);
    }
    camera_data = C2V(gHUD_camera)->type_data;
    camera_data->type = BR_CAMERA_PARALLEL;
    camera_data->hither_z = 1.f;
    camera_data->yon_z = 3.f;
    camera_data->width = C2V(gBack_screen)->width;
    camera_data->height = C2V(gBack_screen)->height;
    BrActorAdd(C2V(gHUD_root), C2V(gHUD_camera));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0047e560, InitHUDActor, InitHUDActor_original)

void (C2_HOOK_FASTCALL * InitShadows_original)(void);
void C2_HOOK_FASTCALL InitShadows(void) {

#if defined(C2_HOOKS_ENABLED)
    InitShadows_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e99d0, InitShadows, InitShadows_original)

void (C2_HOOK_FASTCALL * InitPaletteAnimate_original)(void);
void C2_HOOK_FASTCALL InitPaletteAnimate(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitPaletteAnimate_original();
#else
    C2V(gLast_palette_change) = 0;
    C2V(gPalette_index) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b52a0, InitPaletteAnimate, InitPaletteAnimate_original)

void (C2_HOOK_FASTCALL * SetBRenderScreenAndBuffers_original)(int pX_offset, int pY_offset, int pWidth, int pHeight);
void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight) {

#if 0//defined(C2_HOOKS_ENABLED)
    SetBRenderScreenAndBuffers_original(pX_offset, pY_offset, pWidth, pHeight);
#else

    PDAllocateScreenAndBack();
    if (pWidth == 0) {
        pWidth = C2V(gBack_screen)->width;
    }
    if (pHeight == 0) {
        pHeight = C2V(gBack_screen)->height;
    }
    C2V(gRender_screen) = DRPixelmapAllocateSub(C2V(gBack_screen), pX_offset, pY_offset, pWidth, pHeight);
    C2V(gWidth) = pWidth;
    C2V(gHeight) = pHeight;
    C2V(gY_offset) = pY_offset;
    C2V(gX_offset) = pX_offset;
    if (C2V(gGraf_specs)[C2V(gGraf_spec_index)].doubled) {
        C2V(gScreen)->base_x = (C2V(gGraf_specs)[C2V(gGraf_spec_index)].phys_width - 2 * C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width) / 2;
        C2V(gScreen)->base_y = (C2V(gGraf_specs)[C2V(gGraf_spec_index)].phys_height - 2 * C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height) / 2;
    } else {
        C2V(gScreen)->base_x = (C2V(gGraf_specs)[C2V(gGraf_spec_index)].phys_width - C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width) / 2;
        C2V(gScreen)->base_y = (C2V(gGraf_specs)[C2V(gGraf_spec_index)].phys_height - C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height) / 2;
    }

    C2V(gScreen)->origin_x = 0;
    C2V(gScreen)->origin_y = 0;
    if (C2V(gBack_screen) == NULL) {
        FatalError(kFatalError_AllocateOffScreenBuffer);
    }

    if (C2V(gDepth_buffer) != NULL) {
        BrPixelmapFree(C2V(gDepth_buffer));
        C2V(gDepth_buffer) = NULL;
    }
    C2V(gDepth_buffer) = BrPixelmapMatch(C2V(gBack_screen), BR_PMMATCH_DEPTH_16);
    if (C2V(gDepth_buffer) == NULL) {
        FatalError(kFatalError_AllocateZBuffer);
    }

    BrZbsBegin(C2V(gRender_screen)->type, C2V(gDepth_buffer)->type, C2V(gHeap), 300000); /* FIXME: use sizeof */
    C2V(gBrZb_initialized) = 1;
    C2V(gRear_pixelmap) = DRPixelmapAllocate(C2V(gScreen)->type, 64, 64, NULL, 0);
    BrMapAdd(C2V(gRear_pixelmap));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4980, SetBRenderScreenAndBuffers, SetBRenderScreenAndBuffers_original)

void C2_HOOK_FASTCALL LockBackScreen(int pValue) {

    if (C2V(gBack_screen)->pixels == NULL) {
        BrPixelmapDirectLock(C2V(gBack_screen), 1);
    }
}
C2_HOOK_FUNCTION(0x00516c10, LockBackScreen)

int C2_HOOK_FASTCALL UnlockBackScreen(int pValue) {

    if (C2V(gBack_screen)->pixels != NULL) {
        BrPixelmapDirectUnlock(C2V(gBack_screen));
        return 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00516c30, UnlockBackScreen)

void (C2_HOOK_FASTCALL * AdaptMaterialsForRenderer_original)(br_material** pMaterials, int pCount, tRendererShadingType pShading);
void C2_HOOK_FASTCALL AdaptMaterialsForRenderer(br_material** pMaterials, int pCount, tRendererShadingType pShading) {
#if defined(C2_HOOKS_ENABLED)
    AdaptMaterialsForRenderer_original(pMaterials, pCount, pShading);
#else
#error "Not implemented"
    int i;
    br_material* material;
    tMaterialException *material_exception;

    for (i = 0; i < pCount; i++) {
        material = pMaterials[i];

        if (material->colour_map != NULL) {
            for (material_exception = C2V(gMaterial_exceptions); material_exception != NULL; material_exception = material_exception->next) {
                if (c2_stricmp(material->colour_map->identifier, material_exception->texture_name) == 0) {
                    break;
                }
            }
            if (C2V(gEnable_texture_interpolation)) {
                if (material_exception == NULL || !(material_exception->flags & 0x1)) {
                    material->br_pixelmap_allocate_flags  |= BR_MATF_MAP_INTERPOLATION;
                }
                if (C2v(gEnable_texture_interpolation) && material_exception != NULL && material_exception->flags & 0x8) {
                    material->map_transform.m[2][0] = .02f;
                    material->map_transform.m[2][1] = .02f;
                }
            }
            if (C2V(gEnable_texture_antialiasing)) {
                material->flags |= BR_MATF_MAP_ANTIALIASING;
            }
            if (C2V(gEnable_perspective_maps)) {
                material->flags |= BR_MATF_PERSPECTIVE;
            }
        }
        switch (pShading) {
        case kRendererShadingType_Default:
            material->ka = .2f;
            break;
        case kRendererShadingType_Specular:
            material->ka = .6f;
            material->kd = .2f;
            material->ks = .8f;
            material->flags &= ~BR_MATF_PRELIT;
            material->flags |= BR_MATF_LIGHT;
            material->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Diffuse1:
            material->ka = .4f;
            break;
        case kRendererShadingType_AmbientOnly:
            material->ka = 1.f;
            material->kd = 0.f;
            material->ks = 0.f;
            material->flags &= ~BR_MATF_PRELIT;
            material->flags |= BR_MATF_LIGHT;
            material->flags |= BR_MATF_SMOOTH;
            break;
        default:
            material->ka = 1.f;
            material->kd = 0.f;
            material->ks = 0.f;
            break;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005182f0, AdaptMaterialsForRenderer, AdaptMaterialsForRenderer_original)

void C2_HOOK_FASTCALL DisableLights(void) {
    int i;

    for (i = 0; i < C2V(gNumber_of_lights); i++) {
        BrLightDisable(C2V(gLight_array)[i]);
    }
}
C2_HOOK_FUNCTION(0x0047d6a0, DisableLights)

void C2_HOOK_FASTCALL EnableLights(void) {
    int i;

    for (i = 0; i < C2V(gNumber_of_lights); i++) {
        BrLightEnable(C2V(gLight_array)[i]);
    }
}
C2_HOOK_FUNCTION(0x0047d6d0, EnableLights)
