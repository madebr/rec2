#ifndef GUARD_26_OPPOPROC_H
#define GUARD_26_OPPOPROC_H

#include "rec2_types.h"

// DoNotDprintf

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

extern int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection, tOpponent_spec* pOpponent_spec);

// TimeToStopStruggling

// TimeToBeBrakingWhenStruggling

extern int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, const br_vector2* pStart, tOpponent_spec* pOpponent_spec);

// Distance2D

// SectionLength2D

// NearestSectionInStraight

extern void C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* pSocs, int pCount_socs, float* pDesired_speed, float pSpeed, const br_vector2* pPos2d, tCorner* pCorners, tOpponent_spec* pOpponent_spec);

extern int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar);

// StuffDataFromCorner

// SetCurvature

// ProcessFollowPath

// FollowCheatyPath

#endif // GUARD_26_OPPOPROC_H
