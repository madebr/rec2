#ifndef REC2_POWERUPS_H
#define REC2_POWERUPS_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

C2_HOOK_VARIABLE_DECLARE(tSmashable_item_spec, gPowerup_pickup_smashable);
C2_HOOK_VARIABLE_DECLARE(tSmashable_item_spec, gPowerup_respawn_smashable);
C2_HOOK_VARIABLE_DECLARE(tSmashable_item_spec, gUnknown_smashable_006a3660);

C2_HOOK_VARIABLE_DECLARE_ARRAY(tHeadup_icon, gPickedup_powerups, 5);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gRepeatability_names, 4);

void C2_HOOK_FASTCALL InitPowerups(void);

br_actor* C2_HOOK_FASTCALL CreateActorFromPowerupMap(br_pixelmap* pMap);

void C2_HOOK_FASTCALL LoadPowerups(void);

void C2_HOOK_FASTCALL ReadPowerupSmashables(FILE* pF);

void C2_HOOK_FASTCALL InitShitMines(void);

void C2_HOOK_FASTCALL ProcessShitMines(tU32 pTime);

#endif //REC2_POWERUPS_H
