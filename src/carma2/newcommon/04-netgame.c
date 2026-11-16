/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "04-netgame.h"

#include "00-car.h"
#include "01-network.h"
#include "03-racestrt.h"
#include "07-structur.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "27-powerup.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "52-errors.h"
#include "53-controls.h"
#include "58-crush2.h"
#include "60-phil.h"
#include "63-loading3.h"
#include "72-interface.h"
#include "c2_stdio.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include <stdio.h>
#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x0075b8f8
// GLOBAL: CARMA2_SW 0x00751e58
br_pixelmap* gDigits_pix;

// GLOBAL: CARMA2_HW 0x0068d920
// GLOBAL: CARMA2_SW 0x00687978
int gINT_0068d920;

// GLOBAL: CARMA2_HW 0x0068d924
// GLOBAL: CARMA2_SW 0x0068797c
tNet_game_player_info* gLast_lepper;

// GLOBAL: CARMA2_HW 0x0068d928
// GLOBAL: CARMA2_SW 0x00687980
int gLast_it_change;

// GLOBAL: CARMA2_HW 0x0068d92c
// GLOBAL: CARMA2_SW 0x00687984
tU32 gTime_for_punishment;

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
// FUNCTION: CARMA2_SW 0x00494500
int C2_HOOK_FASTCALL TimeToSendData(void) {
    // GLOBAL: CARMA2_HW 0x00659c30
    // GLOBAL: CARMA2_SW 0x006545c8
    static tU32 last_physics_send_time = (tU32)-1;

    if (gPHIL_last_physics_tick > last_physics_send_time && gPHIL_last_physics_tick < last_physics_send_time + 80) {
        return 0;
    }
    last_physics_send_time = gPHIL_last_physics_tick;
    return 1;
}

// SendCarData

// ReceivedRecover

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

// ReceivedNonCarPosition

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

// FUNCTION: CARMA2_HW 0x00499000
// FUNCTION: CARMA2_SW 0x00495c30
void C2_HOOK_FASTCALL SetUpNetCarPositions(void) {
    int i;
    int j;
    int k;
    int grid_index;
    int racer_count;

    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 0x10);

    DisableNetService();
    if (!gInitialised_grid) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            gNet_players[i].grid_position_set = 0;
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
        }
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        gCurrent_race.opponent_list[i].index = -1;
        gCurrent_race.opponent_list[i].ranking = IRandomBetween(0, 99);
        gCurrent_race.opponent_list[i].car_spec = gNet_players[i].car;
        gCurrent_race.opponent_list[i].net_player_index = i;
        gNet_players[i].opponent_list_index = i;
    }
    racer_count = gNumber_of_net_players;
    if (!gInitialised_grid && gCurrent_net_game->options.grid_start) {
        qsort(gCurrent_race.opponent_list, gNumber_of_net_players, sizeof(tOpp_spec), SortGridFunction);
    }
    gCurrent_race.number_of_racers = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        gNet_players[gCurrent_race.opponent_list[i].net_player_index].opponent_list_index = i;
    }
    for (i = 0; i < racer_count; i++) {
        if ((gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver == eDriver_oppo && !gInitialised_grid)
            || (gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver >= eDriver_net_human
                && !gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set)) {
            grid_index = -1;
            for (j = 0; j < kMax_netplayers && grid_index < 0; j++) {
                grid_index = j;
                for (k = 0; k < racer_count; k++) {
                    if (k != i
                        && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_position_set
                        && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_index == j) {
                        grid_index = -1;
                        break;
                    }
                }
            }
            if (grid_index < 0) {
                FatalError(kFatalError_NetworkCodeSelfCheck);
            }
            SetInitialPosition(&gCurrent_race, i, grid_index);
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_index = grid_index;
            if (!gInitialised_grid) {
                gCurrent_race.number_of_racers = i + 1;
            } else {
                InitialiseCar2(gCurrent_race.opponent_list[i].car_spec, 0);
            }
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set = 1;
        }
    }
    gCurrent_race.number_of_racers = racer_count;
    gInitialised_grid = 1;
    ReenableNetService();
}

// FUNCTION: CARMA2_HW 0x00499220
// FUNCTION: CARMA2_SW 0x00495e50
void C2_HOOK_FASTCALL ReinitialiseCar(tCar_spec* pCar) {

    StopCarSmokingInstantly(pCar);
    LoseAllLocalPowerups(pCar);
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

// DoNetworkHeadups

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x0049a410
void C2_HOOK_FASTCALL CreateBoxes(void) {
    br_camera* camera;
    br_pixelmap* pixelmap;
    br_material* material;
    int i;
    char name[32];

    C2_HOOK_BUG_ON(REC2_ASIZE(gNetscore_box_materials) != 4);
    C2_HOOK_BUG_ON(REC2_ASIZE(gNetscore_box_actors) != 12);

    gNetscore_box_universe = BrActorAllocate(BR_ACTOR_NONE, NULL);
    for (i = 0; i < (int)REC2_ASIZE(gNetscore_box_actors); i++) {
        gNetscore_box_actors[i] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        if (gNetscore_box_actors[i] == NULL) {
            BrFailure("Out of memory for NetPlay");
        }
        gNetscore_box_actors[i]->render_style = BR_RSTYLE_FACES;
    }
    gNetScore_box_model = BrModelAllocate("NetScore Model", 4, 2);
    gNetScore_box_model->faces[0].vertices[0] = 0;
    gNetScore_box_model->faces[0].vertices[1] = 1;
    gNetScore_box_model->faces[0].vertices[2] = 2;
    gNetScore_box_model->faces[1].vertices[0] = 1;
    gNetScore_box_model->faces[1].vertices[1] = 3;
    gNetScore_box_model->faces[1].vertices[2] = 2;
    gNetScore_box_model->faces[0].material = NULL;
    gNetScore_box_model->faces[1].material = NULL;
    BrVector3Set(&gNetScore_box_model->vertices[0].p, 0.0f, 0.0f, -2.0f);
    BrVector3Set(&gNetScore_box_model->vertices[1].p, 100.0f, 0.0f, -2.0f);
    BrVector3Set(&gNetScore_box_model->vertices[2].p, 0.0f, -38.0f, -2.0f);
    BrVector3Set(&gNetScore_box_model->vertices[3].p, 100.0f, -38.0f, -2.0f);
    BrVector2Set(&gNetScore_box_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gNetScore_box_model->vertices[1].map, 1.0f / 1.28f, 0.0f);
    BrVector2Set(&gNetScore_box_model->vertices[2].map, 0.0f, 0.38f / 1.28f);
    BrVector2Set(&gNetScore_box_model->vertices[3].map, 1.0f / 1.28f, 0.38f / 1.28f);
    gNetScore_box_model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrModelAdd(gNetScore_box_model);
    for (i = 0; i < (int)REC2_ASIZE(gNetscore_box_materials); i++) {
        sprintf(name, "NETBOX%i.PIX", i + 1);
        pixelmap = LoadPixelmap(name);
        if (pixelmap == NULL) {
            BrFailure("Cannot find headup graphic %s", name);
        }
        BrMapAdd(pixelmap);
        material = BrMaterialAllocate("NetScore Material");
        material->colour = 0;
        material->colour_map = pixelmap;
        material->flags = BR_MATF_FORCE_FRONT | BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(material);
        gNetscore_box_materials[i] = material;
    }
    gNetscore_box_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    gNetscore_box_camera->identifier = "Net HUD Camera";
    camera = gNetscore_box_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->hither_z = 1.0f;
    camera->yon_z = 3.0f;
    camera->width = gScreen->width;
    camera->height = gScreen->height;
    BrActorAdd(gNetscore_box_universe, gNetscore_box_camera);
    gNetscore_box_created = 1;
}
#endif

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
// FUNCTION: CARMA2_SW 0x004976c0
void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer) {
    int i;
    char s[256];

    C2_HOOK_BUG_ON(REC2_ASIZE(pPlayer->car->power_up_levels) != 3);

    if (!pPlayer->field_0x80) {
        MakeCarIt(pPlayer->car);
        pPlayer->car->invulnerable_no_wastage = 1;
        pPlayer->car->invulnerable_no_crushage = 1;
        pPlayer->car->invulnerable_no_damage = 1;
        pPlayer->field_0x80 = 1;
        for (i = 0; i < (int)REC2_ASIZE(pPlayer->car->power_up_levels); i++) {
            pPlayer->car->power_up_levels[i] = pPlayer->car->power_up_slots[i];
        }
        if (gNet_mode == eNet_mode_host) {
            sprintf(s, "%s", GetMiscString(186));
            NetSendHeadupToPlayer(s, pPlayer->ID);
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (&gNet_players[i] != pPlayer) {
                    sprintf(s, "%s %s", pPlayer->player_name, GetMiscString(eMiscString_is_undead));
                    NetSendHeadupToPlayer(s, gNet_players[i].ID);
                }
            }
        }
        pPlayer->score2 = -(int)(1 + GetTotalTime());
        CheckForVampireWinner();
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

// FUNCTION: CARMA2_HW 0x0049a9e0
// FUNCTION: CARMA2_SW 0x00496b60
void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer) {
    int i;
    char s[256];

    if (gIt_or_fox == pPlayer - gNet_players) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        StopCarBeingIt(gNet_players[i].car);
    }
    if (gCurrent_net_game->type == eNet_game_type_foxy) {
        pPlayer->car->power_up_levels[1] = 0;
    }
    MakeCarIt(pPlayer->car);
    gIt_or_fox = pPlayer - gNet_players;
    sprintf(s, "%s", GetMiscString(eMiscString_you_re_the_fox));
    NetSendHeadupToPlayer(s, pPlayer->ID);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (&gNet_players[i] != pPlayer) {
            sprintf(s, "%s %s", pPlayer->player_name, GetMiscString(eMiscString_is_the_fox));
            NetSendHeadupToPlayer(s, gNet_players[i].ID);
        }
    }
}

// SelectRandomFox

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049abf0
// STUB: CARMA2_SW 0x00496d70
void C2_HOOK_FASTCALL CalcPlayerScores(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL SendPlayerScores(void) {
    tNet_message_chunk* the_contents;
    int i;

    the_contents = NetStartBroadcastContents(eNet_message_chunk_type_player_scores, 0);
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
// FUNCTION: CARMA2_SW 0x00496c80
void C2_HOOK_FASTCALL DoNetGameManagement(void) {
    // GLOBAL: CARMA2_HW 0x00659c34
    // GLOBAL: CARMA2_SW 0x006545cc
    static tU32 last_playerscores_sent = (tU32)-1;

    if (gNet_mode == eNet_mode_host) {
        CalcPlayerScores();
        if (gPHIL_last_physics_tick <= last_playerscores_sent || gPHIL_last_physics_tick >= last_playerscores_sent + 80) {
            last_playerscores_sent = gPHIL_last_physics_tick;
            SendPlayerScores();
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

// FUNCTION: CARMA2_HW 0x0049b7e0
// FUNCTION: CARMA2_SW 0x00497960
void C2_HOOK_FASTCALL UseGeneralScore(int pScore) {
    int i;

    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        gINT_0068d920 = pScore;
        break;
    case eNet_game_type_6:
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (!gNet_players[i].field_0x80 && (pScore & (1 << i))) {
                PlayerIsInfected(&gNet_players[i]);
            }
        }
        break;
    case eNet_game_type_foxy:
        if (gNet_players[gIt_or_fox].ID != (tPlayer_ID)pScore) {
            for (i = 0; i < gNumber_of_net_players; i++) {
                StopCarBeingIt(gNet_players[i].car);
            }
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (gNet_players[i].ID == (tPlayer_ID)pScore) {
                    MakeCarIt(gNet_players[i].car);
                    gIt_or_fox = i;
                }
            }
        }
        break;
    default:
        break;
    }
}

// NetSendEnvironmentChanges

// STUB: CARMA2_HW 0x0049b9a0
// STUB: CARMA2_SW 0x00497b20
void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049bad0
// FUNCTION: CARMA2_SW 0x00497c50
void C2_HOOK_FASTCALL ReceivedGameplay(tNet_message_chunk* pContents, tNet_message* pMessage, tU32 pReceive_time) {
    int must_revert_reentrancy;

    // GLOBAL: CARMA2_HW 0x0068d954
    // GLOBAL: CARMA2_SW 0x006879ac
    static int pause_semaphore;

    if (pContents->gameplay.mess == eNet_gameplay_host_paused) {
        if (!pause_semaphore) {
            pause_semaphore = 1;
            NetFullScreenMessage(eMiscString_please_wait_host_has_paused, 1);
            must_revert_reentrancy = PermitNetServiceReentrancy();
            do {
                NetService(0);
                if (CheckQuit()) {
                    NetFullScreenMessage(eMiscString_please_wait_host_has_paused, 1);
                }
            } while (gWaiting_for_unpause
                && gProgram_state.prog_status != eProg_idling
                && (!gRace_finished || gRace_over_reason != eRace_over_abandoned));
            if (must_revert_reentrancy) {
                HaltNetServiceReentrancy();
            }
            gWaiting_for_unpause = 1;
            FadePaletteDown();
            ClearEntireScreen();
            FadePaletteUp();
            pause_semaphore = 0;
        }
    } else if (pContents->gameplay.mess == eNet_gameplay_host_unpaused) {
        gWaiting_for_unpause = 0;
    } else if (pContents->gameplay.mess == eNet_gameplay_suicide) {
        KnackerThisCar(NetCarFromPlayerID(pMessage->header.field_0x8));
    } else {
        switch (gCurrent_net_game->type) {
        case eNet_game_type_checkpoint:
        case eNet_game_type_4:
        case eNet_game_type_5:
            switch (pContents->gameplay.mess) {
            case eNet_gameplay_checkpoint:
                Checkpoint(pContents->gameplay.param_1, 1);
                break;
            case eNet_gameplay_wrong_checkpoint:
                WrongCheckpoint(pContents->gameplay.param_1);
                break;
            case eNet_gameplay_suddenly_death:
                DoFancyHeadup(26);
                ChangeAmbientPratcam(36);
                gRace_finished = 1;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049bc20
// FUNCTION: CARMA2_SW 0x00497da0
void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* the_message;

    the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_gameplay, 0);
    the_message->contents.gameplay.mess = pMess;
    the_message->contents.gameplay.param_1 = pParam_1;
    the_message->contents.gameplay.param_2 = pParam_2;
    the_message->contents.gameplay.param_3 = pParam_3;
    the_message->contents.gameplay.param_4 = pParam_4;
    NetGuaranteedSendMessageToPlayer(gCurrent_net_game, the_message, pPlayer, NULL);
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

// RemovePlayerFromShapeStatusLists

