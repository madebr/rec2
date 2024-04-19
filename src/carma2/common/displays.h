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

int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup);

int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index);

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void);

void C2_HOOK_FASTCALL ClearQueuedHeadups(void);

int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, char* pText, int pQueue_it);

int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, char* pText);

void C2_HOOK_FASTCALL TransDRPixelmapCleverText(br_pixelmap* pPixelmap, int pX, int pY, tDR_font* pFont, char* pText, int pRight_edge);

void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight);

#endif //REC2_DISPLAYS_H
