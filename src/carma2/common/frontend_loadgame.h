/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef FRONTEND_LOADGAME_H
#define FRONTEND_LOADGAME_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_LOAD_GAME;
extern int gFrontend_count_saved_games;
extern int gFrontend_load_game_index_top;
extern int gFrontend_mouse_down;
extern int gPrev_frontend_mouse_down;


int C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadGameOutFunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadGameUpdateFunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL TryToLoadGame(int pN);

int C2_HOOK_FASTCALL LoadSlot1(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadSlot2(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadSlot3(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadSlot4(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadSlot5(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadSlot6(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadSlot7(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadSlot8(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadGameScrollUp(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL LoadGameScrollDown(tFrontend_spec* pFrontend);

#endif /* FRONTEND_LOADGAME_H */