#include "loadsave.h"

#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

#include "rec2_macros.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gValid_stashed_save_game, 0x00688ae4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSave_game_out_of_sync, 0x0068b8ec);
C2_HOOK_VARIABLE_IMPLEMENT(tSave_game, gStashed_save_game, 0x00688780);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_saved_games, 0x0068c728);
C2_HOOK_VARIABLE_IMPLEMENT(tSave_game*, gSaved_games, 0x0068c72c);

void C2_HOOK_FASTCALL RestoreSinglePlayerState(void) {

    LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), -1);
    DoLoadGame2(&C2V(gStashed_save_game));
    C2V(gValid_stashed_save_game) = 0;
}

void C2_HOOK_FASTCALL MaybeRestoreSavedGame(void) {

    if (C2V(gValid_stashed_save_game)) {
        RestoreSinglePlayerState();
    }
}
C2_HOOK_FUNCTION(0x00466e90, MaybeRestoreSavedGame)

int (C2_HOOK_FASTCALL * LoadTheGame_original)(tSave_game* pSave_game);
int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game) {

#if 0//defined(C2_HOOKS_ENABLED)
    return LoadTheGame_original(pSave_game);
#else
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gProgram_state).cars_available) != 60);

    if (pSave_game->magic != 0x12345678) {
        return 0;
    }
    C2V(gProgram_state).skill_level = pSave_game->skill_level;
    C2V(gProgram_state).game_completed = pSave_game->game_completed;
    C2V(gProgram_state).current_race_index = pSave_game->current_race_index;
    C2V(gCurrent_race_group) = C2V(gRace_list)[C2V(gProgram_state).current_race_index].group;
    C2V(gIs_boundary_race) = pSave_game->is_boundary_race;
    C2V(gProgram_state).credits = pSave_game->credits;
    c2_strcpy(C2V(gProgram_state).car_name, pSave_game->car_name);
    c2_strcpy(C2V(gProgram_state).player_name, pSave_game->player_name);
    for (i = 0; i < C2V(gNumber_of_races); i++) {
        C2V(gRace_list)[i].count_opponents = pSave_game->races_finished[i];
    }
    C2V(gProgram_state).number_of_cars = pSave_game->number_of_cars;
    C2V(gProgram_state).current_car_index = pSave_game->current_car_index;
    for (i = 0; i < REC2_ASIZE(C2V(gProgram_state).cars_available); i++) {
        C2V(gProgram_state).cars_available[i] = pSave_game->cars[i];
    }
    for (i = 0; i < 3; i++) {
        C2V(gCurrent_APO_levels)[i] = C2V(gProgram_state).current_car.power_up_levels[i] = pSave_game->apo_levels[i];
        C2V(gCurrent_APO_potential_levels)[i] = C2V(gProgram_state).current_car.power_up_slots[i] = pSave_game->apo_potential[i];
    }
    C2V(gProgram_state).field_0x2c = 1;
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491ce0, DoLoadGame2, LoadTheGame_original)

void (C2_HOOK_FASTCALL * LoadLastSaveGame_original)(void);
void C2_HOOK_FASTCALL DoLoadMostRecentGame(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadLastSaveGame_original();
#else

    if (StartSavedGamesList()) {
        DoLoadGame(0);
        EndSavedGamesList();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491e50, DoLoadMostRecentGame, LoadLastSaveGame_original)

void (C2_HOOK_FASTCALL * DoSaveGame_original)(void);
void C2_HOOK_FASTCALL DoSaveGame(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    DoSaveGame_original();
#else
    tPath_name the_path;
    tSave_game save_game;
    FILE* f;

    C2_HOOK_BUG_ON(sizeof(save_game) != 0x328);

    if (C2V(gNet_mode) == eNet_mode_none) {
        PathCat(the_path, C2V(gApplication_path), "SAVEDGAMES.ARS");
        PDFileUnlock(the_path);
        f = DRfopen(the_path,"ab+");
        if (f != NULL) {
            PFfseek(f, 0, SEEK_END);
            C2V(gCurrent_APO_potential_levels)[0] = C2V(gProgram_state).current_car.power_up_slots[0];
            C2V(gCurrent_APO_potential_levels)[1] = C2V(gProgram_state).current_car.power_up_slots[1];
            C2V(gCurrent_APO_potential_levels)[2] = C2V(gProgram_state).current_car.power_up_slots[2];
            C2V(gCurrent_APO_levels)[0] = C2V(gProgram_state).current_car.power_up_levels[0];
            C2V(gCurrent_APO_levels)[1] = C2V(gProgram_state).current_car.power_up_levels[1];
            C2V(gCurrent_APO_levels)[2] = C2V(gProgram_state).current_car.power_up_levels[2];
            MakeSavedGame(&save_game);
            Encryptificate(&save_game, 1);
            DRfwrite(&save_game, 1, sizeof(save_game), f);
            PFfclose(f);
            C2V(gSave_game_out_of_sync) = 0;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491ac0, DoSaveGame, DoSaveGame_original)

void C2_HOOK_FASTCALL SplungeSomeData(void* pData, size_t size) {

}
C2_HOOK_FUNCTION(0x00500060, SplungeSomeData)

void C2_HOOK_FASTCALL Encryptificate(tSave_game* pSave_games, int pCount) {

    C2_HOOK_BUG_ON(sizeof(tSave_game) != 0x328);
    SplungeSomeData(pSave_games, pCount * sizeof(tSave_game));
}

int C2_HOOK_FASTCALL StartSavedGamesList(void) {
    tPath_name path;
    FILE* f;
    int filesize;

    C2_HOOK_BUG_ON(sizeof(tSave_game) != 0x328);

    if (C2V(gSaved_games) != NULL) {
        PDFatalError("Saved games list already started");
    }
    PathCat(path, C2V(gApplication_path), "SAVEDGAMES.ARS");
    f = DRfopen(path, "rb");
    if (f == NULL) {
        return 0;
    }
    PFfseek(f, 0, SEEK_END);
    filesize = PFftell(f);
    PFrewind(f);
    C2V(gCount_saved_games) = filesize / sizeof(tSave_game);
    C2V(gSaved_games) = BrMemAllocate(filesize, kMem_misc);
    PFfread(C2V(gSaved_games), 1, filesize, f);
    Encryptificate(C2V(gSaved_games), C2V(gCount_saved_games));
    PFfclose(f);
    return C2V(gCount_saved_games);
}
C2_HOOK_FUNCTION(0x00491bb0, StartSavedGamesList)

void C2_HOOK_FASTCALL EndSavedGamesList(void) {

    if (C2V(gSaved_games) != NULL) {
        BrMemFree(C2V(gSaved_games));
    }
    C2V(gSaved_games) = NULL;
}
C2_HOOK_FUNCTION(0x00491c90, EndSavedGamesList)

tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN) {

    if (C2V(gSaved_games) != NULL && pN < C2V(gCount_saved_games)) {
        return &C2V(gSaved_games)[C2V(gCount_saved_games) - pN - 1];
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x00491cb0, GetNthSavedGame)

int C2_HOOK_FASTCALL DoLoadGame(int pIndex) {

    if (C2V(gSaved_games) != NULL && pIndex >= 0 && pIndex < C2V(gCount_saved_games)) {
        return DoLoadGame2(&C2V(gSaved_games)[C2V(gCount_saved_games) - pIndex - 1]);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00491e20, DoLoadGame)

void C2_HOOK_FASTCALL MakeSavedGame(tSave_game* pSave_game) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gProgram_state).cars_available) != 60);

    pSave_game->magic = 0x12345678;
    pSave_game->skill_level = C2V(gProgram_state).skill_level;
    pSave_game->game_completed = C2V(gProgram_state).game_completed;
    pSave_game->current_race_index = C2V(gProgram_state).current_race_index;
    pSave_game->is_boundary_race = C2V(gIs_boundary_race);
    pSave_game->credits = C2V(gProgram_state).credits;
    c2_strcpy(pSave_game->car_name, C2V(gProgram_state).car_name);
    c2_strcpy(pSave_game->player_name, C2V(gProgram_state).player_name);
    PDGetCurrentTime(pSave_game->time);
    PDGetCurrentDate(pSave_game->date);
    for (i = 0; i < C2V(gNumber_of_races); i++) {
        pSave_game->races_finished[i] = C2V(gRace_list)[i].count_opponents;
    }
    pSave_game->number_of_cars = C2V(gProgram_state).number_of_cars;
    pSave_game->current_car_index = C2V(gProgram_state).current_car_index;
    for (i = 0; i < REC2_ASIZE(C2V(gProgram_state).cars_available); i++) {
        pSave_game->cars[i] = C2V(gProgram_state).cars_available[i];
    }
    for (i = 0; i < 3; i++) {
        pSave_game->apo_levels[i] = C2V(gCurrent_APO_levels)[i];
        pSave_game->apo_potential[i] = C2V(gCurrent_APO_potential_levels)[i];
    }
}
C2_HOOK_FUNCTION(0x004919a0, MakeSavedGame)
