#ifndef REC2_BRMATH_H
#define REC2_BRMATH_H

#include "c2_hooks.h"

#include "brender/br_types.h"

float C2_HOOK_CDECL BrFloatFloor(float f);

float C2_HOOK_CDECL BrFloatCeil(float f);

float C2_HOOK_CDECL BrFloatSqrt(float f);

float C2_HOOK_CDECL BrFloatPow(float a, float b);

float C2_HOOK_CDECL BrFloatAtan2(float x, float y);

#endif // REC2_BRMATH_H
