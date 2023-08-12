#include "physics.h"

#include "rec2_macros.h"

void (C2_HOOK_FASTCALL * InitPhysics_original)(void);
void C2_HOOK_FASTCALL InitPhysics(void) {

    C2_HOOK_ASSUME_UNUSED();
#if defined(C2_HOOKS_ENABLED)
    InitPhysics_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5ca0, InitPhysics, InitPhysics_original)

void (C2_HOOK_FASTCALL * LoadCollisionShape_original)(tCollision_shape** pShape, FILE* pF);
void C2_HOOK_FASTCALL LoadCollisionShape(tCollision_shape** pShape, FILE* pF) {

#if defined(C2_HOOKS_ENABLED)
    LoadCollisionShape_original(pShape, pF);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00488b00, LoadCollisionShape, LoadCollisionShape_original)

void (C2_HOOK_FASTCALL * UpdateCollisionBoundingBox_original)(tCollision_info* pCollision_info);
void C2_HOOK_FASTCALL UpdateCollisionBoundingBox(tCollision_info* pCollision_info) {

#if defined(C2_HOOKS_ENABLED)
    UpdateCollisionBoundingBox_original(pCollision_info);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c60a0, UpdateCollisionBoundingBox, UpdateCollisionBoundingBox_original)

tCollision_info* (C2_HOOK_FAKE_THISCALL * CreateSphericalCollisionObject_original)(br_model* pModel, float pWeight);
tCollision_info* C2_HOOK_FAKE_THISCALL CreateSphericalCollisionObject(br_model* pModel, undefined4 pArg2, float pWeight) {

REC2_THISCALL_UNUSED(pArg2);

#if defined(C2_HOOKS_ENABLED)
    return CreateSphericalCollisionObject_original(pModel, pWeight);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004da400, CreateSphericalCollisionObject, CreateSphericalCollisionObject_original)

