#include "mainloop.h"

#include "car.h"
#include "controls.h"
#include "crush.h"
#include "depth.h"
#include "displays.h"
#include "drmem.h"
#include "drone.h"
#include "explosions.h"
#include "flicplay.h"
#include "globvars.h"
#include "globvrkm.h"
#include "graphics.h"
#include "globvrpb.h"
#include "input.h"
#include "joystick.h"
#include "loading.h"
#include "main.h"
#include "mainmenu.h"
#include "netgame.h"
#include "network.h"
#include "oil.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "powerups.h"
#include "racemem.h"
#include "replay.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "structur.h"
#include "timers.h"
#include "utility.h"
#include "world.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_tick_count, 0x0079efb0);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gOld_camera_time, 0x0068c814);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gActual_last_tick_count, 0x0068c820);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gAverage_frame_period, 0x0068c824);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_net_auto_increase_credits, 0x0068c770);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_credit_headup__mainloop, 0x0068c764);
C2_HOOK_VARIABLE_IMPLEMENT(int, gLast_time_headup, 0x0068c76c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTime_bonus_headup, 0x0068c808);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRace_bonus_headup, 0x0068c810);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gTime_bonus_start, 0x0068c788);
C2_HOOK_VARIABLE_IMPLEMENT(tTime_bonus_state, gTime_bonus_state, 0x0068c784);
C2_HOOK_VARIABLE_IMPLEMENT(int, gWasted_flash_state, 0x0068c768);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gWasted_last_flash, 0x0068c730);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFLOAT_0068c778, 0x0068c778);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCredits_post_race, 0x0068c80c);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gPrev_camera_position, 0x0068c738);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gCamera_period, 0x0074abf0);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gPrev_task_switched_message, 0x0068c834);
C2_HOOK_VARIABLE_IMPLEMENT(int, gWindow_inactive, 0x0068c838);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTotal_bonus_post_race, 0x0068c774);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_active_cars, 0x0074c9ec);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tCar_spec*, gActive_car_list, 25, 0x0074c7c0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gQueued_wasted_messages_count, 0x0068c760);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_wasted_message_start, 0x0068c77c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gQueued_wasted_messages, 5, 0x0068c748);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gHeadup_oppo_ped_text, 64, 0x0079eb00);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCompletion_bonus_post_race, 0x0068c75c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTime_bonus, 0x0068c780);


void C2_HOOK_FASTCALL MungeAIWorld(tU32 pFrame_period) {

    C2V(gTime_stamp_for_this_munging) = GetTotalTime();
    if (C2V(gTime_stamp_for_this_munging) == 0) {
        C2V(gTime_stamp_for_this_munging) = PDGetTotalTime();
    }
    C2V(gDrone_delta_time) = (float)pFrame_period / 1000.f;
    C2V(gFrame_period_for_this_munging) = pFrame_period;
    Timers_Push(TIMER_DRN);
    ProcessDrones();
    Timers_Pop(TIMER_DRN);
    Timers_Push(TIMER_OPP);
    MungeOpponents();
    Timers_Pop(TIMER_OPP);
    C2V(gFirst_drone_processing) = 0;
    ProcessShitMines(C2V(gTime_stamp_for_this_munging));
}
C2_HOOK_FUNCTION(0x00401170, MungeAIWorld)

void (C2_HOOK_FASTCALL * CalculateCameraStuff_original)(tU32 pCamera_period);
void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period) {

#if defined(C2_HOOKS_ENABLED)
    CalculateCameraStuff_original(pCamera_period);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004940e0, CalculateCameraStuff, CalculateCameraStuff_original)

void C2_HOOK_FASTCALL CheckActiveCarList(void) {
    int i;
    int new_count = 0;

    for (i = 0; i < C2V(gNum_active_cars); i++) {
        tCar_spec *c;

        c = C2V(gActive_car_list)[i];
        if (c != NULL && c->driver > 5) {
            if (i != new_count) {
                C2V(gActive_car_list)[new_count] = c;
            }
            new_count += 1;
        }
    }
    C2V(gNum_active_cars) = new_count;
}

void C2_HOOK_FASTCALL CalculateFrameRate(void) {
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_time, 0x0068c734);
    tU32 new_time;
    static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, last_rates, 30, 0x0068c790);
    int i;
    int new_rate;

    new_time = PDGetTotalTime();
    if (new_time != C2V(last_time)) {
        new_rate = 10000 / (new_time - C2V(last_time));
        C2V(last_rates)[REC2_ASIZE(C2V(last_rates)) - 1] = new_rate;
        C2V(gFrame_rate) = new_rate;
        for (i = 0; i < REC2_ASIZE(C2V(last_rates)); i++) {
            C2V(gFrame_rate) += C2V(last_rates)[i];
        }
        C2V(gFrame_rate) /= REC2_ASIZE(C2V(last_rates)) + 1;
        for (i = 0; i < REC2_ASIZE(C2V(last_rates)) - 1; i++) {
            C2V(last_rates)[i] = C2V(last_rates)[i + 1];
        }
        C2V(last_time) = new_time;
    }
}

tRace_result (C2_HOOK_FASTCALL * MainGameLoop_original)(void);
tRace_result C2_HOOK_FASTCALL MainGameLoop(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return MainGameLoop_original();
#else
    int tried_to_allocate_AR;
    tRace_result result;

    tried_to_allocate_AR = 0;
    C2V(gCar_to_view) = &C2V(gProgram_state).current_car;
    C2V(gCar_to_view_id) = 0;
    C2V(gOld_camera_time) = 0;
    C2V(gLast_net_auto_increase_credits) = 0;
    C2V(gLast_credit_headup__mainloop) = 0;
    C2V(gLast_time_headup) = C2V(gTimer);
    C2V(gRace_bonus_headup) = -1;
    C2V(gTime_bonus_headup) = -1;
    C2V(gTime_bonus_start) = 0;
    C2V(gTime_bonus_state) = 0;
    ARMainLoopStart();
    C2V(gActual_last_tick_count) = PDGetTotalTime();
    C2V(gFrame_period) = 100;
    C2V(gQueued_wasted_messages_count) = 0;
    C2V(gWasted_flash_state) = 0;
    C2V(gWasted_last_flash) = 0;
    C2V(gLast_tick_count) = C2V(gActual_last_tick_count);
    RevertPalette();
    C2V(gHost_abandon_game) = 0;
    C2V(gNo_races_yet) = 0;
    C2V(gRace_over_reason) = eRace_not_over_yet;
    C2V(gFLOAT_0068c778) = 0.f;
    C2V(gNo_credits_APO_restore) = 0;
    C2V(gReceived_game_scores) = 0;
    if (C2V(gNet_mode) == eNet_mode_none) {
        C2V(gProgram_state).field_0x2c = 1;
    }
    C2V(gCredits_post_race) = -1;
    ShowSpecialVolumesIfRequ();
    C2V(gProgram_state).racing = 1;
    if (C2V(gNet_mode) == eNet_mode_host) {
        C2V(gCurrent_net_game)->status.stage = eNet_game_playing;
    }
    NetPlayerStatusChanged(ePlayer_status_racing);
    GetAverageGridPosition(&C2V(gCurrent_race));
    ForceRebuildActiveCarList();
    PrintMemoryDump(0, "ABOUT TO ENTER MAINLOOP");
    UpdateFramePeriod(&C2V(gCamera_period));
    BrVector3Set(&C2V(gPrev_camera_position), 1000.f, 1000.f, 1000.f);
    CalculateCameraStuff(C2V(gCamera_period));

    /* Only present in win32 version */
    PDPageInProcessMemory();

    C2V(gRace_start) = PDGetTotalTime();
    Timers_Init();

    if (C2V(gWait_for_it) != 0) {
        C2V(gWait_for_it) += GetTotalTime();
    }
    ClearEntireScreen();
    ResetPalette();
    for (;;) {
        int window_inactive;
        tU32 frame_start_time;

        Timers_StartFrame();
        EdgeTriggerModeOff();
        if (!C2V(gAction_replay_mode)) {
            UpdateFramePeriod(&C2V(gCamera_period));
        }
        frame_start_time = GetTotalTime();
        CyclePollKeys();
        InitSmashQueue();
        CheckSystemKeys(1);
        InitialiseExtraRenders();
        NetReceiveAndProcessMessages();

        if (C2V(gHost_abandon_game) || C2V(gProgram_state).prog_status == eProg_idling) {
            break;
        }
        if (C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_net_game)->type == eNet_game_type_foxy && C2V(gThis_net_player_index) == C2V(gIt_or_fox) && C2V(gMap_view) == 2) {
            ToggleMap();
        }
        ResetGrooveFlags();
        ServiceGameInRace();
        EnterUserMessage();
        ARService();
        ResetLollipopQueue();
        MungePalette();

        window_inactive = PDIsWindowInactive();
        if (window_inactive) {

            if ((tU32)PDGetTotalTime() > C2V(gPrev_task_switched_message) + 1000) {
                C2V(gPrev_task_switched_message) = GetTotalTime();
                NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_task_switched));
            }
        } else {
            if (C2V(gWindow_inactive)) {
                ClearHeadupSlot(4);
                ClearQueuedHeadups();
            }
        }
        C2V(gWindow_inactive) = window_inactive;

        if (!C2V(gAction_replay_mode)) {
            MungeAIWorld(C2V(gFrame_period));
            PollCarControls(C2V(gFrame_period));
        }

        if (!C2V(gAction_replay_mode)) {
            CheckActiveCarList();

            Timers_Push(TIMER_PED);
            MungePedestrians();
            Timers_Pop(TIMER_PED);

            ControlOurCar(C2V(gFrame_period));

            Timers_Push(TIMER_PHS);
            ApplyPhysicsToCar(C2V(gLast_tick_count) - C2V(gRace_start), C2V(gFrame_period));
            Timers_Pop(TIMER_PHS);

            MungeSomeOtherCarGraphics();
            PipeCarPositions();

            Timers_Push(TIMER_CRS);
            CrushBendFlapRend();
            Timers_Pop(TIMER_CRS);

            DoNetGameManagement();
            CheckRecoveryOfCars(C2V(gFrame_period) + C2V(gLast_tick_count) - C2V(gRace_start));
            NetSendMessageStacks();
        } else {
            DoActionReplay(C2V(gFrame_period));
        }
        PollCameraControls(C2V(gCamera_period));
        MungeEngineNoise();
        CalculateCameraStuff(C2V(gCamera_period));
        MungeNapalm();
        if (!C2V(gAction_replay_mode)) {
            DoPowerupPeriodics(C2V(gFrame_period));
        }
        if (!C2V(gAction_replay_mode)) {
            CheckCheckpoints();
            MungeEnvironmentalSound();
        }
        ChangingView();
        MungeCarGraphics(C2V(gFrame_period));
        FunkThoseTronics();
        GrooveThoseDelics();
        DoWheelDamage(C2V(gFrame_period));
        CalculateFrameRate();
        CameraBugFix(&C2V(gProgram_state).current_car, C2V(gCamera_period));
        if (!C2V(gAction_replay_mode)) {
            MungeHeadups();
            ProcessOilSpills(C2V(gFrame_period));
            MungeSmashEdgeTriggers(GetTotalTime());
        }
        MungeDelayedSideEffects();
        MungeShrapnel(C2V(gFrame_period));
        if (!C2V(gAction_replay_mode)) {
            MungePowerupStuff(C2V(gFrame_period));
        }
        ChangeDepthEffect();
        ServiceGameInRace();
        EnterUserMessage();
        SkidsPerFrame();
        FlushSmashQueue(0);
        MungeGlassFragments();
        AnimateSky();
        MungeExplosions();
        LastChanceForPedEffects();
        DoLODCarModels();
        MungeLightingEffects();

        if (C2V(gWait_for_it) != 0) {
            if (GetTotalTime() >= C2V(gWait_for_it)) {
                C2V(gWait_for_it) = 0;
            }
        }
        if (C2V(gWait_for_it) == 0) {
            Timers_Force(TIMER_RQQ);
            RenderAFrame(1);
            Timers_Force(TIMER_OQQ);
        }
        DoComplexCarModels();
        CheckReplayTurnOn();
        if (!C2V(gRecover_car)
                && C2V(gProgram_state).prog_status == eProg_game_ongoing
                && C2V(gPalette_fade_time) == 0
                && (C2V(gNet_mode) == eNet_mode_none
                        || !C2V(gAction_replay_mode)
                        || C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t.v[0] < 500.f)) {

            EnsureRenderPalette();
            EnsurePaletteUp();
        }

        if (KeyIsDown(0) && !C2V(gEntering_message)) {

            WaitForNoKeys();
            if (!C2V(gAction_replay_mode)) {
                tU32 start_menu_time;

                if (C2V(gMap_view) == 2) {
                    ToggleMap();
                }
                start_menu_time = PDGetTotalTime();
                FadePaletteDown();
                ClearEntireScreen();
                GoingToInterfaceFromRace();
                DoOptionsMenu();
                GoingBackToRaceFromInterface();
                AddLostTime(PDGetTotalTime() - start_menu_time);
            } else {
                ToggleReplay(NULL, NULL);
            }
        }
        if (!C2V(gAction_replay_mode)) {
            CheckTimer();
        } else {
            PollActionReplayControls(&C2V(gFrame_period), &C2V(gAverage_frame_period));
        }
        if (!C2V(gAction_replay_mode) && C2V(gKnobbled_frame_period) != 0) {
            while (GetTotalTime() - frame_start_time < C2V(gKnobbled_frame_period)) {
                /* brr */
                /* FIXME: replace with udelay */
            }
        }
        if (!tried_to_allocate_AR) {
            tried_to_allocate_AR = 1;
            PrintMemoryDump(0, "JUST RENDERED 1ST STUFF");
            InitialiseActionReplay();
            PrintMemoryDump(0, "JUST ALLOCATED ACTION REPLAY BUFFER");
        }
        if (C2V(gNet_mode) == eNet_mode_client && C2V(gAbandon_game)) {
            C2V(gProgram_state).prog_status = eProg_idling;
            C2V(gAbandon_game) = 0;
        }
        Timers_EndFrame();

        if (C2V(gProgram_state).prog_status != eProg_game_ongoing) {
            break;
        }
        if (MungeRaceFinished()) {
            break;
        }
        if (C2V(gAbandon_game) || C2V(gHost_abandon_game)) {
            break;
        }
    }
    if (C2V(gCredits_post_race) >= 0) {
        C2V(gProgram_state).credits = C2V(gCredits_post_race) + C2V(gTotal_bonus_post_race);
    }
    PHILDisable();
    PrintMemoryDump(0, "JUST EXITED MAINLOOP");
    FadePaletteDown();
    ClearEntireScreen();
    SuspendPendingFlic();
    RevertPalette();
    if (C2V(gMap_view) == 2) {
        ToggleMap();
    }
    EnsureSpecialVolumesHidden();
    FadePaletteDown();
    NetPlayerStatusChanged(ePlayer_status_loading);
    if (C2V(gAdditional_actors) != NULL && C2V(gAdditional_actors)->children != NULL) {
        AutoSaveAdditionalStuff();
    }
    if (C2V(gProgram_state).prog_status == eProg_game_ongoing) {
        ResetCarScreens();
    }
    if (C2V(gAbandon_game) && C2V(gNet_mode) == eNet_mode_host && C2V(gRace_over_reason) < 8 ) { /* FIXME: 8 == eRace_over_count */
        NetFinishRace(C2V(gCurrent_net_game), eRace_over_abandoned);
    }
    if (C2V(gAction_replay_mode)) {
        ToggleReplay(NULL, NULL);
    }
    if (C2V(gHost_abandon_game) || C2V(gProgram_state).prog_status != eProg_game_ongoing) {
        result = eRace_game_abandonned;
    } else if (C2V(gAbandon_game) == 1) {
        result = eRace_aborted;
    } else if (C2V(gRace_over_reason) == eRace_over_out_of_time || C2V(gRace_over_reason) == eRace_over_6 || C2V(gRace_over_reason) == eRace_over_7) {
        result = eRace_timed_out;
    } else {
        result = eRace_completed;
    }
    if (result < eRace_completed) {
        C2V(gProgram_state).redo_race_index = C2V(gProgram_state).current_race_index;
    } else {
        C2V(gProgram_state).redo_race_index = -1;
    }
    if (C2V(gNet_mode) != eNet_mode_none
            && result == eRace_game_abandonned
            && !C2V(gAbandon_game)
            && C2V(gProgram_state).prog_status != eProg_idling) {

        C2V(gProgram_state).prog_status = eProg_game_starting;
    }
    C2V(gAbandon_game) = 0;
    C2V(gStart_race_sent) = 0;
    C2V(gInitialised_grid) = 0;
    C2V(gHost_abandon_game) = 0;
    DRS3StopAllOutletSoundsExceptCDA();
    if (C2V(gNet_mode) != eNet_mode_none) {
        int i;

        for (i = 0; i < C2V(gNumber_of_net_players); i++) {
            StopCarBeingIt(C2V(gNet_players)[i].car);
        }
    }
    C2V(gProgram_state).racing = 0;
    if (C2V(gNet_mode) == eNet_mode_host) {
        C2V(gCurrent_net_game)->status.stage = eNet_game_stage0;
    }
    WaitForNoKeys();
    return result;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00492980, MainGameLoop, MainGameLoop_original)

tRace_result C2_HOOK_FASTCALL DoRace(void) {
    tRace_result result;

    StartMusicTrack(PercentageChance(50) ? 9998 : 9997);
    C2V(gRace_start) = GetTotalTime();
    result = MainGameLoop();
    return result;
}
C2_HOOK_FUNCTION(0x00492950, DoRace)

void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period) {
    tU32 new_tick_count;
    tU32 new_camera_tick_count;
    int error;

    if (C2V(gAction_replay_mode)) {
        tU32 last_tick_count;

        last_tick_count = C2V(gLast_tick_count);
        C2V(gLast_tick_count) = C2V(gLast_replay_frame_time);
        C2V(gFrame_period) = abs((int)(C2V(gLast_replay_frame_time) - last_tick_count));
        new_camera_tick_count = PDGetTotalTime();
        new_tick_count = GetTotalTime();
        if (C2V(gOld_camera_time) != 0) {
            *pCamera_period = new_camera_tick_count - C2V(gOld_camera_time);
        } else {
            *pCamera_period = 0;
        }
        C2V(gOld_camera_time) = new_camera_tick_count;
        if (C2V(gFrame_period) != 0) {
            *pCamera_period = C2V(gFrame_period);
        }
    } else {
        int new_period;
        new_tick_count = PDGetTotalTime();
        C2V(gLast_tick_count) += C2V(gFrame_period);
        error = new_tick_count - C2V(gLast_tick_count);
        C2V(gFrame_period) = new_tick_count - C2V(gActual_last_tick_count);
        if (C2V(gFrame_period) > 500 && new_tick_count - C2V(gRace_start) > 2000) {
            C2V(gFrame_period) = C2V(gAverage_frame_period) / 10;
            C2V(gLast_tick_count) = new_tick_count;
            if (C2V(gNet_mode) != eNet_mode_none && C2V(gNet_mode) != eNet_mode_host) {
                if (C2V(gNet_mode) == eNet_mode_client) {
                    C2V(gProgram_state).current_car.collision_info->field_0x49c = 0;
                }
            }
        }
        C2V(gAverage_frame_period) = 9 * C2V(gAverage_frame_period) / 10 + C2V(gFrame_period);
        if (new_tick_count - C2V(gRace_start) > 2000) {
            C2V(gFrame_period) = C2V(gAverage_frame_period) / 10;
        }
        new_period = C2V(gFrame_period) + error;
        if (new_period > 0) {
            C2V(gFrame_period) = new_period;
        } else {
            C2V(gLast_tick_count) = new_tick_count;
        }
        *pCamera_period = C2V(gFrame_period);
        C2V(gActual_last_tick_count) = new_tick_count;
    }
    if (C2V(gFrame_period) < 10) {
        C2V(gLast_tick_count) = 10;
        C2V(gLast_tick_count) = new_tick_count;
    } else if (C2V(gFrame_period) > 1000) {
        C2V(gFrame_period) = 1000;
        C2V(gLast_tick_count) = new_tick_count;
    }
    if (*pCamera_period < 10) {
        *pCamera_period = 10;
    } else if (*pCamera_period > 1000) {
        *pCamera_period = 1000;
    }
}
C2_HOOK_FUNCTION(0x00492680, UpdateFramePeriod)

void C2_HOOK_FASTCALL MungeHeadups(void) {
    char the_text[256];
    int net_credits;
    int effective_timer;
    int bonus;
    tU32 the_time;
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_rattle_time, 0x0068c818);

    ClearHeadupSlot(3);
    MungeJoystickHeadups();
    net_credits = C2V(gProgram_state).credits;
    if (fabsf((float)C2V(gProgram_state).credits - (float)C2V(gLast_credit_headup__mainloop)) / (float)C2V(gFrame_rate) > 1.2f) {
        if (C2V(gProgram_state).credits - C2V(gLast_credit_headup__mainloop) > 0) {
            net_credits = (int)((float)C2V(gLast_credit_headup__mainloop) + ((float)C2V(gProgram_state).credits - (float)C2V(gLast_credit_headup__mainloop) + 1000.f) * (float)C2V(gFrame_period) * 1.2f / 1000.f);
        } else {
            net_credits = (int)((float)C2V(gLast_credit_headup__mainloop) - ((float)C2V(gLast_credit_headup__mainloop) - (float)C2V(gProgram_state).credits + 1000.f) * (float)C2V(gFrame_period) * 1.2f / 1000.f);
        }
    }

    if (C2V(gNet_mode) != eNet_mode_none
            && C2V(gCountdown) == 0
            && C2V(gNet_auto_increase_credits_dt)[C2V(gCurrent_net_game)->type] != 0
            && net_credits < C2V(gRecovery_cost).initial_network[C2V(gCurrent_net_game)->type]
            && PDGetTotalTime() - C2V(gLast_net_auto_increase_credits) >= C2V(gNet_auto_increase_credits_dt)[C2V(gCurrent_net_game)->type]) {

        C2V(gLast_net_auto_increase_credits) = PDGetTotalTime();
        C2V(gProgram_state).credits += 50;

#define GET_RECOVERY_COST() ((C2V(gNet_mode) == eNet_mode_none) ? \
    C2V(gRecovery_cost).initial[C2V(gProgram_state).skill_level] : \
    C2V(gRecovery_cost).initial_network[C2V(gCurrent_net_game)->type])

        if (net_credits + 50 > GET_RECOVERY_COST()) {
            C2V(gProgram_state).credits += (int)(GET_RECOVERY_COST() - net_credits - 50.f);
            net_credits = (int)GET_RECOVERY_COST();
        } else {
            net_credits += 50;
        }
    }
    C2V(gLast_credit_headup__mainloop) = net_credits;

    if (C2V(gCountdown) != 0) {
        int new_countdown;

        new_countdown = (int)(7.5f - (float)GetRaceTime() / 1000.f);
        if (new_countdown < 0) {
            new_countdown = 0;
        }
        if (C2V(gCountdown) != new_countdown && new_countdown <= 5) {
            C2V(gCountdown) = new_countdown;
            NewImageHeadupSlot(5, 0, 800, 4 + new_countdown);
            DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_Countdown_Start + C2V(gCountdown));
            if (new_countdown == 0) {
                MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo();
            }
        }
    }
    if (fabsf((float)C2V(gTimer) - (float)C2V(gLast_time_headup)) / (float)C2V(gFrame_period) <= 10.0) {
        effective_timer = C2V(gTimer);
    } else if ((int)C2V(gTimer) < C2V(gLast_time_headup)) {
        effective_timer = C2V(gTimer);
    } else {
        effective_timer = (int)((float)C2V(gLast_time_headup) + 10.f * (float)C2V(gFrame_period));
    }
    C2V(gLast_time_headup) = effective_timer;

    if (C2V(gNet_mode) == eNet_mode_none) {
        if (net_credits < 0) {
            c2_sprintf(the_text, "\xf8%d\xfa %s", -net_credits, GetMiscString(eMiscString_credits_lost));
            ChangeHeadupText(C2V(gCredits_won_headup), the_text);
        } else {
            c2_sprintf(the_text, "\xf8%d\xfa %s", net_credits, GetMiscString(net_credits >= 100000 ? eMiscString_credits_abbrev : eMiscString_credits_gained));
            ChangeHeadupText(C2V(gCredits_won_headup), the_text);
        }
        if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1) {
            c2_sprintf(the_text, "\xf8%d\xfa/\xf8%d\xfa %s", C2V(gCount_killed_peds), C2V(gPed_count), GetMiscString(eMiscString_kills));
            ChangeHeadupText(C2V(gPed_kill_count_headup), the_text);
        } else if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Peds) {
            c2_sprintf(the_text, "\xf8%d\xfa/\xf8%d\xfa %s", C2V(gCount_killed_peds), C2V(gTotal_count_smash_peds), GetMiscString(eMiscString_kills));
            ChangeHeadupText(C2V(gPed_kill_count_headup), the_text);
        }
        if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1
                || C2V(gCurrent_race).race_spec->race_type == kRaceType_Cars) {

            if (C2V(gQueued_wasted_messages_count) != 0 && GetTotalTime() > C2V(gLast_wasted_message_start) + 5000) {
                LoseOldestWastedMessage();
            }

            if (C2V(gQueued_wasted_messages_count) == 0) {
                int count_oppos;

                if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1) {
                    count_oppos = GetCarCount(eVehicle_opponent);
                } else {
                    count_oppos = C2V(gCount_opponents);
                }
                c2_sprintf(the_text, "%s \xf8%d\xfa/\xf8%d",
                    GetMiscString(eMiscString_wasted),
                    count_oppos - NumberOfOpponentsLeft(),
                    count_oppos);
            } else {
                if (Flash(150, &C2V(gWasted_last_flash), &C2V(gWasted_flash_state))) {
                    c2_sprintf(the_text, "\xf9%s %s",
                        C2V(gOpponents)[C2V(gQueued_wasted_messages)[0]].abbrev_name,
                        GetMiscString(eMiscString_wasted));
                } else {
                    c2_sprintf(the_text, " ");
                }
            }
            ChangeHeadupText(C2V(gCar_kill_count_headup), the_text);
        }

        TimerString(effective_timer, the_text, 1, 0, 0);
        ChangeHeadupText(C2V(gTimer_headup), the_text);

        if (C2V(gHeadup_detail_level) == 1 || C2V(gHeadup_detail_level) == 4) {
            c2_sprintf(C2V(gHeadup_oppo_ped_text), "%i  %i  %i",
                GetCarCount(eVehicle_opponent) - NumberOfOpponentsLeft(),
                net_credits,
                C2V(gCount_killed_peds));
        }
        if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1
                || C2V(gCurrent_race).race_spec->race_type == kRaceType_Checkpoints) {

            c2_sprintf(the_text, "%s \xf8%d\xfa/\xf8%d \xfa%s \xf8%d\xfa/\xf8%d",
                GetMiscString(eMiscString_cp), C2V(gCheckpoint), C2V(gCheckpoint_count),
                GetMiscString(eMiscString_lap), C2V(gLap),C2V(gTotal_laps));
            ChangeHeadupText(C2V(gLaps_headup), the_text);
        }
        if (C2V(gCurrent_race).race_spec->race_type > 3) { /* kRaceType_Smash or kRaceType_SmashNPed */
            c2_sprintf(the_text, "%s \xf8%d\xfa/\xf8%d",
                GetMiscString(eMiscString_targets),
                GetRuntimeVariable(C2V(gCurrent_race).race_spec->options.smash.var_smash_number),
                C2V(gCurrent_race).race_spec->options.smash.var_smash_target);
            ChangeHeadupText(C2V(gLaps_headup), the_text);
        }

        the_time = GetTotalTime() - C2V(gTime_bonus_state);

        switch (C2V(gTime_bonus_state)) {
        case eTime_bonus_initial_pause:
            if (the_time >= 500) {
                bonus = C2V(gCompletion_bonus_post_race);
                c2_sprintf(the_text, "%s %d", GetMiscString(eMiscString_completion_bonus_colon), bonus);
                DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_Clap);
                ChangeHeadupText(C2V(gRace_bonus_headup), the_text);
                C2V(gProgram_state).credits += bonus;
                C2V(gTime_bonus_state) = eTime_bonus_race_bonus;
                C2V(gTime_bonus_start) = GetTotalTime();
            }
            C2V(gRace_finished) = 10000;
            break;
        case eTime_bonus_race_bonus:
            if (the_time >= 2000) {
                C2V(gTime_bonus_state) = eTime_bonus_tb_up;
                C2V(gTime_bonus_start) = GetTotalTime();
                C2V(last_rattle_time) = 0;
            }
            C2V(gRace_finished) = 10000;
            break;
        case eTime_bonus_tb_up:
            if (C2V(gTimer) != 0) {
                if (the_time - C2V(last_rattle_time) > 15) {
                    int previous_gtimer;

                    previous_gtimer = C2V(gTimer);
                    C2V(gTimer) -= 1000 * ((the_time - C2V(last_rattle_time)) / 15);
                    if (C2V(gTimer) < 0) {
                        C2V(gTimer) = 0;
                    }
                    C2V(gTime_bonus) += (previous_gtimer - C2V(gTimer)) / 1000 * C2V(gCredits_per_second_time_bonus)[C2V(gProgram_state).skill_level];
                    C2V(last_rattle_time) += 15 * ((the_time - C2V(last_rattle_time)) / 15);
                }
                c2_sprintf(the_text, "%s %d", GetMiscString(eMiscString_time_bonus_colon), C2V(gTime_bonus));
                ChangeHeadupText(C2V(gTime_bonus_headup), the_text);
            } else {
                C2V(gTime_bonus_state) = eTime_bonus_tb_pause;
                C2V(gTime_bonus_start) = GetTotalTime();
                C2V(last_rattle_time) = 0;
            }
            C2V(gRace_finished) = 10000;
            break;
        case eTime_bonus_tb_pause:
            if (the_time >= 1000) {
                C2V(gTime_bonus_state) = eTime_bonus_tb_down;
                C2V(gTime_bonus_start) = GetTotalTime();
                C2V(last_rattle_time) = 0;
            }
            C2V(gRace_finished) = 10000;
            break;
        case eTime_bonus_tb_down:
            if (C2V(gTime_bonus) != 0) {
                if (the_time - C2V(last_rattle_time) > 15) {
                    bonus = C2V(gTime_bonus);
                    C2V(gTime_bonus) -= (the_time - C2V(last_rattle_time)) * C2V(gCredits_per_second_time_bonus)[C2V(gProgram_state).skill_level] / 15;
                    if (C2V(gTime_bonus) < 0) {
                        C2V(gTime_bonus) = 0;
                    }
                    C2V(gProgram_state).credits += bonus - C2V(gTime_bonus);
                    C2V(last_rattle_time) += 15 * ((the_time - C2V(last_rattle_time)) / 15);
                }
                c2_sprintf(the_text, "%s %d", GetMiscString(eMiscString_time_bonus_colon), C2V(gTime_bonus));
                ChangeHeadupText(C2V(gTime_bonus_headup), the_text);
            } else {
                C2V(gTime_bonus_state) = eTime_bonus_end_pause;
                C2V(gTime_bonus_start) = GetTotalTime();
            }
            C2V(gRace_finished) = 10000;
            break;
        case eTime_bonus_end_pause:
            if (the_time >= 2000 && C2V(gRace_finished) > 1) {
                C2V(gRace_finished) = 1;
            }
            break;
        default:
            break;
        }
    } else {
        DoNetworkHeadups(net_credits);
    }
}

void C2_HOOK_FASTCALL CheckTimer(void) {
    tS32 time_in_seconds;
    tS32 time_left;
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_time_in_seconds, 0x0068c828);

    if (!C2V(gFreeze_timer) && !C2V(gCountdown) && !C2V(gRace_finished)) {
        if (C2V(gFrame_period) < C2V(gTimer)) {
            if (C2V(gNet_mode) == eNet_mode_none) {
                C2V(gTimer) -= C2V(gFrame_period);
            }
            time_left = C2V(gTimer) + 500;
            time_in_seconds = time_left / 1000;
            if (time_in_seconds != C2V(last_time_in_seconds) && time_in_seconds <= 10) {
                DRS3StartSound(C2V(gPedestrians_outlet), eSoundId_warning_buzzer_noise);
            }
            C2V(last_time_in_seconds) = time_in_seconds;
        } else {
            C2V(gTimer) = 0;
            RaceCompleted(eRace_over_out_of_time);
        }
    }
}

int C2_HOOK_FASTCALL MungeRaceFinished(void) {

    if (!C2V(gRace_finished) || C2V(gAction_replay_mode) || (C2V(gNet_mode) != eNet_mode_none && C2V(gRace_over_reason) == eRace_not_over_yet)) {
        return 0;
    }
    if (C2V(gNet_mode) == eNet_mode_none && C2V(gRace_over_reason) < 4 && C2V(gCredits_post_race) < 0) {
        int completion_bonus;

        C2V(gCredits_post_race) = C2V(gProgram_state).credits;
        if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Carma1) {
            completion_bonus = C2V(gCurrent_race).completion_bonus[C2V(gProgram_state).skill_level];
        } else {
            /* FIXME: is it int completion_bonus[4][3] instead?*/
            switch (C2V(gRace_over_reason)) {
            case eRace_over_0:
                completion_bonus = C2V(gCurrent_race).completion_bonus[C2V(gProgram_state).skill_level];
                break;
            case eRace_over_1:
                completion_bonus = C2V(gCurrent_race).completion_bonus_peds[C2V(gProgram_state).skill_level];
                break;
            case eRace_over_2:
                completion_bonus = C2V(gCurrent_race).completion_bonus_opponents[C2V(gProgram_state).skill_level];
                break;
            case eRace_over_3:
                /* FIXME: what completion_bonus type is this? */
                completion_bonus = C2V(gCurrent_race).completion_bonus_field_0x3c[C2V(gProgram_state).skill_level];
                break;
            default:
                REC2_UNREACHABLE();
                break;
            }
        }
        C2V(gCompletion_bonus_post_race) = completion_bonus;
        C2V(gTotal_bonus_post_race) = C2V(gCredits_per_second_time_bonus)[C2V(gProgram_state).skill_level] * (C2V(gTimer) / 1000)
                + C2V(gCompletion_bonus_post_race);
        if (!GetRuntimeVariable(99)) {
            NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_game_auto_saved));
        }
    }

    if (C2V(gRace_finished) > C2V(gFrame_period)) {
        C2V(gRace_finished) -= C2V(gFrame_period);
    } else {
        if (C2V(gTimer) == 0 || C2V(gNet_mode) != eNet_mode_none || C2V(gRace_over_reason) == eRace_over_6 || GetRuntimeVariable(99) != 0) {
            C2V(gRace_finished) = 0;
            return 1;
        }
        C2V(gRace_finished) = 15 * (C2V(gTimer) + 300);
        C2V(gRace_bonus_headup) = NewTextHeadupSlot(9, 0, 0, -4, "");
        C2V(gTime_bonus_headup) = NewTextHeadupSlot(10, 0, 0, -4, "");
        C2V(gTime_bonus) = 0;
        C2V(gTime_bonus_start) = GetTotalTime();
        C2V(gTime_bonus_state) = eTime_bonus_initial_pause;
    }
    return PDKeyDown(51) || PDKeyDown(52);
}

void C2_HOOK_FASTCALL LoseOldestWastedMessage(void) {
    int i;

    for (i = 1; i < C2V(gQueued_wasted_messages_count); i++) {
        C2V(gQueued_wasted_messages)[i - 1] = C2V(gQueued_wasted_messages)[i];
    }
    C2V(gQueued_wasted_messages_count) -= 1;
    C2V(gLast_wasted_message_start) = GetTotalTime();
}
