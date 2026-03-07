#include "globvrkm.h"

#include "globvars.h"


// GLOBAL: CARMA2_HW 0x0074c7b0
br_actor* gCamera_list[2];

// GLOBAL: CARMA2_HW 0x0068b910
int gCar_flying;

// GLOBAL: CARMA2_HW 0x00655f54
tCar_spec* gCar_to_view = &gProgram_state.current_car;

// GLOBAL: CARMA2_HW 0x0068d940
tU32 gCar_to_view_id;

// GLOBAL: CARMA2_HW 0x0068b914
int gCamera_reset;

// GLOBAL: CARMA2_HW 0x0068b90c
int gCamera_sign;

// GLOBAL: CARMA2_HW 0x0068b908
br_angle gCamera_yaw;

// GLOBAL: CARMA2_HW 0x0074c9f0
br_scalar gCamera_height;

// GLOBAL: CARMA2_HW 0x00655f48
br_vector3 gView_direction = { { 0.f, 0.f, -1.f }};

// GLOBAL: CARMA2_HW 0x0074c7b8
br_scalar gMin_camera_car_distance;