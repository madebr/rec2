/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "44-mainmenu.h"

#include "01-network.h"
#include "69-sound.h"
#include "72-interface.h"
#include "globvars.h"
#include "globvrpb.h"

// QuitVerifyStart

extern int gMouse_was_started__mainmenu;
extern int gReplace_background;

extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL TurnOnPanelFlics(void);
extern void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);
extern void C2_HOOK_FASTCALL FadePaletteUp(void);

// FUNCTION: CARMA2_HW 0x00494400
int C2_HOOK_FASTCALL QuitVerifyDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out) {
    (void)pCurrent_mode;
    (void)pGo_ahead;
    (void)pEscaped;
    (void)pTimed_out;

    FadePaletteDown();
    TurnOnPanelFlics();
    if (gMouse_was_started__mainmenu != 0) {
        RemoveTransientBitmaps(1);
    }
    if (gReplace_background != 0) {
        if (pCurrent_choice == 0) {
            ClearEntireScreen();
        }
        PDScreenBufferSwap(0);
        FadePaletteUp();
    }
    return pCurrent_choice == 0;
}

// STUB: CARMA2_HW 0x00494450
int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {
    NOT_IMPLEMENTED();
}

// GLOBAL: CARMA2_HW 0x0068c850
int gMouse_was_started__mainmenu = 0;

// GLOBAL: CARMA2_HW 0x0068c854
int gReplace_background = 0;

// FUNCTION: CARMA2_HW 0x00494540
// FUNCTION: CARMA2_SW 0x00491840
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
// FUNCTION: CARMA2_SW 0x00491870
void C2_HOOK_FASTCALL DoOptionsMenu(void) {
    tPlayer_status player_status;

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

