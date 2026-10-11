#include "36-trig.h"

// GLOBAL: CARMA2_HW 0x00661398
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

// FUNCTION: CARMA2_HW 0x005113d0
float C2_HOOK_FASTCALL FastFloatSin(int pAngle_in_degrees) {

    if (pAngle_in_degrees >= 0) {
        pAngle_in_degrees = pAngle_in_degrees % 360;
    } else {
        pAngle_in_degrees = 360 - (int)(0u - (unsigned int)pAngle_in_degrees) % 360;
    }
    if (pAngle_in_degrees > 270) {
        return -gFloat_sine_table[360 - pAngle_in_degrees];
    }
    if (pAngle_in_degrees > 180) {
        return -gFloat_sine_table[pAngle_in_degrees - 180];
    }
    if (pAngle_in_degrees > 90) {
        return gFloat_sine_table[180 - pAngle_in_degrees];
    }
    return gFloat_sine_table[pAngle_in_degrees];
}

// FUNCTION: CARMA2_HW 0x00511440
float C2_HOOK_FASTCALL FastFloatCos(int pAngle_in_degrees) {

    if (pAngle_in_degrees >= 0) {
        pAngle_in_degrees = pAngle_in_degrees % 360;
    } else {
        pAngle_in_degrees = 360 - (int)(0u - (unsigned int)pAngle_in_degrees) % 360;
    }

    if (pAngle_in_degrees > 270) {
        return gFloat_sine_table[pAngle_in_degrees - 270];
    }
    if (pAngle_in_degrees > 180) {
        return -gFloat_sine_table[270 - pAngle_in_degrees];
    }
    if (pAngle_in_degrees > 90) {
        return -gFloat_sine_table[pAngle_in_degrees - 90];
    }
    return gFloat_sine_table[90 - pAngle_in_degrees];
}

// FUNCTION: CARMA2_HW 0x005115b0
br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees) {

    return FastFloatSin((int)pAngle_in_degrees);
}

// FUNCTION: CARMA2_HW 0x00511620
br_scalar C2_HOOK_FASTCALL FastScalarCos(int pAngle_in_degrees) {

    return FastFloatCos(pAngle_in_degrees);
}

// FUNCTION: CARMA2_HW 0x00511780
br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pBR_angle) {

    return FastScalarSin((int)((double)pBR_angle * (360.0 / 65536.0)));
}

// FUNCTION: CARMA2_HW 0x00511810
br_scalar C2_HOOK_FASTCALL FastScalarCosAngle(br_angle pBR_angle) {

    return FastScalarCos((int)((double)pBR_angle * (360.0 / 65536.0)));
}

// FastFloatArcTan2

// FastScalarArcTan2

// FastFloatArcTan2Angle

// FastScalarArcTan2Angle

// FUNCTION: CARMA2_HW 0x00512030
void C2_HOOK_FASTCALL DRMatrix34RotateX(br_matrix34* mat, br_angle rx) {
    br_scalar s;
    br_scalar c;

    s = FastScalarSinAngle(rx);
    c = FastScalarCosAngle(rx);
    mat->m[0][0] = 1.0f;
    mat->m[0][1] = 0.0f;
    mat->m[0][2] = 0.0f;
    mat->m[1][0] = 0.0f;
    mat->m[1][1] = c;
    mat->m[1][2] = s;
    mat->m[2][0] = 0.0;
    mat->m[2][1] = -s;
    mat->m[2][2] = c;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

// FUNCTION: CARMA2_HW 0x00512170
void C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry) {
    br_scalar s;
    br_scalar c;

    s = FastScalarSinAngle(ry);
    c = FastScalarCosAngle(ry);
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
    br_scalar s;
    br_scalar c;

    s = FastScalarSinAngle(rz);
    c = FastScalarCosAngle(rz);
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

// DRMatrix34PostRotateX

// DRMatrix34PostRotateY

// DRMatrix34PreRotateZ

// DRMatrix34PostRotateZ

// FUNCTION: CARMA2_HW 0x00513020
void C2_HOOK_FASTCALL DRMatrix23Rotate(br_matrix23* mat, br_angle rz) {
    br_scalar s;
    br_scalar c;

    s = FastScalarSinAngle(rz);
    c = FastScalarCosAngle(rz);

    mat->m[0][0] = c;
    mat->m[0][1] = s;
    mat->m[1][0] = -s;
    mat->m[1][1] = c;
    mat->m[2][0] = 0.f;
    mat->m[2][1] = 0.f;
}

// DRMatrix23PostRotate

