/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "15-displays.h"

#include "08-loading1.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0067c500
// GLOBAL: CARMA2_SW 0x00676c70
tHeadup gHeadups[37];

// GLOBAL: CARMA2_HW 0x0067fcd8
// GLOBAL: CARMA2_SW 0x0067a438
br_font* gBR_fonts[4];

// GLOBAL: CARMA2_HW 0x0067fd00
// GLOBAL: CARMA2_SW 0x0067a450
br_pixelmap* gRevs[13];

// GLOBAL: CARMA2_HW 0x0074cf84
br_pixelmap* gArmour_colour_map;

// GLOBAL: CARMA2_HW 0x0067fcd0
br_pixelmap* gPowerbar;

// GLOBAL: CARMA2_HW 0x0074cab4
br_pixelmap* gPower_colour_map;

// GLOBAL: CARMA2_HW 0x0074cf5c
br_pixelmap* gOffensive_colour_map;

// GLOBAL: CARMA2_HW 0x006815bc
br_pixelmap* gIcon_test;

// GLOBAL: CARMA2_HW 0x0074ca1c
br_pixelmap* gCurrent_rev;

// GLOBAL: CARMA2_HW 0x0067c4f8
br_pixelmap* gHud_curve1;

// GLOBAL: CARMA2_HW 0x0067fcfc
br_pixelmap* gHud_curve2;

// GLOBAL: CARMA2_HW 0x0067f87c
br_pixelmap* gHud_curve3;

// GLOBAL: CARMA2_HW 0x0067fcf4
br_pixelmap* gGrey_top5;

// GLOBAL: CARMA2_HW 0x0074cf00
br_pixelmap* gDamage_hud;

// GLOBAL: CARMA2_HW 0x0074ca80
br_pixelmap* gTimer_lhs;

// GLOBAL: CARMA2_HW 0x0074ca44
br_pixelmap* gHud_timer_rhs;

// GLOBAL: CARMA2_HW 0x006815b8
// GLOBAL: CARMA2_SW 0x0067bcfc
br_pixelmap* gIcon_armour;

// GLOBAL: CARMA2_HW 0x006815b4
// GLOBAL: CARMA2_SW 0x0067bcf8
br_pixelmap* gIcon_power;

// GLOBAL: CARMA2_HW 0x0067c4f0
// GLOBAL: CARMA2_SW 0x00676c68
br_pixelmap* gIcon_offense;

// GLOBAL: CARMA2_HW 0x0067c4f4
// GLOBAL: CARMA2_SW 0x00676c6c
br_pixelmap* gIcon_greyBloc1;

// GLOBAL: CARMA2_HW 0x0067fce8
// GLOBAL: CARMA2_SW 0x0067a448
br_pixelmap* gIcon_litBloc1;

// GLOBAL: CARMA2_HW 0x0067fcd4
// GLOBAL: CARMA2_SW 0x0067a430
br_pixelmap* gIcon_grnBlock1;

// GLOBAL: CARMA2_SW 0x00679ff0
br_pixelmap* gIcon_greyTop;

// GLOBAL: CARMA2_HW 0x0067fd40
// GLOBAL: CARMA2_SW 0x0067a488
tHud_message gHud_messages[46];

// GLOBAL: CARMA2_HW 0x00655e54
// GLOBAL: CARMA2_SW 0x00650d3c
int gHeadup_detail_level = 5;

// GLOBAL: CARMA2_HW 0x0067fcc0
// GLOBAL: CARMA2_SW 0x0067a428
tU32 gLast_fancy_headup;

// GLOBAL: CARMA2_HW 0x005913d4
// GLOBAL: CARMA2_SW 0x0058c3b4
int gLast_fancy_index = -1;

// GLOBAL: CARMA2_HW 0x0067fd34
// GLOBAL: CARMA2_SW 0x0067a484
tU32 gLast_fancy_time;

// GLOBAL: CARMA2_HW 0x0067fcf8
// GLOBAL: CARMA2_SW 0x0067a44c
tU32 gLast_centre_headup;

// GLOBAL: CARMA2_HW 0x005913d8
// GLOBAL: CARMA2_SW 0x0058c3b8
int gLast_credit_headup__displays = -1;

// GLOBAL: CARMA2_HW 0x0067f874
// GLOBAL: CARMA2_SW 0x00679fe4
tU32 gLast_earn_time;

// GLOBAL: CARMA2_HW 0x005913dc
// GLOBAL: CARMA2_SW 0x0058c3bc
int gLast_time_credit_headup = -1;

// GLOBAL: CARMA2_HW 0x0067fcc4
// GLOBAL: CARMA2_SW 0x0067a42c
tU32 gLast_time_earn_time;

// GLOBAL: CARMA2_HW 0x0067f880
// GLOBAL: CARMA2_SW 0x00679fec
tU32 gLast_time_credit_amount;

// GLOBAL: CARMA2_HW 0x0079eac0
// GLOBAL: CARMA2_SW 0x00795020
int gOld_times[10];

// GLOBAL: CARMA2_HW 0x0067f888
// GLOBAL: CARMA2_SW 0x00679ff4
int gQueued_headup_count;


static void C2_HOOK_FASTCALL LoadHeadupMessageFile(void) {
    FILE* f;
    tPath_name path;
    int i;
    int count;

    PathCat(path, gApplication_path, "SCROLLERS.TXT");
    f = DRfopen(path, "rt");
    if (f == NULL) {
        BrFailure("Couldn't open ", path);
    }
    /*
     * # of Headup scrolly messages
     * Format:  Font index, Message string
     * N.B use underscore "_" where you want a space in the string
     **/
    count = GetAnInt(f);

    C2_HOOK_BUG_ON(sizeof(tHud_message) != 0x88);
    C2_HOOK_BUG_ON(REC2_ASIZE(gHud_messages) != 46);

    if (count > (int)REC2_ASIZE(gHud_messages)) {
        BrFailure("Error - Too many Headup messages ", path);
    }
    for (i = 0; i < count; i++) {
        char s[256];
        char *str;
        tHud_message *hud_message;
        int len;
        int j;

        hud_message = &gHud_messages[i];
        GetALineAndDontArgue(f, s);

        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &hud_message->font1);

        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &hud_message->font2);

        str = strtok(NULL, "\t ,/");
        strcpy(hud_message->message, str);

        len = strlen(hud_message->message);
        for (j = 0; j < len; j++) {
            if (hud_message->message[j] == '_') {
                hud_message->message[j] = ' ';
            }
        }
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00449090
// FUNCTION: CARMA2_SW 0x004494b0
void C2_HOOK_FASTCALL InitHeadups(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gHeadups) != 37);
    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++) {
        gHeadups[i].type = eHeadup_unused;
    }

    gBR_fonts[0] = BrFontProp4x6;
    gBR_fonts[1] = BrFontProp7x9;
    gBR_fonts[2] = gFont_7;
    gBR_fonts[3] = gHeadup_font;
#ifdef REC2_SW
    gGraf_spec_index = 0;
#endif
    gRevs[ 0] = LoadPixelmap("rev1.tif");
    gRevs[ 1] = LoadPixelmap("rev2.tif");
    gRevs[ 2] = LoadPixelmap("rev3.tif");
    gRevs[ 3] = LoadPixelmap("rev4.tif");
    gRevs[ 4] = LoadPixelmap("rev5.tif");
    gRevs[ 5] = LoadPixelmap("rev6.tif");
    gRevs[ 6] = LoadPixelmap("rev7.tif");
    gRevs[ 7] = LoadPixelmap("rev8.tif");
    gRevs[ 8] = LoadPixelmap("rev9.tif");
    gRevs[ 9] = LoadPixelmap("rev10.tif");
    gRevs[10] = LoadPixelmap("rev11.tif");
    gRevs[11] = LoadPixelmap("rev12.tif");
    gRevs[12] = LoadPixelmap("rev13.tif");

#ifdef REC2_HW
    gPowerbar = LoadPixelmap("powerbar.tif");

    gArmour_colour_map = BrPixelmapAllocate(BR_PMT_RGBA_4444,
        gPowerbar->width, gPowerbar->height,
        NULL, 0);
    gArmour_colour_map->identifier = BrResStrDup(gArmour_colour_map, "Armour_area");
    BrMapAdd(gArmour_colour_map);

    gPower_colour_map = LoadPixelmap("powerbar.tif");
    BrMapAdd(gPower_colour_map);

    gOffensive_colour_map = LoadPixelmap("powerbar.tif");
    BrMapAdd(gOffensive_colour_map);

    gIcon_test = LoadPixelmap("icontest.tif");

    gCurrent_rev = gRevs[0];
    BrMapAdd(gRevs[0]);

    gHud_curve1 = LoadPixelmap("hudcurve1.tif");
    BrMapAdd(gHud_curve1);
    gHud_curve2 = LoadPixelmap("hudcurve2.tif");
    BrMapAdd(gHud_curve2);
    gHud_curve3 = LoadPixelmap("hudcurve3.tif");
    BrMapAdd(gHud_curve3);

    gGrey_top5 = LoadPixelmap("greytop5.tif");
    gDamage_hud = BrPixelmapAllocate(BR_PMT_RGBA_4444,
        gGrey_top5->width, gGrey_top5->height,
        NULL, 0);
    BrMapAdd(gDamage_hud);
    BrMapUpdate(gDamage_hud, BR_MAPU_ALL);

    gStatbarHUD5_material->colour_map = gDamage_hud;
    BrMaterialUpdate(gStatbarHUD5_material, BR_MATU_COLOURMAP);

    gTimer_lhs = LoadPixelmap("timerlhs.tif");
    BrMapAdd(gTimer_lhs);
    gTimerLeftHUD_material->colour_map = gTimer_lhs;
    BrMaterialUpdate(gTimerLeftHUD_material, BR_MATU_COLOURMAP);
    gTimerLeftHUD_model->vertices[1].p.v[0] = (float)gTimerLeftHUD_dim_x;
    gTimerLeftHUD_model->vertices[0].p.v[0] = gTimerLeftHUD_model->vertices[1].p.v[0];
    gTimerLeftHUD_model->vertices[3].p.v[1] = -(float)gTimerLeftHUD_dim_y;
    gTimerLeftHUD_model->vertices[0].p.v[1] = gTimerLeftHUD_model->vertices[3].p.v[1];
    gTimerLeftHUD_model->vertices[3].p.v[0] = (float)gTimerLeftHUD_dim_w + gTimerLeftHUD_model->vertices[1].p.v[0];
    gTimerLeftHUD_model->vertices[2].p.v[0] = gTimerLeftHUD_model->vertices[3].p.v[0];
    gTimerLeftHUD_model->vertices[2].p.v[1] = gTimerLeftHUD_model->vertices[3].p.v[1] - (float)gTimerLeftHUD_dim_h;
    gTimerLeftHUD_model->vertices[1].p.v[1] = gTimerLeftHUD_model->vertices[2].p.v[1];
    BrModelUpdate(gTimerLeftHUD_model, BR_MODU_VERTEX_POSITIONS);

    gHud_timer_rhs = LoadPixelmap("timerrhs.tif");
    BrMapAdd(gHud_timer_rhs);
    gTimerRightHUD_material->colour_map = gHud_timer_rhs;
    BrMaterialUpdate(gTimerRightHUD_material, BR_MATU_COLOURMAP);

    gTimerRightHUD_model->vertices[1].p.v[0] = (float)gTimerRightHUD_dim_x;
    gTimerRightHUD_model->vertices[0].p.v[0] = gTimerRightHUD_model->vertices[1].p.v[0];
    gTimerRightHUD_model->vertices[3].p.v[1] = -(float)gTimerRightHUD_dim_y;
    gTimerRightHUD_model->vertices[0].p.v[1] = gTimerRightHUD_model->vertices[3].p.v[1];
    gTimerRightHUD_model->vertices[3].p.v[0] = (float)gTimerRightHUD_dim_w + gTimerRightHUD_model->vertices[1].p.v[0];
    gTimerRightHUD_model->vertices[2].p.v[0] = gTimerRightHUD_model->vertices[3].p.v[0];
    gTimerRightHUD_model->vertices[2].p.v[1] = gTimerRightHUD_model->vertices[3].p.v[1] - (float)gTimerRightHUD_dim_h;
    gTimerRightHUD_model->vertices[1].p.v[1] = gTimerRightHUD_model->vertices[2].p.v[1];
    BrModelUpdate(gTimerRightHUD_model, BR_MODU_VERTEX_POSITIONS);
#endif

    gIcon_armour = LoadPixelmap("iconarmour.tif");
    gIcon_power = LoadPixelmap("iconpower.tif");
    gIcon_offense = LoadPixelmap("iconoffense.tif");
    gIcon_greyBloc1 = LoadPixelmap("greybloc1.tif");
    gIcon_litBloc1 = LoadPixelmap("litbloc1.tif");
    gIcon_grnBlock1 = LoadPixelmap("grnblock1.tif");
#ifdef REC2_SW
    gIcon_greyTop = LoadPixelmap("greytop.tif");
    gGraf_spec_index = 1;
#endif
    LoadHeadupMessageFile();
}

// FUNCTION: CARMA2_HW 0x00449630
// FUNCTION: CARMA2_SW 0x00449780
void C2_HOOK_FASTCALL ClearHeadup(int pIndex) {
    gHeadups[pIndex].type = eHeadup_unused;
}

// FUNCTION: CARMA2_HW 0x00449650
// FUNCTION: CARMA2_SW 0x004497a0
void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index) {
    int i;
    tHeadup* the_headup;

    C2_HOOK_BUG_ON(REC2_ASIZE(gHeadups) != 37);

    the_headup = gHeadups;
    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++, the_headup++) {
        if (the_headup->type != eHeadup_unused && the_headup->slot_index == pSlot_index) {
            ClearHeadup(i);
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00449690
// FUNCTION: CARMA2_SW 0x004497e0
void C2_HOOK_FASTCALL ClearHeadups(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gHeadups) != 37);
    C2_HOOK_BUG_ON(REC2_ASIZE(gOld_times) != 10);

    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++) {
        if (gHeadups[i].type != eHeadup_unused) {
            ClearHeadup(i);
        }
    }
    gLast_fancy_index = -1;
    gLast_credit_headup__displays = -1;
    gLast_time_credit_headup = -1;
    gLast_earn_time = 0;
    for (i = 0; i < (int)REC2_ASIZE(gOld_times); i++) {
        gOld_times[i] = 0;
    }
    gLast_fancy_time = 0;
    gLast_time_earn_time = 0;
    gQueued_headup_count = 0;
    gLast_centre_headup = 0;
}

// FUNCTION: CARMA2_HW 0x004496f0
// FUNCTION: CARMA2_SW 0x00449840
void C2_HOOK_FASTCALL DimAFewBits(void) {
    int i;

#define DIM_INDEX (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0)
    for (i = 0; i < gProgram_state.current_car.dim_count[DIM_INDEX]; i++) {
        DimRectangle(
            gBack_screen,
            gProgram_state.current_car.dim_left[DIM_INDEX][i],
            gProgram_state.current_car.dim_top[DIM_INDEX][i],
            gProgram_state.current_car.dim_right[DIM_INDEX][i],
            gProgram_state.current_car.dim_bottom[DIM_INDEX][i],
            1);
    }
#undef DIM_INDEX
}

// KillOldestQueuedHeadup

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00449830
// STUB: CARMA2_SW 0x00449980
void C2_HOOK_FASTCALL DoTestPowerHeadup(int pLevel, const char* pAPO_str) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00449800
// FUNCTION: CARMA2_SW 0x00449950
void C2_HOOK_FASTCALL DoPSPowerupHeadups(void) {
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[0], "A");
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[1], "P");
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[2], "O");
}

// DoHeadups

// FUNCTION: CARMA2_HW 0x00449fa0
// FUNCTION: CARMA2_SW 0x0044a130
int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index) {
    int i;
    int empty_one;
    tHeadup* the_headup;

    the_headup = gHeadups;
    empty_one = -1;
    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++, the_headup++) {
        if (pSlot_index >= 0 && the_headup->slot_index == pSlot_index) {
            return i;
        } else if (the_headup->type == eHeadup_unused) {
            empty_one = i;
        }
    }
    return empty_one;
}

// IsHeadupTextClever

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0044a220
// STUB: CARMA2_SW 0x0044a3b0
int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00449fd0
// STUB: CARMA2_SW 0x0044a160
int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0044a380
// FUNCTION: CARMA2_SW 0x0044a510
int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText) {
    return NewTextHeadupSlot2(pSlot_index, pFlash_rate, pLifetime, pFont_index, pText, 1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0044a3a0
// STUB: CARMA2_SW 0x0044a530
int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0044a600
// FUNCTION: CARMA2_SW 0x0044a790
void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex) {
    tU32 the_time;
    int temp_ref;
    int center;
    int width;
    tHeadup* the_headup;

    center = gCurrent_graf_data->width / 2;
    the_time = GetTotalTime();
    if (gMap_view == 2) {
        return;
    }
    if (gLast_fancy_index >= 0 && the_time - gLast_fancy_time <= 2000 && gLast_fancy_index > pIndex) {
        return;
    }
    temp_ref = NewImageHeadupSlot(6, 0, 2000, pIndex + 10);
    if (temp_ref >= 0) {
        gLast_fancy_index = pIndex;
        gLast_fancy_headup = temp_ref;
        gLast_fancy_time = the_time;

        the_headup = &gHeadups[temp_ref];
        the_headup->type = eHeadup_fancy;
        the_headup->data.fancy_info.fancy_stage = 0;
        the_headup->data.fancy_info.field_0x108 = 0;
        switch (the_headup->justification) {
        case eJust_left:
            the_headup->data.fancy_info.offset = center;
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.end_offset = -width;
            break;
        case eJust_right:
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.end_offset = center;
            the_headup->data.fancy_info.offset = center + width;
            break;
        case eJust_centre:
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.offset = center + width / 2;
            width = DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            the_headup->data.fancy_info.end_offset = -center - width / 2;
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044a8a0
void C2_HOOK_FASTCALL MoveHeadupTo(int pHeadup_index, int pNew_x, int pNew_y) {
    tHeadup* the_headup;

    if (pHeadup_index >= 0) {
        the_headup = &gHeadups[pHeadup_index];
        the_headup->justification = eJust_left;
        the_headup->original_x = pNew_x;
        the_headup->x = pNew_x;
        the_headup->y = pNew_y;
    }
}

// FUNCTION: CARMA2_HW 0x0044a8d0
// FUNCTION: CARMA2_SW 0x0044aa60
void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text) {
    tHeadup* the_headup;

    if (pHeadup_index < 0) {
        return;
    }
    the_headup = &gHeadups[pHeadup_index];
    strcpy(the_headup->data.text_info.text, pNew_text);
    MungeHeadupWidth(the_headup);
}

// FUNCTION: CARMA2_HW 0x0044a920
// FUNCTION: CARMA2_SW 0x0044aab0
void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont) {

    if (pHeadup_index >= 0) {
        gHeadups[pHeadup_index].data.image_info.font = &gFonts[-pFont];
    }
}

// FUNCTION: CARMA2_HW 0x0044a9d0
// FUNCTION: CARMA2_SW 0x0044ab60
void C2_HOOK_FASTCALL DoDamageScreen(tU32 pThe_time) {
    int i;
    int y_pitch;
    int the_step;
    int the_wobble_x;
    int the_wobble_y;
    br_pixelmap* the_image;
    tDamage_unit* the_damage;
#ifdef REC2_SW
    int shift_x;
    int shift_y;
#endif

    if (&gProgram_state.current_car != gCar_to_view) {
        return;
    }
#ifdef REC2_SW
    if (gHeadup_detail_level == 0 || gHeadup_detail_level == 3) {
        return;
    }
    shift_x = 0;
    shift_y = 0;
#endif

    if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0) {
        if (gProgram_state.which_view != eView_forward) {
            return;
        }
        the_wobble_x = gScreen_wobble_x;
        the_wobble_y = gScreen_wobble_y;
    } else {
        the_wobble_x = gProgram_state.current_car.damage_x_offset;
        the_wobble_y = gProgram_state.current_car.damage_y_offset;
#ifdef REC2_SW
        if (gNet_mode != eNet_mode_none) {
            shift_x = 56 - gProgram_state.current_car.damage_background_x;
            shift_y = -gProgram_state.current_car.damage_background_y;
        }
        DRPixelmapBlend75Copy(gBack_screen,
            (br_int_16)(gProgram_state.current_car.damage_background_x + shift_x),
            (br_int_16)(gProgram_state.current_car.damage_background_y + shift_y),
            gProgram_state.current_car.damage_background);
#endif
    }

#ifdef REC2_HW
    DRPixelmapCopy(gDamage_hud, gGrey_top5);
#endif
    for (i = 0; i < (int)REC2_ASIZE(gProgram_state.current_car.damage_units); ++i) {
        if (i != eDamage_driver) {
            the_damage = &gProgram_state.current_car.damage_units[i];
            the_image = the_damage->images;
            the_step = 5 * the_damage->damage_level / 100;
            y_pitch = (the_image->height / 2) / 5;
            DRPixelmapRectangleMaskedCopy(
#ifdef REC2_SW
                gBack_screen,
                (br_int_16)(the_wobble_x + shift_x + the_damage->x_coord),
                (br_int_16)(the_wobble_y + shift_y + the_damage->y_coord),
#else
                gDamage_hud,
                (br_int_16)(the_wobble_x + the_damage->x_coord),
                (br_int_16)(the_wobble_y + the_damage->y_coord),
#endif
                the_image,
                0,
                (br_int_16)(y_pitch * (2 * the_step + ((pThe_time / the_damage->periods[the_step]) & 1))),
                (br_int_16)the_image->width,
                (br_int_16)y_pitch);
        }
    }
}

// DoInstruments

// ChangingView

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0044b300
// STUB: CARMA2_SW 0x0044b490
void C2_HOOK_FASTCALL EarnCredits2(int pAmount, const char* pPrefix_text) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0044b440
// FUNCTION: CARMA2_SW 0x0044b5d0
void C2_HOOK_FASTCALL EarnCredits(int pAmount) {
    EarnCredits2(pAmount, "");
}

// EarnCredits100

// FUNCTION: CARMA2_HW 0x0044b470
// FUNCTION: CARMA2_SW 0x0044b600
int C2_HOOK_FASTCALL SpendCredits(int pAmount) {
    int amount;

    amount = gProgram_state.credits - pAmount;
    gProgram_state.credits = amount;
    if (amount < 0) {
        gProgram_state.credits = 0;
        return amount;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044b490
// FUNCTION: CARMA2_SW 0x0044b620
void C2_HOOK_FASTCALL AwardTime(tU32 pTime) {
    char s[256];
    tU32 original_amount;
    tU32 the_time;
    int i;

    if (gRace_finished || gFreeze_timer || gNet_mode != eNet_mode_none || gCurrent_race.race_spec->no_time_awards) {
        return;
    }
    the_time = GetTotalTime();
    if (pTime != 0) {
        for (i = REC2_ASIZE(gOld_times) - 1; i > 0; i--) {
            gOld_times[i] = gOld_times[i - 1];
        }
        gOld_times[0] = pTime;
        original_amount = pTime;
        if (gLast_time_credit_headup >= 0 && the_time - gLast_time_earn_time < 2000) {
            pTime += gLast_time_credit_amount;
        }
        gLast_time_credit_amount = pTime;
        gTimer += original_amount * 1000;
        s[0] = '+';
        TimerString(1000 * pTime, &s[1], 0, 0, 0);
        gLast_time_credit_headup = NewTextHeadupSlot2(11, 0, 2000, -2, s, 1);
        gLast_time_earn_time = the_time;
    }
}

// FUNCTION: CARMA2_HW 0x0044b590
// FUNCTION: CARMA2_SW 0x0044b720
void C2_HOOK_FASTCALL AwardTime5(tU32 pTime) {
    // FIXME: AwardTime should be inlined in matched build
#ifdef REC2_MATCHING
    char s[256];
    tU32 original_amount;
    tU32 the_time;
    int i;
    tU32 rounded_time;

    rounded_time = pTime - (pTime + 2) % 5 + 2;
    if (gRace_finished || gFreeze_timer || gNet_mode != eNet_mode_none || gCurrent_race.race_spec->no_time_awards) {
        return;
    }
    the_time = GetTotalTime();
    if (rounded_time != 0) {
        for (i = REC2_ASIZE(gOld_times) - 1; i > 0; i--) {
            gOld_times[i] = gOld_times[i - 1];
        }
        gOld_times[0] = rounded_time;
        original_amount = rounded_time;
        if (gLast_time_credit_headup >= 0 && the_time - gLast_time_earn_time < 2000) {
            rounded_time += gLast_time_credit_amount;
        }
        gLast_time_credit_amount = rounded_time;
        gTimer += original_amount * 1000;
        s[0] = '+';
        TimerString(1000 * rounded_time, &s[1], 0, 0, 0);
        gLast_time_credit_headup = NewTextHeadupSlot2(11, 0, 2000, -2, s, 1);
        gLast_time_earn_time = the_time;
    }
#else
    AwardTime(pTime - (pTime + 2) % 5 + 2);
#endif
}

// DoTestHeadup

// FUNCTION: CARMA2_HW 0x0044ba60
int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth) {
    int width;

    if (gDevious_2d && gTexture_power_of_2) {
        width = 1;
        while (width < pWidth) {
            width <<= 1;
        }
        return width;
    }
    return pWidth;
}

// FUNCTION: CARMA2_HW 0x0044ba90
int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight) {
    int height;

    if (gDevious_2d && gTexture_power_of_2) {
        height = 1;
        while (height < pHeight) {
            height <<= 1;
        }
        return height;
    }
    return pHeight;
}

// FUNCTION: CARMA2_HW 0x0044bac0
// FUNCTION: CARMA2_SW 0x0044b8a0
void C2_HOOK_FASTCALL DrawThisCarIconNow(int pCar_index, int pX, int pY) {
#ifdef REC2_HW
    gCar_icons_model_actor->material->colour_map = gTexture_maps[gCar_icons[pCar_index].index];
    BrMaterialUpdate(gCar_icons_model_actor->material, BR_MATU_COLOURMAP);
    gCar_icons_model_actor->model = gCar_icons[pCar_index].model;
    BrMatrix34Translate(&gCar_icons_model_actor->t.t.mat, (br_scalar)pX, (br_scalar)-pY, 0.0f);
    BrActorAdd(g2d_camera, gCar_icons_model_actor);
    BrZbsSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
    BrActorRemove(gCar_icons_model_actor);
#endif
#ifdef REC2_SW
    DRPixelmapRectangleMaskedCopy(gBack_screen,
        pX,
        pY,
        gIcons_pix,
        0,
        pCar_index * gCurrent_graf_data->car_icon_height,
        gIcons_pix->width,
        gCurrent_graf_data->car_icon_height);
#endif
}

