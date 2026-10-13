#include "11-world1.h"

#include <math.h>
#include <stdint.h>

// DeleteBastards

// DeleteAnyZeroBastards

// ApplyTransToModels

// FindSpecVolIndex

static int C2_HOOK_FASTCALL PhysicsWallNaN(double value) {
    union { double d; tU32 w[2]; } u;
    u.d=value;
    return (u.w[1]&0x7ff00000u)==0x7ff00000u && ((u.w[1]&0xfffffu)||u.w[0]);
}

static double C2_HOOK_CDECL PhysicsJointSum(double pA, double pB) {
    double result;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld pA
        fld pB
        faddp st(1), st(0)
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("fldl %1; fldl %2; faddp; fstpl %0"
        : "=m" (result) : "m" (pA), "m" (pB) : "st", "st(1)");
#else
    result = pA + pB;
#endif
    return result;
}

static double C2_HOOK_CDECL SpecVolSum(double a, double b) {
    if (PhysicsWallNaN(a) && PhysicsWallNaN(b)) {
        return PhysicsJointSum(a, b);
    }
    return a + b;
}

static double C2_HOOK_CDECL SpecVolRowLength(const br_matrix34* matrix, tU32 axis) {
    const volatile br_scalar* row = (const volatile br_scalar*)(uintptr_t)
        ((tU32)(uintptr_t)matrix + axis * 12u);
    double x = row[0];
    double y = row[1];
    double z = row[2];
    double xy = SpecVolSum(x * x, y * y);
    return sqrt(SpecVolSum(xy, z * z));
}

// FUNCTION: CARMA2_HW 0x005074d0
void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* matrix, br_material* material1, br_material* material2, int axis0, int axis1) {
    volatile br_scalar* first = (volatile br_scalar*)(uintptr_t)
        ((tU32)(uintptr_t)material1 + 0x24u);
    volatile br_scalar* second = (volatile br_scalar*)(uintptr_t)
        ((tU32)(uintptr_t)material1 + 0x30u);

    *first = (br_scalar)(SpecVolRowLength(matrix, (tU32)axis0) * 6.0);
    *second = (br_scalar)(SpecVolRowLength(matrix, (tU32)axis1) * 6.0);
    BrMatrix23Copy((br_matrix23*)(uintptr_t)((tU32)(uintptr_t)material2 + 0x24u),
                   (br_matrix23*)(uintptr_t)((tU32)(uintptr_t)material1 + 0x24u));
}

// SetSpecVolMatSize

// UpdateSpecVol

// PutAdditionalNonCarsOnToAdditionalActors

// PutAdditionalNonCarsOnToNonTrackActor

// SaveAdditionalActors

// DoSaveAdditionalStuff

// STUB: CARMA2_HW 0x00507360
void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void) {
    NOT_IMPLEMENTED();
}

