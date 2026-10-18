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

extern void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds);

extern int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks);

// ModelPickBox

// ClipToPlaneGE

// ClipToPlaneLE

// BoundsOverlapTest

// BoundsTransformTest

// LineBoxColl

// SphereBoxIntersection

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
