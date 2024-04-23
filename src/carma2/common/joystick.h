#ifndef REC2_JOYSTICK_H
#define REC2_JOYSTICK_H

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(float, gForce_feedback_upper_limit);
C2_HOOK_VARIABLE_DECLARE(float, Force_feedback_lower_limit);


void C2_HOOK_STDCALL ReadFFB_TXT(void);

int C2_HOOK_FASTCALL PlayFFBEffect(const char* pEffect_name, int pArg2);

void C2_HOOK_FASTCALL StopJoysticks(void);

#endif /* REC2_JOYSTICK_H */
