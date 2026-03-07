#include "matrix23.h"

#include <math.h>

#define A(x, y) A->m[x][y]
#define B(x, y) B->m[x][y]
#define C(x, y) C->m[x][y]
#define M(x, y) mat->m[x][y]


// GLOBAL: CARMA2_HW 0x006b0a20
br_matrix23 mattmp1_23;

// GLOBAL: CARMA2_HW 0x006b0a38
br_matrix23 mattmp2_23;

// FUNCTION: CARMA2_HW 0x00534ae0
void C2_HOOK_CDECL BrMatrix23Copy(br_matrix23* A, const br_matrix23* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
}

// FUNCTION: CARMA2_HW 0x00534b10
void C2_HOOK_CDECL BrMatrix23Mul(br_matrix23* A, const br_matrix23* B, const br_matrix23* C) {

    A(0, 0) = BR_MAC2(B(0, 0), C(0, 0), B(0, 1), C(1, 0));
    A(0, 1) = BR_MAC2(B(0, 0), C(0, 1), B(0, 1), C(1, 1));
    A(1, 0) = BR_MAC2(B(1, 0), C(0, 0), B(1, 1), C(1, 0));
    A(1, 1) = BR_MAC2(B(1, 0), C(0, 1), B(1, 1), C(1, 1));
    A(2, 0) = BR_MAC2(B(2, 0), C(0, 0), B(2, 1), C(1, 0)) + C(2, 0);
    A(2, 1) = BR_MAC2(B(2, 0), C(0, 1), B(2, 1), C(1, 1)) + C(2, 1);
}

// FUNCTION: CARMA2_HW 0x00534ba0
void C2_HOOK_CDECL BrMatrix23Identity(br_matrix23* mat) {

    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
}

// FUNCTION: CARMA2_HW 0x00534bc0
void C2_HOOK_CDECL BrMatrix23Rotate(br_matrix23* mat, br_angle rz) {
    br_scalar s;
    br_scalar c;

    c = BR_COS(rz);
    s = BR_SIN(rz);

    M(0, 0) = c;
    M(0, 1) = s;
    M(1, 0) = -s;
    M(1, 1) = c;
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}

// FUNCTION: CARMA2_HW 0x00534c20
void C2_HOOK_CDECL BrMatrix23Translate(br_matrix23* mat, br_scalar dx, br_scalar dy) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(2, 0) = dx;
    M(2, 1) = dy;
}

// FUNCTION: CARMA2_HW 0x00534c50
void C2_HOOK_CDECL BrMatrix23Scale(br_matrix23* mat, br_scalar sx, br_scalar sy) {

    M(0, 0) = sx;
    M(0, 1) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = sy;
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}

// FUNCTION: CARMA2_HW 0x00534c70
void C2_HOOK_CDECL BrMatrix23ShearX(br_matrix23* mat, br_scalar sy) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = sy;
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}

// FUNCTION: CARMA2_HW 0x00534ca0
void C2_HOOK_CDECL BrMatrix23ShearY(br_matrix23* mat, br_scalar sx) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(1, 0) = sx;
    M(1, 1) = BR_SCALAR(1.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}

// FUNCTION: CARMA2_HW 0x00534cd0
br_scalar C2_HOOK_CDECL BrMatrix23Inverse(br_matrix23* B, const br_matrix23* A) {
    br_scalar det;
    br_scalar idet;
    br_scalar pos;
    br_scalar neg;

    idet = A(0, 0) * A(1, 1);
    if (idet < BR_SCALAR(0.f)) {
        pos = BR_SCALAR(0.f);
        neg = idet;
    } else {
        pos = idet;
        neg = BR_SCALAR(0.f);
    }
    idet = -A(0, 1) * A(1, 0);
    if (idet < BR_SCALAR(0.f)) {
        neg = neg + idet;
    } else {
        pos = pos + idet;
    }
    det = pos + neg;

    if (fabs(det) < 2.384186e-07f) {
        return BR_SCALAR(0.f);
    }
    idet = 1 / det;
    B(0, 0) = A(1, 1) * idet;
    B(0, 1) = -A(0, 1) * idet;
    B(1, 0) = -A(1, 0) * idet;
    B(1, 1) = A(0, 0) * idet;
    B(2, 0) = BR_MAC2(A(1, 0), A(2, 1), -A(2, 0), A(1, 1)) * idet;
    B(2, 1) = BR_MAC2(-A(2, 1), A(0, 0), A(0, 1), A(2, 0)) * idet;
    return det;
}

// FUNCTION: CARMA2_HW 0x00534de0
void C2_HOOK_CDECL BrMatrix23LPInverse(br_matrix23* B, const br_matrix23* A) {

    B(0, 0) = A(1, 1);
    B(0, 1) = -A(0, 1);
    B(1, 0) = -A(1, 0);
    B(1, 1) = A(0, 0);
    B(2, 0) = BR_MAC2(A(1, 0), A(2, 1), -A(2, 0), A(1, 1));
    B(2, 1) = BR_MAC2(-A(2, 1), A(0, 0), A(0, 1), A(2, 0));
}

// FUNCTION: CARMA2_HW 0x00534e30
void C2_HOOK_CDECL BrMatrix23LPNormalise(br_matrix23* A, const br_matrix23* B) {

    br_scalar norm = sqrtf(BR_MAC2(B(1, 0), B(1, 0), B(1, 1), B(1,1)));
    if (norm < 2.384186e-07f) {
        A(1, 0) = BR_SCALAR(1.f);
        A(1, 1) = BR_SCALAR(0.f);
    } else {
        A(1, 0) = B(1, 0) / norm;
        A(1, 1) = B(1, 1) / norm;
    }
    A(0, 0) = A(1, 1);
    A(0, 1) = -A(1, 0);
    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
}

// FUNCTION: CARMA2_HW 0x00534eb0
void C2_HOOK_CDECL BrMatrix23ApplyP(br_vector2* A, const br_vector2* B, const br_matrix23* C) {

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(1, 0)) + C(2, 0);
    A->v[1] = BR_MAC2(B->v[0], C(0, 1), B->v[1], C(1, 1)) + C(2, 1);
}

// FUNCTION: CARMA2_HW 0x00534ef0
void C2_HOOK_CDECL BrMatrix23ApplyV(br_vector2* A, const br_vector2* B, const br_matrix23* C) {

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(1, 0));
    A->v[1] = BR_MAC2(B->v[0], C(0, 1), B->v[1], C(1, 1));
}

// FUNCTION: CARMA2_HW 0x00534f30
void C2_HOOK_CDECL BrMatrix23TApplyP(br_vector2* A, const br_vector2* B, const br_matrix23* C) {

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(0, 1));
    A->v[1] = BR_MAC2(B->v[0], C(1, 0), B->v[1], C(1, 1));
}

// FUNCTION: CARMA2_HW 0x00534f70
void C2_HOOK_CDECL BrMatrix23TApplyV(br_vector2* A, const br_vector2* B, const br_matrix23* C) {

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(0, 1));
    A->v[1] = BR_MAC2(B->v[0], C(1, 0), B->v[1], C(1, 1));
}

// FUNCTION: CARMA2_HW 0x00534fb0
void C2_HOOK_CDECL BrMatrix23Pre(br_matrix23* mat, const br_matrix23* A) {

    BrMatrix23Mul(&mattmp1_23, A, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535000
void C2_HOOK_CDECL BrMatrix23Post(br_matrix23* mat, const br_matrix23* A) {

    BrMatrix23Mul(&mattmp1_23, mat, A);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535050
void C2_HOOK_CDECL BrMatrix23PreRotate(br_matrix23* mat, br_angle rz) {

    BrMatrix23Rotate(&mattmp2_23, rz);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535110
void C2_HOOK_CDECL BrMatrix23PostRotate(br_matrix23* mat, br_angle rz) {

    BrMatrix23Rotate(&mattmp2_23, rz);
    BrMatrix23Mul(&mattmp1_23, mat, &mattmp2_23);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x005351d0
void C2_HOOK_CDECL BrMatrix23PreTranslate(br_matrix23* mat, br_scalar x, br_scalar y) {

    BrMatrix23Translate(&mattmp2_23, x, y);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535260
void C2_HOOK_CDECL BrMatrix23PostTranslate(br_matrix23* A, br_scalar x, br_scalar y) {

    A(2, 0) += x;
    A(2, 1) += y;
}

// FUNCTION: CARMA2_HW 0x00535280
void C2_HOOK_CDECL BrMatrix23PreScale(br_matrix23* mat, br_scalar sx, br_scalar sy) {

    BrMatrix23Scale(&mattmp2_23, sx, sy);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535310
void C2_HOOK_CDECL BrMatrix23PostScale(br_matrix23* mat, br_scalar sx, br_scalar sy) {

    BrMatrix23Scale(&mattmp2_23, sx, sy);
    BrMatrix23Mul(&mattmp1_23, mat, &mattmp2_23);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x005353a0
void C2_HOOK_CDECL BrMatrix23PreShearX(br_matrix23* mat, br_scalar sy) {

    BrMatrix23ShearX(&mattmp2_23, sy);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535430
void C2_HOOK_CDECL BrMatrix23PostShearX(br_matrix23* mat, br_scalar sy) {

    BrMatrix23ShearX(&mattmp2_23, sy);
    BrMatrix23Mul(&mattmp1_23, mat, &mattmp2_23);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x005354c0
void C2_HOOK_CDECL BrMatrix23PreShearY(br_matrix23* mat, br_scalar sx) {

    BrMatrix23ShearY(&mattmp2_23, sx);
    BrMatrix23Mul(&mattmp1_23, &mattmp2_23, mat);
    BrMatrix23Copy(mat, &mattmp1_23);
}

// FUNCTION: CARMA2_HW 0x00535550
void C2_HOOK_CDECL BrMatrix23PostShearY(br_matrix23* mat, br_scalar sx) {

    BrMatrix23ShearY(&mattmp2_23, sx);
    BrMatrix23Mul(&mattmp1_23, mat, &mattmp2_23);
    BrMatrix23Copy(mat, &mattmp1_23);
}