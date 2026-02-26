#ifndef REC2_SMASHING_H
#define REC2_SMASHING_H

#include "rec2_types.h"

#include "c2_hooks.h"

#include "c2_stdio.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmash_vertex, gSmash_glass_fragments, 200);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmash_quad, gSmash_decals, 50);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmashable_race_target, gSmashable_race_targets, 300);
C2_HOOK_VARIABLE_DECLARE(int, gCount_smashable_race_targets);
C2_HOOK_VARIABLE_DECLARE(int, gCount_queued_smashes);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_munge_smash_edge_triggers);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gZero_vector__smash);

C2_HOOK_VARIABLE_DECLARE_ARRAY(tPowerup_queue_item, gPowerup_queue, 50);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tQueued_smash, gQueued_smashes, 20);
C2_HOOK_VARIABLE_DECLARE(br_matrix34, gMatrix34_006b78a0);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gVector3_006a4688);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006a3334);

void C2_HOOK_FASTCALL InitGlassFragments(void);

void C2_HOOK_FASTCALL InitSmashing(void);

void C2_HOOK_FASTCALL InitDecals(void);

void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec*  pInitial_pos);

void C2_HOOK_FASTCALL ReadSmashableInitialSpeed(FILE* pFile, tSmashable_initial_speed_spec* pInitial_speed);

void C2_HOOK_FASTCALL ReadMinMaxTimeInMilliseconds(FILE* pFile, int* pTimes);

void C2_HOOK_FASTCALL InitSmashTargets(void);

void C2_HOOK_FASTCALL AddSmashableRaceTarget(br_model* pModel, br_actor* pActor, int pUnknown);

void C2_HOOK_FASTCALL SplondificatalizeIdentifier(br_material* pMaterial, char** ppIdentifier, int pIndex);

void C2_HOOK_FASTCALL MungeSmashMaterialNames(void);

void C2_HOOK_FASTCALL CleanUpSmashStuff(void);

void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32  pTime, float pDelta_time);

void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);

void C2_HOOK_FASTCALL MungeDelayedSideEffects(void);

void C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor *pActor);

void C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime);

void C2_HOOK_FASTCALL InitSmashQueue(void);

void C2_HOOK_FASTCALL MungeInternalCarGlass(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);

void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec *pCar_Spec, tCar_crush_buffer_entry *pSmash_data);

void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

#endif //REC2_SMASHING_H
