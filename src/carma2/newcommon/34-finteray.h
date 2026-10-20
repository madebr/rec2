#ifndef GUARD_34_FINTERAY_H
#define GUARD_34_FINTERAY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPling_materials;
extern br_scalar gNearest_T;
extern br_actor* gNearest_actor;
extern int gNearest_face;
extern int gNearest_face_group;
extern br_model* gNearest_model;
extern br_matrix34 gPick_model_to_view__finteray;

// BadDiv

// DRVector2AccumulateScale

// PickBoundsTestRay

extern int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* pActor, br_vector3* pPosition, br_vector3* pDir, br_model* pModel, br_material* pMaterial, dr_pick2d_cbfn* pCallback);

// DRSceneRayPick2D

// DRModelPick2D

// FindHighestPolyCallBack

extern int C2_HOOK_CDECL FindHighestCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg);

extern void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* pNormal, br_scalar* pT, br_material** pMaterial, br_actor** pActor);

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
