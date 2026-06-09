#include "brmath.h"

#include "c2_math.h"

// FUNCTION: CARMA2_HW 0x0053f4a0
float C2_HOOK_CDECL BrFloatFloor(float f) {
    return floorf(f);
}

// FUNCTION: CARMA2_HW 0x0053f4c0
float C2_HOOK_CDECL BrFloatCeil(float f) {
    return ceilf(f);
}

// FUNCTION: CARMA2_HW 0x0053f4e0
float C2_HOOK_CDECL BrFloatSqrt(float f) {

#ifdef REC2_MATCHING
    return (float)sqrt(f);
#else
    return sqrtf(f);
#endif
}

// FUNCTION: CARMA2_HW 0x0053f4f0
float C2_HOOK_CDECL BrFloatPow(float a, float b) {

    return powf(a, b);
}

// FUNCTION: CARMA2_HW 0x0053f500
float C2_HOOK_CDECL BrFloatAtan2(float x, float y) {

    return atan2f(x, y);
}
