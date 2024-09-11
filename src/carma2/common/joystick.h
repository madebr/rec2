#ifndef REC2_JOYSTICK_H
#define REC2_JOYSTICK_H

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(float, gForce_feedback_upper_limit);
C2_HOOK_VARIABLE_DECLARE(float, Force_feedback_lower_limit);
C2_HOOK_VARIABLE_DECLARE(int, gJoystick_index);
C2_HOOK_VARIABLE_DECLARE(float, gOriginal_joystick_x);
C2_HOOK_VARIABLE_DECLARE(float, gOriginal_joystick_y);
C2_HOOK_VARIABLE_DECLARE(int, gOriginal_joystick_fbb);
C2_HOOK_VARIABLE_DECLARE(int, gOriginal_joystick_dpad);
C2_HOOK_VARIABLE_DECLARE(int, gOrig_joystick_index);


void C2_HOOK_FASTCALL SetupFFBValues(void);

int C2_HOOK_FASTCALL PlayFFBEffect(const char* pEffect_name, int pArg2);

void C2_HOOK_FASTCALL StopJoysticks(void);

float C2_HOOK_FASTCALL GetJoystickX(void);

float C2_HOOK_FASTCALL GetJoystickY(void);

int C2_HOOK_FASTCALL GetJoystickFBBGain(void);

int C2_HOOK_FASTCALL IsJoystickDPadEnabled(void);

void C2_HOOK_FASTCALL SetJoystickX(float pValue);

void C2_HOOK_FASTCALL SetJoystickY(float pValue);

void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue);

void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled);

void C2_HOOK_FASTCALL Joystick_BackupSettings(void);

void C2_HOOK_FASTCALL EnableJoysticks(void);

void C2_HOOK_FASTCALL DisableJoysticks(void);

#endif /* REC2_JOYSTICK_H */
