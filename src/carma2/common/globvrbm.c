#include "globvrbm.h"

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gPrat_actor, 0x0074ca90);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, g2d_camera, 0x0074cac4);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gPowerupHUD_actor, 0x0074cf34);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gStatbarHUD1_actor, 0x0074ca7c);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gHeadup2_actor, 0x0074cab0);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gHUDsquare_actor, 0x0074cef0);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gMapHUD_actor, 0x0074cf0c);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gTimerRightHUD_actor, 0x0074ca24);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gOffense_actor, 0x0074ca84);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gTimerLeftHUD_actor, 0x0074ca40);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gPower_actor, 0x0074ca98);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gHUDcurve_actor, 0x0074ca68);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gStatbarRightHUD_actor, 0x0074ca58);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gHeadup_actor, 0x0074cef4);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gStatbarHUD5_actor, 0x0074cf18);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gDim_actor, 0x0074cf24);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gArmour_actor, 0x0074cf80);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gHUD_root_actor, 0x0074c9f8);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gStatbarHUD3_actor, 0x0074ca14);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gTestFont_actor, 0x0074cabc);

C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gPrat_model, 0x0074cefc);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gMapHUD_model, 0x0074ca78);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gStatbarRightHUD_model, 0x0074cf6c);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gTestFont_model, 0x0074cf54);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gHeadup_model, 0x0074caa0);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gHUDsquare_model, 0x0074ca3c);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gTimerRightHUD_model, 0x0074cad8);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gOffence_model, 0x0074ca64);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gTimerLeftHUD_model, 0x0074cf88);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gPower_model, 0x0074cee0);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gStatbarHUD5_model, 0x0074ca88);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gStatbarHUD3_model, 0x0074cf44);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gStatbarHUD1_model, 0x0074ca50);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gPowerupHUD_model, 0x0074cf4c);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gHeadup2_model, 0x0074cf3c);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gDim_model, 0x0074ca70);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gHUDcurve_model, 0x0074cef8);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gArmour_model, 0x0074cf08);

C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gPowerupHUD_material, 0x0074cf78);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gTestFont_material, 0x0074cf28);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gArmour_material, 0x0074ca38);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gStatbarHUD1_material, 0x0074cf14);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gStatbarHUD5_material, 0x0074c9fc);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gHeadup_material, 0x0074cf50);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gStatbarHUD3_material, 0x0074cacc);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gOffence_material, 0x0074ca9c);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gPower_material, 0x0074cf58);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gStatbarRightHUD_material, 0x0074ca8c);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gPrat_material, 0x0074ca30);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gHUDcurve_material, 0x0074caa4);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gMapHUD_material, 0x0074cf70);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gHeadup2_material, 0x0074cf1c);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gTimerRightHUD_material, 0x0074cac0);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gDim_material, 0x0074cad4);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gHUDsquare_material, 0x0074cf40);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gTimerLeftHUD_material, 0x0074cf64);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHeadup_dim_w, 0x00655e64, 0xc0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHeadup_dim_h, 0x00655e68, 0x26);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHeadup_dim_x, 0x00655e6c, 0x44);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHeadup_dim_y, 0x00655e70, 0x1b3);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHeadup2_dim_w, 0x00655e74, 0xb8);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHeadup2_dim_h, 0x00655e78, 0x17);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHeadup2_dim_x, 0x00655e7c, 0x124);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHeadup2_dim_y, 0x00655e80, 0x1c3);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPowerupHUD_dim_w, 0x00655e84, 0x40);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPowerupHUD_dim_h, 0x00655e88, 0x40);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPowerupHUD_dim_x, 0x00655e8c, 0xa);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPowerupHUD_dim_y, 0x00655e90, 0x8c);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarHUD1_dim_w, 0x00655e94, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarHUD1_dim_h, 0x00655e98, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT(int, gStatbarHUD1_dim_x, 0x0068b8f0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gStatbarHUD1_dim_y, 0x0068b8f4);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarHUD3_dim_w, 0x00655eac, 0x8e);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarHUD3_dim_h, 0x00655eb0, 0x26);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarHUD3_dim_x, 0x00655eb4, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarHUD3_dim_y, 0x00655eb8, 0xa);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHUDsquare_dim_w, 0x00655ebc, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHUDsquare_dim_h, 0x00655ec0, 0x40);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHUDsquare_dim_x, 0x00655ec4, 0x40);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHUDsquare_dim_y, 0x0068b8f8);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarHUD5_dim_w, 0x00655ec8, 0x40);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarHUD5_dim_h, 0x00655ecc, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarHUD5_dim_x, 0x00655ed0, 0x240);
C2_HOOK_VARIABLE_IMPLEMENT(int, gStatbarHUD5_dim_y, 0x0068b8fc);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarRightHUD_dim_w, 0x00655eec, 0xb2);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarRightHUD_dim_h, 0x00655ef0, 0x26);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarRightHUD_dim_x, 0x00655ef4, 0x18e);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStatbarRightHUD_dim_y, 0x00655ef8, 0xa);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gTimerLeftHUD_dim_w, 0x00655ed4, 0x40);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gTimerLeftHUD_dim_h, 0x00655ed8, 0x40);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gTimerLeftHUD_dim_x, 0x00655edc, 0x10e);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimerLeftHUD_dim_y, 0x0068b900);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gTimerRightHUD_dim_w, 0x00655ee0, 0x40);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gTimerRightHUD_dim_h, 0x00655ee4, 0x40);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gTimerRightHUD_dim_x, 0x00655ee8, 0x14e);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimerRightHUD_dim_y, 0x0068b904);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gArmour_dim_w, 0x00655f0c, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gArmour_dim_h, 0x00655f10, 0x10);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gArmour_dim_x, 0x00655f14, 0x95);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gArmour_dim_y, 0x00655f18, 0x31);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPower_dim_w, 0x00655f1c, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPower_dim_h, 0x00655f20, 0x10);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPower_dim_x, 0x00655f24, 0x12a);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPower_dim_y, 0x00655f28, 0x31);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gOffence_dim_w, 0x00655f2c, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gOffence_dim_h, 0x00655f30, 0x10);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gOffence_dim_x, 0x00655f34, 0x1c0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gOffence_dim_y, 0x00655f38, 0x31);

