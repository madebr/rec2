#include "globvrbm.h"


// GLOBAL: CARMA2_HW 0x0074ca90
br_actor* gPrat_actor;

// GLOBAL: CARMA2_HW 0x0074cac4
br_actor* g2d_camera;

// GLOBAL: CARMA2_HW 0x0074cf34
br_actor* gPowerupHUD_actor;

// GLOBAL: CARMA2_HW 0x0074ca7c
br_actor* gStatbarHUD1_actor;

// GLOBAL: CARMA2_HW 0x0074cab0
br_actor* gHeadup2_actor;

// GLOBAL: CARMA2_HW 0x0074cef0
br_actor* gHUDsquare_actor;

// GLOBAL: CARMA2_HW 0x0074cf0c
br_actor* gMapHUD_actor;

// GLOBAL: CARMA2_HW 0x0074ca24
br_actor* gTimerRightHUD_actor;

// GLOBAL: CARMA2_HW 0x0074ca84
br_actor* gOffense_actor;

// GLOBAL: CARMA2_HW 0x0074ca40
br_actor* gTimerLeftHUD_actor;

// GLOBAL: CARMA2_HW 0x0074ca98
br_actor* gPower_actor;

// GLOBAL: CARMA2_HW 0x0074ca68
br_actor* gHUDcurve_actor;

// GLOBAL: CARMA2_HW 0x0074ca58
br_actor* gStatbarRightHUD_actor;

// GLOBAL: CARMA2_HW 0x0074cef4
br_actor* gHeadup_actor;

// GLOBAL: CARMA2_HW 0x0074cf18
br_actor* gStatbarHUD5_actor;

// GLOBAL: CARMA2_HW 0x0074cf24
br_actor* gDim_actor;

// GLOBAL: CARMA2_HW 0x0074cf80
br_actor* gArmour_actor;

// GLOBAL: CARMA2_HW 0x0074c9f8
br_actor* gHUD_root_actor;

// GLOBAL: CARMA2_HW 0x0074ca14
br_actor* gStatbarHUD3_actor;

// GLOBAL: CARMA2_HW 0x0074cabc
br_actor* gTestFont_actor;


// GLOBAL: CARMA2_HW 0x0074cefc
br_model* gPrat_model;

// GLOBAL: CARMA2_HW 0x0074ca78
br_model* gMapHUD_model;

// GLOBAL: CARMA2_HW 0x0074cf6c
br_model* gStatbarRightHUD_model;

// GLOBAL: CARMA2_HW 0x0074cf54
br_model* gTestFont_model;

// GLOBAL: CARMA2_HW 0x0074caa0
br_model* gHeadup_model;

// GLOBAL: CARMA2_HW 0x0074ca3c
br_model* gHUDsquare_model;

// GLOBAL: CARMA2_HW 0x0074cad8
br_model* gTimerRightHUD_model;

// GLOBAL: CARMA2_HW 0x0074ca64
br_model* gOffence_model;

// GLOBAL: CARMA2_HW 0x0074cf88
br_model* gTimerLeftHUD_model;

// GLOBAL: CARMA2_HW 0x0074cee0
br_model* gPower_model;

// GLOBAL: CARMA2_HW 0x0074ca88
br_model* gStatbarHUD5_model;

// GLOBAL: CARMA2_HW 0x0074cf44
br_model* gStatbarHUD3_model;

// GLOBAL: CARMA2_HW 0x0074ca50
br_model* gStatbarHUD1_model;

// GLOBAL: CARMA2_HW 0x0074cf4c
br_model* gPowerupHUD_model;

// GLOBAL: CARMA2_HW 0x0074cf3c
br_model* gHeadup2_model;

// GLOBAL: CARMA2_HW 0x0074ca70
br_model* gDim_model;

// GLOBAL: CARMA2_HW 0x0074cef8
br_model* gHUDcurve_model;

// GLOBAL: CARMA2_HW 0x0074cf08
br_model* gArmour_model;


// GLOBAL: CARMA2_HW 0x0074cf78
br_material* gPowerupHUD_material;

// GLOBAL: CARMA2_HW 0x0074cf28
br_material* gTestFont_material;

// GLOBAL: CARMA2_HW 0x0074ca38
br_material* gArmour_material;

// GLOBAL: CARMA2_HW 0x0074cf14
br_material* gStatbarHUD1_material;

// GLOBAL: CARMA2_HW 0x0074c9fc
br_material* gStatbarHUD5_material;

// GLOBAL: CARMA2_HW 0x0074cf50
br_material* gHeadup_material;

// GLOBAL: CARMA2_HW 0x0074cacc
br_material* gStatbarHUD3_material;

// GLOBAL: CARMA2_HW 0x0074ca9c
br_material* gOffence_material;

// GLOBAL: CARMA2_HW 0x0074cf58
br_material* gPower_material;

// GLOBAL: CARMA2_HW 0x0074ca8c
br_material* gStatbarRightHUD_material;

// GLOBAL: CARMA2_HW 0x0074ca30
br_material* gPrat_material;

// GLOBAL: CARMA2_HW 0x0074caa4
br_material* gHUDcurve_material;

// GLOBAL: CARMA2_HW 0x0074cf70
br_material* gMapHUD_material;

// GLOBAL: CARMA2_HW 0x0074cf1c
br_material* gHeadup2_material;

// GLOBAL: CARMA2_HW 0x0074cac0
br_material* gTimerRightHUD_material;

// GLOBAL: CARMA2_HW 0x0074cad4
br_material* gDim_material;

// GLOBAL: CARMA2_HW 0x0074cf40
br_material* gHUDsquare_material;

// GLOBAL: CARMA2_HW 0x0074cf64
br_material* gTimerLeftHUD_material;


// GLOBAL: CARMA2_HW 0x00655e64
int gHeadup_dim_w = 0xc0;

// GLOBAL: CARMA2_HW 0x00655e68
int gHeadup_dim_h = 0x26;

// GLOBAL: CARMA2_HW 0x00655e6c
int gHeadup_dim_x = 0x44;

// GLOBAL: CARMA2_HW 0x00655e70
int gHeadup_dim_y = 0x1b3;


// GLOBAL: CARMA2_HW 0x00655e74
int gHeadup2_dim_w = 0xb8;

// GLOBAL: CARMA2_HW 0x00655e78
int gHeadup2_dim_h = 0x17;

// GLOBAL: CARMA2_HW 0x00655e7c
int gHeadup2_dim_x = 0x124;

// GLOBAL: CARMA2_HW 0x00655e80
int gHeadup2_dim_y = 0x1c3;


// GLOBAL: CARMA2_HW 0x00655e84
int gPowerupHUD_dim_w = 0x40;

// GLOBAL: CARMA2_HW 0x00655e88
int gPowerupHUD_dim_h = 0x40;

// GLOBAL: CARMA2_HW 0x00655e8c
int gPowerupHUD_dim_x = 0xa;

// GLOBAL: CARMA2_HW 0x00655e90
int gPowerupHUD_dim_y = 0x8c;


// GLOBAL: CARMA2_HW 0x00655e94
int gStatbarHUD1_dim_w = 0x80;

// GLOBAL: CARMA2_HW 0x00655e98
int gStatbarHUD1_dim_h = 0x80;

// GLOBAL: CARMA2_HW 0x0068b8f0
int gStatbarHUD1_dim_x;

// GLOBAL: CARMA2_HW 0x0068b8f4
int gStatbarHUD1_dim_y;


// GLOBAL: CARMA2_HW 0x00655eac
int gStatbarHUD3_dim_w = 0x8e;

// GLOBAL: CARMA2_HW 0x00655eb0
int gStatbarHUD3_dim_h = 0x26;

// GLOBAL: CARMA2_HW 0x00655eb4
int gStatbarHUD3_dim_x = 0x80;

// GLOBAL: CARMA2_HW 0x00655eb8
int gStatbarHUD3_dim_y = 0xa;


// GLOBAL: CARMA2_HW 0x00655ebc
int gHUDsquare_dim_w = 0x80;

// GLOBAL: CARMA2_HW 0x00655ec0
int gHUDsquare_dim_h = 0x40;

// GLOBAL: CARMA2_HW 0x00655ec4
int gHUDsquare_dim_x = 0x40;

// GLOBAL: CARMA2_HW 0x0068b8f8
int gHUDsquare_dim_y;


// GLOBAL: CARMA2_HW 0x00655ec8
int gStatbarHUD5_dim_w = 0x40;

// GLOBAL: CARMA2_HW 0x00655ecc
int gStatbarHUD5_dim_h = 0x80;

// GLOBAL: CARMA2_HW 0x00655ed0
int gStatbarHUD5_dim_x = 0x240;

// GLOBAL: CARMA2_HW 0x0068b8fc
int gStatbarHUD5_dim_y;


// GLOBAL: CARMA2_HW 0x00655eec
int gStatbarRightHUD_dim_w = 0xb2;

// GLOBAL: CARMA2_HW 0x00655ef0
int gStatbarRightHUD_dim_h = 0x26;

// GLOBAL: CARMA2_HW 0x00655ef4
int gStatbarRightHUD_dim_x = 0x18e;

// GLOBAL: CARMA2_HW 0x00655ef8
int gStatbarRightHUD_dim_y = 0xa;


// GLOBAL: CARMA2_HW 0x00655ed4
int gTimerLeftHUD_dim_w = 0x40;

// GLOBAL: CARMA2_HW 0x00655ed8
int gTimerLeftHUD_dim_h = 0x40;

// GLOBAL: CARMA2_HW 0x00655edc
int gTimerLeftHUD_dim_x = 0x10e;

// GLOBAL: CARMA2_HW 0x0068b900
int gTimerLeftHUD_dim_y;


// GLOBAL: CARMA2_HW 0x00655ee0
int gTimerRightHUD_dim_w = 0x40;

// GLOBAL: CARMA2_HW 0x00655ee4
int gTimerRightHUD_dim_h = 0x40;

// GLOBAL: CARMA2_HW 0x00655ee8
int gTimerRightHUD_dim_x = 0x14e;

// GLOBAL: CARMA2_HW 0x0068b904
int gTimerRightHUD_dim_y;


// GLOBAL: CARMA2_HW 0x00655f0c
int gArmour_dim_w = 0x80;

// GLOBAL: CARMA2_HW 0x00655f10
int gArmour_dim_h = 0x10;

// GLOBAL: CARMA2_HW 0x00655f14
int gArmour_dim_x = 0x95;

// GLOBAL: CARMA2_HW 0x00655f18
int gArmour_dim_y = 0x31;


// GLOBAL: CARMA2_HW 0x00655f1c
int gPower_dim_w = 0x80;

// GLOBAL: CARMA2_HW 0x00655f20
int gPower_dim_h = 0x10;

// GLOBAL: CARMA2_HW 0x00655f24
int gPower_dim_x = 0x12a;

// GLOBAL: CARMA2_HW 0x00655f28
int gPower_dim_y = 0x31;


// GLOBAL: CARMA2_HW 0x00655f2c
int gOffence_dim_w = 0x80;

// GLOBAL: CARMA2_HW 0x00655f30
int gOffence_dim_h = 0x10;

// GLOBAL: CARMA2_HW 0x00655f34
int gOffence_dim_x = 0x1c0;

// GLOBAL: CARMA2_HW 0x00655f38
int gOffence_dim_y = 0x31;
