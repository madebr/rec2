#ifndef GUARD_34_FINTERAY_H
#define GUARD_34_FINTERAY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPling_materials;

// BadDiv

// DRVector2AccumulateScale

extern int C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far);

extern int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* ap, br_vector3* pPosition, br_vector3* pDir, br_model* model, br_material* material, dr_pick2d_cbfn* callback);

// DRSceneRayPick2D

extern int C2_HOOK_FASTCALL DRModelPick2D__finteray(br_model* model, br_material* material, br_actor* actor, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_cbfn* callback, void* arg);

extern int C2_HOOK_CDECL FindHighestPolyCallBack__finteray(br_model* pModel, br_actor* pActor, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pData);

extern int C2_HOOK_CDECL FindHighestCallBack__finteray(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg);

extern void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* nor, br_scalar* t, br_material** material, br_actor** actor);

extern int gTemp_group;
extern int gNearest_face;
extern br_model* gNearest_model;
extern br_actor* gNearest_actor;
extern br_scalar gNearest_T;
extern int gNearest_face_group;
extern br_matrix34 gPick_model_to_view__finteray;

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

extern void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m);

extern void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds);

// ActorBoxPick

// ModelPickBox

// ClipToPlaneGE

// ClipToPlaneLE

extern int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2);

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
