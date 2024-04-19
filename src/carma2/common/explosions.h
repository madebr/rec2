#ifndef REC2_EXPLOSIONS_H
#define REC2_EXPLOSIONS_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

void C2_HOOK_FASTCALL InitExplosions(void);

void C2_HOOK_FASTCALL ResetExplosions(void);

void C2_HOOK_FASTCALL ReadExplosionInfo(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups);

#endif // REC2_EXPLOSIONS_H
