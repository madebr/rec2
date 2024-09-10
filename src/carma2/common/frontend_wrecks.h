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
C2_HOOK_VARIABLE_DECLARE(int, gHierarchy_has_actor);
C2_HOOK_VARIABLE_DECLARE(int, gPicked_wreck);


int C2_HOOK_FASTCALL WrecksInFunc(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL DisposeWrecksGallery(void);

int C2_HOOK_FASTCALL WrecksOutFunc(tFrontend_spec* pFrontend);

int C2_HOOK_CDECL HeirarchyPick(br_actor* a, void* ref);

int C2_HOOK_CDECL WreckPick(br_actor* world, br_model* model, br_material* material, br_vector3* pos, br_vector3* dir, br_scalar near, br_scalar far, void* arg);

int C2_HOOK_FASTCALL KeepInRange(int pIndex, int pRange);

int C2_HOOK_FASTCALL GotItAlready(tCar_spec* pCar);

int C2_HOOK_FASTCALL GetCarSelectedByMouse(int pX, int pY);

int C2_HOOK_FASTCALL WrecksUpdateFunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL ScrollToPrevCar(tFrontend_spec* pFrontend);

#endif /* FRONTEND_WRECKS_H */
