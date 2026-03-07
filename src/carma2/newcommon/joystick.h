#ifndef REC2_JOYSTICK_H
#define REC2_JOYSTICK_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern int gJoystick_index;
extern float gJoystick_x_steering;
extern float gJoystick_y_throttle;


extern int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

#endif /* REC2_JOYSTICK_H */
