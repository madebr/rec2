#include "quat.h"

#include "matrix34.h"
#include "matrix4.h"

#include <math.h>

#define M(x, y) mat->m[x][y]
#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])
#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])

br_quat* C2_HOOK_CDECL BrQuatMul(br_quat* q, const br_quat* l, const br_quat* r) {
    br_scalar x1;
    br_scalar x2;
    br_scalar x3;
    br_scalar x4;
    br_scalar s;

    x1 = (l->x + l->z) * (r->y + r->x);
    x2 = (l->w + l->y) * (r->w - r->z);
    x3 = (l->w - l->y) * (r->w + r->z);
    s = x1 + x2 + x3;
    x4 = ((l->z - l->x) * (r->x - r->y) + s) * 0.5f;
    q->x = x4 + (l->w + l->x) * (r->w + r->x) - s;
    q->y = x4 + (l->w - l->x) * (r->z + r->y) - x3;
    q->z = x4 + (l->y + l->z) * (r->w - r->x) - x2;
    q->w = x4 + (l->z - l->y) * (r->y - r->z) - x1;
    return q;
}
C2_HOOK_FUNCTION(0x005355e0, BrQuatMul)

br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq) {
    br_scalar s;

    s = 1.f / sqrtf(qq->x * qq->x + qq->y * qq->y + qq->z * qq->z + qq->w * qq->w);
    q->x = qq->x * s;
    q->y = qq->y * s;
    q->z = qq->z * s;
    q->w = qq->w * s;
    return q;
}
C2_HOOK_FUNCTION(0x00535710, BrQuatNormalise)

br_quat* C2_HOOK_CDECL BrQuatInvert(br_quat* q, const br_quat* qq) {

    q->x = -qq->x;
    q->y = -qq->y;
    q->z = -qq->z;
    q->w = qq->w;
    return q;
}
C2_HOOK_FUNCTION(0x00535790, BrQuatInvert)

br_quat* C2_HOOK_CDECL BrQuatSlerp(br_quat* q, const br_quat* l, const br_quat* r, br_scalar a, br_int_16 spins) {
    int omega;
    int omega_spin;
    br_scalar s_omega;
    br_scalar c_omega;
    br_scalar scale_l;
    br_scalar scale_r;
    br_quat t;

    // Animating rotation with quaternion curves, Ken Shoemake,SIGGRAPH Computer Graphics, 19(3):245–254, 1985. doi:10.1145/325165.325242.
    c_omega = l->x * r->x + l->y * r->y + l->z * r->z + l->w * r->w;
    if (c_omega >= 0.f) {
        t.x = r->x;
        t.y = r->y;
        t.z = r->z;
        t.w = r->w;
    } else {
        t.x = -r->x;
        t.y = -r->y;
        t.z = -r->z;
        t.w = -r->w;
        c_omega = -c_omega;
    }

    if ((1.f - c_omega) <= BR_SCALAR_EPSILON) {
        scale_l = 1.f - a;
        scale_r = a;
    } else {
        omega = BrRadianToAngle(acosf(c_omega));
        s_omega = sinf(BrAngleToRadian(omega));
        omega_spin = omega + spins * BR_ANGLE_RAD(PI);
        scale_l = sinf(BrAngleToRadian(omega - a * omega_spin)) / s_omega;
        scale_r = sinf(BrAngleToRadian(a * omega_spin)) / s_omega;
    }
    q->x = scale_l * l->x + scale_r * t.x;
    q->y = scale_l * l->y + scale_r * t.y;
    q->z = scale_l * l->z + scale_r * t.z;
    q->w = scale_l * l->w + scale_r * t.w;
    return q;
}
C2_HOOK_FUNCTION(0x005357c0, BrQuatSlerp)

br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q) {
    br_scalar xs;
    br_scalar ys;
    br_scalar zs;
    br_scalar wx;
    br_scalar wy;
    br_scalar wz;
    br_scalar xx;
    br_scalar xy;
    br_scalar xz;
    br_scalar yy;
    br_scalar yz;
    br_scalar zz;

    zz = q->z + q->z;
    yy = q->y + q->y;
    zs = q->z * zz;
    xx = q->x + q->x;
    wx = q->w * xx;
    wy = q->w * yy;
    wz = q->w * zz;
    xy = q->x * yy;
    xz = q->x * zz;
    ys = q->y * yy;
    xs = q->x * xx;
    yz = q->y * zz;

    M(0, 0) = 1.f - (ys + zs);
    M(1, 0) = xy - wz;
    M(2, 0) = xz + wy;
    M(0, 1) = xy + wz;
    M(1, 1) = 1.f - (zs + xs);
    M(2, 1) = yz - wx;
    M(0, 2) = xz - wy;
    M(1, 2) = yz + wx;
    M(2, 2) = 1.f - (xs + ys);
    M(3, 0) = 0.f;
    M(3, 1) = 0.f;
    M(3, 2) = 0.f;

    return mat;
}
C2_HOOK_FUNCTION(0x005359a0, BrQuatToMatrix34)

br_quat* C2_HOOK_CDECL BrMatrix34ToQuat(br_quat* q, const br_matrix34* mat) {
    br_scalar tr;
    br_scalar s;
    int i;
    int j;
    int k;

    tr = M(0, 0) + M(1, 1) + M(2, 2);
    if (tr >= 0.f) {
        tr = sqrtf(tr + 1.f);
        s = 0.5f / tr;
        q->w = tr * 0.5f;
        q->x = (M(1, 2) - M(2, 1)) * s;
        q->y = (M(2, 0) - M(0, 2)) * s;
        q->z = (M(0, 1) - M(1, 0)) * s;
        return q;
    }
    i = M_DIAG(1) >= M_DIAG(0) ? 1 : 0;
    i = M_DIAG(2) >= M_DIAG(i) ? 2 : i;
    j = (i + 1) % 3;
    k = (j + 1) % 3;
    tr = sqrtf(M_DIAG(i) - (M_DIAG(j) + M_DIAG(k)) + 1.f);
    s = 0.5f / tr;
    Q_EL(i) = 0.5f * tr;
    Q_EL(j) = (M(i, j) + M(j, i)) * s;
    Q_EL(k) = (M(i, k) + M(k, i)) * s;
    q->w = (M(k, j) - M(j, k)) * s;
    return q;
}
C2_HOOK_FUNCTION(0x00535ae0, BrMatrix34ToQuat)

br_matrix4* C2_HOOK_CDECL BrQuatToMatrix4(br_matrix4* mat, const br_quat* q) {
    br_matrix34 tmp;

    BrQuatToMatrix34(&tmp, q);
    BrMatrix4Copy34(mat, &tmp);
    return mat;
}
C2_HOOK_FUNCTION(0x00535c70, BrQuatToMatrix4)

br_quat* C2_HOOK_CDECL BrMatrix4ToQuat(br_quat* q, const br_matrix4* mat) {
    br_matrix34 tmp;

    BrMatrix34Copy4(&tmp, mat);
    return BrMatrix34ToQuat(q, &tmp);
}
C2_HOOK_FUNCTION(0x00535ca0, BrMatrix4ToQuat)
