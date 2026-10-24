#ifndef GUARD_26_OPPOPROC_H
#define GUARD_26_OPPOPROC_H

#include "rec2_types.h"

// DoNotDprintf

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

// GetStraight

extern int C2_HOOK_FASTCALL TimeToStopStruggling(tOpponent_spec* pOpponent_spec);

// TimeToBeBrakingWhenStruggling

extern void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...);

// CalcCorners

// Distance2D

// SectionLength2D

// NearestSectionInStraight

// SetMaxSpeedFromSOCs

// CalcSOCs

// StuffDataFromCorner

// SetCurvature

// ProcessFollowPath

// FollowCheatyPath

#endif // GUARD_26_OPPOPROC_H
