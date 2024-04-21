#ifndef REC2_CONTROLS_H
#define REC2_CONTROLS_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gEntering_message);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char*, gAbuse_text, 10);
C2_HOOK_VARIABLE_DECLARE(int, gRecovery_voucher_count);
C2_HOOK_VARIABLE_DECLARE(int, gAuto_repair);
C2_HOOK_VARIABLE_DECLARE(int, gInstant_handbrake);
C2_HOOK_VARIABLE_DECLARE(tU32, gToo_poor_for_recovery_timeout);
C2_HOOK_VARIABLE_DECLARE(int, gCheckpoint_finder_enabled);

void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel);

void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel);

int C2_HOOK_FASTCALL GetSoundDetailLevel(void);

void C2_HOOK_FASTCALL ToggleMiniMap(void);

void C2_HOOK_FASTCALL ToggleDoors(void);

int C2_HOOK_FASTCALL ToggleDoorsCollisionInfoCallback(tCollision_info* pCollision_info, void* data);

int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data);

void C2_HOOK_FASTCALL ToggleMap(void);

void C2_HOOK_FASTCALL FUN00444600(void);

void C2_HOOK_FASTCALL ToggleCheckpointFinder(void);

void C2_HOOK_FASTCALL ToggleMapTrans(void);

void C2_HOOK_FASTCALL SetRecovery(void);

int C2_HOOK_FASTCALL CheckRecoverCost(void);

void C2_HOOK_FASTCALL AbortRace(void);

void C2_HOOK_FASTCALL ToggleHUD(void);

void C2_HOOK_FASTCALL F4Key(void);

void C2_HOOK_FASTCALL F5Key(void);

void C2_HOOK_FASTCALL F6Key(void);

void C2_HOOK_FASTCALL F7Key(void);

void C2_HOOK_FASTCALL F8Key(void);

void C2_HOOK_FASTCALL F10Key(void);

void C2_HOOK_FASTCALL F11Key(void);

void C2_HOOK_FASTCALL F12Key(void);

void C2_HOOK_FASTCALL NumberKey0(void);

void C2_HOOK_FASTCALL NumberKey1(void);

void C2_HOOK_FASTCALL NumberKey2(void);

void C2_HOOK_FASTCALL NumberKey3(void);

void C2_HOOK_FASTCALL NumberKey4(void);

void C2_HOOK_FASTCALL NumberKey5(void);

void C2_HOOK_FASTCALL NumberKey6(void);

void C2_HOOK_FASTCALL NumberKey7(void);

void C2_HOOK_FASTCALL NumberKey8(void);

void C2_HOOK_FASTCALL NumberKey9(void);

void C2_HOOK_FASTCALL ToggleFlying(void);

void C2_HOOK_FASTCALL ScreenSmaller(void);

void C2_HOOK_FASTCALL ScreenLarger(void);

void C2_HOOK_FASTCALL BuyArmour(void);

void C2_HOOK_FASTCALL BuyPower(void);

void C2_HOOK_FASTCALL BuyOffense(void);

void C2_HOOK_FASTCALL FUN0040e430(void);

void C2_HOOK_FASTCALL UserSendMessage(void);

void C2_HOOK_FASTCALL ToggleTargetLock(void);

void C2_HOOK_FASTCALL ToggleTarget(void);

void C2_HOOK_FASTCALL PowerupInventoryToggle(void);

void C2_HOOK_FASTCALL PowerupInventoryNext(void);

void C2_HOOK_FASTCALL PowerupInventoryPrevious(void);

void C2_HOOK_FASTCALL ToggleCam(void);

void C2_HOOK_FASTCALL ToggleCockpit(void);

void C2_HOOK_FASTCALL DisposeAbuseomatic(void);

void C2_HOOK_FASTCALL CheckKevKeys(void);

void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing);

void C2_HOOK_FASTCALL CheckToggles(int pRacing);

void C2_HOOK_FASTCALL CheckOtherRacingKeys(void);

void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL FlipUpCollisionInfo(tCollision_info* pCollision_info);

void C2_HOOK_FASTCALL ResetRecoveryVouchers(void);

#endif //REC2_CONTROLS_H
