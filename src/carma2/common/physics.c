#include "physics.h"

#include "errors.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

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
    C2_HOOK_BUG_ON(sizeof(tCollision_shape_wire_frame__line) != 2);
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

#if 0//defined(C2_HOOKS_ENABLED)
    LoadCollisionShape_original(pShapes, pF);
#else
    int i;
    int count;

    /* Number of 'Bounding shapes' entries. */
    count = GetAnInt(pF);

    for (i = 0; i < count; i++) {
        char s[256];
        tCollision_shape* shape;

        GetAString(pF, s);
        if (DRStricmp(s, "box") == 0) {

            shape = (tCollision_shape*)AllocateBoxCollisionShape(kMem_collision_shape);
            GetThreeFloats(pF,
                &shape->box.common.bb.min.v[0],
                &shape->box.common.bb.min.v[1],
                &shape->box.common.bb.min.v[2]);
            GetThreeFloats(pF,
                &shape->box.common.bb.max.v[0],
                &shape->box.common.bb.max.v[1],
                &shape->box.common.bb.max.v[2]);
        } else if (DRStricmp(s, "polyhedron") == 0) {
            int count_points;
            int j;

            /* number of points */
            count_points = GetAnInt(pF);
            if (count_points > 32) { /* FIXME: magic number */
                c2_sprintf(s, "Ow! Physics shape is mad (having %d points is silly)", count_points);
                PDFatalError(s);
            }
            shape = (tCollision_shape*)AllocatePolyhedronCollisionShape(count_points, kMem_collision_shape);

            shape->polyhedron.polyhedron.count_points = count_points;
            for (j = 0; j < count_points; j++) {
                GetThreeFloats(pF,
                    &shape->polyhedron.polyhedron.points[j].v[0],
                    &shape->polyhedron.polyhedron.points[j].v[1],
                    &shape->polyhedron.polyhedron.points[j].v[2]);
            }
        } else if (DRStricmp(s, "sphere") == 0) {

            shape = (tCollision_shape*)AllocateSphereCollisionShape(kMem_collision_shape);

            shape->sphere.sphere.radius = GetAScalar(pF);
            GetThreeFloats(pF,
                &shape->sphere.sphere.center.v[0],
                &shape->sphere.sphere.center.v[1],
                &shape->sphere.sphere.center.v[2]);
        } else if (DRStricmp(s, "wire") == 0) {
            br_vector3 points[64];
            int count_points;
            int count_lines;
            int j;

            count_points = GetAnInt(pF);
            if (count_points > REC2_ASIZE(points)) {
                FatalError(kFatalError_TooManyExtraPointsForCarIndex_S, " Too many points in wire frame shape ");
            }
            for (j = 0; j < count_points; j++) {
                GetThreeFloats(pF, &points[j].v[0], &points[j].v[1], &points[j].v[2]);
            }
            count_lines = GetAnInt(pF);
            shape = (tCollision_shape*)AllocateWireFrameCollisionShape(count_points, count_lines, kMem_collision_shape);

            for (j = 0; j < count_points; j++) {
                BrVector3Copy(&shape->wireframe.wireframe.points[j], &points[j]);
            }
            for (j = 0; j < count_lines; j++) {
                int index1, index2;

                GetPairOfInts(pF, &index1, &index2);
                shape->wireframe.wireframe.lines[j].index1 = index1;
                shape->wireframe.wireframe.lines[j].index2 = index2;
            }
        } else {
            FatalError(kFatalError_ShapeDataIsWrong);
        }
        ProcessCollisionShape(shape);
        *pShape = shape;
        pShape = &shape->common.next;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00488b00, LoadCollisionShape, LoadCollisionShape_original)

void (C2_HOOK_FASTCALL * UpdateCollisionBoundingBox_original)(tCollision_info* pCollision_info);
void C2_HOOK_FASTCALL UpdateCollisionBoundingBox(tCollision_info* pCollision_info) {

#if 0//defined(C2_HOOKS_ENABLED)
    UpdateCollisionBoundingBox_original(pCollision_info);
#else
    tCollision_shape* shape;
    tCollision_shape* current;

    shape = pCollision_info->shape;
    if (shape == NULL) {
        return;
    }
    pCollision_info->bb1 = shape->common.bb;

    for (current = shape->common.next; current != NULL; current = current->common.next) {
        int i;

        for (i = 0; i < 3; i++) {

            pCollision_info->bb1.min.v[i] = MIN(pCollision_info->bb1.min.v[i], current->common.bb.min.v[i]);
            pCollision_info->bb1.max.v[i] = MAX(pCollision_info->bb1.max.v[i], current->common.bb.max.v[i]);
        }
    }
    pCollision_info->bb2 = pCollision_info->bb1;

    pCollision_info->radius_squared = 0.f
            + MAX(REC2_SQR(pCollision_info->bb2.min.v[0]), REC2_SQR(pCollision_info->bb2.max.v[0]))
            + MAX(REC2_SQR(pCollision_info->bb2.min.v[1]), REC2_SQR(pCollision_info->bb2.max.v[1]))
            + MAX(REC2_SQR(pCollision_info->bb2.min.v[2]), REC2_SQR(pCollision_info->bb2.max.v[2]));
    pCollision_info->radius = sqrtf(pCollision_info->radius_squared);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c60a0, UpdateCollisionBoundingBox, UpdateCollisionBoundingBox_original)

void (C2_HOOK_FASTCALL * ProcessCollisionShape_original)(tCollision_shape* pShape);
void C2_HOOK_FASTCALL ProcessCollisionShape(tCollision_shape* pShape) {

#if defined(C2_HOOKS_ENABLED)
    ProcessCollisionShape_original(pShape);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c5f20, ProcessCollisionShape, ProcessCollisionShape_original)

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

