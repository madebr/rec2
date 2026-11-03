/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "04-netgame.h"

#include "01-network.h"
#include "07-structur.h"
#include "15-displays.h"
#include "27-powerup.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "63-loading3.h"
#include "72-interface.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include <stdio.h>

extern tU32 gPHIL_last_physics_tick;
/* defined in carma2/common/netgame.c, which this translation unit does not include */
extern tU32 gTime_for_punishment;
extern int gNot_shown_race_type_headup;

/* local declarations (functions defined in other translation units whose headers are not
   included here, or whose definitions the central scaffold still has to supply) */
extern void C2_HOOK_FASTCALL MoveHeadupKeepOffset(int pHeadup_index, int pX, int pY);
extern void C2_HOOK_FASTCALL FUN_43d7d0(tNet_game_player_info* pRecipient);
extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize);
extern void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail);
extern void C2_HOOK_FASTCALL SendDetachedNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL SendDetachedNonCarDataToPlayer(tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
extern void C2_HOOK_FASTCALL FUN_42da20(undefined4* pRecipient);
extern void C2_HOOK_FASTCALL FUN_4f0f80(tNet_game_player_info* pRecipient);
extern int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data);
extern int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);
extern void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL CloseDownPowerUpsForCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL NetSendHeadupToPlayer(const char* pMessage, tPlayer_ID pPlayer_id);
extern void C2_HOOK_FASTCALL CalcPlayerScores(void);
extern void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar);
extern intptr_t C2_HOOK_CDECL ForEveryActorModelNoGrooves(br_actor* pActor, void* pContext);
extern int C2_HOOK_FASTCALL ResetFoxModelCallback(br_model* pModel);
extern void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves);
extern int C2_HOOK_FASTCALL ResetFoxLighting(br_material* pMaterial);
extern int gNum_active_non_cars;
extern tNon_car_spec* gActive_non_car_list[99];

// GLOBAL: CARMA2_HW 0x0075b8f8
// GLOBAL: CARMA2_SW 0x00751e58
br_pixelmap* gDigits_pix;

// GLOBAL: CARMA2_HW 0x0068d924
// GLOBAL: CARMA2_SW 0x0068797c
tNet_game_player_info* gLast_lepper;

// GLOBAL: CARMA2_HW 0x0068d928
// GLOBAL: CARMA2_SW 0x00687980
int gLast_it_change;



// GLOBAL: CARMA2_HW 0x0079eae8
int gNet_cash_headup;

// GLOBAL: CARMA2_HW 0x0068d920
int gINT_0068d920;

// GLOBAL: CARMA2_HW 0x0079eaa4
int gNet_ped_headup;

// SendShapeNumbers

// FUNCTION: CARMA2_HW 0x00497ef0
// FUNCTION: CARMA2_SW 0x00494b20
void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id) {
    int i;

    if (pShapes == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id == pPlayer_id) {
            pShapes->items[i].field_0x4 = 0;
            return;
        }
    }
}

// SendNonCar

// HostFillInCarContents

// ClientSendCarData

// DistanceBetweenCars

// FUNCTION: CARMA2_HW 0x00498370
// FUNCTION: CARMA2_SW 0x00494fa0
int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id == pPlayer_id) {
            return (tS8)pShapes->items[i].field_0x4;
        }
    }
    return 0;
}

// AddShapeStuffToMechanicsMessage

// FUNCTION: CARMA2_HW 0x004978d0
int C2_HOOK_FASTCALL TimeToSendData(void) {
    // GLOBAL: CARMA2_HW 0x00659c30
    static tU32 last_physics_send_time = -1;

    if (gPHIL_last_physics_tick > last_physics_send_time && gPHIL_last_physics_tick < last_physics_send_time + 80) {
        return 0;
    }
    last_physics_send_time = gPHIL_last_physics_tick;
    return 1;
}

// SendCarData

// FUNCTION: CARMA2_HW 0x00498390
void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents) {
    int i;

    if (gNet_players[gThis_net_player_index].ID != pContents->player) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].ID == pContents->player) {
                gNet_players[i].car->time_to_recover = pContents->time;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004983f0
void C2_HOOK_FASTCALL ReceivedCrushSync(tNet_message_chunk_car_crush_sync* pContents) {
    int i;
    int j;
    int player_id;
    tCar_spec* car;
    tCar_crush_network_shapes* network_stuff;
    tU16* pCrush_state;
    int* network_id;

    i = 0;
    if (pContents->count > 0) {
        pCrush_state = &pContents->players[0].crush_state;
        do {
            car = NetCarFromPlayerID(*((tPlayer_ID*)pCrush_state - 1));
            if (car != NULL && *pCrush_state == car->car_crush_spec->field_0x24) {
                network_stuff = car->car_crush_spec->network_stuff;
                player_id = pContents->player_id;
                if (network_stuff != NULL) {
                    j = 0;
                    network_id = (int*)network_stuff;
                    while (j < 12) {
                        if (*network_id == player_id) {
                            goto clear_network_flag;
                        }
                        j++;
                        network_id += 2;
                    }
                    goto next_crush_state;
clear_network_flag:
                    network_stuff->items[j].field_0x4 = 0;
next_crush_state:
                    ;
                }
            }
            i++;
            pCrush_state += 4;
        } while (i < pContents->count);
    }
}

// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

// FUNCTION: CARMA2_HW 0x00498bb0
// FUNCTION: CARMA2_SW 0x004957e0
void C2_HOOK_FASTCALL ReceivedCopInfo(void) {
}

// SendAllNonCarPositions

// FUNCTION: CARMA2_HW 0x00498bc0
void C2_HOOK_FASTCALL ReceivedNonCarPosition(tNet_message_chunk_non_car_position* pContents) {
    br_actor* actor;

    actor = gProgram_state.track_spec.non_car_list[pContents->ID];
    if (actor != NULL && gNet_mode != eNet_mode_host) {
        BrMatrix34Copy(&actor->t.t.mat, &pContents->mat);
        if (pContents->flags != 0) {
            actor->identifier[3] = '!';
        }
    }
}

// ReceivedNonCar

// FUNCTION: CARMA2_HW 0x00498e70
// FUNCTION: CARMA2_SW 0x00495aa0
void C2_HOOK_FASTCALL SignalToStartRace(void) {
    tNet_message* the_message;
    int inactive;
    int i;
    int j;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, car_count, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, racing, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, next_race, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, car_list, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tNet_message_chunk_start_race, car_list, 0xf0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_start_race_car, next_car_index, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_start_race_car, matrix, 0x2);
    C2_HOOK_BUG_ON(sizeof(tNet_start_race_car) != 0x14);

    gCurrent_net_game->no_races_yet = 0;
    gSynch_race_start = 0;
    gINT_0074b780 = 1;
    the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_start_race, 0);
    the_message->contents.start_race.racing = (tU8)gProgram_state.racing;
    the_message->contents.start_race.car_count = (tU8)gCurrent_race.number_of_racers;
    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        CompressMatrix34(&the_message->contents.start_race.car_list[i].matrix, &inactive,
            &gCurrent_race.opponent_list[i].car_spec->car_master_actor->t.t.mat);
        for (j = 0; j < gNumber_of_net_players; j++) {
            if (gCurrent_race.opponent_list[i].car_spec == gNet_players[j].car) {
                the_message->contents.start_race.car_list[i].index = (tU8)j;
                if (inactive) {
                    the_message->contents.start_race.car_list[i].index |= 0x80;
                }
                break;
            }
        }
        if (gCurrent_net_game->options.car_choice == eNet_car_all
                || gCurrent_net_game->options.car_choice == eNet_car_novelty) {
            if (gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index < 0) {
                gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index = PickARandomCar();
                gCar_details[gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index].ownership = eCar_owner_someone;
            }
            the_message->contents.start_race.car_list[i].next_car_index = (tU8)gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index;
        }
    }
    if (gPending_race < 0) {
        gPending_race = PickNetRace(gProgram_state.current_race_index,
            gCurrent_net_game->options.race_sequence_type);
    }
    the_message->contents.start_race.next_race = (tU8)gPending_race;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, the_message, NULL);
    if (gProgram_state.racing) {
        SendCurrentPowerups();
    }
}

// SignalToRepositionCar

// STUB: CARMA2_HW 0x00499000
// STUB: CARMA2_SW 0x00495c30
void C2_HOOK_FASTCALL SetUpNetCarPositions(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00499220
void C2_HOOK_FASTCALL ReinitialiseCar(tCar_spec* pCar) {

    StopCarSmokingInstantly(pCar);
    CloseDownPowerUpsForCar(pCar);
    InitialiseCar(pCar);
    TotallyRepairACar(pCar);
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gTime_for_punishment = PDGetTotalTime() + 2000;
        EmptyInventory();
    }
}

// RepositionPlayer

// FUNCTION: CARMA2_HW 0x00499260
// FUNCTION: CARMA2_SW 0x00495e90
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
// FUNCTION: CARMA2_SW 0x00495f10
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

// FUNCTION: CARMA2_HW 0x00499360
void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits) {
    char s[256];
    char s2[256];
    // GLOBAL: CARMA2_HW 0x0068d944
    static tU32 last_flash;
    // GLOBAL: CARMA2_HW 0x0068d948
    static int flash_state;

    if (gNot_shown_race_type_headup) {
        gNot_shown_race_type_headup = 0;
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(gCurrent_net_game->type + 59));
    }
    if (gTime_for_punishment != 0 && PDGetTotalTime() >= gTime_for_punishment) {
        gTime_for_punishment = 0;
        switch (gCurrent_net_game->type) {
        case 2:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(219));
            break;
        case 3:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(220));
            break;
        case 4:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(221));
            break;
        case 7:
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(222));
            break;
        default:
            break;
        }
    }
    if (gNet_mode == eNet_mode_none
        || (float)gProgram_state.credits >= gRecovery_cost.initial_network[gCurrent_net_game->type]
        || Flash(200, &last_flash, &flash_state)) {
        sprintf(s, "\xf8%d\xfa %s", pCredits, GetMiscString(94));
        ChangeHeadupText(gNet_cash_headup, s);
    } else {
        ChangeHeadupText(gNet_cash_headup, "");
    }
    switch (gCurrent_net_game->type) {
    case 5:
        sprintf(s, "\xf8%d\xfa %s", gCurrent_net_game->options.starting_target, GetMiscString(241));
        break;
    case 2:
        sprintf(s, "%s \xf8%d\xfa", GetMiscString(180), gINT_0068d920);
        break;
    case 1:
        sprintf(s, "%s \xf8%d\xfa", GetMiscString(181), gCurrent_net_game->options.starting_target);
        break;
    case 7:
        TimerString(gCurrent_net_game->options.starting_target, s2, 1, 1, 0);
        sprintf(s, "%s \xf8%s\xfa", GetMiscString(182), s2);
        break;
    default:
        s[0] = '\0';
        break;
    }
    ChangeHeadupText(gNet_ped_headup, s);
    switch (gHeadup_detail_level) {
    case 0:
    case 3:
        MoveHeadupKeepOffset(gNet_cash_headup, 0x50, 0x19);
        MoveHeadupKeepOffset(gNet_ped_headup, 0x50, 0);
        break;
    case 1:
    case 4:
        MoveHeadupKeepOffset(gNet_cash_headup, 0x1e0, 0x19);
        MoveHeadupKeepOffset(gNet_ped_headup, 0x1e0, 0);
        break;
    case 2:
    case 5:
        MoveHeadupKeepOffset(gNet_cash_headup, 0x1ae, 0x3c);
        MoveHeadupKeepOffset(gNet_ped_headup, 0x1ae, 0x14);
        break;
    default:
        break;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* The net score board works on twelve 12-byte records; the qsort callbacks below
   sort them (ClipName and DrawScoreBoxes of the original are inlined into
   DoNetScores2, so they have no body of their own in this build). */
typedef struct tNet_score_entry {
    int player_index;
    int score;
    int out_of_game;
} tNet_score_entry;

// FUNCTION: CARMA2_HW 0x0049a330
int C2_HOOK_CDECL SortNetHeadAscending(const void* pFirst, const void* pSecond) {
    const tNet_score_entry* first;
    const tNet_score_entry* second;

    first = (const tNet_score_entry*)pFirst;
    second = (const tNet_score_entry*)pSecond;
    if (first->out_of_game != 0) {
        if (second->out_of_game != 0) {
            return first->out_of_game - second->out_of_game;
        }
        return 2147483647;
    }
    if (second->out_of_game != 0) {
        return -2147483647;
    }
    if (first->score != second->score) {
        return first->score - second->score;
    }
    return *(int*)&gNet_players[first->player_index].last_score_index
        - *(int*)&gNet_players[second->player_index].last_score_index;
}

// FUNCTION: CARMA2_HW 0x0049a3a0
int C2_HOOK_CDECL SortNetHeadDescending(const void* pFirst, const void* pSecond) {
    const tNet_score_entry* first;
    const tNet_score_entry* second;

    first = (const tNet_score_entry*)pFirst;
    second = (const tNet_score_entry*)pSecond;
    if (first->out_of_game != 0) {
        if (second->out_of_game != 0) {
            return first->out_of_game - second->out_of_game;
        }
        return 2147483647;
    }
    if (second->out_of_game != 0) {
        return -2147483647;
    }
    if (first->score != second->score) {
        return second->score - first->score;
    }
    return *(int*)&gNet_players[first->player_index].last_score_index
        - *(int*)&gNet_players[second->player_index].last_score_index;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// ClipName

// CreateBoxes

// DrawScoreBoxes

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00499a00
// STUB: CARMA2_SW 0x004962a0
void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004999f0
// FUNCTION: CARMA2_SW 0x00496290
void C2_HOOK_FASTCALL DoNetScores(void) {

    DoNetScores2(0);
}

// FUNCTION: CARMA2_HW 0x0049a710
// FUNCTION: CARMA2_SW 0x00496890
void C2_HOOK_FASTCALL InitNetHeadups(void) {

    gDigits_pix = LoadPixelmap("HDIGITS.PIX");
    if (gDigits_pix != NULL) {
        BrMapAdd(gDigits_pix);
    }
}

// FUNCTION: CARMA2_HW 0x0049a730
// FUNCTION: CARMA2_SW 0x004968b0
void C2_HOOK_FASTCALL DisposeNetHeadups(void) {

    if (gDigits_pix != NULL) {
        BrMapRemove(gDigits_pix);
        BrPixelmapFree(gDigits_pix);
    }
}

// EverybodysLost

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049a760
// STUB: CARMA2_SW 0x004968e0
void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0049a940
// FUNCTION: CARMA2_SW 0x00496ac0
void C2_HOOK_FASTCALL CheckForVampireWinner(void) {
    int i;
    int players_left;
    int winner_index;

    players_left = 0;
    winner_index = 0;
    if (gProgram_state.racing && gNet_mode == eNet_mode_host) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (!gNet_players[i].field_0x80) {
                players_left++;
                winner_index = i;
            }
        }
        if (players_left <= 1) {
            DeclareWinner(winner_index);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049b540
void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer) {
    int i;
    int infected_count;
    int winner_index;
    tPlayer_ID* player_id;
    int* player_infected;
    char message[256];

    if (pPlayer->field_0x80 == 0) {
        DisposeKevStuffCar(pPlayer->car);
        *(int*)((tU8*)pPlayer->car + 0x1bc) = 1;
        *(int*)((tU8*)pPlayer->car + 0x1b4) = 1;
        *(int*)((tU8*)pPlayer->car + 0x1b8) = 1;
        pPlayer->field_0x80 = 1;
        for (i = 0; i < 3; i++) {
            pPlayer->car->power_up_levels[i] = pPlayer->car->power_up_slots[i];
        }
        if (gNet_mode == eNet_mode_host) {
            sprintf(message, "%s", GetMiscString(0xba));
            NetSendHeadupToPlayer(message, pPlayer->ID);
            i = 0;
            if (0 < gNumber_of_net_players) {
                player_id = &gNet_players[0].ID;
                do {
                    if ((tNet_game_player_info*)(player_id - 9) != pPlayer) {
                        sprintf(message, "%s %s", pPlayer->player_name, GetMiscString(0xb9));
                        NetSendHeadupToPlayer(message, *player_id);
                    }
                    i++;
                    player_id += 0x35;
                } while (i < gNumber_of_net_players);
            }
        }
        pPlayer->score2 = -1 - GetTotalTime();
        infected_count = 0;
        i = gProgram_state.racing;
        winner_index = 0;
        if (i != 0 && gNet_mode == eNet_mode_host) {
            i = 0;
            if (0 < gNumber_of_net_players) {
                player_infected = &gNet_players[0].field_0x80;
                do {
                    if (*player_infected == 0) {
                        infected_count++;
                        winner_index = i;
                    }
                    i++;
                    player_infected += 0x35;
                } while (i < gNumber_of_net_players);
            }
            if (infected_count <= 1) {
                DeclareWinner(winner_index);
            }
        }
    }
}

// FarEnoughAway

// FUNCTION: CARMA2_HW 0x0049a990
// FUNCTION: CARMA2_SW 0x00496b10
void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer) {

    if ((int)(PDGetTotalTime() - gLast_it_change) > 500) {
        if (gLast_lepper != pPlayer) {
            gLast_it_change = PDGetTotalTime();
            if (gIt_or_fox >= 0) {
                gLast_lepper = &gNet_players[gIt_or_fox];
            }
            PlayerIsIt(pPlayer);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049a9e0
// STUB: CARMA2_SW 0x00496b60
void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SelectRandomFox

// SendPlayerScores

// FUNCTION: CARMA2_HW 0x0049ab00
void C2_HOOK_FASTCALL DoNetGameManagement(void) {
    // GLOBAL: CARMA2_HW 0x00659c34
    static tU32 last_playerscores_sent = -1;
    tNet_message_chunk* the_contents;
    int i;

    if (gNet_mode == eNet_mode_host) {
        CalcPlayerScores();
        if (gPHIL_last_physics_tick <= last_playerscores_sent || gPHIL_last_physics_tick >= last_playerscores_sent + 80) {
            last_playerscores_sent = gPHIL_last_physics_tick;
            the_contents = NetStartBroadcastContents((tNet_message_type)24, 0);
            if (the_contents != NULL) {
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
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049b690
// FUNCTION: CARMA2_SW 0x00497810
void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer) {

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
    }
    pPlayer->credits = gCurrent_net_game->options.starting_credits;
    pPlayer->wasted = 0;
    pPlayer->reposition_time = 0;
}

// InitPlayers

// UseGeneralScore

// FUNCTION: CARMA2_HW 0x0049b8d0
void C2_HOOK_FASTCALL NetSendEnvironmentChanges(tNet_game_player_info* pRecipient) {
    int i;
    char exclamation;
    br_actor** non_car_list;
    tNon_car_spec* non_car;
    tNet_message_chunk_non_car_position* message;

    exclamation = '!';
    non_car_list = gProgram_state.track_spec.non_car_list;
    for (i = 0; i < gProgram_state.track_spec.count_non_cars; i++, non_car_list++) {
        if ((*non_car_list)->identifier[3] != exclamation && (*non_car_list)->identifier[3] != '&') {
            continue;
        }
        non_car = (tNon_car_spec*)(*non_car_list)->type_data;
        if (non_car != NULL
                && non_car->driver != 0
                && (tU16)non_car->car_ID == i
                && (non_car->flags & 0x100000) != 0) {
            continue;
        }
        message = (tNet_message_chunk_non_car_position*)NetGetToPlayerContents(0x1c, 0, pRecipient->ID);
        if (message != NULL) {
            BrMatrix34Copy(&message->mat, &(*non_car_list)->t.t.mat);
            message->ID = i;
            message->flags = (*non_car_list)->identifier[3] == exclamation;
        }
    }
    SendDetachedNonCarDataToPlayer(pRecipient);
    FUN_43d7d0(pRecipient);
    FUN_42da20((undefined4*)pRecipient);
    FUN_4f0f80(pRecipient);
    NetSendMessageStacks();
}

// STUB: CARMA2_HW 0x0049b9a0
// STUB: CARMA2_SW 0x00497b20
void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void) {
    NOT_IMPLEMENTED();
}

// ReceivedGameplay

// FUNCTION: CARMA2_HW 0x0049bc20
void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* the_message;

    the_message = NetBuildGuaranteedMessage(0x1b, 0);
    *(int*)((tU8*)the_message + 0x20) = pParam_1;
    *(int*)((tU8*)the_message + 0x24) = pParam_2;
    *(int*)((tU8*)the_message + 0x1c) = pMess;
    *(int*)((tU8*)the_message + 0x28) = pParam_3;
    *(int*)((tU8*)the_message + 0x2c) = pParam_4;
    NetGuaranteedSendMessageToPlayer(gCurrent_net_game, the_message, pPlayer, 0);
}

// FUNCTION: CARMA2_HW 0x0049bc70
// FUNCTION: CARMA2_SW 0x00497df0
void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* the_message;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, mess, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, param_1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, param_2, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, param_3, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, param_4, 0x14);

    the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_gameplay, 0);
    the_message->contents.gameplay.mess = pMess;
    the_message->contents.gameplay.param_1 = pParam_1;
    the_message->contents.gameplay.param_2 = pParam_2;
    the_message->contents.gameplay.param_3 = pParam_3;
    the_message->contents.gameplay.param_4 = pParam_4;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, the_message, NULL);
}

// FUNCTION: CARMA2_HW 0x0049bcb0
// FUNCTION: CARMA2_SW 0x00497e30
void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* the_message;

    if (gNet_mode == eNet_mode_client) {
        the_message = NetBuildGuaranteedMessage(0x1b, 0);
        the_message->contents.gameplay.mess = pMess;
        the_message->contents.gameplay.param_1 = pParam_1;
        the_message->contents.gameplay.param_2 = pParam_2;
        the_message->contents.gameplay.param_3 = pParam_3;
        the_message->contents.gameplay.param_4 = pParam_4;
        NetGuaranteedSendMessageToHost(gCurrent_net_game, the_message, NULL);
    }
}

// FUNCTION: CARMA2_HW 0x0049bd00
// FUNCTION: CARMA2_SW 0x00497e80
void C2_HOOK_FASTCALL InitNetGameplayStuff(void) {

}

// FUNCTION: CARMA2_HW 0x0049bd10
// FUNCTION: CARMA2_SW 0x00497e90
void C2_HOOK_FASTCALL DefaultNetName(void) {

    NetObtainSystemUserName(gProgram_state.player_name, 32);
}

// FUNCTION: CARMA2_HW 0x0049bde0
// FUNCTION: CARMA2_SW 0x00497f60
void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers) {
    tCar_crush_network_shapes* shapes;
    int j;

    for (; pCount > 0; pCount--, pPlayers++) {
        if (pPlayers->car != NULL && pPlayers->car->car_crush_spec != NULL) {
            shapes = pPlayers->car->car_crush_spec->network_stuff;
            for (j = 0; j < (int)REC2_ASIZE(shapes->items); j++) {
                if (shapes->items[j].network_id == 0) {
                    shapes->items[j].network_id = pPlayer_id;
                    shapes->items[j].field_0x4 = 1;
                    break;
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049be40
void C2_HOOK_FASTCALL RemovePlayerFromShapeStatusLists(tPlayer_ID pPlayer_id, int pPlayer_count, tNet_game_player_info* pPlayers) {
    tCar_crush_network_shapes* shapes;
    int j;

    for (; pPlayer_count > 0; pPlayer_count--, pPlayers++) {
        if (pPlayers->car != NULL && pPlayers->car->car_crush_spec != NULL) {
            shapes = pPlayers->car->car_crush_spec->network_stuff;
            for (j = 0; j < (int)REC2_ASIZE(shapes->items); j++) {
                if (shapes->items[j].network_id == pPlayer_id) {
                    shapes->items[j].network_id = 0;
                    break;
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0043d1d0
void C2_HOOK_FASTCALL SendDetachedNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient) {
    int hierarchy_size;
    int contents_size;
    int message;
    int written_size;
    undefined4 car;
    tNet_message* guaranteed_message;

    guaranteed_message = NULL;
    hierarchy_size = GetHierarchyNetworkSize(pNon_car->collision_info);
    contents_size = hierarchy_size + 0x22;
    if (pRecipient == NULL) {
        if (pNon_car->collision_info->disable_move_rotate == 0 || (pNon_car->flags & 0x100000) == 0) {
            message = (int)NetStartBroadcastContents(0x27, contents_size);
        } else {
            guaranteed_message = NetBuildGuaranteedMessage(0x27, contents_size);
            message = (int)guaranteed_message + 0x18;
            pNon_car->flags &= ~0x100000;
        }
    } else {
        message = (int)NetGetToPlayerContents(0x27, contents_size, pRecipient->ID);
    }
    if (message != 0) {
        *(undefined4*)(message + 0x18) = pArg2;
        *(tU16*)(message + 0x20) = (tU16)((pNon_car->actor->identifier[3] == '!') | (pNon_car->collision_info->disable_move_rotate << 1));
        written_size = GetHierarchyNetworkStuff(pNon_car->collision_info, (tU8*)(message + 0x22), 0x48);
        if (written_size != hierarchy_size) {
            FatalError(0x72, "in detached bit non_car object data");
        }
        if (pNon_car->driver == 2
                && pNon_car->actor != NULL
                && (car = *(undefined4*)((tU8*)pNon_car->actor + 0x60)) != 0
                && (car = *(undefined4*)((tU8*)car + 8)) != 0
                && (car = *(undefined4*)((tU8*)car + 0x34)) != 0) {
            car = *(undefined4*)((tU8*)car + 0x2c);
        } else {
            car = 0;
        }
        *(tPlayer_ID*)(message + 4) = NetPlayerFromCar((tCar_spec*)car)->ID;
        *(tU8*)(message + 8) = *(tU8*)((tU8*)*(void**)((tU8*)*(void**)((tU8*)pNon_car->actor + 0x60) + 8) + 0x20);
        CompressVector3((tCompressed_vector3*)(message + 0xa), (br_vector3*)((tU8*)pNon_car->collision_info + 0x24), -10.f, 10.f);
        CompressVector3((tCompressed_vector3*)(message + 0x10), (br_vector3*)((tU8*)pNon_car->collision_info + 0x30), -10.f, 10.f);
        if (pRecipient == NULL) {
            if (guaranteed_message == NULL) {
                NetBroadcastContents((tNet_message_chunk*)message);
                return;
            }
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, guaranteed_message, 0);
        }
    }
}

// SendDetachedNonCarDataToPlayer

// ToggleFlapsFromNetwork

// FUN_43d7d0

// StopCarBeingIt

// STUB: CARMA2_HW 0x004def00
void C2_HOOK_FASTCALL CloseDownPowerUpsForCar(tCar_spec* pCar) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0042da20
void C2_HOOK_FASTCALL FUN_42da20(undefined4* pRecipient) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004f0f80
void C2_HOOK_FASTCALL FUN_4f0f80(tNet_game_player_info* pRecipient) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0049daf0
void C2_HOOK_FASTCALL NetSendHeadupToPlayer(const char* pMessage, tPlayer_ID pPlayer_id) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0049cd90
int C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObj, int pMode) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0043d690
void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x0068d934
int gNot_shown_race_type_headup;

// GLOBAL: CARMA2_HW 0x0068d92c
tU32 gTime_for_punishment;

// STUB: CARMA2_HW 0x0049abf0
void C2_HOOK_FASTCALL CalcPlayerScores(void) { NOT_IMPLEMENTED(); }
