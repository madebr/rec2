#include "angles.h"

#include "matrix34.h"
#include "matrix4.h"
#include "quat.h"

order_info OrderAxes[32] = {
    { 0, 1, 2, },
    { 1, 2, 0, },
    { 2, 0, 1, },
    { 0, 0, 0, },
    { 0, 2, 1, },
    { 1, 0, 2, },
    { 2, 1, 0, },
    { 0, 0, 0, },
    { 0, 1, 2, },
    { 1, 2, 0, },
    { 2, 0, 1, },
    { 0, 0, 0, },
    { 0, 2, 1, },
    { 1, 0, 2, },
    { 2, 1, 0, },
    { 0, 0, 0, },
    { 0, 1, 2, },
    { 1, 2, 0, },
    { 2, 0, 1, },
    { 0, 0, 0, },
    { 0, 2, 1, },
    { 1, 0, 2, },
    { 2, 1, 0, },
    { 0, 0, 0, },
    { 0, 1, 2, },
    { 1, 2, 0, },
    { 2, 0, 1, },
    { 0, 0, 0, },
    { 0, 2, 1, },
    { 1, 0, 2, },
    { 2, 1, 0, },
    { 0, 0, 0, },
};

br_matrix34* (C2_HOOK_CDECL * BrEulerToMatrix34_original)(br_matrix34* mat, const br_euler* euler);
br_matrix34* C2_HOOK_CDECL BrEulerToMatrix34(br_matrix34* mat, const br_euler* euler) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrEulerToMatrix34_original(mat, euler);
#else
    br_uint_8 o;
    br_angle ti;
    br_angle tj;
    br_angle th;
    br_scalar ci;
    br_scalar cj;
    br_scalar ch;
    br_scalar si;
    br_scalar sj;
    br_scalar sh;
    br_scalar cc;
    br_scalar cs;
    br_scalar sc;
    br_scalar ss;
    int a0;
    int a1;
    int a2;

    o = euler->order;
    if (o & 0x10) {
        ti = euler->c;
        tj = euler->b;;
        th = euler->a;
    } else {
        ti = euler->c;
        tj = euler->b;
        th = euler->a;
    }
    if (o & 0x4) {
        ti = -ti;
        tj = -tj;
        th = -th;
    }
    ci = BR_COS(ti);
    cj = BR_COS(tj);
    ch = BR_COS(th);

    si = BR_SIN(ti);
    sj = BR_SIN(tj);
    sh = BR_SIN(th);

    cc = ci * ch;
    cs = ci * sh;
    sc = si * ch;
    ss = si * sh;

    a0 = OrderAxes[o].a0;
    a1 = OrderAxes[o].a1;
    a2 = OrderAxes[o].a2;

    if (o & 0x8) {
        mat->m[a0][a0] =              cj;
        mat->m[a1][a0] =   sj * si;
        mat->m[a2][a0] =   sj * ci;
        mat->m[a0][a1] =   sj * sh;
        mat->m[a1][a1] = -(cj * ss) + cc;
        mat->m[a2][a1] = -(cj * cs) - sc;
        mat->m[a0][a2] = -(sj * ch);
        mat->m[a1][a2] =   cj * sc + cs;
        mat->m[a2][a2] =   cj * cc - ss;
    } else {
        mat->m[a0][a0] =  cj * ch;
        mat->m[a1][a0] =  sj * sc - cs;
        mat->m[a2][a0] =  sj * cc + ss;
        mat->m[a0][a1] =  cj * sh;
        mat->m[a1][a1] =  sj * ss + cc;
        mat->m[a2][a1] =  sj * cs - sc;
        mat->m[a0][a2] =          - sj;
        mat->m[a1][a2] =  cj * si;
        mat->m[a2][a2] =  cj * ci;
    }

    mat->m[3][0] = 0.f;
    mat->m[3][1] = 0.f;
    mat->m[3][2] = 0.f;

    return mat;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00531de0, BrEulerToMatrix34, BrEulerToMatrix34_original)

br_euler* (C2_HOOK_CDECL * BrMatrix34ToEuler_original)(br_euler* euler, const br_matrix34* mat);
br_euler* C2_HOOK_CDECL BrMatrix34ToEuler(br_euler* euler, const br_matrix34* mat) {

#if defined(C2_HOOKS_ENABLED)
    return BrMatrix34ToEuler_original(euler, mat);
#else
    br_uint_8 o;
    int a0;
    int a1;
    int a2;
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005320d0, BrMatrix34ToEuler, BrMatrix34ToEuler_original)

br_matrix4* C2_HOOK_CDECL BrEulerToMatrix4(br_matrix4* mat, const br_euler* euler) {
    br_matrix34 tmp;

    BrEulerToMatrix34(&tmp, euler);
    BrMatrix4Copy34(mat, &tmp);
    return mat;
}
C2_HOOK_FUNCTION(0x00532320, BrEulerToMatrix4)

br_euler* C2_HOOK_CDECL BrMatrix4ToEuler(br_euler* dest, const br_matrix4* mat) {
    br_matrix34 tmp;

    BrMatrix34Copy4(&tmp, mat);
    return BrMatrix34ToEuler(dest, &tmp);
}
C2_HOOK_FUNCTION(0x00532350, BrMatrix4ToEuler)

br_quat* (C2_HOOK_CDECL * BrEulerToQuat_original)(br_quat* q, const br_euler* euler);
br_quat* C2_HOOK_CDECL BrEulerToQuat(br_quat* q, const br_euler* euler) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrEulerToQuat_original(q, euler);
#else
    br_uint_8 o;
    br_angle ti;
    br_angle tj;
    br_angle th;
    br_scalar ci;
    br_scalar cj;
    br_scalar ch;
    br_scalar si;
    br_scalar sj;
    br_scalar sh;
    br_scalar cc;
    br_scalar cs;
    br_scalar sc;
    br_scalar ss;
    int a0;
    int a1;
    int a2;

    o = euler->order;
    if (o & 0x10) {
        ti = euler->c / 2;
        tj = euler->b / 2;
        th = euler->a / 2;
    } else {
        ti = euler->a / 2;
        tj = euler->b / 2;
        th = euler->c / 2;
    }
    if (o & 0x4) {
        tj = -tj;
    }

    ci = BR_COS(ti);
    cj = BR_COS(tj);
    ch = BR_COS(th);

    si = BR_SIN(ti);
    sj = BR_SIN(tj);
    sh = BR_SIN(th);

    cc = ci * ch;
    cs = ci * sh;
    sc = si * ch;
    ss = si * sh;

    a0 = OrderAxes[o].a0;
    a1 = OrderAxes[o].a1;
    a2 = OrderAxes[o].a2;

    if (o & 8) {
        ((br_scalar*)q)[a0] = cj * (cs + sc);
        ((br_scalar*)q)[a1] = sj * (cc + ss);
        ((br_scalar*)q)[a2] = sj * (cs - sc);
        q->w = cj * (cc - ss);
    } else {
        ((br_scalar*)q)[a0] = cj * sc - sj * cs;
        ((br_scalar*)q)[a1] = cj * ss + sj * cc;
        ((br_scalar*)q)[a2] = cj * cs - sj * sc;
        q->w = cj * cc + sj * ss;
    }
    if (o & 4) {
        ((br_scalar*)q)[a1] = -((br_scalar*)q)[a1];
    }
    return q;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00532380, BrEulerToQuat, BrEulerToQuat_original)

br_euler* C2_HOOK_CDECL BrQuatToEuler(br_euler* euler, const br_quat* q) {
    br_matrix34 mat;

    BrQuatToMatrix34(&mat, q);
    return BrMatrix34ToEuler(euler, &mat);
}
C2_HOOK_FUNCTION(0x005325a0, BrQuatToEuler)
