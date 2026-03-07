#include "mainmenu.h"

#include "frontend.h"
#include "globvars.h"
#include "globvrpb.h"
#include "network.h"
#include "sound.h"


// FUNCTION: CARMA2_HW 0x00494450
int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00494540
void C2_HOOK_FASTCALL DoMainScreen(void) {

    StartMusicTrack(9998);
    switch (FRONTEND_Main(kFrontend_menu_main)) {
    case 0:
        gProgram_state.prog_status = eProg_quit;
        break;
    case 1:
        gProgram_state.prog_status = eProg_game_starting;
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00494570
void C2_HOOK_FASTCALL DoOptionsMenu(void) {
    tPlayer_status player_status;

#ifdef REC2_FIX_BUGS
    player_status = ePlayer_status_unknown;
#endif

    if (gNet_mode != eNet_mode_none) {
        player_status = gNet_players[gThis_net_player_index].player_status;
        NetPlayerStatusChanged(ePlayer_status_main_menu);
    }
    if (FRONTEND_Main(kFrontend_menu_options) == 0) {
        gProgram_state.prog_status = eProg_quit;
    }
    if (gNet_mode != eNet_mode_none && gNet_players[gThis_net_player_index].player_status == ePlayer_status_main_menu) {
        NetPlayerStatusChanged(player_status);
    }
}
