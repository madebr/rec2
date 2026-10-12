#include "60-phil.h"
#include <math.h>
#include <stdint.h>
#include "globvars.h"
#include "41-utility.h"

#include "49-physics.h"
#include "55-volume.h"
#include "00-car.h"

#include <string.h>
#include <stddef.h>
#include <stdarg.h>

/* Quaternion APIs used by the packed orientation conversion below. */
extern br_quat* C2_HOOK_CDECL BrQuatNormalise(br_quat* q, const br_quat* qq);
extern br_matrix34* C2_HOOK_CDECL BrQuatToMatrix34(br_matrix34* mat, const br_quat* q);

// GLOBAL: CARMA2_HW 0x006923e0
tPHIL_registry_storage gPHIL_registry;

// GLOBAL: CARMA2_HW 0x0065d004
int gPHIL_disabled = 1;

// GLOBAL: CARMA2_HW 0x0074a5f0
tPhysics_object* gPHIL_list_collision_infos;

// GLOBAL: CARMA2_HW 0x0074a5e8
int gPHIL_munging_objects;

// GLOBAL: CARMA2_HW 0x0074a5f8
int gPHIL_doing_physics;

// GLOBAL: CARMA2_HW 0x0074a5ec
tU32 gPHIL_last_physics_tick;

/* REP MOVSD copies overlapping buffers in forward DWORD order. */
static void C2_HOOK_CDECL PhilCopyWords(void* pOutput, const void* pInput, int pCount) {
    int i;
    volatile tU32 word;

    for (i = 0; i < pCount; i++) {
        memcpy((void*)&word, (const char*)pInput + 4 * i, 4);
        memcpy((char*)pOutput + 4 * i, (const void*)&word, 4);
    }
}

// ScalarToFraction

// FractionToScalar

static double C2_HOOK_CDECL PhilOrientationScalar(double value) {
    union { br_scalar scalar; tU32 word; } bits;
    volatile tU32 stored;

    /* The integer reload forces the scalar rounding even with MSVC5. */
    bits.scalar = (br_scalar)value;
    stored = bits.word;
    bits.word = stored;
    return bits.scalar;
}

// FUNCTION: CARMA2_HW 0x0049be90
void C2_HOOK_FASTCALL GetOrientationFromMatrix(tU16* pOrientation, const br_matrix34* pMatrix) {
    br_quat quaternion;
    double value;
    int i;

    BrMatrix34ToQuat(&quaternion, pMatrix);
    for (i = 0; i < 4; i++) {
        value = (&quaternion.x)[i];
        if (value > 1.0) {
            value = 1.0;
        }
        /* Original C0 test replaces unordered values with the lower bound. */
        if (!(value >= -1.0)) {
            value = -1.0;
        }
        value = ((value - -1.0) * 0.5) * 65534.0 - -0.5;
        pOrientation[i] = (tU16)(br_int_64)value;
    }
}

// FUNCTION: CARMA2_HW 0x0049c000
void C2_HOOK_FASTCALL GetMatrixFromOrientation(br_matrix34* pMatrix, const tU16* pOrientation) {
    br_quat quaternion;
    tU16 parts[4];
    double value;
    int i;

    parts[0] = pOrientation[0];
    parts[1] = pOrientation[1];
    parts[2] = pOrientation[2];
    parts[3] = pOrientation[3];
    for (i = 0; i < 4; i++) {
        /* 0x37800100 is the original scalar; double its product unspilled. */
        value = (double)parts[i] * 1.5259254723787308e-05;
        (&quaternion.x)[i] = (br_scalar)PhilOrientationScalar((value + value) - 1.0);
    }
    BrQuatNormalise(&quaternion, &quaternion);
    BrQuatToMatrix34(pMatrix, &quaternion);
}

// OmegaCompScalarTo16

// OmegaComp16ToScalar

// GetOmega16FromOmega

// GetOmegaFromOmega16

// GetObjectNetworkStuff

// GetVelocityAndPosition

// SizeOfObjectNetworkStuff

// WriteObjectNetworkStuff

// GetHierarchyNetworkStuff

// FindObject

// WriteHierarchyNetworkStuff

// GetHierarchyNetworkSize

// GetNetworkDataSize

// GetSingleMatrixFromNetworkData

// FUNCTION: CARMA2_HW 0x004b5cc0
int C2_HOOK_FASTCALL PHILInit(void) {
    int i;

    for (i = 0; i < 100; i++) {
        gPhil_queued_objects.headers[i].collision_info = NULL;
    }
    gPHIL_count_list_collision_infos = 0;
    gPHIL_count_queued_objects = 0;
    gPHIL_count_queued_objects_for_removal = 0;
    gPHIL_list_collision_infos = NULL;
    gPHIL_munging_objects = 0;
    gPHIL_doing_physics = 0;
    gPHIL_object_added = 0;
    gPHIL_last_physics_tick = 0;
    gPHIL_mechanics_time_sync = 1;
    gPHIL_disabled = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b5d20
void C2_HOOK_FASTCALL PHILDisable(void) {
    gPHIL_disabled = 1;
}

// PHILAddObject

// FUNCTION: CARMA2_HW 0x004b5ea0
int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject) {
    int i;
    int index;
    tPHIL_queued_header* info;
    tPhysics_object* previous;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->collision_info = NULL;
        pObject->field_0x240 = NULL;
        pObject->field_0x239 = 0;
        return 0;
    }
    if (gPHIL_munging_objects && !gPHIL_object_added) {
        index = gPHIL_count_queued_objects_for_removal;
        memcpy((char*)&gPHIL_registry.first_removal + (tU32)index * 4u, &pObject, 4);
        gPHIL_count_queued_objects_for_removal = (int)((tU32)index + 1u);
        return 0;
    }
    for (i = 0; i < 100; i++) {
        if (gPhil_queued_objects.headers[i].collision_info == pObject) {
            break;
        }
    }
    if (i == 100 || pObject->field_0x240 == NULL) {
        return 3;
    }
    info = pObject->field_0x240;
    info->collision_info = NULL;
    pObject->field_0x240 = NULL;
    gPHIL_count_list_collision_infos = (int)((tU32)gPHIL_count_list_collision_infos - 1u);
    if (pObject == gPHIL_list_collision_infos) {
        gPHIL_list_collision_infos = pObject->next;
        if (pObject->next != NULL) {
            pObject->next->prev = NULL;
        }
    }
    for (i = 0; i < 100; i++) {
        previous = gPhil_queued_objects.headers[i].collision_info;
        if (previous != NULL && previous->next == pObject) {
            previous->next = pObject->next;
            if (pObject->next != NULL) {
                pObject->next->prev = gPhil_queued_objects.headers[i].collision_info;
            }
            return 0;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b5fd0
tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void) {
    return gPHIL_disabled == 0 ? gPHIL_list_collision_infos : NULL;
}

// FUNCTION: CARMA2_HW 0x004b5ff0
tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject) {
    return gPHIL_disabled == 0 ? pObject->next : NULL;
}

// FUNCTION: CARMA2_HW 0x004b6010
int C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    info = pObject->field_0x240;
    if (pObject->field_0x239 == 2 || info != NULL) {
        return (int)info->field_0x8;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b6040
int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    info = pObject->field_0x240;
    if (pObject->field_0x239 == 2) {
        info->field_0x8 = 1;
        return 0;
    }
    if (info == NULL) {
        return 3;
    }
    info->field_0x8 = 1;
    pObject->flags |= 0x20;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b6090
int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag) {
    tPHIL_queued_object_info* info;

    if (gPHIL_disabled) {
        return 0;
    }
    info = pObject->field_0x240;
    if (pObject->field_0x239 == 2) {
        info->field_0x8 = 2;
        if (pVelocity != NULL) {
            PhilCopyWords(&info->field_0x18, pVelocity, 3);
            info->flags |= 1;
        }
        if (pOmega != NULL) {
            PhilCopyWords(&info->field_0x24, pOmega, 3);
            info->flags |= 2;
        }
        if (pFlag) {
            info->field_0x4 |= 2;
        } else {
            info->field_0x4 &= ~2u;
        }
        return 0;
    }
    if (info == NULL) {
        return 3;
    }
    info->field_0x8 = 2;
    pObject->flags &= ~0x20;
    if (pFlag) {
        info->field_0x4 |= 2;
    } else {
        info->field_0x4 &= ~2u;
    }
    if (pVelocity != NULL) {
        PhilCopyWords(&pObject->v, pVelocity, 3);
    }
    if (pOmega != NULL) {
        PhilCopyWords(&pObject->omega, pOmega, 3);
    }
    ResetCarSpecialVolume(pObject);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b61a0
int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix) {
    tPHIL_queued_object_info* info;

    if (gPHIL_disabled) {
        return 0;
    }
    info = pObject->field_0x240;
    if (pObject->field_0x239 == 2) {
        PhilCopyWords(&info->field_0x30, pMatrix, 12);
        info->flags |= 4;
        return 0;
    }
    if (info == NULL) {
        return 3;
    }
    if (info->field_0x8 != 1) {
        return 4;
    }
    PhilCopyWords(&info->collision_info->actor->t.t.mat, pMatrix, 12);
    return 0;
}

// PHILAddActiveObject

// PHILAddObjectImmediately

// PHILAddActiveObjectImmediately

// FUNCTION: CARMA2_HW 0x004b63b0
int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...) {
    tPHIL_queued_header* info;
    va_list args;
    int value;

    if (gPHIL_disabled) {
        return 0;
    }
    info = pObject->field_0x240;
    if (info == NULL) {
        return 3;
    }
    if (pProperty < 0 || pProperty >= 9) {
        return 5;
    }
    va_start(args, pProperty);
    switch (pProperty) {
    case 0:
        info->field_0x10 = (br_scalar)va_arg(args, double);
        break;
    case 1:
        if (va_arg(args, int)) {
            *(tU8*)&info->field_0x4 |= 1;
            PositionChildren(pObject);
        } else {
            info->field_0x4 &= 1;
        }
        break;
    case 2:
        info->field_0xc = (br_scalar)va_arg(args, double);
        break;
    case 3:
        value = va_arg(args, int) != 0;
        pObject->flags = (pObject->flags & ~0x40) | (value << 6);
        if (!(pObject->flags & 0x40)) {
            pObject->box_face_ref = (tU16)((tU32)gFace_num__car - 2u);
        }
        break;
    case 4:
        value = va_arg(args, int) != 0;
        pObject->flags = (pObject->flags & ~8) | (value << 3);
        break;
    case 5:
        value = va_arg(args, int) != 0;
        pObject->flags = (pObject->flags & ~0x10) | (value << 4);
        break;
    case 6:
        if (va_arg(args, int)) {
            *(tU8*)&info->field_0x4 |= 4;
        } else {
            info->field_0x4 &= 4;
        }
        break;
    case 7:
        if (va_arg(args, int)) {
            *(tU8*)&info->field_0x4 |= 8;
            pObject->water_d = 10000.f;
        } else {
            info->field_0x4 &= 8;
        }
        break;
    case 8:
        info->field_0x14 = (br_scalar)va_arg(args, double);
        break;
    }
    va_end(args);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b6530
double C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pObject, int pProperty) {
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0.0;
    }
    info = pObject->field_0x240;
    if (info == NULL) {
        return 0.0;
    }
    switch (pProperty) {
    case 0:
        return (double)info->field_0x10;
    case 1:
        return info->field_0x4 & 1;
    case 2:
        return (double)info->field_0xc;
    case 3:
        return ((tU32)pObject->flags >> 6) & 1;
    case 4:
        return ((tU32)pObject->flags >> 3) & 1;
    case 5:
        return ((tU32)pObject->flags >> 4) & 1;
    case 6:
        return (info->field_0x4 >> 2) & 1;
    case 7:
        return (info->field_0x4 >> 3) & 1;
    case 8:
        return (double)info->field_0x14;
    default:
        return 5.0;
    }
}

// FlushQueuedAddsAndRemoves

// ChangedObjectsCallbacks

// ProcessDrag

// MarkObjectAndChildrenAsPassive

// SetStandardGravity


// PHILMungeObjects

// PHILActivatePassive

// PHILInterpolateObjects

// PHILDoPhysics

// PHILApplyPHILObject


static tU32 C2_HOOK_CDECL GravityWord(const void* base, tU32 offset) {
    return *(volatile const tU32*)((tU32)(uintptr_t)base + offset);
}

static double C2_HOOK_CDECL GravityRead(const void* base, tU32 offset) {
    return *(volatile const br_scalar*)((tU32)(uintptr_t)base + offset);
}

static br_scalar C2_HOOK_CDECL GravityRound(double value) {
    volatile br_scalar result = (br_scalar)value;
    return result;
}

static int C2_HOOK_CDECL GravityNaN(double value) {
    tU32 words[2];
    memcpy(words, &value, sizeof(words));
    return (words[1] & 0x7ff00000u) == 0x7ff00000u
        && ((words[1] & 0xfffffu) != 0 || words[0] != 0);
}

static double C2_HOOK_CDECL GravityMultiplyScalar(double left, volatile const br_scalar* right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov ecx, right
        fld qword ptr left
        fmul dword ptr [ecx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fmuls %2; fstpl %0" : "=m"(result) : "m"(left), "m"(*right) : "st");
#else
    result = left * (double)*right;
#endif
    return result;
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

static double C2_HOOK_CDECL GravitySubtractPopWide(double left, double right) {
    if (GravityNaN(left) && GravityNaN(right)) {
        return PhysicsJointSum(left, right);
    }
    return left - right;
}

static volatile const br_scalar* C2_HOOK_CDECL GravityScalarPointer(const void* base, tU32 offset) {
    return (volatile const br_scalar*)((tU32)(uintptr_t)base + offset);
}

// FUNCTION: CARMA2_HW 0x004b6ec0
void C2_HOOK_FASTCALL ProcessGravity(tPHIL_queued_header* header, tPhysics_object* object, br_scalar gravity) {
    const tSpecial_volume* volume = NULL;
    br_vector3 vector;
    double value;
    if ((*(volatile const tU8*)((tU32)(uintptr_t)header + 4) & 8) != 0) {
        volume = (const tSpecial_volume*)(uintptr_t)GravityWord(object, 0x258);
    }
    if (volume != NULL) {
        double magnitude = GravityRead(header, 0x14);
        double viscosity;
        if (!GravityNaN(magnitude) && magnitude != 0.0) {
            viscosity = GravityRead(volume, 0x8c);
            if (!GravityNaN(viscosity) && viscosity > 2.0) {
                double buoyancy = 1.0 - GravityRead(volume, 0x88);
                double standard = (double)gravity;
                buoyancy = GravityMultiplyScalar(buoyancy, GravityScalarPointer(object, 0x254));
                standard = GravityMultiplyScalar(standard, (volatile const br_scalar*)&gGravity_multiplier);
                buoyancy = GravityMultiplyScalar(buoyancy, GravityScalarPointer(header, 0x14));
                standard = standard * 1.4202898550724639;
                value = GravitySubtractPopWide(buoyancy, standard) * 0.04;
            } else {
                value = GravityRead(volume, 0x88);
                value = GravityMultiplyScalar(value, GravityScalarPointer(object, 0x254));
                value = GravityMultiplyScalar(value, &gravity);
                value = GravityMultiplyScalar(value, (volatile const br_scalar*)&gGravity_multiplier);
                value = value * -0.05681159420289855;
            }
        } else {
            value = GravityRead(volume, 0x88);
            value = GravityMultiplyScalar(value, GravityScalarPointer(object, 0x254));
            value = GravityMultiplyScalar(value, &gravity);
            value = GravityMultiplyScalar(value, (volatile const br_scalar*)&gGravity_multiplier);
            value = value * -0.05681159420289855;
        }
    } else {
        value = GravityMultiplyScalar((double)gravity, (volatile const br_scalar*)&gGravity_multiplier);
        value = value * -0.05681159420289855;
    }
    vector.v[0] = 0.0f;
    vector.v[2] = 0.0f;
    vector.v[1] = GravityRound(value);
    FUN_004c29d0(object, &vector);
    FUN_004c2910(object);
}

static tU8* ForceBytes(const void* pBase, unsigned pOffset) {
    return (tU8*)(uintptr_t)pBase + pOffset;
}

static double C2_HOOK_CDECL FrictionRead(const void* base, unsigned offset) {
    return *(volatile const br_scalar*)((const tU8*)base + offset);
}

static tU32 C2_HOOK_CDECL FrictionWord(const void* base, unsigned offset) {
    return *(volatile const tU32*)((const tU8*)base + offset);
}

static void* C2_HOOK_CDECL FrictionPointer(const void* base, unsigned offset) {
    return (void*)(uintptr_t)FrictionWord(base, offset);
}

static br_scalar C2_HOOK_CDECL FrictionScalar(double value) {
    volatile union { br_scalar scalar; tU32 bits; } stored;
    union { br_scalar scalar; tU32 bits; } result;
    stored.scalar = (br_scalar)value;
    result.bits = stored.bits;
    return result.scalar;
}

static volatile const br_scalar* C2_HOOK_CDECL FrictionAt(const void* base, unsigned offset) {
    return (volatile const br_scalar*)((const tU8*)base + offset);
}

static double C2_HOOK_CDECL FrictionProductScalars(volatile const br_scalar* left, volatile const br_scalar* right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, left
        mov edx, right
        fld dword ptr [eax]
        fmul dword ptr [edx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fmuls %2; fstpl %0" : "=m"(result) : "m"(*left), "m"(*right) : "st");
#else
    result = (double)*left * (double)*right;
#endif
    return result;
}

static double C2_HOOK_CDECL PartnerMultiplyWide(double a, double b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 fld qword ptr a
 fmul qword ptr b
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; fmull %2; fstpl %0" : "=m"(result) : "m"(a),"m"(b) : "st");
#else
 result=(double)a * b;
#endif
 return result;
}

static double C2_HOOK_CDECL PartnerMultiplyRawScalar(double a,const br_scalar* b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 fld qword ptr a
 mov ecx,b
 fmul dword ptr [ecx]
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; fmuls %2; fstpl %0" : "=m"(result) : "m"(a),"m"(*b) : "st");
#else
 result=(double)a * *b;
#endif
 return result;
}

static double C2_HOOK_CDECL PartnerMultiplyScalarWide(const br_scalar* a,double b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 mov eax,a
 fld dword ptr [eax]
 fmul qword ptr b
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("flds %1; fmull %2; fstpl %0" : "=m"(result) : "m"(*a),"m"(b) : "st");
#else
 result=(double)*a * b;
#endif
 return result;
}

static const br_scalar* PartnerScalarPtr(const void* base,unsigned offset) {
 return (const br_scalar*)ForceBytes(base,offset);
}

#if defined(_MSC_VER) && _MSC_VER < 1300
static unsigned C2_HOOK_CDECL FrictionStatus(double left, double right) {
    unsigned short status;
    __asm {
        fld qword ptr left
        fcomp qword ptr right
        fnstsw ax
        mov status, ax
    }
    return status;
}
#endif

static int C2_HOOK_CDECL FrictionGreater(double left, double right) {
#if defined(_MSC_VER) && _MSC_VER < 1300
    return (FrictionStatus(left, right) & 0x4100) == 0;
#else
    return left > right;
#endif
}

typedef void (C2_HOOK_FASTCALL* tDragFunction)(tPHIL_queued_header*, tPhysics_object*, br_scalar, int, tSpecial_volume*);

typedef void (C2_HOOK_FASTCALL* tDragWordsFunction)(tPHIL_queued_header*, tPhysics_object*, tU32, int, tSpecial_volume*);

static void C2_HOOK_CDECL DragForward(tDragFunction target, tPHIL_queued_header* header, tPhysics_object* object, const br_scalar* drag, int flags, tSpecial_volume* volume) {
    tDragWordsFunction words_target;
    tU32 bits;
    /* The original recursion MOV/PUSH forwards the unmodified stack DWORD.
     * Same-width ABI view is intentional for the supported32-bit x86 builds. */
    memcpy(&words_target, &target, sizeof(words_target));
    memcpy(&bits, drag, sizeof(bits));
    words_target(header, object, bits, flags, volume);
}

static double C2_HOOK_CDECL DragLength(const void* object, unsigned offset) {
    double x = FrictionRead(object, offset);
    double y = FrictionRead(object, offset + 4);
    double z = FrictionRead(object, offset + 8);
    br_scalar rounded_x = FrictionScalar(x);
    br_scalar rounded_z = FrictionScalar(z);
    double xx = PartnerMultiplyRawScalar(x, &rounded_x);
    double yy = PartnerMultiplyWide(y, y);
    double zz = PartnerMultiplyRawScalar(z, &rounded_z);
    return sqrt(PhysicsJointSum(PhysicsJointSum(xx, yy), zz));
}

// FUNCTION: CARMA2_HW 0x004b6ce0
void C2_HOOK_FASTCALL ProcessDrag2(tPHIL_queued_header* header, tPhysics_object* object, br_scalar drag, int flags, tSpecial_volume* volume) {
    br_scalar effective_drag = FrictionScalar((double)drag * 2e-05); /* original58a728 */
    double length, scale;
    br_vector3 damping;
    tPhysics_object* child;

    if ((flags & 8) != 0 && volume != NULL) {
        double queued_gravity = FrictionRead(header, 0x14);
        double viscosity = FrictionRead(volume, 0x8c);
        if (queued_gravity != 0.0 && queued_gravity == queued_gravity && FrictionGreater(viscosity, 2.0)) {
            scale = FrictionProductScalars(FrictionAt(object, 0x254), FrictionAt(volume, 0x8c));
            scale = PartnerMultiplyRawScalar(scale, &effective_drag);
            effective_drag = FrictionScalar(scale * 0.5);
        } else {
            scale = FrictionProductScalars(FrictionAt(volume, 0x8c), FrictionAt(object, 0x254));
            effective_drag = FrictionScalar(PartnerMultiplyRawScalar(scale, &effective_drag));
        }
    }
    length = DragLength(object, 0x68);
    scale = PartnerMultiplyRawScalar(length, &effective_drag);
    scale = scale * 6.9 / FrictionRead(object, 4); /* original58a738 */
    damping.v[0] = FrictionScalar(PartnerMultiplyScalarWide(PartnerScalarPtr(object, 0x68), scale));
    damping.v[1] = FrictionScalar(PartnerMultiplyScalarWide(PartnerScalarPtr(object, 0x6c), scale));
    damping.v[2] = FrictionScalar(PartnerMultiplyScalarWide(PartnerScalarPtr(object, 0x70), scale));
    DRVector3Diminish((br_vector3*)((tU8*)object + 0x68), &damping);

    length = DragLength(object, 0x74);
    scale = PartnerMultiplyRawScalar(length, &effective_drag);
    damping.v[0] = FrictionScalar(PartnerMultiplyScalarWide(PartnerScalarPtr(object, 0x74), scale));
    damping.v[1] = FrictionScalar(PartnerMultiplyScalarWide(PartnerScalarPtr(object, 0x78), scale));
    damping.v[2] = FrictionScalar(PartnerMultiplyScalarWide(PartnerScalarPtr(object, 0x7c), scale));
    FUN_004c2c90(object, &damping);
    child = (tPhysics_object*)FrictionPointer(object, 0x224);
    while (child != NULL) {
        DragForward(ProcessDrag2, header, child, &drag, flags, volume);
        child = (tPhysics_object*)FrictionPointer(child, 0x220);
    }
}

static void C2_HOOK_CDECL FrictionWrite(void* base, unsigned offset, double value) {
    *(volatile br_scalar*)((tU8*)base + offset) = FrictionScalar(value);
}

static double C2_HOOK_CDECL FrictionAddScalar(double left, volatile const br_scalar* right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov edx, right
        fld qword ptr left
        fadd dword ptr [edx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fadds %2; fstpl %0" : "=m"(result) : "m"(left), "m"(*right) : "st");
#else
    result = left + (double)*right;
#endif
    return result;
}

static double C2_HOOK_CDECL FrictionSubtractScalar(double left, volatile const br_scalar* right) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov edx, right
        fld qword ptr left
        fsub dword ptr [edx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("fldl %1; fsubs %2; fstpl %0" : "=m"(result) : "m"(left), "m"(*right) : "st");
#else
    result = left - (double)*right;
#endif
    return result;
}

static double C2_HOOK_CDECL PartnerSubtractWide(double a, double b) {
 double result;
#if defined(_MSC_VER) && defined(_M_IX86)
 __asm {
 fld qword ptr a
 fsub qword ptr b
 fstp qword ptr result
 }
#elif defined(__GNUC__) && defined(__i386__)
 __asm__ __volatile__("fldl %1; fsubl %2; fstpl %0" : "=m"(result) : "m"(a),"m"(b) : "st");
#else
 result=a-b;
#endif
 return result;
}

// FUNCTION: CARMA2_HW 0x004b6fb0
void C2_HOOK_FASTCALL LevelOutOnSurface(tPhysics_object* object) {
    const br_scalar negative_one = -1.0f; /* original58a758 */
    double first, second;
    unsigned axis_row = 2;
    br_matrix34* actor_matrix;
    br_vector3 axis, correction, local_correction;
    double axis_x, axis_y, axis_z;
    double normal_x, normal_y, normal_z;
    double dot, cross_x, cross_y, cross_z;
    double product_x, product_y, product_z, error, denominator;
    double correction_x, correction_y;
    br_scalar rounded_dot, rounded_y, rounded_z, rounded_error;
    br_scalar rounded_y_product, rounded_z_product, rounded_x_product;

    /* Comparisons and their loads follow the original ordered > tests.
     * Ties and unordered values choose the final Z axis. */
    first = FrictionRead(object, 8);
    second = FrictionRead(object, 12);
    if (FrictionGreater(first, second)) {
        first = FrictionRead(object, 8);
        second = FrictionRead(object, 16);
        if (FrictionGreater(first, second)) axis_row = 0;
    }
    if (axis_row != 0) {
        first = FrictionRead(object, 12);
        second = FrictionRead(object, 16);
        if (FrictionGreater(first, second)) {
            first = FrictionRead(object, 12);
            second = FrictionRead(object, 8);
            if (FrictionGreater(first, second)) axis_row = 1;
        }
    }
    actor_matrix = (br_matrix34*)((tU8*)FrictionPointer(object, 0) + 0x2c);
    axis_x = FrictionRead(actor_matrix, axis_row * 12);
    axis_y = FrictionRead(actor_matrix, axis_row * 12 + 4);
    axis_z = FrictionRead(actor_matrix, axis_row * 12 + 8);
    axis.v[0] = FrictionScalar(PartnerMultiplyRawScalar(axis_x, &negative_one));
    axis.v[1] = FrictionScalar(PartnerMultiplyRawScalar(axis_y, &negative_one));
    axis.v[2] = FrictionScalar(PartnerMultiplyRawScalar(axis_z, &negative_one));

    normal_z = FrictionRead(object, 0x24c);
    normal_x = FrictionRead(object, 0x244);
    normal_y = FrictionRead(object, 0x248);
    product_z = PartnerMultiplyRawScalar(normal_z, &axis.v[2]);
    product_x = PartnerMultiplyRawScalar(normal_x, &axis.v[0]);
    product_y = PartnerMultiplyRawScalar(normal_y, &axis.v[1]);
    dot = PhysicsJointSum(PhysicsJointSum(product_z, product_x), product_y);
    rounded_dot = FrictionScalar(dot);
    if (!FrictionGreater(dot, -0.99)) return; /* original58a760 */

    /* Six original normal loads precede the cross products. */
    normal_x = FrictionRead(object, 0x244);
    normal_y = FrictionRead(object, 0x248);
    first = FrictionRead(object, 0x248);
    normal_z = FrictionRead(object, 0x24c);
    second = FrictionRead(object, 0x24c);
    axis_x = FrictionRead(object, 0x244);
    cross_x = PartnerSubtractWide(PartnerMultiplyRawScalar(first, &axis.v[2]), PartnerMultiplyRawScalar(normal_z, &axis.v[1]));
    product_x = PartnerMultiplyWide(cross_x, cross_x);
    cross_y = PartnerSubtractWide(PartnerMultiplyRawScalar(second, &axis.v[0]), PartnerMultiplyRawScalar(axis_x, &axis.v[2]));
    rounded_y = FrictionScalar(cross_y);
    product_y = PartnerMultiplyRawScalar(cross_y, &rounded_y);
    cross_z = PartnerSubtractWide(PartnerMultiplyRawScalar(normal_x, &axis.v[1]), PartnerMultiplyRawScalar(normal_y, &axis.v[0]));
    rounded_z = FrictionScalar(cross_z);
    error = FrictionSubtractScalar((double)rounded_dot, &negative_one);
    rounded_error = FrictionScalar(error);
    rounded_y_product = FrictionScalar(product_y);
    product_z = PartnerMultiplyRawScalar(cross_z, &rounded_z);
    rounded_z_product = FrictionScalar(product_z);

    /* The original successively recomputes each denominator after changing
     * the preceding component. These three unequal normalizations are real. */
    denominator = sqrt(FrictionAddScalar(FrictionAddScalar(product_x, &rounded_z_product), &rounded_y_product));
    correction_x = PartnerMultiplyWide(error / denominator, cross_x);
    correction.v[0] = FrictionScalar(correction_x);
    product_x = PartnerMultiplyRawScalar(correction_x, &correction.v[0]);
    rounded_x_product = FrictionScalar(product_x);
    denominator = sqrt(FrictionAddScalar((double)rounded_z_product + (double)rounded_y_product, &rounded_x_product));
    correction_y = PartnerMultiplyScalarWide(&rounded_y, (double)rounded_error / denominator);
    correction.v[1] = FrictionScalar(correction_y);
    product_y = PartnerMultiplyRawScalar(correction_y, &correction.v[1]);
    denominator = sqrt(FrictionAddScalar(FrictionAddScalar(product_y, &rounded_z_product), &rounded_x_product));
    correction.v[2] = FrictionScalar(PartnerMultiplyRawScalar((double)rounded_error / denominator, &rounded_z));

    BrMatrix34TApplyV(&local_correction, &correction, actor_matrix);
    FrictionWrite(object, 0x74, FrictionAddScalar(FrictionRead(&local_correction, 0), FrictionAt(object, 0x74)));
    FrictionWrite(object, 0x78, FrictionAddScalar(FrictionRead(&local_correction, 4), FrictionAt(object, 0x78)));
    FrictionWrite(object, 0x7c, FrictionAddScalar(FrictionRead(&local_correction, 8), FrictionAt(object, 0x7c)));
}
