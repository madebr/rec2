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


// static void C2_HOOK_FASTCALL LoadHeadupMessageFile(void);

extern void C2_HOOK_FASTCALL InitHeadups(void);

extern void C2_HOOK_FASTCALL ClearHeadup(int pIndex);

// ClearHeadupSlot

// ClearHeadups

// DimAFewBits

// KillOldestQueuedHeadup

// DoTestPowerHeadup

// DoPSPowerupHeadups

// DoHeadups

// FindAHeadupHoleWoofBarkSoundsABitRude

// IsHeadupTextClever

// MungeHeadupWidth

// NewTextHeadupSlot2

// NewTextHeadupSlot

// NewImageHeadupSlot

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
