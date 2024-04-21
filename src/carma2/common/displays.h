#ifndef REC2_DISPLAYS_H
#define REC2_DISPLAYS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tHeadup, gHeadups, 37);
C2_HOOK_VARIABLE_DECLARE(br_material*, gAcc_poly_material);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gAcc_poly_actor);
C2_HOOK_VARIABLE_DECLARE(br_model*, gAcc_poly_model);
C2_HOOK_VARIABLE_DECLARE(int, gRender_acc_poly_actor);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_token_value, gAccent_poly_prims, 3);
C2_HOOK_VARIABLE_DECLARE(int, gCredits_won_headup);
C2_HOOK_VARIABLE_DECLARE(int, gPed_kill_count_headup);
C2_HOOK_VARIABLE_DECLARE(int, gCar_kill_count_headup);
C2_HOOK_VARIABLE_DECLARE(int, gTimer_headup);
C2_HOOK_VARIABLE_DECLARE(int, gTime_awarded_headup);
C2_HOOK_VARIABLE_DECLARE(int, gLaps_headup);
C2_HOOK_VARIABLE_DECLARE(int, gNet_cash_headup);
C2_HOOK_VARIABLE_DECLARE(int, gNet_ped_headup);

C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gDRFont_to_polyfont_mapping, 24);

C2_HOOK_VARIABLE_DECLARE(int, gLast_fancy_index);
C2_HOOK_VARIABLE_DECLARE(int, gLast_credit_headup__displays);
C2_HOOK_VARIABLE_DECLARE(int, gLast_time_credit_headup);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_earn_time);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_fancy_time);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_time_earn_time);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gOld_times, 10);

C2_HOOK_VARIABLE_DECLARE_ARRAY(br_font*, gBR_fonts, 4);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_pixelmap*, gRevs, 13);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gArmour_colour_map);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gPowerbar);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gPower_colour_map);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gOffensive_colour_map);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gIcon_test);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gCurrent_rev);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gHud_curve1);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gHud_curve2);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gHud_curve3);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gGrey_top5);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gDamage_hud);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gTimer_lhs);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gHud_timer_rhs);
C2_HOOK_VARIABLE_DECLARE(br_material*, gHud_timer_rhs_material);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gIcon_armour);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gIcon_power);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gIcon_offense);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gIcon_greyBloc1);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gIcon_litBloc1);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gIcon_grnBlock1);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tHud_message, gHud_messages, 46);

int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText);

int C2_HOOK_FASTCALL DRTextCleverWidth(const tDR_font* pFont, const char* pText);

void C2_HOOK_FASTCALL DRPixelmapCentredText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText);

void C2_HOOK_FASTCALL OoerrIveGotTextInMeBoxMissus(int pFont_index, const char* pText, br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pCentred);

int C2_HOOK_FASTCALL DRFontToPolyFontHandle(const tDR_font* pFont);

void C2_HOOK_FASTCALL TransDRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);

void C2_HOOK_FASTCALL InitHeadups(void);

void C2_HOOK_FASTCALL ClearHeadup(int pIndex);

void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index);

void C2_HOOK_FASTCALL ClearHeadups(void);

int C2_HOOK_FASTCALL IsHeadupTextClever(char* pText);

int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup);

int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index);

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void);

void C2_HOOK_FASTCALL ClearQueuedHeadups(void);

int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);

int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);

void C2_HOOK_FASTCALL TransDRPixelmapCleverText(br_pixelmap* pPixelmap, int pX, int pY, tDR_font* pFont, const char* pText, int pRight_edge);

void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight);

#endif //REC2_DISPLAYS_H
