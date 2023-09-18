#include "physics.h"

#include "errors.h"

#include <brender/brender.h>

#include "c2_string.h"

#include "rec2_macros.h"

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000

C2_HOOK_VARIABLE_IMPLEMENT(tPhysicsError_cbfn*, gPhysics_error_cb, 0x006baa40);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_buffer1_ptr, 0x00744818);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_buffer2_ptr, 0x00744814);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_buffer3_ptr, 0x0074a5e0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPhysics_other_buffer_capacity, 0x006940c8);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_other_buffer, 0x006940c4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tU8, gPhysics_buffer, 299792, 0x006baa40);

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
#if 0//defined(C2_HOOKS_ENABLED)
    InitPhysics_original();
#else

    SetPhysicsErrorCallback(OnPhysicsError);
    SetPhysicsBuffer(C2V(gPhysics_buffer), sizeof(C2V(gPhysics_buffer)));

    C2_HOOK_BUG_ON(sizeof(C2V(gPhysics_buffer)) != 299792);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5ca0, InitPhysics, InitPhysics_original)

tCollision_shape_box* C2_HOOK_FASTCALL AllocateBoxCollisionShape(br_uint_8 pType) {
    tCollision_shape_box* result;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_box) != 56);
    result = BrMemAllocate(sizeof(tCollision_shape_box), pType);
    result->common.type = kCollisionShapeType_Box;
    return result;
}
C2_HOOK_FUNCTION(0x004c5e00, AllocateBoxCollisionShape)

tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateSphereCollisionShape(br_uint_8 pType) {
    tCollision_shape_sphere* result;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_sphere) != 76);
    result = BrMemAllocate(sizeof(tCollision_shape_sphere), pType);
    result->common.type = kCollisionShapeType_Sphere;
    return result;
}
C2_HOOK_FUNCTION(0x004c5d10, AllocateSphereCollisionShape)

tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocatePolyhedronCollisionShape(int pCount_points, br_uint_8 pType) {
    tCollision_shape_polyhedron* result;
    tU8* raw_memory;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_polyhedron) != 80);
    result = BrMemAllocate(sizeof(tCollision_shape_polyhedron) * pCount_points * sizeof(br_vector3) + (pCount_points + -2) * 38, pType); /* FIXME: 38? */
    raw_memory = (tU8*)result;

    raw_memory += sizeof(tCollision_shape_polyhedron);

    result->polyhedron.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);

    result->polyhedron.edges = (tU8*)raw_memory;
    raw_memory += (pCount_points - 2) * 6;

    result->polyhedron.planes = (br_vector4*)raw_memory;

    result->polyhedron.count_points = 0;
    result->polyhedron.count_edges = 0;
    result->polyhedron.count_planes = 0;
    result->common.type = kCollisionShapeType_Polyhedron;
    return result;
}
C2_HOOK_FUNCTION(0x004c5d70, AllocatePolyhedronCollisionShape)

tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateWireFrameCollisionShape(int pCount_points, int pCount_lines, br_uint_8 pType) {
    tCollision_shape_wireframe* result;
    tU8* raw_memory;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_wireframe) != 72);
    result = BrMemAllocate(sizeof(tCollision_shape_wireframe) + pCount_points * sizeof(br_vector3) + pCount_lines * sizeof(tCollision_shape_wire_frame__line), pType);
    raw_memory = (tU8*)result;

    raw_memory += sizeof(tCollision_shape_wireframe);

    result->wireframe.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);

    result->wireframe.count_lines = pCount_lines;
    result->wireframe.lines = (tCollision_shape_wire_frame__line*)raw_memory;
    result->wireframe.count_points = pCount_points;
    result->common.type = kCollisionShapeType_Wireframe;
    return result;
}
C2_HOOK_FUNCTION(0x004c5dc0, AllocateWireFrameCollisionShape)

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

