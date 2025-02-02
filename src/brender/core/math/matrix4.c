#include "matrix4.h"

#include <math.h>
#include "c2_string.h"

#define A(x, y) A->m[x][y]
#define B(x, y) B->m[x][y]
#define C(x, y) C->m[x][y]
#define M(x, y) mat->m[x][y]


void C2_HOOK_CDECL BrMatrix4Copy(br_matrix4* A, const br_matrix4* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(0, 2) = B(0, 2);
    A(0, 3) = B(0, 3);
    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(1, 2);
    A(1, 3) = B(1, 3);
    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
    A(2, 2) = B(2, 2);
    A(2, 3) = B(2, 3);
    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
    A(3, 3) = B(3, 3);
}
C2_HOOK_FUNCTION(0x00535cd0, BrMatrix4Copy)

void C2_HOOK_CDECL BrMatrix4Mul(br_matrix4* A, const br_matrix4* B, const br_matrix4* C) {

    A(0, 0) = BR_MAC4(B(0, 0), C(0, 0), B(0, 1), C(1, 0), B(0, 2), C(2, 0), B(0, 3), C(3, 0));
    A(0, 1) = BR_MAC4(B(0, 0), C(0, 1), B(0, 1), C(1, 1), B(0, 2), C(2, 1), B(0, 3), C(3, 1));
    A(0, 2) = BR_MAC4(B(0, 0), C(0, 2), B(0, 1), C(1, 2), B(0, 2), C(2, 2), B(0, 3), C(3, 2));
    A(0, 3) = BR_MAC4(B(0, 0), C(0, 3), B(0, 1), C(1, 3), B(0, 2), C(2, 3), B(0, 3), C(3, 3));
    A(1, 0) = BR_MAC4(B(1, 0), C(0, 0), B(1, 1), C(1, 0), B(1, 2), C(2, 0), B(1, 3), C(3, 0));
    A(1, 1) = BR_MAC4(B(1, 0), C(0, 1), B(1, 1), C(1, 1), B(1, 2), C(2, 1), B(1, 3), C(3, 1));
    A(1, 2) = BR_MAC4(B(1, 0), C(0, 2), B(1, 1), C(1, 2), B(1, 2), C(2, 2), B(1, 3), C(3, 2));
    A(1, 3) = BR_MAC4(B(1, 0), C(0, 3), B(1, 1), C(1, 3), B(1, 2), C(2, 3), B(1, 3), C(3, 3));
    A(2, 0) = BR_MAC4(B(2, 0), C(0, 0), B(2, 1), C(1, 0), B(2, 2), C(2, 0), B(2, 3), C(3, 0));
    A(2, 1) = BR_MAC4(B(2, 0), C(0, 1), B(2, 1), C(1, 1), B(2, 2), C(2, 1), B(2, 3), C(3, 1));
    A(2, 2) = BR_MAC4(B(2, 0), C(0, 2), B(2, 1), C(1, 2), B(2, 2), C(2, 2), B(2, 3), C(3, 2));
    A(2, 3) = BR_MAC4(B(2, 0), C(0, 3), B(2, 1), C(1, 3), B(2, 2), C(2, 3), B(2, 3), C(3, 3));
    A(3, 0) = BR_MAC4(B(3, 0), C(0, 0), B(3, 1), C(1, 0), B(3, 2), C(2, 0), B(3, 3), C(3, 0));
    A(3, 1) = BR_MAC4(B(3, 0), C(0, 1), B(3, 1), C(1, 1), B(3, 2), C(2, 1), B(3, 3), C(3, 1));
    A(3, 2) = BR_MAC4(B(3, 0), C(0, 2), B(3, 1), C(1, 2), B(3, 2), C(2, 2), B(3, 3), C(3, 2));
    A(3, 3) = BR_MAC4(B(3, 0), C(0, 3), B(3, 1), C(1, 3), B(3, 2), C(2, 3), B(3, 3), C(3, 3));
}
C2_HOOK_FUNCTION(0x00535d40, BrMatrix4Mul)

void C2_HOOK_CDECL BrMatrix4Identity(br_matrix4* mat) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(0, 2) = BR_SCALAR(0.f);
    M(0, 3) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(1, 2) = BR_SCALAR(0.f);
    M(1, 3) = BR_SCALAR(0.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
    M(2, 2) = BR_SCALAR(1.f);
    M(2, 3) = BR_SCALAR(0.f);
    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 2) = BR_SCALAR(0.f);
    M(3, 3) = BR_SCALAR(1.f);
}
C2_HOOK_FUNCTION(0x00535ff0, BrMatrix4Identity)

void C2_HOOK_CDECL BrMatrix4Scale(br_matrix4* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    M(0, 0) = sx;
    M(1, 1) = sy;
    M(2, 2) = sz;
    M(0, 1) = BR_SCALAR(0.f);
    M(0, 2) = BR_SCALAR(0.f);
    M(0, 3) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 2) = BR_SCALAR(0.f);
    M(1, 3) = BR_SCALAR(0.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
    M(2, 3) = BR_SCALAR(0.f);
    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 2) = BR_SCALAR(0.f);
    M(3, 3) = BR_SCALAR(1.f);
}
C2_HOOK_FUNCTION(0x00536030, BrMatrix4Scale)

br_scalar C2_HOOK_CDECL BrMatrix4Inverse(br_matrix4* A, const br_matrix4* B) {
    int i;
    int j;
    br_scalar det;
    br_scalar idet;

    BrMatrix4Adjoint(A, B);
    det = BrMatrix4Determinant(B);

    if (fabs(det) < BR_SCALAR_EPSILON * 2)
        return 0;

    idet = 1.0f / det;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            A(i, j) = A(i, j) * idet;
        }
    }

    return det;
}
C2_HOOK_FUNCTION(0x00536080, BrMatrix4Inverse)

br_scalar C2_HOOK_STDCALL Determinant3(br_scalar a1, br_scalar a2, br_scalar a3, br_scalar b1, br_scalar b2, br_scalar b3, br_scalar c1, br_scalar c2, br_scalar c3) {

    return BR_MAC3(a1, BR_MAC2(b2, c3, -b3, c2),
        -b1, BR_MAC2(a2, c3, -a3, c2),
        c1, BR_MAC2(a2, b3, -a3, b2));
}
C2_HOOK_FUNCTION(0x005367b0, Determinant3)

br_scalar C2_HOOK_CDECL BrMatrix4Determinant(const br_matrix4* mat) {
    br_scalar a1;
    br_scalar a2;
    br_scalar a3;
    br_scalar a4;
    br_scalar b1;
    br_scalar b2;
    br_scalar b3;
    br_scalar b4;
    br_scalar c1;
    br_scalar c2;
    br_scalar c3;
    br_scalar c4;
    br_scalar d1;
    br_scalar d2;
    br_scalar d3;
    br_scalar d4;

    a1 = M(0, 0);
    b1 = M(0, 1);
    c1 = M(0, 2);
    d1 = M(0, 3);

    a2 = M(1, 0);
    b2 = M(1, 1);
    c2 = M(1, 2);
    d2 = M(1, 3);

    a3 = M(2, 0);
    b3 = M(2, 1);
    c3 = M(2, 2);
    d3 = M(2, 3);

    a4 = M(3, 0);
    b4 = M(3, 1);
    c4 = M(3, 2);
    d4 = M(3, 3);

    return BR_MAC4(a1, Determinant3(b2, b3, b4, c2, c3, c4, d2, d3, d4),
        -b1, Determinant3(a2, a3, a4, c2, c3, c4, d2, d3, d4),
        c1, Determinant3(a2, a3, a4, b2, b3, b4, d2, d3, d4),
        -d1, Determinant3(a2, a3, a4, b2, b3, b4, c2, c3, c4));
}
C2_HOOK_FUNCTION(0x005360f0, BrMatrix4Determinant)

void C2_HOOK_CDECL BrMatrix4Adjoint(br_matrix4* A, const br_matrix4* B) {
    br_scalar a1;
    br_scalar a2;
    br_scalar a3;
    br_scalar a4;
    br_scalar b1;
    br_scalar b2;
    br_scalar b3;
    br_scalar b4;
    br_scalar c1;
    br_scalar c2;
    br_scalar c3;
    br_scalar c4;
    br_scalar d1;
    br_scalar d2;
    br_scalar d3;
    br_scalar d4;

    a1 = B(0, 0);
    b1 = B(0, 1);
    c1 = B(0, 2);
    d1 = B(0, 3);

    a2 = B(1, 0);
    b2 = B(1, 1);
    c2 = B(1, 2);
    d2 = B(1, 3);

    a3 = B(2, 0);
    b3 = B(2, 1);
    c3 = B(2, 2);
    d3 = B(2, 3);

    a4 = B(3, 0);
    b4 = B(3, 1);
    c4 = B(3, 2);
    d4 = B(3, 3);

    /* row column labeling reversed since we transpose rows & columns */

    A(0, 0) = Determinant3(b2, b3, b4, c2, c3, c4, d2, d3, d4);
    A(1, 0) = -Determinant3(a2, a3, a4, c2, c3, c4, d2, d3, d4);
    A(2, 0) = Determinant3(a2, a3, a4, b2, b3, b4, d2, d3, d4);
    A(3, 0) = -Determinant3(a2, a3, a4, b2, b3, b4, c2, c3, c4);

    A(0, 1) = -Determinant3(b1, b3, b4, c1, c3, c4, d1, d3, d4);
    A(1, 1) = Determinant3(a1, a3, a4, c1, c3, c4, d1, d3, d4);
    A(2, 1) = -Determinant3(a1, a3, a4, b1, b3, b4, d1, d3, d4);
    A(3, 1) = Determinant3(a1, a3, a4, b1, b3, b4, c1, c3, c4);

    A(0, 2) = Determinant3(b1, b2, b4, c1, c2, c4, d1, d2, d4);
    A(1, 2) = -Determinant3(a1, a2, a4, c1, c2, c4, d1, d2, d4);
    A(2, 2) = Determinant3(a1, a2, a4, b1, b2, b4, d1, d2, d4);
    A(3, 2) = -Determinant3(a1, a2, a4, b1, b2, b4, c1, c2, c4);

    A(0, 3) = -Determinant3(b1, b2, b3, c1, c2, c3, d1, d2, d3);
    A(1, 3) = Determinant3(a1, a2, a3, c1, c2, c3, d1, d2, d3);
    A(2, 3) = -Determinant3(a1, a2, a3, b1, b2, b3, d1, d2, d3);
    A(3, 3) = Determinant3(a1, a2, a3, b1, b2, b3, c1, c2, c3);
}
C2_HOOK_FUNCTION(0x005362c0, BrMatrix4Adjoint)

void C2_HOOK_CDECL BrMatrix4Perspective(br_matrix4* mat, br_angle field_of_view, br_scalar aspect, br_scalar hither, br_scalar yon) {
    br_scalar scale;

    scale = BR_COS(field_of_view / 2) / BR_SIN(field_of_view / 2);

    M(0, 0) = scale / aspect;
    M(1, 1) = scale;
    M(2, 2) = (yon + hither) / (yon - hither);
    M(3, 2) = BR_SCALAR(-2.f) * ((yon * hither) / (yon - hither));

    M(0, 1) = BR_SCALAR(0.f);
    M(0, 2) = BR_SCALAR(0.f);
    M(0, 3) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 2) = BR_SCALAR(0.f);
    M(1, 3) = BR_SCALAR(0.f);
    M(2, 0) = BR_SCALAR(0.f);
    M(2, 1) = BR_SCALAR(0.f);
    M(2, 3) = BR_SCALAR(-1.f);
    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 3) = BR_SCALAR(0.f);
}
C2_HOOK_FUNCTION(0x00536820, BrMatrix4Perspective)

void C2_HOOK_CDECL BrMatrix4Apply(br_vector4* A, const br_vector4* B, const br_matrix4* C) {

    A->v[0] = BR_MAC4(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0), B->v[3], C(3, 0));
    A->v[1] = BR_MAC4(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1), B->v[3], C(3, 1));
    A->v[2] = BR_MAC4(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2), B->v[3], C(3, 2));
    A->v[3] = BR_MAC4(B->v[0], C(0, 3), B->v[1], C(1, 3), B->v[2], C(2, 3), B->v[3], C(3, 3));
}
C2_HOOK_FUNCTION(0x005368e0, BrMatrix4Apply)

void C2_HOOK_CDECL BrMatrix4ApplyP(br_vector4* A, const br_vector3* B, const br_matrix4* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0)) + C(3, 0);
    A->v[1] = BR_MAC3(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1)) + C(3, 1);
    A->v[2] = BR_MAC3(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2)) + C(3, 2);
    A->v[3] = BR_MAC3(B->v[0], C(0, 3), B->v[1], C(1, 3), B->v[2], C(2, 3)) + C(3, 3);
}
C2_HOOK_FUNCTION(0x00536990, BrMatrix4ApplyP)

void C2_HOOK_CDECL BrMatrix4ApplyV(br_vector4* A, const br_vector3* B, const br_matrix4* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0));
    A->v[1] = BR_MAC3(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1));
    A->v[2] = BR_MAC3(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2));
    A->v[3] = BR_MAC3(B->v[0], C(0, 3), B->v[1], C(1, 3), B->v[2], C(2, 3));
}
C2_HOOK_FUNCTION(0x00536a30, BrMatrix4ApplyV)

void C2_HOOK_CDECL BrMatrix4TApply(br_vector4* A, const br_vector4* B, const br_matrix4* C) {

    A->v[0] = BR_MAC4(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2), B->v[3], C(0, 3));
    A->v[1] = BR_MAC4(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2), B->v[3], C(1, 3));
    A->v[2] = BR_MAC4(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2), B->v[3], C(2, 3));
    A->v[3] = BR_MAC4(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2), B->v[3], C(3, 3));
}
C2_HOOK_FUNCTION(0x00536ac0, BrMatrix4TApply)

void C2_HOOK_CDECL BrMatrix4TApplyP(br_vector4* A, const br_vector3* B, const br_matrix4* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2)) + C(0, 3);
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2)) + C(1, 3);
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2)) + C(2, 3);
    A->v[3] = BR_MAC3(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2)) + C(3, 3);
}
C2_HOOK_FUNCTION(0x00536b70, BrMatrix4TApplyP)

void C2_HOOK_CDECL BrMatrix4TApplyV(br_vector4* A, const br_vector3* B, const br_matrix4* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
    A->v[3] = BR_MAC3(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2));
}
C2_HOOK_FUNCTION(0x00536c10, BrMatrix4TApplyV)

void C2_HOOK_CDECL BrMatrix4Copy34(br_matrix4* A, const br_matrix34* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(0, 2) = B(0, 2);
    A(0, 3) = BR_SCALAR(0.f);
    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(1, 2);
    A(1, 3) = BR_SCALAR(0.f);
    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
    A(2, 2) = B(2, 2);
    A(2, 3) = BR_SCALAR(0.f);
    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
    A(3, 3) = BR_SCALAR(1.f);
}
C2_HOOK_FUNCTION(0x00536ca0, BrMatrix4Copy34)

void C2_HOOK_CDECL BrMatrix4Mul34(br_matrix4* A, const br_matrix34* B, const br_matrix4* C) {

    A(0, 0) = BR_MAC3(B(0, 0), C(0, 0), B(0, 1), C(1, 0), B(0, 2), C(2, 0));
    A(0, 1) = BR_MAC3(B(0, 0), C(0, 1), B(0, 1), C(1, 1), B(0, 2), C(2, 1));
    A(0, 2) = BR_MAC3(B(0, 0), C(0, 2), B(0, 1), C(1, 2), B(0, 2), C(2, 2));
    A(0, 3) = BR_MAC3(B(0, 0), C(0, 3), B(0, 1), C(1, 3), B(0, 2), C(2, 3));
    A(1, 0) = BR_MAC3(B(1, 0), C(0, 0), B(1, 1), C(1, 0), B(1, 2), C(2, 0));
    A(1, 1) = BR_MAC3(B(1, 0), C(0, 1), B(1, 1), C(1, 1), B(1, 2), C(2, 1));
    A(1, 2) = BR_MAC3(B(1, 0), C(0, 2), B(1, 1), C(1, 2), B(1, 2), C(2, 2));
    A(1, 3) = BR_MAC3(B(1, 0), C(0, 3), B(1, 1), C(1, 3), B(1, 2), C(2, 3));
    A(2, 0) = BR_MAC3(B(2, 0), C(0, 0), B(2, 1), C(1, 0), B(2, 2), C(2, 0));
    A(2, 1) = BR_MAC3(B(2, 0), C(0, 1), B(2, 1), C(1, 1), B(2, 2), C(2, 1));
    A(2, 2) = BR_MAC3(B(2, 0), C(0, 2), B(2, 1), C(1, 2), B(2, 2), C(2, 2));
    A(2, 3) = BR_MAC3(B(2, 0), C(0, 3), B(2, 1), C(1, 3), B(2, 2), C(2, 3));
    A(3, 0) = BR_MAC3(B(3, 0), C(0, 0), B(3, 1), C(1, 0), B(3, 2), C(2, 0)) + C(3, 0);
    A(3, 1) = BR_MAC3(B(3, 0), C(0, 1), B(3, 1), C(1, 1), B(3, 2), C(2, 1)) + C(3, 1);
    A(3, 2) = BR_MAC3(B(3, 0), C(0, 2), B(3, 1), C(1, 2), B(3, 2), C(2, 2)) + C(3, 2);
    A(3, 3) = BR_MAC3(B(3, 0), C(0, 3), B(3, 1), C(1, 3), B(3, 2), C(2, 3)) + C(3, 3);
}
C2_HOOK_FUNCTION(0x00536d10, BrMatrix4Mul34)

void C2_HOOK_CDECL BrMatrix4Pre34(br_matrix4* A, const br_matrix34* B) {
    br_matrix4 C;

    c2_memcpy(&C, A, sizeof(*A));
    BrMatrix4Mul34(A, B, &C);
}
C2_HOOK_FUNCTION(0x00536f30, BrMatrix4Pre34)

void C2_HOOK_CDECL BrMatrix4ShearZ(br_matrix4* mat, br_scalar sx, br_scalar sy) {

    M(0, 0) = BR_SCALAR(1.f);
    M(0, 1) = BR_SCALAR(0.f);
    M(0, 2) = BR_SCALAR(0.f);
    M(0, 3) = BR_SCALAR(0.f);
    M(1, 0) = BR_SCALAR(0.f);
    M(1, 1) = BR_SCALAR(1.f);
    M(1, 2) = BR_SCALAR(0.f);
    M(1, 3) = BR_SCALAR(0.f);
    M(2, 0) = sx;
    M(2, 1) = sy;
    M(2, 2) = BR_SCALAR(1.f);
    M(2, 3) = BR_SCALAR(0.f);
    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 2) = BR_SCALAR(0.f);
    M(3, 3) = BR_SCALAR(1.f);
}
C2_HOOK_FUNCTION(0x00536f60, BrMatrix4ShearZ)
