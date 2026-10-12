#include "matrix4.h"

#include "c2_math.h"
#include "c2_string.h"

#define A(x, y) A->m[x][y]
#define B(x, y) B->m[x][y]
#define C(x, y) C->m[x][y]
#define M(x, y) mat->m[x][y]


// FUNCTION: CARMA2_HW 0x00535cd0
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

// FUNCTION: CARMA2_HW 0x00535d40
void C2_HOOK_CDECL BrMatrix4Mul(br_matrix4* A, const br_matrix4* B, const br_matrix4* C) {
    /* Preserve original x87 summation and sequential source reloads. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, C
        mov ecx, B
        mov edx, A
        fld dword ptr [ecx + 0xc]
        fld dword ptr [eax + 0x30]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x20]
        fld dword ptr [ecx + 4]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x10]
        fld dword ptr [eax]
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx]
        fld dword ptr [eax + 4]
        fld dword ptr [eax + 0x24]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x14]
        fxch st(3)
        fmul dword ptr [ecx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [ecx + 4]
        fld dword ptr [eax + 0x34]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [ecx + 0xc]
        faddp st(1), st(0)
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 8]
        fld dword ptr [eax + 0x38]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [eax + 0x18]
        fxch st(3)
        fmul dword ptr [ecx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [ecx + 4]
        fld dword ptr [eax + 0x28]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [ecx + 8]
        faddp st(1), st(0)
        fstp dword ptr [edx + 8]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0xc]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [eax + 0x3c]
        fxch st(2)
        fmul dword ptr [ecx]
        fxch st(3)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [ecx + 0xc]
        fld dword ptr [eax + 0x2c]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [ecx + 8]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0xc]
        fld dword ptr [ecx + 0x10]
        fld dword ptr [ecx + 0x1c]
        fld dword ptr [eax + 0x30]
        fld dword ptr [ecx + 0x18]
        fxch st(3)
        fmul dword ptr [eax]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [eax + 0x20]
        fld dword ptr [ecx + 0x14]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x10]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x10]
        fld dword ptr [ecx + 0x10]
        fld dword ptr [eax + 4]
        fld dword ptr [ecx + 0x1c]
        fld dword ptr [eax + 0x34]
        fld dword ptr [ecx + 0x18]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x24]
        fld dword ptr [ecx + 0x14]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x14]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x14]
        fld dword ptr [ecx + 0x14]
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 0x18]
        fld dword ptr [eax + 0x28]
        fld dword ptr [ecx + 0x10]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 8]
        fld dword ptr [ecx + 0x1c]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x38]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x18]
        fld dword ptr [ecx + 0x10]
        fld dword ptr [eax + 0xc]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [ecx + 0x14]
        fld dword ptr [ecx + 0x18]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x2c]
        fld dword ptr [ecx + 0x1c]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x3c]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x1c]
        fld dword ptr [ecx + 0x20]
        fld dword ptr [ecx + 0x28]
        fld dword ptr [eax + 0x20]
        fld dword ptr [ecx + 0x24]
        fxch st(3)
        fmul dword ptr [eax]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [eax + 0x10]
        fld dword ptr [ecx + 0x2c]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x30]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x20]
        fld dword ptr [ecx + 0x28]
        fld dword ptr [eax + 0x24]
        fld dword ptr [ecx + 0x24]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 0x2c]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x34]
        fld dword ptr [ecx + 0x20]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 4]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x24]
        fld dword ptr [ecx + 0x28]
        fld dword ptr [eax + 0x28]
        fld dword ptr [ecx + 0x20]
        fld dword ptr [eax + 8]
        fld dword ptr [ecx + 0x24]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x18]
        fld dword ptr [ecx + 0x2c]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x38]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x28]
        fld dword ptr [ecx + 0x28]
        fld dword ptr [eax + 0x2c]
        fld dword ptr [ecx + 0x2c]
        fld dword ptr [eax + 0x3c]
        fld dword ptr [ecx + 0x20]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0xc]
        fld dword ptr [ecx + 0x24]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x1c]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x2c]
        fld dword ptr [ecx + 0x30]
        fld dword ptr [ecx + 0x34]
        fld dword ptr [eax + 0x10]
        fld dword ptr [ecx + 0x3c]
        fxch st(3)
        fmul dword ptr [eax]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [eax + 0x30]
        fld dword ptr [ecx + 0x38]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x20]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x30]
        fld dword ptr [ecx + 0x34]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 0x3c]
        fld dword ptr [eax + 0x34]
        fld dword ptr [ecx + 0x30]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 4]
        fld dword ptr [ecx + 0x38]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x24]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x34]
        fld dword ptr [ecx + 0x38]
        fld dword ptr [eax + 0x28]
        fld dword ptr [ecx + 0x34]
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 0x30]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 8]
        fld dword ptr [ecx + 0x3c]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x38]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x38]
        fld dword ptr [ecx + 0x30]
        fld dword ptr [eax + 0xc]
        fld dword ptr [ecx + 0x34]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [ecx + 0x38]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x2c]
        fld dword ptr [ecx + 0x3c]
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x3c]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x3c]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "movl %[matrix], %%eax\n\t"
        "movl %[input], %%ecx\n\t"
        "movl %[output], %%edx\n\t"
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fld dword ptr [eax + 0x30]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [eax + 0x20]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [eax]\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fld dword ptr [eax + 0x24]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fxch st(3)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx + 4]\n\t"
        "fld dword ptr [eax + 0x34]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [ecx + 0xc]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 4]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [eax + 0x38]\n\t"
        "fld dword ptr [ecx + 0xc]\n\t"
        "fld dword ptr [eax + 0x18]\n\t"
        "fxch st(3)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx + 4]\n\t"
        "fld dword ptr [eax + 0x28]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [ecx + 8]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 8]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [eax + 0x1c]\n\t"
        "fld dword ptr [eax + 0x3c]\n\t"
        "fxch st(2)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [ecx + 0xc]\n\t"
        "fld dword ptr [eax + 0x2c]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [ecx + 8]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0xc]\n\t"
        "fld dword ptr [ecx + 0x10]\n\t"
        "fld dword ptr [ecx + 0x1c]\n\t"
        "fld dword ptr [eax + 0x30]\n\t"
        "fld dword ptr [ecx + 0x18]\n\t"
        "fxch st(3)\n\t"
        "fmul dword ptr [eax]\n\t"
        "fxch st(1)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [eax + 0x20]\n\t"
        "fld dword ptr [ecx + 0x14]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x10]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x10]\n\t"
        "fld dword ptr [ecx + 0x10]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fld dword ptr [ecx + 0x1c]\n\t"
        "fld dword ptr [eax + 0x34]\n\t"
        "fld dword ptr [ecx + 0x18]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x24]\n\t"
        "fld dword ptr [ecx + 0x14]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x14]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x14]\n\t"
        "fld dword ptr [ecx + 0x14]\n\t"
        "fld dword ptr [eax + 0x18]\n\t"
        "fld dword ptr [ecx + 0x18]\n\t"
        "fld dword ptr [eax + 0x28]\n\t"
        "fld dword ptr [ecx + 0x10]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 8]\n\t"
        "fld dword ptr [ecx + 0x1c]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x38]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x18]\n\t"
        "fld dword ptr [ecx + 0x10]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [eax + 0x1c]\n\t"
        "fld dword ptr [ecx + 0x14]\n\t"
        "fld dword ptr [ecx + 0x18]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x2c]\n\t"
        "fld dword ptr [ecx + 0x1c]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x3c]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x1c]\n\t"
        "fld dword ptr [ecx + 0x20]\n\t"
        "fld dword ptr [ecx + 0x28]\n\t"
        "fld dword ptr [eax + 0x20]\n\t"
        "fld dword ptr [ecx + 0x24]\n\t"
        "fxch st(3)\n\t"
        "fmul dword ptr [eax]\n\t"
        "fxch st(1)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [ecx + 0x2c]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x30]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x20]\n\t"
        "fld dword ptr [ecx + 0x28]\n\t"
        "fld dword ptr [eax + 0x24]\n\t"
        "fld dword ptr [ecx + 0x24]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fld dword ptr [ecx + 0x2c]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x34]\n\t"
        "fld dword ptr [ecx + 0x20]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 4]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x24]\n\t"
        "fld dword ptr [ecx + 0x28]\n\t"
        "fld dword ptr [eax + 0x28]\n\t"
        "fld dword ptr [ecx + 0x20]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [ecx + 0x24]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x18]\n\t"
        "fld dword ptr [ecx + 0x2c]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x38]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x28]\n\t"
        "fld dword ptr [ecx + 0x28]\n\t"
        "fld dword ptr [eax + 0x2c]\n\t"
        "fld dword ptr [ecx + 0x2c]\n\t"
        "fld dword ptr [eax + 0x3c]\n\t"
        "fld dword ptr [ecx + 0x20]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [ecx + 0x24]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x1c]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x2c]\n\t"
        "fld dword ptr [ecx + 0x30]\n\t"
        "fld dword ptr [ecx + 0x34]\n\t"
        "fld dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [ecx + 0x3c]\n\t"
        "fxch st(3)\n\t"
        "fmul dword ptr [eax]\n\t"
        "fxch st(1)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [eax + 0x30]\n\t"
        "fld dword ptr [ecx + 0x38]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x20]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x30]\n\t"
        "fld dword ptr [ecx + 0x34]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fld dword ptr [ecx + 0x3c]\n\t"
        "fld dword ptr [eax + 0x34]\n\t"
        "fld dword ptr [ecx + 0x30]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 4]\n\t"
        "fld dword ptr [ecx + 0x38]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x24]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x34]\n\t"
        "fld dword ptr [ecx + 0x38]\n\t"
        "fld dword ptr [eax + 0x28]\n\t"
        "fld dword ptr [ecx + 0x34]\n\t"
        "fld dword ptr [eax + 0x18]\n\t"
        "fld dword ptr [ecx + 0x30]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 8]\n\t"
        "fld dword ptr [ecx + 0x3c]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x38]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x38]\n\t"
        "fld dword ptr [ecx + 0x30]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [ecx + 0x34]\n\t"
        "fld dword ptr [eax + 0x1c]\n\t"
        "fld dword ptr [ecx + 0x38]\n\t"
        "fxch st(3)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(1), st(0)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x2c]\n\t"
        "fld dword ptr [ecx + 0x3c]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "fmul dword ptr [eax + 0x3c]\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [edx + 0x3c]\n\t"
        ".att_syntax prefix\n\t"
        :
        : [matrix] "m" (C), [input] "m" (B), [output] "m" (A)
        : "eax", "ecx", "edx", "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)");
#else
#error BrMatrix4Mul requires the original x86 floating-point behavior
#endif
}

// FUNCTION: CARMA2_HW 0x00535ff0
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

// FUNCTION: CARMA2_HW 0x00536030
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

// FUNCTION: CARMA2_HW 0x00536080
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

// FUNCTION: CARMA2_HW 0x005367b0
br_scalar C2_HOOK_STDCALL Determinant3(br_scalar a1, br_scalar a2, br_scalar a3, br_scalar b1, br_scalar b2, br_scalar b3, br_scalar c1, br_scalar c2, br_scalar c3) {

    return BR_MAC3(a1, BR_MAC2(b2, c3, -b3, c2),
        -b1, BR_MAC2(a2, c3, -a3, c2),
        c1, BR_MAC2(a2, b3, -a3, b2));
}

// FUNCTION: CARMA2_HW 0x005360f0
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

// FUNCTION: CARMA2_HW 0x005362c0
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

// FUNCTION: CARMA2_HW 0x00536820
void C2_HOOK_CDECL BrMatrix4Perspective(br_matrix4* mat, br_angle field_of_view, br_scalar aspect, br_scalar hither, br_scalar yon) {
    const double angle_scale = 9.587379924285257e-05;
    const br_scalar minus_two = -2.f;
    br_uint_32 arguments[4];

#if defined(__GNUC__) && defined(__i386__)
    br_uint_32* argument_ptr = arguments;
    const double* angle_ptr = &angle_scale;
    const br_scalar* minus_ptr = &minus_two;
#endif

    arguments[0] = (br_uint_16)field_of_view >> 1;
    memcpy(arguments + 1, &aspect, 4);
    memcpy(arguments + 2, &hither, 4);
    memcpy(arguments + 3, &yon, 4);
    /* Preserve the original FCOS/FSIN operands and the scalar denominator
     * spill, while keeping the quotient live through the aspect division. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, mat
        lea edx, arguments
        xor ecx, ecx
        mov dword ptr [eax + 4], ecx
        fild dword ptr [edx]
        fld dword ptr [edx + 0xc]
        fsub dword ptr [edx + 8]
        fxch st(1)
        fmul qword ptr angle_scale
        fld dword ptr [edx + 0xc]
        fmul dword ptr [edx + 8]
        fld st(1)
        fld st(2)
        fxch st(1)
        fcos
        mov dword ptr [eax + 8], ecx
        mov dword ptr [eax + 0xc], ecx
        mov dword ptr [eax + 0x10], ecx
        mov dword ptr [eax + 0x18], ecx
        mov dword ptr [eax + 0x1c], ecx
        mov dword ptr [eax + 0x20], ecx
        mov dword ptr [eax + 0x24], ecx
        mov dword ptr [eax + 0x2c], 0xbf800000
        mov dword ptr [eax + 0x30], ecx
        mov dword ptr [eax + 0x34], ecx
        mov dword ptr [eax + 0x3c], ecx
        fxch st(1)
        fsin
        fxch st(1)
        fst dword ptr [edx]
        fxch st(1)
        fst dword ptr [edx]
        fdivp st(1), st(0)
        fld dword ptr [edx + 0xc]
        fadd dword ptr [edx + 8]
        fxch st(4)
        fstp dword ptr [edx]
        fxch st(1)
        fdiv dword ptr [edx]
        fxch st(1)
        fst dword ptr [edx + 0xc]
        fdiv dword ptr [edx + 4]
        fxch st(3)
        fdiv dword ptr [edx]
        fxch st(1)
        fmul dword ptr minus_two
        fld dword ptr [edx + 0xc]
        fxch st(4)
        fstp dword ptr [eax]
        fxch st(3)
        fstp dword ptr [eax + 0x14]
        fstp dword ptr [eax + 0x28]
        fxch st(1)
        fstp dword ptr [eax + 0x38]
        fstp st(0)
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "movl %[matrix], %%eax\n\t"
        "movl %[pack], %%edx\n\t"
        "movl %[angle], %%ebx\n\t"
        "movl %[minus], %%esi\n\t"
        ".intel_syntax noprefix\n\t"
        "xor ecx, ecx\n\t"
        "mov dword ptr [eax + 4], ecx\n\t"
        "fild dword ptr [edx]\n\t"
        "fld dword ptr [edx + 0xc]\n\t"
        "fsub dword ptr [edx + 8]\n\t"
        "fxch st(1)\n\t"
        "fmul qword ptr [ebx]\n\t"
        "fld dword ptr [edx + 0xc]\n\t"
        "fmul dword ptr [edx + 8]\n\t"
        "fld st(1)\n\t"
        "fld st(2)\n\t"
        "fxch st(1)\n\t"
        "fcos \n\t"
        "mov dword ptr [eax + 8], ecx\n\t"
        "mov dword ptr [eax + 0xc], ecx\n\t"
        "mov dword ptr [eax + 0x10], ecx\n\t"
        "mov dword ptr [eax + 0x18], ecx\n\t"
        "mov dword ptr [eax + 0x1c], ecx\n\t"
        "mov dword ptr [eax + 0x20], ecx\n\t"
        "mov dword ptr [eax + 0x24], ecx\n\t"
        "mov dword ptr [eax + 0x2c], 0xbf800000\n\t"
        "mov dword ptr [eax + 0x30], ecx\n\t"
        "mov dword ptr [eax + 0x34], ecx\n\t"
        "mov dword ptr [eax + 0x3c], ecx\n\t"
        "fxch st(1)\n\t"
        "fsin \n\t"
        "fxch st(1)\n\t"
        "fst dword ptr [edx]\n\t"
        "fxch st(1)\n\t"
        "fst dword ptr [edx]\n\t"
        "fdivp st(1), st(0)\n\t"
        "fld dword ptr [edx + 0xc]\n\t"
        "fadd dword ptr [edx + 8]\n\t"
        "fxch st(4)\n\t"
        "fstp dword ptr [edx]\n\t"
        "fxch st(1)\n\t"
        "fdiv dword ptr [edx]\n\t"
        "fxch st(1)\n\t"
        "fst dword ptr [edx + 0xc]\n\t"
        "fdiv dword ptr [edx + 4]\n\t"
        "fxch st(3)\n\t"
        "fdiv dword ptr [edx]\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [esi]\n\t"
        "fld dword ptr [edx + 0xc]\n\t"
        "fxch st(4)\n\t"
        "fstp dword ptr [eax]\n\t"
        "fxch st(3)\n\t"
        "fstp dword ptr [eax + 0x14]\n\t"
        "fstp dword ptr [eax + 0x28]\n\t"
        "fxch st(1)\n\t"
        "fstp dword ptr [eax + 0x38]\n\t"
        "fstp st(0)\n\t"
        ".att_syntax prefix\n\t"
        :
        : [matrix] "m" (mat), [pack] "m" (argument_ptr), [angle] "m" (angle_ptr), [minus] "m" (minus_ptr)
        : "eax", "ecx", "edx", "ebx", "esi", "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)");
#else
#error BrMatrix4Perspective requires the original x86 floating-point behavior
#endif
}

// FUNCTION: CARMA2_HW 0x005368e0
void C2_HOOK_CDECL BrMatrix4Apply(br_vector4* A, const br_vector4* B, const br_matrix4* C) {

    A->v[0] = BR_MAC4(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0), B->v[3], C(3, 0));
    A->v[1] = BR_MAC4(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1), B->v[3], C(3, 1));
    A->v[2] = BR_MAC4(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2), B->v[3], C(3, 2));
    A->v[3] = BR_MAC4(B->v[0], C(0, 3), B->v[1], C(1, 3), B->v[2], C(2, 3), B->v[3], C(3, 3));
}

// FUNCTION: CARMA2_HW 0x00536990
void C2_HOOK_CDECL BrMatrix4ApplyP(br_vector4* A, const br_vector3* B, const br_matrix4* C) {
    /* Keep the original x87 operand selection, summation and scalar stores,
     * including sequential input reloads when vectors overlap. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, C
        mov ecx, B
        mov edx, A
        fld dword ptr [eax + 0x20]
        fld dword ptr [eax + 0x10]
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
        fadd dword ptr [eax + 0x30]
        fstp dword ptr [edx]
        fld dword ptr [eax + 0x24]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x14]
        fld dword ptr [eax + 4]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x34]
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 0x28]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x18]
        fld dword ptr [eax + 8]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x38]
        fstp dword ptr [edx + 8]
        fld dword ptr [eax + 0x2c]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [eax + 0xc]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x3c]
        fstp dword ptr [edx + 0xc]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "movl %[matrix], %%eax\n\t"
        "movl %[input], %%ecx\n\t"
        "movl %[output], %%edx\n\t"
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [eax + 0x20]\n\t"
        "fld dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [ecx + 4]\n\t"
        "fld dword ptr [eax]\n\t"
        "fxch st(2)\n\t"
        "fmulp st(4), st(0)\n\t"
        "fmulp st(2), st(0)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x30]\n\t"
        "fstp dword ptr [edx]\n\t"
        "fld dword ptr [eax + 0x24]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fxch st(2)\n\t"
        "fmulp st(3), st(0)\n\t"
        "fmul dword ptr [ecx + 4]\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x34]\n\t"
        "fstp dword ptr [edx + 4]\n\t"
        "fld dword ptr [eax + 0x28]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [eax + 0x18]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fxch st(2)\n\t"
        "fmulp st(3), st(0)\n\t"
        "fmul dword ptr [ecx + 4]\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x38]\n\t"
        "fstp dword ptr [edx + 8]\n\t"
        "fld dword ptr [eax + 0x2c]\n\t"
        "fld dword ptr [ecx + 8]\n\t"
        "fld dword ptr [eax + 0x1c]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fxch st(2)\n\t"
        "fmulp st(3), st(0)\n\t"
        "fmul dword ptr [ecx + 4]\n\t"
        "fxch st(1)\n\t"
        "fmul dword ptr [ecx]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fadd dword ptr [eax + 0x3c]\n\t"
        "fstp dword ptr [edx + 0xc]\n\t"
        ".att_syntax prefix\n\t"
        :
        : [matrix] "m" (C), [input] "m" (B), [output] "m" (A)
        : "eax", "ecx", "edx", "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)");
#else
#error BrMatrix4ApplyP requires the original x86 floating-point behavior
#endif
}

// FUNCTION: CARMA2_HW 0x00536a30
void C2_HOOK_CDECL BrMatrix4ApplyV(br_vector4* A, const br_vector3* B, const br_matrix4* C) {

    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(1, 0), B->v[2], C(2, 0));
    A->v[1] = BR_MAC3(B->v[0], C(0, 1), B->v[1], C(1, 1), B->v[2], C(2, 1));
    A->v[2] = BR_MAC3(B->v[0], C(0, 2), B->v[1], C(1, 2), B->v[2], C(2, 2));
    A->v[3] = BR_MAC3(B->v[0], C(0, 3), B->v[1], C(1, 3), B->v[2], C(2, 3));
}

// FUNCTION: CARMA2_HW 0x00536ac0
void C2_HOOK_CDECL BrMatrix4TApply(br_vector4* A, const br_vector4* B, const br_matrix4* C) {
    /* Keep original x87 operand order, including quiet/signaling NaN selection. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, C
        mov ecx, B
        mov edx, A
        fld dword ptr [eax + 0xc]
        fld dword ptr [eax + 8]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 4]
        fxch st(2)
        fmulp st(4), st(0)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(2), st(0)
        fmul dword ptr [ecx + 4]
        fld dword ptr [eax]
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx]
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x1c]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [ecx + 0xc]
        fld dword ptr [eax + 0x10]
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 0x2c]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [eax + 0x28]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x24]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [ecx + 4]
        fld dword ptr [eax + 0x20]
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 8]
        fld dword ptr [eax + 0x38]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x34]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x3c]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [ecx + 0xc]
        fld dword ptr [eax + 0x30]
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 0xc]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "movl %[c], %%eax; movl %[b], %%ecx; movl %[a], %%edx;\n\t"
        "flds 0xc(%%eax);\n\t"
        "flds 8(%%eax);\n\t"
        "flds 0xc(%%ecx);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds 4(%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(2);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 4(%%ecx);\n\t"
        "flds (%%eax);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fstps (%%edx);\n\t"
        "flds 0x18(%%eax);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds 0x14(%%eax);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds 0x1c(%%eax);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 0xc(%%ecx);\n\t"
        "flds 0x10(%%eax);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 4(%%edx);\n\t"
        "flds 0x2c(%%eax);\n\t"
        "flds 0xc(%%ecx);\n\t"
        "flds 0x28(%%eax);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds 0x24(%%eax);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 4(%%ecx);\n\t"
        "flds 0x20(%%eax);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 8(%%edx);\n\t"
        "flds 0x38(%%eax);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds 0x34(%%eax);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds 0x3c(%%eax);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 0xc(%%ecx);\n\t"
        "flds 0x30(%%eax);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0xc(%%edx);\n\t"
        : : [a] "r"(A), [b] "r"(B), [c] "r"(C)
        : "eax", "ecx", "edx", "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");
#else
    A->v[0] = BR_MAC4(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2), B->v[3], C(0, 3));
    A->v[1] = BR_MAC4(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2), B->v[3], C(1, 3));
    A->v[2] = BR_MAC4(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2), B->v[3], C(2, 3));
    A->v[3] = BR_MAC4(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2), B->v[3], C(3, 3));
#endif
}

// FUNCTION: CARMA2_HW 0x00536b70
void C2_HOOK_CDECL BrMatrix4TApplyP(br_vector4* A, const br_vector3* B, const br_matrix4* C) {
    /* Keep original x87 operand order, including quiet/signaling NaN selection. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, C
        mov ecx, B
        mov edx, A
        fld dword ptr [eax + 4]
        fld dword ptr [eax + 8]
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
        fadd dword ptr [eax + 0xc]
        fstp dword ptr [edx]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x18]
        fld dword ptr [eax + 0x10]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 8]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x1c]
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 0x24]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x28]
        fld dword ptr [eax + 0x20]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 8]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x2c]
        fstp dword ptr [edx + 8]
        fld dword ptr [eax + 0x34]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x38]
        fld dword ptr [eax + 0x30]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 8]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x3c]
        fstp dword ptr [edx + 0xc]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "movl %[c], %%eax; movl %[b], %%ecx; movl %[a], %%edx;\n\t"
        "flds 4(%%eax);\n\t"
        "flds 8(%%eax);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds (%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(2);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fadds 0xc(%%eax);\n\t"
        "fstps (%%edx);\n\t"
        "flds 0x14(%%eax);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds 0x18(%%eax);\n\t"
        "flds 0x10(%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(3);\n\t"
        "fmuls 8(%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fadds 0x1c(%%eax);\n\t"
        "fstps 4(%%edx);\n\t"
        "flds 0x24(%%eax);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds 0x28(%%eax);\n\t"
        "flds 0x20(%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(3);\n\t"
        "fmuls 8(%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fadds 0x2c(%%eax);\n\t"
        "fstps 8(%%edx);\n\t"
        "flds 0x34(%%eax);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds 0x38(%%eax);\n\t"
        "flds 0x30(%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(3);\n\t"
        "fmuls 8(%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fadds 0x3c(%%eax);\n\t"
        "fstps 0xc(%%edx);\n\t"
        : : [a] "r"(A), [b] "r"(B), [c] "r"(C)
        : "eax", "ecx", "edx", "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");
#else
    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2)) + C(0, 3);
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2)) + C(1, 3);
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2)) + C(2, 3);
    A->v[3] = BR_MAC3(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2)) + C(3, 3);
#endif
}

// FUNCTION: CARMA2_HW 0x00536c10
void C2_HOOK_CDECL BrMatrix4TApplyV(br_vector4* A, const br_vector3* B, const br_matrix4* C) {
    /* Keep original x87 operand order, including quiet/signaling NaN selection. */
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
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x14]
        fld dword ptr [eax + 0x10]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 0x28]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x24]
        fld dword ptr [eax + 0x20]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 8]
        fld dword ptr [eax + 0x38]
        fld dword ptr [ecx + 8]
        fld dword ptr [eax + 0x34]
        fld dword ptr [eax + 0x30]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [ecx + 4]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx + 0xc]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "movl %[c], %%eax; movl %[b], %%ecx; movl %[a], %%edx;\n\t"
        "flds 8(%%eax);\n\t"
        "flds 4(%%eax);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds (%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(2);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fstps (%%edx);\n\t"
        "flds 0x18(%%eax);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds 0x14(%%eax);\n\t"
        "flds 0x10(%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(3);\n\t"
        "fmuls 4(%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 4(%%edx);\n\t"
        "flds 0x28(%%eax);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds 0x24(%%eax);\n\t"
        "flds 0x20(%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(3);\n\t"
        "fmuls 4(%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 8(%%edx);\n\t"
        "flds 0x38(%%eax);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds 0x34(%%eax);\n\t"
        "flds 0x30(%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(3);\n\t"
        "fmuls 4(%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0xc(%%edx);\n\t"
        : : [a] "r"(A), [b] "r"(B), [c] "r"(C)
        : "eax", "ecx", "edx", "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");
#else
    A->v[0] = BR_MAC3(B->v[0], C(0, 0), B->v[1], C(0, 1), B->v[2], C(0, 2));
    A->v[1] = BR_MAC3(B->v[0], C(1, 0), B->v[1], C(1, 1), B->v[2], C(1, 2));
    A->v[2] = BR_MAC3(B->v[0], C(2, 0), B->v[1], C(2, 1), B->v[2], C(2, 2));
    A->v[3] = BR_MAC3(B->v[0], C(3, 0), B->v[1], C(3, 1), B->v[2], C(3, 2));
#endif
}

// FUNCTION: CARMA2_HW 0x00536ca0
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

// FUNCTION: CARMA2_HW 0x00536d10
void C2_HOOK_CDECL BrMatrix4Mul34(br_matrix4* A, const br_matrix34* B, const br_matrix4* C) {
    /* Keep original x87 operand order, including quiet/signaling NaN selection. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, C
        mov ecx, B
        mov edx, A
        fld dword ptr [eax + 0x20]
        fld dword ptr [ecx + 8]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax]
        fxch st(2)
        fmulp st(3), st(0)
        fmul dword ptr [eax + 0x10]
        fxch st(1)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [edx]
        fld dword ptr [eax + 4]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 4]
        fld dword ptr [ecx + 8]
        fxch st(3)
        fmul dword ptr [ecx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [eax + 0x24]
        faddp st(1), st(0)
        fstp dword ptr [edx + 4]
        fld dword ptr [eax + 8]
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x28]
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
        fld dword ptr [eax + 0xc]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [ecx + 4]
        fld dword ptr [eax + 0x2c]
        fxch st(3)
        fmul dword ptr [ecx]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [ecx + 8]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0xc]
        fld dword ptr [eax + 0x10]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [ecx + 0x10]
        fld dword ptr [ecx + 0x14]
        fxch st(2)
        fmul dword ptr [eax]
        fxch st(3)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x20]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x10]
        fld dword ptr [ecx + 0x10]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 0x14]
        fld dword ptr [eax + 0x24]
        fld dword ptr [ecx + 0xc]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 4]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x14]
        fld dword ptr [ecx + 0x14]
        fld dword ptr [eax + 0x28]
        fld dword ptr [ecx + 0x10]
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 0xc]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 8]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x18]
        fld dword ptr [ecx + 0xc]
        fld dword ptr [eax + 0xc]
        fld dword ptr [eax + 0x2c]
        fld dword ptr [ecx + 0x14]
        fld dword ptr [eax + 0x1c]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [ecx + 0x10]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x1c]
        fld dword ptr [ecx + 0x18]
        fld dword ptr [ecx + 0x20]
        fld dword ptr [eax + 0x20]
        fld dword ptr [ecx + 0x1c]
        fxch st(3)
        fmul dword ptr [eax]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [eax + 0x10]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x20]
        fld dword ptr [ecx + 0x18]
        fld dword ptr [eax + 4]
        fld dword ptr [ecx + 0x1c]
        fld dword ptr [eax + 0x14]
        fld dword ptr [ecx + 0x20]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x24]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x24]
        fld dword ptr [ecx + 0x18]
        fld dword ptr [eax + 8]
        fld dword ptr [ecx + 0x1c]
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 0x20]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x28]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x28]
        fld dword ptr [ecx + 0x20]
        fld dword ptr [eax + 0x2c]
        fld dword ptr [ecx + 0x1c]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [ecx + 0x18]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0xc]
        faddp st(1), st(0)
        fstp dword ptr [edx + 0x2c]
        fld dword ptr [ecx + 0x24]
        fld dword ptr [ecx + 0x2c]
        fld dword ptr [eax + 0x20]
        fld dword ptr [ecx + 0x28]
        fxch st(3)
        fmul dword ptr [eax]
        fxch st(1)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fxch st(1)
        fmul dword ptr [eax + 0x10]
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x30]
        fstp dword ptr [edx + 0x30]
        fld dword ptr [ecx + 0x2c]
        fld dword ptr [eax + 0x24]
        fld dword ptr [ecx + 0x24]
        fld dword ptr [eax + 4]
        fld dword ptr [ecx + 0x28]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x14]
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x34]
        fstp dword ptr [edx + 0x34]
        fld dword ptr [ecx + 0x28]
        fld dword ptr [eax + 0x18]
        fld dword ptr [ecx + 0x2c]
        fld dword ptr [eax + 0x28]
        fld dword ptr [ecx + 0x24]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 8]
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x38]
        fstp dword ptr [edx + 0x38]
        fld dword ptr [ecx + 0x24]
        fld dword ptr [eax + 0xc]
        fld dword ptr [ecx + 0x2c]
        fld dword ptr [eax + 0x2c]
        fld dword ptr [ecx + 0x28]
        fxch st(3)
        fmulp st(4), st(0)
        fmulp st(1), st(0)
        faddp st(2), st(0)
        fmul dword ptr [eax + 0x1c]
        faddp st(1), st(0)
        fadd dword ptr [eax + 0x3c]
        fstp dword ptr [edx + 0x3c]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "movl %[c], %%eax; movl %[b], %%ecx; movl %[a], %%edx;\n\t"
        "flds 0x20(%%eax);\n\t"
        "flds 8(%%ecx);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds (%%eax);\n\t"
        "fxch %%st(2);\n\t"
        "fmulp %%st(3);\n\t"
        "fmuls 0x10(%%eax);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "faddp %%st(1);\n\t"
        "fstps (%%edx);\n\t"
        "flds 4(%%eax);\n\t"
        "flds 0x14(%%eax);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds 8(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "fmulp %%st(2);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(1);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls 0x24(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 4(%%edx);\n\t"
        "flds 8(%%eax);\n\t"
        "flds 0x18(%%eax);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds 0x28(%%eax);\n\t"
        "fxch %%st(3);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "fmulp %%st(2);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(1);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls 8(%%ecx);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 8(%%edx);\n\t"
        "flds 0xc(%%eax);\n\t"
        "flds 0x1c(%%eax);\n\t"
        "flds 4(%%ecx);\n\t"
        "flds 0x2c(%%eax);\n\t"
        "fxch %%st(3);\n\t"
        "fmuls (%%ecx);\n\t"
        "fxch %%st(1);\n\t"
        "fmulp %%st(2);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(1);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls 8(%%ecx);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0xc(%%edx);\n\t"
        "flds 0x10(%%eax);\n\t"
        "flds 0xc(%%ecx);\n\t"
        "flds 0x10(%%ecx);\n\t"
        "flds 0x14(%%ecx);\n\t"
        "fxch %%st(2);\n\t"
        "fmuls (%%eax);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 0x20(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0x10(%%edx);\n\t"
        "flds 0x10(%%ecx);\n\t"
        "flds 0x14(%%eax);\n\t"
        "flds 0x14(%%ecx);\n\t"
        "flds 0x24(%%eax);\n\t"
        "flds 0xc(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 4(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0x14(%%edx);\n\t"
        "flds 0x14(%%ecx);\n\t"
        "flds 0x28(%%eax);\n\t"
        "flds 0x10(%%ecx);\n\t"
        "flds 0x18(%%eax);\n\t"
        "flds 0xc(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 8(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0x18(%%edx);\n\t"
        "flds 0xc(%%ecx);\n\t"
        "flds 0xc(%%eax);\n\t"
        "flds 0x2c(%%eax);\n\t"
        "flds 0x14(%%ecx);\n\t"
        "flds 0x1c(%%eax);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 0x10(%%ecx);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0x1c(%%edx);\n\t"
        "flds 0x18(%%ecx);\n\t"
        "flds 0x20(%%ecx);\n\t"
        "flds 0x20(%%eax);\n\t"
        "flds 0x1c(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmuls (%%eax);\n\t"
        "fxch %%st(1);\n\t"
        "fmulp %%st(2);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(1);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls 0x10(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0x20(%%edx);\n\t"
        "flds 0x18(%%ecx);\n\t"
        "flds 4(%%eax);\n\t"
        "flds 0x1c(%%ecx);\n\t"
        "flds 0x14(%%eax);\n\t"
        "flds 0x20(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 0x24(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0x24(%%edx);\n\t"
        "flds 0x18(%%ecx);\n\t"
        "flds 8(%%eax);\n\t"
        "flds 0x1c(%%ecx);\n\t"
        "flds 0x18(%%eax);\n\t"
        "flds 0x20(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 0x28(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0x28(%%edx);\n\t"
        "flds 0x20(%%ecx);\n\t"
        "flds 0x2c(%%eax);\n\t"
        "flds 0x1c(%%ecx);\n\t"
        "flds 0x1c(%%eax);\n\t"
        "flds 0x18(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 0xc(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fstps 0x2c(%%edx);\n\t"
        "flds 0x24(%%ecx);\n\t"
        "flds 0x2c(%%ecx);\n\t"
        "flds 0x20(%%eax);\n\t"
        "flds 0x28(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmuls (%%eax);\n\t"
        "fxch %%st(1);\n\t"
        "fmulp %%st(2);\n\t"
        "fxch %%st(1);\n\t"
        "faddp %%st(1);\n\t"
        "fxch %%st(1);\n\t"
        "fmuls 0x10(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fadds 0x30(%%eax);\n\t"
        "fstps 0x30(%%edx);\n\t"
        "flds 0x2c(%%ecx);\n\t"
        "flds 0x24(%%eax);\n\t"
        "flds 0x24(%%ecx);\n\t"
        "flds 4(%%eax);\n\t"
        "flds 0x28(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 0x14(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fadds 0x34(%%eax);\n\t"
        "fstps 0x34(%%edx);\n\t"
        "flds 0x28(%%ecx);\n\t"
        "flds 0x18(%%eax);\n\t"
        "flds 0x2c(%%ecx);\n\t"
        "flds 0x28(%%eax);\n\t"
        "flds 0x24(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 8(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fadds 0x38(%%eax);\n\t"
        "fstps 0x38(%%edx);\n\t"
        "flds 0x24(%%ecx);\n\t"
        "flds 0xc(%%eax);\n\t"
        "flds 0x2c(%%ecx);\n\t"
        "flds 0x2c(%%eax);\n\t"
        "flds 0x28(%%ecx);\n\t"
        "fxch %%st(3);\n\t"
        "fmulp %%st(4);\n\t"
        "fmulp %%st(1);\n\t"
        "faddp %%st(2);\n\t"
        "fmuls 0x1c(%%eax);\n\t"
        "faddp %%st(1);\n\t"
        "fadds 0x3c(%%eax);\n\t"
        "fstps 0x3c(%%edx);\n\t"
        : : [a] "r"(A), [b] "r"(B), [c] "r"(C)
        : "eax", "ecx", "edx", "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");
#else
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
#endif
}

// FUNCTION: CARMA2_HW 0x00536f30
void C2_HOOK_CDECL BrMatrix4Pre34(br_matrix4* A, const br_matrix34* B) {
    br_matrix4 C;

    memcpy(&C, A, sizeof(*A));
    BrMatrix4Mul34(A, B, &C);
}

// FUNCTION: CARMA2_HW 0x00536f60
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
