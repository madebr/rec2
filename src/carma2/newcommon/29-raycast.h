#ifndef GUARD_29_RAYCAST_H
#define GUARD_29_RAYCAST_H

#include "rec2_types.h"

extern br_actor* gY_picking_camera;

extern br_matrix34 gPick_model_to_view__raycast;

// DRActorToRoot

extern void C2_HOOK_FASTCALL InitRayCasting(void);

// BadDiv

// DRVector2AccumulateScale

extern int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far);

extern int C2_HOOK_FASTCALL ActorPick2D(br_actor* pActor, br_model* pModel, br_material* pMaterial, dr_pick2d_cbfn* pCallback, void* pArg);

// DRScenePick2DXY

// DRScenePick2D

extern int C2_HOOK_FASTCALL DRModelPick2D_raycast(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, dr_modelpick2d_raycast_cbfn* pCallback, void* pArg);

// FindHighestPolyCallBack

// FindHighestCallBack

// FindBestY

extern int C2_HOOK_CDECL FindYVerticallyBelowPolyCallBack(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pContext);

extern int C2_HOOK_CDECL FindYVerticallyBelowCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT_near, float pT_far, void* pArg);

extern br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow(br_vector3* pPosition);

extern br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point);

#endif // GUARD_29_RAYCAST_H
