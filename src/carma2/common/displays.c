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

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tHeadup, gHeadups, 37, 0x0067c500);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tQueued_headup, gQueued_headups, 4, 0x0067f890);
C2_HOOK_VARIABLE_IMPLEMENT(int, gQueued_headup_count, 0x0067f888);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_centre_headup, 0x0067fcf8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gDRFont_to_polyfont_mapping, 24, 0x0059ada0, {
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
});
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gAcc_poly_material, 0x00686178);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gAcc_poly_actor, 0x00686174);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gAcc_poly_model, 0x0068617c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRender_acc_poly_actor, 0x00686180);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_token_value, gAccent_poly_prims, 3, 0x005962f8, {
    {
        BRT_BLEND_B,
        { .b = 1 },
    }, {
        BRT_OPACITY_X,
        { .x = 0x4b0000 },
    }, {
        BR_NULL_TOKEN,
        { .u32 = 0 },
    },
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gCredits_won_headup, 0x0079eaa8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPed_kill_count_headup, 0x0079eaf4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCar_kill_count_headup, 0x0079eaf8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimer_headup, 0x0079eaec);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTime_awarded_headup, 0x0079eb40);
C2_HOOK_VARIABLE_IMPLEMENT(int, gLaps_headup, 0x0079eaf0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_cash_headup, 0x0079eae8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_ped_headup, 0x0079eaa4);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gLast_fancy_index, 0x005913d4, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gLast_credit_headup__displays, 0x005913d8, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gLast_time_credit_headup, 0x005913dc, -1);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_earn_time, 0x0067f874);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_fancy_time, 0x0067fd34);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_time_earn_time, 0x0067fcc4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gOld_times, 10, 0x0079eac0);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_font*, gBR_fonts, 4, 0x0067fcd8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gRevs, 13, 0x0067fd00);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gArmour_colour_map, 0x0074cf84);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPowerbar, 0x0067fcd0);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gPower_colour_map, 0x0074cab4);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gOffensive_colour_map, 0x0074cf5c);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gIcon_test, 0x006815bc);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gCurrent_rev, 0x0074ca1c);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gHud_curve1, 0x0067c4f8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gHud_curve2, 0x0067fcfc);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gHud_curve3, 0x0067f87c);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gGrey_top5, 0x0067fcf4);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gDamage_hud, 0x0074cf00);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gTimer_lhs, 0x0074ca80);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gHud_timer_rhs, 0x0074ca44);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gIcon_armour, 0x006815b8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gIcon_power, 0x006815b4);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gIcon_offense, 0x0067c4f0);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gIcon_greyBloc1, 0x0067c4f4);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gIcon_litBloc1, 0x0067fce8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gIcon_grnBlock1, 0x0067fcd4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tHud_message, gHud_messages, 46, 0x0067fd40);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCredit_multiplier, 0x00705be0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gLast_credit_amount, 0x0067f878);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_fancy_headup, 0x0067fcc0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gPrev_ps_apo_levels, 3, 0x005913ec, { -1, -1, -1});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gPrev_ps_drawn_levels, 3, 0x005913f8, { -1, -1, -1});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gPS_apo_level_changed, 3, 0x005913e0, { 1, 1, 1});
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix23, gMatrix23_0068c880, 0x0068c880);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gOrigin_map, 0x0068c868);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const int, gCar_map_colours, 4, 0x00659b40, {
    4, 0, 0x34, 0x84,
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0068d6f4, 0x0068d6f4);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_0074abc4, 0x0074abc4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0068d8b8, 0x0068d8b8);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector2, gVector2_0068d8b0, 0x0068d8b0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0068c874, 0x0068c874);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0068c878, 0x0068c878);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector2, gOrigin_headup_map, 0x0068d898);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBack_original_origin_x, 0x0068d6ec);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBack_original_origin_y, 0x0068d6e8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBack_original_base_x, 0x0068d88c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBack_original_base_y, 0x0068d888);

int (C2_HOOK_FASTCALL * DRTextWidth_original)(const tDR_font* pFont, const char* pText);
int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText) {

#if 0//defined(C2_HOOKS_ENABLED)
    return DRTextWidth_original(pFont, pText);
#else

    return PolyFontTextWidth(C2V(gDRFont_to_polyfont_mapping)[pFont->id], pText);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00465d50, DRTextWidth, DRTextWidth_original)

int C2_HOOK_FASTCALL DRTextCleverWidth(const tDR_font* pFont, const char* pText) {
    int polyfont;
    int i;
    int len;
    int result;

    polyfont = C2V(gDRFont_to_polyfont_mapping)[pFont->id];
    result = 0;
    len = c2_strlen(pText);

    for (i = 0; i < len; i++) {

        if ((tS8)pText[i] < 0) {
            polyfont = C2V(gDRFont_to_polyfont_mapping)[C2V(gFonts)[-(tS8) pText[i]].id];
        } else {
            int inter = 0;
            if (i < len - 1) {
                inter = C2V(gPolyFonts)[polyfont].interCharacterSpacing;
            }

            result += CharacterWidth(pText[i], polyfont) + inter;
        }
    }
    return result;
}
C2_HOOK_FUNCTION(0x00465e10, DRTextCleverWidth)

void C2_HOOK_FASTCALL DRPixelmapCentredText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText) {
    int width_over_2;

    width_over_2 = DRTextWidth(pFont, pText) / 2;
    TransDRPixelmapText(pPixelmap, pX - width_over_2, pY, pFont, pText, width_over_2 + pX);
}
C2_HOOK_FUNCTION(0x00465f10, DRPixelmapCentredText)

void C2_HOOK_FASTCALL OoerrIveGotTextInMeBoxMissus(int pFont_index, const char* pText, br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pCentred) {
    int polyfont;

    polyfont = C2V(gDRFont_to_polyfont_mapping)[C2V(gFonts)[pFont_index].id];
    RenderPolyText(polyfont, pText, pLeft, pTop, pRight, pBottom, pCentred ? eJust_centre : eJust_left, C2V(gRender_poly_text));
}
C2_HOOK_FUNCTION(0x00466000, OoerrIveGotTextInMeBoxMissus)

int C2_HOOK_FASTCALL DRFontToPolyFontHandle(const tDR_font* pFont) {

    return C2V(gDRFont_to_polyfont_mapping)[pFont->id];
}
C2_HOOK_FUNCTION(0x00466210, DRFontToPolyFontHandle)

void (C2_HOOK_FASTCALL *TransDRPixelmapText_original)(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
void C2_HOOK_FASTCALL TransDRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge) {

#if 0//defined(C2_HOOKS_ENABLED)
    TransDRPixelmapText_original(pPixelmap, pX, pY, pFont, pText, pRight_edge);
#else

    RenderPolyTextLine(pText, pX, pY, C2V(gDRFont_to_polyfont_mapping)[pFont->id], eJust_left, C2V(gRender_poly_text));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00465a70, TransDRPixelmapText, TransDRPixelmapText_original)

void C2_HOOK_FASTCALL LoadHeadupMessageFile(void) {
    FILE* f;
    tPath_name path;
    int i;
    int count;

    PathCat(path, C2V(gApplication_path), "SCROLLERS.TXT");
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
    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gHud_messages)) != 46);

    if (count > REC2_ASIZE(C2V(gHud_messages))) {
        BrFailure("Error - Too many Headup messages ", path);
    }
    for (i = 0; i < count; i++) {
        char s[256];
        char *str;
        tHud_message *hud_message;
        int len;
        int j;

        hud_message = &C2V(gHud_messages)[i];
        GetALineAndDontArgue(f, s);

        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%d", &hud_message->font1);

        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &hud_message->font2);

        str = c2_strtok(NULL, "\t ,/");
        c2_strcpy(hud_message->message, str);

        len = c2_strlen(hud_message->message);
        for (j = 0; j < len; j++) {
            if (hud_message->message[j] == '_') {
                hud_message->message[j] = ' ';
            }
        }
    }
    PFfclose(f);
}

void (C2_HOOK_FASTCALL * InitHeadups_original)(void);
void C2_HOOK_FASTCALL InitHeadups(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitHeadups_original();
#else
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gHeadups)) != 37);
    for (i = 0; i < REC2_ASIZE(C2V(gHeadups)); i++) {
        C2V(gHeadups)[i].type = eHeadup_unused;
    }

    C2V(gBR_fonts)[0] = C2V(BrFontProp4x6);
    C2V(gBR_fonts)[1] = C2V(BrFontProp7x9);
    C2V(gBR_fonts)[2] = C2V(gFont_7);
    C2V(gBR_fonts)[3] = C2V(gHeadup_font);
    C2V(gRevs)[ 0] = LoadPixelmap("rev1.tif");
    C2V(gRevs)[ 1] = LoadPixelmap("rev2.tif");
    C2V(gRevs)[ 2] = LoadPixelmap("rev3.tif");
    C2V(gRevs)[ 3] = LoadPixelmap("rev4.tif");
    C2V(gRevs)[ 4] = LoadPixelmap("rev5.tif");
    C2V(gRevs)[ 5] = LoadPixelmap("rev6.tif");
    C2V(gRevs)[ 6] = LoadPixelmap("rev7.tif");
    C2V(gRevs)[ 7] = LoadPixelmap("rev8.tif");
    C2V(gRevs)[ 8] = LoadPixelmap("rev9.tif");
    C2V(gRevs)[ 9] = LoadPixelmap("rev10.tif");
    C2V(gRevs)[10] = LoadPixelmap("rev11.tif");
    C2V(gRevs)[11] = LoadPixelmap("rev12.tif");
    C2V(gRevs)[12] = LoadPixelmap("rev13.tif");
    C2V(gPowerbar) = LoadPixelmap("powerbar.tif");

    C2V(gArmour_colour_map) = BrPixelmapAllocate(BR_PMT_RGBA_4444,
        C2V(gPowerbar)->width, C2V(gPowerbar)->height,
        NULL, 0);
    C2V(gArmour_colour_map)->identifier = BrResStrDup(C2V(gArmour_colour_map), "Armour_area");
    BrMapAdd(C2V(gArmour_colour_map));

    C2V(gPower_colour_map) = LoadPixelmap("powerbar.tif");
    BrMapAdd(C2V(gPower_colour_map));

    C2V(gOffensive_colour_map) = LoadPixelmap("powerbar.tif");
    BrMapAdd(C2V(gOffensive_colour_map));

    C2V(gIcon_test) = LoadPixelmap("icontest.tif");

    C2V(gCurrent_rev) = C2V(gRevs)[0];
    BrMapAdd(C2V(gRevs)[0]);

    C2V(gHud_curve1) = LoadPixelmap("hudcurve1.tif");
    BrMapAdd(C2V(gHud_curve1));
    C2V(gHud_curve2) = LoadPixelmap("hudcurve2.tif");
    BrMapAdd(C2V(gHud_curve2));
    C2V(gHud_curve3) = LoadPixelmap("hudcurve3.tif");
    BrMapAdd(C2V(gHud_curve3));

    C2V(gGrey_top5) = LoadPixelmap("greytop5.tif");
    C2V(gDamage_hud) = BrPixelmapAllocate(BR_PMT_RGBA_4444,
        C2V(gGrey_top5)->width, C2V(gGrey_top5)->height,
        NULL, 0);
    BrMapAdd(C2V(gDamage_hud));
    BrMapUpdate(C2V(gDamage_hud), BR_MAPU_ALL);

    C2V(gStatbarHUD5_material)->colour_map = C2V(gDamage_hud);
    BrMaterialUpdate(C2V(gStatbarHUD5_material), BR_MATU_COLOURMAP);

    C2V(gTimer_lhs) = LoadPixelmap("timerlhs.tif");
    BrMapAdd(C2V(gTimer_lhs));
    C2V(gTimerLeftHUD_material)->colour_map = C2V(gTimer_lhs);
    BrMaterialUpdate(C2V(gTimerLeftHUD_material), BR_MATU_COLOURMAP);
    C2V(gTimerLeftHUD_model)->vertices[1].p.v[0] = (float)C2V(gTimerLeftHUD_dim_x);
    C2V(gTimerLeftHUD_model)->vertices[0].p.v[0] = C2V(gTimerLeftHUD_model)->vertices[1].p.v[0];
    C2V(gTimerLeftHUD_model)->vertices[3].p.v[1] = (float)-C2V(gTimerLeftHUD_dim_y);
    C2V(gTimerLeftHUD_model)->vertices[0].p.v[1] = C2V(gTimerLeftHUD_model)->vertices[3].p.v[1];
    C2V(gTimerLeftHUD_model)->vertices[3].p.v[0] = (float)C2V(gTimerLeftHUD_dim_w) + C2V(gTimerLeftHUD_model)->vertices[1].p.v[0];
    C2V(gTimerLeftHUD_model)->vertices[2].p.v[0] = C2V(gTimerLeftHUD_model)->vertices[3].p.v[0];
    C2V(gTimerLeftHUD_model)->vertices[2].p.v[1] = C2V(gTimerLeftHUD_model)->vertices[3].p.v[1] - (float)C2V(gTimerLeftHUD_dim_h);
    C2V(gTimerLeftHUD_model)->vertices[1].p.v[1] = C2V(gTimerLeftHUD_model)->vertices[2].p.v[1];
    BrModelUpdate(C2V(gTimerLeftHUD_model), BR_MODU_VERTEX_POSITIONS);

    C2V(gHud_timer_rhs) = LoadPixelmap("timerrhs.tif");
    BrMapAdd(C2V(gHud_timer_rhs));
    C2V(gTimerRightHUD_material)->colour_map = C2V(gHud_timer_rhs);
    BrMaterialUpdate(C2V(gTimerRightHUD_material), BR_MATU_COLOURMAP);

    C2V(gTimerRightHUD_model)->vertices[1].p.v[0] = (float)C2V(gTimerRightHUD_dim_x);
    C2V(gTimerRightHUD_model)->vertices[0].p.v[0] = C2V(gTimerRightHUD_model)->vertices[1].p.v[0];
    C2V(gTimerRightHUD_model)->vertices[3].p.v[1] = (float)-C2V(gTimerRightHUD_dim_y);
    C2V(gTimerRightHUD_model)->vertices[0].p.v[1] = C2V(gTimerRightHUD_model)->vertices[3].p.v[1];
    C2V(gTimerRightHUD_model)->vertices[3].p.v[0] = (float)C2V(gTimerRightHUD_dim_w) + C2V(gTimerRightHUD_model)->vertices[1].p.v[0];
    C2V(gTimerRightHUD_model)->vertices[2].p.v[0] = C2V(gTimerRightHUD_model)->vertices[3].p.v[0];
    C2V(gTimerRightHUD_model)->vertices[2].p.v[1] = C2V(gTimerRightHUD_model)->vertices[3].p.v[1] - (float)C2V(gTimerRightHUD_dim_h);
    C2V(gTimerRightHUD_model)->vertices[1].p.v[1] = C2V(gTimerRightHUD_model)->vertices[2].p.v[1];
    BrModelUpdate(C2V(gTimerRightHUD_model), BR_MODU_VERTEX_POSITIONS);

    C2V(gIcon_armour) = LoadPixelmap("iconarmour.tif");
    C2V(gIcon_power) = LoadPixelmap("iconpower.tif");
    C2V(gIcon_offense) = LoadPixelmap("iconoffense.tif");
    C2V(gIcon_greyBloc1) = LoadPixelmap("greybloc1.tif");
    C2V(gIcon_litBloc1) = LoadPixelmap("litbloc1.tif");
    C2V(gIcon_grnBlock1) = LoadPixelmap("grnblock1.tif");

    LoadHeadupMessageFile();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00449090, InitHeadups, InitHeadups_original)

void C2_HOOK_FASTCALL ClearHeadup(int pIndex) {

    C2V(gHeadups)[pIndex].type = eHeadup_unused;
}
C2_HOOK_FUNCTION(0x00449630, ClearHeadup)

void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index) {
    int i;
    tHeadup* the_headup;

    for (i = 0; i < REC2_ASIZE(C2V(gHeadups)); i++) {
        the_headup = &C2V(gHeadups)[i];
        if (the_headup->type != eHeadup_unused && the_headup->slot_index == pSlot_index) {
            ClearHeadup(i);
            return;
        }
        the_headup++;
    }
}
C2_HOOK_FUNCTION(0x00449650, ClearHeadupSlot)

void C2_HOOK_FASTCALL ClearHeadups(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gHeadups)) != 37);
    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gOld_times)) != 10);

    for (i = 0; i < REC2_ASIZE(C2V(gHeadups)); i++) {
        if (C2V(gHeadups)[i].type != eHeadup_unused) {
            ClearHeadup(i);
        }
    }
    C2V(gLast_fancy_index) = -1;
    C2V(gLast_credit_headup__displays) = -1;
    C2V(gLast_time_credit_headup) = -1;
    C2V(gLast_earn_time) = 0;
    for (i = 0; i < REC2_ASIZE(C2V(gOld_times)); i++) {
        C2V(gOld_times)[i] = 0;
    }
    C2V(gLast_fancy_time) = 0;
    C2V(gLast_time_earn_time) = 0;
    C2V(gQueued_headup_count) = 0;
    C2V(gLast_centre_headup) = 0;
}
C2_HOOK_FUNCTION(0x00449690, ClearHeadups)

int C2_HOOK_FASTCALL IsHeadupTextClever(char* pText) {

    while (*pText) {
        if (*(signed char*)pText < 0) {
            return 1;
        }
        pText++;
    }
    return 0;
}

int (C2_HOOK_FASTCALL * MungeHeadupWidth_original)(tHeadup* pHeadup);
int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup) {

    C2_HOOK_BUG_ON(sizeof(tHeadup) != 356);

#if 0//defined(C2_HOOKS_ENABLED)
    return MungeHeadupWidth_original(pHeadup);
#else
    int width;

    C2_HOOK_BUG_ON((int)&((tHeadup*)0)->data.text_info.text != 0x4c);
    C2_HOOK_BUG_ON((int)&((tHeadup*)0)->data.coloured_text_info.coloured_font != 0x148);

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
            width = BrPixelmapTextWidth(C2V(gBack_screen), pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width;
        } else if (pHeadup->justification == eJust_centre) {
            width = BrPixelmapTextWidth(C2V(gBack_screen), pHeadup->data.text_info.font, pHeadup->data.text_info.text);
            pHeadup->x = pHeadup->original_x - width / 2;
        }
    }
    return width;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044a220, MungeHeadupWidth, MungeHeadupWidth_original)

int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index) {
    int i;
    int empty_one;
    tHeadup* the_headup;

    empty_one = -1;
    for (i = 0; i < REC2_ASIZE(C2V(gHeadups)); i++) {
        the_headup = &C2V(gHeadups)[i];
        if (pSlot_index >= 0 && the_headup->slot_index == pSlot_index) {
            return i;
        }
        if (the_headup->type == eHeadup_unused) {
            empty_one = i;
        }
    }
    return empty_one;
}
C2_HOOK_FUNCTION(0x00449fa0, FindAHeadupHoleWoofBarkSoundsABitRude)

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void) {

    C2_HOOK_BUG_ON(sizeof(tQueued_headup) != 0x10c);

    C2V(gQueued_headup_count) -= 1;
    c2_memmove(&C2V(gQueued_headups)[0], &C2V(gQueued_headups)[1], C2V(gQueued_headup_count) * sizeof(tQueued_headup));
}

void C2_HOOK_FASTCALL ClearQueuedHeadups(void) {

    while (C2V(gQueued_headup_count) != 0) {
        KillOldestQueuedHeadup();
    }
}
C2_HOOK_FUNCTION(0x004497b0, ClearQueuedHeadups)

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
        dest_pm = C2V(gArmour_colour_map);
        icon = C2V(gIcon_armour);
        count_slots = C2V(gProgram_state).current_car.power_up_slots[0];
        if (pLevel != C2V(gPrev_ps_apo_levels)[0]
                || C2V(gProgram_state).current_car.power_up_slots[0] != C2V(gPrev_ps_drawn_levels)[0]) {
            C2V(gPS_apo_level_changed)[0] = 1;
        } else {
            C2V(gPS_apo_level_changed)[0] = 1;
        }
        C2V(gPrev_ps_drawn_levels)[0] = C2V(gProgram_state).current_car.power_up_slots[0];
        C2V(gPrev_ps_apo_levels)[0] = pLevel;
        changed = C2V(gPS_apo_level_changed)[0];
        break;
    case 'P':
        dest_pm = C2V(gPower_colour_map);
        icon = C2V(gIcon_power);
        count_slots = C2V(gProgram_state).current_car.power_up_slots[1];
        if (pLevel != C2V(gPrev_ps_apo_levels)[1]
                || C2V(gProgram_state).current_car.power_up_slots[1] != C2V(gPrev_ps_drawn_levels)[1]) {
            C2V(gPS_apo_level_changed)[1] = 1;
        } else {
            C2V(gPS_apo_level_changed)[1] = 1;
        }
        C2V(gPrev_ps_drawn_levels)[1] = C2V(gProgram_state).current_car.power_up_slots[1];
        C2V(gPrev_ps_apo_levels)[1] = pLevel;
        changed = C2V(gPS_apo_level_changed)[1];
        break;
    case 'O':
        dest_pm = C2V(gOffensive_colour_map);
        icon = C2V(gIcon_offense);
        count_slots = C2V(gProgram_state).current_car.power_up_slots[2];
        if (pLevel != C2V(gPrev_ps_apo_levels)[2]
                || C2V(gProgram_state).current_car.power_up_slots[2] != C2V(gPrev_ps_drawn_levels)[2]) {
            C2V(gPS_apo_level_changed)[2] = 1;
        } else {
            C2V(gPS_apo_level_changed)[2] = 1;
        }
        C2V(gPrev_ps_drawn_levels)[2] = C2V(gProgram_state).current_car.power_up_slots[2];
        C2V(gPrev_ps_apo_levels)[2] = pLevel;
        changed = C2V(gPS_apo_level_changed)[2];
        break;
    }
    if (changed) {
        DRPixelmapCopy(dest_pm, C2V(gPowerbar));
        DRPixelmapRectangleMaskedCopy(dest_pm, delta_x, 0, icon, 0, 0, icon->width, icon->height);
        count_slots = count_slots - pLevel;
        level = pLevel;

        /* Draw staggered APO levels */
        for (i = 0; i < 15; i += 5) {
            for (j = 0; j < 10; j++) {
                delta = delta_x;
                if (level > 0) {
                    DRPixelmapRectangleMaskedCopy(dest_pm,
                        j * C2V(gIcon_litBloc1)->width + delta + icon->width + 2, i,
                        C2V(gIcon_litBloc1), 0, 0, C2V(gIcon_litBloc1)->width, C2V(gIcon_litBloc1)->height);
                    level -= 1;
                } else if (count_slots > 0) {
                    DRPixelmapRectangleMaskedCopy(dest_pm,
                        j * C2V(gIcon_grnBlock1)->width + delta + icon->width + 2, i,
                        C2V(gIcon_grnBlock1), 0, 0, C2V(gIcon_grnBlock1)->width, C2V(gIcon_grnBlock1)->height);
                    count_slots -= 1;
                } else {
                    DRPixelmapRectangleMaskedCopy(dest_pm,
                      j * C2V(gIcon_greyBloc1)->width + delta + icon->width + 2, i,
                      C2V(gIcon_greyBloc1), 0, 0, C2V(gIcon_greyBloc1)->width, C2V(gIcon_greyBloc1)->height);
                }
            }
            delta -= 2;
        }
        switch (pAPO_str[0]) {
        case 'A':
            BrMapUpdate(C2V(gArmour_colour_map), BR_MAPU_ALL);
            C2V(gArmour_material)->colour_map = C2V(gArmour_colour_map);
            BrMaterialUpdate(C2V(gArmour_material), BR_MATU_COLOURMAP);
            break;
        case 'P':
            BrMapUpdate(gPower_colour_map, BR_MAPU_ALL);
            C2V(gPower_material)->colour_map = gPower_colour_map;
            BrMaterialUpdate(C2V(gPower_material), BR_MATU_COLOURMAP);
            break;
        case 'O':
            BrMapUpdate(gOffensive_colour_map, BR_MAPU_ALL);
            C2V(gOffence_material)->colour_map = gOffensive_colour_map;
            BrMaterialUpdate(C2V(gOffence_material), BR_MATU_COLOURMAP);
            break;
        }
    }
    C2V(gArmour_actor)->render_style = BR_RSTYLE_FACES;
    C2V(gPower_actor)->render_style = BR_RSTYLE_FACES;
    C2V(gOffense_actor)->render_style = BR_RSTYLE_FACES;
}
C2_HOOK_FUNCTION(0x00449830, DoPSPowerHeadup)

void C2_HOOK_FASTCALL DoPSPowerupHeadups(void) {

    DoPSPowerHeadup(C2V(gProgram_state).current_car.power_up_levels[0], "A");
    DoPSPowerHeadup(C2V(gProgram_state).current_car.power_up_levels[1], "P");
    DoPSPowerHeadup(C2V(gProgram_state).current_car.power_up_levels[2], "O");
}
C2_HOOK_FUNCTION(0x004497b0, DoPSPowerupHeadups)

int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it) {
    int index;
    tHeadup* the_headup;
    tHeadup_slot* headup_slot;
    tU32 time;

    time = PDGetTotalTime();
    if (pQueue_it && pSlot_index == 4 && time - C2V(gLast_centre_headup) < 1000) {
        if (C2V(gQueued_headup_count) == 4) {
            KillOldestQueuedHeadup();
        }
        C2V(gQueued_headups)[C2V(gQueued_headup_count)].flash_rate = pFlash_rate;
        C2V(gQueued_headups)[C2V(gQueued_headup_count)].lifetime = pLifetime;
        C2V(gQueued_headups)[C2V(gQueued_headup_count)].font_index = pFont_index;
        c2_strcpy(C2V(gQueued_headups)[C2V(gQueued_headup_count)].text, pText);
        C2V(gQueued_headup_count)++;
        return -1;
    }
    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index < 0) {
        return index;
    }
    if (pSlot_index == 4) {
        C2V(gLast_centre_headup) = time;
    }
    headup_slot = &C2V(gProgram_state).current_car.headup_slots[C2V(gProgram_state).cockpit_on][pSlot_index];
    the_headup = &C2V(gHeadups)[index];
    the_headup->data.coloured_text_info.coloured_font = &C2V(gFonts)[-pFont_index];
    if (pSlot_index == 4) {
        the_headup->type = eHeadup_box_text;
    } else {
        the_headup->type = eHeadup_coloured_text;
    }
    c2_strcpy(the_headup->data.coloured_text_info.text, pText);

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
C2_HOOK_FUNCTION(0x00449fd0, NewTextHeadupSlot2)

int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText) {

    return NewTextHeadupSlot2(pSlot_index, pFlash_rate, pLifetime, pFont_index, pText, 1);
}
C2_HOOK_FUNCTION(0x0044a380, NewTextHeadupSlot)

void C2_HOOK_FASTCALL TransDRPixelmapCleverText(br_pixelmap* pPixelmap, int pX, int pY, tDR_font* pFont, const char* pText, int pRight_edge) {
    int i;
    char s[512];
    int s_end;
    int x;
    int next_x;
    int len;

    len = c2_strlen(pText);
    s_end = -1;
    x = pX;
    next_x = pX;
    for (i = 0; i < len; i++) {
        tS8 ch = pText[i];

        if (ch < 0) {

            if (s_end >= 0) {
                s[s_end + 1] = '\0';
                RenderPolyTextLine(s, x, pY, C2V(gDRFont_to_polyfont_mapping)[pFont->id], eJust_left, C2V(gRender_poly_text));
                s_end = -1;
                x = next_x;
            }
            /* Change font */
            pY -= (PolyFontHeight(C2V(gDRFont_to_polyfont_mapping)[C2V(gFonts)[-ch].id]) - PolyFontHeight(C2V(gDRFont_to_polyfont_mapping)[pFont->id])) / 2;
            pFont = &C2V(gFonts)[-ch];
        } else {
            s_end += 1;
            s[s_end] = pText[i];
            next_x += CharacterWidth(C2V(gDRFont_to_polyfont_mapping)[pFont->id], pText[i]);
        }
    }
    if (s_end >= 0) {
        s[s_end + 1] = '\0';
        RenderPolyTextLine(s, x, pY, C2V(gDRFont_to_polyfont_mapping)[pFont->id], eJust_left, C2V(gRender_poly_text));
    }
}
C2_HOOK_FUNCTION(0x00465aa0, TransDRPixelmapCleverText)

void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight) {

    if (C2V(gAcc_poly_material) == NULL) {
        C2V(gAcc_poly_material) = BrMaterialAllocate("Acc Poly Mat");
        C2V(gAcc_poly_material)->colour = 0x80;
        C2V(gAcc_poly_material)->flags = BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(C2V(gAcc_poly_material));
        C2V(gAcc_poly_material)->extra_prim = C2V(gAccent_poly_prims);
        C2V(gAccent_poly_prims)[1].v.x = 0x800000;
        BrMaterialUpdate(C2V(gAcc_poly_material), BR_MATU_EXTRA_PRIM);
    }
    if (C2V(gAcc_poly_actor) == NULL) {
        C2V(gAcc_poly_model) = BrModelAllocate("Tint Poly", 6, 2);
        BrVector3Set(&C2V(gAcc_poly_model)->vertices[0].p, pX,          -pY,                -1.01f);
        BrVector3Set(&C2V(gAcc_poly_model)->vertices[1].p, pX + pWidth, -pY,                -1.01f);
        BrVector3Set(&C2V(gAcc_poly_model)->vertices[2].p, pX,          -(pY + pHeight),    -1.01f);
        BrVector3Set(&C2V(gAcc_poly_model)->vertices[3].p, pX + pWidth, -pY,                -1.01f);
        BrVector3Set(&C2V(gAcc_poly_model)->vertices[4].p, pX + pWidth, -(pY + pHeight),    -1.01f);
        BrVector3Set(&C2V(gAcc_poly_model)->vertices[5].p, pX,          -(pY + pHeight),    -1.01f);
        C2V(gAcc_poly_model)->faces[0].vertices[0] = 0;
        C2V(gAcc_poly_model)->faces[0].vertices[1] = 1;
        C2V(gAcc_poly_model)->faces[0].vertices[2] = 2;
        C2V(gAcc_poly_model)->faces[1].vertices[0] = 3;
        C2V(gAcc_poly_model)->faces[1].vertices[1] = 4;
        C2V(gAcc_poly_model)->faces[1].vertices[2] = 5;
        C2V(gAcc_poly_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        C2V(gAcc_poly_actor)->material = C2V(gAcc_poly_material);
        C2V(gAcc_poly_model)->faces[0].material = C2V(gAcc_poly_material);
        C2V(gAcc_poly_model)->faces[1].material = C2V(gAcc_poly_material);
        C2V(gAcc_poly_actor)->model = C2V(gAcc_poly_model);
        BrModelAdd(C2V(gAcc_poly_model));
    }
    if (!C2V(gRender_acc_poly_actor)) {
        C2V(gRender_acc_poly_actor) = 1;
        BrActorAdd(C2V(g2d_camera), C2V(gAcc_poly_actor));
    }
}
C2_HOOK_FUNCTION(0x0045aa60, CreateAccentPolyActor)

void (C2_HOOK_FASTCALL * EarnCredits2_original)(int pAmount, const char* pPrefix_text);
void C2_HOOK_FASTCALL EarnCredits2(int pAmount, const char* pPrefix_text) {

#if 0//defined(C2_HOOKS_ENABLED)
    EarnCredits2_original(pAmount, pPrefix_text);
#else
    char s[256];
    int original_amount;
    tU32 the_time;

    if (C2V(gRace_finished)) {
        return;
    }
    the_time = GetTotalTime();
    if (pAmount == 0) {
        return;
    }
    if (C2V(gCredit_multiplier) != 0) {
        pAmount *= C2V(gCredit_multiplier);
    }
    original_amount = pAmount;
    if (C2V(gLast_credit_headup__displays) >= 0 && the_time - C2V(gLast_earn_time) < 2000) {
        pAmount += C2V(gLast_credit_amount);
    }
    C2V(gLast_credit_amount) = pAmount;
    if (pAmount >= 2) {
        c2_sprintf(s, "%s%d %s", pPrefix_text, pAmount, GetMiscString(eMiscString_credits));
    } else if (pAmount == 1) {
        c2_sprintf(s, "%s1 %s", pPrefix_text, GetMiscString(eMiscString_credit));
    } else if (pAmount == -1) {
        c2_sprintf(s, "%s%s 1 %s", pPrefix_text, GetMiscString(eMiscString_lost_credits_prefix), GetMiscString(eMiscString_credit));
    } else {
        c2_sprintf(s, "%s%s %d %s", GetMiscString(eMiscString_lost_credits_prefix), pPrefix_text, -pAmount, GetMiscString(eMiscString_credits));
    }
    C2V(gProgram_state).credits += original_amount;
    C2V(gLast_credit_headup__displays) = NewTextHeadupSlot(4, 0, 2000, -4, s);
    C2V(gLast_earn_time) = the_time;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044b300, EarnCredits2, EarnCredits2_original)

void C2_HOOK_FASTCALL EarnCredits(int pAmount) {

    EarnCredits2(pAmount, "");
}
C2_HOOK_FUNCTION(0x0044b440, EarnCredits)

int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index) {
    int index;

    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index >= 0) {
        tHeadup* the_headup;
        tHeadup_slot* headup_slot;

        headup_slot = &C2V(gProgram_state).current_car.headup_slots[C2V(gProgram_state).cockpit_on][pSlot_index];
        the_headup = &C2V(gHeadups)[index];
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
            the_headup->x = the_headup->original_x + DRTextWidth(&C2V(gFonts)[C2V(gHud_messages)[pImage_index].font1], C2V(gHud_messages)[pImage_index].message);
            break;
        case eJust_centre:
            the_headup->x = the_headup->original_x + DRTextWidth(&C2V(gFonts)[C2V(gHud_messages)[pImage_index].font1], C2V(gHud_messages)[pImage_index].message) / 2;
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
        c2_strcpy(the_headup->data.image_info.text , C2V(gHud_messages)[pImage_index].message);
        the_headup->data.image_info.font_index = C2V(gHud_messages)[pImage_index].font2;
        the_headup->data.image_info.font = &C2V(gFonts)[C2V(gHud_messages)[pImage_index].font1];
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
C2_HOOK_FUNCTION(0x0044a3a0, NewImageHeadupSlot)

void C2_HOOK_FASTCALL MoveHeadupTo(int pHeadup_index, int pX, int pY) {
    int delta_x;

    if (pHeadup_index >= 0) {
        delta_x = C2V(gHeadups)[pHeadup_index].x - C2V(gHeadups)[pHeadup_index].original_x;
        C2V(gHeadups)[pHeadup_index].original_x = pX;
        C2V(gHeadups)[pHeadup_index].x = delta_x + pX;
        C2V(gHeadups)[pHeadup_index].y = pY;
    }
}
C2_HOOK_FUNCTION(0x0044a860, MoveHeadupTo)

void (C2_HOOK_FASTCALL * DoFancyHeadup_original)(int pIndex);
void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex) {

#if 0//defined(C2_HOOKS_ENABLED)
    DoFancyHeadup_original(pIndex);
#else
    tU32 the_time;
    int temp_ref;
    int center;

    center = C2V(gCurrent_graf_data)->width / 2;
    the_time = GetTotalTime();
    if (C2V(gMap_view) == 2) {
        return;
    }
    if (C2V(gLast_fancy_index) >= 0 && the_time - C2V(gLast_fancy_time) <= 2000 && pIndex < C2V(gLast_fancy_index)) {
        return;
    }
    temp_ref = NewImageHeadupSlot(6, 0, 2000, pIndex + 10);
    if (temp_ref >= 0) {
        tHeadup* the_headup;

        C2V(gLast_fancy_headup) = temp_ref;
        C2V(gLast_fancy_index) = pIndex;
        C2V(gLast_fancy_time) = the_time;

        the_headup = &C2V(gHeadups)[temp_ref];
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044a600, DoFancyHeadup, DoFancyHeadup_original)

int C2_HOOK_FASTCALL SpendCredits(int pAmount) {

    C2V(gProgram_state).credits -= pAmount;
    if (C2V(gProgram_state).credits < 0) {
        C2V(gProgram_state).credits = 0;
    }
    return C2V(gProgram_state).credits;
}
C2_HOOK_FUNCTION(0x0044b470, SpendCredits)

#define HELLO _Pragma("error")

void (C2_HOOK_FASTCALL * ChangingView_original)(void);
void C2_HOOK_FASTCALL ChangingView(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ChangingView_original();
#else
    tU32 the_time;

    the_time = PDGetTotalTime() - C2V(gProgram_state).view_change_start;
    if (C2V(gProgram_state).new_view == eView_undefined) {
        return;
    }
    C2V(gScreen_wobble_x) = 0;
    C2V(gScreen_wobble_y) = 0;
    if (the_time > 175 && C2V(gProgram_state).which_view == C2V(gProgram_state).new_view) {
        switch (C2V(gProgram_state).pending_view) {
        case eView_undefined:
            C2V(gProgram_state).new_view = eView_undefined;
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
            if (C2V(gProgram_state).old_view < C2V(gProgram_state).new_view) {
                C2V(gScreen_wobble_x) = (int)((double)the_time * (double)C2V(gCurrent_graf_data)->cock_margin_x * -2. / 175.);
            } else {
                C2V(gScreen_wobble_x) = (int)((double)the_time * (double)C2V(gCurrent_graf_data)->cock_margin_x * 2. / 175.);
            }
        } else {
            C2V(gProgram_state).which_view = C2V(gProgram_state).new_view;
            switch (C2V(gProgram_state).new_view) {
            case eView_left:
                C2V(gProgram_state).cockpit_image_index = 1;
                break;
            case eView_forward:
                C2V(gProgram_state).cockpit_image_index = 0;
                break;
            case eView_right:
                C2V(gProgram_state).cockpit_image_index = 2;
                break;
            }
            AdjustRenderScreenSize();
            if (C2V(gProgram_state).new_view <= C2V(gProgram_state).old_view) {
                C2V(gScreen_wobble_x) = (int)((double)(175 - the_time) * (double)C2V(gCurrent_graf_data)->cock_margin_x * -2. / 175.);
            } else {
                C2V(gScreen_wobble_x) = (int)((double)(175 - the_time) * (double)C2V(gCurrent_graf_data)->cock_margin_x * 2. / 175.);
            }
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044b170, ChangingView, ChangingView_original)

void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text) {
    tHeadup* the_headup;

    if (pHeadup_index >= 0) {
        the_headup = &C2V(gHeadups)[pHeadup_index];
        c2_strcpy(the_headup->data.text_info.text, pNew_text);
        MungeHeadupWidth(the_headup);
    }
}
C2_HOOK_FUNCTION(0x0044a8d0, ChangeHeadupText)

void C2_HOOK_FASTCALL ChangeHeadupFont(int pHeadup_index, int pFont) {

    if (pHeadup_index >= 0) {
        C2V(gHeadups)[pHeadup_index].data.image_info.font = &C2V(gFonts)[-pFont];
    }
}
C2_HOOK_FUNCTION(0x0044a920, ChangeHeadupFont)

void C2_HOOK_FASTCALL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {
    tU8* ptr;
    tU8* depth_table_ptr;
    tU8* right_ptr;
    int x;
    int y;
    int line_skip;
    int width;

    if (C2V(gDevious_2d)) {
        DeviouslyDimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
        return;
    }

    ptr = (tU8*)pPixelmap->pixels + pLeft + pPixelmap->row_bytes * pTop;
    line_skip = pPixelmap->row_bytes - pRight + pLeft;
    depth_table_ptr = C2V(gDepth_shade_table)->pixels;
    x = C2V(gDepth_shade_table)->row_bytes * C2V(gDim_amount);
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
C2_HOOK_FUNCTION(0x0047cbd0, DimRectangle)

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
C2_HOOK_FUNCTION(0x0047cce0, DimRectangleClipped)

void C2_HOOK_FASTCALL DimAFewBits(void) {
    int i;

#define DIM_INDEX C2V(gProgram_state).cockpit_on && C2V(gProgram_state).cockpit_image_index >= 0
    for (i = 0; i < C2V(gProgram_state).current_car.dim_count[DIM_INDEX]; i++) {
        DimRectangle(
            C2V(gBack_screen),
            C2V(gProgram_state).current_car.dim_left[DIM_INDEX][i],
            C2V(gProgram_state).current_car.dim_top[DIM_INDEX][i],
            C2V(gProgram_state).current_car.dim_right[DIM_INDEX][i],
            C2V(gProgram_state).current_car.dim_bottom[DIM_INDEX][i],
            1);
    }
#undef DIM_INDEX
}
C2_HOOK_FUNCTION(0x004496f0, DimAFewBits)

void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {

    C2V(gDim_model)->vertices[1].p.v[0] = (float)pLeft;
    C2V(gDim_model)->vertices[0].p.v[0] = C2V(gDim_model)->vertices[1].p.v[0];
    C2V(gDim_model)->vertices[3].p.v[0] = (float)pRight;
    C2V(gDim_model)->vertices[2].p.v[0] = C2V(gDim_model)->vertices[3].p.v[0];
    C2V(gDim_model)->vertices[3].p.v[1] = -(float)pTop;
    C2V(gDim_model)->vertices[0].p.v[1] = C2V(gDim_model)->vertices[3].p.v[1];
    C2V(gDim_model)->vertices[2].p.v[1] = -(float)pBottom;
    C2V(gDim_model)->vertices[1].p.v[1] = C2V(gDim_model)->vertices[2].p.v[1];
    BrModelUpdate(C2V(gDim_model), BR_MODU_VERTEX_POSITIONS);
    gDim_actor->render_style = BR_RSTYLE_FACES;
    if (gDim_actor->prev == NULL) {
        BrActorAdd(C2V(g2d_camera), C2V(gDim_actor));
    }
    BrZbsSceneRender(C2V(g2d_camera), C2V(g2d_camera), C2V(gBack_screen), C2V(gDepth_buffer));
    if (C2V(gDim_actor)->parent != NULL) {
        BrActorRemove(C2V(gDim_actor));
    }
    C2V(gDim_actor)->render_style = BR_RSTYLE_NONE;
}
C2_HOOK_FUNCTION(0x0047cad0, DeviouslyDimRectangle)

void C2_HOOK_FASTCALL DoDamageScreen(tU32 pThe_time) {
    int i;
    int y_pitch;
    int the_step;
    int the_wobble_x;
    int the_wobble_y;
    br_pixelmap* the_image;
    tDamage_unit* the_damage;

    if (&C2V(gProgram_state).current_car != C2V(gCar_to_view)) {
        return;
    }
    if (C2V(gProgram_state).cockpit_on && C2V(gProgram_state).cockpit_image_index >= 0) {
        if (C2V(gProgram_state).which_view != eView_forward) {
            return;
        }
        the_wobble_x = C2V(gScreen_wobble_x);
        the_wobble_y = C2V(gScreen_wobble_y);
    } else {
        the_wobble_x = C2V(gProgram_state).current_car.damage_x_offset;
        the_wobble_y = C2V(gProgram_state).current_car.damage_y_offset;
    }

    DRPixelmapCopy(C2V(gDamage_hud), C2V(gGrey_top5));

    for (i = 0; i < REC2_ASIZE(C2V(gProgram_state).current_car.damage_units); i++) {
        the_damage = &C2V(gProgram_state).current_car.damage_units[i];
        if (i != eDamage_driver) {
            the_image = the_damage->images;
            the_step = 5 * the_damage->damage_level / 100;
            y_pitch = (the_image->height / 2) / 5;
            DRPixelmapRectangleMaskedCopy(
                gDamage_hud,
                the_wobble_x + C2V(gProgram_state).current_car.damage_units[i].x_coord,
                the_wobble_y + C2V(gProgram_state).current_car.damage_units[i].y_coord,
                the_image,
                0,
                y_pitch * (2 * the_step + ((pThe_time / the_damage->periods[the_step]) & 1)),
                the_image->width,
                y_pitch);
        }
    }
}
C2_HOOK_FUNCTION(0x0044a9d0, DoDamageScreen)

void C2_HOOK_FASTCALL DoHeadups(tU32 pThe_time) {
    int i;
    int x_offset;
    int y_offset;
    tHeadup* the_headup;

    if (C2V(gNet_mode) != eNet_mode_none) {
        DoNetScores();
    }
    if (C2V(gQueued_headup_count) && PDGetTotalTime() - C2V(gLast_centre_headup) >= 1000) {
        NewTextHeadupSlot(4,
            C2V(gQueued_headups)[0].flash_rate,
            C2V(gQueued_headups)[0].lifetime,
            C2V(gQueued_headups)[0].font_index,
            C2V(gQueued_headups)[0].text);
        KillOldestQueuedHeadup();
    }
    if (C2V(gHeadup_detail_level) == 0 || C2V(gHeadup_detail_level) == 3) {
        MoveHeadupTo(C2V(gTimer_headup), 634, 2);
    } else {
        MoveHeadupTo(C2V(gTimer_headup), 389, 13);
    }

    for (i = 0; i < REC2_ASIZE(C2V(gHeadups)); i++) {
        the_headup = &C2V(gHeadups)[i];
        if (the_headup->type != eHeadup_unused
                && (C2V(gProgram_state).which_view == eView_forward || !the_headup->cockpit_anchored)
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
                            y_offset = C2V(gScreen_wobble_y);
                        } else {
                            y_offset = 0;
                        }
                        if (the_headup->cockpit_anchored) {
                            x_offset = C2V(gScreen_wobble_x);
                        } else {
                            x_offset = 0;
                        }
                        BrPixelmapText(
                            C2V(gRender_screen),
                            x_offset + the_headup->x,
                            y_offset + the_headup->y,
                            the_headup->data.text_info.colour,
                            the_headup->data.text_info.font,
                            the_headup->data.text_info.text);
                        break;
                    case eHeadup_coloured_text:
                        if (C2V(gHeadup_detail_level) == 2 || C2V(gHeadup_detail_level) == 5
                                || (i != C2V(gCredits_won_headup)
                                    && i != C2V(gPed_kill_count_headup)
                                    && i != C2V(gCar_kill_count_headup)
                                    && i != C2V(gLaps_headup))) {
                            if (the_headup->clever) {
                                if (the_headup->cockpit_anchored) {
                                    y_offset = C2V(gScreen_wobble_y);
                                } else {
                                    y_offset = 0;
                                }
                                if (the_headup->cockpit_anchored) {
                                    x_offset = C2V(gScreen_wobble_x);
                                } else {
                                    x_offset = 0;
                                }
                                TransDRPixelmapCleverText(
                                        gBack_screen,
                                        x_offset + the_headup->x,
                                        y_offset + the_headup->y,
                                        the_headup->data.coloured_text_info.coloured_font,
                                        the_headup->data.coloured_text_info.text,
                                        the_headup->right_edge);
                            } else {
                                if (the_headup->cockpit_anchored) {
                                    y_offset = C2V(gScreen_wobble_y);
                                } else {
                                    y_offset = 0;
                                }
                                if (the_headup->cockpit_anchored) {
                                    x_offset = C2V(gScreen_wobble_x);
                                } else {
                                    x_offset = 0;
                                }
                                TransDRPixelmapText(
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
                            the_headup->data.fancy_info.offset -= 325 * C2V(gFrame_period) / 1000;
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
                            the_headup->data.fancy_info.offset -= 325 * C2V(gFrame_period) / 1000;
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
                            y_offset = C2V(gScreen_wobble_y);
                        } else {
                            y_offset = 0;
                        }
                        if (the_headup->cockpit_anchored) {
                            x_offset = C2V(gScreen_wobble_x);
                        } else {
                            x_offset = 0;
                        }
                        OoerrIveGotTextInMeBoxMissus(
                            the_headup->data.coloured_text_info.coloured_font - C2V(gFonts),
                            the_headup->data.coloured_text_info.text,
                            C2V(gRender_screen),
                            C2V(gRender_screen)->width / 10,
                            x_offset + the_headup->y,
                            9 * C2V(gRender_screen)->width / 10,
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
C2_HOOK_FUNCTION(0x00449b10, DoHeadups)

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

    if (C2V(gCar_to_view) != NULL && C2V(gCar_to_view)->driver >= eDriver_oppo) {
        speed_mph = C2V(gCar_to_view)->speedo_speed * WORLD_SCALE / 1600.0f * 1000.f * 3600.0f;
        if (speed_mph < 0.0f) {
            speed_mph = 0.0f;
        }
        if (C2V(gHeadup_detail_level) == 0 || C2V(gHeadup_detail_level) == 3) {
            if (!C2V(gAction_replay_mode)) {
                c2_sprintf(buffer, "%03i", (int)speed_mph);
                RenderPolyTextLine(buffer, 1, 1, kPolyfont_ingame_medium_green, eJust_left, 0);
            }
        } else {
            if (C2V(gProgram_state).cockpit_on && C2V(gProgram_state).cockpit_image_index >= 0) {
                if (C2V(gProgram_state).which_view != eView_forward) {
                    return;
                }
                the_wobble_x = C2V(gScreen_wobble_x);
                the_wobble_y = C2V(gScreen_wobble_y);
            } else {
                the_wobble_x = 0;
                the_wobble_y = 0;
            }
            if (C2V(gProgram_state).current_car.tacho_radius_2[C2V(gProgram_state).cockpit_on] >= 0) {
                int rev;

                rev = div((int)C2V(gCar_to_view)->revs, div(C2V(gCar_to_view)->red_line, 11).quot).quot;
                if (rev < 0) {
                    rev = 0;
                } else if (rev > 11) {
                    rev = 11;
                }
                C2V(gCurrent_rev) = C2V(gRevs)[rev];
            } else {
                speedo_image = C2V(gRevs)[div((int)C2V(gCar_to_view)->revs, 11).quot];
                BrPixelmapRectangleCopy(C2V(gCurrent_rev),
                    C2V(gProgram_state).current_car.tacho_x[C2V(gProgram_state).cockpit_on] + the_wobble_x,
                    C2V(gProgram_state).current_car.tacho_y[C2V(gProgram_state).cockpit_on] + the_wobble_y,
                    speedo_image, 0, 0, speedo_image->width, speedo_image->height);

            }
            if (!C2V(gProgram_state).cockpit_on || C2V(gProgram_state).cockpit_image_index < 0 || C2V(gProgram_state).which_view == eView_forward) {

                gear = (int)C2V(gCar_to_view)->gear;
                if (gear < 0) {
                    gear = -1;
                }
                gear_height = C2V(gProgram_state).current_car.gears_image->height / 8;
                DRPixelmapRectangleMaskedCopy(C2V(gCurrent_rev),
                    C2V(gProgram_state).current_car.gear_x[C2V(gProgram_state).cockpit_on] + the_wobble_x,
                    C2V(gProgram_state).current_car.gear_y[C2V(gProgram_state).cockpit_on] + the_wobble_y,
                    C2V(gProgram_state).current_car.gears_image,
                    0, (gear + 1) * gear_height,
                    C2V(gProgram_state).current_car.gears_image->width,
                    gear_height);
            }
            speedo_image = C2V(gProgram_state).current_car.speedo_image[C2V(gProgram_state).cockpit_on];
            if (C2V(gProgram_state).current_car.speedo_radius_2[C2V(gProgram_state).cockpit_on] >= 0) {
                if (speedo_image != NULL && (!C2V(gProgram_state).cockpit_on || C2V(gProgram_state).cockpit_image_index < 0)) {
                    DRPixelmapRectangleMaskedCopy(C2V(gCurrent_rev),
                        C2V(gProgram_state).current_car.speedo_x[C2V(gProgram_state).cockpit_on] + the_wobble_x,
                        C2V(gProgram_state).current_car.speedo_y[C2V(gProgram_state).cockpit_on] + the_wobble_y,
                        speedo_image, 0, 0, speedo_image->width, speedo_image->height);
                }
                if (speed_mph <= C2V(gProgram_state).current_car.max_speed) {
                    the_angle = DEG_TO_RAD(C2V(gProgram_state).current_car.speedo_start_angle[C2V(gProgram_state).cockpit_on]
                        + ((float)C2V(gProgram_state).current_car.speedo_end_angle[C2V(gProgram_state).cockpit_on] - (float)C2V(gProgram_state).current_car.speedo_start_angle[gProgram_state.cockpit_on]) * speed_mph
                            / (float)C2V(gProgram_state).current_car.max_speed);
                } else {
                    the_angle = DEG_TO_RAD((float)C2V(gProgram_state).current_car.speedo_end_angle[C2V(gProgram_state).cockpit_on]);
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
                    cos_angle = C2V(gCosine_array)[(unsigned int)((REC2_TAU - the_angle2) / REC2_PI * 128.0)];
                } else if (the_angle2 > REC2_PI) {
                    cos_angle = -C2V(gCosine_array)[(unsigned int)((the_angle2 - REC2_PI) / REC2_PI * 128.0)];
                } else if (the_angle2 > REC2_PI_OVER_2) {
                    cos_angle = -C2V(gCosine_array)[(unsigned int)((REC2_PI - the_angle2) / REC2_PI * 128.0)];
                } else {
                    cos_angle = C2V(gCosine_array)[(unsigned int)(the_angle2 / REC2_PI * 128.0)];
                }

                if (the_angle > REC2_3PI_OVER_2) {
                    sin_angle = C2V(gCosine_array)[(unsigned int)((REC2_TAU - the_angle) / REC2_PI * 128.0)];
                } else if (the_angle > REC2_PI) {
                    sin_angle = -C2V(gCosine_array)[(unsigned int)((the_angle - REC2_PI) / REC2_PI * 128.0)];
                } else if (the_angle > REC2_PI_OVER_2) {
                    sin_angle = -C2V(gCosine_array)[(unsigned int)((REC2_PI - the_angle) / REC2_PI * 128.0)];
                } else {
                    sin_angle = C2V(gCosine_array)[(unsigned int)(the_angle / REC2_PI * 128.0)];
                }

                PoshDrawLine(
                    C2V(gBack_screen),
                    (int)((double)C2V(gProgram_state).current_car.speedo_radius_1[C2V(gProgram_state).cockpit_on] * sin_angle
                    + (double)the_wobble_x
                    + (double)C2V(gProgram_state).current_car.speedo_centre_x[C2V(gProgram_state).cockpit_on]),
                    (float)the_angle,
                    (int)((double)C2V(gProgram_state).current_car.speedo_centre_y[C2V(gProgram_state).cockpit_on]
                    - (double)C2V(gProgram_state).current_car.speedo_radius_1[C2V(gProgram_state).cockpit_on] * cos_angle
                    + (double)the_wobble_y),
                    (int)((double)C2V(gProgram_state).current_car.speedo_radius_2[C2V(gProgram_state).cockpit_on] * sin_angle
                    + (double)the_wobble_x
                    + (double)C2V(gProgram_state).current_car.speedo_centre_x[C2V(gProgram_state).cockpit_on]),
                    (int)((double)C2V(gProgram_state).current_car.speedo_centre_y[C2V(gProgram_state).cockpit_on]
                    + (double)the_wobble_y
                    - (double)C2V(gProgram_state).current_car.speedo_radius_2[C2V(gProgram_state).cockpit_on] * cos_angle),
                    C2V(gProgram_state).current_car.speedo_needle_colour[C2V(gProgram_state).cockpit_on]);
            } else if (speedo_image != NULL) {
                DrawNumberAt(speedo_image,
                    C2V(gCurrent_rev),
                    C2V(gProgram_state).current_car.speedo_x[C2V(gProgram_state).cockpit_on] + the_wobble_x,
                    C2V(gProgram_state).current_car.speedo_y[C2V(gProgram_state).cockpit_on] + the_wobble_y,
                    C2V(gProgram_state).current_car.speedo_x_pitch[C2V(gProgram_state).cockpit_on],
                    C2V(gProgram_state).current_car.speedo_y_pitch[C2V(gProgram_state).cockpit_on],
                    (int)speed_mph, 3, 1);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x0044aad0, DoInstruments)

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
C2_HOOK_FUNCTION(0x0047c740, PoshDrawLine)

br_uint_32 C2_HOOK_FASTCALL CarArrowColour(tCar_spec *pCar, tVehicle_type pCategory) {
    tNet_game_player_info* net_player;

    if (C2V(gNet_mode) != eNet_mode_none && pCar->shrapnel_materials[0] != NULL) {
        if (pCar->knackered) {
            return 0x0;
        } else {
            return C2V(gCar_map_colours)[pCategory];
        }
    }
    else {
        net_player = NetPlayerFromCar(pCar);
        if (net_player->field_0x80) {
            return 0xffffffff;
        } else {
            return pCar->shrapnel_materials[0]->index_base;
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

    BrMatrix34ApplyP(&map_pos, pPosition, &C2V(gCurrent_race).map_transformation);
    BrVector3Set(&dir,
        (float)(C2V(gHeadup_map_half_width) + C2V(gINT_0068d890) + C2V(gUINT_0074ab8c)) - map_pos.v[0],
        (float)(C2V(gHeadup_map_half_height) + C2V(gINT_0068c858) + C2V(gUINT_0074ab88)) - map_pos.v[1],
        0.f);
    dot = BrVector3LengthSquared(&dir);
    if (C2V(gINT_0068d6f4) == -1 || dot < C2V(gFLOAT_0074abc4)) {
        C2V(gINT_0068d8b8) = 0;
        C2V(gINT_0068d6f4) = pIndex;
        C2V(gVector2_0068d8b0).v[0] = dir.v[0];
        C2V(gVector2_0068d8b0).v[1] = dir.v[1];
        C2V(gFLOAT_0074abc4) = dot;
    }
    if (C2V(gNet_mode) == eNet_mode_none && (pTime & 0x100)) {
        if (C2V(gMap_view) == 2) {
            PossibleLock(1);
        }
        DRPixelmapRectangleMaskedCopy(pScreen,
            (int)(map_pos.v[0] - C2V(gSmashy_dot)->width / 2),
            (int)(map_pos.v[1] - C2V(gSmashy_dot)->height / 2),
            C2V(gSmashy_dot),
            0,
            0,
            C2V(gSmashy_dot)->width,
            C2V(gSmashy_dot)->height);
        if (C2V(gMap_view) == 2) {
            PossibleUnlock(1);
        }
    }
}

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
    if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1 || C2V(gCurrent_race).race_spec->race_type == kRaceType_Checkpoints) {
        if (C2V(gMap_view) == 2) {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint, 1);
            DoSomeThingsToCheckpoints(pScreen, the_time, DrawCheckpoint, 1);
        }
        else {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint2, 1);
        }
    }
    if ((C2V(gShow_peds_on_map) || C2V(gCurrent_race).race_spec->race_type == kRaceType_Peds)
            || (C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_net_game)->options.show_powerups_on_map)) {

        ped_count = GetPedCount();
        for (i = 0; i < ped_count; i++) {
            if (GetPedPosition(i, &ped_position)) {
                DrawMapSmallBlip(pScreen, the_time, &ped_position, 52);
            }
        }
    }

    if ((C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_net_game)->options.show_powerups_on_map)
            || C2V(gCurrent_race).race_spec->expansion) {

        powerup_count = GetPowerupCount();
        for (i = 0; i < powerup_count; i++) {
            GetPowerupPosition(i, &ped_position);
            DrawMapSmallBlip(pScreen, the_time, &ped_position, 4);
        }
    }
    for (category = C2V(gShow_opponents) ? eVehicle_opponent : eVehicle_self; category >= (C2V(gMap_view) == 1 ? eVehicle_net_player : eVehicle_self); category -= 1) {
        if (category == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(category);
        }
        for (i = 0; i < car_count; i++) {
            if (category == eVehicle_self) {
                car = &C2V(gProgram_state).current_car;
            } else {
                car = GetCarSpec(category, i);
            }
            if (C2V(gNet_mode) != eNet_mode_none) {
                net_player = NetPlayerFromCar(car);
            }
            if (C2V(gNet_mode) == eNet_mode_none
                    || (!car->knackered && !net_player->wasted && net_player->player_status >= ePlayer_status_racing)) {

                if (category == eVehicle_self) {
                    actor = C2V(gPlayer_car_master_actor);
                } else {
                    actor = GetCarSpec(category, i)->car_master_actor;
                }
                DrawCarArrow(pScreen, car, the_time, &actor->t.t.translate.t, category);
            }
        }
    }
    if (C2V(gCurrent_race).race_spec->race_type >= kRaceType_Smash) {
        for (i = 0; i < C2V(gCount_smashable_race_targets); i++) {
            tSmashable_race_target* smash_target = &C2V(gSmashable_race_targets)[i];

            if ((smash_target->model != NULL && smash_target->actor->render_style != BR_RSTYLE_NONE && smash_target->actor->model == smash_target->model)
                    || (smash_target->field_0x8 >= 0 && C2V(gPedestrian_array)[smash_target->field_0x8].hit_points > 0)) {
                if (smash_target->field_0x8 >= 0) {
                    smash_position = &C2V(gPedestrian_array)[smash_target->field_0x8].pos;
                } else {
                    smash_position = &smash_target->actor->t.t.translate.t;
                }

                DrawSmashBlip(pScreen, the_time, smash_position, i);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x00495e10, DoMapOverlays)

void (C2_HOOK_FASTCALL * DoOpponentStatusHeadup_original)(void);
void C2_HOOK_FASTCALL DoOpponentStatusHeadup(void) {

#if defined(C2_HOOKS_ENABLED)
    DoOpponentStatusHeadup_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494900, DoOpponentStatusHeadup, DoOpponentStatusHeadup_original)

void C2_HOOK_FASTCALL CheckpointLine(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    int p2_x;
    int p2_y;

    if (pCheckpoint >= 0 && pCheckpoint < C2V(gCurrent_race).check_point_count) {
        if (C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0] < 0.f) {
            p2_x = 8;
            p2_y = (int)((C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[1] - (float)C2V(gINT_0074abd0))
                * ((float)C2V(gINT_0074abd4) / ((float)C2V(gINT_0074abd4) - C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0]))
                + (float)C2V(gINT_0074abd0));
        } else {
            p2_x = (int)(C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0] + 8.f);
            p2_y = (int)C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[1];
        }
        FancyDrawLine(pMap, C2V(gINT_0074abd4) + 8, C2V(gINT_0074abd0), p2_x, p2_y, 132, 100);
    }
}
C2_HOOK_FUNCTION(0x00496b10, CheckPointLine);

void C2_HOOK_FASTCALL DrawOffsetCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_flash, 0x0068d8f0);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, flash_state, 0x0068d8f4);

    if ((pCheckpoint >= 0 && pCheckpoint < C2V(gCurrent_race).check_point_count && C2V(gTrack_version) >= 1 && !pTarget)
            || Flash(300, &C2V(last_flash), &C2V(flash_state))) {

        if (C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0] >= 0.f
                && C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0] < (float)pMap->width
                && C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[1] >= 0.f
                && C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[1] < (float)pMap->height) {

            DRPixelmapRectangleMaskedCopy(pMap,
                (int)(C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[0] + 8.f - (float)C2V(gCheckpoint_digit_center_x)),
                (int)(C2V(gCurrent_race).checkpoints[pCheckpoint].map_position.v[1] - (float)C2V(gCheckpoint_digit_center_y)),
                C2V(gCheckpoint_numbers),
                0, pCheckpoint * C2V(gCheckpoint_digit_height),
                C2V(gCheckpoint_numbers->width), C2V(gCheckpoint_digit_height));
        }
    }
}
C2_HOOK_FUNCTION(0x00495d00, DrawOffsetCheckpoint);

void C2_HOOK_FASTCALL DoDirectionFinderStuff(br_pixelmap* pMap) {
    tU32 the_time;
    const br_vector2* ptr_v;
    br_scalar tmp;
    br_vector2 tv;

    the_time = PDGetTotalTime();
    if ((C2V(gMini_map_visible) || C2V(gMap_view) == 2)
            && C2V(gCheckpoint_finder_enabled)
            && (C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1 ||
                C2V(gCurrent_race.race_spec)->race_type == kRaceType_Checkpoints)) {
        DoSomeThingsToCheckpoints(pMap, the_time, CheckpointLine, 0);
    }
    if (C2V(gCurrent_race).race_spec->race_type >= kRaceType_Smash) {
        if (C2V(gINT_0068d6f4) < 0) {
            return;
        }
        ptr_v = &C2V(gVector2_0068d8b0);
        tmp = C2V(gFLOAT_0074abc4);
    } else if ((C2V(gNet_mode) != eNet_mode_none
            || C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1
            || C2V(gCurrent_race).race_spec->race_type == kRaceType_Cars) && C2V(gTarget_lock_car_2) != NULL) {
        ptr_v = &C2V(gVector2_0068d6d8);
        tmp = C2V(gFLOAT_0074ab90);
    } else {
        return;
    }
    if (tmp > C2V(gFLOAT_0074abb8)) {
        BrMatrix23TApplyV(&tv, ptr_v, &C2V(gMatrix23_0068c880));
        tmp = sqrtf(tmp) - (float)C2V(gCurrent_graf_data)->field_0x51c;
        BrVector2Normalise(&tv, &tv);
        if (fabsf(tmp * tv.v[0]) > (float)C2V(gINT_0074abd4)) {
            tmp *= (float)C2V(gINT_0074abd4) / fabsf(tmp * tv.v[0]);
        }
        if (C2V(gMini_map_visible) || C2V(gMap_view) == 2) {
            FancyDrawLine(pMap,
                C2V(gINT_0074abd4) + 8, C2V(gINT_0074abd0),
                (int)((float)C2V(gINT_0074abd4) - tmp * tv.v[0] + 8.f),
                (int)((float)C2V(gINT_0074abd0) - tmp * tv.v[1]),
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

    if ((C2V(gMini_map_visible) || C2V(gMap_view) == 2)
            && C2V(gTarget_lock_car_2) != NULL) {
//        y = (int)(((C2V(gPlayer_car_master_actor)->t.t.translate.t.v[1] - C2V(gTarget_lock_car_2)->pos.v[1]) *
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

        delta_height = C2V(gPlayer_car_master_actor)->t.t.translate.t.v[1] - C2V(gTarget_lock_car_2)->pos.v[1];
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

void C2_HOOK_FASTCALL MapOverlay(void) {
    tU32 the_time;
    br_uint_16 original_base_x;
    br_matrix23 mat23;
    br_vector3 player_pos;

    BrMatrix34ApplyP(&C2V(gOrigin_map), &C2V(gPlayer_car_master_actor)->t.t.translate.t,  &C2V(gCurrent_race).map_transformation);
    C2V(gOrigin_headup_map).v[0] = C2V(gOrigin_map).v[0] - (float)(C2V(gINT_0074ab94) / 2);
    C2V(gOrigin_headup_map).v[1] = C2V(gOrigin_map).v[1] - (float)(C2V(gINT_0074abec) / 2);
    C2V(gINT_0068c878) = (int)(gOrigin_headup_map.v[0] + .5f);
    C2V(gINT_0068c874) = (int)(gOrigin_headup_map.v[1] + .5f);
    if (C2V(gINT_0068c878) < 0 || C2V(gINT_0068c874) < 0
            || C2V(gCurrent_race).map_image->width < C2V(gINT_0068c878)
            || C2V(gCurrent_race).map_image->height < C2V(gINT_0068c874)) {
        BrPixelmapFill(C2V(gMap_overlay), 0);
    }
    if (C2V(gINT_0074abec) < C2V(gINT_0074ab94)) {
        C2V(gINT_0068c858) = (C2V(gINT_0074ab94) - C2V(gINT_0074abec)) / 2;
        C2V(gINT_0068d890) = 0;
        BrPixelmapRectangleCopy(C2V(gMap_overlay),
            0, 0,
            gCurrent_race.map_image,
            C2V(gINT_0068c878), C2V(gINT_0068c874) - C2V(gINT_0068c858),
            C2V(gINT_0074ab94), C2V(gINT_0074ab94));
    } else {
        C2V(gINT_0068c858) = 0;
        C2V(gINT_0068d890) = (gINT_0074abec - gINT_0074ab94) / 2;
        BrPixelmapRectangleCopy(C2V(gMap_overlay),
            0, 0,
            C2V(gCurrent_race).map_image,
            C2V(gINT_0068c878) - C2V(gINT_0068d890), C2V(gINT_0068c874),
            C2V(gINT_0074abec), C2V(gINT_0074abec));
    }
    C2V(gCurrent_race).map_transformation.m[3][0] -= C2V(gOrigin_headup_map).v[0] - (float)C2V(gINT_0068d890);
    C2V(gCurrent_race).map_transformation.m[3][1] -= C2V(gOrigin_headup_map).v[1] - (float)C2V(gINT_0068c858);
    BrMatrix34Mul(&C2V(gCar_in_map_space),
        &C2V(gProgram_state).current_car.car_master_actor->t.t.mat,
        &C2V(gCurrent_race).map_transformation);
    mat23.m[0][0] = C2V(gCar_in_map_space).m[0][0];
    mat23.m[0][1] = C2V(gCar_in_map_space).m[0][1];
    mat23.m[1][0] = C2V(gCar_in_map_space).m[2][0];
    mat23.m[1][1] = C2V(gCar_in_map_space).m[2][1];
    mat23.m[2][0] = 0.f;
    mat23.m[2][1] = 0.f;
    BrMatrix23LPNormalise(&C2V(gMatrix23_0068c880), &mat23);
    DoMapOverlays(C2V(gMap_overlay));
    C2V(gCurrent_race).map_transformation.m[3][0] += C2V(gOrigin_headup_map).v[0] - (float)C2V(gINT_0068d890);
    C2V(gCurrent_race).map_transformation.m[3][1] += C2V(gOrigin_headup_map).v[1] - (float)C2V(gINT_0068c858);
    the_time = PDGetTotalTime();
    PossibleLock(1);
    if (C2V(gMini_map_visible) && C2V(gMap_overlay) != NULL) {
        DRPixelmapRotatedAndFeatheredCopy(&C2V(gMatrix23_0068c880),
            C2V(gBack_screen),
            C2V(gHeadup_map_x) + 8, C2V(gHeadup_map_y),
            C2V(gMap_overlay),
            C2V(gINT_0068d890) + C2V(gUINT_0074ab8c), C2V(gINT_0068c858) + C2V(gUINT_0074ab88),
            C2V(gHeadup_map_w), C2V(gHeadup_map_h),
            C2V(gMap_trans));
    }
    InitMap();

    original_base_x = C2V(gMini_map)->base_x;
    C2V(gMini_map)->base_x = C2V(gMini_map)->base_x / 2;
    DoDirectionFinderStuff(C2V(gMini_map));
    C2V(gMini_map)->base_x = original_base_x;
    if (C2V(gMini_map_visible)) {
        if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1
                || C2V(gCurrent_race).race_spec->race_type == kRaceType_Checkpoints) {

            DoSomeThingsToCheckpoints(C2V(gMini_map), the_time, DrawOffsetCheckpoint, 1);
        }
        if (C2V(gMini_map)->type == BR_PMT_INDEX_8) {
            C2V(gMini_map)->base_x = 0;
        } else {
            C2V(gMini_map)->base_x /= 2;
        }
        player_pos.v[0] = (float)(C2V(gHeadup_map_w) / 2 + C2V(gHeadup_map_x) + 8);
        player_pos.v[1] = (float)(C2V(gHeadup_map_h) / 2 + C2V(gHeadup_map_y));
        player_pos.v[2] = C2V(gMini_map_arrow_z);
        DrawArrow(C2V(gBack_screen), 0, &player_pos, CarArrowColour(&C2V(gProgram_state).current_car, eVehicle_self));
    }
    DoNearestOpponentRelativeheightMarkerStuff(C2V(gMini_map));
    C2V(gMini_map)->base_x = original_base_x;
    PossibleUnlock(1);
    if (C2V(gHeadup_detail_level) > 2) {
        DoOpponentStatusHeadup();
    }
}
C2_HOOK_FUNCTION(0x004950b0, MapOverlay)

void C2_HOOK_FASTCALL FinishMap(void) {

    C2V(gBack_screen)->origin_x = C2V(gBack_original_origin_x);
    C2V(gBack_screen)->origin_y = C2V(gBack_original_origin_y);
    C2V(gBack_screen)->base_x = C2V(gBack_original_base_x);
    C2V(gBack_screen)->base_y = C2V(gBack_original_base_y);
}

void C2_HOOK_FASTCALL MapStuffAfterRender(void) {
    if (C2V(gCurrent_race).map_image == NULL) {
        return;
    }
    if (C2V(gMap_view) == 2) {
        BrVector2Set(&C2V(gOrigin_headup_map), 0.f, 0.f);
        DoMapOverlays(C2V(gBack_screen));
        FinishMap();
    }
    else if (C2V(gMap_view) == 1) {
      if (!C2V(gAction_replay_mode)
                && !(C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_net_game)->type == eNet_game_type_foxy && C2V(gThis_net_player_index) == C2V(gIt_or_fox))) {

            MapOverlay();
        }
    }
}
C2_HOOK_FUNCTION(0x00496d80, MapStuffAfterRender)

void C2_HOOK_FASTCALL CleanPolyFontDangler(void) {
    int i;
    br_actor* a;

    for (i = 0; i < C2V(gCount_polyfont_glyph_actors); i++) {
        a = C2V(gPolyfont_glyph_actors)[i];
        if (a->parent != NULL) {
            BrActorRemove(a);
        }
    }
    C2V(gCount_polyfont_glyph_actors) = 0;
}

void C2_HOOK_FASTCALL StopRenderingHeadups(void) {
    int original_origin_x;
    int original_origin_y;
    int original_base_x;
    int original_base_Y;
    int i;
    br_actor* a;

    if (C2V(gCount_polyfont_glyph_actors) != 0) {
        BrActorAdd(C2V(gHUD_root), C2V(gString_root_actor));
    }
    for (i = 0; i < C2V(gHud_actor_storage_size); i++) {
        a = C2V(gHud_actor_storage)[i];
        if (a != NULL && a->prev == NULL) {
            BrActorAdd(C2V(gHUD_root), a);
        }
    }
    original_origin_x = C2V(gRender_screen)->origin_x;
    original_origin_y = C2V(gRender_screen)->origin_y;
    original_base_x = C2V(gRender_screen)->base_x;
    original_base_Y = C2V(gRender_screen)->base_y;
    C2V(gRender_screen)->origin_x = 0;
    C2V(gRender_screen)->origin_y = 0;
    C2V(gRender_screen)->base_x = 0;
    C2V(gRender_screen)->base_y = 0;
    BrZbSceneRender(C2V(gHUD_root), C2V(gHUD_camera), C2V(gRender_screen), C2V(gDepth_buffer));
    C2V(gRender_screen)->origin_x = original_origin_x;
    C2V(gRender_screen)->origin_y = original_origin_y;
    C2V(gRender_screen)->base_x = original_base_x;
    C2V(gRender_screen)->base_y = original_base_Y;
    if (C2V(gString_root_actor)->parent != NULL) {
        BrActorRemove(C2V(gString_root_actor));
    }
    for (i = 0; i < C2V(gHud_actor_storage_size); i++) {
        a = C2V(gHud_actor_storage)[i];
        if (a != NULL && a->parent != NULL) {
            BrActorRemove(a);
        }
    }
    C2V(gHud_actor_storage_size) = 0;
    CleanPolyFontDangler();
    C2V(gRender_poly_text) = 1;
}
C2_HOOK_FUNCTION(0x10062ae8, StopRenderingHeadups)
