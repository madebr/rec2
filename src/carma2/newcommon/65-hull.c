#include "65-hull.h"

// SignedTwiceAreaOf3DTriangle

// CompareAngle

// FUNCTION: CARMA2_HW 0x00420ef0
int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3) {

    return (float)fabs((pV3->v[2] - pV1->v[2]) * (pV2->v[1] - pV1->v[1]) - (pV3->v[1] - pV1->v[1]) * (pV2->v[2] - pV1->v[2])) < 1e-6f
        && (float)fabs((pV3->v[0] - pV1->v[0]) * (pV2->v[2] - pV1->v[2]) - (pV2->v[0] - pV1->v[0]) * (pV3->v[2] - pV1->v[2])) < 1e-6f
        && (float)fabs((pV2->v[0] - pV1->v[0]) * (pV3->v[1] - pV1->v[1]) - (pV3->v[0] - pV1->v[0]) * (pV2->v[1] - pV1->v[1])) < 1e-6f;
}

// Left

// ConvexHull2D

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

