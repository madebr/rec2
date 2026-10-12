#include "quat.h"

#include "matrix34.h"
#include "matrix4.h"

#include "c2_math.h"

#define M(x, y) mat->m[x][y]
#define M_DIAG(x) (((br_scalar*)mat->m)[4*(x)])
#define Q_EL(e) (((br_scalar*)&(q->x))[(e)])

// FUNCTION: CARMA2_HW 0x005355e0
br_quat* C2_HOOK_CDECL BrQuatMul(br_quat* q, const br_quat* l, const br_quat* r) {
#if defined(_MSC_VER) && defined(_M_IX86)
    br_scalar work[8];

    /* Keep the original seven float spills, unspilled final combination, and
     * read-before-write order. Scalar C loses precision and breaks q == l/r. */
    work[7] = 0.5f;
    __asm {
        mov eax, l
        mov ecx, r
        lea edx, work
        mov edi, q
        fld dword ptr [eax + 0xc]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [eax + 0xc]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [eax + 0xc]
        fxch st(4)
        fadd dword ptr [eax]
        fld dword ptr [ecx + 0xc]
        fxch st(4)
        fadd dword ptr [ecx]
        fmulp st(1), st(0)
        fstp dword ptr [edx + 8]
        fld dword ptr [eax + 0xc]
        fxch st(2)
        fsub dword ptr [eax]
        fld dword ptr [ecx + 8]
        fadd dword ptr [ecx + 4]
        fmulp st(1), st(0)
        fstp dword ptr [edx + 12]
        fld dword ptr [ecx + 4]
        fxch st(1)
        fsub dword ptr [ecx]
        fld dword ptr [eax + 4]
        fadd dword ptr [eax + 8]
        fmulp st(1), st(0)
        fstp dword ptr [edx + 16]
        fld dword ptr [eax + 8]
        fxch st(1)
        fadd dword ptr [ecx]
        fld dword ptr [eax + 4]
        fxch st(2)
        fadd dword ptr [eax]
        fmulp st(1), st(0)
        fstp dword ptr [edx]
        fld dword ptr [ecx + 8]
        fxch st(4)
        faddp st(1), st(0)
        fld dword ptr [ecx + 0xc]
        fxch st(4)
        fsubp st(3), st(0)
        fxch st(2)
        fmulp st(2), st(0)
        fsub dword ptr [eax + 4]
        fxch st(1)
        fxch st(2)
        fxch st(1)
        fxch st(2)
        fstp dword ptr [edx + 4]
        fadd dword ptr [ecx + 8]
        fmulp st(1), st(0)
        fstp dword ptr [edx + 24]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 8]
        fld dword ptr [edx + 24]
        fadd dword ptr [edx + 4]
        fxch st(2)
        fsubr dword ptr [ecx]
        fxch st(1)
        fsub dword ptr [eax]
        fxch st(2)
        fadd dword ptr [edx]
        fxch st(2)
        fmulp st(1), st(0)
        fld dword ptr [eax + 8]
        fld dword ptr [eax + 4]
        fld dword ptr [ecx + 4]
        fxch st(4)
        fstp dword ptr [edx + 20]
        fld dword ptr [ecx + 8]
        fxch st(3)
        fadd dword ptr [edx + 20]
        fmul dword ptr [edx + 28]
        fld st(0)
        fxch st(2)
        fsubp st(3), st(0)
        fxch st(3)
        fsubp st(4), st(0)
        fxch st(3)
        fmulp st(1), st(0)
        fxch st(2)
        fadd dword ptr [edx + 8]
        fld st(1)
        fadd dword ptr [edx + 12]
        fxch st(3)
        fadd st(0), st(2)
        fxch st(1)
        fsub dword ptr [edx + 20]
        fxch st(3)
        fsub dword ptr [edx + 24]
        fxch st(1)
        fsub dword ptr [edx]
        fxch st(2)
        fadd dword ptr [edx + 16]
        fxch st(3)
        mov eax, edi
        fstp dword ptr [eax]
        fxch st(1)
        fstp dword ptr [eax + 0xc]
        fxch st(1)
        fsub dword ptr [edx + 4]
        fxch st(1)
        fstp dword ptr [eax + 4]
        fstp dword ptr [eax + 8]
    }
    return q;
#elif defined(__GNUC__) && defined(__i386__)
    br_scalar work[8];
    const br_quat* left = l;

    work[7] = 0.5f;
    __asm__ volatile(
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fxch st(4)\n\t"
        "fadd dword ptr [eax]\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fxch st(4)\n\t"
        "fadd dword ptr [ecx]\n\t"
        "fmulp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 8]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fxch st(2)\n\t"
        "fsub dword ptr [eax]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fadd dword ptr [ecx + 4]\n\t"
        "fmulp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 12]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fxch st(1)\n\t"
        "fsub dword ptr [ecx]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fadd dword ptr [eax + 8]\n\t"
        "fmulp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 16]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fxch st(1)\n\t"
        "fadd dword ptr [ecx]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fxch st(2)\n\t"
        "fadd dword ptr [eax]\n\t"
        "fmulp st(1), st(0)\n\t"
        "fstp dword ptr [edx]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fxch st(4)\n\t"
        "faddp st(1), st(0)\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fxch st(4)\n\t"
        "fsubp st(3), st(0)\n\t"
        "fxch st(2)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fsub dword ptr [eax + 4]\n\t"
        "fxch st(1)\n\t"
        "fxch st(2)\n\t"
        "fxch st(1)\n\t"
        "fxch st(2)\n\t"
        "fstp dword ptr [edx + 4]\n\t"
        "fadd dword ptr [ecx + 8]\n\t"
        "fmulp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 24]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [edx + 24]\n\t"
        "fadd dword ptr [edx + 4]\n\t"
        "fxch st(2)\n\t"
        "fsubr dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "fsub dword ptr [eax]\n\t"
        "fxch st(2)\n\t"
        "fadd dword ptr [edx]\n\t"
        "fxch st(2)\n\t"
        "fmulp st(1), st(0)\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fxch st(4)\n\t"
        "fstp dword ptr [edx + 20]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fxch st(3)\n\t"
        "fadd dword ptr [edx + 20]\n\t"
        "fmul dword ptr [edx + 28]\n\t"
        "fld st(0)\n\t"
        "fxch st(2)\n\t"
        "fsubp st(3), st(0)\n\t"
        "fxch st(3)\n\t"
        "fsubp st(4), st(0)\n\t"
        "fxch st(3)\n\t"
        "fmulp st(1), st(0)\n\t"
        "fxch st(2)\n\t"
        "fadd dword ptr [edx + 8]\n\t"
        "fld st(1)\n\t"
        "fadd dword ptr [edx + 12]\n\t"
        "fxch st(3)\n\t"
        "fadd st(0), st(2)\n\t"
        "fxch st(1)\n\t"
        "fsub dword ptr [edx + 20]\n\t"
        "fxch st(3)\n\t"
        "fsub dword ptr [edx + 24]\n\t"
        "fxch st(1)\n\t"
        "fsub dword ptr [edx]\n\t"
        "fxch st(2)\n\t"
        "fadd dword ptr [edx + 16]\n\t"
        "fxch st(3)\n\t"
        "mov eax, edi\n\t"
        "fstp dword ptr [eax]\n\t"
        "fxch st(1)\n\t"
        "fstp dword ptr [eax + 0xc]\n\t"
        "fxch st(1)\n\t"
        "fsub dword ptr [edx + 4]\n\t"
        "fxch st(1)\n\t"
        "fstp dword ptr [eax + 4]\n\t"
        "fstp dword ptr [eax + 8]\n\t"
        ".att_syntax prefix\n\t"
        : "+a"(left)
        : "c"(r), "d"(work), "D"(q)
        : "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");
    return q;
#else

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
#endif
}

// FUNCTION: CARMA2_HW 0x00535710
br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq) {
    br_scalar s;

    s = 1.0f / (float)sqrt(qq->x * qq->x + qq->y * qq->y + qq->z * qq->z + qq->w * qq->w);
    q->x = qq->x * s;
    q->y = qq->y * s;
    q->z = qq->z * s;
    q->w = qq->w * s;
    return q;
}

// FUNCTION: CARMA2_HW 0x00535790
br_quat* C2_HOOK_CDECL BrQuatInvert(br_quat* q, const br_quat* qq) {
#if defined(_MSC_VER) && defined(_M_IX86)
    /* FLD/FSTP also quiet signaling NaNs in w, unlike an integer copy. */
    __asm {
        mov eax, q
        mov ecx, qq
        fld dword ptr [ecx]
        fchs
        fstp dword ptr [eax]
        fld dword ptr [ecx + 4]
        fchs
        fstp dword ptr [eax + 4]
        fld dword ptr [ecx + 8]
        fchs
        fstp dword ptr [eax + 8]
        fld dword ptr [ecx + 0xc]
        fstp dword ptr [eax + 0xc]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile(
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [ecx]\n\t"
        "fchs\n\t"
        "fstp dword ptr [eax]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fchs\n\t"
        "fstp dword ptr [eax + 4]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fchs\n\t"
        "fstp dword ptr [eax + 8]\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fstp dword ptr [eax + 0xc]\n\t"
        ".att_syntax prefix\n\t"
        :
        : "a"(q), "c"(qq)
        : "memory", "st");
#else
    q->x = -qq->x;
    q->y = -qq->y;
    q->z = -qq->z;
    q->w = qq->w;
#endif
    return q;
}

// FUNCTION: CARMA2_HW 0x005357c0
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

// FUNCTION: CARMA2_HW 0x005359a0
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

// FUNCTION: CARMA2_HW 0x00535ae0
br_quat* C2_HOOK_CDECL BrMatrix34ToQuat(br_quat* q, const br_matrix34* mat) {
#if (defined(_MSC_VER) && defined(_M_IX86)) || (defined(__GNUC__) && defined(__i386__))
    union {
        br_uint_32 words[18];
        br_scalar scalars[18];
        double wide[9];
    } work;
    int negative;
    int i;
    int j;
    int k;

    /* The original trace is 00 + 22 + 11. Its sign test precedes the float
     * root spill, and 0.5/root stays in x87 precision through every output.
     * A double holds that PC53 trace exactly while selecting the branch. */
    work.scalars[13] = 0.5f;
    work.scalars[14] = -1.0f;
    work.scalars[15] = 0.0f;
#if defined(_MSC_VER)
    __asm {
        mov ecx, mat
        lea edx, work
        fld dword ptr [ecx]
        fadd dword ptr [ecx + 32]
        fadd dword ptr [ecx + 16]
        fcom dword ptr [edx + 60]
        fnstsw ax
        fstp qword ptr [edx + 64]
        and eax, 100h
        mov negative, eax
    }
#else
    __asm__ volatile(
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [ecx]\n\t"
        "fadd dword ptr [ecx + 32]\n\t"
        "fadd dword ptr [ecx + 16]\n\t"
        "fcom dword ptr [edx + 60]\n\t"
        "fnstsw ax\n\t"
        "fstp qword ptr [edx + 64]\n\t"
        "and eax, 0x100\n\t"
        ".att_syntax prefix\n\t"
        : "=a"(negative)
        : "c"(mat), "d"(&work)
        : "cc", "memory", "st");
#endif
    if (!negative) {
#if defined(_MSC_VER)
    __asm {
        mov eax, q
        mov ecx, mat
        lea edx, work
        fld qword ptr [edx + 64]
        fsub dword ptr [edx + 56]
        fld dword ptr [edx + 52]
        fxch st(1)
        fsqrt
        fstp dword ptr [edx + 48]
        fdiv dword ptr [edx + 48]
        fld dword ptr [edx + 48]
        fmul dword ptr [edx + 52]
        fstp dword ptr [eax + 12]
        fld dword ptr [ecx + 20]
        fsub dword ptr [ecx + 28]
        fmul st(0), st(1)
        fstp dword ptr [eax]
        fld dword ptr [ecx + 24]
        fsub dword ptr [ecx + 8]
        fmul st(0), st(1)
        fstp dword ptr [eax + 4]
        fld dword ptr [ecx + 4]
        fsub dword ptr [ecx + 12]
        fmul st(0), st(1)
        fstp dword ptr [eax + 8]
        fstp st(0)
    }
#else
    __asm__ volatile(
        ".intel_syntax noprefix\n\t"
        "fld qword ptr [edx + 64]\n\t"
        "fsub dword ptr [edx + 56]\n\t"
        "fld dword ptr [edx + 52]\n\t"
        "fxch st(1)\n\t"
        "fsqrt\n\t"
        "fstp dword ptr [edx + 48]\n\t"
        "fdiv dword ptr [edx + 48]\n\t"
        "fld dword ptr [edx + 48]\n\t"
        "fmul dword ptr [edx + 52]\n\t"
        "fstp dword ptr [eax + 12]\n\t"
        "fld dword ptr [ecx + 20]\n\t"
        "fsub dword ptr [ecx + 28]\n\t"
        "fmul st(0), st(1)\n\t"
        "fstp dword ptr [eax]\n\t"
        "fld dword ptr [ecx + 24]\n\t"
        "fsub dword ptr [ecx + 8]\n\t"
        "fmul st(0), st(1)\n\t"
        "fstp dword ptr [eax + 4]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fsub dword ptr [ecx + 12]\n\t"
        "fmul st(0), st(1)\n\t"
        "fstp dword ptr [eax + 8]\n\t"
        "fstp st(0)\n\t"
        ".att_syntax prefix\n\t"
        :
        : "a"(q), "c"(mat), "d"(&work)
        : "memory", "st", "st(1)");
#endif
        return q;
    }
    /* Strict greater-than keeps the earlier diagonal on ties or unordered.
     * All indices are captured before an overlapping quaternion is written. */
#if defined(_MSC_VER)
    __asm {
        mov ecx, mat
        xor edx, edx
        fld dword ptr [ecx + 16]
        fcomp dword ptr [ecx]
        fnstsw ax
        test ah, 41h
        jne quat_keep_first
        mov edx, 1
    quat_keep_first:
        mov eax, edx
        shl eax, 4
        fld dword ptr [ecx + 32]
        fcomp dword ptr [ecx + eax]
        fnstsw ax
        test ah, 41h
        jne quat_keep_diagonal
        mov edx, 2
    quat_keep_diagonal:
        mov i, edx
    }
#else
    __asm__ volatile(
        ".intel_syntax noprefix\n\t"
        "xor edx, edx\n\t"
        "fld dword ptr [ecx + 16]\n\t"
        "fcomp dword ptr [ecx]\n\t"
        "fnstsw ax\n\t"
        "test ah, 0x41\n\t"
        "jne 1f\n\t"
        "mov edx, 1\n\t"
        "1:\n\t"
        "mov eax, edx\n\t"
        "shl eax, 4\n\t"
        "fld dword ptr [ecx + 32]\n\t"
        "fcomp dword ptr [ecx + eax]\n\t"
        "fnstsw ax\n\t"
        "test ah, 0x41\n\t"
        "jne 2f\n\t"
        "mov edx, 2\n\t"
        "2:\n\t"
        ".att_syntax prefix\n\t"
        : "=a"(negative), "=d"(i)
        : "c"(mat)
        : "cc", "memory", "st");
#endif
    j = (i + 1) % 3;
    k = (j + 1) % 3;
    work.words[0] = 16 * i;
    work.words[1] = 16 * j;
    work.words[2] = 16 * k;
    work.words[3] = 4 * i;
    work.words[4] = 4 * j;
    work.words[5] = 4 * k;
    work.words[6] = 4 * (3 * j + k);
    work.words[7] = 4 * (3 * k + j);
    work.words[8] = 4 * (3 * i + j);
    work.words[9] = 4 * (3 * j + i);
    work.words[10] = 4 * (3 * i + k);
    work.words[11] = 4 * (3 * k + i);
    /* Preserve q[i], w, q[j], q[k] writes and their intervening matrix reads. */
#if defined(_MSC_VER)
    __asm {
        mov eax, q
        mov ecx, mat
        lea edx, work
        mov ebx, dword ptr [edx + 8]
        fld dword ptr [ecx + ebx]
        fld dword ptr [edx + 52]
        fxch st(1)
        mov ebx, dword ptr [edx + 4]
        fadd dword ptr [ecx + ebx]
        mov ebx, dword ptr [edx]
        fsubr dword ptr [ecx + ebx]
        fsub dword ptr [edx + 56]
        fsqrt
        fstp dword ptr [edx + 48]
        fdiv dword ptr [edx + 48]
        fld dword ptr [edx + 48]
        fmul dword ptr [edx + 52]
        mov ebx, dword ptr [edx + 12]
        fstp dword ptr [eax + ebx]
        mov ebx, dword ptr [edx + 24]
        fld dword ptr [ecx + ebx]
        mov ebx, dword ptr [edx + 28]
        fsub dword ptr [ecx + ebx]
        fmul st(0), st(1)
        fstp dword ptr [eax + 12]
        mov ebx, dword ptr [edx + 32]
        fld dword ptr [ecx + ebx]
        mov ebx, dword ptr [edx + 36]
        fadd dword ptr [ecx + ebx]
        fmul st(0), st(1)
        mov ebx, dword ptr [edx + 16]
        fstp dword ptr [eax + ebx]
        mov ebx, dword ptr [edx + 40]
        fld dword ptr [ecx + ebx]
        mov ebx, dword ptr [edx + 44]
        fadd dword ptr [ecx + ebx]
        fmul st(0), st(1)
        mov ebx, dword ptr [edx + 20]
        fstp dword ptr [eax + ebx]
        fstp st(0)
    }
#else
    __asm__ volatile(
        ".intel_syntax noprefix\n\t"
        "mov ebx, dword ptr [edx + 8]\n\t"
        "fld dword ptr [ecx + ebx]\n\t"
        "fld dword ptr [edx + 52]\n\t"
        "fxch st(1)\n\t"
        "mov ebx, dword ptr [edx + 4]\n\t"
        "fadd dword ptr [ecx + ebx]\n\t"
        "mov ebx, dword ptr [edx]\n\t"
        "fsubr dword ptr [ecx + ebx]\n\t"
        "fsub dword ptr [edx + 56]\n\t"
        "fsqrt\n\t"
        "fstp dword ptr [edx + 48]\n\t"
        "fdiv dword ptr [edx + 48]\n\t"
        "fld dword ptr [edx + 48]\n\t"
        "fmul dword ptr [edx + 52]\n\t"
        "mov ebx, dword ptr [edx + 12]\n\t"
        "fstp dword ptr [eax + ebx]\n\t"
        "mov ebx, dword ptr [edx + 24]\n\t"
        "fld dword ptr [ecx + ebx]\n\t"
        "mov ebx, dword ptr [edx + 28]\n\t"
        "fsub dword ptr [ecx + ebx]\n\t"
        "fmul st(0), st(1)\n\t"
        "fstp dword ptr [eax + 12]\n\t"
        "mov ebx, dword ptr [edx + 32]\n\t"
        "fld dword ptr [ecx + ebx]\n\t"
        "mov ebx, dword ptr [edx + 36]\n\t"
        "fadd dword ptr [ecx + ebx]\n\t"
        "fmul st(0), st(1)\n\t"
        "mov ebx, dword ptr [edx + 16]\n\t"
        "fstp dword ptr [eax + ebx]\n\t"
        "mov ebx, dword ptr [edx + 40]\n\t"
        "fld dword ptr [ecx + ebx]\n\t"
        "mov ebx, dword ptr [edx + 44]\n\t"
        "fadd dword ptr [ecx + ebx]\n\t"
        "fmul st(0), st(1)\n\t"
        "mov ebx, dword ptr [edx + 20]\n\t"
        "fstp dword ptr [eax + ebx]\n\t"
        "fstp st(0)\n\t"
        ".att_syntax prefix\n\t"
        :
        : "a"(q), "c"(mat), "d"(&work)
        : "ebx", "memory", "st", "st(1)");
#endif
    return q;
#else

    double tr;
    double s;
    int i;
    int j;
    int k;

    tr = ((double)M(0, 0) + M(2, 2)) + M(1, 1);
    if (tr >= 0.f) {
        tr = (br_scalar)sqrt(tr + 1.0f);
        s = 0.5f / tr;
        q->w = tr * 0.5f;
        q->x = (M(1, 2) - M(2, 1)) * s;
        q->y = (M(2, 0) - M(0, 2)) * s;
        q->z = (M(0, 1) - M(1, 0)) * s;
        return q;
    }
    i = M_DIAG(1) > M_DIAG(0) ? 1 : 0;
    i = M_DIAG(2) > M_DIAG(i) ? 2 : i;
    j = (i + 1) % 3;
    k = (j + 1) % 3;
    tr = (br_scalar)sqrt(M_DIAG(i) - ((double)M_DIAG(k) + M_DIAG(j)) + 1.f);
    s = 0.5f / tr;
    Q_EL(i) = 0.5f * tr;
    q->w = ((double)M(j, k) - M(k, j)) * s;
    Q_EL(j) = ((double)M(i, j) + M(j, i)) * s;
    Q_EL(k) = ((double)M(i, k) + M(k, i)) * s;
    return q;
#endif
}

// FUNCTION: CARMA2_HW 0x00535c70
br_matrix4* C2_HOOK_CDECL BrQuatToMatrix4(br_matrix4* mat, const br_quat* q) {
    br_matrix34 tmp;

    BrQuatToMatrix34(&tmp, q);
    BrMatrix4Copy34(mat, &tmp);
    return mat;
}

// FUNCTION: CARMA2_HW 0x00535ca0
br_quat* C2_HOOK_CDECL BrMatrix4ToQuat(br_quat* q, const br_matrix4* mat) {
    br_matrix34 tmp;

    BrMatrix34Copy4(&tmp, mat);
    return BrMatrix34ToQuat(q, &tmp);
}
