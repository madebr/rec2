#ifndef REC2_JOYSTICK_H
#define REC2_JOYSTICK_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern int gJoystick_index;
extern float gJoystick_x_steering;
extern float gJoystick_y_throttle;
extern int gINT_00595f84;
extern int gINT_00596310;

extern int C2_HOOK_FASTCALL FindJoystickPreferences(FILE* pF, const char* pJoystick_name);
extern int C2_HOOK_FASTCALL ReadJoystickPreferences(FILE* pF, tButtonJoystickInfo* pInfo);
extern void C2_HOOK_FASTCALL EnableJoysticks(void);
extern void C2_HOOK_FASTCALL DisableJoysticks(void);

extern int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

#endif /* REC2_JOYSTICK_H */
