#include "displays.h"

#include "font.h"
#include "globvars.h"
#include "globvrbm.h"
#include "grafdata.h"
#include "graphics.h"
#include "loading.h"
#include "polyfont.h"
#include "utility.h"

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
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gHud_timer_rhs_material, 0x0074cac0);
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
    C2V(gHud_timer_rhs_material)->colour_map = C2V(gHud_timer_rhs);
    BrMaterialUpdate(C2V(gHud_timer_rhs_material), BR_MATU_COLOURMAP);

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
        the_headup->data.image_info.field_0x100 = C2V(gHud_messages)[pImage_index].font2;
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
        the_headup->data.fancy_info.field_0x110 = 0;
        switch (the_headup->justification) {
        case eJust_left:
            the_headup->data.fancy_info.field_0x104 = center;
            the_headup->data.fancy_info.field_0x10c = -DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text);
            break;
        case eJust_right:
            the_headup->data.fancy_info.field_0x104 = center + DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text) / 2;
            the_headup->data.fancy_info.field_0x10c = center;
            break;
        case eJust_centre:
            the_headup->data.fancy_info.field_0x104 =  center + DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text) / 2;
            the_headup->data.fancy_info.field_0x10c = -center - DRTextWidth(the_headup->data.fancy_info.font, the_headup->data.fancy_info.text) / 2;
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

void (C2_HOOK_FASTCALL * ChangingView_original)(void);
void C2_HOOK_FASTCALL ChangingView(void) {

#if defined(C2_HOOKS_ENABLED)
    ChangingView_original();
#else
#error "Not implemented"
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
