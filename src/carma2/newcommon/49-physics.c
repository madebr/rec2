#include "49-physics.h"

#include "00-car.h"
#include "16-graphics1.h"
#include "28-world3.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "52-errors.h"
#include "globvars.h"
#include "c2_string.h"
#include "c2_stdlib.h"

#include <stddef.h>

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000

static int C2_HOOK_FASTCALL PhysicsScalarNaN(const br_scalar* value);
static int C2_HOOK_FASTCALL PhysicsLessOrUnordered(const br_scalar* a, const br_scalar* b);
static int C2_HOOK_FASTCALL PhysicsGreaterOrdered(const br_scalar* a, const br_scalar* b);

// GLOBAL: CARMA2_HW 0x0074a5e4
int gPHIL_mechanics_time_sync;

// GLOBAL: CARMA2_HW 0x0065d010
int gFace_num__car = 1;

// GLOBAL: CARMA2_HW 0x00692dd0
br_scalar gPhysics_bounds_max_x;

// GLOBAL: CARMA2_HW 0x00692dd4
br_scalar gPhysics_bounds_min_x;

// GLOBAL: CARMA2_HW 0x006940c0
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x00744818
void* gPhysics_buffer1_ptr;

// GLOBAL: CARMA2_HW 0x00744814
void* gPhysics_buffer2_ptr;

// GLOBAL: CARMA2_HW 0x0074a5e0
void* gPhysics_buffer3_ptr;

// GLOBAL: CARMA2_HW 0x006940c8
int gPhysics_other_buffer_capacity;

// GLOBAL: CARMA2_HW 0x006940c4
void* gPhysics_other_buffer;

// count

// DrMatrix34ApplyLPInverse

// AddObjectWallForce

// AddObjectObjectForce

// RayCastThruFaceList

// RayCastThruFaceList2

// RayCastInBox

// FaceOffCount2

// FaceOffCount

// FaceOnCount

// EliminateFaces

// GetShapePoint

// GetObjectObjectEdgeIntersections

// GetFaceEdgeIntersection

// GetSphereToFacePoints

// CheckSingleEdgeWithSphere

// GetSphereToEdgePoints

// GetCornerToFacePoints

// CylinderCheck

// CheckTwoEdges

// WorldPointCollision

// CheckSingleEdgeWithObjectEdges

// GetEdgeToEdgePoints

// SetUpWorkspace

// GetObjectWallForceInfo

// GetObjectCornerToObjectFacePoints

// BoundsOverlapTest

// GetObjectObjectEdgePoints

// GetSphereNonSphereForceInfo

// GetSphereSphereForceInfo

// GetSphereShapeForceInfo

// GetNonSphereNonSphereForceInfo

// GetNonSphereShapeForceInfo

// GetObjectObjectForceInfo

// AddOldForces

// TestShapeEdgeIntersection

// TestObjectOverlap

// TestObjectOverlapIgnoreCloseRelatives

// TestObjectOverlapRecurse

// TestForObjectInSensiblePlace

/* Original x87 operand order preserves the tested distinct NaN payloads. */
static br_scalar C2_HOOK_FASTCALL PhysicsObjectEnergy(tPhysics_object* object) {
    br_scalar energy;
#if defined(_MSC_VER) && defined(_M_IX86)
    br_scalar omega;
    __asm {
        mov esi, object
        lea eax, [esi + 8]
        mov ecx, 3
        fldz
    energy_component:
        fld dword ptr [eax + 60h]
        fld dword ptr [eax + 6ch]
        fst omega
        fld st(1)
        fmul st(0), st(2)
        fld dword ptr [esi + 4]
        fxch st(2)
        fmul omega
        fld dword ptr [eax]
        fxch st(3)
        fmulp st(2), st(0)
        fxch st(2)
        fmulp st(2), st(0)
        fxch st(1)
        faddp st(1), st(0)
        add eax, 4
        dec ecx
        faddp st(2), st(0)
        fstp st(0)
        jnz energy_component
        fstp energy
    }
#elif defined(__GNUC__) && defined(__i386__)
    br_scalar omega;
    br_scalar* component = object->I.v;
    int count = 3;
    __asm__ volatile(
        "fldz; 1: flds 0x60(%[p]); flds 0x6c(%[p]); fsts %[omega]; "
        "fld %%st(1); fmul %%st(2),%%st; flds 4(%[object]); fxch %%st(2); "
        "fmuls %[omega]; flds (%[p]); fxch %%st(3); fmulp %%st,%%st(2); "
        "fxch %%st(2); fmulp %%st,%%st(2); fxch %%st(1); faddp %%st,%%st(1); "
        "add $4,%[p]; dec %[n]; faddp %%st,%%st(2); fstp %%st(0); jnz 1b; fstps %[energy]"
        : [energy] "=m" (energy), [omega] "=m" (omega), [p] "+&a" (component), [n] "+&c" (count)
        : [object] "r" (object)
        : "memory", "cc", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)");
#else
    double total = 0.;
    int i;
    for (i = 0; i < 3; i++) {
        double velocity_squared = (double)object->v.v[i] * object->v.v[i];
        double omega_squared = (double)object->omega.v[i] * object->omega.v[i];
        total += (double)object->M * velocity_squared + (double)object->I.v[i] * omega_squared;
    }
    energy = (br_scalar)total;
#endif
    return energy;
}

static br_scalar C2_HOOK_FASTCALL PhysicsAccumulateEnergy(br_scalar total, br_scalar energy, br_scalar children) {
    br_scalar result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld children
        fld energy
        fadd total
        faddp st(1), st(0)
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; flds %2; fadds %3; faddp %%st,%%st(1); fstps %0"
        : "=m" (result) : "m" (children), "m" (energy), "m" (total) : "st", "st(1)");
#else
    volatile double subtotal = (double)energy + total;

    result = (br_scalar)(subtotal + children);
#endif
    return result;
}

// FUNCTION: CARMA2_HW 0x004b7290
br_scalar C2_HOOK_FASTCALL HierarchyCalcEnergy(tPhysics_object* pObject) {
    br_scalar total = 0.f;

    while (pObject != NULL) {
        br_scalar energy = PhysicsObjectEnergy(pObject);
        br_scalar children = HierarchyCalcEnergy(pObject->child);
        total = PhysicsAccumulateEnergy(total, energy, children);
        pObject = pObject->next;
    }
    return total;
}

// CheckCollisionList

// MakeJointVelocitiesVector

// ApplySingleForce

// ApplyJointForces

// SkipForces

// ScaleUpObjectMI

// CheckForTwang

// CreateJointForce

// CreateJointTorque

// AllocateObjectWorkspace

// RedoQuickHingeVelocities

// GetJointForces

// GetUniversalRotationAngle

// GetObjectsParent

// GetJointLimits

// GetListJointLimits

// GetGroupJointLimits

// LimitOmega

// CapOmega

// ListLimitOmega

// GroupLimitOmega

// PositionWorldJointedObject

// TranslateAndRotatePartialObject

// TranslateAndRotateJointedObject

// CountJointForces

// MoveJointedObject

// TellPhysicsJointsHaveChanged

// CombineBounds

// GetWorldBoundingBoxForObjects

// FUNCTION: CARMA2_HW 0x004b9ce0
void C2_HOOK_FASTCALL GetWorldBoundingBoxForObject(tPhysics_object* pObject, br_bounds* pBounds) {
    br_bounds old_bounds;
    tPhysics_object* child;
    int i;

    GetNewBoundingBox(&pObject->field_0xf4, &pObject->bb1, &pObject->transform_matrix);
    GetNewBoundingBox(&old_bounds, &pObject->bb1, &pObject->actor->t.t.mat);
    for (i = 0; i < 3; i++) {
        if (PhysicsGreaterOrdered(&pObject->field_0xf4.min.v[i], &old_bounds.min.v[i])) {
            memcpy(&pObject->field_0xf4.min.v[i], &old_bounds.min.v[i], sizeof(br_scalar));
        }
        if (PhysicsLessOrUnordered(&pObject->field_0xf4.max.v[i], &old_bounds.max.v[i])) {
            memcpy(&pObject->field_0xf4.max.v[i], &old_bounds.max.v[i], sizeof(br_scalar));
        }
        pObject->field_0xf4.min.v[i] = (br_scalar)((double)pObject->field_0xf4.min.v[i] - .0025);
        pObject->field_0xf4.max.v[i] = (br_scalar)((double)pObject->field_0xf4.max.v[i] - -.0025);
    }
    memcpy(&pObject->field_0x10c, &pObject->field_0xf4, sizeof(br_bounds));
    for (child = pObject->child; child != NULL; child = child->next) {
        GetWorldBoundingBoxForObject(child, &pObject->field_0x10c);
    }
    if (pBounds != NULL) {
        for (i = 0; i < 3; i++) {
            if (PhysicsLessOrUnordered(&pObject->field_0x10c.min.v[i], &pBounds->min.v[i])) {
                memcpy(&pBounds->min.v[i], &pObject->field_0x10c.min.v[i], sizeof(br_scalar));
            }
            if (PhysicsGreaterOrdered(&pObject->field_0x10c.max.v[i], &pBounds->max.v[i])) {
                memcpy(&pBounds->max.v[i], &pObject->field_0x10c.max.v[i], sizeof(br_scalar));
            }
        }
    } else {
        double extent = (double)pObject->field_0x10c.max.v[0] - pObject->transform_matrix.m[3][0];
        if (!(gPhysics_bounds_max_x > extent)) {
            gPhysics_bounds_max_x = (br_scalar)extent;
        }
        extent = (double)pObject->field_0x10c.min.v[0] - pObject->transform_matrix.m[3][0];
        if (gPhysics_bounds_min_x >= extent) {
            gPhysics_bounds_min_x = (br_scalar)extent;
        }
    }
}

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

static int C2_HOOK_FASTCALL PhysicsScalarNaN(const br_scalar* value) {
    tU32 bits;

    memcpy(&bits, value, sizeof(bits));
    return (bits & 0x7fffffffu) > 0x7f800000u;
}

static int C2_HOOK_FASTCALL PhysicsLessOrUnordered(const br_scalar* a, const br_scalar* b) {
    return *a < *b || PhysicsScalarNaN(a) || PhysicsScalarNaN(b);
}

static int C2_HOOK_FASTCALL PhysicsGreaterOrdered(const br_scalar* a, const br_scalar* b) {
    return *a > *b && !PhysicsScalarNaN(a) && !PhysicsScalarNaN(b);
}

static br_scalar C2_HOOK_FASTCALL PhysicsScalar(double value) {
    volatile br_scalar rounded = (br_scalar)value;

    return rounded;
}

static void C2_HOOK_FASTCALL PhysicsScalarStore(br_scalar* output, const br_scalar* input) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, output
        mov ecx, input
        fld dword ptr [ecx]
        fstp dword ptr [eax]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; fstps %0" : "=m" (*output) : "m" (*input) : "st");
#else
    volatile double value = *input;

    *output = (br_scalar)value;
#endif
}

// FUNCTION: CARMA2_HW 0x004c2060
void C2_HOOK_FASTCALL PrepareForCollisions(tPhysics_object* pObject, tWorld_callbacks* pWorld_callbacks) {
    tBounds bounds;
    br_bounds transformed;
    br_matrix34 inverse;
    br_matrix34 delta;
    br_matrix34 squared;
    br_matrix34 cubed;
    br_matrix34* actor_matrix;
    br_vector3 movement;
    br_scalar zero = 0.f;
    double high;
    int i;
    int valid_cache;
    int found;

    if (pObject->flags & 0x40) {
        pObject->box_face_start = pObject->box_face_end;
        return;
    }
    if ((pObject->flags & 2) && pObject->parent != NULL) {
        pObject->box_face_start = pObject->parent->box_face_start;
        pObject->box_face_end = pObject->parent->box_face_end;
        pObject->box_face_ref = pObject->parent->box_face_ref;
        return;
    }
    valid_cache = (tU32)pObject->box_face_ref == (tU32)gFace_num__car
        || ((tU32)pObject->box_face_ref == (tU32)gFace_num__car - 1u && (int)pObject->box_face_start > gFace_count);
    if (pObject->flags & 1) {
        if (valid_cache
            && PhysicsGreaterOrdered(&pObject->field_0x10c.min.v[0], &pObject->field_0x124.min.v[0])
            && PhysicsGreaterOrdered(&pObject->field_0x10c.min.v[1], &pObject->field_0x124.min.v[1])
            && PhysicsGreaterOrdered(&pObject->field_0x10c.min.v[2], &pObject->field_0x124.min.v[2])
            && PhysicsLessOrUnordered(&pObject->field_0x10c.max.v[0], &pObject->field_0x124.max.v[0])
            && PhysicsLessOrUnordered(&pObject->field_0x10c.max.v[1], &pObject->field_0x124.max.v[1])
            && PhysicsLessOrUnordered(&pObject->field_0x10c.max.v[2], &pObject->field_0x124.max.v[2])) {
            return;
        }
        for (i = 0; i < 3; i++) {
            movement.v[i] = PhysicsScalar((double)pObject->v.v[i] * 0.12f);
        }
        for (i = 0; i < 3; i++) {
            high = (double)pObject->field_0x10c.max.v[i] - -0.15;
            bounds.original_bounds.min.v[i] = PhysicsScalar((double)pObject->field_0x10c.min.v[i] - 0.15);
            bounds.original_bounds.max.v[i] = PhysicsScalar(high);
            if (PhysicsLessOrUnordered(&movement.v[i], &zero)) {
                bounds.original_bounds.min.v[i] = PhysicsScalar((double)bounds.original_bounds.min.v[i] + movement.v[i]);
            } else {
                bounds.original_bounds.max.v[i] = PhysicsScalar(high + movement.v[i]);
            }
            pObject->field_0x124.min.v[i] = PhysicsScalar((double)bounds.original_bounds.min.v[i] - -0.005);
            pObject->field_0x124.max.v[i] = PhysicsScalar((double)bounds.original_bounds.max.v[i] - 0.005);
        }
        BrMatrix34Identity(&inverse);
        bounds.mat = &inverse;
    } else {
        actor_matrix = &pObject->actor->t.t.mat;
        if (valid_cache) {
            BrMatrix34Mul(&squared, actor_matrix, &pObject->field_0x144);
            GetNewBoundingBox(&transformed, &pObject->bb2, &squared);
            if (PhysicsLessOrUnordered(&transformed.max.v[0], &pObject->field_0x124.max.v[0])
                && PhysicsLessOrUnordered(&transformed.max.v[1], &pObject->field_0x124.max.v[1])
                && PhysicsLessOrUnordered(&transformed.max.v[2], &pObject->field_0x124.max.v[2])
                && PhysicsGreaterOrdered(&transformed.min.v[0], &pObject->field_0x124.min.v[0])
                && PhysicsGreaterOrdered(&transformed.min.v[1], &pObject->field_0x124.min.v[1])
                && PhysicsGreaterOrdered(&transformed.min.v[2], &pObject->field_0x124.min.v[2])) {
                return;
            }
        }
        BrMatrix34LPInverse(&inverse, actor_matrix);
        BrMatrix34Mul(&delta, &pObject->transform_matrix, &inverse);
        GetNewBoundingBox(&bounds.original_bounds, &pObject->bb2, &delta);
        for (i = 0; i < 3; i++) {
            PhysicsScalarStore(&bounds.original_bounds.min.v[i], PhysicsLessOrUnordered(&bounds.original_bounds.min.v[i], &pObject->bb2.min.v[i]) ? &bounds.original_bounds.min.v[i] : &pObject->bb2.min.v[i]);
            PhysicsScalarStore(&bounds.original_bounds.max.v[i], PhysicsGreaterOrdered(&bounds.original_bounds.max.v[i], &pObject->bb2.max.v[i]) ? &bounds.original_bounds.max.v[i] : &pObject->bb2.max.v[i]);
            bounds.original_bounds.min.v[i] = PhysicsScalar((double)bounds.original_bounds.min.v[i] - 0.0025f);
            bounds.original_bounds.max.v[i] = PhysicsScalar((double)bounds.original_bounds.max.v[i] - -0.0025f);
        }
        BrMatrix34Mul(&squared, &delta, &delta);
        BrMatrix34Mul(&cubed, &squared, &delta);
        BrMatrix34LPInverse(&squared, &cubed);
        GetNewBoundingBox(&transformed, &pObject->bb2, &squared);
        for (i = 0; i < 3; i++) {
            PhysicsScalarStore(&bounds.original_bounds.min.v[i], PhysicsLessOrUnordered(&bounds.original_bounds.min.v[i], &transformed.min.v[i]) ? &bounds.original_bounds.min.v[i] : &transformed.min.v[i]);
            PhysicsScalarStore(&bounds.original_bounds.max.v[i], PhysicsGreaterOrdered(&bounds.original_bounds.max.v[i], &transformed.max.v[i]) ? &bounds.original_bounds.max.v[i] : &transformed.max.v[i]);
            bounds.original_bounds.min.v[i] = PhysicsScalar((double)bounds.original_bounds.min.v[i] - 0.02f);
            bounds.original_bounds.max.v[i] = PhysicsScalar((double)bounds.original_bounds.max.v[i] - -0.02f);
        }
        memcpy(&pObject->field_0x124, &bounds.original_bounds, sizeof(pObject->field_0x124));
        BrMatrix34Copy(&pObject->field_0x144, &inverse);
        bounds.mat = actor_matrix;
    }
    pObject->box_face_start = (tU16)gFace_count;
    gPling_face = NULL;
    gActorBoxPick_StopGroovidelics = ((tU8)~(tU8)pObject->flags) >> 7;
    if (pWorld_callbacks != NULL && pWorld_callbacks->find_faces_in_box != NULL) {
        found = pWorld_callbacks->find_faces_in_box(&bounds, &gFace_list__car[gFace_count], (int)(300u - (tU32)gFace_count), pWorld_callbacks);
        gFace_count = (int)((tU32)gFace_count + (tU32)found);
        if (gFace_count >= 300) {
            pObject->box_face_start = 0;
            gFace_count = pWorld_callbacks->find_faces_in_box(&bounds, gFace_list__car, 300, pWorld_callbacks);
            gFace_num__car = (int)((tU32)gFace_num__car + 1u);
        }
    }
    pObject->box_face_end = (tU16)gFace_count;
    pObject->box_face_ref = (tU16)gFace_num__car;
    gActorBoxPick_StopGroovidelics = 1;
    if (pWorld_callbacks != NULL && pWorld_callbacks->new_face_list != NULL) {
        pWorld_callbacks->new_face_list(pObject, (undefined4*)gPling_face);
    }
    if (pObject->flags & 0x80) {
        pObject->box_face_end = pObject->box_face_start;
    }
}

// CheckPointVelocities

// WriteNewPositions

// FUNCTION: CARMA2_HW 0x004b9eb0
void C2_HOOK_FASTCALL SetListDoingNothingFlag(tPhysics_object* pObject, tU8 pFlag) {
    while (pObject != NULL) {
        pObject->disable_move_rotate = pFlag;
        pObject->field_0x1df = 0;
        if (pObject->child != NULL) {
            SetListDoingNothingFlag(pObject->child, pFlag);
        }
        pObject = pObject->next;
    }
}

// FUNCTION: CARMA2_HW 0x004b9e40
void C2_HOOK_FASTCALL SetGroupDoingNothingFlag(tPhysics_object* pObject, tU8 pFlag) {
    tPhysics_object* item = pObject;

    pObject->disable_move_rotate = pFlag;
    do {
        SetHierarchyDoingNothingFlag(item, pFlag);
        memcpy(&item, &item->field183_0x1d8, sizeof(item));
    } while (item != NULL && item != pObject);
}

// FUNCTION: CARMA2_HW 0x004b9ef0
void C2_HOOK_FASTCALL SetHierarchyDoingNothingFlag(tPhysics_object* pObject, tU8 pFlag) {
    tPhysics_object* child;

    pObject->disable_move_rotate = pFlag;
    pObject->field_0x1df = 0;
    for (child = pObject->child; child != NULL; child = child->next) {
        child->disable_move_rotate = pFlag;
        child->field_0x1df = 0;
        if (child->child != NULL) {
            SetListDoingNothingFlag(child->child, pFlag);
        }
    }
}

// CheckDoingNothingFlag

// SetGroup

// GroupTogether

// GetPreCalculatedJointForces

// FUNCTION: CARMA2_HW 0x004b9f40
void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject) {
    while (pObject != NULL) {
        if (pObject->disable_move_rotate) {
            pObject->disable_move_rotate = 0;
            pObject->box_face_ref = (tU16)((tU32)gFace_num__car - 2u);
        }
        if (pObject->child != NULL) {
            MakeObjectListDoSomething(pObject->child);
        }
        pObject = pObject->next;
    }
}

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

// CheckForObjectHierachyTouchingObjectList

// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

// SetObjectTrialPositionType

// FillInTrialWorkspace

// FlagsAreGo

// CollideObjectWithReducedObjectList

// AddObjectToReducedList

// ReducedFindObjectCollisions

// FindObjectCollisions

// DoRigidJoints

// DoInterGroupCollisions

// ReduceEffectOfForces

// DoCollisions

// AddDoubleTorqueToMatrix

// DRMatrix33Inverse

// SetUpQuickHingeData

// InitQuickHinge

// CalcQuickHingeData

// SetUpForce

// SetUpTorqueHingePartners

// SetUpHingePartners

// MakeMatrix

// RestoreListOmega

// RestoreGroupOmegas

// MakeVelocitiesVector2

// MakeVelocitiesVector

// NewAddFrictionObjectObject

// NewAddFriction

// ApplyForces

// DoJointFriction

// ListDoJointFriction

// PostAddFriction

// RotateObjectSecondOrder

// RotateObjectFirstOrder

// RotateObject

// TranslateObject

// GetFacesNearObject

// ResetObjectAndChildren

// ResetObjectList

// RepositionObjectGroup

// SimpleRotate

// InterpolateSingleObject

// InterpolateObjects

// ApplyGravitySingleObject

// ApplyGravity

// SetGravityDeltaV

// TrialPositionChildren

// InternalPositionChildren

// PositionChildren

// ApplyTorque

// ApplyDiminishingTorque

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

// RQExpand

// RQSingleExpand

// RQDcmpV

// MakeIdentity

// SparseRQDcmpV

// MakeMatrixFromIndices

// HasBeenDoneBefore

// RemoveRQSingularities

// AddToMatrix

// RQRemovedOneTooManyForces

// GetInverseM

// RQMatrixSolveNoNegatives

// RQMatrixSolveWithNegatives

// MatrixProcess

// RqBackSub

// GetPhysicsScratchPointer

// IncreasePhysicsScratchPointer

// ClearPhysicsScratchSpace

// FUNCTION: CARMA2_HW 0x004c5d10
tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType) {
    tCollision_shape_sphere* result;

    result = BrMemAllocate(sizeof(tCollision_shape_sphere), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Sphere;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5d70
tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType) {
    tCollision_shape_polyhedron* result;
    tU8* raw_memory;
    int count_edges;

    count_edges = (pCount_points - 2) * 3;
    result = BrMemAllocate(sizeof(tCollision_shape_polyhedron) + pCount_points * sizeof(br_vector3)
        + count_edges * sizeof(tPolyhedron_edge_indexes) + (pCount_points - 2) * 2 * sizeof(br_vector4), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_polyhedron);
    result->polyhedron.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->polyhedron.edges = (tPolyhedron_edge_indexes*)raw_memory;
    raw_memory += count_edges * sizeof(tPolyhedron_edge_indexes);
    result->polyhedron.planes = (br_vector4*)raw_memory;
    result->polyhedron.count_points = 0;
    result->polyhedron.count_edges = 0;
    result->polyhedron.count_planes = 0;
    result->common.type = kCollisionShapeType_Polyhedron;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5dc0
tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType) {
    tCollision_shape_wireframe* result;
    tU8* raw_memory;

    result = BrMemAllocate(sizeof(tCollision_shape_wireframe) + pCount_points * sizeof(br_vector3)
        + pCount_lines * sizeof(tPolyhedron_edge_indexes), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_wireframe);
    result->wireframe.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->wireframe.count_lines = pCount_lines;
    result->wireframe.lines = (tPolyhedron_edge_indexes*)raw_memory;
    result->wireframe.count_points = pCount_points;
    result->common.type = kCollisionShapeType_Wireframe;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5e00
tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType) {
    tCollision_shape_box* result;

    result = BrMemAllocate(sizeof(tCollision_shape_box), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Box;
    return result;
}

// GetJointSize

// FUNCTION: CARMA2_HW 0x004c5e20
tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType) {
    return BrMemAllocate(offsetof(tPhysics_joint, limits) + pCount_limits * sizeof(tPhysics_joint_limit), (br_uint_8)pType);
}

// FUNCTION: CARMA2_HW 0x004c5e40
tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType) {
    size_t size;
    tPhysics_joint* clone;

    size = offsetof(tPhysics_joint, limits) + pJoint->count_limits * sizeof(tPhysics_joint_limit);
    clone = BrMemAllocate(size, (br_uint_8)pType);
    memcpy(clone, pJoint, size);
    return clone;
}

// FreeShape

// FUNCTION: CARMA2_HW 0x004c5e80
void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape) {

    if (pShape->common.next != NULL) {
        FreeShapeList(pShape->common.next);
    }
    BrMemFree(pShape);
}

// FUNCTION: CARMA2_HW 0x004c5ea0
void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint) {

    BrMemFree(pJoint);
}

// GetBoundsFromPointList

// FillInShape

// UpdateCollisionObject

// FUNCTION: CARMA2_HW 0x004c61c0
void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage) {
    if (gPhysics_error_cb != NULL) {
        gPhysics_error_cb(pError, pMessage);
    }
    exit(pError);
}

// PhysicsWarning

// FUNCTION: CARMA2_HW 0x004c61f0
void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn) {
    gPhysics_error_cb = pError_cbfn;
}

// RepositionAndOrientateChildren

// PhysicsAddObject

// PhysicsAddObjectList

// PhysicsRemoveObject

// FUNCTION: CARMA2_HW 0x004c6470
int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    int result;
    tPhysics_object* child;

    result = pCallback(pObject, pContext);
    if (result != 0) {
        return result;
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c64b0
int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    tPhysics_object* child;

    for (child = pObject->child; child != NULL; child = child->next) {
        int result;

        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c6510
void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize) {
    gPhysics_buffer1_ptr = pBuffer;
    pSize -= 3 * PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer2_ptr = pBuffer + PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer3_ptr = pBuffer + 2 * PHYSICS_BUFFER_PART_SIZE;
    pBuffer += 3 * PHYSICS_BUFFER_PART_SIZE;
    if (pSize < PHYSICS_BUFFER_OTHER_SIZE) {
        PhysicsError(ePhysicsError_InsufficientSizedBuffer, "Physics workspace too small");
    }
    gPhysics_other_buffer_capacity = pSize;
    gPhysics_other_buffer = pBuffer;
}

// FUNCTION: CARMA2_HW 0x004b57d0
int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks) {
    tU8 min_x, min_z, max_x, max_z;
    tU32 dimensions;
    int x, z;
    int emitted = 0;

    FillInBounds(bnds);
    XZToColumnXZ(&min_x, &min_z, bnds->real_bounds.min.v[0], bnds->real_bounds.min.v[2], &gProgram_state.track_spec);
    XZToColumnXZ(&max_x, &max_z, bnds->real_bounds.max.v[0], bnds->real_bounds.max.v[2], &gProgram_state.track_spec);
    if (min_x != 0) {
        min_x--;
    }
    if (min_z != 0) {
        min_z--;
    }
    memcpy(&dimensions, &gProgram_state.track_spec, sizeof(dimensions));
    if (max_x + 1 < (int)(dimensions & 0xff)) {
        max_x++;
    }
    if (max_z + 1 < (int)((dimensions >> 8) & 0xff)) {
        max_z++;
    }
    for (x = min_x; x <= max_x; x++) {
        for (z = min_z; z <= max_z; z++) {
            if (gProgram_state.track_spec.columns[z][x].actor_0x0 != NULL) {
                emitted = max_face - ActorBoxPick(bnds, gProgram_state.track_spec.columns[z][x].actor_0x0, NULL, gBlack_material, &face_list[emitted], max_face - emitted, NULL, pWorld_callbacks);
            }
        }
    }
    if (gAdditional_actors != NULL) {
        emitted = max_face - ActorBoxPick(bnds, gAdditional_actors, NULL, gBlack_material, &face_list[emitted], max_face - emitted, NULL, pWorld_callbacks);
    }
    return emitted;
}

// GetFrictionFromFace

// FUNCTION: CARMA2_HW 0x004b5990
void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage) {
    char s[256];

    (void)pMessage;
    switch (pError) {
    case ePhysicsError_UnknownShapeType:
        strcpy(s, "Unknown shape type");
        break;
    case ePhysicsError_WireFrameHasNoPoints:
        strcpy(s, "Wire frame shape has no points");
        break;
    case ePhysicsError_PolyhedronHasNoPoints:
        strcpy(s, "Polyhedron shape has no points");
        break;
    case ePhysicsError_UnknownHingeType:
        strcpy(s, "Unknown hinge type");
        /* The original overwrites this message with the code. */
        /* fall through */
    default:
        sprintf(s, "%d ", pError);
        break;
    }
    FatalError(1000 + pError, s);
}

// ResetMan2

// SetUpTestObjects

// ResetObjectList

// ResetMan

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004b5ca0
void C2_HOOK_FASTCALL InitPhysics(void) {
    PhysicsSetErrorHandler(DoPhysicsError);
    InitPhysicsWorkspace(gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
}
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

