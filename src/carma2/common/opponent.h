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
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_dk_yel);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_md_yel);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_lt_yel);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_dk_red);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_md_red);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_lt_red);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_dk_grn);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_md_grn);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_lt_grn);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_dk_blu);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_lt_blu);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_dk_turq);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_lt_turq);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_dk_gry);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_md_gry);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMat_lt_gry);
C2_HOOK_VARIABLE_DECLARE(int, gNum_of_opponents_pursuing);
C2_HOOK_VARIABLE_DECLARE(int, gNum_of_opponents_getting_near);
C2_HOOK_VARIABLE_DECLARE(int, gNum_of_opponents_completing_race);

void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);

void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp);

void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index);

void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF);

int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo);

void C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec);

void C2_HOOK_FASTCALL ProcessPursueAndTwat(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);

void C2_HOOK_FASTCALL DisposeOpponentPaths(void);

void C2_HOOK_FASTCALL AddIfNotInList(tCollision_info* pObject, tCollision_info** pList, int pCount);

void C2_HOOK_FASTCALL RemoveAnythingStillInList(tCollision_info** pList, int pCount);

void C2_HOOK_FASTCALL NoteCarsCurrentlyUsed(tCollision_info** pCollision_infos, int* pCount);

int C2_HOOK_FASTCALL IsNetCarActive(const br_vector3* pPoint);

void C2_HOOK_FASTCALL RebuildActiveCarList(void);

void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void);

void C2_HOOK_FASTCALL DisposeOpponents(void);

tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);

const char* C2_HOOK_FASTCALL GetDriverName(tVehicle_type pCategory, int pIndex);

int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);

void C2_HOOK_FASTCALL StunTheBugger(tOpponent_spec* pOpponent_spec, int pMilliseconds);

tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex);

tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec);

void C2_HOOK_CDECL DoNotDprintf_opponent(const char* pMessage, ...);

void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info);

void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pNegative_x_vector, br_vector3* pStart, br_vector3* pFinish, br_scalar pLength);

void C2_HOOK_FASTCALL CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(tOpponent_spec* pOpponent_spec);

void C2_HOOK_FASTCALL TurnOpponentPhysicsOn(tOpponent_spec* pOpponent_spec);

void C2_HOOK_FASTCALL ProcessThisOpponent(tOpponent_spec* pOpponent_spec);

void C2_HOOK_FASTCALL StartRecordingTrail(tCar_spec* pPursuee);

void C2_HOOK_FASTCALL RecordNextTrailNode(tCar_spec* pPursuee);

void C2_HOOK_FASTCALL ClearTwattageOccurrenceVariables(tOpponent_spec* pOpponent_spec);

void C2_HOOK_FASTCALL MungeOpponents(void);

void C2_HOOK_FASTCALL UnStunTheBugger(tOpponent_spec* pOpponent_spec);

void C2_HOOK_FASTCALL WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(int pWhat_the_countdown_was);

tS16 C2_HOOK_FASTCALL FindNearestPathSection(undefined4* pArg1, br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance);

#endif //REC2_OPPONENT_H
