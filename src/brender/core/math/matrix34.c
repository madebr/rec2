#include "matrix34.h"

#include "vector.h"

#include "c2_math.h"

#include <string.h>


// GLOBAL: CARMA2_HW 0x006b09c0
br_matrix34 mattmp1;

// GLOBAL: CARMA2_HW 0x006b09f0
br_matrix34 mattmp2;

#define M(x, y) mat->m[x][y]
#define A(x, y) A->m[x][y]
#define B(x, y) B->m[x][y]
#define C(x, y) C->m[x][y]
#define BR_MAC3(a, b, c, d, e, f) ((a) * (b) + (c) * (d) + (e) * (f))
#define BR_MAC4(a, b, c, d, e, f, g, h) ((a) * (b) + (c) * (d) + (e) * (f) + (g) * (h))

// FUNCTION: CARMA2_HW 0x005325d0
void C2_HOOK_CDECL BrMatrix34Copy(br_matrix34* A, const br_matrix34* B) {
#if defined(_MSC_VER) && _MSC_VER < 1300
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
#else
    unsigned int value;
    volatile unsigned int stored_value;
    int i;

    /* Sequential raw MOV copies retain NaN bits and partial overlap behavior. */
    for (i = 0; i < 12; i++) {
        memcpy(&value, (const char*)B + 4 * i, sizeof(value));
        stored_value = value;
        value = stored_value;
        memcpy((char*)A + 4 * i, &value, sizeof(value));
    }
#endif
}

/* MSVC5 otherwise treats the asm block as cheap enough to inline. The original
 * matrix wrappers call this multiply out of line. Keep that call boundary. */
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00532620
void C2_HOOK_CDECL BrMatrix34Mul(br_matrix34* A, const br_matrix34* B, const br_matrix34* C) {
    /* Keep original x87 operand forms where the compiler otherwise changes
     * NaN payload selection or quieting; do not normalize public values. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov ecx, B
        mov eax, C
        mov edx, A
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x18]
        fld dword ptr [eax + 0xc]
        fld dword ptr [eax]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx]
        fld dword ptr [eax + 4]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x10]
        fld dword ptr [eax + 0x1c]
        fxch st(3)
        fmul dword ptr [ecx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [ecx + 8]
        faddp st(1), st(0)
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 8]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x20]
        fxch st(3)
        fmul dword ptr [ecx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [ecx + 8]
        faddp st(1), st(0)
        fstp dword ptr [edx + 8]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [ecx + 0x10]
        fld dword ptr [eax + 0xc]
        fld dword ptr [ecx + 0x14]
        fxch st(3)
        fmul dword ptr [eax]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [eax + 0x18]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0xc]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [eax + 4]
        fld dword ptr [ecx + 0x14]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [ecx + 0x10]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x10]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x10]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [eax + 8]
        fld dword ptr [ecx + 0x10]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 0x14]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x20]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x14]
        fld dword ptr [ecx + 0x18]
        fld dword ptr [ecx + 0x20]
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 0x1c]
        fxch st(3)
        fmul dword ptr [eax]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [eax + 0xc]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x18]
        fld dword ptr [ecx + 0x1c]
        fld dword ptr [eax + 0x10]
        fld dword ptr [ecx + 0x18]
        fld dword ptr [eax + 4]
        fld dword ptr [ecx + 0x20]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x1c]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x1c]
        fld dword ptr [ecx + 0x1c]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 0x18]
        fld dword ptr [eax + 8]
        fld dword ptr [ecx + 0x20]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x20]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x20]
        fld dword ptr [eax + 0xc]
        fld dword ptr [ecx + 0x24]
        fld dword ptr [ecx + 0x28]
        fld dword ptr [ecx + 0x2c]
        fxch st(2)
        fmul dword ptr [eax]
        fxch st(3)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x18]
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x24]
        fstp dword ptr [edx + 0x24]
        fld dword ptr [ecx + 0x28]
        fld dword ptr [eax + 0x10]
        fld dword ptr [ecx + 0x24]
        fld dword ptr [eax + 4]
        fld dword ptr [ecx + 0x2c]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x1c]
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x28]
        fstp dword ptr [edx + 0x28]
        fld dword ptr [ecx + 0x24]
        fld dword ptr [eax + 8]
        fld dword ptr [ecx + 0x28]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 0x2c]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x20]
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x2c]
        fstp dword ptr [edx + 0x2c]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__(
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [eax + 0x18]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [eax]\n\t"
        "fxch st(2)\n\t"
        "fmulp st(3), st(0)\n\t"
        "fmul dword ptr [ecx + 4]\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fld dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [eax + 0x1c]\n\t"
        "fxch st(3)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx + 8]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 4]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fld dword ptr [eax + 0x20]\n\t"
        "fxch st(3)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx + 8]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 8]\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fld dword ptr [ecx + 0x10]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [ecx + 0x14]\n\t"
        "fxch st(3)\n\t"
        "fmul dword ptr [eax]\n\t"
        "fxch st(1)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [eax + 0x18]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0xc]\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fld dword ptr [ecx + 0x14]\n\t"
        "fld dword ptr [eax + 0x1c]\n\t"
        "fld dword ptr [ecx + 0x10]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x10]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x10]\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [ecx + 0x10]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fld dword ptr [ecx + 0x14]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x20]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x14]\n\t"
        "fld dword ptr [ecx + 0x18]\n\t"
        "fld dword ptr [ecx + 0x20]\n\t"
        "fld dword ptr [eax + 0x18]\n\t"
        "fld dword ptr [ecx + 0x1c]\n\t"
        "fxch st(3)\n\t"
        "fmul dword ptr [eax]\n\t"
        "fxch st(1)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [eax + 0xc]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x18]\n\t"
        "fld dword ptr [ecx + 0x1c]\n\t"
        "fld dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [ecx + 0x18]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fld dword ptr [ecx + 0x20]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x1c]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x1c]\n\t"
        "fld dword ptr [ecx + 0x1c]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fld dword ptr [ecx + 0x18]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [ecx + 0x20]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x20]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x20]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [ecx + 0x24]\n\t"
        "fld dword ptr [ecx + 0x28]\n\t"
        "fld dword ptr [ecx + 0x2c]\n\t"
        "fxch st(2)\n\t"
        "fmul dword ptr [eax]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x18]\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x24]\n\t"
        "fstp dword ptr [edx + 0x24]\n\t"
        "fld dword ptr [ecx + 0x28]\n\t"
        "fld dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [ecx + 0x24]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fld dword ptr [ecx + 0x2c]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x1c]\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x28]\n\t"
        "fstp dword ptr [edx + 0x28]\n\t"
        "fld dword ptr [ecx + 0x24]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [ecx + 0x28]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fld dword ptr [ecx + 0x2c]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x20]\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x2c]\n\t"
        "fstp dword ptr [edx + 0x2c]\n\t"
        ".att_syntax prefix\n\t"
        :
        : "c" (B), "a" (C), "d" (A)
        : "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)"
    );
#else
    volatile br_matrix34* output = A;
    const volatile br_matrix34* left = B;
    const volatile br_matrix34* right = C;

    /* Each original scalar store precedes fresh loads for the next element.
     * Keep the individual x87 PC53 sum orders, including overlapping matrices. */
    output->m[0][0] = (float)(((double)left->m[0][2] * right->m[2][0] + (double)right->m[1][0] * left->m[0][1]) + (double)right->m[0][0] * left->m[0][0]);
    output->m[0][1] = (float)(((double)right->m[0][1] * left->m[0][0] + (double)left->m[0][1] * right->m[1][1]) + (double)right->m[2][1] * left->m[0][2]);
    output->m[0][2] = (float)(((double)right->m[0][2] * left->m[0][0] + (double)right->m[1][2] * left->m[0][1]) + (double)right->m[2][2] * left->m[0][2]);
    output->m[1][0] = (float)(((double)left->m[1][0] * right->m[0][0] + (double)left->m[1][1] * right->m[1][0]) + (double)left->m[1][2] * right->m[2][0]);
    output->m[1][1] = (float)(((double)left->m[1][0] * right->m[0][1] + (double)left->m[1][2] * right->m[2][1]) + (double)left->m[1][1] * right->m[1][1]);
    output->m[1][2] = (float)(((double)left->m[1][0] * right->m[0][2] + (double)left->m[1][1] * right->m[1][2]) + (double)left->m[1][2] * right->m[2][2]);
    output->m[2][0] = (float)(((double)left->m[2][0] * right->m[0][0] + (double)left->m[2][2] * right->m[2][0]) + (double)left->m[2][1] * right->m[1][0]);
    output->m[2][1] = (float)(((double)left->m[2][1] * right->m[1][1] + (double)left->m[2][0] * right->m[0][1]) + (double)left->m[2][2] * right->m[2][1]);
    output->m[2][2] = (float)(((double)left->m[2][1] * right->m[1][2] + (double)left->m[2][0] * right->m[0][2]) + (double)left->m[2][2] * right->m[2][2]);
    output->m[3][0] = (float)((((double)left->m[3][0] * right->m[0][0] + (double)left->m[3][1] * right->m[1][0]) + (double)left->m[3][2] * right->m[2][0]) + right->m[3][0]);
    output->m[3][1] = (float)((((double)left->m[3][1] * right->m[1][1] + (double)left->m[3][0] * right->m[0][1]) + (double)left->m[3][2] * right->m[2][1]) + right->m[3][1]);
    output->m[3][2] = (float)((((double)left->m[3][0] * right->m[0][2] + (double)left->m[3][1] * right->m[1][2]) + (double)left->m[3][2] * right->m[2][2]) + right->m[3][2]);
#endif
}

#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x005327c0
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

// FUNCTION: CARMA2_HW 0x005327f0
void C2_HOOK_CDECL BrMatrix34RotateX(br_matrix34* mat, br_angle rx) {
    br_scalar s;
    br_scalar c;

    s = BR_SIN(rx);
    c = BR_COS(rx);

    M(0, 0) = 1.f;
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

// FUNCTION: CARMA2_HW 0x00532860
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

// FUNCTION: CARMA2_HW 0x005328d0
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

/* C-first recovery of existing BrMatrix34Rotate532940/333B. Explicit scalar
 * spills and live wide contributions follow the raw x87 instruction trace. */
static double C2_HOOK_CDECL MatrixRotateScalar(double value) {
    union { br_scalar scalar; br_uint_32 bits; } rounded;
    volatile br_uint_32 stored;
    rounded.scalar = (br_scalar)value;
    stored = rounded.bits;
    rounded.bits = stored;
    return rounded.scalar;
}

#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
/* The original uses FSIN/FCOS, whose quarter-turn result is observably
 * different from the modern CRT. Keep these narrow transcendental operations. */
static double C2_HOOK_CDECL MatrixRotateCosine(double radians) {
    br_scalar result;
    __asm {
        fld radians
        fcos
        fstp result
    }
    return result;
}

static double C2_HOOK_CDECL MatrixRotateSine(double radians) {
    br_scalar result;
    __asm {
        fld radians
        fsin
        fstp result
    }
    return result;
}

static double C2_HOOK_CDECL MatrixRotateSineProduct(double radians, br_scalar x) {
    br_scalar result;
    __asm {
        fld radians
        fsin
        fmul x
        fstp result
    }
    return result;
}

/* Paired NaNs with equal payloads expose the modern SSE operand choice.
 * Preserve the original x87 arithmetic for these individual operations. */
static double C2_HOOK_CDECL MatrixRotateProduct(double left, double right) {
    double result;
    __asm {
        fld left
        fmul right
        fstp result
    }
    return result;
}
static double C2_HOOK_CDECL MatrixRotateSum(double left, double right) {
    double result;
    __asm {
        fld left
        fadd right
        fstp result
    }
    return result;
}
static double C2_HOOK_CDECL MatrixRotateDifference(double left, double right) {
    double result;
    __asm {
        fld left
        fsub right
        fstp result
    }
    return result;
}
#define MR_PRODUCT(l, r) MatrixRotateProduct((l), (r))
#define MR_SUM(l, r) MatrixRotateSum((l), (r))
#define MR_DIFFERENCE(l, r) MatrixRotateDifference((l), (r))
#else
#define MR_PRODUCT(l, r) ((l) * (r))
#define MR_SUM(l, r) ((l) + (r))
#define MR_DIFFERENCE(l, r) ((l) - (r))
#endif

// FUNCTION: CARMA2_HW 0x00532940
void C2_HOOK_CDECL BrMatrix34Rotate(br_matrix34* mat, br_angle r, const br_vector3* a) {
    const volatile br_scalar* input = (const volatile br_scalar*)a;
    double radians = (double)(br_uint_16)r * 0.00009587379924285257;
    double x = input[0], z = input[2], y = input[1];
#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
    double sine = MatrixRotateSine(radians), cosine = MatrixRotateCosine(radians);
#else
    double sine = sin(radians), cosine = cos(radians);
#endif
    br_scalar rounded_sine = (br_scalar)MatrixRotateScalar(sine);
    br_scalar rounded_cosine = (br_scalar)MatrixRotateScalar(cosine);
    double remainder = MR_DIFFERENCE(1.0, rounded_cosine);
    br_scalar rounded_remainder = (br_scalar)MatrixRotateScalar(remainder);
    br_scalar tx = (br_scalar)MatrixRotateScalar(MR_PRODUCT(remainder, x));
    double xz = MR_PRODUCT(z, tx), xy = MR_PRODUCT(y, tx);
    br_scalar rounded_xz = (br_scalar)MatrixRotateScalar(xz);
    br_scalar rounded_xy = (br_scalar)MatrixRotateScalar(xy);
    br_scalar yz = (br_scalar)MatrixRotateScalar(MR_PRODUCT(MR_PRODUCT(z, y), rounded_remainder));
    br_scalar rounded_x = (br_scalar)MatrixRotateScalar(x);
#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
    br_scalar sx = (br_scalar)MatrixRotateSineProduct(radians, (br_scalar)x);
#else
    br_scalar sx = (br_scalar)MatrixRotateScalar(sine * x);
#endif
    double sz = MR_PRODUCT(z, rounded_sine);
    br_scalar rounded_sz = (br_scalar)MatrixRotateScalar(sz);
    br_scalar sy = (br_scalar)MatrixRotateScalar(MR_PRODUCT(y, rounded_sine));

    M(0, 1) = (br_scalar)MatrixRotateScalar(MR_SUM(sz, rounded_xy));
    M(0, 2) = (br_scalar)MatrixRotateScalar(MR_DIFFERENCE(xz, sy));
    M(1, 0) = (br_scalar)MatrixRotateScalar(MR_DIFFERENCE(xy, rounded_sz));
    M(0, 0) = (br_scalar)MatrixRotateScalar(MR_SUM(MR_PRODUCT(MR_PRODUCT(x, rounded_x), rounded_remainder), rounded_cosine));
    y = input[1];
    M(1, 2) = (br_scalar)MatrixRotateScalar(MR_SUM((double)sx, yz));
    M(2, 0) = (br_scalar)MatrixRotateScalar(MR_SUM((double)sy, rounded_xz));
    M(2, 1) = (br_scalar)MatrixRotateScalar(MR_DIFFERENCE((double)yz, sx));
    M(1, 1) = (br_scalar)MatrixRotateScalar(MR_SUM(MR_PRODUCT(MR_PRODUCT(y, y), rounded_remainder), rounded_cosine));
    z = input[2];
    M(3, 2) = 0.0f;
    M(3, 1) = 0.0f;
    M(3, 0) = 0.0f;
    M(2, 2) = (br_scalar)MatrixRotateScalar(MR_SUM(MR_PRODUCT(MR_PRODUCT(z, z), rounded_remainder), rounded_cosine));
}
#undef MR_PRODUCT
#undef MR_SUM
#undef MR_DIFFERENCE

// FUNCTION: CARMA2_HW 0x00532a90
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

// FUNCTION: CARMA2_HW 0x00532ad0
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

#if !defined(_MSC_VER) || _MSC_VER >= 1300
/* The original shear construction copies both argument words before stores.
 * A float assignment through x87 would quiet a signaling NaN in mattmp2. */
static void C2_HOOK_CDECL BrMatrix34RawShearX(br_matrix34* mat, const br_scalar* sy, const br_scalar* sz) {
    br_uint_32 bits_y;
    br_uint_32 bits_z;
    volatile br_uint_32 barrier_y;
    volatile br_uint_32 barrier_z;

    memcpy(&bits_y, sy, sizeof(bits_y));
    memcpy(&bits_z, sz, sizeof(bits_z));
    barrier_y = bits_y;
    barrier_z = bits_z;
    bits_y = barrier_y;
    bits_z = barrier_z;
    M(0, 0) = 1.f;
    memcpy(&M(0, 1), &bits_y, sizeof(bits_y));
    memcpy(&M(0, 2), &bits_z, sizeof(bits_z));
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
#endif

// FUNCTION: CARMA2_HW 0x00532b10
void C2_HOOK_CDECL BrMatrix34ShearX(br_matrix34* mat, br_scalar sy, br_scalar sz) {
#if defined(_MSC_VER) && _MSC_VER < 1300
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
#else
    BrMatrix34RawShearX(mat, &sy, &sz);
#endif
}

// FUNCTION: CARMA2_HW 0x00532b50
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

// FUNCTION: CARMA2_HW 0x00532b90
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

// FUNCTION: CARMA2_HW 0x00532bd0
br_scalar C2_HOOK_CDECL BrMatrix34Inverse(br_matrix34* B, const br_matrix34* A) {
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

// FUNCTION: CARMA2_HW 0x00532eb0
void C2_HOOK_CDECL BrMatrix34LPInverse(br_matrix34* A, const br_matrix34* B) {
    /* Keep original x87 operand forms where the compiler otherwise changes
     * NaN payload selection or quieting; do not normalize public values. */
#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
    __asm {
        mov ecx, B
        mov eax, A
        mov edx, dword ptr [ecx]
        mov dword ptr [eax], edx
        mov edx, dword ptr [ecx + 0xc]
        mov dword ptr [eax + 4], edx
        mov edx, dword ptr [ecx + 0x18]
        mov dword ptr [eax + 8], edx
        mov edx, dword ptr [ecx + 4]
        mov dword ptr [eax + 0xc], edx
        mov edx, dword ptr [ecx + 0x10]
        mov dword ptr [eax + 0x10], edx
        mov edx, dword ptr [ecx + 0x1c]
        mov dword ptr [eax + 0x14], edx
        mov edx, dword ptr [ecx + 8]
        mov dword ptr [eax + 0x18], edx
        mov edx, dword ptr [ecx + 0x14]
        fld dword ptr [eax + 0x18]
        mov dword ptr [eax + 0x1c], edx
        mov edx, dword ptr [ecx + 0x20]
        mov dword ptr [eax + 0x20], edx
        fld dword ptr [ecx + 0x24]
        fld dword ptr [ecx + 0x2c]
        fld dword ptr [ecx + 0x28]
        fxch st(2)
        fmul dword ptr [eax]
        fxch st(3)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0xc]
        fld dword ptr [eax + 0x1c]
        fxch st(1)
        faddp st(2), st(0)
        fxch st(1)
        fchs
        fstp dword ptr [eax + 0x24]
        fld dword ptr [ecx + 0x2c]
        fld dword ptr [ecx + 0x24]
        fld dword ptr [ecx + 0x28]
        fxch st(2)
        fmulp st(3), st(0)
        fld dword ptr [eax + 0x10]
        fxch st(1)
        fmul dword ptr [eax + 4]
        fxch st(1)
        fxch st(3)
        fxch st(2)
        fxch st(1)
        faddp st(2), st(0)
        fxch st(2)
        fmulp st(2), st(0)
        fld dword ptr [eax + 0x20]
        fxch st(2)
        faddp st(1), st(0)
        fchs
        fstp dword ptr [eax + 0x28]
        fld dword ptr [ecx + 0x2c]
        fld dword ptr [ecx + 0x24]
        fld dword ptr [ecx + 0x28]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [eax + 8]
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x14]
        faddp st(1), st(0)
        fchs
        fstp dword ptr [eax + 0x2c]
    }
#else
    volatile br_matrix34* output = A;
    const volatile br_matrix34* input = B;
    br_uint_32 scalar_bits;

    /* Original transpose entries are raw MOV copies, preserving sNaN bits.
     * A local integer copy also retains sequential partial-overlap behavior. */
    memcpy(&scalar_bits, &B(0, 0), sizeof(scalar_bits));
    memcpy(&A(0, 0), &scalar_bits, sizeof(scalar_bits));
    memcpy(&scalar_bits, &B(1, 0), sizeof(scalar_bits));
    memcpy(&A(0, 1), &scalar_bits, sizeof(scalar_bits));
    memcpy(&scalar_bits, &B(2, 0), sizeof(scalar_bits));
    memcpy(&A(0, 2), &scalar_bits, sizeof(scalar_bits));
    memcpy(&scalar_bits, &B(0, 1), sizeof(scalar_bits));
    memcpy(&A(1, 0), &scalar_bits, sizeof(scalar_bits));
    memcpy(&scalar_bits, &B(1, 1), sizeof(scalar_bits));
    memcpy(&A(1, 1), &scalar_bits, sizeof(scalar_bits));
    memcpy(&scalar_bits, &B(2, 1), sizeof(scalar_bits));
    memcpy(&A(1, 2), &scalar_bits, sizeof(scalar_bits));
    memcpy(&scalar_bits, &B(0, 2), sizeof(scalar_bits));
    memcpy(&A(2, 0), &scalar_bits, sizeof(scalar_bits));
    memcpy(&scalar_bits, &B(1, 2), sizeof(scalar_bits));
    memcpy(&A(2, 1), &scalar_bits, sizeof(scalar_bits));
    memcpy(&scalar_bits, &B(2, 2), sizeof(scalar_bits));
    memcpy(&A(2, 2), &scalar_bits, sizeof(scalar_bits));

    output->m[3][0] = (float)(-(((double)input->m[3][0] * output->m[0][0] + (double)input->m[3][2] * output->m[2][0]) + (double)input->m[3][1] * output->m[1][0]));
    output->m[3][1] = (float)(-(((double)output->m[2][1] * input->m[3][2] + (double)input->m[3][0] * output->m[0][1]) + (double)input->m[3][1] * output->m[1][1]));
    output->m[3][2] = (float)(-(((double)output->m[2][2] * input->m[3][2] + (double)input->m[3][0] * output->m[0][2]) + (double)input->m[3][1] * output->m[1][2]));
#endif
}

// FUNCTION: CARMA2_HW 0x00532f60
void C2_HOOK_CDECL BrMatrix34LPNormalise(br_matrix34* A, const br_matrix34* B) {

    BrVector3Normalise((br_vector3*)A->m[2], (br_vector3*)B->m[2]);
    BrVector3Cross((br_vector3*)A->m[0], (br_vector3*)B->m[1], (br_vector3*)A->m[2]);
    BrVector3Normalise((br_vector3*)A->m[0], (br_vector3*)A->m[0]);
    BrVector3Cross((br_vector3*)A->m[1], (br_vector3*)A->m[2], (br_vector3*)A->m[0]);

    A(3, 0) = B(3, 0);
    A(3, 1) = B(3, 1);
    A(3, 2) = B(3, 2);
}

// FUNCTION: CARMA2_HW 0x00533100
void C2_HOOK_CDECL BrMatrix34RollingBall(br_matrix34* mat, int dx, int dy, int radius) {
    br_scalar nx;
    br_scalar ny;
    br_scalar ca;
    br_scalar sa;
    br_scalar dr;
    br_scalar h;

    // The rolling ball, Graphics Gems III (1993), pages 51-60, Academic Press

    dr = (br_scalar)sqrt((float)(dx * dx + dy * dy));
    if (dr == BR_SCALAR(.0f)) {
        BrMatrix34Identity(mat);
        return;
    }
    h = (br_scalar)sqrt(dr * dr + radius * radius);
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

// FUNCTION: CARMA2_HW 0x00533290
br_matrix34* C2_HOOK_CDECL BrBoundsToMatrix34(br_matrix34* mat, const br_bounds* bounds) {
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

// FUNCTION: CARMA2_HW 0x00533350
void C2_HOOK_CDECL BrMatrix34Copy4(br_matrix34* A, const br_matrix4* B) {

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

// FUNCTION: CARMA2_HW 0x005333a0
void C2_HOOK_CDECL BrMatrix34TApplyFV(br_vector3* A, const br_fvector3* B, const br_matrix34* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
}

/* The paired-NaN C-first counterexample requires original operand forms.
 * Each kernel retains one arithmetic operation; C retains grouping and stores.
 * Products of scalar inputs and their sums fit the double exponent range, so
 * private double stores preserve the original PC53 intermediate values. */
static double C2_HOOK_CDECL Matrix34ApplyProductLoaded(const br_scalar* a, const br_scalar* b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, a
        mov ecx, b
        fld dword ptr [eax]
        fld dword ptr [ecx]
        fmulp st(1), st(0)
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; flds %2; fmulp %%st, %%st(1); fstpl %0"
        : "=m"(result) : "m"(*a), "m"(*b) : "st", "st(1)");
#else
    result=(double)*a * *b;
#endif
    return result;
}

static double C2_HOOK_CDECL Matrix34ApplyProductMemory(const br_scalar* a, const br_scalar* b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, a
        mov ecx, b
        fld dword ptr [eax]
        fmul dword ptr [ecx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fmuls %2; fstpl %0"
        : "=m"(result) : "m"(*a), "m"(*b) : "st");
#else
    result=(double)*a * *b;
#endif
    return result;
}

static double C2_HOOK_CDECL Matrix34ApplyAdd(double a, double b) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr a
        fld qword ptr b
        faddp st(1), st(0)
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fldl %2; faddp %%st, %%st(1); fstpl %0"
        : "=m"(result) : "m"(a), "m"(b) : "st", "st(1)");
#else
    result=a+b;
#endif
    return result;
}

// FUNCTION: CARMA2_HW 0x00533410
void C2_HOOK_CDECL BrMatrix34Apply(br_vector3* A, const br_vector4* B, const br_matrix34* C) {
    volatile br_vector3* output=A;
    double first,second,third,fourth;
    first=Matrix34ApplyProductLoaded(&C->m[3][0],&B->v[3]);
    second=Matrix34ApplyProductLoaded(&C->m[2][0],&B->v[2]);
    third=Matrix34ApplyProductMemory(&C->m[1][0],&B->v[1]);
    fourth=Matrix34ApplyProductMemory(&C->m[0][0],&B->v[0]);
    output->v[0]=(float)Matrix34ApplyAdd(Matrix34ApplyAdd(Matrix34ApplyAdd(first,second),third),fourth);
    first=Matrix34ApplyProductLoaded(&C->m[3][1],&B->v[3]);
    second=Matrix34ApplyProductLoaded(&C->m[1][1],&B->v[1]);
    third=Matrix34ApplyProductMemory(&C->m[2][1],&B->v[2]);
    fourth=Matrix34ApplyProductMemory(&C->m[0][1],&B->v[0]);
    output->v[1]=(float)Matrix34ApplyAdd(Matrix34ApplyAdd(Matrix34ApplyAdd(first,second),third),fourth);
    first=Matrix34ApplyProductLoaded(&C->m[2][2],&B->v[2]);
    second=Matrix34ApplyProductLoaded(&C->m[3][2],&B->v[3]);
    third=Matrix34ApplyProductMemory(&C->m[1][2],&B->v[1]);
    fourth=Matrix34ApplyProductMemory(&C->m[0][2],&B->v[0]);
    output->v[2]=(float)Matrix34ApplyAdd(Matrix34ApplyAdd(Matrix34ApplyAdd(first,second),third),fourth);
}

// FUNCTION: CARMA2_HW 0x005334a0
void C2_HOOK_CDECL BrMatrix34ApplyP(br_vector3* A, const br_vector3* B, const br_matrix34* C) {
    /* Keep original x87 operand forms where the compiler otherwise changes
     * NaN payload selection or quieting; do not normalize public values. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, C
        mov ecx, B
        mov edx, A
        fld dword ptr [eax + 0xc]
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 4]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax]
        fxch st(2)
        fmulp st(4), st(0)
        fmulp st(2), st(0)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x24]
        fstp dword ptr [edx]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x10]
        fld dword ptr [eax + 4]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x28]
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 0x20]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x14]
        fld dword ptr [eax + 8]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x2c]
        fstp dword ptr [edx + 8]
    }
#elif defined(__GNUC__) && defined(__i386__)
    /* Scalar C can reverse x87 NaN operands even with explicit double casts.
     * Match each original load, arithmetic operand and sequential output. */
    __asm__ __volatile__(
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [eax + 0x18]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [eax]\n\t"
        "fxch st(2)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x24]\n\t"
        "fstp dword ptr [edx]\n\t"
        "fld dword ptr [eax + 0x1c]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fxch st(2)\n\t"
        "fmulp st(3), st(0)\n\t"
        "fmul dword ptr [ecx + 4]\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x28]\n\t"
        "fstp dword ptr [edx + 4]\n\t"
        "fld dword ptr [eax + 0x20]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fxch st(2)\n\t"
        "fmulp st(3), st(0)\n\t"
        "fmul dword ptr [ecx + 4]\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x2c]\n\t"
        "fstp dword ptr [edx + 8]\n\t"
        ".att_syntax prefix\n\t"
        :
        : "a" (C), "c" (B), "d" (A)
        : "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)"
    );
#else
    volatile br_vector3* output = A;
    const volatile br_vector3* input = B;
    const volatile br_matrix34* matrix = C;

    /* Original x87 stores each coordinate before reloading input and matrix
     * values for the next. Retain that order for overlapping operands, and
     * retain the original PC53 product/sum precision before each scalar store. */
    output->v[0] = (float)((((double)input->v[1] * matrix->m[1][0] + (double)input->v[2] * matrix->m[2][0]) + (double)input->v[0] * matrix->m[0][0]) + matrix->m[3][0]);
    output->v[1] = (float)((((double)input->v[2] * matrix->m[2][1] + (double)input->v[1] * matrix->m[1][1]) + (double)input->v[0] * matrix->m[0][1]) + matrix->m[3][1]);
    output->v[2] = (float)((((double)input->v[2] * matrix->m[2][2] + (double)input->v[1] * matrix->m[1][2]) + (double)input->v[0] * matrix->m[0][2]) + matrix->m[3][2]);
#endif
}

// FUNCTION: CARMA2_HW 0x00533520
void C2_HOOK_CDECL BrMatrix34ApplyV(br_vector3* A, const br_vector3* B, const br_matrix34* C) {
    /* Keep original x87 operand forms where the compiler otherwise changes
     * NaN payload selection or quieting; do not normalize public values. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, C
        mov ecx, B
        mov edx, A
        fld dword ptr [eax + 0x18]
        fld dword ptr [eax + 0xc]
        fld dword ptr [ecx + 8]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax]
        fxch st(2)
        fmulp st(4), st(0)
        fmulp st(2), st(0)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x10]
        fld dword ptr [eax + 4]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x20]
        fld dword ptr [eax + 8]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 8]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 8]
    }
#else
    volatile br_vector3* output = A;
    const volatile br_vector3* input = B;
    const volatile br_matrix34* matrix = C;

    output->v[0] = (float)(((double)matrix->m[2][0] * input->v[2] + (double)matrix->m[1][0] * input->v[1]) + (double)matrix->m[0][0] * input->v[0]);
    output->v[1] = (float)(((double)matrix->m[2][1] * input->v[2] + (double)matrix->m[1][1] * input->v[1]) + (double)matrix->m[0][1] * input->v[0]);
    output->v[2] = (float)(((double)matrix->m[1][2] * input->v[1] + (double)matrix->m[2][2] * input->v[2]) + (double)matrix->m[0][2] * input->v[0]);
#endif
}

// FUNCTION: CARMA2_HW 0x00533590
void C2_HOOK_CDECL BrMatrix34TApply(br_vector4* A, const br_vector4* B, const br_matrix34* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
    A->v[3] = BR_MAC3(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2)) + B->v[3];
}

// FUNCTION: CARMA2_HW 0x00533620
void C2_HOOK_CDECL BrMatrix34TApplyP(br_vector3* A, const br_vector3* B, const br_matrix34* C) {

    // translation elements are presumed zero or irrelevant
    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
}

// FUNCTION: CARMA2_HW 0x00533690
void C2_HOOK_CDECL BrMatrix34TApplyV(br_vector3* A, const br_vector3* B, const br_matrix34* C) {
    /* Keep original x87 operand forms where the compiler otherwise changes
     * NaN payload selection or quieting; do not normalize public values. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, C
        mov ecx, B
        mov edx, A
        fld dword ptr [eax + 8]
        fld dword ptr [eax + 4]
        fld dword ptr [ecx + 8]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax]
        fxch st(2)
        fmulp st(4), st(0)
        fmulp st(2), st(0)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx]
        fld dword ptr [eax + 0x10]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x14]
        fld dword ptr [eax + 0xc]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 8]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x20]
        fld dword ptr [eax + 0x18]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 8]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 8]
    }
#else
    volatile br_vector3* output = A;
    const volatile br_vector3* input = B;
    const volatile br_matrix34* matrix = C;

    output->v[0] = (float)(((double)matrix->m[0][2] * input->v[2] + (double)matrix->m[0][1] * input->v[1]) + (double)matrix->m[0][0] * input->v[0]);
    output->v[1] = (float)(((double)matrix->m[1][1] * input->v[1] + (double)matrix->m[1][2] * input->v[2]) + (double)matrix->m[1][0] * input->v[0]);
    output->v[2] = (float)(((double)matrix->m[2][1] * input->v[1] + (double)matrix->m[2][2] * input->v[2]) + (double)matrix->m[2][0] * input->v[0]);
#endif
}

// FUNCTION: CARMA2_HW 0x00533700
void C2_HOOK_CDECL BrMatrix34Pre(br_matrix34* mat, const br_matrix34* A) {

    BrMatrix34Mul(&mattmp1, A, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533730
void C2_HOOK_CDECL BrMatrix34Post(br_matrix34* mat, const br_matrix34* A) {

    BrMatrix34Mul(&mattmp1, mat, A);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533760
void C2_HOOK_CDECL BrMatrix34PreRotateX(br_matrix34* mat, br_angle rx) {

    BrMatrix34RotateX(&mattmp2, rx);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533860
void C2_HOOK_CDECL BrMatrix34PostRotateX(br_matrix34* mat, br_angle rx) {

    BrMatrix34RotateX(&mattmp2, rx);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533960
void C2_HOOK_CDECL BrMatrix34PreRotateY(br_matrix34* mat, br_angle ry) {

    BrMatrix34RotateY(&mattmp2, ry);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533a60
void C2_HOOK_CDECL BrMatrix34PostRotateY(br_matrix34* mat, br_angle ry) {

    BrMatrix34RotateY(&mattmp2, ry);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533b60
void C2_HOOK_CDECL BrMatrix34PreRotateZ(br_matrix34* mat, br_angle rz) {

    BrMatrix34RotateZ(&mattmp2, rz);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533c50
void C2_HOOK_CDECL BrMatrix34PostRotateZ(br_matrix34* mat, br_angle rz) {

    BrMatrix34RotateZ(&mattmp2, rz);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533d40
void C2_HOOK_CDECL BrMatrix34PreRotate(br_matrix34* mat, br_angle r, const br_vector3* axis) {

    BrMatrix34Rotate(&mattmp2, r, axis);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533d80
void C2_HOOK_CDECL BrMatrix34PostRotate(br_matrix34* mat, br_angle r, const br_vector3* axis) {

    BrMatrix34Rotate(&mattmp2, r, axis);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533dc0
void C2_HOOK_CDECL BrMatrix34PreTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z) {

    BrMatrix34Translate(&mattmp2, x, y, z);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

/* Complete existing BrMatrix34PostTranslate533e60/45B. Original FADD32
 * chooses opposite-sign paired NaNs differently from modern SSE arithmetic. */
static br_scalar C2_HOOK_CDECL MatrixPostTranslateAdd(br_scalar value, const br_scalar* existing) {
    br_scalar result;
#if defined(_MSC_VER) && _MSC_VER >= 1300 && defined(_M_IX86)
    __asm {
        mov eax, existing
        fld value
        fadd dword ptr [eax]
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fadds %2; fstps %0"
        : "=m"(result) : "m"(value), "m"(*existing) : "st");
#else
    result = value + *existing;
#endif
    return result;
}

// FUNCTION: CARMA2_HW 0x00533e60
void C2_HOOK_CDECL BrMatrix34PostTranslate(br_matrix34* mat, br_scalar x, br_scalar y, br_scalar z) {
    br_scalar dx = MatrixPostTranslateAdd(x, &M(3, 0));
    br_scalar dy = MatrixPostTranslateAdd(y, &M(3, 1));
    br_scalar dz = MatrixPostTranslateAdd(z, &M(3, 2));
    M(3, 0) = dx;
    M(3, 1) = dy;
    M(3, 2) = dz;
}

// FUNCTION: CARMA2_HW 0x00533e90
void C2_HOOK_CDECL BrMatrix34PreScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    BrMatrix34Scale(&mattmp2, sx, sy, sz);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00533f30
void C2_HOOK_CDECL BrMatrix34PostScale(br_matrix34* mat, br_scalar sx, br_scalar sy, br_scalar sz) {

    BrMatrix34Scale(&mattmp1, sx, sy, sz);
    BrMatrix34Mul(&mattmp2, mat, &mattmp1);
    BrMatrix34Copy(mat, &mattmp2);
}

// FUNCTION: CARMA2_HW 0x00533fd0
void C2_HOOK_CDECL BrMatrix34PreShearX(br_matrix34* mat, br_scalar sy, br_scalar sz) {
#if defined(_MSC_VER) && _MSC_VER < 1300
    BrMatrix34ShearX(&mattmp2, sy, sz);
#else
    BrMatrix34RawShearX(&mattmp2, &sy, &sz);
#endif
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00534070
void C2_HOOK_CDECL BrMatrix34PostShearX(br_matrix34* mat, br_scalar sy, br_scalar sz) {

    BrMatrix34ShearX(&mattmp2, sy, sz);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00534110
void C2_HOOK_CDECL BrMatrix34PreShearY(br_matrix34* mat, br_scalar sx, br_scalar sz) {

    BrMatrix34ShearY(&mattmp2, sx, sz);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x005341b0
void C2_HOOK_CDECL BrMatrix34PostShearY(br_matrix34* mat, br_scalar sx, br_scalar sz) {

    BrMatrix34ShearY(&mattmp2, sx, sz);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x00534250
void C2_HOOK_CDECL BrMatrix34PreShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy) {

    BrMatrix34ShearZ(&mattmp2, sx, sy);
    BrMatrix34Mul(&mattmp1, &mattmp2, mat);
    BrMatrix34Copy(mat, &mattmp1);
}

// FUNCTION: CARMA2_HW 0x005342f0
void C2_HOOK_CDECL BrMatrix34PostShearZ(br_matrix34* mat, br_scalar sx, br_scalar sy) {

    BrMatrix34ShearZ(&mattmp2, sx, sy);
    BrMatrix34Mul(&mattmp1, mat, &mattmp2);
    BrMatrix34Copy(mat, &mattmp1);
}
