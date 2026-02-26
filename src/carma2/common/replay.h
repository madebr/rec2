#ifndef REC2_REPLAY_H
#define REC2_REPLAY_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tActionReplayCameraMode, gAction_replay_camera_mode);
C2_HOOK_VARIABLE_DECLARE(int, gAction_replay_manual_camera_target_type);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_synch_time);
C2_HOOK_VARIABLE_DECLARE(int, gSingle_frame_mode);
C2_HOOK_VARIABLE_DECLARE(int, gKey_down);

void C2_HOOK_FASTCALL SetQuickTimeDefaults(void);

void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);

void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos);

void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);

void C2_HOOK_FASTCALL ARMainLoopStart(void);

void C2_HOOK_FASTCALL ARService(void);

void C2_HOOK_FASTCALL TurnOnActionReplay(void);

void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2);

void C2_HOOK_FASTCALL InitialiseActionReplay(void);

void C2_HOOK_FASTCALL CheckReplayTurnOn(void);

void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period);

void C2_HOOK_FASTCALL PollActionReplayControls(tU32 *pFrame_period, tU32* pAverage_frame_period);

int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);

float C2_HOOK_FASTCALL ARGetReplayRate(void);

void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption);

void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard);

void C2_HOOK_FASTCALL DoActionReplayHeadups(void);

void C2_HOOK_FASTCALL ARSetReplayRate(float rate);

void C2_HOOK_FASTCALL SynchronizeActionReplay(void);

void C2_HOOK_FASTCALL DoActionReplayPostSwap(void);

#endif //REC2_REPLAY_H
