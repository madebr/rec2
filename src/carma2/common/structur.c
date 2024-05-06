#include "structur.h"

#include "car.h"
#include "controls.h"
#include "crush.h"
#include "cutscene.h"
#include "displays.h"
#include "drmem.h"
#include "errors.h"
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
#include "network.h"
#include "opponent.h"
#include "piping.h"
#include "pratcam.h"
#include "racesumm.h"
#include "racestrt.h"
#include "sound.h"
#include "temp.h"
#include "utility.h"
#include "world.h"

#include "c2_stdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gMirror_on__structur, 0x00660ce0, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gLoad_last_save_game, 0x00660ce8, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMoney_pre_race, 0x007051f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gAPO_pre_race, 3, 0x007051e0);
C2_HOOK_VARIABLE_IMPLEMENT(tRace_over_reason, gRace_over_reason, 0x006b781c);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_checkpoint_time, 0x006aaa34);

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
                RestoreStashedSaveGame();
            }
            if (C2V(gAuto_load) && C2V(gLoad_last_save_game)) {
                LoadLastSaveGame();
                C2V(gLoad_last_save_game) = 0;
            }
            DoMainMenu();
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
        SortOpponents();
        SetInitialPositions(&C2V(gCurrent_race));
    } else {
        DoFrontendMenu(kFrontend_menu_netsync);
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
    DisableMaterialAdapt();
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
