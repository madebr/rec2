#include "matrix34.h"

#include "vector.h"

#include <math.h>

C2_HOOK_VARIABLE_IMPLEMENT(br_matrix34, mattmp1, 0x006b09c0);
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix34, mattmp2, 0x006b09f0);

#define M(x, y) mat->m[x][y]
#define A(x, y) A->m[x][y]
#define B(x, y) B->m[x][y]
#define C(x, y) C->m[x][y]
#define BR_MAC3(a, b, c, d, e, f) ((a) * (b) + (c) * (d) + (e) * (f))
#define BR_MAC4(a, b, c, d, e, f, g, h) ((a) * (b) + (c) * (d) + (e) * (f) + (g) * (h))

void C2_HOOK_CDECL BrMatrix34Copy(br_matrix34* A, br_matrix34* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(0, 2) = B(0, 2);

    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(1, 2);

    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
    A(2, 2) = B(2, 2);

    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
}
C2_HOOK_FUNCTION(0x005325d0, BrMatrix34Copy)

void C2_HOOK_CDECL BrMatrix34Mul(br_matrix34* A, br_matrix34* B, br_matrix34* C) {

    A(0, 0) = BR_MAC3(B(0, 0), C(0, 0), B(0, 1), C(1, 0), B(0, 2), C(2, 0));
    A(0, 1) = BR_MAC3(B(0, 0), C(0, 1), B(0, 1), C(1, 1), B(0, 2), C(2, 1));
    A(0, 2) = BR_MAC3(B(0, 0), C(0, 2), B(0, 1), C(1, 2), B(0, 2), C(2, 2));
    A(1, 0) = BR_MAC3(B(1, 0), C(0, 0), B(1, 1), C(1, 0), B(1, 2), C(2, 0));
    A(1, 1) = BR_MAC3(B(1, 0), C(0, 1), B(1, 1), C(1, 1), B(1, 2), C(2, 1));
    A(1, 2) = BR_MAC3(B(1, 0), C(0, 2), B(1, 1), C(1, 2), B(1, 2), C(2, 2));
    A(2, 0) = BR_MAC3(B(2, 0), C(0, 0), B(2, 1), C(1, 0), B(2, 2), C(2, 0));
    A(2, 1) = BR_MAC3(B(2, 0), C(0, 1), B(2, 1), C(1, 1), B(2, 2), C(2, 1));
    A(2, 2) = BR_MAC3(B(2, 0), C(0, 2), B(2, 1), C(1, 2), B(2, 2), C(2, 2));
    A(3, 0) = BR_MAC3(B(3, 0), C(0, 0), B(3, 1), C(1, 0), B(3, 2), C(2, 0)) + C(3, 0);
    A(3, 1) = BR_MAC3(B(3, 0), C(0, 1), B(3, 1), C(1, 1), B(3, 2), C(2, 1)) + C(3, 1);
    A(3, 2) = BR_MAC3(B(3, 0), C(0, 2), B(3, 1), C(1, 2), B(3, 2), C(2, 2)) + C(3, 2);
}
C2_HOOK_FUNCTION(0x00532620, BrMatrix34Mul)

void C2_HOOK_CDECL BrMatrix34Identity(br_matrix34* mat) {
    // { 1, 0, 0},
    // { 0, 1, 0},
    // { 0, 0, 1}
    // ( 0, 0, 0 }
    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;

    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(1, 2) = 0.f;

    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = 1.f;

    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}
C2_HOOK_FUNCTION(0x005327c0, BrMatrix34Identity)

void C2_HOOK_CDECL BrMatrix34RotateX(br_matrix34* mat, br_angle rx) {
    br_scalar s;
    br_scalar c;

    s = BR_SIN(rx);
    c = BR_COS(rx);

    M(0, 0) = 1;
    M(0, 1) = 0;
    M(0, 2) = 0;
    M(1, 0) = 0;
    M(1, 1) = c;
    M(1, 2) = s;
    M(2, 0) = 0;
    M(2, 1) = -s;
    M(2, 2) = c;
    M(3, 0) = 0;
    M(3, 1) = 0;
    M(3, 2) = 0;
}
C2_HOOK_FUNCTION(0x005327f0, BrMatrix34RotateX)

void C2_HOOK_CDECL BrMatrix34RotateY(br_matrix34* mat, br_angle ry) {
    br_scalar s;
    br_scalar c;

    s = BR_SIN(ry);
    c = BR_COS(ry);

    M(0, 0) = c;
    M(0, 1) = 0;
    M(0, 2) = -s;
    M(1, 0) = 0;
    M(1, 1) = 1;
    M(1, 2) = 0;
    M(2, 0) = s;
    M(2, 1) = 0;
    M(2, 2) = c;
    M(3, 0) = 0;
    M(3, 1) = 0;
    M(3, 2) = 0;
}
C2_HOOK_FUNCTION(0x00532860, BrMatrix34RotateY)

void C2_HOOK_CDECL BrMatrix34RotateZ(br_matrix34* mat, br_angle rz) {
    br_scalar s;
    br_scalar c;

    s = BR_SIN(rz);
    c = BR_COS(rz);

    M(0, 0) = c;
    M(0, 1) = s;
    M(0, 2) = 0;
    M(1, 0) = -s;
    M(1, 1) = c;
    M(1, 2) = 0;
    M(2, 0) = 0;
    M(2, 1) = 0;
    M(2, 2) = 1;
    M(3, 0) = 0;
    M(3, 1) = 0;
    M(3, 2) = 0;
}
C2_HOOK_FUNCTION(0x005328d0, BrMatrix34RotateZ)

void C2_HOOK_CDECL BrMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a) {
    br_scalar t;
    br_scalar s;
    br_scalar c;
    br_scalar txy;
    br_scalar txz;
    br_scalar tyz;
    br_scalar sx;
    br_scalar sy;
    br_scalar sz;

    s = BR_SIN(r);
    c = BR_COS(r);
    t = 1.0f - c;

    txy = t * a->v[0];
    txz = txy * a->v[2];
    txy = txy * a->v[1];
    tyz = t * a->v[1] * a->v[2];

    sx = s * a->v[0];
    sy = s * a->v[1];
    sz = s * a->v[2];

    M(0, 0) = t * a->v[0] * a->v[0] + c;
    M(0, 1) = txy + sz;
    M(0, 2) = txz - sy;
    M(1, 0) = txy - sz;
    M(1, 1) = t * a->v[1] * a->v[1] + c;
    M(1, 2) = tyz + sx;
    M(2, 0) = txz + sy;
    M(2, 1) = tyz - sx;
    M(2, 2) = t * a->v[2] * a->v[2] + c;

    M(3, 0) = M(3, 1) = M(3, 2) = 0.0f;
}
C2_HOOK_FUNCTION(0x00532940, BrMatrix34Rotate)

void C2_HOOK_CDECL BrMatrix34Translate(br_matrix34* mat, br_scalar dx, br_scalar dy, br_scalar dz) {

    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(1, 2) = 0.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = 1.f;
    M(3, 0) = dx;
    M(3, 1) = dy;
    M(3, 2) = dz;
}
C2_HOOK_FUNCTION(0x00532a90, BrMatrix34Translate)

void C2_HOOK_CDECL BrMatrix34Scale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    M(0, 0) = sx;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = sy;
    M(1, 2) = 0.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = sz;
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}
C2_HOOK_FUNCTION(0x00532ad0, BrMatrix34Scale)

void C2_HOOK_CDECL BrMatrix34ShearX(br_matrix34* mat, br_scalar sy, br_scalar sz) {

    M(0, 0) = 1.f;
    M(0, 1) = sy;
    M(0, 2) = sz;
    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(1, 2) = 0.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = 1.f;
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}
C2_HOOK_FUNCTION(0x00532b10, BrMatrix34ShearX)

void C2_HOOK_CDECL BrMatrix34ShearY(br_matrix34* mat, br_scalar sx, br_scalar sz) {

    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = sx;
    M(1, 1) = 1.f;
    M(1, 2) = sz;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = 1.f;
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}
C2_HOOK_FUNCTION(0x00532b50, BrMatrix34ShearY)

void C2_HOOK_CDECL BrMatrix34ShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy) {

    M(0, 0) = 1.f;
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = 1.f;
    M(1, 2) = 0.f;
    M(2, 0) = sx;
    M(2, 1) = sy;
    M(2, 2) = 1.f;
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;
}
C2_HOOK_FUNCTION(0x00532b90, BrMatrix34ShearZ)

br_scalar BrMatrix34Inverse(br_matrix34* B, br_matrix34* A) {
    float idet;
    float det;
    float pos;
    float neg;
    float temp;
    float AF[4][3];
    float BF[4][3];
    int i;

#define AF(x, y) (AF[x][y])
#define BF(x, y) (BF[x][y])

#define ACCUMULATE   \
    if (temp >= 0.f) \
        pos += temp; \
    else             \
        neg += temp;
#define PRECISION_LIMIT BR_SCALAR(1.0e-15)
#define ABS(a) (((a) < 0) ? -(a) : (a))

    for (i = 0; i < 4; i++) {
        AF(i, 0) = A(i, 0);
        AF(i, 1) = A(i, 1);
        AF(i, 2) = A(i, 2);
    }

    pos = neg = 0.0F;
    temp = AF(0, 0) * AF(1, 1) * AF(2, 2);
    ACCUMULATE
    temp = AF(0, 1) * AF(1, 2) * AF(2, 0);
    ACCUMULATE
    temp = AF(0, 2) * AF(1, 0) * AF(2, 1);
    ACCUMULATE
    temp = -AF(0, 2) * AF(1, 1) * AF(2, 0);
    ACCUMULATE
    temp = -AF(0, 1) * AF(1, 0) * AF(2, 2);
    ACCUMULATE
    temp = -AF(0, 0) * AF(1, 2) * AF(2, 1);
    ACCUMULATE
    det = pos + neg;

    if (ABS(det) <= PRECISION_LIMIT)
        return 0.f;

    if ((ABS(det / (pos - neg)) < PRECISION_LIMIT)) {
        return 0.f;
    }

    idet = 1.f / det;

    BF(0, 0) = (AF(1, 1) * AF(2, 2) - AF(1, 2) * AF(2, 1)) * idet;
    BF(1, 0) = -(AF(1, 0) * AF(2, 2) - AF(1, 2) * AF(2, 0)) * idet;
    BF(2, 0) = (AF(1, 0) * AF(2, 1) - AF(1, 1) * AF(2, 0)) * idet;
    BF(0, 1) = -(AF(0, 1) * AF(2, 2) - AF(0, 2) * AF(2, 1)) * idet;
    BF(1, 1) = (AF(0, 0) * AF(2, 2) - AF(0, 2) * AF(2, 0)) * idet;
    BF(2, 1) = -(AF(0, 0) * AF(2, 1) - AF(0, 1) * AF(2, 0)) * idet;
    BF(0, 2) = (AF(0, 1) * AF(1, 2) - AF(0, 2) * AF(1, 1)) * idet;
    BF(1, 2) = -(AF(0, 0) * AF(1, 2) - AF(0, 2) * AF(1, 0)) * idet;
    BF(2, 2) = (AF(0, 0) * AF(1, 1) - AF(0, 1) * AF(1, 0)) * idet;

    BF(3, 0) = -(AF(3, 0) * BF(0, 0) + AF(3, 1) * BF(1, 0) + AF(3, 2) * BF(2, 0));
    BF(3, 1) = -(AF(3, 0) * BF(0, 1) + AF(3, 1) * BF(1, 1) + AF(3, 2) * BF(2, 1));
    BF(3, 2) = -(AF(3, 0) * BF(0, 2) + AF(3, 1) * BF(1, 2) + AF(3, 2) * BF(2, 2));

    for (i = 0; i < 4; i++) {
        B(i, 0) = BF(i, 0);
        B(i, 1) = BF(i, 1);
        B(i, 2) = BF(i, 2);
    }

    return det;
}
C2_HOOK_FUNCTION(0x00532bd0, BrMatrix34Inverse)

void C2_HOOK_CDECL BrMatrix34LPInverse(br_matrix34* A, br_matrix34* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(1, 0);
    A(0, 2) = B(2, 0);

    A(1, 0) = B(0, 1);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(2, 1);

    A(2, 0) = B(0, 2);
    A(2, 1) = B(1, 2);
    A(2, 2) = B(2, 2);

    A(3, 0) = -BR_MAC3(B(3, 0), A(0, 0), B(3, 1), A(1, 0), B(3, 2), A(2, 0));
    A(3, 1) = -BR_MAC3(B(3, 0), A(0, 1), B(3, 1), A(1, 1), B(3, 2), A(2, 1));
    A(3, 2) = -BR_MAC3(B(3, 0), A(0, 2), B(3, 1), A(1, 2), B(3, 2), A(2, 2));
}
C2_HOOK_FUNCTION(0x00532eb0, BrMatrix34LPInverse)

void C2_HOOK_CDECL BrMatrix34LPNormalise(br_matrix34* A, br_matrix34* B) {

    BrVector3Normalise((br_vector3*)A->m[2], (br_vector3*)B->m[2]);
    BrVector3Cross((br_vector3*)A->m[0], (br_vector3*)B->m[1], (br_vector3*)A->m[2]);
    BrVector3Normalise((br_vector3*)A->m[0], (br_vector3*)A->m[0]);
    BrVector3Cross((br_vector3*)A->m[1], (br_vector3*)A->m[2], (br_vector3*)A->m[0]);

    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
}
C2_HOOK_FUNCTION(0x00532f60, BrMatrix34LPNormalise)

void C2_HOOK_CDECL BrMatrix34RollingBall(br_matrix34* mat, int dx, int dy, int radius) {
    br_scalar nx;
    br_scalar ny;
    br_scalar ca;
    br_scalar sa;
    br_scalar dr;
    br_scalar h;

    // The rolling ball, Graphics Gems III (1993), pages 51-60, Academic Press

    dr = sqrtf((float)(dx * dx + dy * dy));
    if (dr == BR_SCALAR(.0f)) {
        BrMatrix34Identity(mat);
        return;
    }
    h = sqrtf(dr * dr + radius * radius);
    ca = radius / h;
    sa = dr / h;
    nx = -dy / dr;
    ny = dx / dr;
    // nz = 0;

    h = (1 - ca);

    M(0, 0) = nx * nx * h + ca;
    M(0, 1) = nx * ny * h;
    M(0, 2) = ny * sa;
    M(1, 1) = ca + ny * ny * h;
    M(1, 2) = -nx * sa;
    M(2, 2) = ca;

    M(1, 0) = M(0, 1);
    M(2, 0) = -M(0, 2);
    M(2, 1) = -M(1, 2);

    M(3, 0) = BR_SCALAR(0.f);
    M(3, 1) = BR_SCALAR(0.f);
    M(3, 2) = BR_SCALAR(0.f);
}
C2_HOOK_FUNCTION(0x00533100, BrMatrix34RollingBall)

br_matrix34* BrBoundsToMatrix34(br_matrix34* mat, br_bounds* bounds) {
    int i;
    br_vector3 tr;
    br_vector3 sc;

    for (i = 0; i < 3; ++i) {
        tr.v[i] = 0.5f * bounds->min.v[i] + 0.5f * bounds->max.v[i];
        if (bounds->min.v[i] == bounds->max.v[i]) {
            sc.v[i] = 1.f;
        } else {
            sc.v[i] = 0.5f * bounds->max.v[i] - 0.5f * bounds->min.v[i];
        }
    }

    M(0, 0) = sc.v[0];
    M(0, 1) = 0.f;
    M(0, 2) = 0.f;
    M(1, 0) = 0.f;
    M(1, 1) = sc.v[1];
    M(1, 2) = 0.f;
    M(2, 0) = 0.f;
    M(2, 1) = 0.f;
    M(2, 2) = sc.v[2];
    M(3, 0) = tr.v[0];
    M(3, 1) = tr.v[1];
    M(3, 2) = tr.v[2];
    return mat;
}
C2_HOOK_FUNCTION(0x00533290, BrBoundsToMatrix34)

void C2_HOOK_CDECL BrMatrix34Copy4(br_matrix34* A, br_matrix4* B) {

    A(0, 0) = B(0, 0);
    A(0, 1) = B(0, 1);
    A(0, 2) = B(0, 2);

    A(1, 0) = B(1, 0);
    A(1, 1) = B(1, 1);
    A(1, 2) = B(1, 2);

    A(2, 0) = B(2, 0);
    A(2, 1) = B(2, 1);
    A(2, 2) = B(2, 2);

    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
}
C2_HOOK_FUNCTION(0x00533350, BrMatrix34Copy4)

void C2_HOOK_CDECL BrMatrix34TApplyFV(br_vector3* A, br_fvector3* B, br_matrix34* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
}
C2_HOOK_FUNCTION(0x005333a0, BrMatrix34TApplyFV)

void C2_HOOK_CDECL BrMatrix34Apply(br_vector3* A, br_vector4* B, br_matrix34* C) {

    A->v[0] = BR_MAC4(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0), B->v[3], C(3, 0));
    A->v[1] = BR_MAC4(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1), B->v[3], C(3, 1));
    A->v[2] = BR_MAC4(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2), B->v[3], C(3, 2));
}
C2_HOOK_FUNCTION(0x00533410, BrMatrix34Apply)

void C2_HOOK_CDECL BrMatrix34ApplyP(br_vector3* A, br_vector3* B, br_matrix34* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0)) + C(3, 0);
    A->v[1] = BR_MAC3(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1)) + C(3, 1);
    A->v[2] = BR_MAC3(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2)) + C(3, 2);
}
C2_HOOK_FUNCTION(0x005334a0, BrMatrix34ApplyP)

void C2_HOOK_CDECL BrMatrix34ApplyV(br_vector3* A, br_vector3* B, br_matrix34* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0));
    A->v[1] = BR_MAC3(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1));
    A->v[2] = BR_MAC3(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2));
}
C2_HOOK_FUNCTION(0x00533520, BrMatrix34ApplyV)

void C2_HOOK_CDECL BrMatrix34TApply(br_vector4* A, br_vector4* B, br_matrix34* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
    A->v[3] = BR_MAC3(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2)) + B->v[3];
}
C2_HOOK_FUNCTION(0x00533590, BrMatrix34TApply)

void C2_HOOK_CDECL BrMatrix34TApplyP(br_vector3* A, br_vector3* B, br_matrix34* C) {

    // translation elements are presumed zero or irrelevant
    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
}
C2_HOOK_FUNCTION(0x00533620, BrMatrix34TApplyP)

void C2_HOOK_CDECL BrMatrix34TApplyV(br_vector3* A, br_vector3* B, br_matrix34* C) {

    // translation elements are presumed zero or irrelevant
    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
}
C2_HOOK_FUNCTION(0x00533690, BrMatrix34TApplyV)

void C2_HOOK_CDECL BrMatrix34Pre(br_matrix34* mat, br_matrix34* A) {

    BrMatrix34Mul(&C2V(mattmp1), A, mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533700, BrMatrix34Pre)

void C2_HOOK_CDECL BrMatrix34Post(br_matrix34* mat, br_matrix34* A) {

    BrMatrix34Mul(&C2V(mattmp1), mat, A);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533730, BrMatrix34Post)

void C2_HOOK_CDECL BrMatrix34PreRotateX(br_matrix34* mat, br_angle rx) {

    BrMatrix34RotateX(&C2V(mattmp2), rx);
    BrMatrix34Mul(&C2V(mattmp1), &C2V(mattmp2), mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533760, BrMatrix34PreRotateX)

void C2_HOOK_CDECL BrMatrix34PostRotateX(br_matrix34* mat, br_angle rx) {

    BrMatrix34RotateX(&C2V(mattmp2), rx);
    BrMatrix34Mul(&C2V(mattmp1), mat, &C2V(mattmp2));
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533860, BrMatrix34PostRotateX)

void C2_HOOK_CDECL BrMatrix34PreRotateY(br_matrix34* mat, br_angle ry) {

    BrMatrix34RotateY(&C2V(mattmp2), ry);
    BrMatrix34Mul(&C2V(mattmp1), &C2V(mattmp2), mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533960, BrMatrix34PreRotateY)

void C2_HOOK_CDECL BrMatrix34PostRotateY(br_matrix34* mat, br_angle ry) {

    BrMatrix34RotateY(&C2V(mattmp2), ry);
    BrMatrix34Mul(&C2V(mattmp1), mat, &C2V(mattmp2));
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533a60, BrMatrix34PostRotateY)

void C2_HOOK_CDECL BrMatrix34PreRotateZ(br_matrix34* mat, br_angle rz) {

    BrMatrix34RotateZ(&C2V(mattmp2), rz);
    BrMatrix34Mul(&C2V(mattmp1), &C2V(mattmp2), mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533b60, BrMatrix34PreRotateZ)

void C2_HOOK_CDECL BrMatrix34PostRotateZ(br_matrix34* mat, br_angle rz) {

    BrMatrix34RotateZ(&C2V(mattmp2), rz);
    BrMatrix34Mul(&C2V(mattmp1), mat, &C2V(mattmp2));
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533c50, BrMatrix34PostRotateZ)

void C2_HOOK_CDECL BrMatrix34PreRotate(br_matrix34* mat, br_angle r, br_vector3* axis) {

    BrMatrix34Rotate(&C2V(mattmp2), r, axis);
    BrMatrix34Mul(&C2V(mattmp1), &C2V(mattmp2), mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533d40, BrMatrix34PreRotate)

void C2_HOOK_CDECL BrMatrix34PostRotate(br_matrix34* mat, br_angle r, br_vector3* axis) {

    BrMatrix34Rotate(&C2V(mattmp2), r, axis);
    BrMatrix34Mul(&C2V(mattmp1), mat, &C2V(mattmp2));
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533d80, BrMatrix34PostRotate)

void C2_HOOK_CDECL BrMatrix34PreTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z) {

    BrMatrix34Translate(&C2V(mattmp2), x, y, z);
    BrMatrix34Mul(&C2V(mattmp1), &C2V(mattmp2), mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533dc0, BrMatrix34PreTranslate)

void C2_HOOK_CDECL BrMatrix34PostTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z) {

    M(3, 0) += x;
    M(3, 1) += y;
    M(3, 2) += z;
}
C2_HOOK_FUNCTION(0x00533e60, BrMatrix34PostTranslate)

void C2_HOOK_CDECL BrMatrix34PreScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    BrMatrix34Scale(&C2V(mattmp2), sx, sy, sz);
    BrMatrix34Mul(&C2V(mattmp1), &C2V(mattmp2), mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533e90, BrMatrix34PreScale)

void C2_HOOK_CDECL BrMatrix34PostScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    BrMatrix34Scale(&C2V(mattmp1), sx, sy, sz);
    BrMatrix34Mul(&C2V(mattmp2), mat, &C2V(mattmp1));
    BrMatrix34Copy(mat, &C2V(mattmp2));
}
C2_HOOK_FUNCTION(0x00533f30, BrMatrix34PostScale)

void C2_HOOK_CDECL BrMatrix34PreShearX(br_matrix34* mat, br_scalar sy, br_scalar sz) {

    BrMatrix34ShearX(&C2V(mattmp2), sy, sz);
    BrMatrix34Mul(&C2V(mattmp1), &C2V(mattmp2), mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00533fd0, BrMatrix34PreShearX)

void C2_HOOK_CDECL BrMatrix34PostShearX(br_matrix34* mat, br_scalar sy, br_scalar sz) {

    BrMatrix34ShearX(&C2V(mattmp2), sy, sz);
    BrMatrix34Mul(&C2V(mattmp1), mat, &C2V(mattmp2));
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00534070, BrMatrix34PostShearX)

void C2_HOOK_CDECL BrMatrix34PreShearY(br_matrix34* mat, br_scalar sx, br_scalar sz) {

    BrMatrix34ShearY(&C2V(mattmp2), sx, sz);
    BrMatrix34Mul(&C2V(mattmp1), &C2V(mattmp2), mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00534110, BrMatrix34PreShearY)

void C2_HOOK_CDECL BrMatrix34PostShearY(br_matrix34* mat, br_scalar sx, br_scalar sz) {

    BrMatrix34ShearY(&C2V(mattmp2), sx, sz);
    BrMatrix34Mul(&C2V(mattmp1), mat, &C2V(mattmp2));
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x005341b0, BrMatrix34PostShearY)

void C2_HOOK_CDECL BrMatrix34PreShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy) {

    BrMatrix34ShearZ(&C2V(mattmp2), sx, sy);
    BrMatrix34Mul(&C2V(mattmp1), &C2V(mattmp2), mat);
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x00534250, BrMatrix34PreShearZ)

void C2_HOOK_CDECL BrMatrix34PostShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy) {

    BrMatrix34ShearZ(&C2V(mattmp2), sx, sy);
    BrMatrix34Mul(&C2V(mattmp1), mat, &C2V(mattmp2));
    BrMatrix34Copy(mat, &C2V(mattmp1));
}
C2_HOOK_FUNCTION(0x005342f0, BrMatrix34PostShearZ)
