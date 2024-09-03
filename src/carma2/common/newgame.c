#include "newgame.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "rec2_types.h"

#include "rec2_macros.h"
#include "c2_stdio.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tNet_game_options, gNet_settings, 9, 0x00763960);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gNet_grid_starts, 10, 0x0059c7e8, {
    1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
});

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
