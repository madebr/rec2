#ifndef GUARD_15_DISPLAYS_H
#define GUARD_15_DISPLAYS_H

#include "c2_hooks.h"

// LoadHeadupMessageFile

extern void C2_HOOK_FASTCALL InitHeadups(void);

// ClearHeadup

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

extern int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth);

extern int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight);

// DrawThisCarIconNow

#endif // GUARD_15_DISPLAYS_H
