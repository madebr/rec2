#ifndef REC2_RAYCAST_H
#define REC2_RAYCAST_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(br_actor*, gY_picking_camera);

void C2_HOOK_FASTCALL InitRayCasting(void);

int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far);

int C2_HOOK_FASTCALL ActorPick2D(br_actor* ap, br_model* model, br_material* material, dr_pick2d_cbfn* callback, void* arg);

int C2_HOOK_FASTCALL DRActorToRoot(br_actor* a, br_actor* world, br_matrix34* m);

int C2_HOOK_FASTCALL DRScenePick2DXY(br_actor* world, br_actor* camera, br_pixelmap* viewport, int pick_x, int pick_y, dr_pick2d_cbfn* callback, void* arg);

int C2_HOOK_CDECL FindYVerticallyBelowPolyCallBack(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pContext);

int C2_HOOK_FASTCALL BadDiv_raycast(br_scalar a, br_scalar b);

void C2_HOOK_FASTCALL DRVector2AccumulateScale_raycast(br_vector2* a, br_vector2* b, br_scalar s);

int C2_HOOK_FASTCALL DRModelPick2D_raycast(br_model* model, br_material* material, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_raycast_cbfn* callback, void* arg);

int C2_HOOK_CDECL FindYVerticallyBelowCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT_near, float pT_far, void* pArg);

br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point);

#endif //REC2_RAYCAST_H
