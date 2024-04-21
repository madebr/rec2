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
C2_HOOK_VARIABLE_IMPLEMENT(int, gCollision_info_uid_counter, 0x006a0adc);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gNo_recursive_collision_info_rebuild, 0x0065d004, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gFace_num__car, 0x0065d010, 1);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gPhysics_reference_normal_comparison, 0x00679420);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_shape_polyhedron_data*, gPolyhedron_to_sort, 0x0067942c);

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

int (C2_HOOK_FASTCALL * ResetMechanics_original)(void);
int C2_HOOK_FASTCALL ResetMechanics(void) {

#if defined(C2_HOOKS_ENABLED)
    return ResetMechanics_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5cc0, ResetMechanics, ResetMechanics_original)

void C2_HOOK_FASTCALL DisableRecursiveCollisions(void) {
    C2V(gNo_recursive_collision_info_rebuild) = 1;
}
C2_HOOK_FUNCTION(0x004b5d20, DisableRecursiveCollisions)

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
C2_HOOK_FUNCTION(0x004c5d70, AllocatePolyhedronCollisionShape)

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

void C2_HOOK_FASTCALL CalculateBoundingBox(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds) {
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
C2_HOOK_FUNCTION(0x004c5eb0, CalculateBoundingBox)

tPhysicsError C2_HOOK_FASTCALL ProcessPolyhedronCollisionShape(tCollision_shape_polyhedron_data* pPolyhedron) {
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
            int edge_j;

            for (j = 0; j < pPolyhedron->count_edges; j++) {
                if (i == pPolyhedron->edges[j].index1 || i == pPolyhedron->edges[j].index2) {
                    break;
                }
            }
            edge_j = j;
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
C2_HOOK_FUNCTION(0x00420fb0, ProcessPolyhedronCollisionShape)

void C2_HOOK_FASTCALL ProcessCollisionShape(tCollision_shape* pShape) {
    int i;
    tPhysicsError error;

    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        break;
    case kCollisionShapeType_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 4) {
            PhysicsError(ePhysicsError_PolyhedronHasNoPoints);
        }
        error = ProcessPolyhedronCollisionShape(&pShape->polyhedron.polyhedron);
        if (error != ePhysicsError_Ok) {
            if (error == ePhysicsError_WireFrameHasNoPoints) {
                PDFatalError("This polyhedron is too wierd for me");
            }
            pShape->common.type = kCollisionShapeType_Wireframe_Polyhedron;
        }
        CalculateBoundingBox(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        break;
    case kCollisionShapeType_Wireframe:
        if (pShape->wireframe.wireframe.count_points == 0) {
            PhysicsError(ePhysicsError_WireFrameHasNoPoints);
        }
        CalculateBoundingBox(pShape->wireframe.wireframe.points, pShape->wireframe.wireframe.count_points, &pShape->common.bb);
        break;
    case kCollisionShapeType_Wireframe_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 3) {
            PhysicsError(4);
        }
        AddPolyhedronCollisionShapePlanes(&pShape->polyhedron.polyhedron);
        CalculateBoundingBox(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
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
        PhysicsError(ePhysicsError_UnknownShapeType);
        break;
    }
}
C2_HOOK_FUNCTION(0x004c5f20, ProcessCollisionShape)

tCollision_info* (C2_HOOK_FAKE_THISCALL * CreateSphericalCollisionObject_original)(br_model* pModel, float pWeight);
tCollision_info* C2_HOOK_FAKE_THISCALL CreateSphericalCollisionObject(br_model* pModel, undefined4 pArg2, float pWeight) {

    REC2_THISCALL_UNUSED(pArg2);

#if 0//defined(C2_HOOKS_ENABLED)
    return CreateSphericalCollisionObject_original(pModel, pWeight);
#else
    tCollision_info* collision_info;
    tCollision_shape* shape;
    br_vector3 tv;

    C2_HOOK_BUG_ON(sizeof(tCollision_info) != 1240);

    collision_info = BrMemAllocate(sizeof(tCollision_info), kMem_collision_object);
    collision_info->shape = shape = (tCollision_shape*)AllocateSphereCollisionShape(kMem_collision_shape);
    BrVector3Set(&collision_info->shape->sphere.sphere.center, 0.f, 0.f, 0.f);
    BrVector3Sub(&tv, &pModel->bounds.max, &pModel->bounds.min);
    shape->sphere.sphere.radius = (tv.v[0] + tv.v[1] + tv.v[2]) / 6.f;
    ProcessCollisionShape(shape);
    collision_info->uid = C2V(gCollision_info_uid_counter);
    C2V(gCollision_info_uid_counter)++;
    UpdateCollisionBoundingBox(collision_info);
    collision_info->M = pWeight;
    BrVector3Set(&collision_info->I,
        REC2_SQR(shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(shape->sphere.sphere.radius) * pWeight / 6.f);
    BrVector3SetFloat(&collision_info->field7_0x54, 0.f, -0.05797102f, 0.f);
    collision_info->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&collision_info->actor->t.t.mat, &collision_info->transform_matrix);
    collision_info->actor->model = pModel;
    collision_info->box_face_ref = C2V(gFace_num__car) - 2;
    return collision_info;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004da400, CreateSphericalCollisionObject, CreateSphericalCollisionObject_original)

int (C2_HOOK_FASTCALL * MarkCollisionInfoAsProcessed_original)(tCollision_info* pCollision_info);
int C2_HOOK_FASTCALL MarkCollisionInfoAsProcessed(tCollision_info* pCollision_info) {

#if defined(C2_HOOKS_ENABLED)
    return MarkCollisionInfoAsProcessed_original(pCollision_info);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5d40, MarkCollisionInfoAsProcessed, MarkCollisionInfoAsProcessed_original)

void C2_HOOK_FASTCALL FUN_004c2b10(tCollision_info *pCollision_info) {

    FUN_004c2b20(pCollision_info, pCollision_info);
}
C2_HOOK_FUNCTION(0x004c2b10, FUN_004c2b10)

void C2_HOOK_FASTCALL FUN_004c2b20(tCollision_info *pParent, tCollision_info *pRoot) {
    if (!(pParent->flags_0x19c & 0x400)) {
        pRoot = pParent;
    }
    for (tCollision_info *child = pParent->child; child != NULL; child = child->next) {
        if (pParent->flags_0x19c & 0x400) {
            BrMatrix34Mul(&pParent->transform_matrix, &pParent->field_0x1e8, &pRoot->transform_matrix);
        } else {
            br_vector3 tv1;
            br_vector3 tv2;
            BrMatrix34ApplyP(&tv1, &child->physics_joint1->field_0x14, &pParent->transform_matrix);
            BrMatrix34ApplyP(&tv2, &child->physics_joint1->field_0x08, &child->transform_matrix);
            BrVector3Sub(&tv1, &tv1, &tv2);
            BrVector3Accumulate((br_vector3*)&child->transform_matrix.m[3], &tv1);
        }
        FUN_004c2b20(child, pRoot);
    }
}
C2_HOOK_FUNCTION(0x004c2b20, FUN_004c2b20)

int C2_HOOK_CDECL SetCollisionInfoParam(tCollision_info *pCollision_info, int pParam, ...) {
    va_list va;
    tCollision_info_owner* owner;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x04, 0x04);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x08, 0x08);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x0c, 0x0c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info_owner, field_0x14, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x240, 0x240);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, box_face_ref, 0x178);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, flags_0x19c, 0x19c);

    if (C2V(gNo_recursive_collision_info_rebuild)) {
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
                FUN_004c2b10(pCollision_info);
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
            pCollision_info->flags_0x19c = (pCollision_info->flags_0x19c & ~(1 << 6)) | (v << 6);
            if (!v) {
                pCollision_info->box_face_ref = C2V(gFace_num__car) - 2;
            }
            break;
        }
        case 4: {
            int v = va_arg(va, int) != 0;
            pCollision_info->flags_0x19c = (pCollision_info->flags_0x19c & ~(1 << 3)) | (v << 3);
            break;
        }
        case 5: {
            int v = va_arg(va, int) != 0;
            pCollision_info->flags_0x19c = (pCollision_info->flags_0x19c & ~(1 << 4)) | (v << 4);
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
C2_HOOK_FUNCTION(0x004b63b0, SetCollisionInfoParam)

int (C2_HOOK_FASTCALL * RemoveFromCollisionInfoList_original)(tCollision_info* pCollision_info);
int C2_HOOK_FASTCALL RemoveFromCollisionInfoList(tCollision_info* pCollision_info) {

#if defined(C2_HOOKS_ENABLED)
    return RemoveFromCollisionInfoList_original(pCollision_info);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5ea0, RemoveFromCollisionInfoList, RemoveFromCollisionInfoList_original)

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

int C2_HOOK_FASTCALL EnumCollisionInfo(tCollision_info* pCollision_info, tEnumCollision_cbfn* pCallback, void* pUser_data) {
    int r;
    tCollision_info* child;

    r = pCallback(pCollision_info, pUser_data);
    if (r != 0) {
        return r;
    }
    for (child = pCollision_info->child; child != NULL; child = child->next) {
        r = EnumCollisionInfo(child,pCallback, pUser_data);
        if (r != 0) {
            return r;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004c6470, EnumCollisionInfo)

int (C2_HOOK_FASTCALL * FUN_00429070_original)(tCollision_info* pCollision_info_1, tCollision_info* pCollision_info_2, br_vector3* pVec3, tCar_callbacks* pCar_callbacks);
int C2_HOOK_FASTCALL FUN_00429070(tCollision_info* pCollision_info_1, tCollision_info* pCollision_info_2, br_vector3* pVec3, tCar_callbacks* pCar_callbacks) {

#if defined(C2_HOOKS_ENABLED)
    return FUN_00429070_original(pCollision_info_1, pCollision_info_2, pVec3, pCar_callbacks);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00429070, FUN_00429070, FUN_00429070_original)
