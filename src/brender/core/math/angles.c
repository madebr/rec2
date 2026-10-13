#include "angles.h"
#include "c2_math.h"

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

#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(off)
#endif
static double C2_HOOK_CDECL ActivationEulerScalar(double value) {
    union { br_scalar scalar; br_uint_32 bits; } rounded;
    volatile br_uint_32 stored;

    rounded.scalar = (br_scalar)value;
    stored = rounded.bits;
    rounded.bits = stored;
    return rounded.scalar;
}

/* C-first physical evidence euler-c-first-focus retains five modern-only
 * quarter-turn differences. The original FSIN/FCOS tiny residue differs
 * from the modern CRT. Keep only these elementary transcendental operations. */
static double C2_HOOK_CDECL ActivationEulerCosine(double radians) {
#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
    double result;
    __asm {
        fld qword ptr radians
        fcos
        fstp qword ptr result
    }
    return result;
#else
    return cos(radians);
#endif
}

static double C2_HOOK_CDECL ActivationEulerSine(double radians) {
#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
    double result;
    __asm {
        fld qword ptr radians
        fsin
        fstp qword ptr result
    }
    return result;
#else
    return sin(radians);
#endif
}
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00531de0
br_matrix34* C2_HOOK_CDECL BrEulerToMatrix34(br_matrix34* mat, const br_euler* euler) {
    const volatile br_euler* input = euler;
    volatile br_matrix34* output = mat;
    unsigned order = input->order;
    unsigned i, j, h;
    unsigned a0, a1, a2;
    double ri, rj, rh;
    double ci, cj, ch, si, sj, sh;
    double ci_wide, ch_wide, si_wide, sh_wide;
    double cc, cs, sc, ss;
    const double radians_per_angle = 9.587379924285257e-5;

    if (order & 0x10) {
        i = input->c;
        j = input->b;
        h = input->a;
    } else {
        i = input->a;
        j = input->b;
        h = input->c;
    }
    if (order & 4) {
        i = (0u - i) & 0xffffu;
        j = (0u - j) & 0xffffu;
        h = (0u - h) & 0xffffu;
    }
    ri = i * radians_per_angle;
    rh = h * radians_per_angle;
    rj = j * radians_per_angle;
    ch_wide = ActivationEulerCosine(rh);
    si_wide = ActivationEulerSine(ri);
    sh_wide = ActivationEulerSine(rh);
    ci_wide = ActivationEulerCosine(ActivationEulerScalar(ri));
    cj = ActivationEulerScalar(ActivationEulerCosine(rj));
    sj = ActivationEulerScalar(ActivationEulerSine(ActivationEulerScalar(rj)));
    ci = ActivationEulerScalar(ci_wide);
    ch = ActivationEulerScalar(ch_wide);
    si = ActivationEulerScalar(si_wide);
    sh = ActivationEulerScalar(sh_wide);
    cc = ActivationEulerScalar(ch_wide * ci_wide);
    cs = ActivationEulerScalar(sh_wide * ci_wide);
    sc = ActivationEulerScalar(si_wide * ch);
    ss = ActivationEulerScalar(sh * si);
    /* ci's scalar spill is dead, while its wide register value remains live. */
    (void)ci;
    a0 = OrderAxes[order].a0;
    a1 = OrderAxes[order].a1;
    a2 = OrderAxes[order].a2;
    if (order & 8) {
        output->m[a0][a0] = (br_scalar)cj;
        output->m[a1][a0] = (br_scalar)(sj * si);
        output->m[a2][a0] = (br_scalar)(sj * ci_wide);
        output->m[a0][a1] = (br_scalar)(sh * sj);
        output->m[a1][a1] = (br_scalar)(cc - ss * cj);
        output->m[a2][a1] = (br_scalar)(-(cs * cj) - sc);
        output->m[a0][a2] = (br_scalar)(-(sj * ch));
        output->m[a1][a2] = (br_scalar)(sc * cj + cs);
        output->m[a2][a2] = (br_scalar)(cc * cj - ss);
    } else {
        output->m[a0][a0] = (br_scalar)(ch * cj);
        output->m[a1][a0] = (br_scalar)(sc * sj - cs);
        output->m[a2][a0] = (br_scalar)(cc * sj + ss);
        output->m[a0][a1] = (br_scalar)(sh * cj);
        output->m[a1][a1] = (br_scalar)(ss * sj + cc);
        output->m[a2][a1] = (br_scalar)(cs * sj - sc);
        output->m[a0][a2] = (br_scalar)(-sj);
        output->m[a1][a2] = (br_scalar)(si * cj);
        output->m[a2][a2] = (br_scalar)(cj * ci_wide);
    }
    output->m[3][2] = 0.f;
    output->m[3][1] = 0.f;
    output->m[3][0] = 0.f;
    return mat;
}

// FUNCTION: CARMA2_HW 0x005320d0
br_euler* C2_HOOK_CDECL BrMatrix34ToEuler(br_euler* euler, const br_matrix34* mat) {
    br_uint_8 o;
    int a0;
    int a1;
    int a2;

    o = euler->order;

    a0 = OrderAxes[o].a0;
    a1 = OrderAxes[o].a1;
    a2 = OrderAxes[o].a2;

    if (o & 0x8) {
        br_scalar sy;

        sy = (br_scalar)sqrt(mat->m[a1][a0] * mat->m[a1][a0] + mat->m[a2][a0] * mat->m[a2][a0]);

        if (sy > 16 * BR_SCALAR_EPSILON) {
            euler->a = BR_ATAN2( mat->m[a1][a0],  mat->m[a2][a0]);
            euler->b = BR_ATAN2( sy,              mat->m[a0][a0]);
            euler->c = BR_ATAN2( mat->m[a0][a1], -mat->m[a0][a2]);
        } else {
            euler->a = BR_ATAN2(-mat->m[a2][a1],  mat->m[a1][a1]);
            euler->b = BR_ATAN2( sy,              mat->m[a0][a0]);
            euler->c = 0;
        }
    } else {
        br_scalar cy;

        cy = (br_scalar)sqrt(mat->m[a0][a0] * mat->m[a0][a0] + mat->m[a0][a1] * mat->m[a0][a1]);

        if (cy > 16 * BR_SCALAR_EPSILON) {
            euler->a = BR_ATAN2( mat->m[a1][a2], mat->m[a2][a2]);
            euler->b = BR_ATAN2(-mat->m[a0][a2], cy);
            euler->c = BR_ATAN2( mat->m[a0][a1], mat->m[a0][a0]);
        } else {
            euler->a = BR_ATAN2(-mat->m[a2][a1], mat->m[a1][a1]);
            euler->b = BR_ATAN2(-mat->m[a0][a2], cy);
            euler->c = 0;
        }
    }

    if (o & 4) {
        euler->a = -euler->a;
        euler->b = -euler->b;
        euler->c = -euler->c;
    }

    if (o & 0x10) {
        br_angle t;

        t  = euler->a;
        euler->a = euler->c;
        euler->c = t;
    }

    return euler;
}

// FUNCTION: CARMA2_HW 0x00532320
br_matrix4* C2_HOOK_CDECL BrEulerToMatrix4(br_matrix4* mat, const br_euler* euler) {
    br_matrix34 tmp;

    BrEulerToMatrix34(&tmp, euler);
    BrMatrix4Copy34(mat, &tmp);
    return mat;
}

// FUNCTION: CARMA2_HW 0x00532350
br_euler* C2_HOOK_CDECL BrMatrix4ToEuler(br_euler* dest, const br_matrix4* mat) {
    br_matrix34 tmp;

    BrMatrix34Copy4(&tmp, mat);
    return BrMatrix34ToEuler(dest, &tmp);
}

// FUNCTION: CARMA2_HW 0x00532380
br_quat* C2_HOOK_CDECL BrEulerToQuat(br_quat* q, const br_euler* euler) {
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
}

// FUNCTION: CARMA2_HW 0x005325a0
br_euler* C2_HOOK_CDECL BrQuatToEuler(br_euler* euler, const br_quat* q) {
    br_matrix34 mat;

    BrQuatToMatrix34(&mat, q);
    return BrMatrix34ToEuler(euler, &mat);
}
