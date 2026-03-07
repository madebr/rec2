#include "structur.h"

#include "car.h"
#include "controls.h"
#include "crush.h"
#include "cutscene.h"
#include "displays.h"
#include "drmem.h"
#include "errors.h"
#include "finteray.h"
#include "flicplay.h"
#include "frontend.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "loading.h"
#include "loadsave.h"
#include "mainloop.h"
#include "mainmenu.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "piping.h"
#include "pratcam.h"
#include "racesumm.h"
#include "racestrt.h"
#include "sound.h"
#include "temp.h"
#include "tinted.h"
#include "utility.h"
#include "world.h"

#include "c2_stdlib.h"


// GLOBAL: CARMA2_HW 0x00660ce0
int gMirror_on__structur = 1;

// GLOBAL: CARMA2_HW 0x00660ce8
int gLoad_last_save_game = 1;

// GLOBAL: CARMA2_HW 0x007051f8
int gMoney_pre_race;

// GLOBAL: CARMA2_HW 0x007051e0
int gAPO_pre_race[3];

// GLOBAL: CARMA2_HW 0x006b781c
tRace_over_reason gRace_over_reason;

// GLOBAL: CARMA2_HW 0x006aaa34
tU32 gLast_checkpoint_time;

// GLOBAL: CARMA2_HW 0x00660cdc
int gLast_wrong_checkpoint = -1;

// GLOBAL: CARMA2_HW 0x006aaa38
int gHud_tinted1_visible;

// GLOBAL: CARMA2_HW 0x006aaa3c
int gHud_tinted2_visible;

// FUNCTION: CARMA2_HW 0x004e3410
void C2_HOOK_FASTCALL RecordAPOAndCredits(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, power_up_levels, 0x18a8);

    gMoney_pre_race = gProgram_state.credits;
    gAPO_pre_race[0] = gProgram_state.current_car.power_up_levels[0];
    gAPO_pre_race[1] = gProgram_state.current_car.power_up_levels[1];
    gAPO_pre_race[2] = gProgram_state.current_car.power_up_levels[2];
}

// FUNCTION: CARMA2_HW 0x004e3440
int C2_HOOK_FASTCALL DoPostRace(tRace_result pRace_result) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00503c50
void C2_HOOK_FASTCALL DoProgram(void) {

    InitialiseProgramState();
    while (gProgram_state.prog_status != eProg_quit) {
        switch (gProgram_state.prog_status) {
        case eProg_intro:
            DisposeGameIfNecessary();
            DoLogos();
            break;
        case eProg_opening:
            DisposeGameIfNecessary();
            DoProgOpeningAnimation();
            break;
        case eProg_idling:
            DisposeGameIfNecessary();
            if (gNo_current_game) {
                LoadRaces(gRace_list, &gNumber_of_races, -1);
                InitGame(gDev_initial_race);
                MaybeRestoreSavedGame();
            }
            if (gAuto_load && gLoad_last_save_game) {
                DoLoadMostRecentGame();
                gLoad_last_save_game = 0;
            }
            DoMainScreen();
            break;
        case eProg_demo:
            DoProgramDemo();
            break;
        case eProg_game_starting:
            DoGame();
            break;
        default:
            break;
        }
    }
}

void C2_HOOK_FASTCALL InitialiseProgramState(void) {

    gProgram_state.mirror_on = gMirror_on__structur;
    gProgram_state.prog_status = eProg_intro;
    gProgram_state.last_slot = 0;
    gProgram_state.view_type = eVT_Scene;
    gProgram_state.frank_or_anniness = eFrankie;
    gProgram_state.racing = 0;
    gProgram_state.cut_scene = 0;
    gProgram_state.saving = 0;
    gProgram_state.loading = 0;
    gProgram_state.dont_save_or_load = 0;
    gProgram_state.dont_load = 0;
    gProgram_state.field_0x2c = 0;
    if (gAusterity_mode) {
        gProgram_state.prat_cam_on = 0;
    } else {
        gProgram_state.prat_cam_on = gPratcam_on;
    }
    gPratcam_on = gProgram_state.prat_cam_on;
    gProgram_state.cockpit_on = gCockpit_on;
    gProgram_state.car_name[0] = '\0';
    SetSoundVolumes(0);
}

void C2_HOOK_FASTCALL DoLogos(void) {

    ClearEntireScreen();
    DoSCILogo();
    DoOpeningAnimation();
    DoStainlessLogo();
    gProgram_state.prog_status = eProg_opening;
}

void C2_HOOK_FASTCALL DoProgramDemo(void) {

    DoLogos();
    gProgram_state.prog_status = eProg_idling;
    DRS3StopOutletSound(gEffects_outlet);
}

void C2_HOOK_FASTCALL DoProgOpeningAnimation(void) {

    gProgram_state.prog_status = eProg_idling;
    DRS3StopOutletSound(gEffects_outlet);
}

void C2_HOOK_FASTCALL SwapNetCarsLoad(void) {
    int i;

    DisableNetService();
    AboutToLoadFirstCar();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].next_car_index >= 0) {
            gNet_players[i].car_index = gNet_players[i].next_car_index;
        }
        gNet_players[i].next_car_index = -1;
        LoadCar(gOpponents[gNet_players[i].car_index].car_file_name,
            (gThis_net_player_index == i) ? eDriver_local_human : eDriver_net_human,
            gNet_players[i].car, gNet_players[i].car_index, gNet_players[i].player_name,
            &gNet_cars_storage_space);
    }
    ReenableNetService();
}

void C2_HOOK_FASTCALL SwapNetCarsDispose(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tCar_detail_info) != 20);

    DisableNetService();
    for (i = 0; i < gNumber_of_net_players; i++) {
        DisposeCar(gNet_players[i].car, gNet_players[i].car_index);
        if (gNet_players[i].car_index >= 0) {
            gCar_details[gNet_players[i].car_index].ownership = eCar_owner_none;
        }
    }
    ClearOutStorageSpace(&gOur_car_storage_space);
    ClearOutStorageSpace(&gNet_cars_storage_space);
    ReenableNetService();
}

void C2_HOOK_FASTCALL DoGame(void) {
    tSO_result options_result;
    tRace_result race_result;
    int i;

    PrintMemoryDump(0,"START OF DoGame()");
    gAbandon_game = 0;
    gDisallow_abandon_race = 0;
    gCar_to_view = &gProgram_state.current_car;
    StartLoadingScreen();
    gProgram_state.prog_status = eProg_game_ongoing;
    if (gNet_mode != gNet_mode_of_last_game) {
        gProgram_state.prog_status = eProg_idling;
        return;
    }
    PrintMemoryDump(0, "BEFORE START RACE SCREEN");
    LoadRaceInfo(gProgram_state.current_race_index, &gCurrent_race);
    SelectOpponents(&gCurrent_race);
    if (gNet_mode == eNet_mode_none) {
        SelectOpponents(&gCurrent_race);
        LoadRaceInfo(gProgram_state.current_race_index, &gCurrent_race);
        FillInRaceInfo(&gCurrent_race);
    } else {
        FillInRaceInfo(&gCurrent_race);
        DisposeRaceInfo(&gCurrent_race);
    }
    PrintMemoryDump(0, "AFTER START RACE SCREEN");
    if (gNet_mode != eNet_mode_none) {
        if ((gCurrent_net_game->options.car_choice == eNet_car_novelty || gCurrent_net_game->options.car_choice == eNet_car_all)
                && !gNo_races_yet) {
            SwapNetCarsLoad();
        }
    }
    PrintMemoryDump(0, "AFTER LOADING OPPONENTS IN");
    InitRace();
    if (gNet_mode != gNet_mode_of_last_game) {
        gProgram_state.prog_status = eProg_idling;
        return;
    }
    if (gNet_mode == eNet_mode_none) {
        DoGridPosition();
        SetInitialPositions(&gCurrent_race);
    } else {
        FRONTEND_Main(kFrontend_menu_netsync);
        dr_dprintf("Returned from NetSync screen");
        SwitchToLoresMode();
    }
    if (gProgram_state.prog_status == eProg_idling) {
        dr_dprintf("Calling DisposeRace()");
        DisposeRace();
        dr_dprintf("Calling DisposeTrack()");
        DisposeTrack();
        dr_dprintf("Returning from DoGame()");
        return;
    }
    InitialiseAIWorldForRace(&gCurrent_race);
    InitialiseCarsEtc(&gCurrent_race);
    InitSoundSources();
    InitLastDamageArrayEtc();
    FogificateMaterials(kRendererShadingType_Default);
    EnableAutoFogification();
    RecordAPOAndCredits();
    race_result = DoRace();
    DisableAutoFogification();
    DisposeRace();
    PrintMemoryDump(0, "AFTER DISPOSING RACE");
    if (gNet_mode == eNet_mode_none) {
        options_result = DoPostRace(race_result);
        if (race_result == eRace_completed) {
            gSave_game_out_of_sync = 1;
            DoEndRaceAnimation();
            DoEndRaceSummary(eRace_completed, options_result);
            DoSaveGame();
        }
    } else {
        gProgram_state.current_race_index = gPending_race;
        gCurrent_net_game->start_race = gPending_race;
        gPending_race = -1;
        if (race_result != eRace_game_abandonned && race_result != eRace_aborted) {
            DoEndRaceSummary(race_result, 0);
        }
    }
    if (gNet_mode == eNet_mode_none) {
        TotallyRepairCar();
    } else {
        for (i = 0; i < gNumber_of_net_players; i++) {
            TotallyRepairACar(gNet_players[i].car);
        }
    }
    if (gNet_mode == eNet_mode_none) {
        DisposeOpponentsCars(&gCurrent_race);
    } else {
        if ((gCurrent_net_game->options.car_choice == eNet_car_all || gCurrent_net_game->options.car_choice == eNet_car_novelty)
                && !gNo_races_yet) {
            SwapNetCarsDispose();
        }
    }
    PrintMemoryDump(0, "AFTER DISPOSING CARS");
    DisposeTrack();
    PrintMemoryDump(0, "AFTER DISPOSING TRACK");
    if (gProgram_state.prog_status == eProg_game_ongoing) {
        if (gNet_mode == eNet_mode_none) {
            gProgram_state.prog_status = eProg_idling;
        } else {
            gProgram_state.prog_status = eProg_game_starting;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00503b00
int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness) {
    int i;
    int count;
    int temp_array[40];

    while (1) {
        count = 0;
        for (i = 0; i < gNumber_of_racers; i++) {
            if ((pNastiness == 0 || gOpponents[i].strength_rating == pNastiness)
                && gProgram_state.current_car.index != i
                && !gOpponents[i].picked
                && gOpponents[i].strength_rating < 6
                && gOpponents[i].strength_rating > 0) {
                temp_array[count] = i;
                count++;
            }
        }
        if (count != 0) {
            break;
        }
        if (pNastiness < 5) {
            pNastiness++;
        } else {
            pNastiness = 0;
        }
    }
    i = temp_array[IRandomBetween(0, count - 1)];
    gOpponents[i].picked = 1;
    return i;
}

int C2_HOOK_FASTCALL NumberOfOpponentsStillRunning(void) {
    int i;
    int car_count;
    int result;
    tCar_spec* the_car;

    result = 0;
    car_count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < car_count; i++) {
        the_car = GetCarSpec(eVehicle_opponent, i);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, knackered, 0x1d4);

        if (!the_car->knackered) {
            result++;
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x005039e0
void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        return;
    }

    pRace_info->number_of_racers = pRace_info->race_spec->count_explicit_opponents;
    for (i = 0; i < gNumber_of_racers; i++) {
        gOpponents[i].picked = 0;
    }
    for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
        if (pRace_info->race_spec->explicit_opponents[i] >= 0) {
            gOpponents[i].picked = 1;
            pRace_info->opponent_list[i].index = pRace_info->race_spec->explicit_opponents[i];
            pRace_info->opponent_list[i].ranking = -(i + 1);
        }
    }
    for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
        if (pRace_info->race_spec->explicit_opponents[i] < 0) {
            pRace_info->opponent_list[i].index = ChooseOpponent(-pRace_info->race_spec->explicit_opponents[i]);
            pRace_info->opponent_list[i].ranking = IRandomBetween(0, 99);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00503050
int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void) {
    int result;
    int count;
    int i;

    result = 0;
    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        tCar_spec* car_spec = GetCarSpec(eVehicle_opponent, i);

        if (!car_spec->knackered && (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || (gCurrent_race.race_spec->race_type == kRaceType_Cars && car_spec->is_race_goal))) {
            result += 1;
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x005030b0
void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason) {

    if (gRace_finished) {
        return;
    }
    if (gNet_mode == eNet_mode_host && pReason < eRace_over_8) {
        NetFinishRace(gCurrent_net_game, pReason);
    }
    if (pReason == eRace_over_out_of_time || pReason == eRace_over_7) {
        ChangeAmbientPratcam(35);
    } else if (pReason < eRace_over_abandoned) {
        ChangeAmbientPratcam(34);
    }
    gRace_over_reason = pReason;
    if (gMap_view == 2) {
        ToggleMap();
    }
    switch (gRace_over_reason) {
    case eRace_over_0:
    case eRace_over_1:
    case eRace_over_2:
    case eRace_over_3:
        ChangeAmbientPratcam(34);
        DoFancyHeadup(gCurrent_race.race_spec->is_boundary ? 34 : 23);
        DRS3StartSound(gPedestrians_outlet, eSoundId_RaceComplete);
        break;
    case eRace_over_abandoned:
        if (gNet_mode == eNet_mode_client) {
            gHost_abandon_game = 1;
            NetFullScreenMessage(eMiscString_host_abandoned_race, 0);
        }
        break;
    case eRace_over_out_of_time:
        ChangeAmbientPratcam(35);
        DoFancyHeadup(gCurrent_race.race_spec->is_boundary ? 33 : 22);
        DRS3StartSound(gPedestrians_outlet, eSoundId_OutOfTime2);
        break;
    case eRace_over_6:
        ChangeAmbientPratcam(36);
        DoFancyHeadup(gCurrent_race.race_spec->is_boundary ? 33 : 24);
        break;
    case eRace_over_7:
        ChangeAmbientPratcam(35);
        DoFancyHeadup(35);
        break;
    case eRace_over_8:
        ChangeAmbientPratcam(34);
        DoFancyHeadup(29);
        break;
    case eRace_over_9:
        ChangeAmbientPratcam(36);
        DoFancyHeadup(26);
        break;
    default:
        break;
    }
    if (gNet_mode == eNet_mode_none) {
        gRace_finished = 10000;
    } else {
        gRace_finished = 8000;
    }
}

// FUNCTION: CARMA2_HW 0x00503270
void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound) {

    PratcamEvent(33);
    DoFancyHeadup(21);
    if (pDo_sound) {
        DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
    }
}

// FUNCTION: CARMA2_HW 0x005032a0
void C2_HOOK_FASTCALL IncrementCheckpoint(void) {
    int done_voice;

    if (gRace_finished) {
        return;
    }
    if (!(gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints)) {
        return;
    }
    done_voice = 0;
    gLast_checkpoint_time = GetTotalTime();
    if (gCheckpoint < gCheckpoint_count) {
        gCheckpoint += 1;
    } else {
        gCheckpoint = 1;
        if (gLap < gTotal_laps) {
            gLap += 1;
            if (gLap == gTotal_laps) {
                PratcamEvent(33);
                NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_final_lap));
                DRS3StartSound(gPedestrians_outlet, eSoundId_FinalLap);
                done_voice = 1;
            }
        } else {
            gLap = gTotal_laps;
            gCheckpoint = gCheckpoint_count;
            RaceCompleted(eRace_over_0);
        }
    }
    if (!gRace_finished) {
        PratcamEvent(33);
        DoFancyHeadup(21);
        if (!done_voice) {
            DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
        }
        if (gCredits_checkpoint[gProgram_state.skill_level] != 0) {
            EarnCredits(gCredits_checkpoint[gProgram_state.skill_level]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x005033b0
void C2_HOOK_FASTCALL IncrementLap(void) {
    int i;

    for (i = gCheckpoint; i <= gCheckpoint_count; i++) {
        IncrementCheckpoint();
    }
}

// FUNCTION: CARMA2_HW 0x005033e0
void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index) {

    if (gRace_finished) {
        return;
    }
    if (!(gCurrent_race.race_spec->race_type == kRaceType_Carma1 ||
        gCurrent_race.race_spec->race_type == kRaceType_Checkpoints)) {
        return;
    }
    if ((gLast_wrong_checkpoint == pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 20000)
            || (gLast_wrong_checkpoint != pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 2000)) {

        if (gNet_mode == eNet_mode_none) {
            int next_checkpoint = pCheckpoint_index + 2;
            if (gCurrent_race.check_point_count < next_checkpoint) {
                if (gLap == 1) {
                    next_checkpoint = -1;
                } else {
                    next_checkpoint = 1;
                }
            }
            if (next_checkpoint == gCheckpoint) {
                return;
            }
        }
    }
    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_wrong_checkpoint));
    DRS3StartSound(gPedestrians_outlet, eSoundId_WrongCheckpoint);
    gLast_checkpoint_time = GetTotalTime();
    gLast_wrong_checkpoint = pCheckpoint_index;
}

int C2_HOOK_FASTCALL RayHitFace(br_vector3* pV0, br_vector3* pV1, br_vector3* pV2, br_vector3* pNormal, br_vector3* pStart, br_vector3* pDir) {
    tFace_ref the_face;
    br_scalar rt;
    br_vector3 rp;

    the_face.material = NULL;
    BrVector3Copy(&the_face.v[0], pV0);
    BrVector3Copy(&the_face.v[1], pV1);
    BrVector3Copy(&the_face.v[2], pV2);
    BrVector3Copy(&the_face.normal, pNormal);
    CheckSingleFace(&the_face, pStart, pDir, &the_face.normal, &rt, &rp);
    return rt >= 0.f && rt <= 1.f;
}

// FUNCTION: CARMA2_HW 0x005034b0
void C2_HOOK_FASTCALL CheckCheckpoints(void) {
    tCar_spec* car;
    br_vector3 orig;
    br_vector3 dir;
    int i;
    int j;
    int cat;
    int car_count;
    int car_index;
    tNet_game_player_info* net_player;

    if (gNet_mode == eNet_mode_client) {
        return;
    }
    if (gNet_mode == eNet_mode_host && gCurrent_net_game->type != eNet_game_type_5 && gCurrent_net_game->type != eNet_game_type_checkpoint && gCurrent_net_game->type != eNet_game_type_4) {
        return;
    }
    for (cat = eVehicle_self; cat <= gNet_mode; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (car_index = 0; car_index < car_count; car_index++) {
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, car_index);
            }
            BrVector3Copy(&orig, (br_vector3*)car->old_frame_mat.m[3]);
            BrVector3Sub(&dir, &car->car_master_actor->t.t.translate.t, &orig);
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                for (j = 0; j < gCurrent_race.checkpoints[i].count_quads; j++) {
                    if (
                            RayHitFace(&gCurrent_race.checkpoints[i].quads[j].points[0],
                                       &gCurrent_race.checkpoints[i].quads[j].points[1],
                                       &gCurrent_race.checkpoints[i].quads[j].points[2],
                                       &gCurrent_race.checkpoints[i].normals[j],
                                       &orig, &dir)
                            || RayHitFace(&gCurrent_race.checkpoints[i].quads[j].points[0],
                                          &gCurrent_race.checkpoints[i].quads[j].points[2],
                                          &gCurrent_race.checkpoints[i].quads[j].points[3],
                                          &gCurrent_race.checkpoints[i].normals[j],
                                          &orig,
                                          &dir)) {
                        if (gNet_mode == eNet_mode_none) {
                            if (i + 1 == gCheckpoint) {
                                IncrementCheckpoint();
                            } else {
                                WrongCheckpoint(i);
                            }
                        } else {
                            net_player = NetPlayerFromCar(car);
                            if (gCurrent_net_game->type == eNet_game_type_checkpoint) {
                                if (net_player->score2 & (1 << i)) {
                                    net_player->score2 &= ~(1 << i);
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            } else if (gCurrent_net_game->type == eNet_game_type_5) {
                                if ((net_player->score2 % gCurrent_race.check_point_count) == i) {
                                    net_player->score2 += 1;
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                    gLast_checkpoint_time = GetTotalTime();
                                    if (net_player->score2 >= gCurrent_net_game->options.starting_target * gCurrent_race.check_point_count) {
                                        DeclareWinner(net_player - gNet_players);
                                    }
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            } else {
                                if ((net_player->score2 % gCurrent_race.check_point_count) == i) {
                                    net_player->score2 += 1;
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                    break;
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            }
                        }
                        break;
                    }
                }
            }
            BrMatrix34Copy(&car->old_frame_mat, &car->car_master_actor->t.t.mat);
        }
    }
}

// FUNCTION: CARMA2_HW 0x005039b0
void C2_HOOK_FASTCALL TotalRepair(void) {

    TotallyRepairCar();
    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_instant_repair));
}

// FUNCTION: CARMA2_HW 0x00503ba0
int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence) {
    int i;
    int new_index;
    int races_count;
    int most_seldom_seen;
    int races_to_pick_from[50];

    if (pNet_race_sequence == eNet_sequence_sequential) {
        new_index = pCurrent_race + 1;
        if (new_index >= gNumber_of_races) {
            new_index = 0;
        }
    } else {
        most_seldom_seen = 10000;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].been_there_done_that < most_seldom_seen) {
                most_seldom_seen = gRace_list[i].been_there_done_that;
            }
        }
        races_count = 0;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].been_there_done_that == most_seldom_seen && (i != pCurrent_race)) {
                races_to_pick_from[races_count] = i;
                races_count++;
            }
        }
        new_index = races_to_pick_from[IRandomBetween(0, races_count - 1)];
        gRace_list[new_index].been_there_done_that += 1;
    }
    return new_index;
}

// FUNCTION: CARMA2_HW 0x00504170
void C2_HOOK_FASTCALL JumpTheStart(void) {
    char s[256];

    if (gNet_mode == eNet_mode_none || gProgram_state.credits >= gJump_start_fine[gProgram_state.skill_level]) {
        WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(gCountdown);
        gCountdown = 0;
        DRS3StopOutletSound(gPedestrians_outlet);
        DRS3StartSound(gPedestrians_outlet, eSoundId_HeyYou);
        SpendCredits(gJump_start_fine[gProgram_state.skill_level]);
        c2_sprintf(s, "%s %d %s",
            GetMiscString(eMiscString_bad_boy),
            gJump_start_fine[gProgram_state.skill_level],
            GetMiscString(eMiscString_credit_fine));
        NewTextHeadupSlot(4, 0, 1000, -4, s);
    }
}

// FUNCTION: CARMA2_HW 0x00504230
void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void) {

    gInterface_within_race_mode = 1;
    gHud_tinted1_visible = !!TintedPolyIsOn(gHud_tinted1);
    gHud_tinted2_visible = !!TintedPolyIsOn(gHud_tinted2);
    TurnTintedPolyOff(gHud_tinted1);
    TurnTintedPolyOff(gHud_tinted2);
    PlayFlicsFromDisk();
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
}

// FUNCTION: CARMA2_HW 0x005042a0
void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void) {

    gInterface_within_race_mode = 0;
    if (gHud_tinted1_visible) {
        TurnTintedPolyOn(gHud_tinted1);
    }
    if (gHud_tinted2_visible) {
        TurnTintedPolyOn(gHud_tinted2);
    }
    SwitchToLoresMode();
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_unpaused, 0, 0, 0, 0);
    }
}

// FUNCTION: CARMA2_HW 0x0049bcb0
void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {

    NOT_IMPLEMENTED();
}
