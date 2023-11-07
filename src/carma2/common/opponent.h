#ifndef REC2_OPPONENT_H
#define REC2_OPPONENT_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_stdio.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gActive_car_list_rebuild_required);
C2_HOOK_VARIABLE_DECLARE(int, gBIG_APC_index);
C2_HOOK_VARIABLE_DECLARE(int, gNumber_of_cops_before_faffage);
C2_HOOK_VARIABLE_DECLARE(tU32, gNext_grudge_reduction);
C2_HOOK_VARIABLE_DECLARE(int, gFirst_frame);
C2_HOOK_VARIABLE_DECLARE(int, gGrudge_reduction_per_period);
C2_HOOK_VARIABLE_DECLARE(int, gAcknowledged_start);
C2_HOOK_VARIABLE_DECLARE(int, gStart_jumped);
C2_HOOK_VARIABLE_DECLARE(br_scalar, gMinimum_yness_before_knackerisation);
C2_HOOK_VARIABLE_DECLARE(tU32, gAcme_frame_count);
C2_HOOK_VARIABLE_DECLARE(br_scalar, gHead_on_cos_value);
C2_HOOK_VARIABLE_DECLARE(int, gBig_bang);
C2_HOOK_VARIABLE_DECLARE(float, gOpponent_nastyness_frigger);
C2_HOOK_VARIABLE_DECLARE(br_scalar, gIn_view_distance);
C2_HOOK_VARIABLE_DECLARE(int, gChallenger_index__opponent);
C2_HOOK_VARIABLE_DECLARE(tU8*, gBit_per_node);

void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);

void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp);

void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index);

void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF);

int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo);

void C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec);

void C2_HOOK_FASTCALL ProcessPursueAndTwat(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);

void C2_HOOK_FASTCALL DisposeOpponentPaths(void);

void C2_HOOK_FASTCALL RebuildActiveCarList(void);

void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void);

void C2_HOOK_FASTCALL DisposeOpponents(void);

tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);

int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);

void C2_HOOK_FASTCALL StunTheBugger(tOpponent_spec* pOpponent_spec, int pMilliseconds);

tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex);

tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec);

void C2_HOOK_CDECL oppo_dprintf(const char* pMessage, ...);

void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info);

void C2_HOOK_FASTCALL MungeOpponents(void);

#endif //REC2_OPPONENT_H
