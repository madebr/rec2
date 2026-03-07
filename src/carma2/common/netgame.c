#include "netgame.h"

#include "globvars.h"
#include "globvrpb.h"
#include "network.h"
#include "opponent.h"
#include "physics.h"
#include "platform.h"
#include "utility.h"

#include <brender/brender.h>


// GLOBAL: CARMA2_HW 0x0074a738
int gInitialised_grid;

// GLOBAL: CARMA2_HW 0x0074a734
int gIt_or_fox;

// GLOBAL: CARMA2_HW 0x0068d924
tNet_game_player_info* gLast_lepper;

// GLOBAL: CARMA2_HW 0x0068d928
tU32 gLast_it_change;

// GLOBAL: CARMA2_HW 0x0068d934
int gNot_shown_race_type_headup;

// GLOBAL: CARMA2_HW 0x0068d938
int gWinner_declared;

// GLOBAL: CARMA2_HW 0x0068d92c
tU32 gTime_for_punishment;

// GLOBAL: CARMA2_HW 0x0068d920
int gINT_0068d920;

// FUNCTION: CARMA2_HW 0x0049bd10
void C2_HOOK_FASTCALL DefaultNetName(void) {

    /* FIXME: 32 overflows the player_name field */
    C2_HOOK_ASSERT((uintptr_t)&gProgram_state.player_name == 0x0075d590);
    NetObtainSystemUserName(gProgram_state.player_name, 32);
}

// FUNCTION: CARMA2_HW 0x0049ee20
void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length) {

    PDNetObtainSystemUserName(pName, pMax_length);
    pName[9] = '\0';
}


// FUNCTION: CARMA2_HW 0x00499260
void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (!pCar->disabled) {
            pCar->disabled = 1;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] < 500.0f) {
            BrVector3Accumulate(&pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004992e0
void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (pCar->disabled) {
            pCar->disabled = 0;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] > 500.0f) {
            BrVector3Sub(&pCar->car_master_actor->t.t.translate.t, &pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049b690
void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer) {

    C2_HOOK_BUG_ON(sizeof(tNet_game_details) != 0x78);
    C2_HOOK_BUG_ON(sizeof(tNet_game_options) != 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, field_0x14, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, start_race, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, options, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, type, 0x74);
    C2_HOOK_BUG_ON(sizeof(tNet_game_player_info) != 0xd4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, reposition_time, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, score2, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, credits, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, wasted, 0x6c);

    PossibleService();
    switch (gCurrent_net_game->type) {
    case eNet_game_type_fight_to_death:
        pPlayer->score2 = 100;
        break;
    case eNet_game_type_1:
    case eNet_game_type_2:
    case eNet_game_type_4:
    case eNet_game_type_5:
    case eNet_game_type_6:
        pPlayer->score2 = 0;
        break;
    case eNet_game_type_checkpoint:
        pPlayer->score2 = 0xffff;
        break;
    case eNet_game_type_foxy:
        pPlayer->score2 = 0;
        break;
    default:
        abort();
    }
    pPlayer->credits = gCurrent_net_game->options.starting_credits;
    pPlayer->wasted = 0;
    pPlayer->reposition_time = 0;
}

// FUNCTION: CARMA2_HW 0x0049b720
void C2_HOOK_FASTCALL InitPlayers(void) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        InitialisePlayerScore(&gNet_players[i]);
    }
    if (gNet_mode == eNet_mode_host) {
        gLast_it_change = 0;
        gLast_lepper = NULL;
    }
    gTime_for_punishment = 0;
    gNot_shown_race_type_headup = 1;
    gIt_or_fox = -1;
    gWinner_declared = 0;
}

// FUNCTION: CARMA2_HW 0x0049bd00
void C2_HOOK_FASTCALL InitNetGameplayStuff(void) {

}

// FUNCTION: CARMA2_HW 0x0049a760
void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049bc20
void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049bc70
void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049abf0
void C2_HOOK_FASTCALL CalcPlayerScores(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL SendPlayerScores(void) {
    tNet_message_chunk* the_contents;
    int i;

    the_contents = NetStartBroadcastContents(24, 0);
    if (the_contents == NULL) {
        return;
    }
    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        the_contents->scores.general_score = gINT_0068d920;
        break;
    case eNet_game_type_6:
        the_contents->scores.general_score = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].field_0x80) {
                the_contents->scores.general_score |= 1 << i;
            }
        }
        break;
    case eNet_game_type_foxy:
        the_contents->scores.general_score = gNet_players[gIt_or_fox].ID;
        break;
    default:
        break;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        the_contents->scores.scores[i] = gNet_players[i].score2;
    }
    NetBroadcastContents(the_contents);
}

// FUNCTION: CARMA2_HW 0x0049ab00
void C2_HOOK_FASTCALL DoNetGameManagement(void) {
    // GLOBAL: CARMA2_HW 0x00659c34
    static tU32 last_playerscores_sent = -1;

    if (gNet_mode == eNet_mode_host) {
        CalcPlayerScores();
        if (gPHIL_last_physics_tick <= last_playerscores_sent || gPHIL_last_physics_tick >= last_playerscores_sent + 80) {
            last_playerscores_sent = gPHIL_last_physics_tick;
            SendPlayerScores();
        }
    }
}

// FUNCTION: CARMA2_HW 0x00499000
void C2_HOOK_FASTCALL SetUpNetCarPositions(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00498e70
void C2_HOOK_FASTCALL SignalToStartRace(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00499360
void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00497900
void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time) {

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00499a00
void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004999f0
void C2_HOOK_FASTCALL DoNetScores(void) {

    DoNetScores2(0);
}
