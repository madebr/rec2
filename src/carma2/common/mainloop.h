#ifndef REC2_MAINLOOP_H
#define REC2_MAINLOOP_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tU32 gLast_tick_count;
extern tU32 gOld_camera_time;
extern tU32 gActual_last_tick_count;
extern tU32 gAverage_frame_period;
extern tU32 gLast_net_auto_increase_credits;
extern tU32 gLast_credit_headup__mainloop;
extern int gLast_time_headup;
extern int gTime_bonus_headup;
extern int gRace_bonus_headup;
extern tU32 gTime_bonus_start;
extern tTime_bonus_state gTime_bonus_state;
extern int gWasted_flash_state;
extern tU32 gWasted_last_flash;
extern float gFLOAT_0068c778;
extern int gCredits_post_race;
extern br_vector3 gPrev_camera_position;
extern tU32 gCamera_period;
extern tU32 gPrev_task_switched_message;
extern int gWindow_inactive;
extern int gTotal_bonus_post_race;
extern int gQueued_wasted_messages_count;
extern tU32 gLast_wasted_message_start;
extern int gQueued_wasted_messages[5];
extern char gHeadup_oppo_ped_text[64];
extern int gCompletion_bonus_post_race;
extern int gTime_bonus;


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