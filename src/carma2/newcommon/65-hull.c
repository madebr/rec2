#include "65-hull.h"

#include "brender/brender.h"

// GLOBAL: CARMA2_HW 0x00679420
br_vector3 gPhysics_reference_normal_comparison;

// GLOBAL: CARMA2_HW 0x0067942c
tCollision_shape_polyhedron_data* gPolyhedron_to_sort;

// ConvexHull2D

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

