#include "15-displays.h"

#include "08-loading1.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrkm.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x005913d4
int gLast_fancy_index = -1;

// GLOBAL: CARMA2_HW 0x005913d8
int gLast_credit_headup__displays = -1;

// GLOBAL: CARMA2_HW 0x005913dc
int gLast_time_credit_headup = -1;

// GLOBAL: CARMA2_HW 0x005913e0
int gPS_apo_level_changed[3] = { 1, 1, 1 };

// GLOBAL: CARMA2_HW 0x005913ec
int gPrev_ps_apo_levels[3] = { -1, -1, -1 };

// GLOBAL: CARMA2_HW 0x005913f8
int gPrev_ps_drawn_levels[3] = { -1, -1, -1 };

// GLOBAL: CARMA2_HW 0x0067c500
tHeadup gHeadups[37];

// GLOBAL: CARMA2_HW 0x0067f874
tU32 gLast_earn_time;

// GLOBAL: CARMA2_HW 0x0067f878
int gLast_credit_amount;

// GLOBAL: CARMA2_HW 0x0067f888
int gQueued_headup_count;

// GLOBAL: CARMA2_HW 0x0067f890
tQueued_headup gQueued_headups[4];

// GLOBAL: CARMA2_HW 0x0067fcc0
tU32 gLast_fancy_headup;

// GLOBAL: CARMA2_HW 0x0067fcc4
tU32 gLast_time_earn_time;

// GLOBAL: CARMA2_HW 0x0067fcd8
br_font* gBR_fonts[4];

// GLOBAL: CARMA2_HW 0x0067fcf8
tU32 gLast_centre_headup;

// GLOBAL: CARMA2_HW 0x0067fd00
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
br_pixelmap* gIcon_armour;

// GLOBAL: CARMA2_HW 0x006815b4
br_pixelmap* gIcon_power;

// GLOBAL: CARMA2_HW 0x0067c4f0
br_pixelmap* gIcon_offense;

// GLOBAL: CARMA2_HW 0x0067c4f4
br_pixelmap* gIcon_greyBloc1;

// GLOBAL: CARMA2_HW 0x0067fce8
br_pixelmap* gIcon_litBloc1;

// GLOBAL: CARMA2_HW 0x0067fcd4
br_pixelmap* gIcon_grnBlock1;

// GLOBAL: CARMA2_HW 0x0067fd34
tU32 gLast_fancy_time;

// GLOBAL: CARMA2_HW 0x0067fd40
tHud_message gHud_messages[46];

// GLOBAL: CARMA2_HW 0x00705be0
int gCredit_multiplier;

// GLOBAL: CARMA2_HW 0x0079eac0
int gOld_times[10];

// GLOBAL: CARMA2_HW 0x00655e54
int gHeadup_detail_level = 5;

// FUNCTION: CARMA2_HW 0x00449090
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

    gIcon_armour = LoadPixelmap("iconarmour.tif");
    gIcon_power = LoadPixelmap("iconpower.tif");
    gIcon_offense = LoadPixelmap("iconoffense.tif");
    gIcon_greyBloc1 = LoadPixelmap("greybloc1.tif");
    gIcon_litBloc1 = LoadPixelmap("litbloc1.tif");
    gIcon_grnBlock1 = LoadPixelmap("grnblock1.tif");

    {
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
}

// FUNCTION: CARMA2_HW 0x00449630
void C2_HOOK_FASTCALL ClearHeadup(int pIndex) {
    gHeadups[pIndex].type = eHeadup_unused;
}

// FUNCTION: CARMA2_HW 0x00449650
void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index) {
    int i;
    tHeadup* the_headup;

    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++) {
        the_headup = &gHeadups[i];
        if (the_headup->type != eHeadup_unused && the_headup->slot_index == pSlot_index) {
            ClearHeadup(i);
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00449690
void C2_HOOK_FASTCALL ClearHeadups(void) {
    int i;

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
void C2_HOOK_FASTCALL DimAFewBits(void) {
    int i;
    int dim_index;

    for (i = 0; ; ++i) {
        dim_index = gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0;
        if (i >= gProgram_state.current_car.dim_count[dim_index]) {
            break;
        }
        DimRectangle(
            gBack_screen,
            gProgram_state.current_car.dim_left[dim_index][i],
            gProgram_state.current_car.dim_top[dim_index][i],
            gProgram_state.current_car.dim_right[dim_index][i],
            gProgram_state.current_car.dim_bottom[dim_index][i],
            1);
    }
}

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void) {
    if (gQueued_headup_count != 0) {
        gQueued_headup_count--;
        memmove(&gQueued_headups[0], &gQueued_headups[1], gQueued_headup_count * sizeof(tQueued_headup));
    }
}

// FUNCTION: CARMA2_HW 0x004497b0
void C2_HOOK_FASTCALL ClearQueuedHeadups(void) {
    while (gQueued_headup_count != 0) {
        KillOldestQueuedHeadup();
    }
}

// FUNCTION: CARMA2_HW 0x00449800
void C2_HOOK_FASTCALL DoPSPowerupHeadups(void) {
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[0], "A");
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[1], "P");
    DoTestPowerHeadup(gProgram_state.current_car.power_up_levels[2], "O");
}

// FUNCTION: CARMA2_HW 0x00449830
void C2_HOOK_FASTCALL DoTestPowerHeadup(int pLevel, const char* pAPO_str) {
    int changed;
    br_pixelmap* dest_pm;
    br_pixelmap* icon;
    br_pixelmap* block;
    int count_slots;
    int level;
    int i;
    int j;
    int delta_x = 2;

    switch (pAPO_str[0]) {
    case 'A':
        dest_pm = gArmour_colour_map;
        icon = gIcon_armour;
        count_slots = gProgram_state.current_car.power_up_slots[0];
        changed = pLevel != gPrev_ps_apo_levels[0] || count_slots != gPrev_ps_drawn_levels[0];
        gPS_apo_level_changed[0] = changed;
        gPrev_ps_apo_levels[0] = pLevel;
        gPrev_ps_drawn_levels[0] = count_slots;
        break;
    case 'P':
        dest_pm = gPower_colour_map;
        icon = gIcon_power;
        count_slots = gProgram_state.current_car.power_up_slots[1];
        changed = pLevel != gPrev_ps_apo_levels[1] || count_slots != gPrev_ps_drawn_levels[1];
        gPS_apo_level_changed[1] = changed;
        gPrev_ps_apo_levels[1] = pLevel;
        gPrev_ps_drawn_levels[1] = count_slots;
        break;
    case 'O':
        dest_pm = gOffensive_colour_map;
        icon = gIcon_offense;
        count_slots = gProgram_state.current_car.power_up_slots[2];
        /* Offense redraws use the cached power slot count. */
        changed = pLevel != gPrev_ps_apo_levels[2] || count_slots != gPrev_ps_drawn_levels[1];
        gPS_apo_level_changed[2] = changed;
        gPrev_ps_drawn_levels[2] = count_slots;
        gPrev_ps_apo_levels[2] = pLevel;
        break;
    default:
        return;
    }

    if (changed) {
        DRPixelmapCopy(dest_pm, gPowerbar);
        DRPixelmapRectangleMaskedCopy(dest_pm, (br_int_16)delta_x, 0, icon, 0, 0,
            (br_int_16)icon->width, (br_int_16)icon->height);
        count_slots -= pLevel;
        level = pLevel;
        for (i = 0; i < 15; i += 5) {
            for (j = 0; j < 10; ++j) {
                if (level > 0) {
                    block = gIcon_litBloc1;
                } else if (count_slots > 0) {
                    block = gIcon_grnBlock1;
                } else {
                    block = gIcon_greyBloc1;
                }
                DRPixelmapRectangleMaskedCopy(
                    dest_pm,
                    (br_int_16)(j * block->width + delta_x + icon->width + 2),
                    (br_int_16)i,
                    block, 0, 0, (br_int_16)block->width, (br_int_16)block->height);
                if (level > 0) {
                    --level;
                } else if (count_slots > 0) {
                    --count_slots;
                }
            }
            delta_x -= 2;
        }

        switch (pAPO_str[0]) {
        case 'A':
            BrMapUpdate(gArmour_colour_map, BR_MAPU_ALL);
            gArmour_material->colour_map = gArmour_colour_map;
            BrMaterialUpdate(gArmour_material, BR_MATU_COLOURMAP);
            break;
        case 'P':
            BrMapUpdate(gPower_colour_map, BR_MAPU_ALL);
            gPower_material->colour_map = gPower_colour_map;
            BrMaterialUpdate(gPower_material, BR_MATU_COLOURMAP);
            break;
        case 'O':
            BrMapUpdate(gOffensive_colour_map, BR_MAPU_ALL);
            gOffence_material->colour_map = gOffensive_colour_map;
            BrMaterialUpdate(gOffence_material, BR_MATU_COLOURMAP);
            break;
        }
    }
    gArmour_actor->render_style = BR_RSTYLE_FACES;
    gPower_actor->render_style = BR_RSTYLE_FACES;
    gOffense_actor->render_style = BR_RSTYLE_FACES;
}

// DoHeadups

// FUNCTION: CARMA2_HW 0x00449fa0
int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index) {
    int i;
    int empty_one;
    tHeadup* the_headup;

    empty_one = -1;
    for (i = 0; i < (int)REC2_ASIZE(gHeadups); i++) {
        the_headup = &gHeadups[i];
        if (pSlot_index >= 0 && the_headup->slot_index == pSlot_index) {
            return i;
        }
        if (the_headup->type == eHeadup_unused) {
            empty_one = i;
        }
    }
    return empty_one;
}

int C2_HOOK_FASTCALL IsHeadupTextClever(char* pText) {
    while (*pText != '\0') {
        if ((tS8)*pText < 0) {
            return 1;
        }
        pText++;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0044a220
int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup) {
    int width;

    width = 0;
    if (pHeadup->type == eHeadup_box_text) {
        return 0;
    } else if (pHeadup->type == eHeadup_coloured_text) {
        pHeadup->clever = IsHeadupTextClever(pHeadup->data.text_info.text);
        if (pHeadup->justification == eJust_left) {
            pHeadup->x = pHeadup->original_x;
        } else if (pHeadup->justification == eJust_right) {
            if (pHeadup->clever) {
                width = DRTextCleverWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
            } else {
                width = DRTextWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
            }
            pHeadup->x = pHeadup->original_x - width;
        } else if (pHeadup->justification == eJust_centre) {
            if (pHeadup->clever) {
                width = DRTextCleverWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
            } else {
                width = DRTextWidth(pHeadup->data.coloured_text_info.coloured_font, pHeadup->data.text_info.text);
            }
            pHeadup->x = pHeadup->original_x - width / 2;
        }
    } else if (pHeadup->type == eHeadup_fancy) {
        return 0;
    } else {
        pHeadup->clever = 0;
        if (pHeadup->justification == eJust_left) {
            pHeadup->x = pHeadup->original_x;
        } else if (pHeadup->justification == eJust_right) {
            width = BrPixelmapTextWidth(gBack_screen, pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width;
        } else if (pHeadup->justification == eJust_centre) {
            width = BrPixelmapTextWidth(gBack_screen, pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width / 2;
        }
    }
    return width;
}

// FUNCTION: CARMA2_HW 0x00449fd0
int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it) {
    int index;
    int width;
    tHeadup* the_headup;
    tHeadup_slot* headup_slot;
    tQueued_headup* queued_headup;
    tU32 time;

    time = PDGetTotalTime();
    if (pQueue_it && pSlot_index == 4 && time - gLast_centre_headup < 1000) {
        if (gQueued_headup_count == 4) {
            KillOldestQueuedHeadup();
        }
        index = gQueued_headup_count;
        queued_headup = &gQueued_headups[index];
        queued_headup->flash_rate = pFlash_rate;
        queued_headup->lifetime = pLifetime;
        queued_headup->font_index = pFont_index;
        strcpy(queued_headup->text, pText);
        gQueued_headup_count = index + 1;
        return -1;
    }
    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index < 0) {
        return index;
    }
    if (pSlot_index == 4) {
        gLast_centre_headup = time;
    }
    headup_slot = &gProgram_state.current_car.headup_slots[gProgram_state.cockpit_on][pSlot_index];
    the_headup = &gHeadups[index];
    the_headup->data.coloured_text_info.coloured_font = &gFonts[-pFont_index];
    if (pSlot_index == 4) {
        the_headup->type = eHeadup_box_text;
    } else {
        the_headup->type = eHeadup_coloured_text;
    }
    strcpy(the_headup->data.coloured_text_info.text, pText);
    the_headup->slot_index = pSlot_index;
    the_headup->justification = headup_slot->justification;
    if (pSlot_index < 0) {
        the_headup->cockpit_anchored = 0;
    } else {
        the_headup->cockpit_anchored = headup_slot->cockpit_anchored;
    }
    the_headup->field_0x3c = headup_slot->field_0x28;
    the_headup->dimmed_background = headup_slot->dimmed_background;
    the_headup->dim_left = headup_slot->dim_left;
    the_headup->dim_top = headup_slot->dim_top;
    the_headup->dim_right = headup_slot->dim_right;
    the_headup->dim_bottom = headup_slot->dim_bottom;
    the_headup->original_x = headup_slot->x;
    width = MungeHeadupWidth(the_headup);
    the_headup->right_edge = width + the_headup->x;
    the_headup->y = headup_slot->y;
    if (pFlash_rate) {
        the_headup->flash_period = 1000 / pFlash_rate;
    } else {
        the_headup->flash_period = 0;
    }
    the_headup->last_flash = 0;
    the_headup->flash_state = 0;
    if (pLifetime) {
        the_headup->end_time = GetTotalTime() + pLifetime;
    } else {
        the_headup->end_time = 0;
    }
    return index;
}

// FUNCTION: CARMA2_HW 0x0044a380
int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText) {
    return NewTextHeadupSlot2(pSlot_index, pFlash_rate, pLifetime, pFont_index, pText, 1);
}

// FUNCTION: CARMA2_HW 0x0044a3a0
int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index) {
    int index;
    int width;
    tHeadup* the_headup;
    tHeadup_slot* headup_slot;

    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index >= 0) {
        headup_slot = &gProgram_state.current_car.headup_slots[gProgram_state.cockpit_on][pSlot_index];
        the_headup = &gHeadups[index];
        the_headup->type = eHeadup_image;
        the_headup->slot_index = pSlot_index;
        the_headup->justification = headup_slot->justification;
        if (pSlot_index >= 0) {
            the_headup->cockpit_anchored = headup_slot->cockpit_anchored;
        } else {
            the_headup->cockpit_anchored = 0;
        }
        the_headup->dimmed_background = headup_slot->dimmed_background;
        the_headup->dim_left = headup_slot->dim_left;
        the_headup->dim_top = headup_slot->dim_top;
        the_headup->dim_right = headup_slot->dim_right;
        the_headup->dim_bottom = headup_slot->dim_bottom;
        the_headup->original_x = headup_slot->x;

        switch (headup_slot->justification) {
        case eJust_left:
            the_headup->x = the_headup->original_x;
            break;
        case eJust_right:
            width = DRTextWidth(&gFonts[gHud_messages[pImage_index].font1], gHud_messages[pImage_index].message);
            the_headup->x = the_headup->original_x + width;
            break;
        case eJust_centre:
            width = DRTextWidth(&gFonts[gHud_messages[pImage_index].font1], gHud_messages[pImage_index].message);
            the_headup->x = the_headup->original_x + width / 2;
            break;
        }
        the_headup->y = headup_slot->y;
        if (pFlash_rate != 0) {
            the_headup->flash_period = 1000 / pFlash_rate;
        } else {
            the_headup->flash_period = 0;
        }
        the_headup->last_flash = 0;
        the_headup->flash_state = 0;
        if (pLifetime != 0) {
            the_headup->end_time = GetTotalTime() + pLifetime;
        } else {
            the_headup->end_time = 0;
        }
        strcpy(the_headup->data.image_info.text, gHud_messages[pImage_index].message);
        the_headup->data.image_info.font_index = gHud_messages[pImage_index].font2;
        the_headup->data.image_info.font = &gFonts[gHud_messages[pImage_index].font1];
        switch (the_headup->justification) {
        case eJust_left:
            the_headup->data.image_info.field_0x104 = 0;
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x10c = -width;
            break;
        case eJust_right:
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x104 = width;
            the_headup->data.image_info.field_0x10c = 0;
            break;
        case eJust_centre:
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x104 = width / 2;
            width = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x10c = -(width / 2);
            break;
        }
    }
    return index;
}

// FUNCTION: CARMA2_HW 0x0044a600
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
    if (gLast_fancy_index >= 0 && the_time - gLast_fancy_time <= 2000 && pIndex < gLast_fancy_index) {
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

// FUNCTION: CARMA2_HW 0x0044a860
void C2_HOOK_FASTCALL MoveHeadupTo(int pHeadup_index, int pX, int pY) {
    int delta_x;

    if (pHeadup_index >= 0) {
        delta_x = gHeadups[pHeadup_index].x - gHeadups[pHeadup_index].original_x;
        gHeadups[pHeadup_index].original_x = pX;
        gHeadups[pHeadup_index].x = delta_x + pX;
        gHeadups[pHeadup_index].y = pY;
    }
}

// FUNCTION: CARMA2_HW 0x0044a8d0
void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text) {
    tHeadup* the_headup;

    if (pHeadup_index >= 0) {
        the_headup = &gHeadups[pHeadup_index];
        strcpy(the_headup->data.text_info.text, pNew_text);
        MungeHeadupWidth(the_headup);
    }
}

// FUNCTION: CARMA2_HW 0x0044a9d0
void C2_HOOK_FASTCALL DoDamageScreen(tU32 pThe_time) {
    int i;
    int y_pitch;
    int the_step;
    int the_wobble_x;
    int the_wobble_y;
    tU32 phase;
    br_pixelmap* the_image;
    tDamage_unit* the_damage;

    if (&gProgram_state.current_car != gCar_to_view) {
        return;
    }
    if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0) {
        if (gProgram_state.which_view != eView_forward) {
            return;
        }
        the_wobble_x = gScreen_wobble_x;
        the_wobble_y = gScreen_wobble_y;
    } else {
        the_wobble_x = gProgram_state.current_car.damage_x_offset;
        the_wobble_y = gProgram_state.current_car.damage_y_offset;
    }

    DRPixelmapCopy(gDamage_hud, gGrey_top5);
    for (i = 0; i < (int)REC2_ASIZE(gProgram_state.current_car.damage_units); ++i) {
        the_damage = &gProgram_state.current_car.damage_units[i];
        if (i != eDamage_driver) {
            the_image = the_damage->images;
            the_step = 5 * the_damage->damage_level / 100;
            y_pitch = (the_image->height / 2) / 5;
            phase = (pThe_time / (tU32)the_damage->periods[the_step]) & 1;
            DRPixelmapRectangleMaskedCopy(
                gDamage_hud,
                (br_int_16)(the_wobble_x + the_damage->x_coord),
                (br_int_16)(the_wobble_y + the_damage->y_coord),
                the_image,
                0,
                (br_int_16)(y_pitch * (2 * the_step + phase)),
                (br_int_16)the_image->width,
                (br_int_16)y_pitch);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044aad0
void C2_HOOK_FASTCALL DoInstruments(void) {
    char buffer[8];
    br_pixelmap* speedo_image;
    int cockpit;
    int the_wobble_x;
    int the_wobble_y;
    int rev;
    int rev_divisor;
    int gear;
    int gear_height;
    double the_angle;
    double the_angle2;
    double sin_angle;
    double cos_angle;
    double speed_mph;

    if (gCar_to_view == NULL || (int)gCar_to_view->driver < eDriver_oppo) {
        return;
    }
    speed_mph = (double)gCar_to_view->speedo_speed * WORLD_SCALE * 2250.0;
    if (!(speed_mph >= 0.0)) {
        speed_mph = 0.0;
    }
    if (gHeadup_detail_level == 0 || gHeadup_detail_level == 3) {
        if (!gAction_replay_mode) {
            sprintf(buffer, "%03i", (int)speed_mph);
            PolyFontText(buffer, 1, 1, kPolyfont_ingame_medium_green, eJust_left, 0);
        }
        return;
    }

    cockpit = gProgram_state.cockpit_on;
    if (cockpit && gProgram_state.cockpit_image_index >= 0) {
        if (gProgram_state.which_view != eView_forward) {
            return;
        }
        the_wobble_x = gScreen_wobble_x;
        the_wobble_y = gScreen_wobble_y;
    } else {
        the_wobble_x = 0;
        the_wobble_y = 0;
    }

    if (gProgram_state.current_car.tacho_radius_2[cockpit] < 0) {
        speedo_image = gRevs[div((int)gCar_to_view->revs, 11).quot];
        BrPixelmapRectangleCopy(
            gCurrent_rev,
            gProgram_state.current_car.tacho_x[cockpit] + the_wobble_x,
            gProgram_state.current_car.tacho_y[cockpit] + the_wobble_y,
            speedo_image, 0, 0, speedo_image->width, speedo_image->height);
    } else {
        rev_divisor = div(gCar_to_view->red_line, 11).quot;
        rev = div((int)gCar_to_view->revs, rev_divisor).quot;
        if (rev < 0) {
            rev = 0;
        } else if (rev > 11) {
            rev = 11;
        }
        gCurrent_rev = gRevs[rev];
    }

    cockpit = gProgram_state.cockpit_on;
    if (!cockpit || gProgram_state.cockpit_image_index < 0 || gProgram_state.which_view == eView_forward) {
        gear = gCar_to_view->gear;
        if (gear < 0) {
            gear = -1;
        }
        speedo_image = gProgram_state.current_car.gears_image;
        gear_height = speedo_image->height / 8;
        DRPixelmapRectangleMaskedCopy(
            gCurrent_rev,
            (br_int_16)(gProgram_state.current_car.gear_x[cockpit] + the_wobble_x),
            (br_int_16)(gProgram_state.current_car.gear_y[cockpit] + the_wobble_y),
            speedo_image, 0, (br_int_16)((gear + 1) * gear_height),
            (br_int_16)speedo_image->width, (br_int_16)gear_height);
        cockpit = gProgram_state.cockpit_on;
    }

    speedo_image = gProgram_state.current_car.speedo_image[cockpit];
    if (gProgram_state.current_car.speedo_radius_2[cockpit] < 0) {
        if (speedo_image != NULL) {
            DrawNumberAt(
                speedo_image, gCurrent_rev,
                gProgram_state.current_car.speedo_x[cockpit] + the_wobble_x,
                gProgram_state.current_car.speedo_y[cockpit] + the_wobble_y,
                gProgram_state.current_car.speedo_x_pitch[cockpit],
                gProgram_state.current_car.speedo_y_pitch[cockpit],
                (int)speed_mph, 3, 1);
        }
        return;
    }

    if (speedo_image != NULL && (!cockpit || gProgram_state.cockpit_image_index < 0)) {
        DRPixelmapRectangleMaskedCopy(
            gCurrent_rev,
            (br_int_16)(gProgram_state.current_car.speedo_x[cockpit] + the_wobble_x),
            (br_int_16)(gProgram_state.current_car.speedo_y[cockpit] + the_wobble_y),
            speedo_image, 0, 0,
            (br_int_16)speedo_image->width, (br_int_16)speedo_image->height);
        cockpit = gProgram_state.cockpit_on;
    }
    if (!(speed_mph > gProgram_state.current_car.max_speed)) {
        the_angle = gProgram_state.current_car.speedo_start_angle[cockpit]
            + (gProgram_state.current_car.speedo_end_angle[cockpit]
                - gProgram_state.current_car.speedo_start_angle[cockpit]) * speed_mph
                / gProgram_state.current_car.max_speed;
    } else {
        the_angle = gProgram_state.current_car.speedo_end_angle[cockpit];
    }
    the_angle *= REC2_PI / 180.0;
    if (!(the_angle >= 0.0)) {
        the_angle += REC2_TAU;
    } else if (!(the_angle < REC2_TAU)) {
        the_angle -= REC2_TAU;
    }
    the_angle2 = REC2_PI_OVER_2 - the_angle;
    if (!(the_angle2 >= 0.0)) {
        the_angle2 += REC2_TAU;
    }

    if (the_angle2 > REC2_3PI_OVER_2) {
        cos_angle = gCosine_array[(int)((REC2_TAU - the_angle2) * (128.0 / REC2_PI))];
    } else if (the_angle2 > REC2_PI) {
        cos_angle = -gCosine_array[(int)((the_angle2 - REC2_PI) * (128.0 / REC2_PI))];
    } else if (the_angle2 > REC2_PI_OVER_2) {
        cos_angle = -gCosine_array[(int)((REC2_PI - the_angle2) * (128.0 / REC2_PI))];
    } else {
        cos_angle = gCosine_array[(int)(the_angle2 * (128.0 / REC2_PI))];
    }
    if (the_angle > REC2_3PI_OVER_2) {
        sin_angle = gCosine_array[(int)((REC2_TAU - the_angle) * (128.0 / REC2_PI))];
    } else if (the_angle > REC2_PI) {
        sin_angle = -gCosine_array[(int)((the_angle - REC2_PI) * (128.0 / REC2_PI))];
    } else if (the_angle > REC2_PI_OVER_2) {
        sin_angle = -gCosine_array[(int)((REC2_PI - the_angle) * (128.0 / REC2_PI))];
    } else {
        sin_angle = gCosine_array[(int)(the_angle * (128.0 / REC2_PI))];
    }

    PoshDrawLine(
        gBack_screen,
        (int)(gProgram_state.current_car.speedo_radius_1[cockpit] * sin_angle
            + the_wobble_x + gProgram_state.current_car.speedo_centre_x[cockpit]),
        (br_scalar)the_angle,
        (int)(gProgram_state.current_car.speedo_centre_y[cockpit]
            - gProgram_state.current_car.speedo_radius_1[cockpit] * cos_angle + the_wobble_y),
        (int)(gProgram_state.current_car.speedo_radius_2[cockpit] * sin_angle
            + the_wobble_x + gProgram_state.current_car.speedo_centre_x[cockpit]),
        (int)(gProgram_state.current_car.speedo_centre_y[cockpit] + (double)the_wobble_y
            - gProgram_state.current_car.speedo_radius_2[cockpit] * cos_angle),
        gProgram_state.current_car.speedo_needle_colour[cockpit]);
}

// ChangingView

// FUNCTION: CARMA2_HW 0x0044b300
void C2_HOOK_FASTCALL EarnCredits2(int pAmount, const char* pPrefix_text) {
    char s[256];
    int original_amount;
    tU32 the_time;
    const char* unit_text;
    const char* lost_text;

    if (gRace_finished) {
        return;
    }
    the_time = GetTotalTime();
    if (pAmount == 0) {
        return;
    }
    if (gCredit_multiplier != 0) {
        pAmount *= gCredit_multiplier;
    }
    original_amount = pAmount;
    if (gLast_credit_headup__displays >= 0 && the_time - gLast_earn_time < 2000) {
        pAmount += gLast_credit_amount;
    }
    gLast_credit_amount = pAmount;
    if (pAmount >= 2) {
        unit_text = GetMiscString(eMiscString_credits);
        sprintf(s, "%s%d %s", pPrefix_text, pAmount, unit_text);
    } else if (pAmount == 1) {
        unit_text = GetMiscString(eMiscString_credit);
        sprintf(s, "%s1 %s", pPrefix_text, unit_text);
    } else if (pAmount >= -1) {
        unit_text = GetMiscString(eMiscString_credit);
        lost_text = GetMiscString(eMiscString_lost_credits_prefix);
        sprintf(s, "%s%s 1 %s", pPrefix_text, lost_text, unit_text);
    } else {
        unit_text = GetMiscString(eMiscString_credits);
        lost_text = GetMiscString(eMiscString_lost_credits_prefix);
        sprintf(s, "%s%s %d %s", lost_text, pPrefix_text, -pAmount, unit_text);
    }
    gProgram_state.credits += original_amount;
    gLast_credit_headup__displays = NewTextHeadupSlot(4, 0, 2000, -4, s);
    gLast_earn_time = the_time;
}

// FUNCTION: CARMA2_HW 0x0044b440
void C2_HOOK_FASTCALL EarnCredits(int pAmount) {
    EarnCredits2(pAmount, "");
}

// FUNCTION: CARMA2_HW 0x0044b450
void C2_HOOK_FASTCALL EarnCredits100(int pAmount) {
    EarnCredits((pAmount + 50) / 100 * 100);
}

// FUNCTION: CARMA2_HW 0x0044b470
int C2_HOOK_FASTCALL SpendCredits(int pAmount) {
    int remaining;

    remaining = gProgram_state.credits - pAmount;
    gProgram_state.credits = remaining;
    if (remaining < 0) {
        gProgram_state.credits = 0;
        return remaining;
    }
    return 0;
}

// AwardTime

// AwardTime5

// DoTestHeadup

// FUNCTION: CARMA2_HW 0x0044ba60
int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth) {
    int width;

    if (gDevious_2d && gTexture_power_of_2) {
        width = 1;
        while (width < pWidth) {
            width *= 2;
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
            height *= 2;
        }
        return height;
    }
    return pHeight;
}

// DrawThisCarIconNow

