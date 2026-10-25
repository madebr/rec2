/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_65_HULL_H
#define GUARD_65_HULL_H

#include "c2_hooks.h"
#include "rec2_types.h"


extern br_scalar C2_HOOK_FASTCALL SignedTwiceAreaOf3DTriangle(const br_vector3* pV1, const br_vector3* pV2);

extern int C2_HOOK_CDECL CompareAngle(const void *pLhs, const void *pRhs);

extern int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3);

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

#endif // GUARD_65_HULL_H
