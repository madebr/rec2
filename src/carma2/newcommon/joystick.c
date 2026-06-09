#include "joystick.h"


// GLOBAL: CARMA2_HW 0x00595f88
int gJoystick_index = -1;

// GLOBAL: CARMA2_HW 0x00595f90
float gJoystick_x_steering = 1.f;

// GLOBAL: CARMA2_HW 0x00595f94
float gJoystick_y_throttle = 1.f;


// STUB: CARMA2_HW 0x0045c0b0
int C2_HOOK_FASTCALL LoadJoystickPreferences(void) {

    NOT_IMPLEMENTED();
    return 0;
}
