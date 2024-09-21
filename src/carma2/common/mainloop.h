#ifndef REC2_MAINLOOP_H
#define REC2_MAINLOOP_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tU32, gLast_tick_count);
C2_HOOK_VARIABLE_DECLARE(tU32, gOld_camera_time);
C2_HOOK_VARIABLE_DECLARE(tU32, gActual_last_tick_count);
C2_HOOK_VARIABLE_DECLARE(tU32, gAverage_frame_period);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_net_auto_increase_credits);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_credit_headup__mainloop);
C2_HOOK_VARIABLE_DECLARE(int, gLast_time_headup);
C2_HOOK_VARIABLE_DECLARE(int, gTime_bonus_headup);
C2_HOOK_VARIABLE_DECLARE(int, gRace_bonus_headup);
C2_HOOK_VARIABLE_DECLARE(tU32, gTime_bonus_start);
C2_HOOK_VARIABLE_DECLARE(tTime_bonus_state, gTime_bonus_state);
C2_HOOK_VARIABLE_DECLARE(int, gWasted_flash_state);
C2_HOOK_VARIABLE_DECLARE(tU32, gWasted_last_flash);
C2_HOOK_VARIABLE_DECLARE(float, gFLOAT_0068c778);
C2_HOOK_VARIABLE_DECLARE(int, gCredits_post_race);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gPrev_camera_position);
C2_HOOK_VARIABLE_DECLARE(tU32, gCamera_period);
C2_HOOK_VARIABLE_DECLARE(tU32, gPrev_task_switched_message);
C2_HOOK_VARIABLE_DECLARE(int, gWindow_inactive);
C2_HOOK_VARIABLE_DECLARE(int, gTotal_bonus_post_race);
C2_HOOK_VARIABLE_DECLARE(int, gNum_active_cars);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tCar_spec*, gActive_car_list, 25);
C2_HOOK_VARIABLE_DECLARE(int, gQueued_wasted_messages_count);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_wasted_message_start);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gQueued_wasted_messages, 5);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gHeadup_oppo_ped_text, 64);
C2_HOOK_VARIABLE_DECLARE(int, gCompletion_bonus_post_race);
C2_HOOK_VARIABLE_DECLARE(int, gTime_bonus);


void C2_HOOK_FASTCALL MungeAIWorld(tU32 pTime);

void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period);

void C2_HOOK_FASTCALL CheckActiveCarList(void);

void C2_HOOK_FASTCALL CalculateFrameRate(void);

tRace_result C2_HOOK_FASTCALL MainGameLoop(void);

tRace_result C2_HOOK_FASTCALL DoRace(void);

void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period);

void C2_HOOK_FASTCALL MungeHeadups(void);

void C2_HOOK_FASTCALL CheckTimer(void);

int C2_HOOK_FASTCALL MungeRaceFinished(void);

void C2_HOOK_FASTCALL LoseOldestWastedMessage(void);

#endif // REC2_MAINLOOP_H
