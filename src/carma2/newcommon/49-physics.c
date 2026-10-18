#include "49-physics.h"

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

// FUNCTION: CARMA2_HW 0x004225b0
void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix) {
    br_vector3 relative;

    BrVector3Sub(&relative, source, (br_vector3*)matrix->m[3]);
    BrMatrix34TApplyV(dest, &relative, matrix);
}

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

// HierarchyCalcEnergy

// CheckCollisionList

// MakeJointVelocitiesVector

// ApplySingleForce

// ApplyJointForces

// SkipForces

// ScaleUpObjectMI

// CheckForTwang

// FUNCTION: CARMA2_HW 0x004b8c20
void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0x0, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_a, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_b, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_a, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_b, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_a, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_b, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, type, 0x5c);

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    pForce->field_0xc = NULL;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, pDirection, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 10;
}

// FUNCTION: CARMA2_HW 0x004b8e10
void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, &pForce->direction_a, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 11;
}

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

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

// PrepareForCollisions

// CheckPointVelocities

// WriteNewPositions

// SetListDoingNothingFlag

// SetGroupDoingNothingFlag

// SetHierarchyDoingNothingFlag

// CheckDoingNothingFlag

// SetGroup

// GroupTogether

// GetPreCalculatedJointForces

// MakeObjectListDoSomething

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

// FUNCTION: CARMA2_HW 0x004c03f0
void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* pObject) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, rotate_omega, 0x1c0);

    while (pObject != NULL) {
        BrVector3Copy(&pObject->omega, &pObject->rotate_omega);
        if (pObject->child != NULL) {
            RestoreListOmega(pObject->child);
        }
        pObject = pObject->next;
    }
}

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

// FUNCTION: CARMA2_HW 0x004c63b0
void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->next = pParent->child;
    pParent->child = pChild;
    pChild->parent = pParent;
}

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
int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks) {
    tU8 cx_min;
    tU8 cz_min;
    tU8 cx_max;
    tU8 cz_max;
    int x;
    int z;
    int emitted;
    tTrack_spec* track_spec;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTrack_spec, ncolumns_x, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTrack_spec, ncolumns_z, 0x1);

    emitted = 0;
    track_spec = &gProgram_state.track_spec;
    FillInBounds(pBounds);
    XZToColumnXZ(&cx_min, &cz_min, pBounds->real_bounds.min.v[0], pBounds->real_bounds.min.v[2], track_spec);
    XZToColumnXZ(&cx_max, &cz_max, pBounds->real_bounds.max.v[0], pBounds->real_bounds.max.v[2], track_spec);
    if (cx_min > 0) {
        cx_min--;
    }
    if (cz_min > 0) {
        cz_min--;
    }
    if (cx_max + 1 < track_spec->ncolumns_x) {
        cx_max++;
    }
    if (cz_max + 1 < track_spec->ncolumns_z) {
        cz_max++;
    }
    for (x = cx_min; x <= cx_max; x++) {
        for (z = cz_min; z <= cz_max; z++) {
            if (track_spec->columns[z][x].actor_0x0 != NULL) {
                emitted = pMax_face - ActorBoxPick(pBounds, track_spec->columns[z][x].actor_0x0, NULL, gBlack_material, &pFace_list[emitted], pMax_face - emitted, NULL, pWorld_callbacks);
            }
        }
    }
    if (gAdditional_actors != NULL) {
        emitted = pMax_face - ActorBoxPick(pBounds, gAdditional_actors, NULL, gBlack_material, &pFace_list[emitted], pMax_face - emitted, NULL, pWorld_callbacks);
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
#ifndef REC2_MATCHING
        break;
#endif
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

