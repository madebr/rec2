#include "11-world1.h"
#include "28-world3.h"

#include <math.h>
#include <stdint.h>

// GLOBAL: CARMA2_HW 0x006ab7a8
int gDelete_count;

// GLOBAL: CARMA2_HW 0x006aafc8
br_actor* gDelete_list[500];

#if defined(__GNUC__)
typedef tU32 tEditorDeleteWord __attribute__((__may_alias__));
#else
typedef tU32 tEditorDeleteWord;
#endif

static tU32 C2_HOOK_CDECL EditorDeleteRead(const void* base, tU32 offset) {
    return *(const volatile tEditorDeleteWord*)(uintptr_t)((tU32)(uintptr_t)base + offset);
}

static int C2_HOOK_CDECL EditorDeleteZeroOrUnordered(const br_matrix34* matrix, tU32 offset) {
    tU32 bits = EditorDeleteRead(matrix, offset) & 0x7fffffffu;
    return bits == 0 || bits > 0x7f800000u;
}

// FUNCTION: CARMA2_HW 0x00507210
br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* actor, br_matrix34* matrix, void* data) {
    tU32 identifier;
    tS32 count;
    tU32 index = 0;
    tU32 actor_word = (tU32)(uintptr_t)actor;

    (void)data;

    if (actor_word != *(const volatile tEditorDeleteWord*)&gAdditional_actors) {
        identifier = EditorDeleteRead(actor, 0x14);
        if (identifier == 0 || *(const volatile unsigned char*)(uintptr_t)identifier == '&') {
            if (EditorDeleteZeroOrUnordered(matrix, 0x24)
                    && EditorDeleteZeroOrUnordered(matrix, 0x28)
                    && EditorDeleteZeroOrUnordered(matrix, 0x2c)) {
                goto add_actor;
            }
        }
    }
    if (EditorDeleteRead(actor, 0x18) != 0
            || *(const volatile unsigned char*)(uintptr_t)((tU32)(uintptr_t)actor + 0x12u) != 1) {
        return 0;
    }

add_actor:
    count = *(const volatile int*)&gDelete_count;
    while ((tS32)index < count) {
        if (EditorDeleteRead(gDelete_list, index * 4u) == actor_word) {
            return 0;
        }
        index += 1u;
    }
    *(volatile tEditorDeleteWord*)(uintptr_t)((tU32)(uintptr_t)gDelete_list + (tU32)count * 4u) = actor_word;
    *(volatile tEditorDeleteWord*)&gDelete_count = (tU32)count + 1u;
    return 0;
}

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

