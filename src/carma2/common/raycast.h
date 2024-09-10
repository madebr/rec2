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

#endif //REC2_RAYCAST_H
