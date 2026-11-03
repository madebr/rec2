/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_31_RACESUMM_H
#define GUARD_31_RACESUMM_H

#include "c2_hooks.h"
#include "rec2_types.h"

// SortScores

// SortGameScores

// DoNetRaceSummary

extern void C2_HOOK_FASTCALL DoEndRaceSummary(tRace_result pRace_result, int pGame_completed);

extern int C2_HOOK_FASTCALL AnyBuyableCars(void);

extern void C2_HOOK_FASTCALL RecordAPOAndCredits(void);

// PickRaceWhenGameCompleted

// MungeRaceProgression

#endif // GUARD_31_RACESUMM_H
