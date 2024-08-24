#include "graphics.h"

#include "displays.h"
#include "errors.h"
#include "globvars.h"
#include "grafdata.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "physics.h"
#include "tinted.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include "brender/brender.h"
#include "brender/br_types.h"

#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

#include <float.h>
#include <math.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tWobble_spec, gWobble_array, 5, 0x006a22f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gCosine_array, 64, 0x00705080);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gEvalu, 0x006a22b8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPalette_changed, 0x0074cf04);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPalette_munged, 0x006923c0);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gCurrent_palette_pixels, 0x0074a680);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFaded_palette, 0x006923c8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gRender_palette, 0x0074a674);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gCurrent_palette, 0x0074a678);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gCurrent_splash, 0x0068be20);

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gOrig_render_palette, 0x006923ac);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gFlic_palette, 0x0074a67c);
C2_HOOK_VARIABLE_IMPLEMENT(br_colour*, gScratch_pixels, 0x006923b8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gScratch_palette, 0x006923a8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPalette_0074a604, 0x0074a604);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPalette_0074a600, 0x0074a600);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gMini_map_glowing_line_palettes, 3, 0x0074a660);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPalette_0074a66c, 0x0074a66c);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPalette_0074a5fc, 0x0074a5fc);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPalette_0074a670, 0x0074a670);

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

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tShadow_level, gShadow_level, 0x0065fdc8, eShadow_us_only);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDR_font, gFonts, 24, 0x007663e0);

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gRear_pixelmap, 0x006a22bc);

C2_HOOK_VARIABLE_IMPLEMENT(int, gWidth, 0x00703e24);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHeight, 0x00703e20);
C2_HOOK_VARIABLE_IMPLEMENT(int, gX_offset, 0x00705060);
C2_HOOK_VARIABLE_IMPLEMENT(int, gY_offset, 0x006baa2c);

C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_palette_change, 0x006923a4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPalette_index, 0x006923b4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gFont_names, 24, 0x0059ad30, {
    "TYPEABLE",
    "ORANGHED",
    "BLUEHEAD",
    "GREENHED",
    "MEDIUMHD",
    "TIMER",
    "NEWHITE",
    "NEWRED",
    "NEWBIGGR",
    "GRNDK",
    "GRNLIT",
    "GRYDK",
    "GRYLIT",
    "BUTTIN",
    "BUTTOUT",
    "LITPLAQ",
    "DRKPLAQ",
    "RED1",
    "RED1GLOW",
    "GRN1",
    "GRN1GLOW",
    "RED2",
    "RED2GLOW",
    "OPPOSTAT",
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gMap_render_x_i, 0x0074abc0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMap_render_y_i, 0x0074abbc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMap_render_width_i, 0x0074abe8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMap_render_height_i, 0x0074abc8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tClip_details, gShadow_clip_planes, 8, 0x006a2448);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFancy_shadow, 0x006a23d8);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gShadow_material, 0x006a27e0);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gShadow_light_ray, 0x006a27d0);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gShadow_light_z, 0x006a27c0);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gShadow_light_x, 0x006a27b0);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gShadow_model, 0x006a27e4);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gShadow_actor, 0x006a2444);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_polyfont_glyph_actors, 0x00686490);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gPolyfont_glyph_actors, 256, 0x0074cae0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSaved_table, gSaved_shade_tables, 100, 0x006a2488);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSaved_table_count, 0x006a27a8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTarget_lock_enabled, 0x0068d8c8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gLit_op_stat, 0x0068d8e0);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCursor_tinted_top, 0x0058fdc8, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCursor_tinted_left, 0x0058fdd0, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCursor_tinted_bottom, 0x0058fdcc, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCursor_tinted_right, 0x0058fdd4, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCursor_tinted_center, 0x0058fdd8, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCursor_line_width, 0x0058fde0, 5);

C2_HOOK_VARIABLE_IMPLEMENT(int, gTransient_bitmap_index, 0x0067be94);
C2_HOOK_VARIABLE_IMPLEMENT(tMouse_coord, gMouse_last_pos, 0x0079ecb0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMouse_in_use, 0x0079ecb8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCurrent_cursor_index, 0x0067be90);

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

    C2V(gEvalu) = LoadPixelmap("Evalu01.PIX");
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
    C2V(gPalette_changed) = 0;
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
    C2V(gPalette_changed) = 0;
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
        C2V(gCurrent_splash) = LoadPixelmap(pPixmap_name);
        BRPM_convert(C2V(gCurrent_splash), C2V(gBack_screen)->type);
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

void (C2_HOOK_FASTCALL * BRPM_convert_original)(br_pixelmap* pixelmap, int pixelType);
void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pixelmap, int pixelType) {
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
C2_HOOK_FUNCTION_ORIGINAL(0x00518700, BRPM_convert, BRPM_convert_original)

void (C2_HOOK_FASTCALL * AllocateCursorActors_original)(void);
void C2_HOOK_FASTCALL AllocateCursorActors(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    AllocateCursorActors_original();
#else
    br_pixelmap *mse_cross;
    br_pixelmap *mse_line;

    mse_cross = BrMapFind("mse_corn");
    mse_cross = BrMapFind("mse_cros");
    mse_line = BrMapFind("mse_line");
    C2V(gCursor_tinted_top) = CreateTintedPoly(0, 0, C2V(gCursor_line_width), C2V(gCurrent_graf_data)->height, 1, 127, 192, 0);
    C2V(gCursor_tinted_left) = CreateTintedPoly(0, 0, C2V(gCurrent_graf_data)->width, C2V(gCursor_line_width), 1, 127, 192, 0);
    C2V(gCursor_tinted_bottom) = CreateTintedPoly(0, 0, C2V(gCursor_line_width), C2V(gCurrent_graf_data)->height, 1, 127, 192, 0);
    C2V(gCursor_tinted_right) = CreateTintedPoly(0, 0, C2V(gCurrent_graf_data)->width, C2V(gCursor_line_width), 1, 127, 192, 0);
    C2V(gCursor_tinted_center) = CreateTintedPoly(0, 0, 16, 16, 1, 127, 192, 0);
    if (mse_line != NULL) {
        C2V(gTintedPolys)[C2V(gCursor_tinted_left)].material->colour_map = mse_line;
        C2V(gTintedPolys)[C2V(gCursor_tinted_left)].material->ka = 1.0f;
        BrMaterialUpdate(C2V(gTintedPolys)[C2V(gCursor_tinted_left)].material, BR_MATU_ALL);
        C2V(gTintedPolys)[C2V(gCursor_tinted_right)].material->colour_map = mse_line;
        BrMaterialUpdate(C2V(gTintedPolys)[C2V(gCursor_tinted_right)].material, BR_MATU_ALL);
        C2V(gTintedPolys)[C2V(gCursor_tinted_top)].material->colour_map = mse_line;
        BrMatrix23Rotate(&(C2V(gTintedPolys)[C2V(gCursor_tinted_top)].material)->map_transform, BrDegreeToAngle(90));
        BrMaterialUpdate(C2V(gTintedPolys)[C2V(gCursor_tinted_top)].material, BR_MATU_ALL);
        (C2V(gTintedPolys)[C2V(gCursor_tinted_bottom)].material)->colour_map = mse_line;
        BrMatrix23Rotate(&(C2V(gTintedPolys)[C2V(gCursor_tinted_bottom)].material)->map_transform, BrDegreeToAngle(90));
        BrMaterialUpdate(C2V(gTintedPolys)[C2V(gCursor_tinted_bottom)].material, BR_MATU_ALL);
    }
    if (mse_cross != NULL) {
        C2V(gTintedPolys)[C2V(gCursor_tinted_center)].material->colour_map = mse_cross;
        BrMaterialUpdate(C2V(gTintedPolys)[C2V(gCursor_tinted_center)].material, BR_MATU_ALL);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043e3f0, AllocateCursorActors, AllocateCursorActors_original)

void (C2_HOOK_FASTCALL * StartMouseCursor_original)(void);
void C2_HOOK_FASTCALL StartMouseCursor(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    StartMouseCursor_original();
#else
    if (C2V(gCursor_tinted_top) == -1) {
        AllocateCursorActors();
    }
    C2V(gNext_transient) = 0;
    C2V(gTransient_bitmap_index) = 0;
    GetMousePosition(&C2V(gMouse_last_pos).x, &C2V(gMouse_last_pos).y);
    C2V(gMouse_in_use) = 0;
    C2V(gCurrent_cursor_index) = 2;
    C2V(gMouse_started) = 1;
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
#if 0//defined(C2_HOOKS_ENABLED)
    InitTransientBitmaps_original();
#else
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gTransient_bitmaps)) != 50);
    C2_HOOK_BUG_ON(sizeof(tTransient_bm) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTransient_bm, in_use, 4);

    for (i = 0; i < REC2_ASIZE(C2V(gTransient_bitmaps)); i++) {
        C2V(gTransient_bitmaps)[i].in_use = 0;
    }
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

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleShadow_original();
#else
    C2V(gShadow_level) += 1;
    if (C2V(gShadow_level) > eShadow_everyone) {
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

void C2_HOOK_FASTCALL ShadowMode(void) {

    C2V(gFancy_shadow) = !C2V(gFancy_shadow);
    if (C2V(gFancy_shadow)) {
        NewTextHeadupSlot(4, 0, 2000, -4, "Translucent shadow");
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, "Solid shadow");
    }
}
C2_HOOK_FUNCTION(0x004e9b90, ShadowMode)

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

#if 0//defined(C2_HOOKS_ENABLED)
    return DoMouseCursor_original();
#else
    int delta_time;
    int mouse_moved;
    int button_down;
    int new_required;
    tU32 time_now;
    int pos_x, pos_y;

    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_call_time, 0x0067c398);
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_required_change, 0x0067c39c);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, delta_x, 0x0067c3a0);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, required_cursor, 0x0067c3a4);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, zero_count, 0x0067c3a8);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, button_was_down, 0x0067c3ac);
    static C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, draw_cursor, 0x0058fddc, 1);

    do {
        time_now = PDGetTotalTime();
        if (C2V(last_call_time) == 0) {
            delta_time = 1000;
        } else {
            delta_time = time_now - C2V(last_call_time);
        }
    } while (delta_time <= 20);
    GetMousePosition(&pos_x, &pos_y);
    mouse_moved = pos_x != C2V(gMouse_last_pos).x || pos_y != C2V(gMouse_last_pos).y;
    button_down = EitherMouseButtonDown();
    if (C2V(gMouse_in_use) || mouse_moved) {
        C2V(gMouse_in_use) = 1;
        if (pos_x == C2V(gMouse_last_pos).x) {
            if (C2V(zero_count) > 4) {
                C2V(delta_x) = 0;
            }
            C2V(zero_count) += 1;
        } else {
            C2V(zero_count) = 0;
            C2V(delta_x) = 1000 * (pos_x - C2V(gMouse_last_pos).x) / delta_time;
        }
        if (C2V(delta_x) < -10) {
            new_required = 0;
        } else if (C2V(delta_x) > 10) {
            new_required = 3;
        } else {
            new_required = 2;
        }
        if (new_required != C2V(required_cursor) && time_now - C2V(last_required_change) >= 200) {
            C2V(last_required_change) = time_now;
            C2V(required_cursor) = new_required;
        }
        C2V(gCurrent_cursor_index) = 2;
        if (!C2V(gNoTransients)) {
            br_pixelmap *map;
            int idx = C2V(gTransient_bitmap_index);

            C2V(gTransient_bitmaps)[idx].x_coord = (pos_x - 7) & ~3;
            C2V(gTransient_bitmaps)[idx].y_coord = pos_y - 7;
            C2V(gTransient_bitmaps)[idx].in_use = 1;
            C2V(gTransient_bitmaps)[idx].order_number = C2V(gNext_transient);
            C2V(gNext_transient) += 1;
            map = C2V(gTransient_bitmaps)[idx].pixmap;
            BrPixelmapRectangleCopy(map,
                0, 0,
                C2V(gBack_screen),
                C2V(gTransient_bitmaps)[idx].x_coord,
                C2V(gTransient_bitmaps)[idx].y_coord,
                map->width,
                map->height);
        }
        if (C2V(draw_cursor) && C2V(draw_cursor) == 1) {
            PossibleUnlock(1);
            ResetTintedVertices(C2V(gCursor_tinted_top),
                pos_x - 1, 0,
                C2V(gCursor_line_width), pos_y - 9);
            ResetTintedVertices(C2V(gCursor_tinted_left),
                0, pos_y - 1,
                pos_x - 9, C2V(gCursor_line_width));
            ResetTintedVertices(C2V(gCursor_tinted_bottom),
                pos_x - 1, pos_y + 5 + C2V(gCursor_line_width),
                C2V(gCursor_line_width), C2V(gCurrent_graf_data)->height - C2V(gCursor_line_width) - pos_y + 9);
            ResetTintedVertices(C2V(gCursor_tinted_right),
                pos_x + 5 + C2V(gCursor_line_width), pos_y - 1,
                C2V(gCurrent_graf_data)->width - C2V(gCursor_line_width) - pos_x + 9, C2V(gCursor_line_width));
            ResetTintedVertices(C2V(gCursor_tinted_center),
                pos_x - 7, pos_y - 7,
                16, 16);
            TurnTintedPolyOn(C2V(gCursor_tinted_top));
            TurnTintedPolyOn(C2V(gCursor_tinted_left));
            TurnTintedPolyOn(C2V(gCursor_tinted_bottom));
            TurnTintedPolyOn(C2V(gCursor_tinted_right));
            TurnTintedPolyOn(C2V(gCursor_tinted_center));
            ProcessTintedPoly(C2V(gCursor_tinted_top));
            ProcessTintedPoly(C2V(gCursor_tinted_left));
            ProcessTintedPoly(C2V(gCursor_tinted_bottom));
            ProcessTintedPoly(C2V(gCursor_tinted_right));
            ProcessTintedPoly(C2V(gCursor_tinted_center));
            RenderTintedPolys();
            TurnTintedPolyOff(C2V(gCursor_tinted_top));
            TurnTintedPolyOff(C2V(gCursor_tinted_left));
            TurnTintedPolyOff(C2V(gCursor_tinted_bottom));
            TurnTintedPolyOff(C2V(gCursor_tinted_right));
            TurnTintedPolyOff(C2V(gCursor_tinted_center));
        }
    }
    C2V(last_call_time) = time_now;
    C2V(gMouse_last_pos).y = pos_y;
    C2V(gMouse_last_pos).x = pos_x;
    C2V(button_was_down) = button_down;
    return mouse_moved;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043e0a0, DoMouseCursor, DoMouseCursor_original)

void (C2_HOOK_FASTCALL * LoadFont_original)(int pFont_ID);
void C2_HOOK_FASTCALL LoadFont(int pFont_ID) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadFont_original(pFont_ID);
#else
    int i;
    FILE* file;
    tPath_name the_path;

    if (C2V(gFonts)[pFont_ID].images != NULL) {
        return;
    }
    PathCat(the_path, C2V(gApplication_path), C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, C2V(gFont_names)[pFont_ID]);
    c2_strcat(the_path, ".PIX");
    if (C2V(gFonts)[pFont_ID].file_read_once) {
        return;
    }
    the_path[c2_strlen(the_path) - 4] = '\0';
    c2_strcat(the_path, ".TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        FatalError(kFatalError_CannotLoadFontWidthTable_S, C2V(gFont_names)[pFont_ID]);
    }
    C2V(gFonts)[pFont_ID].height = GetAnInt(file);
    C2V(gFonts)[pFont_ID].width = GetAnInt(file);
    C2V(gFonts)[pFont_ID].spacing = GetAnInt(file);
    C2V(gFonts)[pFont_ID].offset = GetAnInt(file);
    C2V(gFonts)[pFont_ID].num_entries = GetAnInt(file);
    C2V(gFonts)[pFont_ID].id = pFont_ID;
    if (C2V(gFonts)[pFont_ID].width <= 0) {
        for (i = 0; i < C2V(gFonts)[pFont_ID].num_entries; i++) {
            C2V(gFonts)[pFont_ID].width_table[i] = GetAnInt(file);
        }
    }
    PFfclose(file);
    C2V(gFonts)[pFont_ID].file_read_once = 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00465850, LoadFont, LoadFont_original)

void C2_HOOK_FASTCALL DisposeFont(int pFont_ID) {

    if (C2V(gFonts)[pFont_ID].images != NULL) {
        BrPixelmapFree(C2V(gFonts)[pFont_ID].images);
        C2V(gFonts)[pFont_ID].images = NULL;
        C2V(gFonts)[pFont_ID].file_read_once = 0;
    }
}
C2_HOOK_FUNCTION(0x00465a00, DisposeFont)

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

void (C2_HOOK_FASTCALL * Init2DStuffForPolyFonts_original)(void);
void C2_HOOK_FASTCALL Init2DStuffForPolyFonts(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    Init2DStuffForPolyFonts_original();
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
C2_HOOK_FUNCTION_ORIGINAL(0x0047e560, Init2DStuffForPolyFonts, Init2DStuffForPolyFonts_original)

void (C2_HOOK_FASTCALL * InitShadows_original)(void);
void C2_HOOK_FASTCALL InitShadow(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitShadows_original();
#else
    int i;

    C2_HOOK_BUG_ON(sizeof(C2V(gShadow_clip_planes)[0]) != 8);

    for (i = 0; i < REC2_ASIZE(C2V(gShadow_clip_planes)); i++) {
        C2V(gShadow_clip_planes)[i].clip = BrActorAllocate(BR_ACTOR_CLIP_PLANE, NULL);
        BrActorAdd(C2V(gUniverse_actor), C2V(gShadow_clip_planes)[i].clip);
        BrClipPlaneDisable(C2V(gShadow_clip_planes)[i].clip);
        BrMatrix34Identity(&C2V(gShadow_clip_planes)[i].clip->t.t.mat);
    }
    C2V(gFancy_shadow) = 1;
    C2V(gShadow_material) = BrMaterialFind("SHADOW.MAT");
    BrVector3Set(&C2V(gShadow_light_ray), 0.f, -1.f, 0.f);
    BrVector3Set(&C2V(gShadow_light_z), 0.f, 0.f, -1.f);
    BrVector3Set(&C2V(gShadow_light_x), 1.f, 0.f, 0.f);

    C2V(gShadow_model) = BrModelAllocate("", 0, 0);
    C2V(gShadow_model)->flags = BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL;
    C2V(gShadow_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    C2V(gShadow_actor)->model = C2V(gShadow_model);
    BrActorAdd(C2V(gUniverse_actor), C2V(gShadow_actor));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e99d0, InitShadow, InitShadows_original)

br_uint_32 C2_HOOK_CDECL SaveShadeTable(br_pixelmap* pTable, void* pArg) {
    br_pixelmap* copy;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gSaved_shade_tables)) != 100);
    if (C2V(gSaved_table_count) == REC2_ASIZE(C2V(gSaved_shade_tables))) {
        return 1;
    }
    C2V(gSaved_shade_tables)[C2V(gSaved_table_count)].original = pTable;
    C2_HOOK_BUG_ON(sizeof(br_pixelmap) != 0x44);
    copy = BrMemAllocate(sizeof(br_pixelmap), kMem_misc);
    C2V(gSaved_shade_tables)[C2V(gSaved_table_count)].copy = copy;
    C2V(gSaved_table_count) += 1;
    c2_memcpy(copy, pTable, sizeof(br_pixelmap));
    return 0;
}
C2_HOOK_FUNCTION(0x004e9b10, SaveShadeTable)

void C2_HOOK_FASTCALL SaveShadeTables(void) {
    PossibleService();
    C2V(gSaved_table_count) = 0;
    BrTableEnum("*", (br_table_enum_cbfn*)SaveShadeTable, NULL);
}
C2_HOOK_FUNCTION(0x004e9ae0, SaveShadeTables)

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

void C2_HOOK_FASTCALL MungePalette(void) {

}
C2_HOOK_FUNCTION(0x004b5320, MungePalette)

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

    BrZbsBegin(C2V(gRender_screen)->type, C2V(gDepth_buffer)->type, C2V(gPhysics_buffer), 300000); /* FIXME: use sizeof */
    C2V(gBrZb_initialized) = 1;
    C2V(gRear_pixelmap) = DRPixelmapAllocate(C2V(gScreen)->type, 64, 64, NULL, 0);
    BrMapAdd(C2V(gRear_pixelmap));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4980, SetBRenderScreenAndBuffers, SetBRenderScreenAndBuffers_original)

void C2_HOOK_FASTCALL PossibleLock(int pValue) {

    if (C2V(gBack_screen)->pixels == NULL) {
        BrPixelmapDirectLock(C2V(gBack_screen), 1);
    }
}
C2_HOOK_FUNCTION(0x00516c10, PossibleLock)

int C2_HOOK_FASTCALL PossibleUnlock(int pValue) {

    if (C2V(gBack_screen)->pixels != NULL) {
        BrPixelmapDirectUnlock(C2V(gBack_screen));
        return 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00516c30, PossibleUnlock)

tMaterial_exception* C2_HOOK_FASTCALL FindExceptionInList(const char* pIdentifier, tMaterial_exception* pList) {

    for (; pList != NULL; pList = pList->next) {
        if (DRStricmp(pIdentifier, pList->texture_name) == 0) {
            break;
        }
    }
    return pList;
}

void (C2_HOOK_FASTCALL * GlorifyMaterial_original)(br_material** pMaterials, int pCount, tRendererShadingType pShading);
void C2_HOOK_FASTCALL GlorifyMaterial(br_material** pMaterials, int pCount, tRendererShadingType pShading) {
#if 0//defined(C2_HOOKS_ENABLED)
    AdaptMaterialsForRenderer_original(pMaterials, pCount, pShading);
#else
    int i;
    br_material* material;
    tMaterial_exception *material_exception;

    for (i = 0; i < pCount; i++) {
        material = pMaterials[i];

        if (material->colour_map != NULL) {
            material_exception = FindExceptionInList(material->colour_map->identifier, C2V(gMaterial_exceptions));
            if (C2V(gEnable_texture_interpolation)) {
                if (material_exception == NULL || !(material_exception->flags & 0x1)) {
                    material->flags |= BR_MATF_MAP_INTERPOLATION;
                }
                if (C2V(gEnable_texture_interpolation) && material_exception != NULL && material_exception->flags & 0x8) {
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
            material->kd = .8f;
            material->ks = .0f;
            material->flags &= ~BR_MATF_PRELIT;
            material->flags |= BR_MATF_LIGHT;
            material->flags |= BR_MATF_SMOOTH;
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
C2_HOOK_FUNCTION_ORIGINAL(0x005182f0, GlorifyMaterial, GlorifyMaterial_original)

int C2_HOOK_FASTCALL FindBestMatchingPaletteIndex(br_colour pColour) {
    double min_error = DBL_MAX;
    int i;
    int pal_index;
    br_colour* pal_colours;

    pal_index = 127;
    pal_colours = C2V(gRender_palette)->pixels;
    for (i = 0; i < 256; i++) {
        int dr, dg, db;
        double error;

        dr = ((pal_colours[i] >> 16) & 0xff) - ((pColour >> 16) & 0xff);
        dg = ((pal_colours[i] >>  8) & 0xff) - ((pColour >>  8) & 0xff);
        db = ((pal_colours[i] >>  0) & 0xff) - ((pColour >>  0) & 0xff);
        error = (double)(dr * dr + dg * dg + db * db);
        if (error < min_error) {
            pal_index = i;
            min_error = error;
        }
    }
    return pal_index;
}
C2_HOOK_FUNCTION(0x005184c0, FindBestMatchingPaletteIndex)

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

int C2_HOOK_FASTCALL SwitchToHiresMode(void) {

    return 0;
}
C2_HOOK_FUNCTION(0x0047c650, SwitchToHiresMode)

int C2_HOOK_FASTCALL SwitchToLoresMode(void) {

    return 0;
}
C2_HOOK_FUNCTION(0x0047c660, SwitchToLoresMode)

void C2_HOOK_FASTCALL AdjustRenderScreenSize(void) {

    ReinitialiseRenderStuff();
    if (C2V(gMap_view) == 2) {
        C2V(gRender_screen)->base_x = C2V(gMap_render_x_i);
        C2V(gRender_screen)->base_y = C2V(gMap_render_y_i);
        C2V(gRender_screen)->width = C2V(gMap_render_width_i);
        C2V(gRender_screen)->height = C2V(gMap_render_height_i);
    } else {
        C2V(gRender_screen)->base_x = C2V(gProgram_state).current_render_left;
        C2V(gRender_screen)->base_y = C2V(gProgram_state).current_render_top;
        C2V(gRender_screen)->height = C2V(gProgram_state).current_render_bottom - C2V(gProgram_state).current_render_top;
        C2V(gRender_screen)->width = C2V(gProgram_state).current_render_right - C2V(gProgram_state).current_render_left;
    }
    if (C2V(gRender_screen)->row_bytes == C2V(gRender_screen)->width) {
        C2V(gRender_screen)->flags |= BR_PMF_ROW_WHOLEPIXELS;
    } else {
        C2V(gRender_screen)->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
    }
    C2V(gRender_screen)->origin_x = C2V(gRender_screen)->width / 2;
    C2V(gRender_screen)->origin_y = C2V(gRender_screen)->height / 2;
    C2V(gWidth) = C2V(gRender_screen)->width;
    C2V(gHeight) = C2V(gRender_screen)->height;
    ReinitialiseForwardCamera();
    ResetTintedVertices(C2V(gHud_tinted1),
        C2V(gRender_screen)->base_x,
        C2V(gRender_screen)->base_y,
        C2V(gRender_screen)->width,
        C2V(gRender_screen)->height);
    ResetTintedVertices(C2V(gHud_tinted2),
        C2V(gRender_screen)->base_x,
        C2V(gRender_screen)->base_y,
        C2V(gRender_screen)->width,
        C2V(gRender_screen)->height);
    ResetTintedVertices(C2V(gHud_tinted4),
        C2V(gRender_screen)->base_x,
        C2V(gRender_screen)->base_y,
        C2V(gRender_screen)->width,
        C2V(gRender_screen)->height);
}
C2_HOOK_FUNCTION(0x004e4b40, AdjustRenderScreenSize)

void C2_HOOK_FASTCALL RemovePolyFontActors(void) {
    int i;

    for (i = 0; i < C2V(gCount_polyfont_glyph_actors); i++) {
        br_actor* actor = C2V(gPolyfont_glyph_actors)[i];
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    C2V(gCount_polyfont_glyph_actors) = 0;
}
C2_HOOK_FUNCTION(0x004e5c70, RemovePolyFontActors)

void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight) {
    int y_count;
    int x_count;
    int dest_row_wrap;
    int source_row_wrap;
    tU16 the_byte;
    tU16* source_ptr;
    tU16* dest_ptr;

    source_ptr = (tU16*)((tU8*)pSource->pixels + pSource->row_bytes * pSource_y) + pSource_x;
    dest_ptr = (tU16*)((tU8*)pDest->pixels + pDest->row_bytes * pDest_y + pDest->base_x) + pDest_x;
    source_row_wrap = pSource->row_bytes / sizeof(tU16) - pWidth;
    dest_row_wrap = pDest->row_bytes / sizeof(tU16) - pWidth;

    if (pDest_y < 0) {
        pHeight += pDest_y;
        if (pHeight <= 0) {
            return;
        }
        source_ptr = (tU16*)((tU8*)source_ptr - pDest_y * pSource->row_bytes);
        dest_ptr = (tU16*)((tU8*)dest_ptr - pDest_y * pDest->row_bytes);
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
        source_row_wrap += pDest_x + pWidth - pDest->width;
        dest_row_wrap += pDest_x + pWidth - pDest->width;
        pWidth = pDest->width - pDest_x;
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
}
C2_HOOK_FUNCTION(0x0047ba80, DRPixelmapRectangleMaskedCopy)

void C2_HOOK_FASTCALL DrawNumberAt(br_pixelmap* pSrc, br_pixelmap* pDest, int pX, int pY, int pX_pitch, int pY_pitch, int pValue ,int pDigit_count, int pLeading_zeroes) {
    int i;

    for (i = pDigit_count - 1; i >= 0; i--) {
        int the_value = pValue % 10;
        pValue /= 10;
        if (pValue != 0 || pLeading_zeroes || pDigit_count - 1 == i) {
            DRPixelmapRectangleMaskedCopy(pDest, pX + pX_pitch * i, pY, pSrc, 0, the_value * pY_pitch, pSrc->width, pY_pitch);
        }
    }
}
C2_HOOK_FUNCTION(0x004657c0, DrawNumberAt)

void C2_HOOK_FASTCALL DisposeSavedShadeTables(void) {
    int i;

    for (i = 0; i < C2V(gSaved_table_count); i++) {
        BrMemFree(C2V(gSaved_shade_tables)[i].copy);
    }
}
C2_HOOK_FUNCTION(0x004e9b60, DisposeSavedShadeTables)

void C2_HOOK_FASTCALL KillSplashScreen(void) {

    if (C2V(gCurrent_splash) != NULL) {
        BrMapRemove(C2V(gCurrent_splash));
        BrPixelmapFree(C2V(gCurrent_splash));
        C2V(gCurrent_splash) = NULL;
    }
}
C2_HOOK_FUNCTION(0x0047b960, KillSplashScreen)

void C2_HOOK_FASTCALL InitTargetLock(void) {

    C2V(gTarget_lock_enabled) = 0;
}
C2_HOOK_FUNCTION(0x004948a0, InitTargetLock)

void C2_HOOK_FASTCALL SetIntegerMapRenders(void) {

    C2V(gMap_render_width) = 128.f;
    C2V(gMap_render_height) = 80.f;
    C2V(gMap_render_x_i) = ((int)C2V(gMap_render_x)) & ~3;
    C2V(gMap_render_y_i) = ((int)C2V(gMap_render_y)) & ~1;
    C2V(gMap_render_width_i) = (int)C2V(gMap_render_width);
    C2V(gMap_render_height_i) = (int)C2V(gMap_render_height);
}
C2_HOOK_FUNCTION(0x004948b0, SetIntegerMapRenders)

void C2_HOOK_FASTCALL VerifyPaletteBlackness(br_pixelmap* pPalette) {
    int modified;
    int i;
    br_colour *pixels;

    pixels = pPalette->pixels;
    modified = pixels[0] != BR_COLOUR_RGBA(0, 0, 0, 0);
    if (modified) {
        pixels[0] = BR_COLOUR_RGBA(0, 0, 0, 0);
    }
    for (i = 1; i < 256; i++) {
        br_colour c = pixels[i];
        if (BR_COLOUR_RED(c) == 0 && BR_COLOUR_GRN(c) == 0 && BR_COLOUR_BLU(c) == 0) {
            pixels[i] = BR_COLOUR_RGB(1, 1, 1);
            modified = 1;
        }
    }
    if (modified) {
        BrMapUpdate(pPalette, BR_MAPU_ALL);
    }
}
C2_HOOK_FUNCTION(0x00517fa0, VerifyPaletteBlackness)

void C2_HOOK_FASTCALL ResetPalette(void) {

    InitPaletteAnimate();
    DRSetPalette(C2V(gRender_palette));
}
C2_HOOK_FUNCTION(0x004b5330, ResetPalette)

void C2_HOOK_FASTCALL EnsureRenderPalette(void) {

    if (C2V(gPalette_munged)) {
        ResetPalette();
        C2V(gPalette_munged) = 0;
    }
}
C2_HOOK_FUNCTION(0x004b5770, EnsureRenderPalette)

void (C2_HOOK_FASTCALL * RenderAFrame_original)(int pDepth_mask_on);
void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on) {

#if defined(C2_HOOKS_ENABLED)
    RenderAFrame_original(pDepth_mask_on);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4e40, RenderAFrame, RenderAFrame_original)

void (C2_HOOK_FASTCALL * ResetLollipopQueue_original)(void);
void C2_HOOK_FASTCALL ResetLollipopQueue(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetLollipopQueue_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4900, ResetLollipopQueue, ResetLollipopQueue_original)
