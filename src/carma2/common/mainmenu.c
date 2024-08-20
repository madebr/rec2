#include "mainmenu.h"

#include "frontend.h"
#include "globvars.h"
#include "globvrpb.h"
#include "network.h"


int (C2_HOOK_FASTCALL * DoVerifyQuit_original)(int pReplace_background);
int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {

#if defined(C2_HOOKS_ENABLED)
    return DoVerifyQuit_original(pReplace_background);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494450, DoVerifyQuit, DoVerifyQuit_original)

void (C2_HOOK_FASTCALL * DoMainMenu_original)(void);
void C2_HOOK_FASTCALL DoMainScreen(void) {

#if defined(C2_HOOKS_ENABLED)
    DoMainMenu_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494540, DoMainScreen, DoMainMenu_original)

void (C2_HOOK_FASTCALL * DoOptionsMenu_original)(void);
void C2_HOOK_FASTCALL DoOptionsMenu(void) {

#if 0// defined(C2_HOOKS_ENABLED)
    DoOptionsMenu_original();
#else
    tPlayer_status player_status;

#ifdef REC2_FIX_BUGS
    player_status = ePlayer_status_unknown;
#endif

    if (C2V(gNet_mode) != eNet_mode_none) {
        player_status = C2V(gNet_players)[C2V(gThis_net_player_index)].player_status;
        NetPlayerStatusChanged(ePlayer_status_main_menu);
    }
    if (FRONTEND_Main(kFrontend_menu_options) == 0) {
        C2V(gProgram_state).prog_status = eProg_quit;
    }
    if (C2V(gNet_mode) != eNet_mode_none && C2V(gNet_players)[C2V(gThis_net_player_index)].player_status == ePlayer_status_main_menu) {
        NetPlayerStatusChanged(player_status);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494570, DoOptionsMenu, DoOptionsMenu_original)
