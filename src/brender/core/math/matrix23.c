#include "matrix23.h"

#include <math.h>

#define A(x, y) A->m[x][y]
#define B(x, y) B->m[x][y]
#define C(x, y) C->m[x][y]
#define M(x, y) mat->m[x][y]

C2_HOOK_VARIABLE_IMPLEMENT(br_matrix23, mattmp1_23, 0x006b0a20);
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix23, mattmp2_23, 0x006b0a38);

void C2_HOOK_CDECL BrMatrix23Copy(br_matrix23* A, const br_matrix23* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
}
C2_HOOK_FUNCTION(0x00534ae0, BrMatrix23Copy)

void C2_HOOK_CDECL BrMatrix23Mul(br_matrix23* A, const br_matrix23* B, const br_matrix23* C) {

    A(0, 0) = BR_MAC2(B(0, 0), C(0, 0), B(0, 1), C(1, 0));
    A(0, 1) = BR_MAC2(B(0, 0), C(0, 1), B(0, 1), C(1, 1));
    A(1, 0) = BR_MAC2(B(1, 0), C(0, 0), B(1, 1), C(1, 0));
    A(1, 1) = BR_MAC2(B(1, 0), C(0, 1), B(1, 1), C(1, 1));
    A(2, 0) = BR_MAC2(B(2, 0), C(0, 0), B(2, 1), C(1, 0)) + C(2, 0);
    A(2, 1) = BR_MAC2(B(2, 0), C(0, 1), B(2, 1), C(1, 1)) + C(2, 1);
}
C2_HOOK_FUNCTION(0x00534b10, BrMatrix23Mul)

void C2_HOOK_CDECL BrMatrix23Identity(br_matrix23* mat) {

    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
}
C2_HOOK_FUNCTION(0x00534ba0, BrMatrix23Identity)

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
C2_HOOK_FUNCTION(0x00534bc0, BrMatrix23Rotate)

void C2_HOOK_CDECL BrMatrix23Translate(br_matrix23* mat, br_scalar dx, br_scalar dy) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(2, 0) = dx;
    M(2, 1) = dy;
}
C2_HOOK_FUNCTION(0x00534c20, BrMatrix23Translate)

void C2_HOOK_CDECL BrMatrix23Scale(br_matrix23* mat, br_scalar sx, br_scalar sy) {

    M(0, 0) = sx;
    M(0, 1) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = sy;
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}
C2_HOOK_FUNCTION(0x00534c50, BrMatrix23Scale)

void C2_HOOK_CDECL BrMatrix23ShearX(br_matrix23* mat, br_scalar sy) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = sy;
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}
C2_HOOK_FUNCTION(0x00534c70, BrMatrix23ShearX)

void C2_HOOK_CDECL BrMatrix23ShearY(br_matrix23* mat, br_scalar sx) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(1, 0) = sx;
    M(1, 1) = BR_SCALAR(1.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
}
C2_HOOK_FUNCTION(0x00534ca0, BrMatrix23ShearY)

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
C2_HOOK_FUNCTION(0x00534cd0, BrMatrix23Inverse)

void C2_HOOK_CDECL BrMatrix23LPInverse(br_matrix23* B, const br_matrix23* A) {

    B(0, 0) = A(1, 1);
    B(0, 1) = -A(0, 1);
    B(1, 0) = -A(1, 0);
    B(1, 1) = A(0, 0);
    B(2, 0) = BR_MAC2(A(1, 0), A(2, 1), -A(2, 0), A(1, 1));
    B(2, 1) = BR_MAC2(-A(2, 1), A(0, 0), A(0, 1), A(2, 0));
}
C2_HOOK_FUNCTION(0x00534de0, BrMatrix23LPInverse)

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
C2_HOOK_FUNCTION(0x00534e30, BrMatrix23LPNormalise)

void C2_HOOK_CDECL BrMatrix23ApplyP(br_vector2* A, const br_vector2* B, const br_matrix23* C) {

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(1, 0)) + C(2, 0);
    A->v[1] = BR_MAC2(B->v[0], C(0, 1), B->v[1], C(1, 1)) + C(2, 1);
}
C2_HOOK_FUNCTION(0x00534eb0, BrMatrix23ApplyP)

void C2_HOOK_CDECL BrMatrix23ApplyV(br_vector2* A, const br_vector2* B, const br_matrix23* C) {

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(1, 0));
    A->v[1] = BR_MAC2(B->v[0], C(0, 1), B->v[1], C(1, 1));
}
C2_HOOK_FUNCTION(0x00534ef0, BrMatrix23ApplyV)

void C2_HOOK_CDECL BrMatrix23TApplyP(br_vector2* A, const br_vector2* B, const br_matrix23* C) {

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(0, 1));
    A->v[1] = BR_MAC2(B->v[0], C(1, 0), B->v[1], C(1, 1));
}
C2_HOOK_FUNCTION(0x00534f30, BrMatrix23TApplyP)

void C2_HOOK_CDECL BrMatrix23TApplyV(br_vector2* A, const br_vector2* B, const br_matrix23* C) {

    A->v[0] = BR_MAC2(B->v[0], C(0, 0), B->v[1], C(0, 1));
    A->v[1] = BR_MAC2(B->v[0], C(1, 0), B->v[1], C(1, 1));
}
C2_HOOK_FUNCTION(0x00534f70, BrMatrix23TApplyV)

void C2_HOOK_CDECL BrMatrix23Pre(br_matrix23* mat, const br_matrix23* A) {

    BrMatrix23Mul(&C2V(mattmp1_23), A, mat);
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x00534fb0, BrMatrix23Pre)

void C2_HOOK_CDECL BrMatrix23Post(br_matrix23* mat, const br_matrix23* A) {

    BrMatrix23Mul(&C2V(mattmp1_23), mat, A);
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x00535000, BrMatrix23Post)

void C2_HOOK_CDECL BrMatrix23PreRotate(br_matrix23* mat, br_angle rz) {

    BrMatrix23Rotate(&C2V(mattmp2_23), rz);
    BrMatrix23Mul(&C2V(mattmp1_23), &C2V(mattmp2_23), mat);
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x00535050, BrMatrix23PreRotate)

void C2_HOOK_CDECL BrMatrix23PostRotate(br_matrix23* mat, br_angle rz) {

    BrMatrix23Rotate(&C2V(mattmp2_23), rz);
    BrMatrix23Mul(&C2V(mattmp1_23), mat, &C2V(mattmp2_23));
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x00535110, BrMatrix23PostRotate)

void C2_HOOK_CDECL BrMatrix23PreTranslate(br_matrix23* mat, br_scalar x, br_scalar y) {

    BrMatrix23Translate(&C2V(mattmp2_23), x, y);
    BrMatrix23Mul(&C2V(mattmp1_23), &C2V(mattmp2_23), mat);
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x005351d0, BrMatrix23PreTranslate)

void C2_HOOK_CDECL BrMatrix23PostTranslate(br_matrix23* A, br_scalar x, br_scalar y) {

    A(2, 0) += x;
    A(2, 1) += y;
}
C2_HOOK_FUNCTION(0x00535260, BrMatrix23PostTranslate)

void C2_HOOK_CDECL BrMatrix23PreScale(br_matrix23* mat, br_scalar sx, br_scalar sy) {

    BrMatrix23Scale(&C2V(mattmp2_23), sx, sy);
    BrMatrix23Mul(&C2V(mattmp1_23), &C2V(mattmp2_23), mat);
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x00535280, BrMatrix23PreScale)

void C2_HOOK_CDECL BrMatrix23PostScale(br_matrix23* mat, br_scalar sx, br_scalar sy) {

    BrMatrix23Scale(&C2V(mattmp2_23), sx, sy);
    BrMatrix23Mul(&C2V(mattmp1_23), mat, &C2V(mattmp2_23));
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x00535310, BrMatrix23PostScale)

void C2_HOOK_CDECL BrMatrix23PreShearX(br_matrix23* mat, br_scalar sy) {

    BrMatrix23ShearX(&C2V(mattmp2_23), sy);
    BrMatrix23Mul(&C2V(mattmp1_23), &C2V(mattmp2_23), mat);
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x005353a0, BrMatrix23PreShearX)

void C2_HOOK_CDECL BrMatrix23PostShearX(br_matrix23* mat, br_scalar sy) {

    BrMatrix23ShearX(&C2V(mattmp2_23), sy);
    BrMatrix23Mul(&C2V(mattmp1_23), mat, &C2V(mattmp2_23));
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x00535430, BrMatrix23PostShearX)

void C2_HOOK_CDECL BrMatrix23PreShearY(br_matrix23* mat, br_scalar sx) {

    BrMatrix23ShearY(&C2V(mattmp2_23), sx);
    BrMatrix23Mul(&C2V(mattmp1_23), &C2V(mattmp2_23), mat);
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x005354c0, BrMatrix23PreShearY)

void C2_HOOK_CDECL BrMatrix23PostShearY(br_matrix23* mat, br_scalar sx) {

    BrMatrix23ShearY(&C2V(mattmp2_23), sx);
    BrMatrix23Mul(&C2V(mattmp1_23), mat, &C2V(mattmp2_23));
    BrMatrix23Copy(mat, &C2V(mattmp1_23));
}
C2_HOOK_FUNCTION(0x00535550, BrMatrix23PostShearY)
