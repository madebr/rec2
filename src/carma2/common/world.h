#ifndef REC2_WORLD_H
#define REC2_WORLD_H

#include "c2_hooks.h"

#include "rec2_types.h"

tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void);

void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel);

tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void);

void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel);

tWall_texturing_level C2_HOOK_FASTCALL GetWallTexturingLevel(void);

void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel);

void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel);

int C2_HOOK_FASTCALL GetCarSimplificationLevel(void);

void C2_HOOK_FASTCALL ParseSpecialVolume(tTWTFILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx);

#endif // REC2_WORLD_H
