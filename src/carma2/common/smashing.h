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

void C2_HOOK_FASTCALL InitGlassFragments(void);

void C2_HOOK_FASTCALL InitSmashing(void);

void C2_HOOK_FASTCALL InitDecals(void);

void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec*  pInitial_pos);

void C2_HOOK_FASTCALL ReadSmashableInitialSpeed(FILE* pFile, tSmashable_initial_speed_spec* pInitial_speed);

void C2_HOOK_FASTCALL ReadMinMaxTimeInMilliseconds(FILE* pFile, int* pTimes);

void C2_HOOK_FASTCALL InitSmashTargets(void);

void C2_HOOK_FASTCALL AddSmashableRaceTarget(br_model* pModel, br_actor* pActor, int pUnknown);

void C2_HOOK_FASTCALL MungeSmashMaterialNames(void);

void C2_HOOK_FASTCALL CleanUpSmashStuff(void);

void C2_HOOK_FASTCALL MungeGlassFragments(void);

void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);

void C2_HOOK_FASTCALL MungeDelayedSideEffects(void);

void C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime);

void C2_HOOK_FASTCALL InitSmashQueue(void);

#endif //REC2_SMASHING_H
