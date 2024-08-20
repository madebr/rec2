#ifndef REC2_STRUCTUR_H
#define REC2_STRUCTUR_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gMirror_on__structur);
C2_HOOK_VARIABLE_DECLARE(int, gLoad_last_save_game);
C2_HOOK_VARIABLE_DECLARE(int, gMoney_pre_race);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gAPO_pre_race, 3);
C2_HOOK_VARIABLE_DECLARE(tRace_over_reason, gRace_over_reason);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_checkpoint_time);
C2_HOOK_VARIABLE_DECLARE(int, gLast_wrong_checkpoint);

void C2_HOOK_FASTCALL RecordAPOAndCredits(void);

int C2_HOOK_FASTCALL DoPostRace(tRace_result pRace_result);

void C2_HOOK_FASTCALL DoProgram(void);

void C2_HOOK_FASTCALL InitialiseProgramState(void);

void C2_HOOK_FASTCALL DoLogos(void);

void C2_HOOK_FASTCALL DoProgramDemo(void);

void C2_HOOK_FASTCALL DoProgOpeningAnimation(void);

void C2_HOOK_FASTCALL SwapNetCarsLoad(void);

void C2_HOOK_FASTCALL DoGame(void);

int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness);

int C2_HOOK_FASTCALL NumberOfOpponentsStillRunning(void);

void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info);

int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void);

void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);

void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound);

void C2_HOOK_FASTCALL IncrementCheckpoint(void);

void C2_HOOK_FASTCALL IncrementLap(void);

void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index);

void C2_HOOK_FASTCALL CheckCheckpoints(void);

void C2_HOOK_FASTCALL TotalRepair(void);

int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence);

void C2_HOOK_FASTCALL JumpTheStart(void);

void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void);

void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void);

void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

#endif // REC2_STRUCTUR_H
