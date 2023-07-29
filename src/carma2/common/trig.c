#include "trig.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(float, gFloat_sine_table, 91, 0x00661398, {
    0.0f,
    0.017452f,
    0.034899f,
    0.052336f,
    0.069756f,
    0.087156f,
    0.104528f,
    0.121869f,
    0.139173f,
    0.156434f,
    0.173648f,
    0.190809f,
    0.207912f,
    0.224951f,
    0.241922f,
    0.258819f,
    0.275637f,
    0.292372f,
    0.309017f,
    0.325568f,
    0.342020f,
    0.358368f,
    0.374607f,
    0.390731f,
    0.406737f,
    0.422618f,
    0.438371f,
    0.453990f,
    0.469472f,
    0.484810f,
    0.5f,
    0.51503801f,
    0.52991903f,
    0.54463899f,
    0.55919302f,
    0.57357597f,
    0.58778501f,
    0.60181499f,
    0.61566103f,
    0.62932003f,
    0.64278799f,
    0.65605903f,
    0.66913098f,
    0.68199801f,
    0.69465798f,
    0.70710701f,
    0.71934003f,
    0.731354f,
    0.74314499f,
    0.75471002f,
    0.76604402f,
    0.77714598f,
    0.78801101f,
    0.79863602f,
    0.809017f,
    0.819152f,
    0.82903802f,
    0.83867103f,
    0.84804797f,
    0.85716701f,
    0.86602497f,
    0.87462002f,
    0.88294798f,
    0.89100701f,
    0.898794f,
    0.906308f,
    0.91354501f,
    0.92050499f,
    0.92718399f,
    0.93357998f,
    0.93969297f,
    0.94551897f,
    0.95105702f,
    0.95630503f,
    0.96126199f,
    0.96592599f,
    0.97029603f,
    0.97437f,
    0.97814798f,
    0.98162699f,
    0.98480803f,
    0.987688f,
    0.99026799f,
    0.99254602f,
    0.99452198f,
    0.99619502f,
    0.99756402f,
    0.99862999f,
    0.99939102f,
    0.99984801f,
    1.0f,
});

float C2_HOOK_FASTCALL FastFloatSin(int pAngle_in_degrees) {

    if (pAngle_in_degrees >= 0) {
        pAngle_in_degrees = pAngle_in_degrees % 360;
    } else {
        pAngle_in_degrees = 360 - -pAngle_in_degrees % 360;
    }
    if (pAngle_in_degrees > 270) {
        return -C2V(gFloat_sine_table)[360 - pAngle_in_degrees];
    }
    if (pAngle_in_degrees > 180) {
        return -C2V(gFloat_sine_table)[pAngle_in_degrees - 180];
    }
    if (pAngle_in_degrees > 90) {
        return C2V(gFloat_sine_table)[180 - pAngle_in_degrees];
    }
    return C2V(gFloat_sine_table)[pAngle_in_degrees];
}
C2_HOOK_FUNCTION(0x005113d0, FastFloatSin)

float C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees) {

    if (pAngle_in_degrees >= 0) {
        pAngle_in_degrees = pAngle_in_degrees % 360;
    } else {
        pAngle_in_degrees = 360 - -pAngle_in_degrees % 360;
    }

    if (pAngle_in_degrees > 270) {
        return C2V(gFloat_sine_table)[pAngle_in_degrees - 270];
    }
    if (pAngle_in_degrees > 180) {
        return -C2V(gFloat_sine_table)[270 - pAngle_in_degrees];
    }
    if (pAngle_in_degrees > 90) {
        return -C2V(gFloat_sine_table)[pAngle_in_degrees - 90];
    }
    return C2V(gFloat_sine_table)[90 - pAngle_in_degrees];
}
C2_HOOK_FUNCTION(0x00511440, FastFloatCos)

float C2_HOOK_FASTCALL FastFloatTan(int pAngle_in_degrees) {

    return FastFloatSin(pAngle_in_degrees) / FastFloatCos(pAngle_in_degrees);
}
C2_HOOK_FUNCTION(0x005114b0, FastFloatTan)

br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees) {

    return FastFloatSin((int)pAngle_in_degrees);
}
C2_HOOK_FUNCTION(0x005115b0, FastScalarSin)

br_scalar C2_HOOK_FASTCALL FastScalarCos(int pAngle_in_degrees) {

    return FastFloatCos(pAngle_in_degrees);
}
C2_HOOK_FUNCTION(0x00511620, FastScalarCos)

br_scalar C2_HOOK_FASTCALL FastScalarTan(int pAngle_in_degrees) {

    return FastScalarSin(pAngle_in_degrees) / FastScalarCos(pAngle_in_degrees);
}
C2_HOOK_FUNCTION(0x00511690, FastScalarTan)

br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pBR_angle) {

    return FastScalarSin((int)BrAngleToDegrees(pBR_angle));
}
C2_HOOK_FUNCTION(0x00511780, FastScalarSinAngle)

br_scalar C2_HOOK_FASTCALL FastScalarCosAngle(br_angle pBR_angle) {

    return FastScalarCos((int)BrAngleToDegrees(pBR_angle));
}
C2_HOOK_FUNCTION(0x00511810, FastScalarCosAngle)

br_scalar C2_HOOK_FASTCALL FastScalarTanAngle(br_angle pBR_angle) {
    int angle_in_degrees;

    angle_in_degrees = (int)BrAngleToDegrees(pBR_angle);
    return FastScalarSin(angle_in_degrees) / FastScalarCos(angle_in_degrees);
}
C2_HOOK_FUNCTION(0x005118a0, FastScalarTanAngle)

float C2_HOOK_STDCALL FastFloatArcSin(float pValue) {
    float low_limit;
    float high_limit;
    float mid_point;

    if (pValue < 0.f) {
        return -FastFloatArcSin(-pValue);
    }
    high_limit = 90.f * pValue;
    low_limit = high_limit - 19.f;
    if (low_limit < 0.f) {
        low_limit = 0.f;
    }
    while (high_limit - low_limit >= 1.f) {
        mid_point = (low_limit + high_limit) / 2.f;

        if (C2V(gFloat_sine_table)[(int)mid_point] > pValue) {
            high_limit = mid_point;
        } else {
            low_limit = mid_point;
        }
    }
    return low_limit;
}
C2_HOOK_FUNCTION(0x005119c0, FastFloatArcSin)

float C2_HOOK_STDCALL FastFloatArcCos(float pValue) {

    return 90.f - FastFloatArcSin(pValue);
}
C2_HOOK_FUNCTION(0x00511a80, FastFloatArcCos)

br_scalar C2_HOOK_STDCALL FastScalarArcSin(br_scalar pValue) {

    return FastFloatArcSin(pValue);
}
C2_HOOK_FUNCTION(0x00511b50, FastScalarArcSin)

br_scalar C2_HOOK_STDCALL FastScalarArcCos(br_scalar pValue) {

    return 90.f - FastScalarArcSin(pValue);
}
C2_HOOK_FUNCTION(0x00511c10, FastScalarArcCos)

float C2_HOOK_STDCALL FastFloatArcTan2(float pY, float pX) {
    float abs_x;
    float abs_y;

    abs_x = fabsf(pX);
    abs_y = fabsf(pY);
    if (pX == 0.0f) {
        if (pY < 0.0f) {
            return 270.0;
        } else if (pY == 0.0f) {
            return 0.0f;
        } else {
            return 90.0f;
        }
    } else if (pX > 0.0f) {
        if (pY >= 0.0) {
            if (abs_y <= abs_x) {
                return abs_y / abs_x * 45.0f;
            } else {
                return (2.0f - abs_x / abs_y) * 45.0f;
            }
        } else {
            if (abs_y <= abs_x) {
                return (8.0f - abs_y / abs_x) * 45.0f;
            } else {
                return (abs_x / abs_y + 6.0f) * 45.0f;
            }
        }
    } else {
        if (pY >= 0.0f) {
            if (abs_y <= abs_x) {
                return (4.0f - abs_y / abs_x) * 45.0f;
            } else {
                return (abs_x / abs_y + 2.0f) * 45.0f;
            }
        } else {
            if (abs_y <= abs_x) {
                return (abs_y / abs_x + 4.0f) * 45.0f;
            } else {
                return (6.0f - abs_x / abs_y) * 45.0f;
            }
        }
    }
}
C2_HOOK_FUNCTION(0x00511ce0, FastFloatArcTan2)

br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX) {

    return FastFloatArcTan2(pY, pX);
}
C2_HOOK_FUNCTION(0x00511e50, FastScalarArcTan2)

br_angle C2_HOOK_STDCALL FastFloatArcTan2Angle(float pY, float pX) {
    float abs_x;
    float abs_y;

    abs_x = fabsf(pX);
    abs_y = fabsf(pY);
    if (pX == 0.0) {
        if (pY >= 0.0) {
            if (pY <= 0.0) {
                return 0;
            } else {
                return 16380;
            }
        } else {
            return -16396;
        }
    } else if (pX >= 0.0) {
        if (pY >= 0.0) {
            if (abs_y <= (double)abs_x) {
                return (br_angle)(abs_y / abs_x * 8192.0f);
            } else {
                return (br_angle)((2.0 - abs_x / abs_y) * 8192.0f);
            }
        } else if (abs_y <= abs_x) {
            return (br_angle)((8.0 - abs_y / abs_x) * 8192.0f);
        } else {
            return (br_angle)((abs_x / abs_y + 6.0) * 8192.0f);
        }
    } else if (pY >= 0.0) {
        if (abs_y <= abs_x) {
            return (br_angle)((4.0 - abs_y / abs_x) * 8192.0f);
        } else {
            return (br_angle)((abs_x / abs_y + 2.0) * 8192.0f);
        }
    } else if (abs_y <= abs_x) {
        return (br_angle)((abs_y / abs_x + 4.0) * 8192.0f);
    } else {
        return (br_angle)((6.0 - abs_x / abs_y) * 8192.0f);
    }
}
C2_HOOK_FUNCTION(0x00511e70, FastFloatArcTan2Angle)

br_angle C2_HOOK_STDCALL FastScalarArcTan2Angle(br_scalar pY, br_scalar pX) {

    return FastFloatArcTan2Angle(pY, pX);
}
C2_HOOK_FUNCTION(0x00512010, FastScalarArcTan2Angle)
