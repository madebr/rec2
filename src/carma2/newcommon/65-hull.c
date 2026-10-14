#include "65-hull.h"

#include "brender/brender.h"
#include "c2_stdlib.h"

// GLOBAL: CARMA2_HW 0x00679420
br_vector3 gPhysics_reference_normal_comparison;

// GLOBAL: CARMA2_HW 0x0067942c
tCollision_shape_polyhedron_data* gPolyhedron_to_sort;

// FUNCTION: CARMA2_HW 0x00420910
tPhysicsError C2_HOOK_FASTCALL ConvexHull2D(tCollision_shape_polyhedron_data* pPolyhedron) {
    int first;
    int i;
    int index_min;
    int new_count;
    int read_index;
    int write_index;
    int indices[301];
    br_vector3 saved_points[300];
    br_vector3 edge1;
    br_vector3 edge2;
    br_vector3 normal;

    /* The first non-collinear triple fixes the projection plane. */
    first = 0;
    if (Collinear(&pPolyhedron->points[0], &pPolyhedron->points[1], &pPolyhedron->points[2])) {
        first = 1;
        if (first == pPolyhedron->count_points - 3) {
            return ePhysicsError_UnknownShapeType;
        }
        while (Collinear(&pPolyhedron->points[first], &pPolyhedron->points[first + 1], &pPolyhedron->points[first + 2])) {
            first += 1;
            if (first == pPolyhedron->count_points - 3) {
                return ePhysicsError_UnknownShapeType;
            }
        }
    }

    BrVector3Sub(&edge1, &pPolyhedron->points[first + 1], &pPolyhedron->points[first + 0]);
    BrVector3Sub(&edge2, &pPolyhedron->points[first + 2], &pPolyhedron->points[first + 0]);
    BrVector3Cross(&normal, &edge1, &edge2);
    BrVector3Normalise(&gPhysics_reference_normal_comparison, &normal);

    index_min = 0;
    for (i = 1; i < pPolyhedron->count_points; i++) {
        if (!(pPolyhedron->points[i].v[0] >= pPolyhedron->points[index_min].v[0])
                || (pPolyhedron->points[i].v[0] == pPolyhedron->points[index_min].v[0]
                    && (!(pPolyhedron->points[i].v[1] >= pPolyhedron->points[index_min].v[1])
                        || (pPolyhedron->points[i].v[1] == pPolyhedron->points[index_min].v[1]
                            && !(pPolyhedron->points[i].v[2] >= pPolyhedron->points[index_min].v[2]))))) {
            index_min = i;
        }
    }

    BrVector3Copy(&edge1, &pPolyhedron->points[0]);
    BrVector3Copy(&pPolyhedron->points[0], &pPolyhedron->points[index_min]);
    BrVector3Copy(&pPolyhedron->points[index_min], &edge1);

    for (i = 0; i < pPolyhedron->count_points; i++) {
        indices[i + 1] = i;
    }
    gPolyhedron_to_sort = pPolyhedron;
    qsort(&indices[2], pPolyhedron->count_points - 1, sizeof(indices[0]), CompareAngle);
    indices[0] = indices[pPolyhedron->count_points];
    indices[1] = 0;

    new_count = 1;
    read_index = 2;
    write_index = 1;
    for (i = 1; i < pPolyhedron->count_points; ) {
        BrVector3Sub(&edge1, &pPolyhedron->points[indices[write_index]], &pPolyhedron->points[indices[write_index - 1]]);
        BrVector3Sub(&edge2, &pPolyhedron->points[indices[read_index]], &pPolyhedron->points[indices[write_index]]);
        if (!(SignedTwiceAreaOf3DTriangle(&edge1, &edge2) > 1.0e-6f)) {
            new_count -= 1;
            write_index -= 1;
        } else {
            write_index += 1;
            new_count += 1;
            indices[write_index] = indices[read_index];
            read_index += 1;
            i += 1;
        }
    }

    pPolyhedron->count_points = new_count;
    pPolyhedron->count_edges = new_count;
    pPolyhedron->count_planes = 1;
    for (i = 0; i < new_count; i++) {
        BrVector3Copy(&saved_points[i], &pPolyhedron->points[indices[i]]);
    }
    for (i = 0; i < new_count; i++) {
        BrVector3Copy(&pPolyhedron->points[i], &saved_points[i]);
        pPolyhedron->edges[i].index1 = (tU8)i;
        pPolyhedron->edges[i].index2 = (tU8)(i + 1);
    }
    /* Edge zero starts and ends at the first hull point. */
    pPolyhedron->edges[0].index2 = 0;
    pPolyhedron->planes[0].v[0] = gPhysics_reference_normal_comparison.v[0];
    pPolyhedron->planes[0].v[1] = gPhysics_reference_normal_comparison.v[1];
    pPolyhedron->planes[0].v[2] = gPhysics_reference_normal_comparison.v[2];
    pPolyhedron->planes[0].v[3] = BrVector3Dot(&gPhysics_reference_normal_comparison, &pPolyhedron->points[0]);
    return ePhysicsError_Ok;
}

// FUNCTION: CARMA2_HW 0x00420d60
br_scalar C2_HOOK_FASTCALL SignedTwiceAreaOf3DTriangle(const br_vector3* pV1, const br_vector3* pV2) {
    br_vector3 cross;

    BrVector3Cross(&cross, pV1, pV2);
    return BrVector3Dot(&cross, &gPhysics_reference_normal_comparison);
}

// FUNCTION: CARMA2_HW 0x00420dc0
int C2_HOOK_CDECL CompareAngle(const void* pIndex1, const void* pIndex2) {
    const int* index1 = pIndex1;
    const int* index2 = pIndex2;
    br_vector3 edge1;
    br_vector3 edge2;
    br_scalar area;
    br_scalar length1;
    br_scalar length2;

    BrVector3Sub(&edge1, &gPolyhedron_to_sort->points[*index1], &gPolyhedron_to_sort->points[0]);
    BrVector3Sub(&edge2, &gPolyhedron_to_sort->points[*index2], &gPolyhedron_to_sort->points[0]);
    area = SignedTwiceAreaOf3DTriangle(&edge1, &edge2);
    if (area > 1.0e-6f) {
        return -1;
    }
    if (!(area >= -1.0e-6f)) {
        return 1;
    }
    length1 = BrVector3Length(&edge1);
    length2 = BrVector3Length(&edge2);
    if (!(length1 >= length2)) {
        return -1;
    }
    if (length1 > length2) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00420ef0
int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3) {
    br_scalar dx21;
    br_scalar dy21;
    br_scalar dz21;
    br_scalar dx31;
    br_scalar dy31;
    br_scalar dz31;
    br_scalar cross;

    dy21 = pV2->v[1] - pV1->v[1];
    dz21 = pV2->v[2] - pV1->v[2];
    dy31 = pV3->v[1] - pV1->v[1];
    dz31 = pV3->v[2] - pV1->v[2];
    cross = dz31 * dy21 - dy31 * dz21;
    if (cross >= 1.0e-6f || cross <= -1.0e-6f) {
        return 0;
    }
    dx21 = pV2->v[0] - pV1->v[0];
    dx31 = pV3->v[0] - pV1->v[0];
    cross = dx31 * dz21 - dx21 * dz31;
    if (cross >= 1.0e-6f || cross <= -1.0e-6f) {
        return 0;
    }
    cross = dx21 * dy31 - dx31 * dy21;
    if (cross >= 1.0e-6f || cross <= -1.0e-6f) {
        return 0;
    }
    return 1;
}

// Left

// DRVector4Negate

// ConstructTetrahedron

// CheckSignOfNormal

// AddNewEdge

// PointOutsidePlane

// AdjustEdgePlanes

// AddPointToPolyhedron

// AdjustEdges

// PointOnShape

// ConvexHull3D

// GetPointSample

// SampledConvexHull

// NPointConvexHullUsingSample

