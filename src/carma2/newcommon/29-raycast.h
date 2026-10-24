#ifndef GUARD_29_RAYCAST_H
#define GUARD_29_RAYCAST_H

#include "rec2_types.h"

extern br_actor* gY_picking_camera;

// DRActorToRoot

extern void C2_HOOK_FASTCALL InitRayCasting(void);

// BadDiv

// DRVector2AccumulateScale

// PickBoundsTestRay

// ActorPick2D

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
