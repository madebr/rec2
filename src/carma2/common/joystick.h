#ifndef REC2_JOYSTICK_H
#define REC2_JOYSTICK_H

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(float, gForce_feedback_upper_limit);
C2_HOOK_VARIABLE_DECLARE(float, Force_feedback_lower_limit);


void C2_HOOK_STDCALL ReadFFB_TXT(void);

#endif /* REC2_JOYSTICK_H */
