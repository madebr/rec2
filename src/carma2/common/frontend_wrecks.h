#ifndef FRONTEND_WRECKS_H
#define FRONTEND_WRECKS_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_WRECKS;
extern tWreck_gallery_sell_info gWreck_gallery_sell_infos[32];
extern tWreck_gallery_car_info gWreck_gallery_car_infos[30];
extern br_actor* gFrontend_wrecks_light;
extern int gFrontend_wrecks_car_count;
extern float gFrontend_wreck_bought_car_dz;
extern int gFrontend_wrecks_current;
extern int gFrontend_wrecks_hscroll;
extern int gFrontend_wrecks_pending_hscroll;
extern tU32 gFrontend_wrecks_previous_update;
extern int gFrontend_wrecks_rotate_prev_x;
extern int gFrontend_wrecks_rotate_prev_y;
extern int gFrontend_opponent_profile_pic_needs_update;
extern int gHierarchy_has_actor;
extern int gPicked_wreck;


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

int C2_HOOK_FASTCALL ScrollToNextCar(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL BuyCurrentCar(tFrontend_spec* pFrontend);

#endif /* FRONTEND_WRECKS_H */