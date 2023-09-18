#include "physics.h"

#include "errors.h"

#include "c2_string.h"

#include "rec2_macros.h"

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000

C2_HOOK_VARIABLE_IMPLEMENT(tPhysicsError_cbfn*, gPhysics_error_cb, 0x006940c0);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_buffer1_ptr, 0x00744818);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_buffer2_ptr, 0x00744814);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_buffer3_ptr, 0x0074a5e0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPhysics_other_buffer_capacity, 0x006940c8);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_other_buffer, 0x006baa40);

void C2_HOOK_FASTCALL OnPhysicsError(tPhysicsError pError) {
    char s[256];

    switch(pError) {
    case ePhysicsError_UnknownShapeType:
        c2_strcpy(s, "Unknown shape type");
        break;
    case ePhysicsError_WireFrameHasNoPoints:
        c2_strcpy(s, "Wire frame shape has no points");
        break;
    case ePhysicsError_PolyhedronHasNoPoints:
        c2_strcpy(s, "Polyhedron shape has no points");
        break;
    case ePhysicsError_UnknownHingeType:
        c2_strcpy(s, "Unknown hinge type");
        break;
    default:
        sprintf(s, "%d", pError);
        break;
    }
    FatalError(1000 + pError, s);
}
C2_HOOK_FUNCTION(0x004b5990, OnPhysicsError)

void C2_HOOK_FASTCALL SetPhysicsErrorCallback(tPhysicsError_cbfn *pError_cbfn) {

    C2V(gPhysics_error_cb) = pError_cbfn;
}
C2_HOOK_FUNCTION(0x004c61f0, SetPhysicsErrorCallback)

void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError) {

    if (C2V(gPhysics_error_cb) != NULL) {
        C2V(gPhysics_error_cb)(pError);
    }

    c2_exit(pError);
}

void C2_HOOK_FASTCALL SetPhysicsBuffer(tU8* pBuffer, int pSize) {

    C2V(gPhysics_buffer1_ptr) = pBuffer + 0 * PHYSICS_BUFFER_PART_SIZE;
    C2V(gPhysics_buffer2_ptr) = pBuffer + 1 * PHYSICS_BUFFER_PART_SIZE;
    C2V(gPhysics_buffer3_ptr) = pBuffer + 2 * PHYSICS_BUFFER_PART_SIZE;

    if (pSize - 3 * PHYSICS_BUFFER_PART_SIZE < PHYSICS_BUFFER_OTHER_SIZE) {
        PhysicsError(ePhysicsError_InsufficientSizedBuffer);
    }
    C2V(gPhysics_other_buffer_capacity) = pSize - 3 * PHYSICS_BUFFER_PART_SIZE;
    C2V(gPhysics_other_buffer) = pBuffer + 3 * PHYSICS_BUFFER_PART_SIZE;
}
C2_HOOK_FUNCTION(0x004c6510, SetPhysicsBuffer)

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

