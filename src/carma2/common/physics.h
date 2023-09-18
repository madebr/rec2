#ifndef REC2_PHYSICS_H
#define REC2_PHYSICS_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(tPhysicsError_cbfn*, gPhysics_error_cb);
C2_HOOK_VARIABLE_DECLARE(void*, gPhysics_buffer1_ptr);
C2_HOOK_VARIABLE_DECLARE(void*, gPhysics_buffer2_ptr);
C2_HOOK_VARIABLE_DECLARE(void*, gPhysics_buffer3_ptr);
C2_HOOK_VARIABLE_DECLARE(int, gPhysics_other_buffer_capacity);
C2_HOOK_VARIABLE_DECLARE(void*, gPhysics_other_buffer);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tU8, gPhysics_buffer, 299792);

void C2_HOOK_FASTCALL OnPhysicsError(tPhysicsError pError);

void C2_HOOK_FASTCALL SetPhysicsErrorCallback(tPhysicsError_cbfn *pError_cbfn);

void C2_HOOK_FASTCALL SetPhysicsBuffer(tU8* pBuffer, int pSize);

void C2_HOOK_FASTCALL InitPhysics(void);

void C2_HOOK_FASTCALL LoadCollisionShape(tCollision_shape** pShape, FILE* pF);

void C2_HOOK_FASTCALL UpdateCollisionBoundingBox(tCollision_info* pCollision_info);

tCollision_info* C2_HOOK_FAKE_THISCALL CreateSphericalCollisionObject(br_model* pModel, undefined4 pArg2, float pWeight);

#endif //REC2_PHYSICS_H
