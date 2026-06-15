#include "displays.h"

#include "controls.h"
#include "depth.h"
#include "font.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "loading.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "pedestrn.h"
#include "polyfont.h"
#include "powerups.h"
#include "smashing.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include "c2_string.h"

#include "rec2_macros.h"


// GLOBAL: CARMA2_HW 0x0067c500
tHeadup gHeadups[37];

// GLOBAL: CARMA2_HW 0x0067f890
tQueued_headup gQueued_headups[4];

// GLOBAL: CARMA2_HW 0x0067f888
int gQueued_headup_count;

// GLOBAL: CARMA2_HW 0x0067fcf8
tU32 gLast_centre_headup;

// GLOBAL: CARMA2_HW 0x0059ada0
int gDRFont_to_polyfont_mapping[24] = {
    kPolyfont_ingame_tiny_red,
    kPolyfont_ingame_tiny_yellow,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_green,
    kPolyfont_ingame_italic_yellow,
    kPolyfont_ingame_big_timer,
    kPolyfont_ingame_medium_blue,
    kPolyfont_ingame_medium_red,
    kPolyfont_ingame_medium_orange,
    kPolyfont_ingame_tiny_green,
    kPolyfont_ingame_medium_green,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_red,
    kPolyfont_ingame_tiny_yellow,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_italic_green,
    kPolyfont_ingame_italic_green,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_medium_blue,
};

// GLOBAL: CARMA2_HW 0x00686178
br_material* gAcc_poly_material;

// GLOBAL: CARMA2_HW 0x00686174
br_actor* gAcc_poly_actor;

// GLOBAL: CARMA2_HW 0x0068617c
br_model* gAcc_poly_model;

// GLOBAL: CARMA2_HW 0x00686180
int gRender_acc_poly_actor;

// GLOBAL: CARMA2_HW 0x005962f8
br_token_value gAccent_poly_prims[] = {
    {
        BRT_BLEND_B,
        { 1 },
    }, {
        BRT_OPACITY_X,
        { 0x4b0000 },
    }, {
        BR_NULL_TOKEN,
        { 0 },
    },
};

// GLOBAL: CARMA2_HW 0x0079eaa8
int gCredits_won_headup;

// GLOBAL: CARMA2_HW 0x0079eaf4
int gPed_kill_count_headup;

// GLOBAL: CARMA2_HW 0x0079eaf8
int gCar_kill_count_headup;

// GLOBAL: CARMA2_HW 0x0079eaec
int gTimer_headup;

// GLOBAL: CARMA2_HW 0x0079eb40
int gTime_awarded_headup;

// GLOBAL: CARMA2_HW 0x0079eaf0
int gLaps_headup;

// GLOBAL: CARMA2_HW 0x0079eae8
int gNet_cash_headup;

// GLOBAL: CARMA2_HW 0x0079eaa4
int gNet_ped_headup;


// GLOBAL: CARMA2_HW 0x005913d4
int gLast_fancy_index = -1;

// GLOBAL: CARMA2_HW 0x005913d8
int gLast_credit_headup__displays = -1;

// GLOBAL: CARMA2_HW 0x005913dc
int gLast_time_credit_headup = -1;

// GLOBAL: CARMA2_HW 0x0067f874
tU32 gLast_earn_time;

// GLOBAL: CARMA2_HW 0x0067fd34
tU32 gLast_fancy_time;

// GLOBAL: CARMA2_HW 0x0067fcc4
tU32 gLast_time_earn_time;

// GLOBAL: CARMA2_HW 0x0079eac0
int gOld_times[10];


// GLOBAL: CARMA2_HW 0x0067fcd8
br_font* gBR_fonts[4];

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

// GLOBAL: CARMA2_HW 0x0067fd40
tHud_message gHud_messages[46];

// GLOBAL: CARMA2_HW 0x00705be0
int gCredit_multiplier;

// GLOBAL: CARMA2_HW 0x0067f878
int gLast_credit_amount;

// GLOBAL: CARMA2_HW 0x0067fcc0
tU32 gLast_fancy_headup;

// GLOBAL: CARMA2_HW 0x005913ec
int gPrev_ps_apo_levels[3] = { -1, -1, -1};

// GLOBAL: CARMA2_HW 0x005913f8
int gPrev_ps_drawn_levels[3] = { -1, -1, -1};

// GLOBAL: CARMA2_HW 0x005913e0
int gPS_apo_level_changed[3] = { 1, 1, 1};

// GLOBAL: CARMA2_HW 0x0068c880
br_matrix23 gMatrix23_0068c880;

// GLOBAL: CARMA2_HW 0x0068c868
br_vector3 gOrigin_map;

// GLOBAL: CARMA2_HW 0x00659b40
const int gCar_map_colours[4] = {
    4, 0, 0x34, 0x84,
};

// GLOBAL: CARMA2_HW 0x0068d6f4
int gINT_0068d6f4;

// GLOBAL: CARMA2_HW 0x0074abc4
float gFLOAT_0074abc4;

// GLOBAL: CARMA2_HW 0x0068d8b8
int gINT_0068d8b8;

// GLOBAL: CARMA2_HW 0x0068d8b0
br_vector2 gVector2_0068d8b0;

// GLOBAL: CARMA2_HW 0x0068c874
int gINT_0068c874;

// GLOBAL: CARMA2_HW 0x0068c878
int gINT_0068c878;

// GLOBAL: CARMA2_HW 0x0068d898
br_vector2 gOrigin_headup_map;

// GLOBAL: CARMA2_HW 0x0068d6ec
int gBack_original_origin_x;

// GLOBAL: CARMA2_HW 0x0068d6e8
int gBack_original_origin_y;

// GLOBAL: CARMA2_HW 0x0068d88c
int gBack_original_base_x;

// GLOBAL: CARMA2_HW 0x0068d888
int gBack_original_base_y;

// FUNCTION: CARMA2_HW 0x00465d50
int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText) {

    return PolyFontTextWidth(gDRFont_to_polyfont_mapping[pFont->id], pText);
}

// FUNCTION: CARMA2_HW 0x00465e10
int C2_HOOK_FASTCALL DRTextCleverWidth(const tDR_font* pFont, const char* pText) {
    int polyfont;
    int i;
    int len;
    int result;

    polyfont = gDRFont_to_polyfont_mapping[pFont->id];
    result = 0;
    len = strlen(pText);

    for (i = 0; i < len; i++) {

        if ((tS8)pText[i] < 0) {
            polyfont = gDRFont_to_polyfont_mapping[gFonts[-(tS8) pText[i]].id];
        } else {
            int inter = 0;
            if (i < len - 1) {
                inter = gPoly_fonts[polyfont].interCharacterSpacing;
            }

            result += CharacterWidth(pText[i], polyfont) + inter;
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00465f10
void C2_HOOK_FASTCALL DRPixelmapCentredText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText) {
    int width_over_2;

    width_over_2 = DRTextWidth(pFont, pText) / 2;
    DRPixelmapText(pPixelmap, pX - width_over_2, pY, pFont, pText, width_over_2 + pX);
}

// FUNCTION: CARMA2_HW 0x00466000
void C2_HOOK_FASTCALL OoerrIveGotTextInMeBoxMissus(int pFont_index, const char* pText, br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pCentred) {
    int polyfont;

    polyfont = gDRFont_to_polyfont_mapping[gFonts[pFont_index].id];
    PolyFontTextInABox(polyfont, pText, pLeft, pTop, pRight, pBottom, pCentred ? eJust_centre : eJust_left, gRender_poly_text);
}

// FUNCTION: CARMA2_HW 0x00466210
int C2_HOOK_FASTCALL GetPolyFontIndexToReplaceDRfontWith(const tDR_font* pFont) {

    return gDRFont_to_polyfont_mapping[pFont->id];
}

// FUNCTION: CARMA2_HW 0x00465a70
void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge) {

    RenderPolyTextLine(pText, pX, pY, gDRFont_to_polyfont_mapping[pFont->id], eJust_left, gRender_poly_text);
}

void C2_HOOK_FASTCALL LoadHeadupMessageFile(void) {
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

    if (count > REC2_ASIZE(gHud_messages)) {
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
void C2_HOOK_FASTCALL InitHeadups(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gHeadups) != 37);
    for (i = 0; i < REC2_ASIZE(gHeadups); i++) {
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
    gTimerLeftHUD_model->vertices[3].p.v[1] = (float)-gTimerLeftHUD_dim_y;
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
    gTimerRightHUD_model->vertices[3].p.v[1] = (float)-gTimerRightHUD_dim_y;
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

    LoadHeadupMessageFile();
}

// FUNCTION: CARMA2_HW 0x00449630
void C2_HOOK_FASTCALL ClearHeadup(int pIndex) {

    gHeadups[pIndex].type = eHeadup_unused;
}

// FUNCTION: CARMA2_HW 0x00449650
void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index) {
    int i;
    tHeadup* the_headup;

    for (i = 0; i < REC2_ASIZE(gHeadups); i++) {
        the_headup = &gHeadups[i];
        if (the_headup->type != eHeadup_unused && the_headup->slot_index == pSlot_index) {
            ClearHeadup(i);
            return;
        }
        the_headup++;
    }
}

// FUNCTION: CARMA2_HW 0x00449690
void C2_HOOK_FASTCALL ClearHeadups(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gHeadups) != 37);
    C2_HOOK_BUG_ON(REC2_ASIZE(gOld_times) != 10);

    for (i = 0; i < REC2_ASIZE(gHeadups); i++) {
        if (gHeadups[i].type != eHeadup_unused) {
            ClearHeadup(i);
        }
    }
    gLast_fancy_index = -1;
    gLast_credit_headup__displays = -1;
    gLast_time_credit_headup = -1;
    gLast_earn_time = 0;
    for (i = 0; i < REC2_ASIZE(gOld_times); i++) {
        gOld_times[i] = 0;
    }
    gLast_fancy_time = 0;
    gLast_time_earn_time = 0;
    gQueued_headup_count = 0;
    gLast_centre_headup = 0;
}

int C2_HOOK_FASTCALL IsHeadupTextClever(char* pText) {

    while (*pText) {
        if (*(signed char*)pText < 0) {
            return 1;
        }
        pText++;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0044a220
int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup) {
    int width;

    C2_HOOK_BUG_ON(sizeof(tHeadup) != 356);

#ifndef REC2_MATCHING
    C2_HOOK_BUG_ON((int)&((tHeadup*)0)->data.text_info.text != 0x4c);
    C2_HOOK_BUG_ON((int)&((tHeadup*)0)->data.coloured_text_info.coloured_font != 0x148);
#endif

    width = 0;
    if (pHeadup->type == eHeadup_box_text) {
        return 0;
    } else if (pHeadup->type == eHeadup_coloured_text) {

        pHeadup->clever = IsHeadupTextClever(pHeadup->data.text_info.text);
        if (pHeadup->justification == eJust_left) {
            pHeadup->x = pHeadup->original_x;
        } else if (pHeadup->justification == eJust_right) {
            if (pHeadup->clever) {
                width = DRTextCleverWidth(
                    pHeadup->data.coloured_text_info.coloured_font,
                    pHeadup->data.text_info.text);
            } else {
                width = DRTextWidth(
                    pHeadup->data.coloured_text_info.coloured_font,
                    pHeadup->data.text_info.text);
            }
            pHeadup->x = pHeadup->original_x - width;
        } else if (pHeadup->justification == eJust_centre) {
            if (pHeadup->clever) {
                width = DRTextCleverWidth(
                    pHeadup->data.coloured_text_info.coloured_font,
                    pHeadup->data.text_info.text);
            } else {
                width = DRTextWidth(
                    pHeadup->data.coloured_text_info.coloured_font,
                    pHeadup->data.text_info.text);
            }
            pHeadup->x = pHeadup->original_x - width / 2;
        }
    } else if (pHeadup->type ==eHeadup_fancy) {
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

// FUNCTION: CARMA2_HW 0x00449fa0
int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index) {
    int i;
    int empty_one;
    tHeadup* the_headup;

    empty_one = -1;
    for (i = 0; i < REC2_ASIZE(gHeadups); i++) {
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

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void) {

    C2_HOOK_BUG_ON(sizeof(tQueued_headup) != 0x10c);

    gQueued_headup_count -= 1;
    memmove(&gQueued_headups[0], &gQueued_headups[1], gQueued_headup_count * sizeof(tQueued_headup));
}

// FUNCTION: CARMA2_HW 0x004497b0
void C2_HOOK_FASTCALL ClearQueuedHeadups(void) {

    while (gQueued_headup_count != 0) {
        KillOldestQueuedHeadup();
    }
}

// FUNCTION: CARMA2_HW 0x00449830
void C2_HOOK_FASTCALL DoPSPowerHeadup(int pLevel, const char* pAPO_str) {
    int changed;
    br_pixelmap *dest_pm;
    br_pixelmap *icon;
    int count_slots;
    int level;
    int i;
    int j;
    int delta;
    int delta_x = 2;

    switch (pAPO_str[0]) {
    case 'A':
        dest_pm = gArmour_colour_map;
        icon = gIcon_armour;
        count_slots = gProgram_state.current_car.power_up_slots[0];
        if (pLevel != gPrev_ps_apo_levels[0]
                || gProgram_state.current_car.power_up_slots[0] != gPrev_ps_drawn_levels[0]) {
            gPS_apo_level_changed[0] = 1;
        } else {
            gPS_apo_level_changed[0] = 1;
        }
        gPrev_ps_drawn_levels[0] = gProgram_state.current_car.power_up_slots[0];
        gPrev_ps_apo_levels[0] = pLevel;
        changed = gPS_apo_level_changed[0];
        break;
    case 'P':
        dest_pm = gPower_colour_map;
        icon = gIcon_power;
        count_slots = gProgram_state.current_car.power_up_slots[1];
        if (pLevel != gPrev_ps_apo_levels[1]
                || gProgram_state.current_car.power_up_slots[1] != gPrev_ps_drawn_levels[1]) {
            gPS_apo_level_changed[1] = 1;
        } else {
            gPS_apo_level_changed[1] = 1;
        }
        gPrev_ps_drawn_levels[1] = gProgram_state.current_car.power_up_slots[1];
        gPrev_ps_apo_levels[1] = pLevel;
        changed = gPS_apo_level_changed[1];
        break;
    case 'O':
        dest_pm = gOffensive_colour_map;
        icon = gIcon_offense;
        count_slots = gProgram_state.current_car.power_up_slots[2];
        if (pLevel != gPrev_ps_apo_levels[2]
                || gProgram_state.current_car.power_up_slots[2] != gPrev_ps_drawn_levels[2]) {
            gPS_apo_level_changed[2] = 1;
        } else {
            gPS_apo_level_changed[2] = 1;
        }
        gPrev_ps_drawn_levels[2] = gProgram_state.current_car.power_up_slots[2];
        gPrev_ps_apo_levels[2] = pLevel;
        changed = gPS_apo_level_changed[2];
        break;
#ifdef REC2_FIX_BUGS
    default:
        abort();
#endif
    }
    if (changed) {
        DRPixelmapCopy(dest_pm, gPowerbar);
        DRPixelmapRectangleMaskedCopy(dest_pm, delta_x, 0, icon, 0, 0, icon->width, icon->height);
        count_slots = count_slots - pLevel;
        level = pLevel;

        /* Draw staggered APO levels */
        for (i = 0; i < 15; i += 5) {
            for (j = 0; j < 10; j++) {
                delta = delta_x;
                if (level > 0) {
                    DRPixelmapRectangleMaskedCopy(dest_pm,
                        j * gIcon_litBloc1->width + delta + icon->width + 2, i,
                        gIcon_litBloc1, 0, 0, gIcon_litBloc1->width, gIcon_litBloc1->height);
                    level -= 1;
                } else if (count_slots > 0) {
                    DRPixelmapRectangleMaskedCopy(dest_pm,
                        j * gIcon_grnBlock1->width + delta + icon->width + 2, i,
                        gIcon_grnBlock1, 0, 0, gIcon_grnBlock1->width, gIcon_grnBlock1->height);
                    count_slots -= 1;
                } else {
                    DRPixelmapRectangleMaskedCopy(dest_pm,
                      j * gIcon_greyBloc1->width + delta + icon->width + 2, i,
                      gIcon_greyBloc1, 0, 0, gIcon_greyBloc1->width, gIcon_greyBloc1->height);
                }
            }
            delta -= 2;
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

// FUNCTION: CARMA2_HW 0x00449800
void C2_HOOK_FASTCALL DoPSPowerupHeadups(void) {

    DoPSPowerHeadup(gProgram_state.current_car.power_up_levels[0], "A");
    DoPSPowerHeadup(gProgram_state.current_car.power_up_levels[1], "P");
    DoPSPowerHeadup(gProgram_state.current_car.power_up_levels[2], "O");
}

// FUNCTION: CARMA2_HW 0x00449fd0
int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it) {
    int index;
    tHeadup* the_headup;
    tHeadup_slot* headup_slot;
    tU32 time;

    time = PDGetTotalTime();
    if (pQueue_it && pSlot_index == 4 && time - gLast_centre_headup < 1000) {
        if (gQueued_headup_count == 4) {
            KillOldestQueuedHeadup();
        }
        gQueued_headups[gQueued_headup_count].flash_rate = pFlash_rate;
        gQueued_headups[gQueued_headup_count].lifetime = pLifetime;
        gQueued_headups[gQueued_headup_count].font_index = pFont_index;
        strcpy(gQueued_headups[gQueued_headup_count].text, pText);
        gQueued_headup_count++;
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
    the_headup->right_edge = MungeHeadupWidth(the_headup) + the_headup->x;
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

// FUNCTION: CARMA2_HW 0x00465aa0
void C2_HOOK_FASTCALL DRPixelmapCleverText(br_pixelmap* pPixelmap, int pX, int pY, tDR_font* pFont, const char* pText, int pRight_edge) {
    int i;
    char s[512];
    int s_end;
    int x;
    int next_x;
    int len;

    len = strlen(pText);
    s_end = -1;
    x = pX;
    next_x = pX;
    for (i = 0; i < len; i++) {
        tS8 ch = pText[i];

        if (ch < 0) {

            if (s_end >= 0) {
                s[s_end + 1] = '\0';
                RenderPolyTextLine(s, x, pY, gDRFont_to_polyfont_mapping[pFont->id], eJust_left, gRender_poly_text);
                s_end = -1;
                x = next_x;
            }
            /* Change font */
            pY -= (PolyFontHeight(gDRFont_to_polyfont_mapping[gFonts[-ch].id]) - PolyFontHeight(gDRFont_to_polyfont_mapping[pFont->id])) / 2;
            pFont = &gFonts[-ch];
        } else {
            s_end += 1;
            s[s_end] = pText[i];
            next_x += CharacterWidth(gDRFont_to_polyfont_mapping[pFont->id], pText[i]);
        }
    }
    if (s_end >= 0) {
        s[s_end + 1] = '\0';
        RenderPolyTextLine(s, x, pY, gDRFont_to_polyfont_mapping[pFont->id], eJust_left, gRender_poly_text);
    }
}

// FUNCTION: CARMA2_HW 0x0045aa60
void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight) {

    if (gAcc_poly_material == NULL) {
        gAcc_poly_material = BrMaterialAllocate("Acc Poly Mat");
        gAcc_poly_material->colour = 0x80;
        gAcc_poly_material->flags = BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(gAcc_poly_material);
        gAcc_poly_material->extra_prim = gAccent_poly_prims;
        gAccent_poly_prims[1].v.x = 0x800000;
        BrMaterialUpdate(gAcc_poly_material, BR_MATU_EXTRA_PRIM);
    }
    if (gAcc_poly_actor == NULL) {
        gAcc_poly_model = BrModelAllocate("Tint Poly", 6, 2);
        BrVector3Set(&gAcc_poly_model->vertices[0].p, pX,          -pY,                -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[1].p, pX + pWidth, -pY,                -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[2].p, pX,          -(pY + pHeight),    -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[3].p, pX + pWidth, -pY,                -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[4].p, pX + pWidth, -(pY + pHeight),    -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[5].p, pX,          -(pY + pHeight),    -1.01f);
        gAcc_poly_model->faces[0].vertices[0] = 0;
        gAcc_poly_model->faces[0].vertices[1] = 1;
        gAcc_poly_model->faces[0].vertices[2] = 2;
        gAcc_poly_model->faces[1].vertices[0] = 3;
        gAcc_poly_model->faces[1].vertices[1] = 4;
        gAcc_poly_model->faces[1].vertices[2] = 5;
        gAcc_poly_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gAcc_poly_actor->material = gAcc_poly_material;
        gAcc_poly_model->faces[0].material = gAcc_poly_material;
        gAcc_poly_model->faces[1].material = gAcc_poly_material;
        gAcc_poly_actor->model = gAcc_poly_model;
        BrModelAdd(gAcc_poly_model);
    }
    if (!gRender_acc_poly_actor) {
        gRender_acc_poly_actor = 1;
        BrActorAdd(g2d_camera, gAcc_poly_actor);
    }
}

// FUNCTION: CARMA2_HW 0x0044b300
void C2_HOOK_FASTCALL EarnCredits2(int pAmount, const char* pPrefix_text) {
    char s[256];
    int original_amount;
    tU32 the_time;

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
        sprintf(s, "%s%d %s", pPrefix_text, pAmount, GetMiscString(eMiscString_credits));
    } else if (pAmount == 1) {
        sprintf(s, "%s1 %s", pPrefix_text, GetMiscString(eMiscString_credit));
    } else if (pAmount == -1) {
        sprintf(s, "%s%s 1 %s", pPrefix_text, GetMiscString(eMiscString_lost_credits_prefix), GetMiscString(eMiscString_credit));
    } else {
        sprintf(s, "%s%s %d %s", GetMiscString(eMiscString_lost_credits_prefix), pPrefix_text, -pAmount, GetMiscString(eMiscString_credits));
    }
    gProgram_state.credits += original_amount;
    gLast_credit_headup__displays = NewTextHeadupSlot(4, 0, 2000, -4, s);
    gLast_earn_time = the_time;
}

// FUNCTION: CARMA2_HW 0x0044b440
void C2_HOOK_FASTCALL EarnCredits(int pAmount) {

    EarnCredits2(pAmount, "");
}

// FUNCTION: CARMA2_HW 0x0044a3a0
int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index) {
    int index;

    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index >= 0) {
        tHeadup* the_headup;
        tHeadup_slot* headup_slot;

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
            the_headup->x = the_headup->original_x + DRTextWidth(&gFonts[gHud_messages[pImage_index].font1], gHud_messages[pImage_index].message);
            break;
        case eJust_centre:
            the_headup->x = the_headup->original_x + DRTextWidth(&gFonts[gHud_messages[pImage_index].font1], gHud_messages[pImage_index].message) / 2;
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
        strcpy(the_headup->data.image_info.text , gHud_messages[pImage_index].message);
        the_headup->data.image_info.font_index = gHud_messages[pImage_index].font2;
        the_headup->data.image_info.font = &gFonts[gHud_messages[pImage_index].font1];
        switch (the_headup->justification) {
        case eJust_left:
            the_headup->data.image_info.field_0x104 = 0;
            the_headup->data.image_info.field_0x10c = -DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            break;
        case eJust_right:
            the_headup->data.image_info.field_0x104 = DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text);
            the_headup->data.image_info.field_0x10c = 0;
            break;
        case eJust_centre:
            the_headup->data.image_info.field_0x104 =  DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text) / 2;
            the_headup->data.image_info.field_0x10c = -DRTextWidth(the_headup->data.image_info.font, the_headup->data.image_info.text) / 2;
            break;
        }
    }
    return index;
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

// FUNCTION: CARMA2_HW 0x0044a600
void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex) {
    tU32 the_time;
    int temp_ref;
    int center;

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
        tHeadup* the_headup;

        gLast_fancy_headup = temp_ref;
        gLast_fancy_index = pIndex;
        gLast_fancy_time = the_time;

        the_headup = &gHeadups[temp_ref];
        the_headup->type = eHeadup_fancy;
        the_headup->data.fancy_info.field_0x108 = 0;
        the_headup->data.fancy_info.fancy_stage = 0;
        switch (the_headup->justification) {
        case eJust_left:
            the_headup->data.fancy_info.offset = center;
            the_headup->data.fancy_info.end_offset = -DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            break;
        case eJust_right:
            the_headup->data.fancy_info.offset = center + DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text) / 2;
            the_headup->data.fancy_info.end_offset = center;
            break;
        case eJust_centre:
            the_headup->data.fancy_info.offset =  center + DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text) / 2;
            the_headup->data.fancy_info.end_offset = -center - DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text) / 2;
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044b470
int C2_HOOK_FASTCALL SpendCredits(int pAmount) {

    gProgram_state.credits -= pAmount;
    if (gProgram_state.credits < 0) {
        gProgram_state.credits = 0;
    }
    return gProgram_state.credits;
}

#define HELLO _Pragma("error")

// FUNCTION: CARMA2_HW 0x0044b170
void C2_HOOK_FASTCALL ChangingView(void) {
    tU32 the_time;

    the_time = PDGetTotalTime() - gProgram_state.view_change_start;
    if (gProgram_state.new_view == eView_undefined) {
        return;
    }
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
    if (the_time > 175 && gProgram_state.which_view == gProgram_state.new_view) {
        switch (gProgram_state.pending_view) {
        case eView_undefined:
            gProgram_state.new_view = eView_undefined;
            break;
        case eView_left:
            LookLeft();
            break;
        case eView_forward:
            LookForward();
            break;
        case eView_right:
            LookRight();
            break;
        }
    } else {
        if (the_time < 88) {
            if (gProgram_state.old_view < gProgram_state.new_view) {
                gScreen_wobble_x = (int)((double)the_time * (double)gCurrent_graf_data->cock_margin_x * -2. / 175.);
            } else {
                gScreen_wobble_x = (int)((double)the_time * (double)gCurrent_graf_data->cock_margin_x * 2. / 175.);
            }
        } else {
            gProgram_state.which_view = gProgram_state.new_view;
            switch (gProgram_state.new_view) {
#ifdef REC2_FIX_BUGS
            default:
                abort();
#endif
            case eView_left:
                gProgram_state.cockpit_image_index = 1;
                break;
            case eView_forward:
                gProgram_state.cockpit_image_index = 0;
                break;
            case eView_right:
                gProgram_state.cockpit_image_index = 2;
                break;
            }
            AdjustRenderScreenSize();
            if (gProgram_state.new_view <= gProgram_state.old_view) {
                gScreen_wobble_x = (int)((double)(175 - the_time) * (double)gCurrent_graf_data->cock_margin_x * -2. / 175.);
            } else {
                gScreen_wobble_x = (int)((double)(175 - the_time) * (double)gCurrent_graf_data->cock_margin_x * 2. / 175.);
            }
        }
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

// FUNCTION: CARMA2_HW 0x0044a920
void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont) {

    if (pHeadup_index >= 0) {
        gHeadups[pHeadup_index].data.image_info.font = &gFonts[-pFont];
    }
}

// FUNCTION: CARMA2_HW 0x0047cbd0
void C2_HOOK_FASTCALL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {
    tU8* ptr;
    tU8* depth_table_ptr;
    tU8* right_ptr;
    int x;
    int y;
    int line_skip;
    int width;

    if (gDevious_2d) {
        DeviouslyDimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
        return;
    }

    ptr = (tU8*)pPixelmap->pixels + pLeft + pPixelmap->row_bytes * pTop;
    line_skip = pPixelmap->row_bytes - pRight + pLeft;
    depth_table_ptr = gDepth_shade_table->pixels;
    x = gDepth_shade_table->row_bytes * gDim_amount;
    width = pRight - pLeft;

    if (pKnock_out_corners) {
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr + x];
        }
        ptr += line_skip + 1;
        for (y = pTop + 1; y < (pBottom - 1); y++, ptr += line_skip) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr + x];
            }
        }
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr + x];
        }
    } else {
        for (y = pTop; y < pBottom; y++) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr + x];
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

// FUNCTION: CARMA2_HW 0x004496f0
void C2_HOOK_FASTCALL DimAFewBits(void) {
    int i;

#define DIM_INDEX gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0
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

// FUNCTION: CARMA2_HW 0x0047cad0
void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {

    gDim_model->vertices[1].p.v[0] = (float)pLeft;
    gDim_model->vertices[0].p.v[0] = gDim_model->vertices[1].p.v[0];
    gDim_model->vertices[3].p.v[0] = (float)pRight;
    gDim_model->vertices[2].p.v[0] = gDim_model->vertices[3].p.v[0];
    gDim_model->vertices[3].p.v[1] = -(float)pTop;
    gDim_model->vertices[0].p.v[1] = gDim_model->vertices[3].p.v[1];
    gDim_model->vertices[2].p.v[1] = -(float)pBottom;
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

// FUNCTION: CARMA2_HW 0x0044a9d0
void C2_HOOK_FASTCALL DoDamageScreen(tU32 pThe_time) {
    int i;
    int y_pitch;
    int the_step;
    int the_wobble_x;
    int the_wobble_y;
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

    for (i = 0; i < REC2_ASIZE(gProgram_state.current_car.damage_units); i++) {
        the_damage = &gProgram_state.current_car.damage_units[i];
        if (i != eDamage_driver) {
            the_image = the_damage->images;
            the_step = 5 * the_damage->damage_level / 100;
            y_pitch = (the_image->height / 2) / 5;
            DRPixelmapRectangleMaskedCopy(
                gDamage_hud,
                the_wobble_x + gProgram_state.current_car.damage_units[i].x_coord,
                the_wobble_y + gProgram_state.current_car.damage_units[i].y_coord,
                the_image,
                0,
                y_pitch * (2 * the_step + ((pThe_time / the_damage->periods[the_step]) & 1)),
                the_image->width,
                y_pitch);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00449b10
void C2_HOOK_FASTCALL DoHeadups(tU32 pThe_time) {
    int i;
    int x_offset;
    int y_offset;
    tHeadup* the_headup;

    if (gNet_mode != eNet_mode_none) {
        DoNetScores();
    }
    if (gQueued_headup_count && PDGetTotalTime() - gLast_centre_headup >= 1000) {
        NewTextHeadupSlot(4,
            gQueued_headups[0].flash_rate,
            gQueued_headups[0].lifetime,
            gQueued_headups[0].font_index,
            gQueued_headups[0].text);
        KillOldestQueuedHeadup();
    }
    if (gHeadup_detail_level == 0 || gHeadup_detail_level == 3) {
        MoveHeadupTo(gTimer_headup, 634, 2);
    } else {
        MoveHeadupTo(gTimer_headup, 389, 13);
    }

    for (i = 0; i < REC2_ASIZE(gHeadups); i++) {
        the_headup = &gHeadups[i];
        if (the_headup->type != eHeadup_unused
                && (gProgram_state.which_view == eView_forward || !the_headup->cockpit_anchored)
                && (the_headup->type == eHeadup_image
                    || the_headup->type == eHeadup_fancy
                    || (the_headup->type == eHeadup_text && the_headup->data.text_info.text[0] != '\0')
                    || ((the_headup->type == eHeadup_coloured_text || the_headup->type == eHeadup_box_text)
                        && the_headup->data.text_info.text[0] != '\0'))) {
            if (the_headup->type == eHeadup_fancy || the_headup->end_time == 0 || pThe_time < the_headup->end_time) {
                if (the_headup->flash_period == 0
                        || Flash(the_headup->flash_period, &the_headup->last_flash, &the_headup->flash_state)) {

                    switch (the_headup->type) {
                    case eHeadup_text:
                        if (the_headup->cockpit_anchored) {
                            y_offset = gScreen_wobble_y;
                        } else {
                            y_offset = 0;
                        }
                        if (the_headup->cockpit_anchored) {
                            x_offset = gScreen_wobble_x;
                        } else {
                            x_offset = 0;
                        }
                        BrPixelmapText(
                            gRender_screen,
                            x_offset + the_headup->x,
                            y_offset + the_headup->y,
                            the_headup->data.text_info.colour,
                            the_headup->data.text_info.font,
                            the_headup->data.text_info.text);
                        break;
                    case eHeadup_coloured_text:
                        if (gHeadup_detail_level == 2 || gHeadup_detail_level == 5
                                || (i != gCredits_won_headup
                                    && i != gPed_kill_count_headup
                                    && i != gCar_kill_count_headup
                                    && i != gLaps_headup)) {
                            if (the_headup->clever) {
                                if (the_headup->cockpit_anchored) {
                                    y_offset = gScreen_wobble_y;
                                } else {
                                    y_offset = 0;
                                }
                                if (the_headup->cockpit_anchored) {
                                    x_offset = gScreen_wobble_x;
                                } else {
                                    x_offset = 0;
                                }
                                DRPixelmapCleverText(
                                        gBack_screen,
                                        x_offset + the_headup->x,
                                        y_offset + the_headup->y,
                                        the_headup->data.coloured_text_info.coloured_font,
                                        the_headup->data.coloured_text_info.text,
                                        the_headup->right_edge);
                            } else {
                                if (the_headup->cockpit_anchored) {
                                    y_offset = gScreen_wobble_y;
                                } else {
                                    y_offset = 0;
                                }
                                if (the_headup->cockpit_anchored) {
                                    x_offset = gScreen_wobble_x;
                                } else {
                                    x_offset = 0;
                                }
                                DRPixelmapText(
                                        gBack_screen,
                                        x_offset + the_headup->x,
                                        y_offset + the_headup->y,
                                        the_headup->data.coloured_text_info.coloured_font,
                                        the_headup->data.coloured_text_info.text,
                                        the_headup->right_edge);
                            }
                        }
                        break;
                    case eHeadup_image:
                        RenderPolyTextLine(the_headup->data.image_info.text,
                             the_headup->original_x + the_headup->data.image_info.field_0x104,
                             the_headup->y,
                             the_headup->data.image_info.font_index, eJust_centre, 0);
                        break;

                    case eHeadup_fancy:
                        switch (the_headup->data.fancy_info.fancy_stage) {
                        case 0:
                            the_headup->data.fancy_info.offset -= 325 * gFrame_period / 1000;
                            if (the_headup->data.fancy_info.offset <= 0) {
                                the_headup->data.fancy_info.offset = 0;
                                the_headup->data.fancy_info.fancy_stage = 1;
                                the_headup->data.fancy_info.start_time = GetTotalTime();
                            }
                            RenderPolyTextLine(the_headup->data.fancy_info.text,
                                the_headup->data.fancy_info.offset,
                                the_headup->y,
                                the_headup->data.fancy_info.font_index,
                                eJust_centre,
                                0);
                            break;
                        case 1:
                            if (pThe_time - the_headup->data.fancy_info.start_time > 1000) {
                                the_headup->data.fancy_info.fancy_stage = 2;
                                the_headup->data.fancy_info.start_time = GetTotalTime();
                            }
                            RenderPolyTextLine(the_headup->data.fancy_info.text,
                                the_headup->data.fancy_info.offset,
                                the_headup->y,
                                the_headup->data.fancy_info.font_index,
                                eJust_centre,
                                0);
                            break;
                        case 2:
                            the_headup->data.fancy_info.offset -= 325 * gFrame_period / 1000;
                            if (the_headup->data.fancy_info.offset <= the_headup->data.fancy_info.end_offset) {
                                ClearHeadup(i);
                            } else {
                                RenderPolyTextLine(the_headup->data.fancy_info.text,
                                    the_headup->data.fancy_info.offset,
                                    the_headup->y,
                                    the_headup->data.fancy_info.font_index,
                                    eJust_centre,
                                    0);
                            }
                            break;
                        default:
                            break;
                        }
                        break;

                    case eHeadup_box_text:
                        if (the_headup->cockpit_anchored) {
                            y_offset = gScreen_wobble_y;
                        } else {
                            y_offset = 0;
                        }
                        if (the_headup->cockpit_anchored) {
                            x_offset = gScreen_wobble_x;
                        } else {
                            x_offset = 0;
                        }
                        OoerrIveGotTextInMeBoxMissus(
                            the_headup->data.coloured_text_info.coloured_font - gFonts,
                            the_headup->data.coloured_text_info.text,
                            gRender_screen,
                            gRender_screen->width / 10,
                            x_offset + the_headup->y,
                            9 * gRender_screen->width / 10,
                            y_offset + the_headup->y + 60,
                            1);
                        break;
                    default:
                        break;
                    }
                }
            } else {
                ClearHeadup(i);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044aad0
void C2_HOOK_FASTCALL DoInstruments(tU32 pThe_time) {
    char buffer[8];
    br_pixelmap* speedo_image;
    int the_wobble_x;
    int the_wobble_y;
    int gear;
    int gear_height; /* Added by dethrace. */
    double the_angle;
    double the_angle2;
    double sin_angle;
    double cos_angle;
    double speed_mph;

    if (gCar_to_view != NULL && gCar_to_view->driver >= eDriver_oppo) {
        speed_mph = gCar_to_view->speedo_speed * WORLD_SCALE / 1600.0f * 1000.f * 3600.0f;
        if (speed_mph < 0.0f) {
            speed_mph = 0.0f;
        }
        if (gHeadup_detail_level == 0 || gHeadup_detail_level == 3) {
            if (!gAction_replay_mode) {
                sprintf(buffer, "%03i", (int)speed_mph);
                RenderPolyTextLine(buffer, 1, 1, kPolyfont_ingame_medium_green, eJust_left, 0);
            }
        } else {
            if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0) {
                if (gProgram_state.which_view != eView_forward) {
                    return;
                }
                the_wobble_x = gScreen_wobble_x;
                the_wobble_y = gScreen_wobble_y;
            } else {
                the_wobble_x = 0;
                the_wobble_y = 0;
            }
            if (gProgram_state.current_car.tacho_radius_2[gProgram_state.cockpit_on] >= 0) {
                int rev;

                rev = div((int)gCar_to_view->revs, div(gCar_to_view->red_line, 11).quot).quot;
                if (rev < 0) {
                    rev = 0;
                } else if (rev > 11) {
                    rev = 11;
                }
                gCurrent_rev = gRevs[rev];
            } else {
                speedo_image = gRevs[div((int)gCar_to_view->revs, 11).quot];
                BrPixelmapRectangleCopy(gCurrent_rev,
                    gProgram_state.current_car.tacho_x[gProgram_state.cockpit_on] + the_wobble_x,
                    gProgram_state.current_car.tacho_y[gProgram_state.cockpit_on] + the_wobble_y,
                    speedo_image, 0, 0, speedo_image->width, speedo_image->height);

            }
            if (!gProgram_state.cockpit_on || gProgram_state.cockpit_image_index < 0 || gProgram_state.which_view == eView_forward) {

                gear = (int)gCar_to_view->gear;
                if (gear < 0) {
                    gear = -1;
                }
                gear_height = gProgram_state.current_car.gears_image->height / 8;
                DRPixelmapRectangleMaskedCopy(gCurrent_rev,
                    gProgram_state.current_car.gear_x[gProgram_state.cockpit_on] + the_wobble_x,
                    gProgram_state.current_car.gear_y[gProgram_state.cockpit_on] + the_wobble_y,
                    gProgram_state.current_car.gears_image,
                    0, (gear + 1) * gear_height,
                    gProgram_state.current_car.gears_image->width,
                    gear_height);
            }
            speedo_image = gProgram_state.current_car.speedo_image[gProgram_state.cockpit_on];
            if (gProgram_state.current_car.speedo_radius_2[gProgram_state.cockpit_on] >= 0) {
                if (speedo_image != NULL && (!gProgram_state.cockpit_on || gProgram_state.cockpit_image_index < 0)) {
                    DRPixelmapRectangleMaskedCopy(gCurrent_rev,
                        gProgram_state.current_car.speedo_x[gProgram_state.cockpit_on] + the_wobble_x,
                        gProgram_state.current_car.speedo_y[gProgram_state.cockpit_on] + the_wobble_y,
                        speedo_image, 0, 0, speedo_image->width, speedo_image->height);
                }
                if (speed_mph <= gProgram_state.current_car.max_speed) {
                    the_angle = DEG_TO_RAD(gProgram_state.current_car.speedo_start_angle[gProgram_state.cockpit_on]
                        + ((float)gProgram_state.current_car.speedo_end_angle[gProgram_state.cockpit_on] - (float)gProgram_state.current_car.speedo_start_angle[gProgram_state.cockpit_on]) * speed_mph
                            / (float)gProgram_state.current_car.max_speed);
                } else {
                    the_angle = DEG_TO_RAD((float)gProgram_state.current_car.speedo_end_angle[gProgram_state.cockpit_on]);
                }
                if (the_angle < 0.0) {
                    the_angle += REC2_TAU;
                } else if (the_angle >= REC2_TAU) {
                    the_angle -= REC2_TAU;
                }
                the_angle2 = REC2_PI_OVER_2 - the_angle;
                if (the_angle2 < 0.0) {
                    the_angle2 += REC2_TAU;
                }
                if (the_angle2 > REC2_3PI_OVER_2) {
                    cos_angle = gCosine_array[(unsigned int)((REC2_TAU - the_angle2) / REC2_PI * 128.0)];
                } else if (the_angle2 > REC2_PI) {
                    cos_angle = -gCosine_array[(unsigned int)((the_angle2 - REC2_PI) / REC2_PI * 128.0)];
                } else if (the_angle2 > REC2_PI_OVER_2) {
                    cos_angle = -gCosine_array[(unsigned int)((REC2_PI - the_angle2) / REC2_PI * 128.0)];
                } else {
                    cos_angle = gCosine_array[(unsigned int)(the_angle2 / REC2_PI * 128.0)];
                }

                if (the_angle > REC2_3PI_OVER_2) {
                    sin_angle = gCosine_array[(unsigned int)((REC2_TAU - the_angle) / REC2_PI * 128.0)];
                } else if (the_angle > REC2_PI) {
                    sin_angle = -gCosine_array[(unsigned int)((the_angle - REC2_PI) / REC2_PI * 128.0)];
                } else if (the_angle > REC2_PI_OVER_2) {
                    sin_angle = -gCosine_array[(unsigned int)((REC2_PI - the_angle) / REC2_PI * 128.0)];
                } else {
                    sin_angle = gCosine_array[(unsigned int)(the_angle / REC2_PI * 128.0)];
                }

                PoshDrawLine(
                    gBack_screen,
                    (int)((double)gProgram_state.current_car.speedo_radius_1[gProgram_state.cockpit_on] * sin_angle
                    + (double)the_wobble_x
                    + (double)gProgram_state.current_car.speedo_centre_x[gProgram_state.cockpit_on]),
                    (float)the_angle,
                    (int)((double)gProgram_state.current_car.speedo_centre_y[gProgram_state.cockpit_on]
                    - (double)gProgram_state.current_car.speedo_radius_1[gProgram_state.cockpit_on] * cos_angle
                    + (double)the_wobble_y),
                    (int)((double)gProgram_state.current_car.speedo_radius_2[gProgram_state.cockpit_on] * sin_angle
                    + (double)the_wobble_x
                    + (double)gProgram_state.current_car.speedo_centre_x[gProgram_state.cockpit_on]),
                    (int)((double)gProgram_state.current_car.speedo_centre_y[gProgram_state.cockpit_on]
                    + (double)the_wobble_y
                    - (double)gProgram_state.current_car.speedo_radius_2[gProgram_state.cockpit_on] * cos_angle),
                    gProgram_state.current_car.speedo_needle_colour[gProgram_state.cockpit_on]);
            } else if (speedo_image != NULL) {
                DrawNumberAt(speedo_image,
                    gCurrent_rev,
                    gProgram_state.current_car.speedo_x[gProgram_state.cockpit_on] + the_wobble_x,
                    gProgram_state.current_car.speedo_y[gProgram_state.cockpit_on] + the_wobble_y,
                    gProgram_state.current_car.speedo_x_pitch[gProgram_state.cockpit_on],
                    gProgram_state.current_car.speedo_y_pitch[gProgram_state.cockpit_on],
                    (int)speed_mph, 3, 1);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047c740
void C2_HOOK_FASTCALL PoshDrawLine(br_pixelmap* pDestn, int pX1, float pAngle, int pY1, int pX2, int pY2, int pColour) {

    // FIXME: order of arguments is non-sensical (pX1 and pAngle should be switched)
    if (pColour < 0) {
        if (pAngle >= 0.785 && pAngle <= 5.498 && (pAngle <= 2.356 || pAngle >= 3.926)) {
            if ((pAngle <= 0.785 || pAngle >= 1.57) && (pAngle <= 3.926 || pAngle >= 4.712)) {
                DRDrawLine(pDestn, pX1 - 1, pY1, pX2 - 1, pY2, -pColour - 1);
                DRDrawLine(pDestn, pX1 + 1, pY1, pX2 + 1, pY2, 1 - pColour);
            } else {
                DRDrawLine(pDestn, pX1 - 1, pY1, pX2 - 1, pY2, 1 - pColour);
                DRDrawLine(pDestn, pX1 + 1, pY1, pX2 + 1, pY2, -pColour - 1);
            }
        } else {
            DRDrawLine(pDestn, pX1, pY1 + 1, pX2, pY2 + 1, -pColour - 1);
            DRDrawLine(pDestn, pX1, pY1 - 1, pX2, pY2 - 1, 1 - pColour);
        }
        DRDrawLine(pDestn, pX1, pY1, pX2, pY2, -pColour);
    } else {
        DRDrawLine(pDestn, pX1, pY1, pX2, pY2, pColour);
    }
}

br_uint_32 C2_HOOK_FASTCALL CarArrowColour(tCar_spec *pCar, tVehicle_type pCategory) {
    tNet_game_player_info* net_player;

    if (gNet_mode != eNet_mode_none && pCar->shrapnel_materials[0] != NULL) {
        net_player = NetPlayerFromCar(pCar);
        if (net_player->field_0x80) {
            return 0xffffffff;
        } else {
            return pCar->shrapnel_materials[0]->index_base;
        }
    } else {
        if (pCar->knackered) {
            return 0x0;
        } else {
            return gCar_map_colours[pCategory];
        }
    }
}

void C2_HOOK_FASTCALL DrawCarArrow(br_pixelmap* pScreen, tCar_spec *pCar, tU32 pTime, const br_vector3* pPosition, tVehicle_type category) {

    DrawMapBlip(pScreen, pCar, pTime, &pCar->car_master_actor->t.t.mat, pPosition, CarArrowColour(pCar, category));
}

void C2_HOOK_FASTCALL DrawSmashBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3* pPosition, int pIndex) {
    br_vector3 map_pos;
    br_vector3 dir;
    br_scalar dot;

    BrMatrix34ApplyP(&map_pos, pPosition, &gCurrent_race.map_transformation);
    BrVector3Set(&dir,
        (float)(gHeadup_map_half_width + gINT_0068d890 + gUINT_0074ab8c) - map_pos.v[0],
        (float)(gHeadup_map_half_height + gINT_0068c858 + gUINT_0074ab88) - map_pos.v[1],
        0.f);
    dot = BrVector3LengthSquared(&dir);
    if (gINT_0068d6f4 == -1 || dot < gFLOAT_0074abc4) {
        gINT_0068d8b8 = 0;
        gINT_0068d6f4 = pIndex;
        gVector2_0068d8b0.v[0] = dir.v[0];
        gVector2_0068d8b0.v[1] = dir.v[1];
        gFLOAT_0074abc4 = dot;
    }
    if (gNet_mode == eNet_mode_none && (pTime & 0x100)) {
        if (gMap_view == 2) {
            PossibleLock(1);
        }
        DRPixelmapRectangleMaskedCopy(pScreen,
            (int)(map_pos.v[0] - gSmashy_dot->width / 2),
            (int)(map_pos.v[1] - gSmashy_dot->height / 2),
            gSmashy_dot,
            0,
            0,
            gSmashy_dot->width,
            gSmashy_dot->height);
        if (gMap_view == 2) {
            PossibleUnlock(1);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00495e10
void C2_HOOK_FASTCALL DoMapOverlays(br_pixelmap* pScreen) {
    tU32 the_time;
    tVehicle_type category;
    int car_count;
    tCar_spec* car;
    int ped_count;
    int powerup_count;
    tNet_game_player_info* net_player;
    int i;
    br_vector3 ped_position;
    br_vector3* smash_position;
    br_actor* actor;

    the_time = PDGetTotalTime();
    if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {
        if (gMap_view == 2) {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint, 1);
            DoSomeThingsToCheckpoints(pScreen, the_time, DrawCheckpoint, 1);
        }
        else {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint2, 1);
        }
    }
    if ((gShow_peds_on_map || gCurrent_race.race_spec->race_type == kRaceType_Peds)
            || (gNet_mode != eNet_mode_none && gCurrent_net_game->options.show_powerups_on_map)) {

        ped_count = GetPedCount();
        for (i = 0; i < ped_count; i++) {
            if (GetPedPosition(i, &ped_position)) {
                DrawMapSmallBlip(pScreen, the_time, &ped_position, 52);
            }
        }
    }

    if ((gNet_mode != eNet_mode_none && gCurrent_net_game->options.show_powerups_on_map)
            || gCurrent_race.race_spec->expansion) {

        powerup_count = GetPowerupCount();
        for (i = 0; i < powerup_count; i++) {
            GetPowerupPosition(i, &ped_position);
            DrawMapSmallBlip(pScreen, the_time, &ped_position, 4);
        }
    }
    for (category = gShow_opponents ? eVehicle_opponent : eVehicle_self; category >= (gMap_view == 1 ? eVehicle_net_player : eVehicle_self); category -= 1) {
        if (category == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(category);
        }
        for (i = 0; i < car_count; i++) {
            if (category == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(category, i);
            }
            if (gNet_mode != eNet_mode_none) {
                net_player = NetPlayerFromCar(car);
            }
            if (gNet_mode == eNet_mode_none
                    || (!car->knackered && !net_player->wasted && net_player->player_status >= ePlayer_status_racing)) {

                if (category == eVehicle_self) {
                    actor = gPlayer_car_master_actor;
                } else {
                    actor = GetCarSpec(category, i)->car_master_actor;
                }
                DrawCarArrow(pScreen, car, the_time, &actor->t.t.translate.t, category);
            }
        }
    }
    if (gCurrent_race.race_spec->race_type >= kRaceType_Smash) {
        for (i = 0; i < gCount_smashable_race_targets; i++) {
            tSmashable_race_target* smash_target = &gSmashable_race_targets[i];

            if ((smash_target->model != NULL && smash_target->actor->render_style != BR_RSTYLE_NONE && smash_target->actor->model == smash_target->model)
                    || (smash_target->field_0x8 >= 0 && gPedestrian_array[smash_target->field_0x8].hit_points > 0)) {
                if (smash_target->field_0x8 >= 0) {
                    smash_position = &gPedestrian_array[smash_target->field_0x8].pos;
                } else {
                    smash_position = &smash_target->actor->t.t.translate.t;
                }

                DrawSmashBlip(pScreen, the_time, smash_position, i);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00494900
void C2_HOOK_FASTCALL DoOpponentStatusHeadup(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00496b10
void C2_HOOK_FASTCALL CheckpointLine(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    int p2_x;
    int p2_y;

    if (pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count) {
        if (gCurrent_race.checkpoints[pCheckpoint].map_position.v[0] < 0.f) {
            p2_x = 8;
            p2_y = (int)((gCurrent_race.checkpoints[pCheckpoint].map_position.v[1] - (float)gINT_0074abd0)
                * ((float)gINT_0074abd4 / ((float)gINT_0074abd4 - gCurrent_race.checkpoints[pCheckpoint].map_position.v[0]))
                + (float)gINT_0074abd0);
        } else {
            p2_x = (int)(gCurrent_race.checkpoints[pCheckpoint].map_position.v[0] + 8.f);
            p2_y = (int)gCurrent_race.checkpoints[pCheckpoint].map_position.v[1];
        }
        FancyDrawLine(pMap, gINT_0074abd4 + 8, gINT_0074abd0, p2_x, p2_y, 132, 100);
    }
}

// FUNCTION: CARMA2_HW 0x00495d00
void C2_HOOK_FASTCALL DrawOffsetCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    // GLOBAL: CARMA2_HW 0x0068d8f0
    static tU32 last_flash;
    // GLOBAL: CARMA2_HW 0x0068d8f4
    static int flash_state;

    if ((pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count && gTrack_version >= 1 && !pTarget)
            || Flash(300, &last_flash, &flash_state)) {

        if (gCurrent_race.checkpoints[pCheckpoint].map_position.v[0] >= 0.f
                && gCurrent_race.checkpoints[pCheckpoint].map_position.v[0] < (float)pMap->width
                && gCurrent_race.checkpoints[pCheckpoint].map_position.v[1] >= 0.f
                && gCurrent_race.checkpoints[pCheckpoint].map_position.v[1] < (float)pMap->height) {

            DRPixelmapRectangleMaskedCopy(pMap,
                (int)(gCurrent_race.checkpoints[pCheckpoint].map_position.v[0] + 8.f - (float)gCheckpoint_digit_center_x),
                (int)(gCurrent_race.checkpoints[pCheckpoint].map_position.v[1] - (float)gCheckpoint_digit_center_y),
                gCheckpoint_numbers,
                0, pCheckpoint * gCheckpoint_digit_height,
                gCheckpoint_numbers->width, gCheckpoint_digit_height);
        }
    }
}

void C2_HOOK_FASTCALL DoDirectionFinderStuff(br_pixelmap* pMap) {
    tU32 the_time;
    const br_vector2* ptr_v;
    br_scalar tmp;
    br_vector2 tv;

    the_time = PDGetTotalTime();
    if ((gMini_map_visible || gMap_view == 2)
            && gCheckpoint_finder_enabled
            && (gCurrent_race.race_spec->race_type == kRaceType_Carma1 ||
                gCurrent_race.race_spec->race_type == kRaceType_Checkpoints)) {
        DoSomeThingsToCheckpoints(pMap, the_time, CheckpointLine, 0);
    }
    if (gCurrent_race.race_spec->race_type >= kRaceType_Smash) {
        if (gINT_0068d6f4 < 0) {
            return;
        }
        ptr_v = &gVector2_0068d8b0;
        tmp = gFLOAT_0074abc4;
    } else if ((gNet_mode != eNet_mode_none
            || gCurrent_race.race_spec->race_type == kRaceType_Carma1
            || gCurrent_race.race_spec->race_type == kRaceType_Cars) && gTarget_lock_car_2 != NULL) {
        ptr_v = &gVector2_0068d6d8;
        tmp = gFLOAT_0074ab90;
    } else {
        return;
    }
    if (tmp > gFLOAT_0074abb8) {
        BrMatrix23TApplyV(&tv, ptr_v, &gMatrix23_0068c880);
        tmp = sqrtf(tmp) - (float)gCurrent_graf_data->field_0x51c;
        BrVector2Normalise(&tv, &tv);
        if (fabsf(tmp * tv.v[0]) > (float)gINT_0074abd4) {
            tmp *= (float)gINT_0074abd4 / fabsf(tmp * tv.v[0]);
        }
        if (gMini_map_visible || gMap_view == 2) {
            FancyDrawLine(pMap,
                gINT_0074abd4 + 8, gINT_0074abd0,
                (int)((float)gINT_0074abd4 - tmp * tv.v[0] + 8.f),
                (int)((float)gINT_0074abd0 - tmp * tv.v[1]),
                52, 75);
        }
    }
}

void C2_HOOK_FASTCALL DoNearestOpponentRelativeheightMarkerStuff(br_pixelmap* pMap) {
    int half_height;
    int border_height;
    br_scalar delta_height;
    int ym;
    int yt;

    if ((gMini_map_visible || gMap_view == 2)
            && gTarget_lock_car_2 != NULL) {
//        y = (int)(((gPlayer_car_master_actor->t.t.translate.t.v[1] - gTarget_lock_car_2->pos.v[1]) *
//                (float)gCurrent_graf_data->field_0x518 * 0.5f * 0.46f);
        half_height = pMap->height / 2;
        border_height = (int)(pMap->height * 0.05f);
        BrPixelmapLine(pMap, 1, border_height, 3, border_height, RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 1, pMap->height - border_height, 3, pMap->height - border_height, RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 0, border_height + 1, 0, pMap->height - (border_height + 1), RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 4, border_height + 1, 4, pMap->height - (border_height + 1), RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 0, half_height, 4, half_height, RGB565_TO_BACKSCREEN_COLOUR(31, 62, 31));

        BrPixelmapPixelSet(pMap, 0, border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        BrPixelmapPixelSet(pMap, 4, border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        BrPixelmapPixelSet(pMap, 0, pMap->height - border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        BrPixelmapPixelSet(pMap, 4, pMap->height - border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));

        delta_height = gPlayer_car_master_actor->t.t.translate.t.v[1] - gTarget_lock_car_2->pos.v[1];
        if (fabsf(delta_height) > 14.9999997f / WORLD_SCALE) {
            delta_height = (float)sign((int)delta_height) * 14.9999997f / WORLD_SCALE;
        }
        ym = half_height;
        if (fabsf(delta_height * 0.46f) >= 0.05f) {
            ym = (int)((float)(half_height - border_height) * delta_height * 0.46f + (float)half_height);
        }
        yt = border_height + 1;
        if (ym > border_height) {
            yt = ym;
            if (ym >= pMap->height - border_height) {
                yt = pMap->height - border_height - 1;
            }
        }
        if (yt < half_height) {
            BrPixelmapLine(pMap, 1, yt, 1, half_height - 1, RGB565_TO_BACKSCREEN_COLOUR(0, 38, 31));
            BrPixelmapLine(pMap, 2, yt, 2, half_height - 1, RGB565_TO_BACKSCREEN_COLOUR(6, 0, 31));
            BrPixelmapLine(pMap, 3, yt, 3, half_height - 1, 0x15);
        } else if (yt > half_height) {
            BrPixelmapLine(pMap, 1, half_height + 1, 1, yt, RGB565_TO_BACKSCREEN_COLOUR(0, 38, 31));
            BrPixelmapLine(pMap, 2, half_height + 1, 2, yt, RGB565_TO_BACKSCREEN_COLOUR(6, 0, 31));
            BrPixelmapLine(pMap, 3, half_height + 1, 3, yt, 0x15);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004950b0
void C2_HOOK_FASTCALL MapOverlay(void) {
    tU32 the_time;
    br_uint_16 original_base_x;
    br_matrix23 mat23;
    br_vector3 player_pos;

    BrMatrix34ApplyP(&gOrigin_map, &gPlayer_car_master_actor->t.t.translate.t,  &gCurrent_race.map_transformation);
    gOrigin_headup_map.v[0] = gOrigin_map.v[0] - (float)(gINT_0074ab94 / 2);
    gOrigin_headup_map.v[1] = gOrigin_map.v[1] - (float)(gINT_0074abec / 2);
    gINT_0068c878 = (int)(gOrigin_headup_map.v[0] + .5f);
    gINT_0068c874 = (int)(gOrigin_headup_map.v[1] + .5f);
    if (gINT_0068c878 < 0 || gINT_0068c874 < 0
            || gCurrent_race.map_image->width < gINT_0068c878
            || gCurrent_race.map_image->height < gINT_0068c874) {
        BrPixelmapFill(gMap_overlay, 0);
    }
    if (gINT_0074abec < gINT_0074ab94) {
        gINT_0068c858 = (gINT_0074ab94 - gINT_0074abec) / 2;
        gINT_0068d890 = 0;
        BrPixelmapRectangleCopy(gMap_overlay,
            0, 0,
            gCurrent_race.map_image,
            gINT_0068c878, gINT_0068c874 - gINT_0068c858,
            gINT_0074ab94, gINT_0074ab94);
    } else {
        gINT_0068c858 = 0;
        gINT_0068d890 = (gINT_0074abec - gINT_0074ab94) / 2;
        BrPixelmapRectangleCopy(gMap_overlay,
            0, 0,
            gCurrent_race.map_image,
            gINT_0068c878 - gINT_0068d890, gINT_0068c874,
            gINT_0074abec, gINT_0074abec);
    }
    gCurrent_race.map_transformation.m[3][0] -= gOrigin_headup_map.v[0] - (float)gINT_0068d890;
    gCurrent_race.map_transformation.m[3][1] -= gOrigin_headup_map.v[1] - (float)gINT_0068c858;
    BrMatrix34Mul(&gCar_in_map_space,
        &gProgram_state.current_car.car_master_actor->t.t.mat,
        &gCurrent_race.map_transformation);
    mat23.m[0][0] = gCar_in_map_space.m[0][0];
    mat23.m[0][1] = gCar_in_map_space.m[0][1];
    mat23.m[1][0] = gCar_in_map_space.m[2][0];
    mat23.m[1][1] = gCar_in_map_space.m[2][1];
    mat23.m[2][0] = 0.f;
    mat23.m[2][1] = 0.f;
    BrMatrix23LPNormalise(&gMatrix23_0068c880, &mat23);
    DoMapOverlays(gMap_overlay);
    gCurrent_race.map_transformation.m[3][0] += gOrigin_headup_map.v[0] - (float)gINT_0068d890;
    gCurrent_race.map_transformation.m[3][1] += gOrigin_headup_map.v[1] - (float)gINT_0068c858;
    the_time = PDGetTotalTime();
    PossibleLock(1);
    if (gMini_map_visible && gMap_overlay != NULL) {
        DRPixelmapRotatedAndFeatheredCopy(&gMatrix23_0068c880,
            gBack_screen,
            gHeadup_map_x + 8, gHeadup_map_y,
            gMap_overlay,
            gINT_0068d890 + gUINT_0074ab8c, gINT_0068c858 + gUINT_0074ab88,
            gHeadup_map_w, gHeadup_map_h,
            gMap_trans);
    }
    InitMap();

    original_base_x = gMini_map->base_x;
    gMini_map->base_x = gMini_map->base_x / 2;
    DoDirectionFinderStuff(gMini_map);
    gMini_map->base_x = original_base_x;
    if (gMini_map_visible) {
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1
                || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {

            DoSomeThingsToCheckpoints(gMini_map, the_time, DrawOffsetCheckpoint, 1);
        }
        if (gMini_map->type == BR_PMT_INDEX_8) {
            gMini_map->base_x = 0;
        } else {
            gMini_map->base_x /= 2;
        }
        player_pos.v[0] = (float)(gHeadup_map_w / 2 + gHeadup_map_x + 8);
        player_pos.v[1] = (float)(gHeadup_map_h / 2 + gHeadup_map_y);
        player_pos.v[2] = gMini_map_arrow_z;
        DrawArrow(gBack_screen, 0, &player_pos, CarArrowColour(&gProgram_state.current_car, eVehicle_self));
    }
    DoNearestOpponentRelativeheightMarkerStuff(gMini_map);
    gMini_map->base_x = original_base_x;
    PossibleUnlock(1);
    if (gHeadup_detail_level > 2) {
        DoOpponentStatusHeadup();
    }
}

void C2_HOOK_FASTCALL FinishMap(void) {

    gBack_screen->origin_x = gBack_original_origin_x;
    gBack_screen->origin_y = gBack_original_origin_y;
    gBack_screen->base_x = gBack_original_base_x;
    gBack_screen->base_y = gBack_original_base_y;
}

// FUNCTION: CARMA2_HW 0x00496d80
void C2_HOOK_FASTCALL MapStuffAfterRender(void) {
    if (gCurrent_race.map_image == NULL) {
        return;
    }
    if (gMap_view == 2) {
        BrVector2Set(&gOrigin_headup_map, 0.f, 0.f);
        DoMapOverlays(gBack_screen);
        FinishMap();
    }
    else if (gMap_view == 1) {
      if (!gAction_replay_mode
                && !(gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox)) {

            MapOverlay();
        }
    }
}

void C2_HOOK_FASTCALL CleanPolyFontDangler(void) {
    int i;
    br_actor* a;

    for (i = 0; i < gCount_polyfont_glyph_actors; i++) {
        a = gPolyfont_glyph_actors[i];
        if (a->parent != NULL) {
            BrActorRemove(a);
        }
    }
    gCount_polyfont_glyph_actors = 0;
}

// FUNCTION: CARMA2_HW 0x004e5b00
void C2_HOOK_FASTCALL StopRenderingHeadups(void) {
    int original_origin_x;
    int original_origin_y;
    int original_base_x;
    int original_base_Y;
    int i;
    br_actor* a;

    if (gCount_polyfont_glyph_actors != 0) {
        BrActorAdd(gHUD_root, gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        a = gHud_actor_storage[i];
        if (a != NULL && a->prev == NULL) {
            BrActorAdd(gHUD_root, a);
        }
    }
    original_origin_x = gRender_screen->origin_x;
    original_origin_y = gRender_screen->origin_y;
    original_base_x = gRender_screen->base_x;
    original_base_Y = gRender_screen->base_y;
    gRender_screen->origin_x = 0;
    gRender_screen->origin_y = 0;
    gRender_screen->base_x = 0;
    gRender_screen->base_y = 0;
    BrZbSceneRender(gHUD_root, gHUD_camera, gRender_screen, gDepth_buffer);
    gRender_screen->origin_x = original_origin_x;
    gRender_screen->origin_y = original_origin_y;
    gRender_screen->base_x = original_base_x;
    gRender_screen->base_y = original_base_Y;
    if (gString_root_actor->parent != NULL) {
        BrActorRemove(gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        a = gHud_actor_storage[i];
        if (a != NULL && a->parent != NULL) {
            BrActorRemove(a);
        }
    }
    gHud_actor_storage_size = 0;
    CleanPolyFontDangler();
    gRender_poly_text = 1;
}
