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

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gMirror_on__structur, 0x00660ce0, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gLoad_last_save_game, 0x00660ce8, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMoney_pre_race, 0x007051f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gAPO_pre_race, 3, 0x007051e0);
C2_HOOK_VARIABLE_IMPLEMENT(tRace_over_reason, gRace_over_reason, 0x006b781c);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_checkpoint_time, 0x006aaa34);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gLast_wrong_checkpoint, 0x00660cdc, -1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHud_tinted1_visible, 0x006aaa38);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHud_tinted2_visible, 0x006aaa3c);

void C2_HOOK_FASTCALL StashCreditsAndAPO(void) {

    C2V(gMoney_pre_race) = C2V(gProgram_state).credits;
    C2V(gAPO_pre_race)[0] = C2V(gProgram_state).current_car.power_up_levels[0];
    C2V(gAPO_pre_race)[1] = C2V(gProgram_state).current_car.power_up_levels[1];
    C2V(gAPO_pre_race)[2] = C2V(gProgram_state).current_car.power_up_levels[2];
}
C2_HOOK_FUNCTION(0x004e3410, StashCreditsAndAPO)

int (C2_HOOK_FASTCALL * DoPostRace_original)(tRace_result pRace_result);
int C2_HOOK_FASTCALL DoPostRace(tRace_result pRace_result) {

#if defined(C2_HOOKS_ENABLED)
    return DoPostRace_original(pRace_result);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e3440, DoPostRace, DoPostRace_original)

void (C2_HOOK_FASTCALL * DoProgram_original)(void);
void C2_HOOK_FASTCALL DoProgram(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    DoProgram_original();
#else
    InitialiseProgramState();
    while (C2V(gProgram_state).prog_status != eProg_quit) {
        switch (C2V(gProgram_state).prog_status) {
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
            if (C2V(gNo_current_game)) {
                LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), -1);
                InitGame(C2V(gDev_initial_race));
                MaybeRestoreSavedGame();
            }
            if (C2V(gAuto_load) && C2V(gLoad_last_save_game)) {
                DoLoadMostRecentGame();
                C2V(gLoad_last_save_game) = 0;
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00503c50, DoProgram, DoProgram_original);

void C2_HOOK_FASTCALL InitialiseProgramState(void) {

    C2V(gProgram_state).mirror_on = C2V(gMirror_on__structur);
    C2V(gProgram_state).prog_status = eProg_intro;
    C2V(gProgram_state).last_slot = 0;
    C2V(gProgram_state).view_type = eVT_Scene;
    C2V(gProgram_state).frank_or_anniness = eFrankie;
    C2V(gProgram_state).racing = 0;
    C2V(gProgram_state).cut_scene = 0;
    C2V(gProgram_state).saving = 0;
    C2V(gProgram_state).loading = 0;
    C2V(gProgram_state).dont_save_or_load = 0;
    C2V(gProgram_state).dont_load = 0;
    C2V(gProgram_state).field_0x2c = 0;
    if (C2V(gAusterity_mode)) {
        C2V(gProgram_state).prat_cam_on = 0;
    } else {
        C2V(gProgram_state).prat_cam_on = C2V(gPratcam_on);
    }
    C2V(gPratcam_on) = C2V(gProgram_state).prat_cam_on;
    C2V(gProgram_state).cockpit_on = C2V(gCockpit_on);
    C2V(gProgram_state).car_name[0] = '\0';
    SetSoundVolumes(0);
}

void C2_HOOK_FASTCALL DoLogos(void) {

    ClearEntireScreen();
    DoSCILogo();
    DoOpeningAnimation();
    DoStainlessLogo();
    C2V(gProgram_state).prog_status = eProg_opening;
}

void C2_HOOK_FASTCALL DoProgramDemo(void) {

    DoLogos();
    C2V(gProgram_state).prog_status = eProg_idling;
    DRS3StopOutletSound(C2V(gEffects_outlet));
}

void C2_HOOK_FASTCALL DoProgOpeningAnimation(void) {

    C2V(gProgram_state).prog_status = eProg_idling;
    DRS3StopOutletSound(C2V(gEffects_outlet));
}

void C2_HOOK_FASTCALL SwapNetCarsLoad(void) {
    int i;

    DisableNetService();
    AboutToLoadFirstCar();
    for (i = 0; i < C2V(gNumber_of_net_players); i++) {
        if (C2V(gNet_players)[i].next_car_index >= 0) {
            C2V(gNet_players)[i].car_index = C2V(gNet_players)[i].next_car_index;
        }
        C2V(gNet_players)[i].next_car_index = -1;
        LoadCar(C2V(gOpponents)[C2V(gNet_players)[i].car_index].car_file_name,
            (C2V(gThis_net_player_index) == i) ? eDriver_local_human : eDriver_net_human,
            C2V(gNet_players)[i].car, C2V(gNet_players)[i].car_index, C2V(gNet_players)[i].player_name,
            &C2V(gNet_cars_storage_space));
    }
    ReenableNetService();
}

void C2_HOOK_FASTCALL SwapNetCarsDispose(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tCar_detail_info) != 20);

    DisableNetService();
    for (i = 0; i < C2V(gNumber_of_net_players); i++) {
        DisposeCar(C2V(gNet_players)[i].car, C2V(gNet_players)[i].car_index);
        if (C2V(gNet_players)[i].car_index >= 0) {
            C2V(gCar_details)[C2V(gNet_players)[i].car_index].ownership = eCar_owner_none;
        }
    }
    ClearOutStorageSpace(&C2V(gOur_car_storage_space));
    ClearOutStorageSpace(&C2V(gNet_cars_storage_space));
    ReenableNetService();
}

void C2_HOOK_FASTCALL DoGame(void) {
    tSO_result options_result;
    tRace_result race_result;
    int i;

    PrintMemoryDump(0,"START OF DoGame()");
    C2V(gAbandon_game) = 0;
    C2V(gDisallow_abandon_race) = 0;
    C2V(gCar_to_view) = &C2V(gProgram_state).current_car;
    StartLoadingScreen();
    C2V(gProgram_state).prog_status = eProg_game_ongoing;
    if (C2V(gNet_mode) != C2V(gNet_mode_of_last_game)) {
        C2V(gProgram_state).prog_status = eProg_idling;
        return;
    }
    PrintMemoryDump(0, "BEFORE START RACE SCREEN");
    LoadRaceInfo(C2V(gProgram_state).current_race_index, &C2V(gCurrent_race));
    SelectOpponents(&C2V(gCurrent_race));
    if (C2V(gNet_mode) == eNet_mode_none) {
        SelectOpponents(&C2V(gCurrent_race));
        LoadRaceInfo(C2V(gProgram_state).current_race_index, &C2V(gCurrent_race));
        FillInRaceInfo(&C2V(gCurrent_race));
    } else {
        FillInRaceInfo(&C2V(gCurrent_race));
        DisposeRaceInfo(&C2V(gCurrent_race));
    }
    PrintMemoryDump(0, "AFTER START RACE SCREEN");
    if (C2V(gNet_mode) != eNet_mode_none) {
        if ((C2V(gCurrent_net_game)->options.car_choice == eNet_car_novelty || C2V(gCurrent_net_game)->options.car_choice == eNet_car_all)
                && !C2V(gNo_races_yet)) {
            SwapNetCarsLoad();
        }
    }
    PrintMemoryDump(0, "AFTER LOADING OPPONENTS IN");
    InitRace();
    if (C2V(gNet_mode) != C2V(gNet_mode_of_last_game)) {
        C2V(gProgram_state).prog_status = eProg_idling;
        return;
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        DoGridPosition();
        SetInitialPositions(&C2V(gCurrent_race));
    } else {
        FRONTEND_Main(kFrontend_menu_netsync);
        dr_dprintf("Returned from NetSync screen");
        SwitchToLoresMode();
    }
    if (C2V(gProgram_state).prog_status == eProg_idling) {
        dr_dprintf("Calling DisposeRace()");
        DisposeRace();
        dr_dprintf("Calling DisposeTrack()");
        DisposeTrack();
        dr_dprintf("Returning from DoGame()");
        return;
    }
    InitOpponentsAndDrones(&C2V(gCurrent_race));
    InitialiseCarsEtc(&C2V(gCurrent_race));
    InitSoundSources();
    InitLastDamageArrayEtc();
    AdaptFoggedMaterialsForRenderer(kRendererShadingType_Default);
    EnableMaterialAdapt();
    StashCreditsAndAPO();
    race_result = DoRace();
    DisableAutoFogification();
    DisposeRace();
    PrintMemoryDump(0, "AFTER DISPOSING RACE");
    if (C2V(gNet_mode) == eNet_mode_none) {
        options_result = DoPostRace(race_result);
        if (race_result == eRace_completed) {
            C2V(gSave_game_out_of_sync) = 1;
            DoEndRaceAnimation();
            DoEndRaceSummary(eRace_completed, options_result);
            WriteSaveGame();
        }
    } else {
        C2V(gProgram_state).current_race_index = C2V(gPending_race);
        C2V(gCurrent_net_game)->start_race = C2V(gPending_race);
        C2V(gPending_race) = -1;
        if (race_result != eRace_game_abandonned && race_result != eRace_aborted) {
            DoEndRaceSummary(race_result, 0);
        }
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        TotallyRepairCar();
    } else {
        for (i = 0; i < C2V(gNumber_of_net_players); i++) {
            TotallyRepairACar(C2V(gNet_players)[i].car);
        }
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        DisposeAllCars(&C2V(gCurrent_race));
    } else {
        if ((C2V(gCurrent_net_game)->options.car_choice == eNet_car_all || C2V(gCurrent_net_game)->options.car_choice == eNet_car_novelty)
                && !C2V(gNo_races_yet)) {
            SwapNetCarsDispose();
        }
    }
    PrintMemoryDump(0, "AFTER DISPOSING CARS");
    DisposeTrack();
    PrintMemoryDump(0, "AFTER DISPOSING TRACK");
    if (C2V(gProgram_state).prog_status == eProg_game_ongoing) {
        if (C2V(gNet_mode) == eNet_mode_none) {
            C2V(gProgram_state).prog_status = eProg_idling;
        } else {
            C2V(gProgram_state).prog_status = eProg_game_starting;
        }
    }
}

int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness) {
    int i;
    int count;
    int temp_array[40];

    while (1) {
        count = 0;
        for (i = 0; i < C2V(gNumber_of_racers); i++) {
            if ((pNastiness == 0 || C2V(gOpponents)[i].strength_rating == pNastiness)
                && C2V(gProgram_state).current_car.index != i
                && !C2V(gOpponents)[i].picked
                && C2V(gOpponents)[i].strength_rating < 6
                && C2V(gOpponents)[i].strength_rating > 0) {
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
    C2V(gOpponents)[i].picked = 1;
    return i;
}
C2_HOOK_FUNCTION(0x00503b00, ChooseOpponent)

int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void) {
    int i;
    int car_count;
    int result;
    tCar_spec* the_car;

    result = 0;
    car_count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < car_count; i++) {
        the_car = GetCarSpec(eVehicle_opponent, i);
        if (!the_car->knackered) {
            result++;
        }
    }
    return result;
}

void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info) {
    int i;

    if (C2V(gNet_mode) != eNet_mode_none) {
        return;
    }

    pRace_info->number_of_racers = pRace_info->race_spec->count_explicit_opponents;
    for (i = 0; i < C2V(gNumber_of_racers); i++) {
        C2V(gOpponents)[i].picked = 0;
    }
    for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
        if (pRace_info->race_spec->explicit_opponents[i] >= 0) {
            C2V(gOpponents)[i].picked = 1;
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
C2_HOOK_FUNCTION(0x005039e0, SelectOpponents)

int C2_HOOK_FASTCALL NumberOfOpponentObjectivesLeft(void) {
    int result;
    int count;
    int i;

    result = 0;
    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        tCar_spec* car_spec = GetCarSpec(eVehicle_opponent, i);

        if (!car_spec->knackered && (C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1 || (C2V(gCurrent_race).race_spec->race_type == kRaceType_Cars && car_spec->is_race_goal))) {
            result += 1;
        }
    }
    return result;
}
C2_HOOK_FUNCTION(0x00503050, NumberOfOpponentObjectivesLeft)

void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason) {

    if (C2V(gRace_finished)) {
        return;
    }
    if (C2V(gNet_mode) == eNet_mode_host && pReason < eRace_over_8) {
        NetFinishRace(C2V(gCurrent_net_game), pReason);
    }
    if (pReason == eRace_over_out_of_time || pReason == eRace_over_7) {
        ChangeAmbientPratcam(35);
    } else if (pReason < eRace_over_abandoned) {
        ChangeAmbientPratcam(34);
    }
    C2V(gRace_over_reason) = pReason;
    if (C2V(gMap_view) == 2) {
        ToggleMap();
    }
    switch (C2V(gRace_over_reason)) {
    case eRace_over_0:
    case eRace_over_1:
    case eRace_over_2:
    case eRace_over_3:
        ChangeAmbientPratcam(34);
        DoFancyHeadup(C2V(gCurrent_race).race_spec->is_boundary ? 34 : 23);
        DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_RaceComplete);
        break;
    case eRace_over_abandoned:
        if (C2V(gNet_mode) == eNet_mode_client) {
            C2V(gHost_abandon_game) = 1;
            NetFullScreenMessage(eMiscString_host_abandoned_race, 0);
        }
        break;
    case eRace_over_out_of_time:
        ChangeAmbientPratcam(35);
        DoFancyHeadup(C2V(gCurrent_race).race_spec->is_boundary ? 33 : 22);
        DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_OutOfTime2);
        break;
    case eRace_over_6:
        ChangeAmbientPratcam(36);
        DoFancyHeadup(C2V(gCurrent_race).race_spec->is_boundary ? 33 : 24);
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
    if (C2V(gNet_mode) == eNet_mode_none) {
        C2V(gRace_finished) = 10000;
    } else {
        C2V(gRace_finished) = 8000;
    }
}
C2_HOOK_FUNCTION(0x005030b0, RaceCompleted)

void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound) {

    PratcamEvent(33);
    DoFancyHeadup(21);
    if (pDo_sound) {
        DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_Checkpoint);
    }
}
C2_HOOK_FUNCTION(0x00503270, Checkpoint)

void C2_HOOK_FASTCALL IncrementCheckpoint(void) {
    int done_voice;

    if (C2V(gRace_finished)) {
        return;
    }
    if (!(C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1 || C2V(gCurrent_race).race_spec->race_type == kRaceType_Checkpoints)) {
        return;
    }
    done_voice = 0;
    C2V(gLast_checkpoint_time) = GetTotalTime();
    if (C2V(gCheckpoint) < C2V(gCheckpoint_count)) {
        C2V(gCheckpoint) += 1;
    } else {
        C2V(gCheckpoint) = 1;
        if (C2V(gLap) < C2V(gTotal_laps)) {
            C2V(gLap) += 1;
            if (C2V(gLap) == C2V(gTotal_laps)) {
                PratcamEvent(33);
                NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_final_lap));
                DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_FinalLap);
                done_voice = 1;
            }
        } else {
            C2V(gLap) = C2V(gTotal_laps);
            C2V(gCheckpoint) = C2V(gCheckpoint_count);
            RaceCompleted(eRace_over_0);
        }
    }
    if (!C2V(gRace_finished)) {
        PratcamEvent(33);
        DoFancyHeadup(21);
        if (!done_voice) {
            DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_Checkpoint);
        }
        if (C2V(gCredits_checkpoint[C2V(gProgram_state).skill_level]) != 0) {
            EarnCredits(C2V(gCredits_checkpoint[C2V(gProgram_state).skill_level]));
        }
    }
}
C2_HOOK_FUNCTION(0x005032a0, IncrementCheckpoint)

void C2_HOOK_FASTCALL IncrementLap(void) {
    int i;

    for (i = C2V(gCheckpoint); i <= C2V(gCheckpoint_count); i++) {
        IncrementCheckpoint();
    }
}
C2_HOOK_FUNCTION(0x005033b0, IncrementLap)

void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index) {

    if (C2V(gRace_finished)) {
        return;
    }
    if (!(C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1 ||
        C2V(gCurrent_race).race_spec->race_type == kRaceType_Checkpoints)) {
        return;
    }
    if ((C2V(gLast_wrong_checkpoint) == pCheckpoint_index && GetTotalTime() - C2V(gLast_checkpoint_time) > 20000)
            || (C2V(gLast_wrong_checkpoint) != pCheckpoint_index && GetTotalTime() - C2V(gLast_checkpoint_time) > 2000)) {

        if (C2V(gNet_mode) == eNet_mode_none) {
            int next_checkpoint = pCheckpoint_index + 2;
            if (C2V(gCurrent_race).check_point_count < next_checkpoint) {
                if (C2V(gLap) == 1) {
                    next_checkpoint = -1;
                } else {
                    next_checkpoint = 1;
                }
            }
            if (next_checkpoint == C2V(gCheckpoint)) {
                return;
            }
        }
    }
    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_wrong_checkpoint));
    DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_WrongCheckpoint);
    C2V(gLast_checkpoint_time) = GetTotalTime();
    C2V(gLast_wrong_checkpoint) = pCheckpoint_index;
}
C2_HOOK_FUNCTION(0x005033e0, WrongCheckpoint)

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

    if (C2V(gNet_mode) == eNet_mode_client) {
        return;
    }
    if (C2V(gNet_mode) == eNet_mode_host && C2V(gCurrent_net_game)->type != eNet_game_type_5 && C2V(gCurrent_net_game)->type != eNet_game_type_3 && C2V(gCurrent_net_game)->type != eNet_game_type_4) {
        return;
    }
    for (cat = eVehicle_self; cat <= C2V(gNet_mode); cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (car_index = 0; car_index < car_count; car_index++) {
            if (cat == eVehicle_self) {
                car = &C2V(gProgram_state).current_car;
            } else {
                car = GetCarSpec(cat, car_index);
            }
            BrVector3Copy(&orig, (br_vector3*)car->old_frame_mat.m[3]);
            BrVector3Sub(&dir, &car->car_master_actor->t.t.translate.t, &orig);
            for (i = 0; i < C2V(gCurrent_race).check_point_count; i++) {
                for (j = 0; j < C2V(gCurrent_race).checkpoints[i].count_quads; j++) {
                    if (
                            RayHitFace(&C2V(gCurrent_race).checkpoints[i].quads[j].points[0],
                                       &C2V(gCurrent_race).checkpoints[i].quads[j].points[1],
                                       &C2V(gCurrent_race).checkpoints[i].quads[j].points[2],
                                       &C2V(gCurrent_race).checkpoints[i].normals[j],
                                       &orig, &dir)
                            || RayHitFace(&C2V(gCurrent_race).checkpoints[i].quads[j].points[0],
                                          &C2V(gCurrent_race).checkpoints[i].quads[j].points[2],
                                          &C2V(gCurrent_race).checkpoints[i].quads[j].points[3],
                                          &C2V(gCurrent_race).checkpoints[i].normals[j],
                                          &orig,
                                          &dir)) {
                        if (C2V(gNet_mode) == eNet_mode_none) {
                            if (i + 1 == C2V(gCheckpoint)) {
                                IncrementCheckpoint();
                            } else {
                                WrongCheckpoint(i);
                            }
                        } else {
                            net_player = NetPlayerFromCar(car);
                            if (C2V(gCurrent_net_game)->type == eNet_game_type_3) {
                                if (net_player->score2 & (1 << i)) {
                                    net_player->score2 &= ~(1 << i);
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            } else if (C2V(gCurrent_net_game)->type == eNet_game_type_5) {
                                if ((net_player->score2 % C2V(gCurrent_race).check_point_count) == i) {
                                    net_player->score2 += 1;
                                    SendGameplay(net_player->ID, eNet_gameplay_checkpoint, i, 0, 0, 0);
                                    C2V(gLast_checkpoint_time) = GetTotalTime();
                                    if (net_player->score2 >= C2V(gCurrent_net_game)->options.starting_target * C2V(gCurrent_race).check_point_count) {
                                        DeclareWinner(net_player - C2V(gNet_players));
                                    }
                                } else {
                                    SendGameplay(net_player->ID, eNet_gameplay_wrong_checkpoint, i, 0, 0, 0);
                                }
                            } else {
                                if ((net_player->score2 % C2V(gCurrent_race).check_point_count) == i) {
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
C2_HOOK_FUNCTION(0x005034b0, CheckCheckpoints)

void C2_HOOK_FASTCALL TotalRepair(void) {

    TotallyRepairCar();
    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_instant_repair));
}
C2_HOOK_FUNCTION(0x005039b0, TotalRepair)

int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence) {
    int i;
    int new_index;
    int races_count;
    int most_seldom_seen;
    int races_to_pick_from[50];

    if (pNet_race_sequence == eNet_sequence_sequential) {
        new_index = pCurrent_race + 1;
        if (new_index >= C2V(gNumber_of_races)) {
            new_index = 0;
        }
    } else {
        most_seldom_seen = 10000;
        for (i = 0; i < C2V(gNumber_of_races); i++) {
            if (C2V(gRace_list)[i].been_there_done_that < most_seldom_seen) {
                most_seldom_seen = C2V(gRace_list)[i].been_there_done_that;
            }
        }
        races_count = 0;
        for (i = 0; i < C2V(gNumber_of_races); i++) {
            if (C2V(gRace_list)[i].been_there_done_that == most_seldom_seen && (i != pCurrent_race)) {
                races_to_pick_from[races_count] = i;
                races_count++;
            }
        }
        new_index = races_to_pick_from[IRandomBetween(0, races_count - 1)];
        C2V(gRace_list)[new_index].been_there_done_that += 1;
    }
    return new_index;
}
C2_HOOK_FUNCTION(0x00503ba0, PickNetRace)

void C2_HOOK_FASTCALL JumpTheStart(void) {
    char s[256];

    if (C2V(gNet_mode) == eNet_mode_none || C2V(gProgram_state).credits >= C2V(gJump_start_fine)[C2V(gProgram_state).skill_level]) {
        WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(C2V(gCountdown));
        C2V(gCountdown) = 0;
        DRS3StopOutletSound(C2V(gPedestrians_outlet));
        DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_HeyYou);
        SpendCredits(C2V(gJump_start_fine)[C2V(gProgram_state).skill_level]);
        c2_sprintf(s, "%s %d %s",
            GetMiscString(eMiscString_bad_boy),
            C2V(gJump_start_fine)[C2V(gProgram_state).skill_level],
            GetMiscString(eMiscString_credit_fine));
        NewTextHeadupSlot(4, 0, 1000, -4, s);
    }
}
C2_HOOK_FUNCTION(0x00504170, JumpTheStart)

void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void) {

    C2V(gInterface_within_race_mode) = 1;
    C2V(gHud_tinted1_visible) = !!IsTintedVisible(C2V(gHud_tinted1));
    C2V(gHud_tinted2_visible) = !!IsTintedVisible(C2V(gHud_tinted2));
    TurnTintedPolyOff(C2V(gHud_tinted1));
    TurnTintedPolyOff(C2V(gHud_tinted2));
    PlayFlicsFromDisk();
    if (C2V(gNet_mode) == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
}
C2_HOOK_FUNCTION(0x00504230, GoingToInterfaceFromRace)

void (C2_HOOK_FASTCALL * SendGameplayToHost_original)(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);
void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {

#if defined(C2_HOOKS_ENABLED)
    SendGameplayToHost_original(pMess, pParam_1, pParam_2, pParam_3, pParam_4);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049bcb0, SendGameplayToHost, SendGameplayToHost_original)
