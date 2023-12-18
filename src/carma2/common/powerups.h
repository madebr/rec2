#ifndef REC2_POWERUPS_H
#define REC2_POWERUPS_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tHeadup_icon, gPickedup_powerups, 5);

void C2_HOOK_FASTCALL InitPowerups(void);

br_actor* C2_HOOK_FASTCALL CreateActorFromPowerupMap(br_pixelmap* pMap);

void C2_HOOK_FASTCALL LoadPowerups(void);

#endif //REC2_POWERUPS_H
