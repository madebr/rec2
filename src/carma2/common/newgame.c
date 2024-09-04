#include "newgame.h"

#include "globvars.h"
#include "init.h"
#include "loading.h"
#include "network.h"
#include "structur.h"
#include "utility.h"

#include "rec2_types.h"

#include "rec2_macros.h"
#include "c2_stdio.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tNet_game_options, gNet_settings, 9, 0x00763960);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gNet_grid_starts, 10, 0x0059c7e8, {
    1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gRace_index, 0x00764e98);
C2_HOOK_VARIABLE_IMPLEMENT(int, gLast_graph_sel, 0x00688760);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tJoinable_game, gGames_to_join, 6, 0x00688718);

void C2_HOOK_FASTCALL DefaultNetSettings(void) {
    FILE* file;
    tNet_game_options* pOptions;
    tPath_name the_path;
    int i;

    PathCat(the_path, C2V(gApplication_path), "NETDFLT.TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        return;
    }
    ReadNetworkSettings(file, &C2V(gNet_settings)[0]);
    PFrewind(file);
    for (i = 1; i < REC2_ASIZE(C2V(gNet_settings)); i++) {
        pOptions = &C2V(gNet_settings)[i];
        ReadNetworkSettings(file, pOptions);
    }
    PFfclose(file);
}
C2_HOOK_FUNCTION(0x004663e0, DefaultNetSettings)

void C2_HOOK_FASTCALL SetOptions(tNet_game_type pGame_type, tNet_game_options* pGame_options) {

    pGame_options->show_players_on_map = C2V(gNet_settings)[0].show_players_on_map;
    pGame_options->enable_text_messages = C2V(gNet_settings)[0].enable_text_messages;
    pGame_options->show_powerups_on_map = C2V(gNet_settings)[0].show_powerups_on_map;
    pGame_options->powerup_respawn = C2V(gNet_settings)[0].powerup_respawn;
    pGame_options->open_game = C2V(gNet_settings)[pGame_type + 1].open_game;
    pGame_options->starting_credits = C2V(gNet_settings)[0].starting_credits;
    pGame_options->grid_start = C2V(gNet_settings)[pGame_type + 1].grid_start || C2V(gNet_grid_starts)[pGame_type + 1];
    pGame_options->waste_to_transfer = C2V(gNet_settings)[pGame_type + 1].waste_to_transfer;
    pGame_options->random_car_choice = C2V(gNet_settings)[pGame_type + 1].random_car_choice;
    pGame_options->race_sequence_type = C2V(gNet_settings)[0].race_sequence_type;
    pGame_options->car_choice = C2V(gNet_settings)[0].car_choice;
    pGame_options->starting_target = C2V(gNet_settings)[pGame_type + 1].starting_target;

}
C2_HOOK_FUNCTION(0x00468140, SetOptions)

void C2_HOOK_FASTCALL SetGameTarget(tNet_game_type* pGame_type, tNet_game_options* pGame_options) {

}

void C2_HOOK_FASTCALL ReadNetGameChoices(tNet_game_type* pGame_type, tNet_game_options* pGame_options, int* pRace_index) {

    SetOptions(*pGame_type, pGame_options);
    SetGameTarget(pGame_type, pGame_options);
    *pRace_index = PickNetRace(-1, pGame_options->race_sequence_type);
}

void C2_HOOK_FASTCALL DisposeJoinList(int pExemption) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gGames_to_join)); i++) {
        if (i == pExemption) {
            continue;
        }
        if (C2V(gGames_to_join)[i].game != NULL) {
            DisposeJoinableGame(i);
        }
    }
}

void C2_HOOK_FASTCALL DisposeJoinableGame(int pIndex) {

    NetDisposeGameDetails(C2V(gGames_to_join)[pIndex].game);
    C2V(gGames_to_join)[pIndex].game = NULL;
}

int (C2_HOOK_FASTCALL * ChooseNetCar_original)(tNet_game_details* pNet_game, tNet_game_options* pOptions, int* pCar_index, int pIm_the_host_so_fuck_off);
int C2_HOOK_FASTCALL ChooseNetCar(tNet_game_details* pNet_game, tNet_game_options* pOptions, int* pCar_index, int pIm_the_host_so_fuck_off) {

#if defined(C2_HOOKS_ENABLED)
    return ChooseNetCar_original(pNet_game, pOptions, pCar_index, pIm_the_host_so_fuck_off);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00468d70, ChooseNetCar, ChooseNetCar_original)

void (C2_HOOK_FASTCALL * AddToJoinList_original)(tNet_game_details* pGame);
void C2_HOOK_FASTCALL AddToJoinList(tNet_game_details* pGame) {

#if defined(C2_HOOKS_ENABLED)
    AddToJoinList_original(pGame);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004681d0, AddToJoinList, AddToJoinList_original)

void C2_HOOK_FASTCALL SetUpOtherNetThings(tNet_game_details* pGame_details) {

    C2V(gProgram_state).skill_level = 1;
    InitGame(pGame_details->start_race);
}

void C2_HOOK_FASTCALL InitGamesToJoin(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gGames_to_join)); i++) {

        C2V(gGames_to_join)[i].game = NULL;
    }
}
