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
C2_HOOK_VARIABLE_DECLARE_ARRAY(tShit_mine, gShit_mines, 20);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tRepulse_link, gRepulse_links, 6);
C2_HOOK_VARIABLE_DECLARE(tPhysics_joint*, gMutant_tail_first_joint);
C2_HOOK_VARIABLE_DECLARE(tCollision_info*, gMutant_tail_first_collision_info);
C2_HOOK_VARIABLE_DECLARE(int, gMutant_tail_state);
C2_HOOK_VARIABLE_DECLARE(int, gINT_0074a5ec);

void C2_HOOK_FASTCALL InitRepulseEffects(void);

void C2_HOOK_FASTCALL InitTail(void);

void C2_HOOK_FASTCALL InitPowerups(void);

void C2_HOOK_FASTCALL ResetPowerups(void);

br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pMap);

void C2_HOOK_FASTCALL LoadPowerups(void);

void C2_HOOK_FASTCALL ReadPowerupSmashables(FILE* pF);

void C2_HOOK_FASTCALL InitMineShit(void);

int C2_HOOK_FASTCALL MineExplode(tShit_mine *pMine);

void C2_HOOK_FASTCALL ProcessShitMines(tU32 pTime);

void C2_HOOK_FASTCALL RemoveTail(void);

void C2_HOOK_FASTCALL CloseDownPowerUps(void);

void C2_HOOK_FASTCALL SetAPOToInitialValues(void);

void C2_HOOK_FASTCALL MungePowerupStuff(undefined4 pArg1);

void C2_HOOK_FASTCALL DoPowerupPeriodics(tU32 pFrame_period);

void C2_HOOK_CDECL RenderSpinningPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen);

#endif //REC2_POWERUPS_H
