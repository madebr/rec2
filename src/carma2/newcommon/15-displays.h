#ifndef GUARD_15_DISPLAYS_H
#define GUARD_15_DISPLAYS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gHeadup_detail_level;

extern int gLast_fancy_index;
extern int gLast_credit_headup__displays;
extern int gLast_time_credit_headup;
extern tU32 gLast_earn_time;
extern tU32 gLast_time_earn_time;
extern tU32 gLast_fancy_time;
extern tU32 gLast_fancy_headup;
extern int gOld_times[10];
extern tHeadup gHeadups[37];
extern int gQueued_headup_count;
extern tQueued_headup gQueued_headups[4];
extern tU32 gLast_centre_headup;
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

extern void C2_HOOK_FASTCALL InitHeadups(void);

extern void C2_HOOK_FASTCALL ClearHeadup(int pIndex);

extern void C2_HOOK_FASTCALL ClearHeadupSlot(int pSlot_index);

extern void C2_HOOK_FASTCALL ClearHeadups(void);

// DimAFewBits

extern void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void);

extern void C2_HOOK_FASTCALL ClearQueuedHeadups(void);

// DoTestPowerHeadup

// DoPSPowerupHeadups

// DoHeadups

extern int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index);

extern int C2_HOOK_FASTCALL IsHeadupTextClever(char* pText);

extern int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup);

extern int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText, int pQueue_it);

extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);

extern int C2_HOOK_FASTCALL NewImageHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pImage_index);

extern void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex);

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

extern int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth);

extern int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight);

// DrawThisCarIconNow

#endif // GUARD_15_DISPLAYS_H
