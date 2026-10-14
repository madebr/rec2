#ifndef GUARD_65_HULL_H
#define GUARD_65_HULL_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern br_vector3 gPhysics_reference_normal_comparison;
extern tCollision_shape_polyhedron_data* gPolyhedron_to_sort;

// ConvexHull2D

extern br_scalar C2_HOOK_FASTCALL SignedTwiceAreaOf3DTriangle(const br_vector3* pV1, const br_vector3* pV2);

extern int C2_HOOK_CDECL CompareAngle(const void* pIndex1, const void* pIndex2);

extern int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3);

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

#endif // GUARD_65_HULL_H
