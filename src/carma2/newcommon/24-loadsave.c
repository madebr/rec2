#include "24-loadsave.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0068b8ec
int gSave_game_out_of_sync;

// GLOBAL: CARMA2_HW 0x0068c728
int gCount_saved_games;

// GLOBAL: CARMA2_HW 0x0068c72c
tSave_game* gSaved_games;

void C2_HOOK_FASTCALL Encryptificate(tSave_game* pSave_games, int pCount) {

    C2_HOOK_BUG_ON(sizeof(tSave_game) != 0x328);
    SplungeSomeData(pSave_games, pCount * sizeof(tSave_game));
}

// MakeSavedGame

// STUB: CARMA2_HW 0x00491ac0
void C2_HOOK_FASTCALL DoSaveGame(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00491bb0
int C2_HOOK_FASTCALL StartSavedGamesList(void) {
    tPath_name path;
    FILE* f;
    br_size_t filesize;

    if (gSaved_games != NULL) {
        PDFatalError("Saved games list already started");
    }
    PathCat(path, gApplication_path, "SAVEDGAMES.ARS");
    f = DRfopen(path, "rb");
    if (f == NULL) {
        return 0;
    }
    PFfseek(f, 0, SEEK_END);
    filesize = PFftell(f);
    PFrewind(f);
    gCount_saved_games = filesize / sizeof(tSave_game);
    gSaved_games = BrMemAllocate(filesize, kMem_misc);
    PFfread(gSaved_games, 1, filesize, f);
    Encryptificate(gSaved_games, gCount_saved_games);
    PFfclose(f);
    return gCount_saved_games;
}

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
    for (i = 0; i < REC2_ASIZE(gProgram_state.cars_available); i++) {
        gProgram_state.cars_available[i] = pSave_game->cars[i];
    }
    for (i = 0; i < 3; i++) {
        gCurrent_APO_levels[i] = gProgram_state.current_car.power_up_levels[i] = pSave_game->apo_levels[i];
        gProgram_state.current_car.power_up_slots[i] = gCurrent_APO_potential_levels[i] = pSave_game->apo_potential[i];
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

