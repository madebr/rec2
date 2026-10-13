#include "15-displays.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrbm.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0067c500
tHeadup gHeadups[37];

// GLOBAL: CARMA2_HW 0x0067f888
int gQueued_headup_count;

// GLOBAL: CARMA2_HW 0x0067f890
tQueued_headup gQueued_headups[4];

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

// ClearHeadup

// ClearHeadupSlot

// ClearHeadups

// DimAFewBits

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void) {
    if (gQueued_headup_count != 0) {
        gQueued_headup_count--;
        memmove(&gQueued_headups[0], &gQueued_headups[1], gQueued_headup_count * sizeof(tQueued_headup));
    }
}

// DoTestPowerHeadup

// DoPSPowerupHeadups

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

// IsHeadupTextClever

// MungeHeadupWidth

// NewTextHeadupSlot2

// NewTextHeadupSlot

// NewImageHeadupSlot

// DoFancyHeadup

// MoveHeadupTo

// ChangeHeadupText

// DoDamageScreen

// DoInstruments

// ChangingView

// EarnCredits2

// EarnCredits

// EarnCredits100

// SpendCredits

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

