#include "physics.h"

#include "car.h"
#include "errors.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

#include <brender/brender.h>

#include <stdarg.h>
#include "c2_stdlib.h"
#include "c2_string.h"

#include "rec2_macros.h"

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000

C2_HOOK_VARIABLE_IMPLEMENT(tPhysicsError_cbfn*, gPhysics_error_cb, 0x006940c0);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_buffer1_ptr, 0x00744818);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_buffer2_ptr, 0x00744814);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_buffer3_ptr, 0x0074a5e0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPhysics_other_buffer_capacity, 0x006940c8);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gPhysics_other_buffer, 0x006940c4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tU8, gPhysics_buffer, 299792, 0x006baa40);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCollision_info_uid_counter, 0x006a0adc);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPHIL_enabled, 0x0065d004, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gFace_num__car, 0x0065d010, 1);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gPhysics_reference_normal_comparison, 0x00679420);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_shape_polyhedron_data*, gPolyhedron_to_sort, 0x0067942c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPhil_object_info_00692458, gPhil_objects_00692458, 100, 0x00692458);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPHIL_count_list_collision_infos, 0x006923e0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPHIL_count_queued_objects, 0x00692dc0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPHIL_count_queued_removed_objects, 0x00692dbc);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_info*, gPHIL_list_collision_infos, 0x0074a5f0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPHIL_munging_objects, 0x0074a5e8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPHIL_doing_physics, 0x0074a5f8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPHIL_object_added, 0x006923e4);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gPHIL_last_physics_tick, 0x0074a5ec);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPHIL_mechanics_time_sync, 0x0074a5e4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPrepared_objects, 0x00692dcc);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_info**, gReduced_object_list, 0x006940a8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006940ac, 0x006940ac);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tQueued_object_info, gPHIL_queued_objects, 1, 0x006923f0);


void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError) {
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
C2_HOOK_FUNCTION(0x004b5990, DoPhysicsError)

void C2_HOOK_FASTCALL SetUpTestObjects(void) {

}
C2_HOOK_FUNCTION(0x004b5aa0, SetUpTestObjects)

void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn *pError_cbfn) {

    C2V(gPhysics_error_cb) = pError_cbfn;
}
C2_HOOK_FUNCTION(0x004c61f0, PhysicsSetErrorHandler)

void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, undefined4 pArg2) {

    if (C2V(gPhysics_error_cb) != NULL) {
        C2V(gPhysics_error_cb)(pError);
    }

    c2_exit(pError);
}

void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize) {

    C2V(gPhysics_buffer1_ptr) = pBuffer + 0 * PHYSICS_BUFFER_PART_SIZE;
    C2V(gPhysics_buffer2_ptr) = pBuffer + 1 * PHYSICS_BUFFER_PART_SIZE;
    C2V(gPhysics_buffer3_ptr) = pBuffer + 2 * PHYSICS_BUFFER_PART_SIZE;

    if (pSize - 3 * PHYSICS_BUFFER_PART_SIZE < PHYSICS_BUFFER_OTHER_SIZE) {
        PhysicsError(ePhysicsError_InsufficientSizedBuffer, 0);
    }
    C2V(gPhysics_other_buffer_capacity) = pSize - 3 * PHYSICS_BUFFER_PART_SIZE;
    C2V(gPhysics_other_buffer) = pBuffer + 3 * PHYSICS_BUFFER_PART_SIZE;
}
C2_HOOK_FUNCTION(0x004c6510, InitPhysicsWorkspace)

void (C2_HOOK_FASTCALL * InitPhysics_original)(void);
void C2_HOOK_FASTCALL InitPhysics(void) {

    C2_HOOK_ASSUME_UNUSED();
#if 0//defined(C2_HOOKS_ENABLED)
    InitPhysics_original();
#else

    PhysicsSetErrorHandler(DoPhysicsError);
    InitPhysicsWorkspace(C2V(gPhysics_buffer), sizeof(C2V(gPhysics_buffer)));

    C2_HOOK_BUG_ON(sizeof(C2V(gPhysics_buffer)) != 299792);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5ca0, InitPhysics, InitPhysics_original)

int (C2_HOOK_FASTCALL * PHILInit_original)(void);
int C2_HOOK_FASTCALL PHILInit(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return PHILInit_original();
#else
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gPhil_objects_00692458)) != 100);
    C2_HOOK_BUG_ON(sizeof(tPhil_object_info_00692458) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhil_object_info_00692458, collision_info, 0x0);

    for (i = 0; i < REC2_ASIZE(C2V(gPhil_objects_00692458)); i++) {
        C2V(gPhil_objects_00692458)[i].collision_info = NULL;
    }
    C2V(gPHIL_count_list_collision_infos) = 0;
    C2V(gPHIL_count_queued_objects) = 0;
    C2V(gPHIL_count_queued_removed_objects) = 0;
    C2V(gPHIL_list_collision_infos) = NULL;
    C2V(gPHIL_munging_objects) = 0;
    C2V(gPHIL_doing_physics) = 0;
    C2V(gPHIL_object_added) = 0;
    C2V(gPHIL_last_physics_tick) = 0;
    C2V(gPHIL_mechanics_time_sync) = 1;
    C2V(gPHIL_enabled) = 0;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5cc0, PHILInit, PHILInit_original)

void C2_HOOK_FASTCALL PHILDisable(void) {
    C2V(gPHIL_enabled) = 1;
}
C2_HOOK_FUNCTION(0x004b5d20, PHILDisable)

tCollision_shape_box* C2_HOOK_FASTCALL AllocateBoxCollisionShape(br_uint_8 pType) {
    tCollision_shape_box* result;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_box) != 56);
    result = BrMemAllocate(sizeof(tCollision_shape_box), pType);
    result->common.type = kCollisionShapeType_Box;
    return result;
}
C2_HOOK_FUNCTION(0x004c5e00, AllocateBoxCollisionShape)

tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(br_uint_8 pType) {
    tCollision_shape_sphere* result;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_sphere) != 76);
    result = BrMemAllocate(sizeof(tCollision_shape_sphere), pType);
    result->common.type = kCollisionShapeType_Sphere;
    return result;
}
C2_HOOK_FUNCTION(0x004c5d10, AllocateShapeSphere)

tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, br_uint_8 pType) {
    tCollision_shape_polyhedron* result;
    tU8* raw_memory;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_polyhedron) != 80);
    C2_HOOK_BUG_ON(sizeof(tPolyhedron_edge_indexes) != 2);
    result = BrMemAllocate(sizeof(tCollision_shape_polyhedron) * pCount_points * sizeof(br_vector3) + (pCount_points + -2) * 38, pType); /* FIXME: 38? */
    raw_memory = (tU8*)result;

    raw_memory += sizeof(tCollision_shape_polyhedron);

    result->polyhedron.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);

    result->polyhedron.edges = (tPolyhedron_edge_indexes*)raw_memory;
    raw_memory += (pCount_points - 2) * 3 * sizeof(tPolyhedron_edge_indexes);

    result->polyhedron.planes = (br_vector4*)raw_memory;

    result->polyhedron.count_points = 0;
    result->polyhedron.count_edges = 0;
    result->polyhedron.count_planes = 0;
    result->common.type = kCollisionShapeType_Polyhedron;
    return result;
}
C2_HOOK_FUNCTION(0x004c5d70, AllocateShapePolyhedron)

tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateWireFrameCollisionShape(int pCount_points, int pCount_lines, br_uint_8 pType) {
    tCollision_shape_wireframe* result;
    tU8* raw_memory;

    C2_HOOK_BUG_ON(sizeof(tCollision_shape_wireframe) != 72);
    C2_HOOK_BUG_ON(sizeof(tPolyhedron_edge_indexes) != 2);
    result = BrMemAllocate(sizeof(tCollision_shape_wireframe) + pCount_points * sizeof(br_vector3) + pCount_lines * sizeof(tPolyhedron_edge_indexes), pType);
    raw_memory = (tU8*)result;

    raw_memory += sizeof(tCollision_shape_wireframe);

    result->wireframe.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);

    result->wireframe.count_lines = pCount_lines;
    result->wireframe.lines = (tPolyhedron_edge_indexes*)raw_memory;
    result->wireframe.count_points = pCount_points;
    result->common.type = kCollisionShapeType_Wireframe;
    return result;
}
C2_HOOK_FUNCTION(0x004c5dc0, AllocateWireFrameCollisionShape)

void (C2_HOOK_FASTCALL * LoadCollisionShape_original)(tCollision_shape** pShape, FILE* pF);
void C2_HOOK_FASTCALL ReadMechanicsShapes(tCollision_shape** pShape, FILE* pF) {

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
            shape = (tCollision_shape*)AllocateShapePolyhedron(count_points, kMem_collision_shape);

            shape->polyhedron.polyhedron.count_points = count_points;
            for (j = 0; j < count_points; j++) {
                GetThreeFloats(pF,
                    &shape->polyhedron.polyhedron.points[j].v[0],
                    &shape->polyhedron.polyhedron.points[j].v[1],
                    &shape->polyhedron.polyhedron.points[j].v[2]);
            }
        } else if (DRStricmp(s, "sphere") == 0) {

            shape = (tCollision_shape*)AllocateShapeSphere(kMem_collision_shape);

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
        FillInShape(shape);
        *pShape = shape;
        pShape = &shape->common.next;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00488b00, ReadMechanicsShapes, LoadCollisionShape_original)

void (C2_HOOK_FASTCALL * UpdateCollisionObject_original)(tCollision_info* pCollision_info);
void C2_HOOK_FASTCALL UpdateCollisionObject(tCollision_info* pCollision_info) {

#if 0//defined(C2_HOOKS_ENABLED)
    UpdateCollisionObject_original(pCollision_info);
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
C2_HOOK_FUNCTION_ORIGINAL(0x004c60a0, UpdateCollisionObject, UpdateCollisionObject_original)

int C2_HOOK_FASTCALL ArePointsColinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3) {
    br_scalar dx31, dx21;
    br_scalar dy31, dy21;
    br_scalar dz31, dz21;

    dy21 = pV2->v[1] - pV1->v[1];
    dz21 = pV2->v[2] - pV1->v[2];
    dy31 = pV3->v[1] - pV1->v[1];
    dz31 = pV3->v[2] - pV1->v[2];
    if (fabsf(dz31 * dy21 - dy31 - dz21) >= 1e-6f) {
        return 0;
    }
    dx21 = pV2->v[0] - pV1->v[0];
    dx31 = pV3->v[0] - pV1->v[0];
    if (fabsf(dx31 * dz21 - dx21 * dz31) >= 1e-6f) {
        return 0;
    }
    if (fabsf(dx21 * dy31 - dx31 * dy21) >= 1e-6f) {
        return 0;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x00420ef0, ArePointsColinear)

static br_scalar inline C2_HOOK_FASTCALL PlaneValue(const br_vector4* pPlane, const br_vector3* pP) {

    return BrVector3Dot(pPlane, pP) + pPlane->v[3];
}

tPhysicsError C2_HOOK_FASTCALL ProcessTetrahedronPolyhedronCollisionShape(tCollision_shape_polyhedron_data* pPolyhedron, tPolyhedron_edge_indexes* pEdges) {
    int first_non_colinear;
    int i;
    br_vector3 tv2;
    br_vector3 tv1;

    first_non_colinear = 0;
    while (ArePointsColinear(&pPolyhedron->points[first_non_colinear + 0], &pPolyhedron->points[first_non_colinear + 1], &pPolyhedron->points[first_non_colinear + 2])) {
        first_non_colinear++;
        if (first_non_colinear == pPolyhedron->count_points - 3) {
            return ePhysicsError_UnknownShapeType;
        }
    }

    if (first_non_colinear != 0) {
        BrVector3Copy(&pPolyhedron->points[0], &pPolyhedron->points[first_non_colinear + 0]);
        BrVector3Copy(&pPolyhedron->points[1], &pPolyhedron->points[first_non_colinear + 1]);
        BrVector3Copy(&pPolyhedron->points[2], &pPolyhedron->points[first_non_colinear + 2]);
    }
    pPolyhedron->edges[0].index1 = 0;
    pPolyhedron->edges[0].index2 = 1;
    pPolyhedron->edges[1].index1 = 1;
    pPolyhedron->edges[1].index2 = 2;
    pPolyhedron->edges[2].index1 = 0;
    pPolyhedron->edges[2].index2 = 2;

    /* PlaneEquation */
    BrVector3Sub(&tv1, &pPolyhedron->points[2], &pPolyhedron->points[1]);
    BrVector3Sub(&tv2, &pPolyhedron->points[1], &pPolyhedron->points[0]);
    BrVector3Cross(&pPolyhedron->planes[0], &tv2, &tv1);
    BrVector3Normalise(&pPolyhedron->planes[0], &pPolyhedron->planes[0]);
    pPolyhedron->planes[0].v[3] = -BrVector3Dot(&pPolyhedron->planes[0], &pPolyhedron->points[0]);

    pEdges[2].index1 = 0;
    pEdges[1].index1 = 0;
    pEdges[0].index1 = 0;

    for (i = 0; i < pPolyhedron->count_points; i++) {

        if (fabsf(PlaneValue(&pPolyhedron->planes[0], &pPolyhedron->points[i])) > 1e-6f) {
            break;
        }
    }
    if (i == pPolyhedron->count_points) {
        return 7;
    }
    BrVector3Copy(&pPolyhedron->points[3], &pPolyhedron->points[i]);
    /* Points inside the polyhedron must have positive plane value */
    if (PlaneValue(&pPolyhedron->planes[0], &pPolyhedron->points[i]) < 1e-6f) {
        BrVector4Negate(&pPolyhedron->planes[0], &pPolyhedron->planes[0]);
    }
    for (i = 0; i < 3; i++) {
        int o;
        int o_i;

        o = i + 1;
        o_i = o % 3;

        pPolyhedron->edges[3 + i].index1 = i;
        pPolyhedron->edges[3 + i].index2 = 3;

        /* PlaneEquation */
        BrVector3Sub(&tv1, &pPolyhedron->points[3], &pPolyhedron->points[i]);
        BrVector3Sub(&tv2, &pPolyhedron->points[o_i], &pPolyhedron->points[i]);
        BrVector3Cross(&pPolyhedron->planes[o], &tv1, &tv2);
        BrVector3Normalise(&pPolyhedron->planes[o], &pPolyhedron->planes[o]);
        pPolyhedron->planes[o].v[3] = -BrVector3Dot(&pPolyhedron->planes[o], &pPolyhedron->points[3]);

        pEdges[i].index2 = o;
        pEdges[3 + i].index1 = o;
        pEdges[3 + i].index2 = 1 + (o + 1) % 3;
    }

    for (i = 0; i < 3; i++) {

        if (PlaneValue(&pPolyhedron->planes[1 + i], &pPolyhedron->points[(2 + i) % 3]) < 1e-6f) {
            BrVector4Negate(&pPolyhedron->planes[1 + i], &pPolyhedron->planes[1 + i]);
        }
    }

    pPolyhedron->count_points = 4;
    pPolyhedron->count_planes = 4;
    pPolyhedron->count_edges = 6;
    return ePhysicsError_Ok;
}
C2_HOOK_FUNCTION(0x004211f0, ProcessTetrahedronPolyhedronCollisionShape)

typedef enum {
    ePlaneSide_positive = 0,
    ePlaneSide_negative = 1,
    ePlaneSide_zero = 2,
} tPlane_side;

static tPlane_side calculate_plane_side(br_vector4* pPlane, br_vector3* pPoint) {
    br_scalar s;


    s = PlaneValue(pPlane, pPoint);
    if (s < -1e-6f) {
        return ePlaneSide_negative;
    } else if (s >= 1e-6f) {
        return ePlaneSide_positive;
    } else {
        return ePlaneSide_zero;
    }
}

int C2_HOOK_FASTCALL PolyhedronCollisionShape_AddPoint(tCollision_shape_polyhedron_data* pPolyhedron, br_vector3* pVertex, tU8 pVertex_index, tPolyhedron_edge_indexes* pEdge_to_plane_indices) {
    tPlane_side plane_side_buffer[596];
    tPolyhedron_edge_indexes new_edge_to_plane_indices[894];
    tPolyhedron_edge_indexes new_edges[894];
    br_vector4 new_planes[596];
    int positive_side_edges[596];
    int all_positive;
    int i;
    int j;
    int count_new_planes;
    int count_new_edges;
    int final_count_edges;
    int final_count_planes;

    all_positive = 1;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        plane_side_buffer[i] = calculate_plane_side(&pPolyhedron->planes[i], pVertex);
        all_positive &= plane_side_buffer[i] != ePlaneSide_negative;
    }
    /* If all positive ==> point is inside polyhedron */
    if (all_positive) {
        return 0;
    }

    count_new_planes = 0;
    count_new_edges = 0;

    for (i = 0; i < pPolyhedron->count_edges; i++) {
        tU8 plane1;
        tPlane_side side1;
        tU8 plane2;
        tPlane_side side2;

        plane1 = pEdge_to_plane_indices[i].index1;
        side1 = plane_side_buffer[plane1];
        plane2 = pEdge_to_plane_indices[i].index2;
        side2 = plane_side_buffer[plane2];
        positive_side_edges[i] = side1 == ePlaneSide_positive || side2 == ePlaneSide_positive;
        if ((side1 ^ side2) & ePlaneSide_negative) {
            /* Exactly one plane of edge has vertex on negative side */
            if (!(side1 & ePlaneSide_zero) && !(side2 & ePlaneSide_zero)) {
                br_vector3* v1,* v2;
                br_vector3 tv1, tv2;
                br_vector4 *new_plane;
                /* vertex does not lie on edge */
                v1 = &pPolyhedron->points[pPolyhedron->edges[i].index1];
                v2 = &pPolyhedron->points[pPolyhedron->edges[i].index2];
                BrVector3Sub(&tv1, pVertex, v1);
                BrVector3Sub(&tv2, v2, v1);
                new_plane = &new_planes[count_new_planes];
                BrVector3Cross(new_plane, &tv1, &tv2);
                BrVector3Normalise(new_plane, new_plane);
                if (BrVector3Dot(&pPolyhedron->planes[plane1], &pPolyhedron->planes[plane1]) * BrVector3Dot(new_plane, &pPolyhedron->planes[plane2])
                        - BrVector3Dot(&pPolyhedron->planes[plane1], &pPolyhedron->planes[plane2]) * BrVector3Dot(new_plane, &pPolyhedron->planes[plane1]) < 0.f) {
                    BrVector4Negate(new_plane, new_plane);
                }
                new_plane->v[3] = -BrVector3Dot(new_plane, pVertex);
                if (side1 == ePlaneSide_positive) {
                    pEdge_to_plane_indices[i].index2 = count_new_planes + pPolyhedron->count_planes;
                } else {
                    pEdge_to_plane_indices[i].index1 = count_new_planes + pPolyhedron->count_planes;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index1 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = count_new_planes + pPolyhedron->count_planes;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index1;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = count_new_planes + pPolyhedron->count_planes;
                    count_new_edges += 1;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index2 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = count_new_planes + pPolyhedron->count_planes;
                        count_new_planes += 1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index2;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = count_new_planes + pPolyhedron->count_planes;
                    count_new_edges += 1;
                    count_new_planes += 1;
                }
            } else {
                if (!(side1 & ePlaneSide_zero)) {
                    plane1 = plane2;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index1 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = plane1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index1;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = plane1;
                    count_new_edges += 1;
                }

                for (j = 0; j < count_new_edges; j++) {
                    if (new_edges[j].index1 == pPolyhedron->edges[i].index2 && new_edges[j].index2 == pVertex_index) {
                        new_edge_to_plane_indices[j].index2 = plane1;
                        break;
                    }
                }
                if (j >= count_new_edges) {
                    new_edges[count_new_edges].index1 = pPolyhedron->edges[i].index2;
                    new_edges[count_new_edges].index2 = pVertex_index;
                    new_edge_to_plane_indices[count_new_edges].index1 = plane1;
                    count_new_edges += 1;
                }

            }
        }
    }

    final_count_edges = 0;
    for (i = 0; i < pPolyhedron->count_edges; i++) {
        if (positive_side_edges[i]) {
            if (i != final_count_edges) {
                pPolyhedron->edges[final_count_edges] = pPolyhedron->edges[i];
                pEdge_to_plane_indices[final_count_edges] = pEdge_to_plane_indices[i];
            }
            final_count_edges += 1;
        }
    }
    for (i = 0; i < count_new_edges; i++) {
        pPolyhedron->edges[final_count_edges] = new_edges[i];
        pEdge_to_plane_indices[final_count_edges] = new_edge_to_plane_indices[i];
        final_count_edges += 1;
    }
    pPolyhedron->count_edges = final_count_edges;

    final_count_planes = 0;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        if (!(plane_side_buffer[i] & ePlaneSide_negative)) {
            if (i != final_count_planes) {
                BrVector4Copy(&pPolyhedron->planes[final_count_planes], &pPolyhedron->planes[i]);
                for (j = 0; j < pPolyhedron->count_edges; j++) {
                    if (pEdge_to_plane_indices[j].index1 == i) {
                        pEdge_to_plane_indices[j].index1 = final_count_planes;
                    } else if (pEdge_to_plane_indices[j].index2 == i) {
                        pEdge_to_plane_indices[j].index2 = final_count_planes;
                    }
                }
            }
            final_count_planes += 1;
        }
    }
    for (i = 0; i < count_new_planes; i++) {
        BrVector4Copy(&pPolyhedron->planes[final_count_planes], &new_planes[i]);
        for (j = 0; j < pPolyhedron->count_edges; j++) {
            if (pEdge_to_plane_indices[j].index1 == pPolyhedron->count_planes + i) {
                pEdge_to_plane_indices[j].index1 = final_count_planes;
            } else if (pEdge_to_plane_indices[j].index2 == pPolyhedron->count_planes + i) {
                pEdge_to_plane_indices[j].index2 = final_count_planes;
            }
        }
        final_count_planes += 1;
    }
    pPolyhedron->count_planes = final_count_planes;
    return 1;
}
C2_HOOK_FUNCTION(0x00421790, PolyhedronCollisionShape_AddPoint)

br_scalar C2_HOOK_FASTCALL ComparePolyhedronPlaneToNormal(const br_vector3* pV1, const br_vector3* pV2) {
    br_vector3 tv;

    BrVector3Cross(&tv, pV1, pV2);
    return BrVector3Dot(&tv, &C2V(gPhysics_reference_normal_comparison));
}
C2_HOOK_FUNCTION(0x00420d60, ComparePolyhedronPlaneToNormal)

int C2_HOOK_CDECL ComparePolyhedronPointIndicesToNormal(const int* pIndex1, const int* pIndex2) {
    br_vector3 tv1, tv2;
    br_scalar s;
    br_scalar s1, s2;

    BrVector3Sub(&tv1, &C2V(gPolyhedron_to_sort)->points[*pIndex1], &C2V(gPolyhedron_to_sort)->points[0]);
    BrVector3Sub(&tv2, &C2V(gPolyhedron_to_sort)->points[*pIndex2], &C2V(gPolyhedron_to_sort)->points[0]);
    /* Calculate norm2 of normal on vectors v1/v2 with orthonormal */
    s = ComparePolyhedronPlaneToNormal(&tv1, &tv2);
    if (s > 1e-6f) {
        return -1;
    }
    if (s < -1e-6f) {
        return 1;
    }
    s1 = BrVector3LengthSquared(&tv1);
    s2 = BrVector3LengthSquared(&tv2);
    if (s1 < s2) {
        return -1;
    } else if (s2 > s1) {
        return 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00420dc0, ComparePolyhedronPointIndicesToNormal)

tPhysicsError C2_HOOK_FASTCALL AddPolyhedronCollisionShapePlanes(tCollision_shape_polyhedron_data* pPolyhedron) {
    int first_vertex;
    br_vector3 tv1;
    br_vector3 tv2;
    br_vector3 tv3;
    int i;
    int indices_buffer[301];
    br_vector3 point_buffer[300];
    int index_min;
    int new_count_points;
    int read_idx;
    int write_idx;

    for (first_vertex = 0; ArePointsColinear(&pPolyhedron->points[first_vertex + 0], &pPolyhedron->points[first_vertex + 1], &pPolyhedron->points[first_vertex + 2]); first_vertex++) {
        if (first_vertex == pPolyhedron->count_points - 3) {
            return ePhysicsError_UnknownShapeType;
        }
    }
    BrVector3Sub(&tv1, &pPolyhedron->points[first_vertex + 1], &pPolyhedron->points[first_vertex + 0]);
    BrVector3Sub(&tv2, &pPolyhedron->points[first_vertex + 2], &pPolyhedron->points[first_vertex + 0]);
    BrVector3Cross(&tv3, &tv1, &tv2);
    BrVector3Normalise(&C2V(gPhysics_reference_normal_comparison), &tv3);

    index_min = 0;
    for (i = 1; i < pPolyhedron->count_points; i++) {

        if (pPolyhedron->points[i].v[0] < pPolyhedron->points[index_min].v[0]
                || (pPolyhedron->points[i].v[0] == pPolyhedron->points[index_min].v[0]
                        && (pPolyhedron->points[i].v[1] < pPolyhedron->points[index_min].v[1]
                            || (pPolyhedron->points[i].v[1] == pPolyhedron->points[index_min].v[1]
                               && pPolyhedron->points[i].v[2] < pPolyhedron->points[index_min].v[2])))) {
            index_min = i;
        }
    }

    BrVector3Copy(&tv1, &pPolyhedron->points[0]);
    BrVector3Copy(&pPolyhedron->points[0], &pPolyhedron->points[index_min]);
    BrVector3Copy(&pPolyhedron->points[index_min], &tv1);
    for (i = 0; i < pPolyhedron->count_points; i++) {
        indices_buffer[i + 1] = i;
    }
    C2V(gPolyhedron_to_sort) = pPolyhedron;
    c2_qsort(&indices_buffer[2], pPolyhedron->count_points - 1, sizeof(indices_buffer[1]), (int(C2_HOOK_CDECL*)(const void*, const void*))ComparePolyhedronPointIndicesToNormal);
    indices_buffer[0] = indices_buffer[pPolyhedron->count_points];
    indices_buffer[1] = 0;

    new_count_points = 1;
    read_idx = 2;
    write_idx = 1;
    for (i = 1; i < pPolyhedron->count_points; ) {
        BrVector3Sub(&tv1, &pPolyhedron->points[indices_buffer[write_idx]], &pPolyhedron->points[indices_buffer[write_idx - 1]]);
        BrVector3Sub(&tv2, &pPolyhedron->points[indices_buffer[read_idx]], &pPolyhedron->points[indices_buffer[write_idx]]);

        if (ComparePolyhedronPlaneToNormal(&tv1, &tv2) <= 1e-6f) {
            new_count_points -= 1;
            write_idx -= 1;
        } else {
            write_idx += 1;
            new_count_points += 1;
            indices_buffer[write_idx] = indices_buffer[read_idx];
            read_idx += 1;
            i++;
        }
    }
    pPolyhedron->count_points = new_count_points;
    pPolyhedron->count_edges = new_count_points;
    pPolyhedron->count_planes = 1;
    for (i = 0; i < new_count_points; i++) {
        BrVector3Copy(&point_buffer[i], &pPolyhedron->points[indices_buffer[i]]);
    }
    for (i = 0; i < new_count_points; i++) {
        BrVector3Copy(&pPolyhedron->points[i], &point_buffer[i]);
        pPolyhedron->edges[i].index1 = i + 0;
        pPolyhedron->edges[i].index2 = i + 1;
    }
    pPolyhedron->edges[0].index2 = 0;
    BrVector3Copy(&pPolyhedron->planes[0], &C2V(gPhysics_reference_normal_comparison));
    pPolyhedron->planes[0].v[3] = BrVector3Dot(&pPolyhedron->planes[0], &pPolyhedron->points[0]);
    return ePhysicsError_Ok;
}
C2_HOOK_FUNCTION(0x00420910, AddPolyhedronCollisionShapePlanes)

void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds) {
    int i;

    BrVector3Copy(&pBounds->min, &pVertices[0]);
    BrVector3Copy(&pBounds->max, &pVertices[0]);
    for (i = 1; i < pCount_vertices; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (pVertices[i].v[j] < pBounds->min.v[j]) {
                pBounds->min.v[j] = pVertices[i].v[j];
            } else if (pVertices[i].v[j] > pBounds->max.v[j]) {
                pBounds->max.v[j] = pVertices[i].v[j];
            }
        }
    }
}
C2_HOOK_FUNCTION(0x004c5eb0, GetBoundsFromPointList)

tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron) {
    int original_count_points;
    br_vector3 original_points[300];
    tPolyhedron_edge_indexes indices[894];

    if (pPolyhedron->count_points > REC2_ASIZE(original_points)) {
        pPolyhedron->count_points = REC2_ASIZE(original_points);
    }

    original_count_points = pPolyhedron->count_points;
    c2_memcpy(original_points, pPolyhedron->points, sizeof(original_points));
    switch (ProcessTetrahedronPolyhedronCollisionShape(pPolyhedron, indices)) {
    case ePhysicsError_Ok: {
        int new_count_points;
        int i;
        int j;

        new_count_points = 4;
        for (i = 0; i < original_count_points; i++) {

            if (PolyhedronCollisionShape_AddPoint(pPolyhedron, &original_points[i], new_count_points, indices)) {
                if (i != new_count_points) {
                    BrVector3Copy(&pPolyhedron->points[new_count_points], &original_points[i]);
                }
                new_count_points++;
            }
        }
        pPolyhedron->count_points = new_count_points;

        new_count_points = 0;
        for (i = 0; i < pPolyhedron->count_points; i++) {

            for (j = 0; j < pPolyhedron->count_edges; j++) {
                if (i == pPolyhedron->edges[j].index1 || i == pPolyhedron->edges[j].index2) {
                    break;
                }
            }
            if (j >= pPolyhedron->count_edges) {
                continue;
            }
            if (i != new_count_points) {
                BrVector3Copy(&pPolyhedron->points[new_count_points], &pPolyhedron->points[i]);
                for (j = 0; j < pPolyhedron->count_edges; j++) {
                    if (pPolyhedron->edges[j].index1 == i) {
                        pPolyhedron->edges[j].index1 = new_count_points;
                    } else if (pPolyhedron->edges[j].index2 == i) {
                        pPolyhedron->edges[j].index2 = new_count_points;
                    }
                }
            }
            new_count_points++;
        }
        pPolyhedron->count_points = new_count_points;
        return ePhysicsError_Ok;
    }
    default: {
        tPhysicsError err;

        pPolyhedron->count_points = original_count_points;
        c2_memcpy(pPolyhedron->points, original_points, sizeof(original_points));
        err = AddPolyhedronCollisionShapePlanes(pPolyhedron);
        if (err == ePhysicsError_Ok) {
            return ePhysicsError_WireFrameHasNoPoints;
        }
        return err;
    }
    case ePhysicsError_UnknownShapeType:
        return ePhysicsError_UnknownShapeType;
    }
}
C2_HOOK_FUNCTION(0x00420fb0, ConvexHull3D)

void C2_HOOK_FASTCALL FillInShape(tCollision_shape* pShape) {
    int i;
    tPhysicsError error;

    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        break;
    case kCollisionShapeType_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 4) {
            PhysicsError(ePhysicsError_PolyhedronHasNoPoints, 0);
        }
        error = ConvexHull3D(&pShape->polyhedron.polyhedron);
        if (error != ePhysicsError_Ok) {
            if (error == ePhysicsError_WireFrameHasNoPoints) {
                PDFatalError("This polyhedron is too wierd for me");
            }
            pShape->common.type = kCollisionShapeType_Wireframe_Polyhedron;
        }
        GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        break;
    case kCollisionShapeType_Wireframe:
        if (pShape->wireframe.wireframe.count_points == 0) {
            PhysicsError(ePhysicsError_WireFrameHasNoPoints, 0);
        }
        GetBoundsFromPointList(pShape->wireframe.wireframe.points, pShape->wireframe.wireframe.count_points, &pShape->common.bb);
        break;
    case kCollisionShapeType_Wireframe_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 3) {
            PhysicsError(4, 0);
        }
        AddPolyhedronCollisionShapePlanes(&pShape->polyhedron.polyhedron);
        GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        break;
    case kCollisionShapeType_Sphere:
        BrVector3Copy(&pShape->common.bb.min, &pShape->sphere.sphere.center);
        BrVector3Copy(&pShape->common.bb.max, &pShape->sphere.sphere.center);
        for (i = 0; i < 3; i++) {
            pShape->common.bb.min.v[i] -= pShape->sphere.sphere.radius;
            pShape->common.bb.max.v[i] += pShape->sphere.sphere.radius;
        }
        pShape->sphere.sphere.radius_squared = REC2_SQR(pShape->sphere.sphere.radius);
        break;
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, 0);
        break;
    }
}
C2_HOOK_FUNCTION(0x004c5f20, FillInShape)

tCollision_info* (C2_HOOK_FAKE_THISCALL * CreateSphericalCollisionObject_original)(br_model* pModel, float pWeight);
tCollision_info* C2_HOOK_FAKE_THISCALL MungeSphereObject(br_model* pModel, undefined4 pArg2, float pWeight) {

    REC2_THISCALL_UNUSED(pArg2);

#if 0//defined(C2_HOOKS_ENABLED)
    return CreateSphericalCollisionObject_original(pModel, pWeight);
#else
    tCollision_info* collision_info;
    tCollision_shape* shape;
    br_vector3 tv;

    C2_HOOK_BUG_ON(sizeof(tCollision_info) != 1240);

    collision_info = BrMemAllocate(sizeof(tCollision_info), kMem_collision_object);
    collision_info->shape = shape = (tCollision_shape*)AllocateShapeSphere(kMem_collision_shape);
    BrVector3Set(&collision_info->shape->sphere.sphere.center, 0.f, 0.f, 0.f);
    BrVector3Sub(&tv, &pModel->bounds.max, &pModel->bounds.min);
    shape->sphere.sphere.radius = (tv.v[0] + tv.v[1] + tv.v[2]) / 6.f;
    FillInShape(shape);
    collision_info->uid = C2V(gCollision_info_uid_counter);
    C2V(gCollision_info_uid_counter)++;
    UpdateCollisionObject(collision_info);
    collision_info->M = pWeight;
    BrVector3Set(&collision_info->I,
        REC2_SQR(shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(shape->sphere.sphere.radius) * pWeight / 6.f);
    BrVector3SetFloat(&collision_info->field_0x54, 0.f, -0.05797102f, 0.f);
    collision_info->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&collision_info->actor->t.t.mat, &collision_info->transform_matrix);
    collision_info->actor->model = pModel;
    collision_info->box_face_ref = C2V(gFace_num__car) - 2;
    return collision_info;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004da400, MungeSphereObject, CreateSphericalCollisionObject_original)

void C2_HOOK_FASTCALL PositionChildren(tCollision_info *pCollision_info) {

    InternalPositionChildren(pCollision_info, pCollision_info);
}
C2_HOOK_FUNCTION(0x004c2b10, PositionChildren)

void C2_HOOK_FASTCALL InternalPositionChildren(tCollision_info *pParent, tCollision_info *pRoot) {
    if (!(pParent->flags & 0x400)) {
        pRoot = pParent;
    }
    for (tCollision_info *child = pParent->child; child != NULL; child = child->next) {
        if (pParent->flags & 0x400) {
            BrMatrix34Mul(&pParent->transform_matrix, &pParent->field_0x1e8, &pRoot->transform_matrix);
        } else {
            br_vector3 tv1;
            br_vector3 tv2;
            BrMatrix34ApplyP(&tv1, &child->physics_joint1->field_0x14, &pParent->transform_matrix);
            BrMatrix34ApplyP(&tv2, &child->physics_joint1->field_0x08, &child->transform_matrix);
            BrVector3Sub(&tv1, &tv1, &tv2);
            BrVector3Accumulate((br_vector3*)&child->transform_matrix.m[3], &tv1);
        }
        InternalPositionChildren(child, pRoot);
    }
}
C2_HOOK_FUNCTION(0x004c2b20, InternalPositionChildren)

int C2_HOOK_CDECL PHILSetObjectProperty(tCollision_info *pCollision_info, int pParam, ...) {
    va_list va;
    tCollision_info_owner* owner;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x04, 0x04);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x08, 0x08);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x0c, 0x0c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x14, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x240, 0x240);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, box_face_ref, 0x178);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, flags, 0x19c);

    if (C2V(gPHIL_enabled)) {
        return 0;
    }
    if (pCollision_info->field_0x239 != 2) {
    }
    owner = (tCollision_info_owner*)pCollision_info->field_0x240;
    if (owner == NULL) {
        return 3;
    }

    va_start(va, pParam);
    switch (pParam) {
        case 0:
            owner->field_0x10 = (float)va_arg(va, double);
            break;
        case 1: {
            int v = !!va_arg(va, int);
            if (v) {
                owner->field_0x04 |= 0x1;
                PositionChildren(pCollision_info);
            } else {
                owner->field_0x04 &= 0x1;
            }
            break;
        }
        case 2:
            owner->field_0x0c = (float)va_arg(va, double);
            break;
        case 3: {
            int v = va_arg(va, int) != 0;
            pCollision_info->flags = (pCollision_info->flags & ~(1 << 6)) | (v << 6);
            if (!v) {
                pCollision_info->box_face_ref = C2V(gFace_num__car) - 2;
            }
            break;
        }
        case 4: {
            int v = va_arg(va, int) != 0;
            pCollision_info->flags = (pCollision_info->flags & ~(1 << 3)) | (v << 3);
            break;
        }
        case 5: {
            int v = va_arg(va, int) != 0;
            pCollision_info->flags = (pCollision_info->flags & ~(1 << 4)) | (v << 4);
            break;
        }
        case 6: {
            int v = va_arg(va, int) != 0;
            if (v) {
                owner->field_0x04 |= 0x4;
            } else {
                owner->field_0x04 &= 0x4;
            }
            break;
        }
        case 7: {
            int v = va_arg(va, int) != 0;
            if (v) {
                owner->field_0x04 |= 0x8;
                pCollision_info->water_d = 10000.f;
            } else {
                owner->field_0x04 &= 0x8;
            }
            break;
        }
        case 8:
            owner->field_0x14 = (float)va_arg(va, double);
            break;
        default:
            va_end(va);
            return 5;
    }
    va_end(va);
    return 0;
}
C2_HOOK_FUNCTION(0x004b63b0, PHILSetObjectProperty)

float C2_HOOK_FASTCALL PHILGetObjectProperty(tCollision_info *pCollision_info, int pParam) {
    tCollision_info_owner* owner;

    owner = (tCollision_info_owner*)pCollision_info->field_0x240;
    if (C2V(gPHIL_enabled) || owner == NULL) {
        return 0.f;
    }
    switch (pParam) {
    case 0:
        return owner->field_0x10;
    case 1:
        return (float)!!(owner->field_0x04 & 0x1);
    case 2:
        return owner->field_0x0c;
    case 3:
        return (float)!!(pCollision_info->flags & 0x40);
    case 4:
        return (float)!!(pCollision_info->flags & 0x8);
    case 5:
        return (float)!!(pCollision_info->flags & 0x10);
    case 6:
        return (float)!!(owner->field_0x04 & 0x4);
    case 7:
        return (float)!!(owner->field_0x04 & 0x8);
    case 8:
        return owner->field_0x14;
    default:
        return 5.f;
    }
}
C2_HOOK_FUNCTION(0x004b6530, PHILGetObjectProperty)

void C2_HOOK_FASTCALL SetCollisionInfoDoNothing(tCollision_info *pCollision_info, tU8 pDisable) {

    while (pCollision_info != NULL) {
        pCollision_info->disable_move_rotate = pDisable;
        pCollision_info->field_0x1df = 0;
        if (pCollision_info->child != NULL) {
            SetCollisionInfoDoNothing(pCollision_info->child, pDisable);
        }
        pCollision_info = pCollision_info->next;
    }
}
C2_HOOK_FUNCTION(0x004b9eb0, SetCollisionInfoDoNothing)

void C2_HOOK_FASTCALL SetCollisionInfoChildsDoNothing(tCollision_info *pCollision_info, tU8 pDisable) {
    tCollision_info *child;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x1df, 0x1df);

    pCollision_info->disable_move_rotate = pDisable;
    pCollision_info->field_0x1df = 0;
    for (child = pCollision_info->child; child != NULL; child = child->next) {
        child->disable_move_rotate = pDisable;
        child->field_0x1df = 0;
        if (child->child != NULL) {
            SetCollisionInfoDoNothing(child->child,pDisable);
        }
    }
}
C2_HOOK_FUNCTION(0x004b9ef0, SetCollisionInfoChildsDoNothing)

int (C2_HOOK_FASTCALL * PHILRemoveObject_original)(tCollision_info* pCollision_info);
int C2_HOOK_FASTCALL PHILRemoveObject(tCollision_info* pCollision_info) {

#if defined(C2_HOOKS_ENABLED)
    return PHILRemoveObject_original(pCollision_info);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5ea0, PHILRemoveObject, PHILRemoveObject_original)

void C2_HOOK_FASTCALL AddCollisionInfoChild(tCollision_info* pParent, tCollision_info* pChild) {
    tCollision_info* current;
    tCollision_info* last;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, parent, 0x228);

    pChild->parent = pParent;
    last = pChild;
    for (current = pChild->next; current != NULL; current = current->next) {
        current->parent = pParent;
        last = current;
    }
    last->next = pParent->child;
    pParent->child = pChild;
}
C2_HOOK_FUNCTION(0x004c63d0, AddCollisionInfoChild)

int C2_HOOK_FASTCALL PhysicsObjectRecurse(tCollision_info* pCollision_info, tEnumCollision_cbfn* pCallback, void* pUser_data) {
    int r;
    tCollision_info* child;

    r = pCallback(pCollision_info, pUser_data);
    if (r != 0) {
        return r;
    }
    for (child = pCollision_info->child; child != NULL; child = child->next) {
        r = PhysicsObjectRecurse(child,pCallback, pUser_data);
        if (r != 0) {
            return r;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004c6470, PhysicsObjectRecurse)

int (C2_HOOK_FASTCALL * TestForObjectInSensiblePlace_original)(tCollision_info* pCollision_info_1, tCollision_info* pCollision_info_2, br_vector3* pVec3, tCar_callbacks* pCar_callbacks);
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tCollision_info* pCollision_info_1, tCollision_info* pCollision_info_2, br_vector3* pVec3, tCar_callbacks* pCar_callbacks) {

#if defined(C2_HOOKS_ENABLED)
    return TestForObjectInSensiblePlace_original(pCollision_info_1, pCollision_info_2, pVec3, pCar_callbacks);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00429070, TestForObjectInSensiblePlace, TestForObjectInSensiblePlace_original)

tCollision_info* (C2_HOOK_FAKE_THISCALL * CreateBoxCollisionShapeWithMass_original)(br_model* model, undefined4 pArg2, float pMass);
tCollision_info* C2_HOOK_FAKE_THISCALL MungeBoxObject(br_model* pModel, undefined4 pArg2, float pMass) {

#if 0//defined(C2_HOOKS_ENABLED)
    return CreateBoxCollisionShapeWithMass_original(pModel REC2_THISCALL_EDX, pMass);
#else
    tCollision_info* box;
    br_vector3 tv1;

    C2_HOOK_BUG_ON(sizeof(tCollision_info) != 0x4d8);

    box = BrMemAllocate(sizeof(tCollision_info), kMem_collision_shape);
    box->shape = (tCollision_shape*)AllocateBoxCollisionShape(kMem_collision_shape);
    box->shape->box.common.bb = pModel->bounds;
    FillInShape(box->shape);
    box->uid = C2V(gCollision_info_uid_counter);
    C2V(gCollision_info_uid_counter) += 1;
    UpdateCollisionObject(box);
    box->M = pMass;
    BrVector3Sub(&tv1, &pModel->bounds.max, &pModel->bounds.min);
    BrVector3Scale(&tv1, &tv1, .5f);
    BrVector3Set(&box->I,
        (REC2_SQR(tv1.v[1]) + REC2_SQR(tv1.v[2])) * pMass / 12.f,
        (REC2_SQR(tv1.v[0]) + REC2_SQR(tv1.v[2])) * pMass / 12.f,
        (REC2_SQR(tv1.v[0]) + REC2_SQR(tv1.v[1])) * pMass / 12.f);
    BrVector3SetFloat(&box->field_0x54, 0.f, 1/-17.25f, 0.f);
    box->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&box->actor->t.t.mat, &box->transform_matrix);
    box->actor->model = pModel;
    box->box_face_ref = C2V(gFace_num__car) - 2;
    return box;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004da290, MungeBoxObject, CreateBoxCollisionShapeWithMass_original)

tPhysics_joint* (C2_HOOK_FASTCALL * AllocatePhysicsJoint_original)(int pCount_limits, int pType);
tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType) {

#if 0//defined(C2_HOOKS_ENABLED)
    return AllocatePhysicsJoint_original(pCount_limits, pType);
#else
    C2_HOOK_BUG_ON(offsetof(tPhysics_joint, limits) != 0x54);
    C2_HOOK_BUG_ON(sizeof(tPhysics_joint_limit) != 0x20);

    return BrMemAllocate(offsetof(tPhysics_joint, limits) + pCount_limits * sizeof(tPhysics_joint_limit), pType);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c5e20, AllocatePhysicsJoint, AllocatePhysicsJoint_original)

tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType) {
    size_t size;
    tPhysics_joint* clone;

    size = offsetof(tPhysics_joint, limits) + pJoint->count_limits * sizeof(tPhysics_joint_limit);
    clone = BrMemAllocate(size, pType);
    c2_memcpy(clone, pJoint, size);
    return clone;
}
C2_HOOK_FUNCTION(0x004c5e40, ClonePhysicsJoint)

void (C2_HOOK_FASTCALL * CollisionInfoAddChild_original)(tCollision_info* pParent, tCollision_info* pChild);
void C2_HOOK_FASTCALL PhysicsAddObject(tCollision_info* pParent, tCollision_info* pChild) {

#if 0//defined(C2_HOOKS_ENABLED)
    CollisionInfoAddChild_original(pParent, pChild);
#else

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, parent, 0x228);

    pChild->next = pParent->child;
    pParent->child = pChild;
    pChild->parent = pParent;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c63b0, PhysicsAddObject, CollisionInfoAddChild_original)

void (C2_HOOK_FASTCALL * ApplyPhysicsToCars_original)(tU32 pLast_tick_time, tU32 pFrame_period);
void C2_HOOK_FASTCALL ApplyPhysicsToCars(tU32 pLast_tick_time, tU32 pFrame_period) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPhysicsToCars_original(pLast_tick_time, pFrame_period);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00416340, ApplyPhysicsToCars, ApplyPhysicsToCars_original)

tCollision_info* C2_HOOK_FASTCALL PHILGetFirstObject(void) {
    if (!C2V(gPHIL_enabled)) {
        return NULL;
    }
    return C2V(gPHIL_list_collision_infos);
}
C2_HOOK_FUNCTION(0x004b5fd0, PHILGetFirstObject)

tCollision_info* C2_HOOK_FASTCALL PHILGetNextObject(tCollision_info* pCollision_info) {
    if (!C2V(gPHIL_enabled)) {
        return NULL;
    }
    return pCollision_info->next;
}
C2_HOOK_FUNCTION(0x004b5ff0, PHILGetNextObject)

void (C2_HOOK_FASTCALL * InternalPrepareObject_original)(tCollision_info* pObject);
void C2_HOOK_FASTCALL InternalPrepareObject(tCollision_info* pObject) {

#if defined(C2_HOOKS_ENABLED)
    InternalPrepareObject_original(pObject);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b99e0, InternalPrepareObject, InternalPrepareObject_original)

void (C2_HOOK_FAKE_THISCALL * MoveJointedObject_original)(tCollision_info* pObject, undefined4 pArg2, float pDelta_time);
void C2_HOOK_FAKE_THISCALL MoveJointedObject(tCollision_info* pObject, undefined4 pArg2, float pDelta_time) {

#if defined(C2_HOOKS_ENABLED)
    MoveJointedObject_original(pObject, pArg2, pDelta_time);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b7510, MoveJointedObject, MoveJointedObject_original)

void C2_HOOK_FASTCALL MakeObjectListDoSomething(tCollision_info* pObject) {

    while (pObject != NULL) {
        if (pObject->disable_move_rotate) {
            pObject->disable_move_rotate = 0;
            pObject->box_face_ref = C2V(gFace_num__car) - 2;
        }
        if (pObject->child != NULL) {
            MakeObjectListDoSomething(pObject->child);
        }
        pObject = pObject->next;
    }
}
C2_HOOK_FUNCTION(0x004b9f40, MakeObjectListDoSomething)

int (C2_HOOK_FASTCALL * CheckForObjectHierachyTouchingAnotherObject_original)(tCollision_info* pObject, tCollision_info* pList, tCollision_info* pList_original);
int C2_HOOK_FASTCALL CheckForObjectHierachyTouchingAnotherObject(tCollision_info* pObject, tCollision_info* pList, tCollision_info* pList_original) {

#if defined(C2_HOOKS_ENABLED)
    return CheckForObjectHierachyTouchingAnotherObject_original(pObject, pList, pList_original);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b9f90, CheckForObjectHierachyTouchingAnotherObject, CheckForObjectHierachyTouchingAnotherObject_original)

int C2_HOOK_FASTCALL PHILMakeObjectActive(tCollision_info* pObject, const br_vector3* pVel, const br_vector3* pOmega, int pArg4) {
    tQueued_object_info* object_info;

    if (C2V(gPHIL_enabled)) {
        return 0;
    }
    object_info = pObject->field_0x240;
    if (pObject->field_0x239 == 2) {
        object_info->field_0x8 = 2;
        if (pVel != NULL) {
            BrVector3Copy(&object_info->field_0x18, pVel);
        }
        if (pOmega != NULL) {
            BrVector3Copy(&object_info->field_0x24, pOmega);
        }
        if (pArg4) {
            object_info->field_0x4 |= 0x2;
        } else {
            object_info->field_0x4 &= ~0x2;
        }
        return 0;
    }
    if (object_info == NULL) {
        return 2;
    }
    object_info->field_0x8 = 2;
    pObject->flags &= ~0x20;
    if (pArg4) {
        object_info->field_0x4 |= 0x2;
    } else {
        object_info->field_0x4 &= 0x2;
    }
    if (pVel != NULL) {
        BrVector3Copy(&pObject->v, pVel);
    }
    if (pOmega != NULL) {
        BrVector3Copy(&pObject->omega, pOmega);
    }
    ResetCarSpecialVolume(pObject);
    return 0;
}
C2_HOOK_FUNCTION(0x004b6090, PHILMakeObjectActive)

void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingObjectList(tCollision_info* pObject, tCollision_info* pList, tCollision_info* pList_original) {
    tCollision_info* item = pList;

    for (item = pList; item != NULL; item = item->next) {
        if (item->disable_move_rotate) {
            CheckForObjectHierachyTouchingAnotherObject(pObject, item, pList_original);
            if (item->child != NULL) {
                CheckForObjectHierachyTouchingObjectList(pObject, item->child, pList_original);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x004ba190, CheckForObjectHierachyTouchingObjectList)

int (C2_HOOK_FASTCALL * PHILAddActiveObject_original)(tCollision_info* pInfo, undefined4* pArg2, const br_vector3* pArg3, const br_vector3* pArg4);
int C2_HOOK_FASTCALL PHILAddActiveObject(tCollision_info* pInfo, undefined4* pArg2, const br_vector3* pArg3, const br_vector3* pArg4) {

#if defined(C2_HOOKS_ENABLED)
    return PHILAddActiveObject_original(pInfo, pArg2, pArg3, pArg4);
#else
    if (C2V(gPHIL_enabled)) {
        return 0;
    }
    if (PHILAddObject(pInfo) != 0) {
        return 0;
    }
    if (pArg2 != NULL && !C2V(gPHIL_enabled)) {
        NOT_IMPLEMENTED();
    }
    return PHILMakeObjectActive(pInfo, pArg3, pArg4, 0);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b6210, PHILAddActiveObject, PHILAddActiveObject_original)

void C2_HOOK_FASTCALL SwapPair(tCollision_info* pObj1, tCollision_info* pObj2) {
    tCollision_info* obj1_prev;
    tCollision_info* obj2_next;

    obj1_prev = pObj1->prev;
    obj2_next = pObj2->next;
    if (obj1_prev != NULL) {
        obj1_prev->next = pObj2;
    }
    if (obj2_next != NULL) {
        obj2_next->prev = pObj1;
    }
    pObj1->prev = pObj2;
    pObj2->prev = obj1_prev;
    pObj1->next = obj2_next;
    pObj2->next = pObj1;
}

void C2_HOOK_FASTCALL CheckObjectsPostionInList(tCollision_info* pObject, tCollision_info** pList) {

    if (pObject->prev != NULL && pObject->transform_matrix.m[3][0] < pObject->prev->transform_matrix.m[3][0]) {
        do {
            SwapPair(pObject->prev, pObject);
        } while (pObject->prev != NULL && pObject->transform_matrix.m[3][0] < pObject->prev->transform_matrix.m[3][0]);
        if (pObject->prev == NULL) {
            *pList = pObject;
        }
    } else if (pObject->next != NULL && pObject->transform_matrix.m[3][0] > pObject->next->transform_matrix.m[3][0]) {
        do {
            SwapPair(pObject, pObject->next);
        } while (pObject->next != NULL && pObject->transform_matrix.m[3][0] > pObject->next->transform_matrix.m[3][0]);
    }
}

void C2_HOOK_FASTCALL MakeObjectDoSomething(tCollision_info* pObject, tCollision_info* pList) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, parent, 0x228);

    if (pObject->disable_move_rotate) {
        while (pObject->parent != NULL && pObject->parent->physics_joint1 != NULL && pObject->parent->physics_joint1->type != eJoint_none) {
            pObject = pObject->parent;
        }
        pObject->box_face_ref = C2V(gFace_num__car) - 2;
        pObject->disable_move_rotate = 0;
        if (pObject->child != NULL || pObject->physics_joint1 != NULL || pObject->physics_joint2 != NULL) {
            if (pObject->field_0x218 == 0) {
                MoveJointedObject(pObject REC2_THISCALL_EDX, 0.f);
            }
            MakeObjectListDoSomething(pObject->child);
        }
        CheckForObjectHierachyTouchingObjectList(pObject, pList, pList);
    }
}

void C2_HOOK_FASTCALL AddObjectToReducedList(tCollision_info* pObject, tCollision_info** pReduced_list) {

    pObject->field_0x234 = *pReduced_list;
    pReduced_list[0] = pObject;
}

void C2_HOOK_FASTCALL PrepareObject(tCollision_info* pObject, tCollision_info** pList) {

    InternalPrepareObject(pObject);
    C2V(gPrepared_objects) = 1;
    CheckObjectsPostionInList(pObject, pList);
    if (C2V(gINT_006940ac)) {
        MakeObjectDoSomething(pObject, *pList);
    }
    if (C2V(gReduced_object_list) != NULL) {
        AddObjectToReducedList(pObject, C2V(gReduced_object_list));
    }
}
C2_HOOK_FUNCTION(0x004b9770, PrepareObject)

int C2_HOOK_FASTCALL PHILAddObject(tCollision_info* pObject) {
    int i;
    tPhil_object_info_00692458* object_info;

    if (C2V(gPHIL_enabled)) {
        return 0;
    }
    if (C2V(gPHIL_munging_objects) && !C2V(gPHIL_object_added)) {
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_object_info, object, 0x0);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_object_info, field_0x8, 0x8);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_object_info, flags, 0x60);

        C2V(gPHIL_queued_objects)[C2V(gPHIL_count_queued_objects)].object = pObject;
        C2V(gPHIL_queued_objects)[C2V(gPHIL_count_queued_objects)].field_0x8 = 1;
        C2V(gPHIL_queued_objects)[C2V(gPHIL_count_queued_objects)].flags = 0;
        pObject->field_0x240 = &C2V(gPHIL_queued_objects)[C2V(gPHIL_count_queued_objects)];
        pObject->field_0x239 = 2;
        C2V(gPHIL_count_queued_objects) += 1;
        return 0;
    }
    object_info = NULL;
    for (i = 0; i < REC2_ASIZE(C2V(gPhil_objects_00692458)); i++) {
        tPhil_object_info_00692458* current_object_info = &C2V(gPhil_objects_00692458)[i];

        if (current_object_info->collision_info == NULL) {
            if (object_info == NULL) {
                object_info = object_info;
            }
        } else if (current_object_info->collision_info == pObject) {
            return 2;
        }
    }
    if (object_info == NULL) {
        return 1;
    }
    c2_memset(object_info, 0, sizeof(*object_info));
    object_info->collision_info = pObject;
    object_info->field_0x8 = 1;
    pObject->field_0x240 = object_info;
    pObject->flags |= 0x20;
    pObject->field_0x239 = 1;
    C2V(gPHIL_count_list_collision_infos) += 1;
    if (C2V(gPHIL_list_collision_infos) != NULL) {
        pObject->next = C2V(gPHIL_list_collision_infos)->next;
        C2V(gPHIL_list_collision_infos)->next = pObject;
        pObject->prev = C2V(gPHIL_list_collision_infos);
        if (pObject->next != NULL) {
            pObject->next->prev = pObject;
        }
    } else {
        C2V(gPHIL_list_collision_infos) = pObject;
        pObject->next = NULL;
        pObject->prev = NULL;
    }
    if (C2V(gPHIL_doing_physics)) {
        PrepareObject(pObject, &C2V(gPHIL_list_collision_infos));
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004b5d40, PHILAddObject)

void (C2_HOOK_FASTCALL * PHILDoPhysics_original)(tCar_callbacks* pCallbacks, tU32 pLast_tick_time, tU32 pFrame_period);
void C2_HOOK_FASTCALL PHILDoPhysics(tCar_callbacks* pCallbacks, tU32 pLast_tick_time, tU32 pFrame_period) {

#if defined(C2_HOOKS_ENABLED)
    PHILDoPhysics_original(pCallbacks, pLast_tick_time, pFrame_period);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b6630, PHILDoPhysics, PHILDoPhysics_original)

int C2_HOOK_FASTCALL TimeToSendData(void) {
    static C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU32, last_physics_send_time, 0x00659c30, -1);

    if (C2V(gPHIL_last_physics_tick) > C2V(last_physics_send_time) && C2V(gPHIL_last_physics_tick) < C2V(last_physics_send_time) + 80) {
        return 0;
    }
    C2V(last_physics_send_time) = C2V(gPHIL_last_physics_tick);
    return 1;
}
C2_HOOK_FUNCTION(0x004978d0, TimeToSendData)
