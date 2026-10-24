#ifndef GUARD_29_RAYCAST_H
#define GUARD_29_RAYCAST_H

#include "rec2_types.h"

extern br_actor* gY_picking_camera;

extern br_matrix34 gPick_model_to_view__raycast;

// DRActorToRoot

extern void C2_HOOK_FASTCALL InitRayCasting(void);

// BadDiv

// DRVector2AccumulateScale

extern int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds3* pBounds, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, br_scalar* pNew_t_near, br_scalar* pNew_t_far);

extern int C2_HOOK_FASTCALL ActorPick2D(br_actor* pActor, br_model* pModel, br_material* pMaterial, dr_pick2d_cbfn* pCallback, void* pArg);

// DRScenePick2DXY

// DRScenePick2D

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// FindBestY

// FindYVerticallyBelowPolyCallBack

// FindYVerticallyBelowCallBack

extern br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow(br_vector3* pPosition);

extern br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point);

#endif // GUARD_29_RAYCAST_H
