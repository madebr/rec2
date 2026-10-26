/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_24_LOADSAVE_H
#define GUARD_24_LOADSAVE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gINT_0068b8e8;
extern int gSave_game_out_of_sync;
extern int gINT_0068b8e8;
extern int gCount_saved_games;

// Encryptificate

// MakeSavedGame

extern void C2_HOOK_FASTCALL DoSaveGame(void);

// StartSavedGamesList

extern void C2_HOOK_FASTCALL EndSavedGamesList(void);

extern tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN);

extern int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game);

extern int C2_HOOK_FASTCALL DoLoadGame(int pIndex);

// DoLoadMostRecentGame

#endif // GUARD_24_LOADSAVE_H
