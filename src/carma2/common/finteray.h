#ifndef REC2_FINTERAY_H
#define REC2_FINTERAY_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gPling_materials);
C2_HOOK_VARIABLE_DECLARE(int, gTemp_group);
C2_HOOK_VARIABLE_DECLARE(br_scalar, gNearest_T);

void C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt);

void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m);

int C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far);

void C2_HOOK_FASTCALL DRVector2AccumulateScale__finteray(br_vector2* a, const br_vector2* b, br_scalar s);

int C2_HOOK_FASTCALL DRModelPick2D__finteray(br_model* model, br_material* material, br_actor* actor, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_cbfn* callback, void* arg);

int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* ap, br_vector3* pPosition, br_vector3* pDir, br_model* model, br_material* material, dr_pick2d_cbfn* callback);

#endif
