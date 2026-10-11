#ifndef GUARD_49_PHYSICS_H
#define GUARD_49_PHYSICS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPHIL_mechanics_time_sync;
extern int gFace_num__car;
extern tPhysicsError_cbfn* gPhysics_error_cb;
extern void* gPhysics_buffer1_ptr;
extern void* gPhysics_buffer2_ptr;
extern void* gPhysics_buffer3_ptr;
extern int gPhysics_other_buffer_capacity;
extern void* gPhysics_other_buffer;

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

// HierarchyCalcEnergy

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

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

extern void C2_HOOK_FASTCALL PrepareForCollisions(tPhysics_object* pObject, tWorld_callbacks* pWorld_callbacks);

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
