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
C2_HOOK_VARIABLE_DECLARE(br_model*, gModel_powerup_armor);
C2_HOOK_VARIABLE_DECLARE(br_model*, gModel_powerup_power);
C2_HOOK_VARIABLE_DECLARE(br_model*, gModel_powerup_offence);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gInventory, 20);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006a0a6c);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006a0a5c);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tRespawn_powerup, gRespawn_powerups, 100);
C2_HOOK_VARIABLE_DECLARE(int, gInventory_selected);
C2_HOOK_VARIABLE_DECLARE(int, gNumber_of_icons);
C2_HOOK_VARIABLE_DECLARE(tU32, gNext_goody_time);
C2_HOOK_VARIABLE_DECLARE(undefined4, gDAT_006a0958);
C2_HOOK_VARIABLE_DECLARE(int, gCount_cloaked_cars);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tCar_spec*, gCloaked_cars, 12);

int C2_HOOK_FASTCALL GetPowerupCount(void);

void C2_HOOK_FASTCALL InitRepulseEffects(void);

void C2_HOOK_FASTCALL InitTail(void);

void C2_HOOK_FASTCALL InitPowerups(void);

void C2_HOOK_FASTCALL RemoveRepulseEffect(tRepulse_link* pRepulse);

void C2_HOOK_FASTCALL ResetRepulseEffects(void);

void C2_HOOK_FASTCALL EmptyInventory(void);

void C2_HOOK_FASTCALL ResetPowerups(void);

void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime);

br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pMap);

void C2_HOOK_FASTCALL LoadPowerups(void);

void C2_HOOK_FASTCALL ReadPowerupSmashables(FILE* pF);

void C2_HOOK_FASTCALL InitMineShit(void);

int C2_HOOK_FASTCALL MineExplode(tShit_mine *pMine);

void C2_HOOK_FASTCALL ProcessShitMines(tU32 pTime);

void C2_HOOK_FASTCALL RemoveTail(void);

void C2_HOOK_FASTCALL CloseDownPowerUps(void);

void C2_HOOK_FASTCALL MaxOutAPO(void);

void C2_HOOK_FASTCALL CheckRespawnQueue(tU32 pTime);

void C2_HOOK_FASTCALL MungePowerupStuff(undefined4 pArg1);

tU32 C2_HOOK_FASTCALL GetNextGoodyTime(tU32 pTime);

void C2_HOOK_FASTCALL AutoGoody(tU32 pTime);

void C2_HOOK_FASTCALL PeriodicCloaking(void);

void C2_HOOK_FASTCALL LosePowerup(tPowerup* pPowerup);

int C2_HOOK_FASTCALL GotPowerupXX(tCar_spec* pCar, int pArg2, int pArg3);

void C2_HOOK_FASTCALL MungeKeyboardPowerup(void);

void C2_HOOK_FASTCALL DoPowerupPeriodics(tU32 pFrame_period);

void C2_HOOK_CDECL RenderSpinningPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen);

void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity);

void C2_HOOK_CDECL RenderChangingPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen);

br_model* C2_HOOK_FASTCALL DuplicateModel(br_model* pModel, const char* pName);

void C2_HOOK_FASTCALL SetChangingPowerup(br_actor* pActor);

int C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex);

int C2_HOOK_FASTCALL GotCredits(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetPedSize(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetPedExplode(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL PickAtRandom(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL DoInstantRepair(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL FreezeTimer(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetEngineFactor(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetUnderwater(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL GotTimeOrPower(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL TrashBodywork(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL TakeDrugs(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetOpponentsSpeed(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetGravity(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetPinball(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetWallclimb(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetBouncey(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetSuspension(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetSuicidalPedestrians(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL GotVouchers(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetMassMultiplier(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL HitMine(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetPedHeadSize(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL ShitMine(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL PissOutOil(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL KangerooJump(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL AnnihilatePeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL RepulseOpponents(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetGhostPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL RepulsePedestrians(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL GainAPO(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL GainAPOPotential(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetPissed(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL NapalmPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL PedValium(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL PowerupCancel(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL GrowATail(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL CutOffTail(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar_spec);

int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetPedSize(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetInvulnerability(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetEngineFactor(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL PukeDrugsBackUp(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetOpponentsSpeed(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetGravity(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetBouncey(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetSuspension(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetMassMultiplier(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL HidePedestrians(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetPedHeadSize(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetGhostPeds(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetPissed(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL TheEffectsOfDrugs(tPowerup* powerup, tU32 time);

void C2_HOOK_FASTCALL DoBouncey(tPowerup* powerup, tU32 time);

void C2_HOOK_FASTCALL MungeVouchers(tPowerup* powerup, tU32 time);

void C2_HOOK_FASTCALL PissedEffect(tPowerup* powerup, tU32 time);

void C2_HOOK_FASTCALL SendMines(tU32 pNext_frame_time);

void C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory);

void C2_HOOK_FASTCALL LosePowerupX(tPowerup* pPowerup, undefined4 pArg2);

void C2_HOOK_FASTCALL KeyboardPowerupFinished(tPowerup* pPowerup, int pSelect_new);

int C2_HOOK_FASTCALL GotPowerupX(tCar_spec* pCar, int pIndex, int pArg3, int pMessage, tU32 pTime);

int C2_HOOK_FASTCALL DrawSinglePowerupIcon(int pDraw, int pTime, tPowerup* pPowerup, tHeadup_icon* pIcon, int pX, int pY, int pUpdate_x);

void C2_HOOK_FASTCALL DrawPowerupIcons2(tU32 pTime, tHeadup_icon* pHeadup_icons, int* pCount_icons, int pMax_count_icons, int pX, int pY);

void C2_HOOK_FASTCALL DrawKeyPowerups(tU32 pTime);

void C2_HOOK_FASTCALL DrawPowerups(tU32 pTime);

#endif //REC2_POWERUPS_H
