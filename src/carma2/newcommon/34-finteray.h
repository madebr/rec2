#ifndef GUARD_34_FINTERAY_H
#define GUARD_34_FINTERAY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPling_materials;

// BadDiv

// DRVector2AccumulateScale

// PickBoundsTestRay

// ActorRayPick2D

// DRSceneRayPick2D

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// ActorFindFace

extern void C2_HOOK_FASTCALL EnablePlingMaterials(void);

extern void C2_HOOK_FASTCALL DisablePlingMaterials(void);

// StationarySphereSphereCollide

// SphereSphereCollide

// StationarySphereEdgeCollide

// PointSphereCollision

// SphereEdgeCollide

// SpherePolyFaceCollide

// SphereBoxFaceCollide

// SphereFaceCollide

// CheckSingleFace

// MultiRayCheckSingleFace

// GetNewBoundingBox

// FillInBounds

// ActorBoxPick

// ModelPickBox

// ClipToPlaneGE

// ClipToPlaneLE

// BoundsOverlapTest

// BoundsTransformTest

// LineBoxColl

extern int C2_HOOK_FASTCALL SphereBoxIntersection(const br_bounds* pBounds, const br_vector3* pCenter, br_scalar pRadius_squared, br_vector3* pClosest);

// LineBoxCollWithSphere

// LineSphereColl

// LineSphereCollWithSphere

// LinePolyColl

// SpherePolyIntersection

// LinePolyCollWithSphere

// ShapeRayCast

// ShapeRayCastWithSphere

// CastRayDownThruFaces

#endif // GUARD_34_FINTERAY_H
