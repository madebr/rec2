#ifndef REC2_GLOBVRBM_H
#define REC2_GLOBVRBM_H

#include "c2_hooks.h"

#include <brender/br_types.h>

C2_HOOK_VARIABLE_DECLARE(int, gCar_flying);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gPrat_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, g2d_camera);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gPowerupHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gStatbarHUD1_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHeadup2_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHUDsquare_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gMapHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gTimerRightHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gOffense_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gTimerLeftHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gPower_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHUDcurve_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gStatbarRightHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHeadup_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gStatbarHUD5_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gDim_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gArmour_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHUD_root_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gStatbarHUD3_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gTestFont_actor);

C2_HOOK_VARIABLE_DECLARE(br_model*, gPrat_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gMapHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gStatbarRightHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gTestFont_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gHeadup_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gHUDsquare_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gTimerRightHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gOffence_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gTimerLeftHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gPower_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gStatbarHUD5_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gStatbarHUD3_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gStatbarHUD1_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gPowerupHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gHeadup2_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gDim_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gHUDcurve_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gArmour_model);

C2_HOOK_VARIABLE_DECLARE(br_material*, gPowerupHUD_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gTestFont_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gArmour_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gStatbarHUD1_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gStatbarHUD5_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gHeadup_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gStatbarHUD3_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gOffence_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gPower_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gStatbarRightHUD_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gPrat_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gHUDcurve_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMapHUD_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gHeadup2_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gTimerRightHUD_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gDim_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gHUDsquare_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gTimerLeftHUD_material);

C2_HOOK_VARIABLE_DECLARE(int, gHeadup_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gHeadup2_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup2_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup2_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup2_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gPowerupHUD_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gPowerupHUD_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gPowerupHUD_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gPowerupHUD_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD1_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD1_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD1_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD1_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD3_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD3_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD3_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD3_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gHUDsquare_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gHUDsquare_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gHUDsquare_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gHUDsquare_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD5_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD5_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD5_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD5_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gStatbarRightHUD_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarRightHUD_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarRightHUD_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarRightHUD_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gTimerLeftHUD_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gTimerLeftHUD_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gTimerLeftHUD_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gTimerLeftHUD_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gArmour_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gArmour_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gArmour_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gArmour_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gPower_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gPower_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gPower_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gPower_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gOffence_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gOffence_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gOffence_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gOffence_dim_y);

#endif //REC2_GLOBVRBM_H
