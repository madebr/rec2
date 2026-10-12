#ifndef GUARD_49_PHYSICS_H
#define GUARD_49_PHYSICS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPHIL_mechanics_time_sync;
extern int gFace_num__car;
extern br_scalar gPhysics_bounds_max_x;
extern br_scalar gPhysics_bounds_min_x;
extern tPhysicsError_cbfn* gPhysics_error_cb;
extern tPolyhedron_edge_indexes gPhysics_box_edges_58f7f0[12];

extern char gPhysics_empty_message;
extern tU32 gPhysics_contact_679430;
extern tU32 gPhysics_contact_679434;
extern tU32 gPhysics_contact_679438;
extern void* gPhysics_buffer1_ptr;
extern void* gPhysics_buffer2_ptr;
extern void* gPhysics_buffer3_ptr;
extern int gPhysics_other_buffer_capacity;
extern void* gPhysics_other_buffer;

// count

void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest,const br_vector3* source,const br_matrix34* matrix);

typedef struct {
    br_scalar t;
    tPhysics_object* object1;
    tPhysics_object* object2;
    tFace_ref* face;
    tU32 field_0x10;
    br_vector3 position;
    br_vector3 reference_position;
    br_vector3 normal;
    tU8 field_0x38[36];
    int type;
} tPhysics_contact;

int C2_HOOK_FASTCALL AddObjectWallForce(tPhysics_contact* pContacts, const br_vector3* pPosition, br_scalar pT, const br_vector3* pNormal, int pUnused, int pCount, tPhysics_object* pObject, int pType, tFace_ref* pFace, const br_vector3* pReference_position);

// AddObjectObjectForce

tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

// RayCastThruFaceList2

// RayCastInBox

// FaceOffCount2

// FaceOffCount

// FaceOnCount

int C2_HOOK_FASTCALL EliminateFaces(tPhysics_object* pObject, tPhysics_shape* pShape, br_matrix34* pTransform);

// GetShapePoint

// GetObjectObjectEdgeIntersections

// GetFaceEdgeIntersection

// GetSphereToFacePoints

int C2_HOOK_FASTCALL CheckSingleEdgeWithSphere(br_vector3* pEdge_start, br_vector3* pEdge_end, tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_contact* pContacts, int pCount, br_vector3* pSphere_start, br_vector3* pSphere_end, tFace_ref* pFace);

int C2_HOOK_FASTCALL GetSphereToEdgePoints(tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_contact* pContacts, int pCount, int pUnused_capacity, br_vector3* pSphere_start, br_vector3* pSphere_end, br_matrix34* pUnused_matrix);

/* The original wall-contact workspace contains two arrays of 64 points. */
typedef struct {
    br_vector3 original_points[64];
    br_vector3 trial_points[64];
    tU32 vertex_flags[64];
    tU16 count_points;
    tU16 count_edges;
    tPolyhedron_edge_indexes* edges;
} tPhysics_wall_workspace;

int C2_HOOK_FASTCALL GetCornerToFacePoints(tPhysics_object* pObject, tPhysics_shape* pUnused_shape, tPhysics_contact* pContacts, int pCount, int pCapacity, tPhysics_wall_workspace* pWorkspace, void* pUnused_argument);

int C2_HOOK_FASTCALL FUN_004236a0(tPhysics_object* object, tPhysics_shape* shape, tPhysics_contact* contacts, int count, int capacity, tPhysics_wall_workspace* workspace, void* unused, int* touched);

int C2_HOOK_FASTCALL FUN_00424140(tPhysics_object* object, tPhysics_shape* shape, tPhysics_contact* contacts, int count, int unused_capacity, tPhysics_wall_workspace* workspace, void* unused);

int C2_HOOK_FASTCALL FUN_00424270(br_vector3* start, br_vector3* end, tPhysics_object* object, tPhysics_shape* shape, tPhysics_contact* contacts, int count, tFace_ref* face, tPhysics_wall_workspace* workspace);

int C2_HOOK_FASTCALL FUN_00424990(const br_vector3* old_origin, const br_vector3* old_direction, const br_vector3* moving_origin, const br_vector3* moving_direction, const br_vector3* velocity, const br_vector3* direction_change, tPhysics_contact* output, int unused, tPhysics_object* object, tPhysics_object* other);

int C2_HOOK_FASTCALL FUN_00425630(const br_vector3* n0, const br_vector3* n1, const br_vector3* old_origin, const br_vector3* old_direction, const br_vector3* moving_origin, const br_vector3* velocity0, const br_vector3* edge_change, const br_vector3* velocity1, tPhysics_contact* output, int unused, tPhysics_object* object, tPhysics_object* other);

int C2_HOOK_FASTCALL FUN_004228b0(tPhysics_object* object, tPhysics_contact* contacts, int initial_count);

void C2_HOOK_FASTCALL FUN_004b9230(tPhysics_object* pObject, tPhysics_object** pSelected, br_scalar pTime);

void C2_HOOK_FAKE_THISCALL FUN_004b8ee0(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime);

void C2_HOOK_FAKE_THISCALL FUN_004bc8e0(tPhysics_object* object, undefined4 unused, br_scalar factor);

int C2_HOOK_FASTCALL FUN_004bd720(tPhysics_object* object);

int C2_HOOK_FASTCALL FUN_004bd7f0(tPhysics_object* list);

tPhysics_object** C2_HOOK_FASTCALL FUN_004bd830(tPhysics_object* scan, tPhysics_object* context, tPhysics_object** output);

void C2_HOOK_FASTCALL FUN_004bd9b0(tPhysics_object* object, tU32 state);

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

br_scalar C2_HOOK_FASTCALL HierarchyCalcEnergy(tPhysics_object* pObject);

// CheckCollisionList

typedef struct {
    tU32 field_0;
    tPhysics_object* object_a;
    tPhysics_object* object_b;
    tU32 field_c;
    tU32 field_10;
    br_vector3 point_a;
    br_vector3 point_b;
    br_vector3 direction_a;
    br_vector3 direction_b;
    tU8 field_44[0x18];
    int type;
} tPhysics_joint_force;

void C2_HOOK_FASTCALL MakeJointVelocitiesVector(br_scalar* pOutput, tPhysics_joint_force* const* pForces, int pCount);

// ApplySingleForce

void C2_HOOK_FAKE_THISCALL ApplySingleForce(tPhysics_joint_force* record, undefined4 unused, br_scalar impulse);

// SkipForces

// ScaleUpObjectMI

// CheckForTwang

void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

int C2_HOOK_FASTCALL AllocateObjectWorkspace(tPhysics_object* object, tPhysics_joint_force* output);

void C2_HOOK_FASTCALL RedoQuickHingeVelocities(tPhysics_object* object);

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

int C2_HOOK_FASTCALL CountJointForces(tPhysics_object* pObject, tPhysics_object* pRoot);

// MoveJointedObject

// TellPhysicsJointsHaveChanged

// CombineBounds

// GetWorldBoundingBoxForObjects

void C2_HOOK_FASTCALL GetWorldBoundingBoxForObject(tPhysics_object* pObject, br_bounds* pBounds);

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

extern void C2_HOOK_FASTCALL PrepareForCollisions(tPhysics_object* pObject, tWorld_callbacks* pWorld_callbacks);

// CheckPointVelocities

void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject);

void C2_HOOK_FASTCALL SetListDoingNothingFlag(tPhysics_object* pObject, tU8 pFlag);

void C2_HOOK_FASTCALL SetGroupDoingNothingFlag(tPhysics_object* pObject, tU8 pFlag);

void C2_HOOK_FASTCALL SetHierarchyDoingNothingFlag(tPhysics_object* pObject, tU8 pFlag);

// CheckDoingNothingFlag

// SetGroup

// GroupTogether

// GetPreCalculatedJointForces

void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject);

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

void C2_HOOK_FASTCALL DRMatrix33Inverse(br_scalar* result, const br_scalar* pInput);

// SetUpQuickHingeData

// InitQuickHinge

// CalcQuickHingeData

void C2_HOOK_FASTCALL SetUpForce(br_vector3* out_omega, br_vector3* out_velocity, const br_vector3* lever, const br_vector3* force, const tPhysics_object* object);

tPhysics_object* C2_HOOK_FASTCALL SetUpTorqueHingePartners(br_vector3* pAngular, br_vector3* pLinear, br_vector3* pPartner_angular, br_vector3* pPartner_linear, const br_vector3* pTorque, tPhysics_object* pObject);

tPhysics_object* C2_HOOK_FASTCALL SetUpHingePartners(br_vector3* pAngular, br_vector3* pLinear, br_vector3* pPartner_angular, br_vector3* pPartner_linear, tPhysics_object* pObject);

void C2_HOOK_FASTCALL MakeMatrix(br_scalar* matrix, int dimension, tPhysics_joint_force* const* records, int count);

void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* object);

// RestoreGroupOmegas

int C2_HOOK_FASTCALL MakeVelocitiesVector2(br_scalar* pOutput, tPhysics_joint_force* const* pForces, int pCapacity, int pCount);

// MakeVelocitiesVector

// NewAddFrictionObjectObject

void C2_HOOK_FASTCALL NewAddFriction(tPhysics_joint_force* force, tWorld_callbacks* callbacks);

int C2_HOOK_FASTCALL FUN_004baa00(tPhysics_object* objects);

// ApplyForces

void C2_HOOK_FASTCALL DoJointFriction(tPhysics_object* object);

// ListDoJointFriction

void C2_HOOK_FASTCALL PostAddFriction(tPhysics_object* object);

// RotateObjectSecondOrder

void C2_HOOK_FAKE_THISCALL RotateObjectFirstOrder(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime);

void C2_HOOK_FAKE_THISCALL RotateObject(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime);

void C2_HOOK_FAKE_THISCALL TranslateObject(tPhysics_object* pObject, undefined4 pUnused, br_scalar pTime);

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

void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object* pParent, tPhysics_object* pRoot);

void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pObject);

// ApplyTorque

// ApplyDiminishingTorque

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

extern int gPhysics_rq_mode_6940b8;

int C2_HOOK_FASTCALL FUN_004c3b60(br_scalar* original, br_scalar* rhs, br_scalar* result, int size, int unused1, unsigned char* excluded, int* excluded_count, int unconstrained, int unused2, const void* const* cached_groups, int group_count);

void C2_HOOK_FASTCALL FUN_004c61e0(const char* ignored);

void C2_HOOK_FASTCALL RQExpand(br_scalar* matrix, const br_scalar* original, int old_size, int size, br_scalar* orthogonal, const unsigned char* indices);

void C2_HOOK_FASTCALL RQSingleExpand(br_scalar* matrix, const br_scalar* original, int size, br_scalar* orthogonal, const unsigned char* indices);

void C2_HOOK_FASTCALL RQFromCachedGroups(br_scalar* matrix, br_scalar* orthogonal, const br_scalar* original, br_scalar* result, const br_scalar* rhs, const unsigned char* indices, int size, const void* const* cached_groups, int group_count);

int C2_HOOK_FASTCALL RQDcmpV(br_scalar* matrix, int size, br_scalar* orthogonal);

// MakeIdentity

void C2_HOOK_FASTCALL SparseRQDcmpV(br_scalar* matrix, br_scalar* orthogonal, const br_scalar* rhs, br_scalar* result, int size);

// MakeMatrixFromIndices

// HasBeenDoneBefore

// RemoveRQSingularities

// AddToMatrix

// RQRemovedOneTooManyForces

// GetInverseM

// RQMatrixSolveNoNegatives

// RQMatrixSolveWithNegatives

int C2_HOOK_FASTCALL MatrixProcess(br_scalar* matrix, br_scalar* orthogonal, int size);

void C2_HOOK_FASTCALL RqBackSub(br_scalar* matrix, br_scalar* orthogonal, const br_scalar* rhs, br_scalar* result, int size);

extern void* gPhysics_scratch_ptr;

void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize);

void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize);

void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void);

extern tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType);

extern tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType);

extern tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType);

extern tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType);

// GetJointSize

extern tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType);

extern tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType);

// FreeShape

extern void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape);

extern void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint);

// GetBoundsFromPointList

// FillInShape

// UpdateCollisionObject

extern void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);

// PhysicsWarning

extern void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn);

// RepositionAndOrientateChildren

// PhysicsAddObject

// PhysicsAddObjectList

// PhysicsRemoveObject

extern int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize);

extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks);

// GetFrictionFromFace

extern void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage);

// ResetMan2

// SetUpTestObjects

// ResetObjectList

// ResetMan

extern void C2_HOOK_FASTCALL InitPhysics(void);

// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

#endif // GUARD_49_PHYSICS_H
