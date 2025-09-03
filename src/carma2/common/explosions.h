#ifndef REC2_EXPLOSIONS_H
#define REC2_EXPLOSIONS_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmash_explosion, gSmash_explosions, 20);

void C2_HOOK_FASTCALL InitExplosions(void);

void C2_HOOK_FASTCALL ResetExplosions(void);

void C2_HOOK_FASTCALL ReadExplosionInfo(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups);

void C2_HOOK_FASTCALL MungeExplosions(void);

void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);

#endif // REC2_EXPLOSIONS_H
