#ifndef REC2_CONTROLS_H
#define REC2_CONTROLS_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gEntering_message);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char*, gAbuse_text, 10);
C2_HOOK_VARIABLE_DECLARE(int, gRecovery_voucher_count);
C2_HOOK_VARIABLE_DECLARE(int, gAuto_repair);
C2_HOOK_VARIABLE_DECLARE(int, gInstant_handbrake);

void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel);

void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel);

int C2_HOOK_FASTCALL GetSoundDetailLevel(void);

void C2_HOOK_FASTCALL DisposeAbuseomatic(void);

void C2_HOOK_FASTCALL CheckKevKeys(void);

void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing);

void C2_HOOK_FASTCALL CheckToggles(int pRacing);

void C2_HOOK_FASTCALL CheckOtherRacingKeys(void);

void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* car);

void C2_HOOK_FASTCALL ResetRecoveryVouchers(void);

#endif //REC2_CONTROLS_H
