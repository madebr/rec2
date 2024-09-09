#ifndef FRONTEND_WRECKS_H
#define FRONTEND_WRECKS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_WRECKS);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tWreck_gallery_sell_info, gWreck_gallery_sell_infos, 32);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tWreck_gallery_car_info, gWreck_gallery_car_infos, 30);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gFrontend_wrecks_light);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_wrecks_car_count);
C2_HOOK_VARIABLE_DECLARE(float, gFrontend_wreck_bought_car_dz);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_wrecks_current);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_wrecks_hscroll);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_wrecks_pending_hscroll);
C2_HOOK_VARIABLE_DECLARE(tU32, gFrontend_wrecks_previous_update);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_wrecks_rotate_prev_x);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_wrecks_rotate_prev_y);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_opponent_profile_pic_needs_update);


int C2_HOOK_FASTCALL WrecksInFunc(tFrontend_spec* pFrontend);

#endif /* FRONTEND_WRECKS_H */
