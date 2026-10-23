#include "36-trig.h"

#include "c2_math.h"

// FastFloatSin

// FastFloatCos

// FastScalarSin

// FastScalarCos

// FastScalarSinAngle

// FastScalarCosAngle

// FUNCTION: CARMA2_HW 0x00511ce0
float C2_HOOK_STDCALL FastFloatArcTan2(float pY, float pX) {
    float abs_x;
    float abs_y;

    abs_x = (float)fabs(pX);
    abs_y = (float)fabs(pY);
    if (pX == 0.f) {
        if (pY < 0.f) {
            return 270.f;
        }
        if (pY > 0.f) {
            return 90.f;
        }
        return 0.f;
    }
    if (pX < 0.f) {
        if (pY < 0.f) {
            if (abs_x < abs_y) {
                return (float)((6.f - abs_x / abs_y) * 45.0);
            }
            return (float)((abs_y / abs_x + 4.f) * 45.0);
        }
        if (abs_x < abs_y) {
            return (float)((abs_x / abs_y + 2.f) * 45.0);
        }
        return (float)((4.f - abs_y / abs_x) * 45.0);
    }
    if (pY < 0.f) {
        if (abs_x < abs_y) {
            return (float)((abs_x / abs_y + 6.f) * 45.0);
        }
        return (float)((8.f - abs_y / abs_x) * 45.0);
    }
    if (abs_x < abs_y) {
        return (float)((2.f - abs_x / abs_y) * 45.0);
    }
    return (float)(abs_y / abs_x * 45.0);
}

// FUNCTION: CARMA2_HW 0x00511e50
br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX) {
    return FastFloatArcTan2(pY, pX);
}

// FUNCTION: CARMA2_HW 0x00511e70
br_angle C2_HOOK_STDCALL FastFloatArcTan2Angle(float pY, float pX) {
    float abs_x;
    float abs_y;

    abs_x = (float)fabs(pX);
    abs_y = (float)fabs(pY);
    if (pX == 0.f) {
        if (pY < 0.f) {
            return BR_ANGLE_DEG(270);
        }
        if (pY > 0.f) {
            return BR_ANGLE_DEG(90);
        }
        return 0;
    }
    if (pX < 0.f) {
        if (pY < 0.f) {
            if (abs_x < abs_y) {
                return (br_angle)((6.f - abs_x / abs_y) * 8192.f);
            }
            return (br_angle)((abs_y / abs_x + 4.f) * 8192.f);
        }
        if (abs_x < abs_y) {
            return (br_angle)((abs_x / abs_y + 2.f) * 8192.f);
        }
        return (br_angle)((4.f - abs_y / abs_x) * 8192.f);
    }
    if (pY < 0.f) {
        if (abs_x < abs_y) {
            return (br_angle)((abs_x / abs_y + 6.f) * 8192.f);
        }
        return (br_angle)((8.f - abs_y / abs_x) * 8192.f);
    }
    if (abs_x < abs_y) {
        return (br_angle)((2.f - abs_x / abs_y) * 8192.f);
    }
    return (br_angle)(abs_y / abs_x * 8192.f);
}

// FUNCTION: CARMA2_HW 0x00512010
br_angle C2_HOOK_STDCALL FastScalarArcTan2Angle(br_scalar pY, br_scalar pX) {
    return FastFloatArcTan2Angle(pY, pX);
}

// DRMatrix34RotateX

// DRMatrix34RotateY

// DRMatrix34RotateZ

// DRMatrix34PostRotateX

// STUB: CARMA2_HW 0x00512ad0
// STUB: CARMA2_SW 0x0050df10
void C2_HOOK_FASTCALL DRMatrix34PostRotateY(br_matrix34* pMat, br_angle pRy) {
    NOT_IMPLEMENTED();
}

// DRMatrix34PreRotateZ

// DRMatrix34PostRotateZ

// DRMatrix23Rotate

// DRMatrix23PostRotate

