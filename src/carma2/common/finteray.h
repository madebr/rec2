#ifndef REC2_FINTERAY_H
#define REC2_FINTERAY_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gPling_materials);
C2_HOOK_VARIABLE_DECLARE(int, gTemp_group);
C2_HOOK_VARIABLE_DECLARE(int, gNearest_face);
C2_HOOK_VARIABLE_DECLARE(br_model*, gNearest_model);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gNearest_actor);
C2_HOOK_VARIABLE_DECLARE(br_scalar, gNearest_T);
C2_HOOK_VARIABLE_DECLARE(int, gNearest_face_group);
C2_HOOK_VARIABLE_DECLARE(br_matrix34, gPick_model_to_view__finteray);

void C2_HOOK_FASTCALL EnablePlingMaterials(void);

void C2_HOOK_FASTCALL DisablePlingMaterials(void);

void C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt);

void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m);

int C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far);

void C2_HOOK_FASTCALL DRVector2AccumulateScale__finteray(br_vector2* a, const br_vector2* b, br_scalar s);

int C2_HOOK_FASTCALL DRModelPick2D__finteray(br_model* model, br_material* material, br_actor* actor, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_cbfn* callback, void* arg);

int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* ap, br_vector3* pPosition, br_vector3* pDir, br_model* model, br_material* material, dr_pick2d_cbfn* callback);

int C2_HOOK_CDECL FindHighestPolyCallBack__finteray(br_model* pModel, br_actor* pActor, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pData);

int C2_HOOK_CDECL FindHighestCallBack__finteray(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg);

int C2_HOOK_FASTCALL DRSceneRayPick2D(br_actor* pWorld, br_vector3* pPosition, br_vector3* pDir, dr_pick2d_cbfn* pCallback);

void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* nor, br_scalar* t, br_material** material, br_actor** actor);

void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);

void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos);

void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds);

int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2);

#endif
