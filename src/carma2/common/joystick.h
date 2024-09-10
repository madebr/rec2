#ifndef REC2_JOYSTICK_H
#define REC2_JOYSTICK_H

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(float, gForce_feedback_upper_limit);
C2_HOOK_VARIABLE_DECLARE(float, Force_feedback_lower_limit);
C2_HOOK_VARIABLE_DECLARE(int, gJoystick_index);


void C2_HOOK_FASTCALL SetupFFBValues(void);

int C2_HOOK_FASTCALL PlayFFBEffect(const char* pEffect_name, int pArg2);

void C2_HOOK_FASTCALL StopJoysticks(void);

float C2_HOOK_FASTCALL GetJoystickX(void);

float C2_HOOK_FASTCALL GetJoystickY(void);

#endif /* REC2_JOYSTICK_H */
