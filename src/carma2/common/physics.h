#ifndef REC2_PHYSICS_H
#define REC2_PHYSICS_H

#include "rec2_types.h"

#include "c2_hooks.h"

void C2_HOOK_FASTCALL OnPhysicsError(tPhysicsError pError);

void C2_HOOK_FASTCALL InitPhysics(void);

void C2_HOOK_FASTCALL LoadCollisionShape(tCollision_shape** pShape, FILE* pF);

void C2_HOOK_FASTCALL UpdateCollisionBoundingBox(tCollision_info* pCollision_info);

tCollision_info* C2_HOOK_FAKE_THISCALL CreateSphericalCollisionObject(br_model* pModel, undefined4 pArg2, float pWeight);

#endif //REC2_PHYSICS_H
