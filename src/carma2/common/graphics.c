#include "graphics.h"

#include "car.h"
#include "controls.h"
#include "depth.h"
#include "displays.h"
#include "errors.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "oil.h"
#include "mainloop.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "pedestrn.h"
#include "polyfont.h"
#include "physics.h"
#include "piping.h"
#include "polyfont.h"
#include "powerups.h"
#include "replay.h"
#include "spark.h"
#include "tinted.h"
#include "trig.h"
#include "utility.h"
#include "video.h"
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
C2_HOOK_VARIABLE_IMPLEMENT(float, gCamera_to_horiz_angle, 0x00704e44);
C2_HOOK_VARIABLE_IMPLEMENT(float, gYon_squared, 0x00762100);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMirror_on__graphics, 0x00704e40);
C2_HOOK_VARIABLE_IMPLEMENT(int, gShadow_clip_plane_count, 0x006a27dc);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_vector3, gShadow_points, 8, 0x006a23e0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_matrix34, gIdentity34, 0x00655e00, {
    {
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 1.0 },
        { 0.0, 0.0, 0.0 },
    }
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gShadow_dim_amount, 0x006a2440);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tOppo_status_messages, gOppo_status_messages, 9, 0x0068d6f8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gInitMap_done, 0x0068d8cc);
C2_HOOK_VARIABLE_IMPLEMENT(float, gHeadup_map_x_float, 0x0074abdc);
C2_HOOK_VARIABLE_IMPLEMENT(float, gHeadup_map_y_float, 0x0074abd8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gHeadup_map_w_float, 0x0074abcc);
C2_HOOK_VARIABLE_IMPLEMENT(float, gHeadup_map_h_float, 0x0074ab80);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gUINT_0074ab88, 0x0074ab88);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gUINT_0074ab8c, 0x0074ab8c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0074ab94, 0x0074ab94);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0074abec, 0x0074abec);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0074abe4, 0x0074abe4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0074ab98, 0x0074ab98);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gMap_overlay, 0x0068d8d8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gMini_map, 0x0068d8dc);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gCheckpoint_numbers, 0x0068d6e4);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gSmashy_dot, 0x0068d884);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gAlt_meter, 0x0068c87c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCheckpoint_digit_height, 0x0068c860);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCheckpoint_digit_center_x, 0x0068d8bc);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gCheckpoint_digit_center_y, 0x0068c85c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMini_map_x, 0x0074aba0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMini_map_y, 0x0074aba4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0074abd4, 0x0074abd4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0074abd0, 0x0074abd0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00655f04, 0x00655f04);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00655f08, 0x00655f08);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_0068d8a0, 0x0068d8a0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_0068d8a4, 0x0068d8a4);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_0074ab9c, 0x0074ab9c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_0074abb8, 0x0074abb8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMini_map_arrow_z, 0x0068d8a8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_lollipops, 0x006a22c4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gScreen_wobble_x, 0x00705188);
C2_HOOK_VARIABLE_IMPLEMENT(int, gScreen_wobble_y, 0x00705184);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHud_actor_storage_size, 0x00703e28);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_ADV_INIT(const int, gArrows, [2][4][60], 0x006593a0, {
    {
        // inner arrow (=fill)
        { 10,  0,  0, -1,  0,  1,  0,  0, -1,  0, -2,  0,  1, -1,  1,  1,  1, -2,  2,  2,  2, },
        { 11,  0,  0, -1,  0,  1,  0,  0, -1,  1, -1,  1, -2, -2,  1, -1,  1,  0,  1,  1,  1,  1,  2, },
        {  9,  0,  0, -2,  0, -1,  0,  1,  0,  0, -1,  1, -1,  2, -2,  0,  1,  0,  2, },
        { 11,  0,  0, -1,  0,  1,  0, -2, -1, -1, -1,  0, -1,  1, -1,  2, -1, -1,  1,  0,  1, -1,  2, },
    },
    {
        // outer arrow (=border)
        { 26,  1, -3,  1, -2,  1, -1,  2, -1,  2,  0,  2,  1,  3,  1,  3,  2,  3,  3,  2,  3,  1,  3,  1,  2,  0,  2, -1,  2,
          -1,  3, -2,  3, -3,  3, -3,  2, -3,  1, -2,  1, -2,  0, -2, -1, -1, -1, -1, -2, -1, -3,  0, -3, },
        { 22,  0, -3,  1, -3,  2, -3,  2, -2,  2, -1,  2,  0,  2,  1,  2,  2,  2,  3,  1,  3,  0,  3,  0,  2, -1,  2, -2,  2,
          -3,  2, -3,  1, -3,  0, -2,  0, -2, -1, -1, -1, -1, -2,  0, -2, },
        { 24,  1, -3,  2, -3,  3, -3,  3, -2,  3, -1,  2, -1,  2,  0,  2,  1,  1,  1,  1,  2,  1,  3,  0,  3, -1,  3, -1,  2,
          -1,  1, -2,  1, -3,  1, -3,  0, -3, -1, -2, -1, -1, -1, -1, -2,  0, -2,  1, -2, },
        { 22, -3, -2, -2, -2, -1, -2,  0, -2,  1, -2,  2, -2,  3, -2,  3, -1,  3,  0,  2,  0,  2,  1,  1,  1,  1,  2,  0,  2,
           0,  3, -1,  3, -2,  3, -2,  2, -2,  1, -2,  0, -3,  0, -3, -1, },
    },
});
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix34, gCar_in_map_space, 0x0068d6a8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHeadup_map_half_width, 0x0074abe4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHeadup_map_half_height, 0x0074ab98);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0068c858, 0x0068c858);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0068d890, 0x0068d890);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_0068d6e0, 0x0068d6e0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_0074ab90, 0x0074ab90);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector2, gVector2_0068d6d8, 0x0068d6d8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const int, gMini_map_glowing_line_animation_indices, 5, 0x006569b0, {
    0, 1, 2, 1, 0
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_matrix34, gSheer_mat, 0x0065fb20, {
    { { 1.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 1.0 },
      { 0.0, 0.0, 0.0 } }
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gAR_fudge_headups, 0x006a2358);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tOpponent_Status, gPrevious_opponent_status, 0x00659b20, eOpponent_status_Uninitialized);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gTime_oppobar_target_wasted, 0x0068d8d0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gPrevious_rear_yon, 0x006a234c);

#define SHADOW_D_IGNORE_FLAG 10000.f
#define SHADOW_MAX_RENDER_DISTANCE 36.0f

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

void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette) {

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

void C2_HOOK_FASTCALL Darken(tU8* pPtr, unsigned int pDarken_amount) {

    *pPtr = (pDarken_amount * *pPtr) / 256;
}

void C2_HOOK_FASTCALL SetFadedPalette(int pDegree) {
    int j;

    c2_memcpy(C2V(gScratch_pixels), C2V(gCurrent_palette)->pixels, 4 * 256);
    for (j = 0; j < 256; j++) {
        Darken((tU8*)&C2V(gScratch_pixels)[4 * j + 0], pDegree);
        Darken((tU8*)&C2V(gScratch_pixels)[4 * j + 1], pDegree);
        Darken((tU8*)&C2V(gScratch_pixels)[4 * j + 2], pDegree);
        Darken((tU8*)&C2V(gScratch_pixels)[4 * j + 3], pDegree);
    }
    DRSetPalette2(C2V(gScratch_palette), 0);
}

void C2_HOOK_FASTCALL FadePaletteUp(void) {
    int i;
    int start_time;
    int the_time;

    if (C2V(gFaded_palette)) {
        C2V(gFaded_palette) = 0;
        start_time = PDGetTotalTime();
        while (1) {
            the_time = PDGetTotalTime() - start_time;
            if (the_time >= 500) {
                break;
            }
            i = (the_time * 256) / 500;
            SetFadedPalette(i);
        }
        DRSetPalette(C2V(gCurrent_palette));
    }
}
C2_HOOK_FUNCTION(0x004b5470, FadePaletteUp)

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

    (void)C2V(button_was_down);

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
            SetTintedPolySize(C2V(gCursor_tinted_top),
                pos_x - 1, 0,
                C2V(gCursor_line_width), pos_y - 9);
            SetTintedPolySize(C2V(gCursor_tinted_left),
                0, pos_y - 1,
                pos_x - 9, C2V(gCursor_line_width));
            SetTintedPolySize(C2V(gCursor_tinted_bottom),
                pos_x - 1, pos_y + 5 + C2V(gCursor_line_width),
                C2V(gCursor_line_width), C2V(gCurrent_graf_data)->height - C2V(gCursor_line_width) - pos_y + 9);
            SetTintedPolySize(C2V(gCursor_tinted_right),
                pos_x + 5 + C2V(gCursor_line_width), pos_y - 1,
                C2V(gCurrent_graf_data)->width - C2V(gCursor_line_width) - pos_x + 9, C2V(gCursor_line_width));
            SetTintedPolySize(C2V(gCursor_tinted_center),
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

void C2_HOOK_FASTCALL ReadMoodMessages(void) {
    tPath_name path;
    FILE *f;
    int mood;

    PathCat(path, C2V(gApplication_path), "STATUS.TXT");
    f = PFfopen(path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantFindFile_S, "STATUS.TXT");
    }
    for (mood = 0; mood < REC2_ASIZE(C2V(gOppo_status_messages)); mood += 1) {
        int i;

        C2_HOOK_ASSERT(mood < REC2_ASIZE(C2V(gOppo_status_messages)));

        C2V(gOppo_status_messages)[mood].count = GetAnInt(f);
        C2_HOOK_ASSERT(C2V(gOppo_status_messages)[mood].count < REC2_ASIZE(C2V(gOppo_status_messages)[mood].messages));
        for (i = 0; i < C2V(gOppo_status_messages)[mood].count; i++) {
            char s[256];

            GetALineAndDontArgue(f, s);
            C2V(gOppo_status_messages)[mood].messages[i] = (char *) BrMemAllocate(c2_strlen(s) + 1, kMem_misc_string);
            c2_strcpy(C2V(gOppo_status_messages)[mood].messages[i], s);
        }
    }
    PFfclose(f);
}

void (C2_HOOK_FASTCALL * InitMap_original)(void);
void C2_HOOK_FASTCALL InitMap(void) {

#if defined(C2_HOOKS_ENABLED)
    InitMap_original();
#else

    if (!C2V(gInitMap_done)) {
        C2V(gHeadup_map_x_float) = (float)C2V(gHeadup_map_x);
        C2V(gHeadup_map_y_float) = (float)C2V(gHeadup_map_y);
        C2V(gHeadup_map_w_float) = (float)C2V(gHeadup_map_w);
        C2V(gHeadup_map_h_float) = (float)C2V(gHeadup_map_h);
    } else {
        C2V(gHeadup_map_x) = ((int)C2V(gHeadup_map_x_float) + 3) & ~3;
        C2V(gHeadup_map_y) = (int)C2V(gHeadup_map_y_float);
        C2V(gHeadup_map_w) = ((int)C2V(gHeadup_map_w_float) + 3) % ~3;
        C2V(gHeadup_map_h) = (int)C2V(gHeadup_map_h_float);
    }
    if (C2V(gHeadup_map_x) % 4 != 0) {
        C2V(gHeadup_map_x) -= C2V(gHeadup_map_x) % 4;
        C2V(gHeadup_map_x_float) = (float)C2V(gHeadup_map_x);
    }
    if (C2V(gHeadup_map_w) % 4 != 0) {
        C2V(gHeadup_map_w) -= C2V(gHeadup_map_w) % 4;
        C2V(gHeadup_map_w_float) = (float)C2V(gHeadup_map_w);
    }
    if (C2V(gHeadup_map_w) > 64) {
        C2V(gHeadup_map_w) = 64;
        C2V(gHeadup_map_w_float) = 64.f;
    }
    if (C2V(gHeadup_map_h) > 64) {
        C2V(gHeadup_map_h) = 64;
        C2V(gHeadup_map_h_float) = 64.f;
    }
    C2V(gUINT_0074ab8c) = (int)(3.f + 0.3f * (float)C2V(gHeadup_map_w)) & ~3;
    C2V(gUINT_0074ab88) = (int)(3.f + 0.3f * (float)C2V(gHeadup_map_h)) & ~3;
    if (C2V(gUINT_0074ab8c) > C2V(gUINT_0074ab88)) {
        C2V(gUINT_0074ab88) = C2V(gUINT_0074ab8c);
    } else {
        C2V(gUINT_0074ab8c) = C2V(gUINT_0074ab88);
    }
    C2V(gINT_0074ab94) = C2V(gHeadup_map_w) + 2 * C2V(gUINT_0074ab88);
    C2V(gINT_0074abec) = C2V(gHeadup_map_h) + 2 * C2V(gUINT_0074ab88);
    C2V(gINT_0074abe4) = C2V(gHeadup_map_w) / 2;
    C2V(gINT_0074ab98) = C2V(gHeadup_map_h) / 2;
    if (C2V(gMap_overlay) == NULL) {
        int side = MAX(C2V(gINT_0074abec), C2V(gINT_0074ab94));
        C2V(gMap_overlay) = DRPixelmapAllocate(C2V(gBack_screen)->type, side, side, NULL, 0);
    }
    C2V(gMini_map_x) = C2V(gHeadup_map_x) + C2V(gHeadup_map_w) / 16;
    C2V(gMini_map_y) = C2V(gHeadup_map_y) + C2V(gHeadup_map_h) / 16;
    if (C2V(gMini_map_x) % 2 != 0) {
        C2V(gMini_map_x) -= 1;
    }
    C2V(gINT_0074abd4) = C2V(gHeadup_map_w) / 2 - C2V(gMini_map_x) + C2V(gHeadup_map_x);
    C2V(gINT_0074abd0) = C2V(gHeadup_map_h) / 2 - C2V(gMini_map_y) + C2V(gHeadup_map_y);
    if (C2V(gMini_map) != NULL) {
        BrPixelmapFree(C2V(gMini_map));
    }
    C2V(gINT_00655f04) = C2V(gHeadup_map_x);
    C2V(gINT_00655f08) = C2V(gHeadup_map_y);
    C2V(gMini_map) = DRPixelmapAllocateSub(C2V(gBack_screen),
        C2V(gMini_map_x),
        C2V(gMini_map_y),
        C2V(gHeadup_map_w) - (C2V(gHeadup_map_w) + 8) / 8 + 8,
        C2V(gHeadup_map_h) - C2V(gHeadup_map_h) / 8);
    C2V(gMini_map)->pixels = (br_uint_8*)C2V(gMini_map)->pixels + C2V(gMini_map_y) * C2V(gMini_map)->row_bytes + C2V(gMini_map_x);
    C2V(gMini_map)->base_x = C2V(gMini_map_x);
    C2V(gMini_map)->base_y = 0;
    C2V(gFLOAT_0068d8a0) = (float)C2V(gINT_0074abd4);
    C2V(gFLOAT_0068d8a4) = (float)C2V(gINT_0074abd0);
    C2V(gMini_map_arrow_z) = 0.f;
    C2V(gFLOAT_0074ab9c) = REC2_SQR((float)(C2V(gINT_0074ab98) + C2V(gINT_0074abe4)) * 0.45f);
    C2V(gFLOAT_0074abb8) = REC2_SQR((float)(C2V(gCurrent_graf_data)->field_0x51c + 1));
    if (!C2V(gInitMap_done)) {
        C2V(gCheckpoint_numbers) = LoadPixelmap("CPNUMB.PIX");
        if (C2V(gCheckpoint_numbers) == NULL) {
            FatalError(kFatalError_CantFindFile_S, "CPNUMB.PIX");
        }
        BRPM_convert(C2V(gCheckpoint_numbers), C2V(gBack_screen)->type);
        C2V(gSmashy_dot) = LoadPixelmap("SMASHY.PIX");
        if (C2V(gSmashy_dot) == NULL) {
            FatalError(kFatalError_CantFindFile_S, "SMASHY.PIX");
        }
        BRPM_convert(C2V(gSmashy_dot), C2V(gBack_screen)->type);
        C2V(gAlt_meter) = LoadPixelmap("ALTMETER.PIX");
        if (C2V(gAlt_meter) == NULL) {
            FatalError(kFatalError_CantFindFile_S, "ALTMETER.PIX");
        }
        C2V(gCheckpoint_digit_height) = C2V(gCheckpoint_numbers)->height / 10;
        C2V(gCheckpoint_digit_center_y) = C2V(gCheckpoint_digit_height) / 2;
        C2V(gCheckpoint_digit_center_x) = C2V(gCheckpoint_numbers)->width / 2;
        C2V(gIcons_pix) = LoadPixelmap("CARICONS.PIX");
        if (C2V(gIcons_pix) != NULL) {
            BRPM_convert(C2V(gIcons_pix), C2V(gBack_screen)->type);
            BrMapAdd(C2V(gIcons_pix));
        }
        ReadMoodMessages();
    }
    C2V(gInitMap_done) = 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00496e30, InitMap, InitMap_original)

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
    SetTintedPolySize(C2V(gHud_tinted1),
        C2V(gRender_screen)->base_x,
        C2V(gRender_screen)->base_y,
        C2V(gRender_screen)->width,
        C2V(gRender_screen)->height);
    SetTintedPolySize(C2V(gHud_tinted2),
        C2V(gRender_screen)->base_x,
        C2V(gRender_screen)->base_y,
        C2V(gRender_screen)->width,
        C2V(gRender_screen)->height);
    SetTintedPolySize(C2V(gHud_tinted4),
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

void C2_HOOK_FASTCALL CleanPolyFontDanglers(void) {
    int i;

    for (i = 0; i < C2V(gCount_polyfont_glyph_actors); i++) {
        br_actor* a = C2V(gPolyfont_glyph_actors)[i];
        if (a->parent != NULL) {
            BrActorRemove(a);
        }
    }
    C2V(gCount_polyfont_glyph_actors) = 0;
}

void C2_HOOK_FASTCALL StartRenderingHeadups(void) {

    CleanPolyFontDanglers();
    C2V(gHud_actor_storage_size) = 0;
    C2V(gRender_poly_text) = 0;
}
C2_HOOK_FUNCTION(0x004e5a70, StartRenderingHeadups)

void C2_HOOK_FASTCALL CalculateWobblitude(tU32 pThe_time) {
    int i;
    tU32 time_going;
    double angle;
    double mod_angle;
    double cosine_over_angle;

    if (C2V(gProgram_state).new_view != eView_undefined) {
        return;
    }
    C2V(gScreen_wobble_x) = 0;
    C2V(gScreen_wobble_y) = 0;
    for (i = 0; i < REC2_ASIZE(C2V(gWobble_array)); i++) {
        if (C2V(gWobble_array)[i].time_started != 0) {
            time_going = pThe_time - C2V(gWobble_array)[i].time_started;
            if (time_going > 1000) {
                C2V(gWobble_array)[i].time_started = 0;
            } else {
                mod_angle = fmodf(time_going / C2V(gWobble_array)[i].period, TAU_F);
                if (mod_angle > 1.5f * PI_F) {
                    cosine_over_angle = C2V(gCosine_array)[(unsigned int)((TAU_F - mod_angle) / PI_F * 128.f)];
                } else if (mod_angle > PI_F) {
                    cosine_over_angle = -C2V(gCosine_array)[(unsigned int)((mod_angle - PI_F) / PI_F * 128.f)];
                } else if (mod_angle > .5f * PI_F) {
                    cosine_over_angle = -C2V(gCosine_array)[(unsigned int)((PI_F - mod_angle) / PI_F * 128.f)];
                } else {
                    cosine_over_angle = C2V(gCosine_array)[(unsigned int)(mod_angle / PI_F * 128.f)];
                }
                angle = cosine_over_angle / (1.f + time_going * .0035f);
                C2V(gScreen_wobble_x) += (int)(C2V(gWobble_array)[i].amplitude_x * angle);
                C2V(gScreen_wobble_y) += (int)(C2V(gWobble_array)[i].amplitude_y * angle);
            }
        }
    }
    if (C2V(gScreen_wobble_x) > C2V(gCurrent_graf_data)->cock_margin_x) {
        C2V(gScreen_wobble_x) = C2V(gCurrent_graf_data)->cock_margin_x;
    } else if (C2V(gScreen_wobble_x) < -C2V(gCurrent_graf_data)->cock_margin_x) {
        C2V(gScreen_wobble_x) = -C2V(gCurrent_graf_data)->cock_margin_x;
    }
    if (C2V(gScreen_wobble_y) > C2V(gCurrent_graf_data)->cock_margin_y) {
        C2V(gScreen_wobble_y) = C2V(gCurrent_graf_data)->cock_margin_y;
    } else if (C2V(gScreen_wobble_y) < -C2V(gCurrent_graf_data)->cock_margin_y) {
        C2V(gScreen_wobble_y) = -C2V(gCurrent_graf_data)->cock_margin_y;
    }
    PipeSingleScreenWobble(C2V(gScreen_wobble_x), C2V(gScreen_wobble_y));
}

void (C2_HOOK_FASTCALL * RenderAFrame_original)(int pDepth_mask_on);
void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on) {

#if defined(C2_HOOKS_ENABLED)
    RenderAFrame_original(pDepth_mask_on);
#else
    int i;
    int x_shift;
    int y_shift;
    int cockpit_on;
    int real_origin_x = 0;
    int real_origin_y = 0;
    int real_base_x = 0;
    int real_base_y = 0;
    char* old_pixels;
    br_matrix34 old_camera_matrix;
    br_matrix34 old_mirror_cam_matrix;
    tU32 the_time;

    C2V(gRender_screen)->pixels = C2V(gBack_screen)->pixels;
    the_time = GetTotalTime();
    old_pixels = C2V(gRender_screen)->pixels;
    cockpit_on = C2V(gProgram_state).cockpit_on && C2V(gProgram_state).cockpit_image_index >= 0 && C2V(gMap_view) != 2;
    C2V(gMirror_on__graphics) = C2V(gProgram_state).mirror_on && cockpit_on && C2V(gProgram_state).which_view == eView_forward;

    StartRenderingHeadups();
    MapStuffBeforeRender();
    if (!C2V(gAction_replay_mode)) {
        CalculateWobblitude(the_time);
    }
    if (cockpit_on) {
        if (-C2V(gScreen_wobble_x) > C2V(gX_offset)) {
            x_shift = -C2V(gX_offset);
        } else if (C2V(gScreen_wobble_x) + C2V(gX_offset) + C2V(gRender_screen)->width > C2V(gBack_screen)->width) {
            x_shift = C2V(gBack_screen)->width - C2V(gRender_screen)->width - C2V(gX_offset);
        } else {
            x_shift = C2V(gScreen_wobble_x);
        }
        if (-C2V(gScreen_wobble_y) > C2V(gY_offset)) {
            y_shift = -C2V(gY_offset);
        } else if (C2V(gScreen_wobble_y) + C2V(gY_offset) + C2V(gRender_screen)->height > C2V(gBack_screen)->height) {
            y_shift = C2V(gBack_screen)->height - C2V(gRender_screen)->height - C2V(gY_offset);
        } else {
            y_shift = C2V(gScreen_wobble_y);
        }
    } else {
        x_shift = 0;
        y_shift = 0;
    }
    BrMatrix34Copy(&old_camera_matrix, &C2V(gCamera)->t.t.mat);
    if (C2V(gMirror_on__graphics)) {
        BrMatrix34Copy(&old_mirror_cam_matrix, &C2V(gRearview_camera)->t.t.mat);
    }
    if (cockpit_on) {
        C2V(gSheer_mat).m[2][1] = y_shift / (float)C2V(gRender_screen)->height;
        C2V(gSheer_mat).m[2][0] = -x_shift / (float)C2V(gRender_screen)->width;
        BrMatrix34Pre(&C2V(gCamera)->t.t.mat, &C2V(gSheer_mat));
        C2V(gCamera)->t.t.translate.t.v[0] -= C2V(gScreen_wobble_x) * 1.5f / C2V(gRender_screen)->width / WORLD_SCALE;
        C2V(gCamera)->t.t.translate.t.v[1] += C2V(gScreen_wobble_y) * 1.5f / C2V(gRender_screen)->width / WORLD_SCALE;
    }
    C2V(gRender_screen)->pixels = (char*)C2V(gRender_screen)->pixels + x_shift + y_shift * C2V(gRender_screen)->row_bytes;
    if (C2V(gRender_indent) && C2V(gMap_view) != 2) {
        BrPixelmapRectangleFill(
                C2V(gBack_screen),
                0,
                0,
                C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width,
                C2V(gProgram_state).current_render_top,
                0);
        BrPixelmapRectangleFill(
                C2V(gBack_screen),
                0,
                C2V(gProgram_state).current_render_bottom,
                C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width,
                C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height - C2V(gProgram_state).current_render_bottom,
                0);
        BrPixelmapRectangleFill(
                C2V(gBack_screen),
                0,
                C2V(gProgram_state).current_render_top,
                C2V(gProgram_state).current_render_left,
                C2V(gProgram_state).current_render_bottom - C2V(gProgram_state).current_render_top,
                0);
        BrPixelmapRectangleFill(
                C2V(gBack_screen),
                C2V(gProgram_state).current_render_right,
                C2V(gProgram_state).current_render_top,
                C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width - C2V(gProgram_state).current_render_right,
                C2V(gProgram_state).current_render_bottom - C2V(gProgram_state).current_render_top,
                0);
    }
    C2V(gRendering_mirror) = 0;
    for (i = 0; i < 1; i++) {
        DoACompleteRenderPass(0, &C2V(gCamera_to_world), C2V(gCamera), C2V(gRender_screen), C2V(gDepth_buffer));
    }
    C2V(gCamera)->t.t.mat = old_camera_matrix;
    if (C2V(gMirror_on__graphics)) {
        DoACompleteRenderPass(1, &C2V(gRearview_camera_to_world), C2V(gRearview_camera), C2V(gRearview_screen), C2V(gRearview_depth_buffer));
        C2V(gRearview_camera)->t.t.mat = old_mirror_cam_matrix;
    }
    RenderTintedPolys();
    if (C2V(gMap_view) != 2) {
        DimAFewBits();
        DoDamageScreen(the_time);
        if (!C2V(gAction_replay_mode) || C2V(gAR_fudge_headups)) {
            DoHeadups(the_time);
        }
        DoInstruments(the_time);
        if (!C2V(gAction_replay_mode) || C2V(gAR_fudge_headups)) {
            DrawPowerups(the_time);
        }
    }
    if (C2V(gMap_view) != 2 && (!C2V(gAction_replay_mode) || C2V(gAR_fudge_headups))) {
        DoTestHeadup();
        DoPSPowerupHeadups();
    }
    MapStuffAfterRender();
    if (C2V(gNet_mode) != eNet_mode_none) {
        DisplayUserMessage();
    }
    if (C2V(gAction_replay_mode)) {
        MovieRecordFrame(C2V(gBack_screen), C2V(gFrame_period));
    }
    if (C2V(gAction_replay_mode) && !C2V(gAR_fudge_headups)) {
        DoActionReplayHeadups();
    }
    StopRenderingHeadups();
    if (C2V(gAction_replay_mode)) {
        SynchronizeActionReplay();
    } else {
        PipeSingleFrameFinish();
    }
    C2V(gRender_screen)->pixels = old_pixels;
    if (!C2V(gPalette_fade_time) || GetRaceTime() > C2V(gPalette_fade_time) + 500u) {
        PDScreenBufferSwap(0);
    }
    if (C2V(gAction_replay_mode)) {
        DoActionReplayPostSwap();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4e40, RenderAFrame, RenderAFrame_original)

void (C2_HOOK_FASTCALL * ResetLollipopQueue_original)(void);
void C2_HOOK_FASTCALL ResetLollipopQueue(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ResetLollipopQueue_original();
#else
    C2V(gNumber_of_lollipops) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4900, ResetLollipopQueue, ResetLollipopQueue_original)

void C2_HOOK_FASTCALL RevertPalette(void) {

    c2_memcpy(C2V(gRender_palette)->pixels, C2V(gOrig_render_palette)->pixels, 256 * sizeof(br_colour));
    DRSetPalette3(C2V(gRender_palette), 1);
}
C2_HOOK_FUNCTION(0x004b52b0, RevertPalette)

void C2_HOOK_FASTCALL MungeClipPlane(br_vector3* pLight, tCar_spec* pCar, br_vector3* p1, br_vector3* p2, br_vector3* pOffset) {
    br_vector3 v1;
    br_vector3 v2;
    br_vector3 v3;
    br_vector3 v4;
    br_scalar length;
    br_actor* new_clip;

    BrMatrix34ApplyP(&v1, p1, &pCar->car_master_actor->t.t.mat);
    BrMatrix34ApplyP(&v2, p2, &pCar->car_master_actor->t.t.mat);
    BrVector3Sub(&v3, p2, p1);
    BrVector3Cross(&v4, &v3, pLight);
    if (fabsf(v4.v[0]) >= 0.01f || fabsf(v4.v[1]) >= 0.01f || fabsf(v4.v[2]) >= 0.01f) {
        BrVector3Sub(&v3, p1, pOffset);
        if (BrVector3Dot(&v3, &v4) > 0.f) {
            BrVector3Negate(&v4, &v4);
        }
        BrVector3Normalise(&v3, &v4);
        BrMatrix34ApplyV(&v4, &v3, &pCar->car_master_actor->t.t.mat);

        new_clip = C2V(gShadow_clip_planes)[C2V(gShadow_clip_plane_count)].clip;
        ((br_vector4*)new_clip->type_data)->v[0] = v4.v[0];
        ((br_vector4*)new_clip->type_data)->v[1] = v4.v[1];
        ((br_vector4*)new_clip->type_data)->v[2] = v4.v[2];
        ((br_vector4*)new_clip->type_data)->v[3] = -BrVector3Dot(&v1, &v4);
        length = REC2_SQR(v1.v[2] - v2.v[2]) + REC2_SQR(v1.v[0] - v2.v[0]);
        C2V(gShadow_clip_planes)[C2V(gShadow_clip_plane_count)].length = length;
        C2V(gShadow_clip_plane_count) += 1;
    }
}
C2_HOOK_FUNCTION(0x004e9680, MungeClipPlane)

br_scalar C2_HOOK_FASTCALL DistanceFromPlane(br_vector3* pPos, br_scalar arg2, br_scalar pA, br_scalar pB, br_scalar pC, br_scalar pD) {

    return fabsf((pPos->v[0] * pA + pPos->v[1] * pB + pPos->v[2] * pC + pD) / (pA * pA + pB * pB + pC * pC));
}
C2_HOOK_FUNCTION(0x0047b910, DistanceFromPlane)

void C2_HOOK_FASTCALL DoTestHeadup(void) {
    static C2_HOOK_VARIABLE_IMPLEMENT(int, do_headup_material, 0x006815c0);

    C2V(do_headup_material) = !C2V(do_headup_material);
    if (C2V(do_headup_material)) {
        BrMapUpdate(C2V(gCurrent_rev), BR_MAPU_ALL);
        C2V(gStatbarHUD1_material)->colour_map = C2V(gCurrent_rev);
        BrMaterialUpdate(C2V(gStatbarHUD1_material), BR_MATU_COLOURMAP);
    } else {
        BrMapUpdate(C2V(gDamage_hud), BR_MAPU_ALL);
        C2V(gStatbarHUD5_material)->colour_map = C2V(gDamage_hud);
        BrMaterialUpdate(C2V(gStatbarHUD5_material), BR_MATU_COLOURMAP);
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        BrMatrix34Translate(&C2V(gArmour_actor)->t.t.mat, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&C2V(gPower_actor)->t.t.mat, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&C2V(gOffense_actor)->t.t.mat, 0.f, 0.f, 0.f);
        if (C2V(gHeadup_detail_level) % 3 > 0) {
            BrMatrix34Translate(&C2V(gStatbarHUD1_actor)->t.t.mat, 142.f, 0.f, 0.f);
            BrMatrix34Translate(&C2V(gStatbarHUD5_actor)->t.t.mat, -178.f, 0.f, 0.f);
            RenderThisHeadup(C2V(gStatbarHUD1_actor));
            RenderThisHeadup(C2V(gStatbarHUD5_actor));
            RenderThisHeadup(C2V(gTimerLeftHUD_actor));
            RenderThisHeadup(C2V(gTimerRightHUD_actor));
            if (C2V(gHeadup_detail_level) % 3 > 1) {
                RenderThisHeadup(C2V(gArmour_actor));
                RenderThisHeadup(C2V(gPower_actor));
                RenderThisHeadup(C2V(gOffense_actor));
                RenderThisHeadup(C2V(gStatbarRightHUD_actor));
                RenderThisHeadup(C2V(gStatbarHUD3_actor));
                BrMatrix34Translate(&C2V(gStatbarHUD1_actor)->t.t.mat, 0.f, 0.f, 0.f);
                BrMatrix34Translate(&C2V(gStatbarHUD5_actor)->t.t.mat, 0.f, 0.f, 0.f);
            } else {
                RenderPolyTextLine(C2V(gHeadup_oppo_ped_text), 335, 50, kPolyfont_ingame_medium_blue, eJust_centre, 0);
            }
        }
        if (C2V(gHeadup_detail_level) >= 1) {
            RenderThisHeadup(C2V(gHeadup_actor));
        }
    } else {
        if (C2V(gHeadup_detail_level) % 3 >= 1) {
            BrMatrix34Translate(&C2V(gStatbarHUD5_actor)->t.t.mat, -448.f, 0.f, 0.f);
            RenderThisHeadup(C2V(gStatbarHUD5_actor));
            RenderThisHeadup(C2V(gStatbarHUD1_actor));
            if (C2V(gHeadup_detail_level) % 3 >= 2) {
                BrMatrix34Translate(&C2V(gArmour_actor)->t.t.mat, 48.f, 35.f, 0.f);
                BrMatrix34Translate(&C2V(gPower_actor)->t.t.mat, -102.f, 5.f, 0.f);
                BrMatrix34Translate(&C2V(gOffense_actor)->t.t.mat, -252.f, -25.f, 0.f);
                RenderThisHeadup(C2V(gArmour_actor));
                RenderThisHeadup(C2V(gPower_actor));
                RenderThisHeadup(C2V(gOffense_actor));
            }
        }
        if (C2V(gHeadup_detail_level) >= 3) {
            RenderThisHeadup(C2V(gHeadup_actor));
        }
    }
}
C2_HOOK_FUNCTION(0x0044b6a0, DoTestHeadup)

void C2_HOOK_FASTCALL TryThisEdge(tCar_spec* pCar, br_vector3* pLight, int pIndex_1, br_scalar pSign_1, int pIndex_2, br_scalar pSign_2, int pPoint_index_1, int pPoint_index_2, br_vector3* pOffset) {
    br_scalar dot_1;
    br_scalar dot_2;
    br_scalar mult;

    dot_1 = pSign_1 * pLight->v[pIndex_1];
    dot_2 = pSign_2 * pLight->v[pIndex_2];
    mult = dot_1 * dot_2;
    if (mult < 0 || (mult == 0 && (dot_1 > 0 || dot_2 > 0))) {
        if (C2V(gShadow_clip_plane_count) < 6) {
            MungeClipPlane(pLight, pCar, &C2V(gShadow_points)[pPoint_index_1], &C2V(gShadow_points)[pPoint_index_2], pOffset);
        }
    }
}

void C2_HOOK_FASTCALL ProcessShadow(tCar_spec* pCar, br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform, br_scalar pDistance_factor) {
    int i;
    int face_count;
    int oily_count;
    int f_num;
    br_vector3 pos;
    br_vector3 light_ray_car;
    br_vector3 shadow_points_world[8];
    br_vector3 poly_centre;
    br_vector3 car_to_poly;
    br_vector3 ray;
    br_vector3 ray_pos;
    br_vector3 normal;
    br_vector4* clip_normal;
    br_bounds3 bounds;
    br_vector3 center;
    br_scalar oily_size;
    br_scalar highest_underneath;
    br_scalar shadow_scaling_factor;
    br_scalar first_poly_below;
    br_scalar distance;
    br_scalar ray_length;
    tBounds kev_bounds;
    tFace_ref the_list[100];
    tFace_ref* face_ref;
    tFace_ref* list_ptr;
    br_actor* oily_actor;
    br_material* material;
    br_vertex verts[48];
    br_face faces[16];
    tUser_crush_data* user_crush_data;
    br_model* car_crush_model;

#if defined(REC2_FIX_BUGS)
    ray_length = 0.f;
#endif

    f_num = 0;
    user_crush_data = pCar->car_model_actor->user;
    car_crush_model = user_crush_data->models[0];
    BrVector3Copy(&bounds.min, &car_crush_model->bounds.min);
    BrVector3Copy(&bounds.max, &car_crush_model->bounds.max);
    BrVector3Set(&C2V(gShadow_points)[0], bounds.max.v[0], bounds.max.v[1], bounds.max.v[2]);
    BrVector3Set(&C2V(gShadow_points)[1], bounds.max.v[0], bounds.max.v[1], bounds.min.v[2]);
    BrVector3Set(&C2V(gShadow_points)[2], bounds.min.v[0], bounds.max.v[1], bounds.min.v[2]);
    BrVector3Set(&C2V(gShadow_points)[3], bounds.min.v[0], bounds.max.v[1], bounds.max.v[2]);
    BrVector3Set(&C2V(gShadow_points)[4], bounds.max.v[0], bounds.min.v[1], bounds.max.v[2]);
    BrVector3Set(&C2V(gShadow_points)[5], bounds.max.v[0], bounds.min.v[1], bounds.min.v[2]);
    BrVector3Set(&C2V(gShadow_points)[6], bounds.min.v[0], bounds.min.v[1], bounds.min.v[2]);
    BrVector3Set(&C2V(gShadow_points)[7], bounds.min.v[0], bounds.min.v[1], bounds.max.v[2]);
    C2V(gShadow_clip_plane_count) = 0;
    BrMatrix34TApplyV(&light_ray_car, &C2V(gShadow_light_ray), &pCar->car_master_actor->t.t.mat);
    Vector3Average(&center, &bounds.max, &bounds.min);
    TryThisEdge(pCar, &light_ray_car, 2,  1.f, 1,  1.f, 0, 3, &center);
    TryThisEdge(pCar, &light_ray_car, 2, -1.f, 1,  1.f, 1, 2, &center);
    TryThisEdge(pCar, &light_ray_car, 2, -1.f, 1, -1.f, 6, 5, &center);
    TryThisEdge(pCar, &light_ray_car, 2,  1.f, 1, -1.f, 7, 4, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 1,  1.f, 1, 0, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 1,  1.f, 2, 3, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 1, -1.f, 7, 6, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 1, -1.f, 4, 5, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 2,  1.f, 4, 0, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 2,  1.f, 3, 7, &center);
    TryThisEdge(pCar, &light_ray_car, 0, -1.f, 2, -1.f, 2, 6, &center);
    TryThisEdge(pCar, &light_ray_car, 0,  1.f, 2, -1.f, 5, 1, &center);

    for (i = 0; i < C2V(gShadow_clip_plane_count); ++i) {
        BrClipPlaneEnable(C2V(gShadow_clip_planes)[i].clip);
    }

    face_count = 0;
    if (!C2V(gAction_replay_mode) && pCar->number_of_wheels_on_ground > 2) {
        face_count = GetPrecalculatedFacesUnderCar(pCar, &face_ref);
    }
    if (face_count != 0) {
        highest_underneath = 0.0;
    } else {
        BrVector3Set(&kev_bounds.original_bounds.min,  1000.f,  1000.f,  1000.f);
        BrVector3Set(&kev_bounds.original_bounds.max, -1000.f, -1000.f, -1000.f);
        for (i = 0; i < REC2_ASIZE(shadow_points_world); i++) {
            BrMatrix34ApplyP(&shadow_points_world[i], &C2V(gShadow_points)[i], &pCar->car_master_actor->t.t.mat);
            if (shadow_points_world[i].v[0] < kev_bounds.original_bounds.min.v[0]) {
                kev_bounds.original_bounds.min.v[0] = shadow_points_world[i].v[0];
            } else if (shadow_points_world[i].v[0] > kev_bounds.original_bounds.max.v[0]) {
                kev_bounds.original_bounds.max.v[0] = shadow_points_world[i].v[0];
            }
            if (shadow_points_world[i].v[1] < kev_bounds.original_bounds.min.v[1]) {
                kev_bounds.original_bounds.min.v[1] = shadow_points_world[i].v[1];
            } else if (shadow_points_world[i].v[1] > kev_bounds.original_bounds.max.v[1]) {
                kev_bounds.original_bounds.max.v[1] = shadow_points_world[i].v[1];
            }
            if (shadow_points_world[i].v[2] < kev_bounds.original_bounds.min.v[2]) {
                kev_bounds.original_bounds.min.v[2] = shadow_points_world[i].v[2];
            } else if (shadow_points_world[i].v[2] > kev_bounds.original_bounds.max.v[2]) {
                kev_bounds.original_bounds.max.v[2] = shadow_points_world[i].v[2];
            }
        }
        kev_bounds.original_bounds.min.v[1] -= 4.4f;
        kev_bounds.mat = &C2V(gIdentity34);
        face_count = FindFacesInBox(&kev_bounds, the_list, REC2_ASIZE(the_list), NULL);
        face_ref = the_list;
        highest_underneath = 1000.f;
        ray_length = kev_bounds.original_bounds.max.v[1] - kev_bounds.original_bounds.min.v[1];
        BrVector3Set(&ray, 0.f, -ray_length, 0.f);
        BrVector3Copy(&ray_pos, &pCar->car_master_actor->t.t.translate.t);
        ray_pos.v[1] = kev_bounds.original_bounds.max.v[1];
    }
    if (face_count != 0) {
        first_poly_below = -1000.0;
        for (i = 0; i < face_count; i++) {
            list_ptr = &face_ref[i];

            if (list_ptr->normal.v[1] >= -0.1 || (list_ptr->material != NULL && (list_ptr->material->flags & BR_MATF_TWO_SIDED))) {
                if (list_ptr->normal.v[1] < 0.0
                        || (list_ptr->material != NULL && list_ptr->material->identifier != NULL && (list_ptr->material->identifier[0] == '!' || list_ptr->material->identifier[0] == '>'))
                        || ((list_ptr->v[0].v[1] > pCar->pos.v[1] || list_ptr->v[1].v[1] > pCar->pos.v[1] || list_ptr->v[2].v[1] > pCar->pos.v[1]) && list_ptr->normal.v[1] < 0.1f)) {
                    list_ptr->d += SHADOW_D_IGNORE_FLAG;
                }
            } else {
                BrVector3Add(&poly_centre, &list_ptr->v[0], &list_ptr->v[1]);
                BrVector3Add(&poly_centre, &poly_centre, &list_ptr->v[2]);
                BrVector3InvScale(&poly_centre, &poly_centre, 3.f);
                if (poly_centre.v[1] > first_poly_below) {
                    BrVector3Sub(&car_to_poly, &poly_centre, &pCar->car_master_actor->t.t.translate.t);

                    if (BrVector3Dot(&list_ptr->normal, &car_to_poly) > 0.0) {
                        first_poly_below = poly_centre.v[1];
                    }
                }
                list_ptr->d += SHADOW_D_IGNORE_FLAG;
            }
        }
        for (i = 0; i < face_count; i++) {
            list_ptr = &face_ref[i];

            if (list_ptr->d < SHADOW_D_IGNORE_FLAG / 2.f) {
                if (list_ptr->v[0].v[1] >= first_poly_below || list_ptr->v[1].v[1] >= first_poly_below || list_ptr->v[2].v[1] >= first_poly_below) {
                    if (C2V(gFancy_shadow)) {
                        faces[f_num].material = list_ptr->material;
                        if (C2V(gShadow_workaround)) {
                            list_ptr->material->ka = 0.75f * C2V(gLighting_data).ambient_else;
                            BrMaterialUpdate(list_ptr->material, BR_MATU_LIGHTING);
                        } else if (list_ptr->material != NULL && list_ptr->material->colour_map != NULL && (!(list_ptr->material->flags & BR_MATF_LIGHT))) {
                            list_ptr->material->flags |= BR_MATF_LIGHT | BR_MATF_SMOOTH;
                            BrMaterialUpdate(list_ptr->material, BR_MATU_RENDERING);
                        }
                    } else {
                        faces[f_num].material = C2V(gShadow_material);
                    }

                    BrVector3Copy(&verts[3 * f_num + 0].p, &list_ptr->v[0]);
                    BrVector2Copy(&verts[3 * f_num + 0].map, list_ptr->map[0]);
                    BrVector3Copy(&verts[3 * f_num + 1].p, &list_ptr->v[1]);
                    BrVector2Copy(&verts[3 * f_num + 1].map, list_ptr->map[1]);
                    BrVector3Copy(&verts[3 * f_num + 2].p, &list_ptr->v[2]);
                    BrVector2Copy(&verts[3 * f_num + 2].map, list_ptr->map[2]);
                    faces[f_num].vertices[0] = 3 * f_num + 0;
                    faces[f_num].vertices[1] = 3 * f_num + 1;
                    faces[f_num].vertices[2] = 3 * f_num + 2;
                    f_num += 1;
                    if (highest_underneath > 0.0) {
                        CheckSingleFace(list_ptr, &ray_pos, &ray, &normal, &distance, &pos);
                        if (distance < 1.f && ray_length * distance < highest_underneath) {
                            highest_underneath = ray_length * distance;
                        }
                    }
                    if (f_num >= REC2_ASIZE(faces)) {
                        break;
                    }
                }
            } else {
                list_ptr->d -= SHADOW_D_IGNORE_FLAG;
            }
        }
        highest_underneath -= (bounds.max.v[1] - bounds.min.v[1]);
        if (highest_underneath > 2.2f) {
            highest_underneath = 2.2f;
        } else if (highest_underneath < 0.f) {
            highest_underneath = 0.0;
        }
        if (C2V(gFancy_shadow)) {
            C2V(gShadow_dim_amount) = (int)(2.5f + (2.2f - highest_underneath) * 5.0f / 2.2f);
            for (i = 0; i < C2V(gSaved_table_count); i++) {
                C2V(gSaved_shade_tables)[i].original->height = 1;
                C2V(gSaved_shade_tables)[i].original->pixels = (tU8*)C2V(gDepth_shade_table)->pixels + C2V(gShadow_dim_amount) * C2V(gDepth_shade_table)->row_bytes;
                BrTableUpdate(C2V(gSaved_shade_tables)[i].original, BR_TABU_ALL);
            }
        }
        shadow_scaling_factor = 0.4f + (2.2f - highest_underneath) * 0.52f / 2.2f;
        for (i = 0; i < C2V(gShadow_clip_plane_count); i++) {
            clip_normal = C2V(gShadow_clip_planes)[i].clip->type_data;
            distance = DistanceFromPlane(&pCar->car_master_actor->t.t.translate.t, clip_normal->v[2], clip_normal->v[0], clip_normal->v[1], clip_normal->v[2], clip_normal->v[3]);
            BrVector3Set(&C2V(gShadow_clip_planes)[i].clip->t.t.translate.t,
                (1.f - shadow_scaling_factor) * distance * clip_normal->v[0],
                (1.f - shadow_scaling_factor) * distance * clip_normal->v[1],
                (1.f - shadow_scaling_factor) * distance * clip_normal->v[2]);
        }
        if (f_num != 0) {
            DisableLights();
            BrZbsSceneRenderBegin(C2V(gUniverse_actor), C2V(gCamera), C2V(gRender_screen), C2V(gDepth_buffer));
            EnableLights();
            BrSetScreenZOffset(2);
            C2V(gShadow_model)->vertices = verts;
            C2V(gShadow_model)->faces = faces;
            C2V(gShadow_model)->nfaces = f_num;
            C2V(gShadow_model)->nvertices = 3 * f_num;
            C2V(gShadow_actor)->render_style = BR_RSTYLE_FACES;
            BrModelAdd(C2V(gShadow_model));
            BrZbsSceneRenderAdd(C2V(gShadow_actor));
            BrModelRemove(C2V(gShadow_model));
            if (pCar->shadow_intersection_flags != 0) {
                oily_count = GetOilSpillCount();
                for (i = 0; i < oily_count; ++i) {
                    if (pCar->shadow_intersection_flags & (1 << i)) {
                        GetOilSpillDetails(i, &oily_actor, &oily_size);
                        if (oily_actor) {
                            MungeIndexedOilsHeightAboveGround(i);
                            BrZbSceneRenderAdd(oily_actor);
                        }
                    }
                }
            }
            BrSetScreenZOffset(0);
            BrZbSceneRenderEnd();

            for (i = 0; i < f_num; i++) {
                if (C2V(gFancy_shadow)) {
                    material = C2V(gShadow_model)->faces[i].material;
                    if (C2V(gShadow_workaround)) {
                        SmoothificateWorldMaterial(material);
                    } else {
                        if (material != NULL && material->colour_map != NULL && (material->flags & BR_MATF_LIGHT)) {
                            material->flags &= ~(BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH);
                            BrMaterialUpdate(material, BR_MATU_RENDERING);
                        }
                    }
                }
            }
        }
    }
    C2V(gShadow_actor)->render_style = BR_RSTYLE_NONE;
    for (i = 0; i < C2V(gShadow_clip_plane_count); i++) {
        BrClipPlaneDisable(C2V(gShadow_clip_planes)[i].clip);
    }
}
C2_HOOK_FUNCTION(0x004e7650, ProcessShadow)

void C2_HOOK_FASTCALL InitNearestCar(void) {

    C2V(gINT_0068d6f4) = -1;
    if (C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_net_game)->type == eNet_game_type_foxy) {
        C2V(gTarget_lock_car_2) = NULL;
    } else if (C2V(gPrevious_opponent_status) == eOpponent_status_Wasted && C2V(gTarget_lock_car_1) != NULL) {
        if (PDGetTotalTime() - C2V(gTime_oppobar_target_wasted) > 2500) {
            C2V(gTarget_lock_car_2) = NULL;
            C2V(gTarget_lock_car_1) = NULL;
            if (C2V(gTarget_lock_enabled)) {
                C2V(gTarget_lock_enabled) = 0;
            }
        } else {
            C2V(gFLOAT_0074ab90) = 0.f;
        }
    } else if (C2V(gTarget_lock_enabled)) {
        C2V(gFLOAT_0074ab90) = 0.f;
    } else {
        C2V(gTarget_lock_car_2) = NULL;
    }
}

void C2_HOOK_FASTCALL StartMap(void) {

    C2V(gBack_original_origin_x) = C2V(gBack_screen)->origin_x;
    C2V(gBack_original_origin_y) = C2V(gBack_screen)->origin_y;
    C2V(gBack_original_base_x) = C2V(gBack_screen)->base_x;
    C2V(gBack_original_base_y) = C2V(gBack_screen)->base_y;
    C2V(gBack_screen)->origin_x = 0;
    C2V(gBack_screen)->origin_y = 0;
    C2V(gBack_screen)->base_x = 0;
    C2V(gBack_screen)->base_y = 0;
}

void C2_HOOK_FASTCALL CopyMapToScreen(void) {

    if (C2V(gCurrent_race).map_image != NULL) {
        if (C2V(gGraf_data_index) == 0) {
            DRPixelmapCopy(C2V(gBack_screen), C2V(gCurrent_race).map_image);
        } else {
            PossibleUnlock(1);
            DRPixelmapRectangleCopy(C2V(gBack_screen),
                -C2V(gBack_screen)->origin_x,
                -C2V(gBack_screen)->origin_y,
                C2V(gCurrent_race).map_image,
                C2V(gCurrent_race).map_image->origin_x,
                C2V(gCurrent_race).map_image->origin_y,
                640, 480);
        }
    }
    DimRectangleClipped(C2V(gBack_screen),
        C2V(gMap_render_x_i) - C2V(gCurrent_graf_data)->map_render_x_marg,
        C2V(gMap_render_y_i) - C2V(gCurrent_graf_data)->map_render_y_marg,
        C2V(gMap_render_width_i) + C2V(gMap_render_x_i) + C2V(gCurrent_graf_data)->map_render_x_marg,
        C2V(gMap_render_height_i) + C2V(gMap_render_y_i) + C2V(gCurrent_graf_data)->map_render_y_marg,
        1);
}

void (C2_HOOK_FASTCALL * MapStuffBeforeRender_original)(void);
void C2_HOOK_FASTCALL MapStuffBeforeRender(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    MapStuffBeforeRender_original();
#else
    InitNearestCar();
    if (C2V(gCurrent_race).map_image != NULL && C2V(gMap_view) == 2) {
        StartMap();
        CopyMapToScreen();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00496be0, MapStuffBeforeRender, MapStuffBeforeRender_original)

void C2_HOOK_FASTCALL FoxyStuff(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth) {
    int count;
    int i;
    int j;
    tCar_spec* fox_car;

    count = 0;
    if (C2V(gNet_mode) != eNet_mode_none && (C2V(gCurrent_net_game)->type == eNet_game_type_6 || C2V(gCurrent_net_game)->type == eNet_game_type_foxy)) {
        BrZbsSceneRenderBegin(C2V(gUniverse_actor), pCamera, pColour, pDepth);
        for (i = 0; i < C2V(gNumber_of_net_players); i++) {
            tNet_game_player_info* net_player = &C2V(gNet_players)[i];
            if ((net_player->field_0x80 || i == C2V(gIt_or_fox)) && !IsCarCloaked(net_player->car)) {
                fox_car = net_player->car;

                if (fox_car->car_master_actor->render_style != BR_RSTYLE_NONE) {
                    br_actor **wheel_actors;

                    count += 1;
                    wheel_actors = fox_car->wheel_actors;
                    for (j = 0; j < REC2_ASIZE(fox_car->wheel_actors); j++) {
                        if (wheel_actors[j] != NULL) {
                            wheel_actors[j]->render_style = BR_RSTYLE_NONE;
                        }
                    }
                    BrZbsSceneRenderAdd(fox_car->car_master_actor);
                }
            }
        }
        BrZbsSceneRenderEnd();
        if (count != 0) {
            if (C2V(gHud_tinted4) == -1) {
                C2V(gHud_tinted4) = CreateTintedPoly(0, 0, 640, 480, 3, 2, 0, 0);
            }
            TurnTintedPolyOn(C2V(gHud_tinted4));
            ProcessTintedPoly(C2V(gHud_tinted4));
            RenderTintedPolys();
            TurnTintedPolyOff(C2V(gHud_tinted4));
            BrZbsSceneRenderBegin(C2V(gUniverse_actor), pCamera, pColour, pDepth);
            for (i = 0; i < C2V(gNumber_of_net_players); i++) {
                tNet_game_player_info* net_player = &C2V(gNet_players)[i];
                if ((net_player->field_0x80 || i == C2V(gIt_or_fox))) {
                    fox_car = net_player->car;

                    if (fox_car->car_master_actor->render_style != BR_RSTYLE_NONE) {
                        br_actor **wheel_actors;

                        wheel_actors = fox_car->wheel_actors;
                        for (j = 0; j < REC2_ASIZE(fox_car->wheel_actors); j++) {
                            if (wheel_actors[j] != NULL) {
                                wheel_actors[j]->render_style = BR_RSTYLE_DEFAULT;
                                BrZbsSceneRenderAdd(wheel_actors[j]);
                            }
                        }
                    }
                }
            }
            BrZbsSceneRenderEnd();
            fox_car->car_master_actor->render_style = BR_RSTYLE_NONE;
        }
    }
}

int C2_HOOK_FASTCALL ConditionallyFillWithSky(br_pixelmap* pPixelmap) {
    int bgnd_col;

    if (C2V(gProgram_state).current_depth_effect.sky_texture != NULL && ((C2V(gLast_camera_special_volume) == NULL) || C2V(gLast_camera_special_volume)->sky_col <= -1)) {
        return 0;
    }
    if (C2V(gProgram_state).current_depth_effect.type == eDepth_effect_fog || C2V(gSwap_depth_effect_type) == eDepth_effect_fog) {
        bgnd_col = 0xff;
    } else if (C2V(gProgram_state).current_depth_effect.type == eDepth_effect_darkness || C2V(gSwap_depth_effect_type) == eDepth_effect_darkness) {
        bgnd_col = 0x00;
    } else if (C2V(gLast_camera_special_volume) != NULL && C2V(gLast_camera_special_volume)->sky_col >= 0) {
        bgnd_col = C2V(gLast_camera_special_volume)->sky_col;
    } else {
        bgnd_col = 0x00;
    }

    if (C2V(gNet_mode) != eNet_mode_none && (C2V(gCurrent_net_game)->type == eNet_game_type_6 || C2V(gCurrent_net_game)->type == eNet_game_type_foxy)) {
        SetSkyColour(((tU32*)C2V(gRender_palette)->pixels)[bgnd_col]);
        return 0;
    } else {

        if (pPixelmap->type != BR_PMT_INDEX_8) {
            bgnd_col = PaletteEntry16Bit(C2V(gRender_palette), bgnd_col);
            bgnd_col = (bgnd_col << 16) | bgnd_col;
        }
        BrPixelmapFill(pPixelmap, bgnd_col);
        return 1;
    }
}

void C2_HOOK_FASTCALL FixificateClipulatingPlaneyThings(br_actor* pCamera) {
}

void C2_HOOK_FASTCALL ProcessNonTrackActors(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {

    BrZbSceneRenderAdd(C2V(gNon_track_actor));
}

void C2_HOOK_FASTCALL CancelificateClipulatingPlaneyThings(void) {

    // empty
}

void (C2_HOOK_FASTCALL * DoARenderPass_original)(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth, float pYon_factor, int pShadows, int pEffects);
void C2_HOOK_FASTCALL DoARenderPass(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth, float pYon_factor, int pShadows, int pEffects) {

#if 0//defined(C2_HOOKS_ENABLED)
    DoARenderPass_original)pMat34, pCamera, pColour, pDepth, pYon_factor, pShadows, pEffects);
#else
    br_camera* camera_data = (br_camera*)pCamera->type_data;
    br_scalar original_yon = camera_data->yon_z;
    int i;

    camera_data->yon_z *= pYon_factor;
    BrPixelmapFill(pDepth, 0xffffffff);
    if (!C2V(gAFE)) {
        FoxyStuff(pMat34, pCamera, pColour, pDepth);
    }
    if (!ConditionallyFillWithSky(pColour->width == C2V(gBack_screen)->width ? C2V(gBack_screen) : pColour)) {
        BrZbsSceneRenderBegin(C2V(gUniverse_actor), pCamera, pColour, pDepth);
        DepthEffectSky(pColour, pDepth, pCamera, pMat34);
        BrZbsSceneRenderEnd();
    }
    DoSpecialCameraEffect(pCamera, pMat34);
    if (pShadows) {
        RenderShadows(C2V(gUniverse_actor), &C2V(gProgram_state).track_spec, pCamera, pMat34);
    }
    FixificateClipulatingPlaneyThings(C2V(gCamera));
    BrZbsSceneRenderBegin(C2V(gUniverse_actor), pCamera, pColour, pDepth);
    for (i = REC2_ASIZE(C2V(gOther_selfs)); i > 0; i--) {
        if (i != 2) {
            BrSetScreenZOffset(i);
            BrZbsSceneRenderAdd(C2V(gOther_selfs)[i - 1]);
        }
    }
    BrSetScreenZOffset(0);
    ProcessTrack(C2V(gUniverse_actor), &C2V(gProgram_state).track_spec, pCamera, pMat34);
    ProcessNonTrackActors(pColour, pDepth, pCamera, pMat34);
    RenderLimbs();
    RenderLollipops(pColour, pDepth, pCamera, pMat34);
    /* FIXME: DepthEffect(pColour, pDepth) in software render mode? */
    if (pEffects) {
        RenderSparks(pColour, pDepth, pCamera, pMat34, C2V(gFrame_period));
        RenderSmoke(pColour, pDepth, pCamera, pMat34, C2V(gFrame_period));
        RenderElectroBastardRays(pColour, pDepth, pCamera, pMat34, C2V(gFrame_period));
    }
    BrZbsSceneRenderEnd();
    CancelificateClipulatingPlaneyThings();
    camera_data->yon_z = original_yon;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e5680, DoARenderPass, DoARenderPass_original)

void (C2_HOOK_FASTCALL * DoACompleteRenderPass_original)(int pMirror, br_matrix34* pCamera_to_world, br_actor* pCamera, br_pixelmap* pScreen, br_pixelmap* pDepth);
void C2_HOOK_FASTCALL DoACompleteRenderPass(int pMirror, br_matrix34* pCamera_to_world, br_actor* pCamera, br_pixelmap* pScreen, br_pixelmap* pDepth) {

#if defined(C2_HOOKS_ENABLED)
    DoACompleteRenderPass_original(pMirror, pCamera_to_world, pCamera, pScreen, pDepth);
#else
    int i;
    br_matrix34 mat;

    C2V(gRendering_mirror) = pMirror;
    if (pMirror && C2V(gPrevious_rear_yon) != ((br_camera*)pCamera->type_data)->yon_z) {
        C2V(gPrevious_rear_yon) = ((br_camera*)pCamera->type_data)->yon_z;
        MungeRearviewSky();
    }
    if (!C2V(gRendering_mirror)) {
        C2V(gRear_pixelmap)->base_x = 0;
        C2V(gRear_pixelmap)->base_y = 0;
        for (i = 0; i < C2V(gCount_extra_renders); i++) {
            tExtra_render *extra_render = &C2V(gExtra_renders)[i];

            C2V(gRear_pixelmap)->origin_x = extra_render->material->colour_map->width / 2;
            C2V(gRear_pixelmap)->origin_y = extra_render->material->colour_map->height / 2;
            C2V(gRear_pixelmap)->width = extra_render->material->colour_map->width;
            C2V(gRear_pixelmap)->height = extra_render->material->colour_map->width;
            if (C2V(gRear_pixelmap)->width == C2V(gRear_pixelmap)->row_bytes) {
                C2V(gRear_pixelmap)->flags |= BR_PMF_ROW_WHOLEPIXELS;
            } else {
                C2V(gRear_pixelmap)->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
            }
            BrActorToActorMatrix34(&mat, extra_render->actor, C2V(gUniverse_actor));
            DoARenderPass(&mat, extra_render->actor, C2V(gRear_pixelmap), C2V(gDepth_buffer), 1.f, 0, 0);
            DRPixelmapCopyMapBlack(extra_render->material->colour_map, C2V(gRear_pixelmap));
            BrMaterialUpdate(extra_render->material, BR_MATU_COLOURMAP);
        }
    }
    DoARenderPass(C2V(pCamera_to_world), pCamera, pScreen, pDepth, 1.f, !pMirror, !pMirror);
    C2V(gRendering_mirror) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e54f0, DoACompleteRenderPass, DoACompleteRenderPass_original)

void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDestn, int pX1, int pY1, int pX2, int pY2, int pColour) {

    if (C2V(gBack_screen)->type == BR_PMT_RGB_565) {
        pColour = PaletteEntry16Bit(C2V(gRender_palette), pColour);
    }
    BrPixelmapLine(pDestn, pX1, pY1, pX2, pY2, pColour);
}
C2_HOOK_FUNCTION(0x0047b450, DRDrawLine)

void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev) {
    int i;
    int checkpoint;
    int checkpoints[10];
    int checkpoint_flags;

    for (i = 0; i < C2V(gCurrent_race).check_point_count; i++) {
        checkpoints[i] = 0;
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        pCallback(pMap, C2V(gCheckpoint) - 1, pTime, 1);
        checkpoints[C2V(gCheckpoint) - 1] = 1;
    } else {
        if (C2V(gCurrent_net_game)->type == eNet_game_type_checkpoint) {
            checkpoint_flags = C2V(gNet_players)[C2V(gThis_net_player_index)].score2;
            for (i = 0; i < C2V(gCurrent_race).check_point_count; i++, checkpoint_flags >>= 1) {
                if (checkpoint_flags & 0x1) {
                    pCallback(pMap, i, pTime, 1);
                    checkpoints[i] = 1;
                }
            }
        } else if (C2V(gCurrent_net_game)->type == eNet_game_type_5 || C2V(gCurrent_net_game)->type == eNet_game_type_4) {
            if (C2V(gNet_players)[C2V(gThis_net_player_index)].score2 >= 0) {
                checkpoint = C2V(gNet_players)[C2V(gThis_net_player_index)].score2 % C2V(gCurrent_race).check_point_count;
                pCallback(pMap, checkpoint, pTime, 1);
                checkpoints[checkpoint] = 1;
            }
        }
    }
    if (pDraw_prev) {
        if (C2V(gNet_mode) == eNet_mode_none
                || C2V(gCurrent_net_game)->type == eNet_game_type_checkpoint
                || C2V(gCurrent_net_game)->type == eNet_game_type_4
                || C2V(gCurrent_net_game)->type == eNet_game_type_5) {

            for (i = 0; i < C2V(gCurrent_race).check_point_count; i++) {

                if (!checkpoints[i]) {
                    pCallback(pMap, i, pTime, 0);
                }
            }
        }
    }
}
C2_HOOK_FUNCTION(0x00495ba0, DoSomeThingsToCheckpoints)

void C2_HOOK_FASTCALL CalcMapCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    br_vector3 p;

    BrMatrix34ApplyP(&p, &C2V(gCurrent_race).checkpoints[pCheckpoint].pos, &C2V(gCurrent_race).map_transformation);
    BrVector2Copy(&C2V(gCurrent_race).checkpoints[pCheckpoint].map_position, &p);
}
C2_HOOK_FUNCTION(0x004968f0, CalcMapCheckpoint)

void C2_HOOK_FASTCALL DrawCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_flash, 0x0068d8e8);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, flash_rate, 0x0068d8ec);

    if ((pCheckpoint >= 0 && pCheckpoint < C2V(gCurrent_race).check_point_count
            && C2V(gTrack_version) >= 1 && pTarget == 0) || Flash(300, &C2V(last_flash), &C2V(flash_rate))) {

        if (C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0] >= 0.f
                && C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0] < (float)pMap->width
                && C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[1] >= 0.f
                && C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[1] < (float)pMap->height) {

            if (C2V(gMap_view) == 2) {
                PossibleLock(1);
            }
            DRPixelmapRectangleMaskedCopy(pMap,
                (int)(C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0] - (float)C2V(gCheckpoint_digit_center_x)),
                (int)(C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[1] - (float)C2V(gCheckpoint_digit_center_y)),
                C2V(gCheckpoint_numbers),
                0,
                pCheckpoint * C2V(gCheckpoint_digit_height),
                C2V(gCheckpoint_numbers)->width,
                C2V(gCheckpoint_digit_height));
            if (C2V(gMap_view) == 2) {
                PossibleUnlock(1);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x004969e0, DrawCheckpoint)

void C2_HOOK_FASTCALL CalcMapCheckpoint2(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    br_vector3 pos_mapspace;
    br_vector2 p;

    BrMatrix34ApplyP(&pos_mapspace, &C2V(gCurrent_race).checkpoints[pCheckpoint].pos, &C2V(gCurrent_race).map_transformation);
    BrVector2Sub(&p, &pos_mapspace, &C2V(gOrigin_map));
    BrVector2Add(&p, &p, &C2V(gOrigin_headup_map));
    BrMatrix23TApplyV(&C2V(gCurrent_race).checkpoints[pCheckpoint].map_position,
        &p, &C2V(gMatrix23_0068c880));
    C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0] += (float)C2V(gINT_0074abd4);
    C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[1] += (float)C2V(gINT_0074abd0);
}
C2_HOOK_FUNCTION(0x00496940, CalcMapCheckpoint2)

void C2_HOOK_FASTCALL DrawMapSmallBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3 *pPos, int pColour) {
    br_vector3 map_pos;

    if (pTime & 0x100) {
        pColour = 0;
    }
    BrMatrix34ApplyP(&map_pos, pPos, &C2V(gCurrent_race).map_transformation);
    if (map_pos.v[0] >= 0.f && map_pos.v[0] < (float)pScreen->width
            && map_pos.v[1] >= 0 && map_pos.v[1] < (float)pScreen->height) {

        if (C2V(gMap_view) == 2) {
            PossibleLock(1);
        }
        if (C2V(gBack_screen)->type == BR_PMT_INDEX_8) {
            ((br_uint_8*)pScreen->pixels)[(int)map_pos.v[1] * pScreen->row_bytes + (int)map_pos.v[0]] = pColour;
        } else {
            ((br_uint_16*)pScreen->pixels)[(int)map_pos.v[1] * pScreen->row_bytes + (int)map_pos.v[0]] = PaletteEntry16Bit(C2V(gRender_palette), pColour);
        }
        if (C2V(gMap_view) == 2) {
            PossibleUnlock(1);
        }
    }
}
C2_HOOK_FUNCTION(0x004967c0, DrawMapSmallBlip)

br_uint_32 C2_HOOK_FASTCALL OppositeColour(br_uint_32 pColour) {
    if (pColour < 0xe0) {
        if ((pColour & 7) < 4) {
            return 0xff;
        } else {
            return 0;
        }
    } else {
        if ((pColour & 0xf) < 8) {
            return 0xff;
        } else {
            return 0;
        }
    }
}

void C2_HOOK_FASTCALL DrawArrow(br_pixelmap* pScreen, br_uint_32 pArrow_index, const br_vector3 *pPos, br_uint_32 pColour) {
    int colours[2];
    int colour;
    int x;
    int y;
    int temp;
    int point_count;
    const int* arrow_ptr;
    int i;
    int j;

    if (pColour == 0xffffffff) {
        colours[0] = 0x0;
        colours[1] = 0x84;
    } else {
        colours[0] = pColour;
        colours[1] = OppositeColour(pColour);
    }

    if (C2V(gBack_screen)->type != BR_PMT_INDEX_8) {
        colours[0] = PaletteEntry16Bit(gRender_palette, colours[0]);
        colours[1] = PaletteEntry16Bit(gRender_palette, colours[1]);
        if (pColour == 0xffffffff) {
            colours[1] = Colour24BitTo16Bit(0xff00ff);
        }
    }

    for (i = 0; i < REC2_ASIZE(colours); i++) {
        colour = colours[i];
        point_count = C2V(gArrows)[i][pArrow_index & 0x3][0];
        arrow_ptr = &C2V(gArrows)[i][pArrow_index & 0x3][1];
        for (j = 0; j < point_count; j++, arrow_ptr += 2) {
            if (pArrow_index & 0x8) {
                x = -arrow_ptr[0];
                y = -arrow_ptr[1];
            } else {
                x = arrow_ptr[0];
                y = arrow_ptr[1];
            }
            if (pArrow_index & 0x4) {
                temp = x;
                x = -y;
                y = temp;
            }
            BrPixelmapPixelSet(pScreen,
                (int)(pPos->v[0] + x),
                (int)(pPos->v[1] + y),
                colour);
        }
    }
}
C2_HOOK_FUNCTION(0x00495a00, DrawArrow)

void C2_HOOK_FASTCALL DrawMapBlip(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, br_matrix34* pTrans, const br_vector3* pPos, br_uint_32 pColour) {
    tU32 time_diff;
    br_vector3 map_pos;
    float tv1;
    float tv2;
    int local_pos[2];
    int colour;
    int colours[2];
    float bearing;
    int arrow_index;
    int size;

    time_diff = pTime - C2V(gMap_time);
    BrMatrix34ApplyP(&map_pos, pPos, &C2V(gCurrent_race).map_transformation);
    map_pos.v[2] = 0.0;
    if (((pCar != NULL && pCar->driver == eDriver_oppo) || (pCar != NULL && pCar->driver == eDriver_net_human))
            && (!pCar->knackered || pCar == C2V(gTarget_lock_car_1))
            && (C2V(gCurrent_race).race_spec->race_type != kRaceType_Cars || pCar->is_race_goal)
            && (C2V(gNet_mode) == eNet_mode_none
                    || (C2V(gCurrent_net_game)->options.race_sequence_type == eNet_game_type_6 && NetPlayerFromCar(pCar)->field_0x80 == 0)
                    || (C2V(gCurrent_net_game)->options.race_sequence_type == eNet_game_type_foxy && pCar == C2V(gNet_players)[C2V(gIt_or_fox)].car)
                    || (C2V(gCurrent_net_game)->options.race_sequence_type != eNet_game_type_6 && C2V(gCurrent_net_game)->options.race_sequence_type != eNet_game_type_foxy))) {

        local_pos[0] = (int)((float)(C2V(gHeadup_map_half_width) + C2V(gINT_0068d890) + C2V(gUINT_0074ab8c)) - map_pos.v[0]);
        local_pos[1] = (int)((float)(C2V(gHeadup_map_half_height) + C2V(gINT_0068c858) + C2V(gUINT_0074ab88)) - map_pos.v[1]);
        tv1 = 0.f -map_pos.v[2];
        tv2 = (float)local_pos[1] * (float)local_pos[1] + (float)local_pos[0] * (float)local_pos[0] + tv1 * tv1;
        if (((C2V(gTarget_lock_car_2) == NULL
                || (!C2V(gTarget_lock_enabled) && tv2 < C2V(gFLOAT_0074ab90))
                || (C2V(gTarget_lock_enabled) && pCar == C2V(gTarget_lock_car_2))))
                        && !pCar->knackered) {

            C2V(gVector2_0068d6d8).v[0] = (float)local_pos[0];
            C2V(gVector2_0068d6d8).v[1] = (float)local_pos[1];
            C2V(gFLOAT_0068d6e0) = tv1;
            C2V(gTarget_lock_car_2) = pCar;
            C2V(gFLOAT_0074ab90) = tv2;
        }
    }
    if (C2V(gMini_map_visible) || C2V(gMap_view) == 2) {
        BrMatrix34Mul(&C2V(gCar_in_map_space), pTrans, &C2V(gCurrent_race).map_transformation);
        bearing = FastScalarArcTan2(C2V(gCar_in_map_space).m[2][0], C2V(gCar_in_map_space).m[2][1]);
        bearing = (360.f - bearing + 12.25f) / 22.5f;
        arrow_index = ((int)bearing) % 16;
        if (pCar == NULL || pCar->driver != eDriver_local_human || (pTime & 0x100)) {
            DrawArrow(pScreen, arrow_index, &map_pos, pColour);
        }
        colours[0] = pColour;
        colours[1] = OppositeColour(pColour);
        if (C2V(gBack_screen)->type != BR_PMT_INDEX_8) {
            colours[0] = PaletteEntry16Bit(C2V(gRender_palette), pColour);
            colours[1] = PaletteEntry16Bit(C2V(gRender_palette), colours[1]);
        }
        colour = colours[pTime & 0x100];
        if (C2V(gNet_mode) != eNet_mode_none
                && C2V(gCurrent_net_game)->type == eNet_game_type_foxy
                && pCar == C2V(gNet_players)[C2V(gIt_or_fox)].car) {

            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] - 8.f),
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] - 8.f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] + 8.f),
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] + 8.f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] - 8.f),
                (int)(map_pos.v[0] - 8.f),
                (int)(map_pos.v[1] + 8.f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] - 8.f),
                (int)(map_pos.v[0] + 8.f),
                (int)(map_pos.v[1] + 8.f),
                colour);
        }
        if (time_diff <= 500 && pCar != NULL && pCar->driver == eDriver_local_human) {

            size = (35000 - 70 * time_diff) / 500;

            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] - 0.5f) - size,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] - 0.5f) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] + 0.5f) + size,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] + 0.5f) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] - 0.5f) - size,
                (int)(map_pos.v[0] - 0.5f) - size,
                (int)(map_pos.v[1] + 0.5f) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] - 0.5f) - size,
                (int)(map_pos.v[0] + 0.5f) + size,
                (int)(map_pos.v[1] + 0.5f) + size,
                colour);
        }
    }
}
C2_HOOK_FUNCTION(0x00496270, DrawMapBlip)

void C2_HOOK_FASTCALL DRPixelmapBlendedLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, br_pixelmap* pPalette) {

    DRDrawLine(pMap, pX1, pY1, pX2, pY2, pColour);
}

void C2_HOOK_FASTCALL DR8BitFancyDrawLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period) {
    int anim_index;

    anim_index = (PDGetTotalTime() / pTime_period) % BR_ASIZE(C2V(gMini_map_glowing_line_animation_indices));
    DRPixelmapBlendedLine(pMap, pX1, pY1, pX2, pY2, pColour, C2V(gMini_map_glowing_line_palettes)[C2V(gMini_map_glowing_line_animation_indices)[anim_index]]);
    if (fabsf((float)(pY2 - pY1)) < fabsf((float)(pX2 - pX1))) {
        DRPixelmapBlendedLine(pMap, pX1, pY1 - 1, pX2, pY2 - 1, pColour, C2V(gMini_map_glowing_line_palettes)[C2V(gMini_map_glowing_line_animation_indices)[anim_index]]);
        DRPixelmapBlendedLine(pMap, pX1, pY1 + 1, pX2, pY2 + 1, pColour, C2V(gMini_map_glowing_line_palettes)[C2V(gMini_map_glowing_line_animation_indices)[anim_index]]);
    } else {
        DRPixelmapBlendedLine(pMap, pX1 - 1, pY1, pX2 - 1, pY2, pColour, C2V(gMini_map_glowing_line_palettes)[C2V(gMini_map_glowing_line_animation_indices)[anim_index]]);
        DRPixelmapBlendedLine(pMap, pX1 + 1, pY1, pX2 + 1, pY2, pColour, C2V(gMini_map_glowing_line_palettes)[C2V(gMini_map_glowing_line_animation_indices)[anim_index]]);
    }
}

void C2_HOOK_FASTCALL FancyDrawLine(br_pixelmap *pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period) {

    if (pMap->type == BR_PMT_INDEX_8) {
        DR8BitFancyDrawLine(pMap, pX1, pY1, pX2, pY2, pColour, pTime_period);
    } else {
        BrPixelmapLine(pMap, pX1, pY1, pX2, pY2, PaletteEntry16Bit(C2V(gRender_palette), pColour));
    }
}
C2_HOOK_FUNCTION(0x0047d2b0, FancyDrawLine)

void C2_HOOK_FASTCALL DR16BitPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height) {
    int width_div_2;
    int height_div_2;
    int dy;
    tU16 *ptr_write;
    float src_f_x;
    float src_f_y;
    int width;

    width_div_2 = pSrc_width / 2;
    height_div_2 = pSrc_height / 2;
    for (dy = -height_div_2; dy < height_div_2; dy++, pDest_y++) {
        src_f_x = (float)pSrc_x - (pMat->m[0][0] * (float)width_div_2 + pMat->m[0][1] * (float)dy);
        src_f_y = (float)pSrc_y + (pMat->m[1][0] * (float)width_div_2 + pMat->m[1][1] * (float)dy) + (float)pSrc_height / 2.f;
        if (-width_div_2 < width_div_2) {
            width = 2 * width_div_2;
            ptr_write = (tU16*)((tU8*)pDest->pixels + pDest_y * pDest->row_bytes) + pDest_x;
            for (; width != 0; width--) {
                *ptr_write++ = *((tU16*)((tU8*)pSrc->pixels + ((int)src_f_y * pSrc->row_bytes)) + width_div_2 + (int)src_f_x);
                src_f_x += pMat->m[0][0];
                src_f_y -= pMat->m[1][0];
            }
        }
    }
}
C2_HOOK_FUNCTION(0x0047d0d0, DR16BitPixelmapRotatedAndFeatheredCopy)

void C2_HOOK_FASTCALL DR8BitPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans) {
    int width_div_2;
    int width_near1;
    int width_near2;
    int width_near3;
    int height_div_2;
    int height_near1;
    int height_near2;
    int height_near3;
    float src_f_x;
    float src_f_y;
    int src_x;
    int src_y;
    int dy;
    int dx;
    int dx_bit0;
    int dy_bit1;
    tU8 src_pix;
    tU8* ptr_write;

    width_div_2 = pSrc_width / 2;
    width_near1 = width_div_2 - pSrc_width * 1 / 24;
    width_near2 = width_div_2 - pSrc_width * 2 / 24;
    width_near3 = width_div_2 - pSrc_width * 3 / 24;
    height_div_2 = pSrc_height / 2;
    height_near1 = height_div_2 - pSrc_height * 1 / 24;
    height_near2 = height_div_2 - pSrc_height * 2 / 12;
    height_near3 = height_div_2 - pSrc_height * 3 / 24;
    for (dy = -height_div_2; dy < height_div_2; dy++, pDest_y++) {
        src_f_x = (float)pSrc_x - (pMat->m[0][0] * (float)width_div_2 + pMat->m[0][1] * (float)dy);
        src_f_y = (float)pSrc_y + (pMat->m[1][0] * (float)width_div_2 + pMat->m[1][1] * (float)dy) + (float)pSrc_height / 2.f;
        ptr_write = (tU8*)pDest->pixels + pDest_y * pDest->row_bytes + pDest_x;
        for (dx = -width_div_2; dx < width_div_2; dx++) {
            src_y = (int)src_f_y;
            src_x = (int)src_f_x;
            src_f_x += pMat->m[0][0];
            src_f_y -= pMat->m[1][0];
            if (pTrans) {
                *ptr_write = ((tU8*)C2V(gPalette_0074a600)->pixels)[*ptr_write + ((tU8*)pSrc->pixels)[src_y * pSrc->row_bytes + width_div_2 + src_x] * 256];
            } else {
                src_pix = ((tU8*)pSrc->pixels)[src_y * pSrc->row_bytes + width_div_2 + src_x];
                dx_bit0 = ((src_pix >> 0) & 0x1) - ((*ptr_write >> 0) & 0x1) + dx;
                dy_bit1 = ((src_pix >> 1) & 0x1) - ((*ptr_write >> 1) & 0x1) + dy;

                if (-width_div_2 <= dx_bit0 && dx_bit0 <= width_div_2
                        && -height_div_2 <= dy_bit1 && dy_bit1 <= height_div_2) {
                    if (!(-width_near1 <= dx_bit0 && dx_bit0 <= width_near1
                            && -height_near1 <= dy_bit1 && dy_bit1 <= height_near1)) {
                        *ptr_write = ((tU8*)C2V(gPalette_0074a604)->pixels)[*ptr_write + 256 * src_pix];
                    } else if (!(-width_near2 <= dx_bit0 && dx_bit0 <= width_near2
                            && -height_near2 <= dy_bit1 && dy_bit1 <= height_near2)) {
                        *ptr_write = ((tU8*)C2V(gPalette_0074a600)->pixels)[*ptr_write + 256 * src_pix];
                    } else if (!(-width_near3 <= dx_bit0 && dx_bit0 <= width_near3
                            && -height_near3 <= dy_bit1 && dy_bit1 <= height_near3)) {
                        *ptr_write = ((tU8*)C2V(gPalette_0074a5fc)->pixels)[*ptr_write + 256 * src_pix];
                    } else {
                        *ptr_write = src_pix;
                    }
                }
            }
        }
    }
}

void (C2_HOOK_FASTCALL * DRPixelmapRotatedAndFeatheredCopy_original)(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans);
void C2_HOOK_FASTCALL DRPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans) {

#if 0//defined(C2_HOOKS_ENABLED)
    DRPixelmapRotatedAndFeatheredCopy_original(pMat, pDest, pDest_x, pDest_y, pSrc, pSrc_x, pSrc_y, pSrc_width, pSrc_height, pTrans) {
#else
    if (pDest->type == BR_PMT_INDEX_8) {
        DR8BitPixelmapRotatedAndFeatheredCopy(pMat, pDest, pDest_x, pDest_y, pSrc, pSrc_x, pSrc_y, pSrc_width, pSrc_height, pTrans);
    } else {
        DR16BitPixelmapRotatedAndFeatheredCopy(pMat, pDest, pDest_x, pDest_y, pSrc, pSrc_x, pSrc_y, pSrc_width, pSrc_height);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0047cd40, DRPixelmapRotatedAndFeatheredCopy, DRPixelmapRotatedAndFeatheredCopy_original)

void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack8Bit(br_pixelmap* pDest, br_pixelmap* pSrc) {
    tU16 dest_row_bytes = pDest->row_bytes;
    tU16 src_row_bytes = pSrc->row_bytes;
    tU8 *dest_pixels = pDest->pixels;
    tU16 dest_width = pDest->width;
    tU16 src_width = pSrc->width;
    tU8 *src_pixels = pSrc->pixels;
    tU32 src_width_dwords = src_width / sizeof(tU32);
    tU32 dwords_remaining;
    int y;

    for (y = 0; y < (int)pSrc->height; y++) {

        for (dwords_remaining = src_width_dwords; dwords_remaining != 0; dwords_remaining--, src_pixels+=4, dest_pixels+=4) {
            tU8 p0 = src_pixels[0];
            tU8 p1 = src_pixels[1];
            tU8 p2 = src_pixels[2];
            tU8 p3 = src_pixels[3];
            dest_pixels[0] = p0 != 0 ? p0 : 0xf0;
            dest_pixels[1] = p1 != 0 ? p1 : 0xf0;
            dest_pixels[2] = p2 != 0 ? p2 : 0xf0;
            dest_pixels[3] = p3 != 0 ? p3 : 0xf0;
        }
        src_pixels += src_row_bytes - src_width;
        dest_pixels += dest_row_bytes - dest_width;
    }
}

void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack(br_pixelmap* pDest, br_pixelmap* pSrc) {
    if (pDest->type == BR_PMT_INDEX_8) {
        DRPixelmapCopyMapBlack8Bit(pDest, pSrc);
    } else {
        BrPixelmapCopy(pDest, pSrc);
    }
}
C2_HOOK_FUNCTION(0x0047d5b0, DRPixelmapCopyMapBlack)

void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {
    int i;
    int cat;
    int car_count;
    tCar_spec* the_car;
    br_vector3 camera_to_car;
    br_scalar distance_factor;

    if (C2V(gShadow_level) == eShadow_none) {
        return;
    }
    for (cat = eVehicle_self;; ++cat) {
        if (C2V(gShadow_level) == eShadow_everyone) {
            if (cat >= eVehicle_not_really) {
                break;
            }
        } else {
            if (cat > (C2V(gShadow_level) == eShadow_us_and_opponents ? eVehicle_rozzer : eVehicle_self)) {
                break;
            }
        }

        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                the_car = &C2V(gProgram_state).current_car;
            } else {
                the_car = GetCarSpec(cat, i);
            }
            if (!the_car->active) {
                continue;
            }
            if (IsCarCloaked(the_car)) {
                continue;
            }
            if (C2V(gAction_replay_mode)) {
                continue;
            }

            BrVector3Sub(&camera_to_car, (br_vector3*)C2V(gCamera_to_world).m[3], &the_car->car_master_actor->t.t.translate.t);
            distance_factor = BrVector3LengthSquared(&camera_to_car);
            if (distance_factor <= SHADOW_MAX_RENDER_DISTANCE) {
                ProcessShadow(the_car, C2V(gUniverse_actor), &C2V(gProgram_state).track_spec, C2V(gCamera), &C2V(gCamera_to_world), distance_factor);
            }
        }
    }
    if (C2V(gFancy_shadow)) {
        for (i = 0; i < C2V(gSaved_table_count); i++) {
            C2V(gSaved_shade_tables)[i].original->height = C2V(gSaved_shade_tables)[i].copy->height;
            C2V(gSaved_shade_tables)[i].original->pixels = C2V(gSaved_shade_tables)[i].copy->pixels;
            BrTableUpdate(C2V(gSaved_shade_tables)[i].original, BR_TABU_ALL);
        }
    }
}
C2_HOOK_FUNCTION(0x004e74d0, RenderShadows)
