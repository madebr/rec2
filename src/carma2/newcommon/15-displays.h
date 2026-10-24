#ifndef GUARD_15_DISPLAYS_H
#define GUARD_15_DISPLAYS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gHeadup_detail_level;

extern tHeadup gHeadups[37];
extern br_font* gBR_fonts[4];
extern br_pixelmap* gRevs[13];
extern br_pixelmap* gArmour_colour_map;
extern br_pixelmap* gPowerbar;
extern br_pixelmap* gPower_colour_map;
extern br_pixelmap* gOffensive_colour_map;
extern br_pixelmap* gIcon_test;
extern br_pixelmap* gCurrent_rev;
extern br_pixelmap* gHud_curve1;
extern br_pixelmap* gHud_curve2;
extern br_pixelmap* gHud_curve3;
extern br_pixelmap* gGrey_top5;
extern br_pixelmap* gDamage_hud;
extern br_pixelmap* gTimer_lhs;
extern br_pixelmap* gHud_timer_rhs;
extern br_pixelmap* gIcon_armour;
extern br_pixelmap* gIcon_power;
extern br_pixelmap* gIcon_offense;
extern br_pixelmap* gIcon_greyBloc1;
extern br_pixelmap* gIcon_litBloc1;
extern br_pixelmap* gIcon_grnBlock1;
extern tHud_message gHud_messages[46];
extern tU32 gLast_fancy_headup;
extern int gLast_fancy_index;
extern tU32 gLast_fancy_time;
extern tU32 gLast_centre_headup;
extern int gLast_credit_headup__displays;
extern tU32 gLast_earn_time;
extern int gLast_time_credit_headup;
extern tU32 gLast_time_earn_time;
extern int gOld_times[10];
extern int gQueued_headup_count;


// static void C2_HOOK_FASTCALL LoadHeadupMessageFile(void);

extern void C2_HOOK_FASTCALL InitHeadups(void);

extern void C2_HOOK_FASTCALL ClearHeadup(int pIndex);

extern void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index);

extern void C2_HOOK_FASTCALL ClearHeadups(void);

extern void C2_HOOK_FASTCALL DimAFewBits(void);

// KillOldestQueuedHeadup

extern void C2_HOOK_FASTCALL DoTestPowerHeadup(int pLevel, const char* pAPO_str);

extern void C2_HOOK_FASTCALL DoPSPowerupHeadups(void);

// DoHeadups

extern int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index);

// IsHeadupTextClever

extern int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup);

extern int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);

extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);

extern int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index);

extern void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex);

extern void C2_HOOK_FASTCALL MoveHeadupTo(int pHeadup_index, int pNew_x, int pNew_y);

extern void C2_HOOK_FASTCALL ChangeHeadupText(int pHeadup_index, char* pNew_text);

extern void C2_HOOK_FASTCALL DoDamageScreen(tU32 pThe_time);

// DoInstruments

// ChangingView

extern void C2_HOOK_FASTCALL EarnCredits2(int pAmount, const char* pPrefix_text);

extern void C2_HOOK_FASTCALL EarnCredits(int pAmount);

// EarnCredits100

extern int C2_HOOK_FASTCALL SpendCredits(int pAmount);

// AwardTime

// AwardTime5

// DoTestHeadup

extern int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth);

extern int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight);

extern void C2_HOOK_FASTCALL DrawThisCarIconNow(int pCar_index, int pX, int pY);

#endif // GUARD_15_DISPLAYS_H
