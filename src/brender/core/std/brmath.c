#include "brmath.h"

#include <math.h>

float C2_HOOK_CDECL BrFloatFloor(float f) {
    return floorf(f);
}
C2_HOOK_FUNCTION(0x0053f4a0, BrFloatFloor)

float C2_HOOK_CDECL BrFloatCeil(float f) {
    return ceilf(f);
}
C2_HOOK_FUNCTION(0x0053f4c0, BrFloatCeil)

float C2_HOOK_CDECL BrFloatSqrt(float f) {
    return sqrtf(f);
}
C2_HOOK_FUNCTION(0x0053f4e0, BrFloatSqrt)

float C2_HOOK_CDECL BrFloatPow(float a, float b) {
    return powf(a, b);
}
C2_HOOK_FUNCTION(0x0053f4f0, BrFloatPow)

float C2_HOOK_CDECL BrFloatAtan2(float x, float y) {
    return atan2f(x, y);
}
C2_HOOK_FUNCTION(0x0053f500, BrFloatAtan2)
