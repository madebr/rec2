/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_71_NEWGAME_H
#define GUARD_71_NEWGAME_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tNet_game_options gNet_settings[9];

// SetGameTarget

// SetOptions

// ReadNetGameChoices

extern int C2_HOOK_FASTCALL PickARandomCar(void);

// RequestCarDetails

extern void C2_HOOK_FASTCALL PollCarDetails(tNet_game_details* pNet_game);

extern void C2_HOOK_FASTCALL SetNetAvailability(tNet_game_options* pOptions);

extern void C2_HOOK_FASTCALL DefaultNetSettings(void);

// DisposeJoinableGame

// InitGamesToJoin

// DisposeJoinList

// AddToJoinList

// FillInRaceDescription

#endif // GUARD_71_NEWGAME_H
