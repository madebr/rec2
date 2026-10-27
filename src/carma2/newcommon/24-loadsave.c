/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "24-loadsave.h"

#include "globvars.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0068b8e8
// GLOBAL: CARMA2_SW 0x00685e48
int gINT_0068b8e8;

// GLOBAL: CARMA2_HW 0x0068b8ec
int gSave_game_out_of_sync;

// GLOBAL: CARMA2_HW 0x0068c72c
tSave_game* gSaved_games;

// GLOBAL: CARMA2_HW 0x0068c728
int gCount_saved_games;

// Encryptificate

// MakeSavedGame

// STUB: CARMA2_HW 0x00491ac0
void C2_HOOK_FASTCALL DoSaveGame(void) {
    NOT_IMPLEMENTED();
}

// StartSavedGamesList

// FUNCTION: CARMA2_HW 0x00491c90
void C2_HOOK_FASTCALL EndSavedGamesList(void) {

    if (gSaved_games != NULL) {
        BrMemFree(gSaved_games);
    }
    gSaved_games = NULL;
}

// FUNCTION: CARMA2_HW 0x00491cb0
tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN) {

    if (gSaved_games != NULL && pN < gCount_saved_games) {
        return &gSaved_games[gCount_saved_games - pN - 1];
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00491ce0
int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gProgram_state.cars_available) != 60);
    C2_HOOK_BUG_ON(REC2_ASIZE(gCurrent_APO_levels) != 3);

    if (pSave_game->magic != 0x12345678) {
        return 0;
    }
    gProgram_state.skill_level = pSave_game->skill_level;
    gProgram_state.game_completed = pSave_game->game_completed;
    gProgram_state.current_race_index = pSave_game->current_race_index;
    gCurrent_race_group = gRace_list[gProgram_state.current_race_index].group;
    gIs_boundary_race = pSave_game->is_boundary_race;
    gProgram_state.credits = pSave_game->credits;
    strcpy(gProgram_state.car_name, pSave_game->car_name);
    strcpy(gProgram_state.player_name, pSave_game->player_name);
    for (i = 0; i < gNumber_of_races; i++) {
        gRace_list[i].count_opponents = pSave_game->races_finished[i];
    }
    gProgram_state.number_of_cars = pSave_game->number_of_cars;
    gProgram_state.current_car_index = pSave_game->current_car_index;
    memcpy(gProgram_state.cars_available, pSave_game->cars, sizeof(gProgram_state.cars_available));
    for (i = 0; i < (int)REC2_ASIZE(gCurrent_APO_levels); i++) {
        gCurrent_APO_levels[i] = gProgram_state.current_car.power_up_levels[i] = pSave_game->apo_levels[i];
        gCurrent_APO_potential_levels[i] = gProgram_state.current_car.power_up_slots[i] = pSave_game->apo_potential[i];
    }
    gProgram_state.field_0x2c = 1;
    return 1;
}

// FUNCTION: CARMA2_HW 0x00491e20
int C2_HOOK_FASTCALL DoLoadGame(int pIndex) {

    if (gSaved_games != NULL && pIndex >= 0 && pIndex < gCount_saved_games) {
        return DoLoadGame2(GetNthSavedGame(pIndex));
    }
    return 0;
}

// DoLoadMostRecentGame

