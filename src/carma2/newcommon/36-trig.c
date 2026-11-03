/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "36-trig.h"

#include "c2_math.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00661398
// GLOBAL: CARMA2_SW 0x0065bcf0
float gFloat_sine_table[91] = {
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
};

// GLOBAL: CARMA2_HW 0x006ab950
br_matrix23 mat23tmp1;

// GLOBAL: CARMA2_HW 0x006ab968
br_matrix23 mat23tmp2;

// GLOBAL: CARMA2_HW 0x006ab980
br_matrix34 mattmp1__trig;

// GLOBAL: CARMA2_HW 0x006ab9b0
br_matrix34 mattmp2__trig;

// FUNCTION: CARMA2_HW 0x005113d0
// FUNCTION: CARMA2_SW 0x0050c820
float C2_HOOK_FASTCALL FastFloatSin(int pAngle_degrees) {

    if (pAngle_degrees < 0) {
        pAngle_degrees = 360 - -pAngle_degrees % 360;
    } else {
        pAngle_degrees = pAngle_degrees % 360;
    }
    if (pAngle_degrees > 270) {
        return -gFloat_sine_table[360 - pAngle_degrees];
    }
    if (pAngle_degrees > 180) {
        return -gFloat_sine_table[pAngle_degrees - 180];
    }
    if (pAngle_degrees > 90) {
        return *(gFloat_sine_table + 90 - (pAngle_degrees - 90));
    }
    return gFloat_sine_table[pAngle_degrees];
}

// FUNCTION: CARMA2_HW 0x00511440
// FUNCTION: CARMA2_SW 0x0050c890
float C2_HOOK_FASTCALL FastFloatCos(int pAngle_degrees) {

    if (pAngle_degrees < 0) {
        pAngle_degrees = 360 - -pAngle_degrees % 360;
    } else {
        pAngle_degrees = pAngle_degrees % 360;
    }

    if (pAngle_degrees > 270) {
        return gFloat_sine_table[pAngle_degrees - 270];
    }
    if (pAngle_degrees > 180) {
        return -gFloat_sine_table[270 - pAngle_degrees];
    }
    if (pAngle_degrees > 90) {
        return -gFloat_sine_table[pAngle_degrees - 90];
    }
    return *(gFloat_sine_table + 90 - pAngle_degrees);
}

// FUNCTION: CARMA2_HW 0x005115b0
// FUNCTION: CARMA2_SW 0x0050ca00
br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_degrees) {

    return FastFloatSin(pAngle_degrees);
}

// FUNCTION: CARMA2_HW 0x00511620
// FUNCTION: CARMA2_SW 0x0050ca70
br_scalar C2_HOOK_FASTCALL FastScalarCos(int pAngle_degrees) {

    return FastFloatCos(pAngle_degrees);
}

// FUNCTION: CARMA2_HW 0x00511780
// FUNCTION: CARMA2_SW 0x0050cbd0
br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pAngle) {

    return FastScalarSin((int)((double)pAngle * (360.0 / 65536.0)));
}

// FUNCTION: CARMA2_HW 0x00511810
// FUNCTION: CARMA2_SW 0x0050cc60
br_scalar C2_HOOK_FASTCALL FastScalarCosAngle(br_angle pAngle) {

    return FastScalarCos((int)((double)pAngle * (360.0 / 65536.0)));
}

// FUNCTION: CARMA2_HW 0x005119c0
// FUNCTION: CARMA2_SW 0x0050ce10
float C2_HOOK_STDCALL FastFloatArcSin(float pValue) {
    float low_limit;
    float high_limit;
    float mid_point;

    C2_HOOK_BUG_ON(REC2_ASIZE(gFloat_sine_table) != 91);

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

        if (gFloat_sine_table[(int)mid_point] > pValue) {
            high_limit = mid_point;
        } else {
            low_limit = mid_point;
        }
    }
    return low_limit;
}

// FUNCTION: CARMA2_HW 0x00511a80
// FUNCTION: CARMA2_SW 0x0050ced0
float C2_HOOK_STDCALL FastFloatArcCos(float pValue) {

    return 90.f - FastFloatArcSin(pValue);
}

// FUNCTION: CARMA2_HW 0x00511b50
// FUNCTION: CARMA2_SW 0x0050cfa0
br_scalar C2_HOOK_STDCALL FastScalarArcSin(br_scalar pValue) {

    return FastFloatArcSin(pValue);
}

// FUNCTION: CARMA2_HW 0x00511c10
// FUNCTION: CARMA2_SW 0x0050d060
br_scalar C2_HOOK_STDCALL FastScalarArcCos(br_scalar pValue) {

    return 90.f - FastFloatArcSin(pValue);
}

// FUNCTION: CARMA2_HW 0x00511ce0
// FUNCTION: CARMA2_SW 0x0050d130
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
// FUNCTION: CARMA2_SW 0x0050d2a0
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

// FUNCTION: CARMA2_HW 0x00512030
void C2_HOOK_FASTCALL DRMatrix34RotateX(br_matrix34* mat, br_angle rx) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rx * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table + 360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table + 270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mat->m[0][0] = 1.0f;
    mat->m[0][1] = 0.0f;
    mat->m[0][2] = 0.0f;
    mat->m[1][0] = 0.0f;
    mat->m[1][1] = c;
    mat->m[1][2] = s;
    mat->m[2][0] = 0.0f;
    mat->m[2][1] = -s;
    mat->m[2][2] = c;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

// FUNCTION: CARMA2_HW 0x00512170
void C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)ry * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table + 360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table + 270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mat->m[0][0] = c;
    mat->m[0][1] = 0.0f;
    mat->m[0][2] = -s;
    mat->m[1][0] = 0.0f;
    mat->m[1][1] = 1.0f;
    mat->m[1][2] = 0.0f;
    mat->m[2][0] = s;
    mat->m[2][1] = 0.0f;
    mat->m[2][2] = c;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

// FUNCTION: CARMA2_HW 0x005122b0
void C2_HOOK_FASTCALL DRMatrix34RotateZ(br_matrix34* mat, br_angle rz) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rz * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table + 360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table + 270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mat->m[0][0] = c;
    mat->m[0][1] = s;
    mat->m[0][2] = 0.0f;
    mat->m[1][0] = -s;
    mat->m[1][1] = c;
    mat->m[1][2] = 0.0f;
    mat->m[2][0] = 0.0f;
    mat->m[2][1] = 0.0f;
    mat->m[2][2] = 1.0f;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

// Needed by DRMatrix34PreRotate/DRMatrix34PostRotate; 36-trig.c owns it.
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x005123f0
void C2_HOOK_FASTCALL DRMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a) {
    br_scalar sz;
    br_scalar sy;
    br_scalar sx;
    br_scalar tyz;
    br_scalar txz;
    br_scalar txy;
    br_scalar c;
    br_scalar s;
    br_scalar t;

    s = FastScalarSinAngle(r);
    c = FastScalarCosAngle(r);
    t = 1.f - c;
    txy = t * a->v[0] * a->v[1];
    txz = t * a->v[0] * a->v[2];
    tyz = t * a->v[1] * a->v[2];
    sx = s * a->v[0];
    sy = s * a->v[1];
    sz = s * a->v[2];

    mat->m[0][0] = a->v[0] * a->v[0] * t + c;
    mat->m[0][1] = sx + txy;
    mat->m[0][2] = txz - sy;
    mat->m[1][0] = txy - sz;
    mat->m[1][1] = a->v[1] * a->v[1] * t + c;
    mat->m[1][2] = sx + tyz;
    mat->m[2][0] = sy + txz;
    mat->m[2][1] = tyz - sx;
    mat->m[2][2] = a->v[2] * a->v[2] * t + c;
    mat->m[3][2] = 0.f;
    mat->m[3][1] = 0.f;
    mat->m[3][0] = 0.f;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x005127b0
void C2_HOOK_FASTCALL DRMatrix34PostRotateX(br_matrix34* mat, br_angle rx) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rx * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table + 360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table + 270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mattmp2__trig.m[0][0] = 1.0f;
    mattmp2__trig.m[0][1] = 0.0f;
    mattmp2__trig.m[0][2] = 0.0f;
    mattmp2__trig.m[1][0] = 0.0f;
    mattmp2__trig.m[1][1] = c;
    mattmp2__trig.m[1][2] = s;
    mattmp2__trig.m[2][0] = 0.0f;
    mattmp2__trig.m[2][1] = -s;
    mattmp2__trig.m[2][2] = c;
    mattmp2__trig.m[3][0] = 0.0f;
    mattmp2__trig.m[3][1] = 0.0f;
    mattmp2__trig.m[3][2] = 0.0f;
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// STUB: CARMA2_HW 0x00512ad0
// STUB: CARMA2_SW 0x0050df10
void C2_HOOK_FASTCALL DRMatrix34PostRotateY(br_matrix34* mat, br_angle ry) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00512c60
void C2_HOOK_FASTCALL DRMatrix34PreRotateZ(br_matrix34* mat, br_angle rz) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rz * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table + 360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table + 270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mattmp2__trig.m[0][0] = c;
    mattmp2__trig.m[0][1] = s;
    mattmp2__trig.m[0][2] = 0.0f;
    mattmp2__trig.m[1][0] = -s;
    mattmp2__trig.m[1][1] = c;
    mattmp2__trig.m[1][2] = 0.0f;
    mattmp2__trig.m[2][0] = 0.0f;
    mattmp2__trig.m[2][1] = 0.0f;
    mattmp2__trig.m[2][2] = 1.0f;
    mattmp2__trig.m[3][0] = 0.0f;
    mattmp2__trig.m[3][1] = 0.0f;
    mattmp2__trig.m[3][2] = 0.0f;
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512df0
void C2_HOOK_FASTCALL DRMatrix34PostRotateZ(br_matrix34* mat, br_angle rz) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rz * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table + 360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table + 270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mattmp2__trig.m[0][0] = c;
    mattmp2__trig.m[0][1] = s;
    mattmp2__trig.m[0][2] = 0.0f;
    mattmp2__trig.m[1][0] = -s;
    mattmp2__trig.m[1][1] = c;
    mattmp2__trig.m[1][2] = 0.0f;
    mattmp2__trig.m[2][0] = 0.0f;
    mattmp2__trig.m[2][1] = 0.0f;
    mattmp2__trig.m[2][2] = 1.0f;
    mattmp2__trig.m[3][0] = 0.0f;
    mattmp2__trig.m[3][1] = 0.0f;
    mattmp2__trig.m[3][2] = 0.0f;
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512f80
void C2_HOOK_FASTCALL DRMatrix34PreRotate(br_matrix34* mat, br_angle r, br_vector3* axis) {

    DRMatrix34Rotate(&mattmp2__trig, r, axis);
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// DRMatrix34PostRotate

// DRMatrix23Rotate

// DRMatrix23PostRotate

