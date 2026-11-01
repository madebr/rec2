/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_49_PHYSICS_H
#define GUARD_49_PHYSICS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tPhysicsError_cbfn* gPhysics_error_cb;
extern void* gPhysics_buffer1_ptr;
extern void* gPhysics_buffer2_ptr;
extern int gImmortal_peds;
extern int gBOOL_00744804;
extern tPedestrian* gPedestrian_array;
extern void* gPhysics_buffer3_ptr;
extern int gPHIL_mechanics_time_sync;
extern int gPhysics_other_buffer_capacity;
extern tProximity_ray gProximity_rays[20];
extern br_actor* gPed_limbed_actors[30];
extern void* gPhysics_other_buffer;
extern void* gPhysics_scratch_ptr;

// count

void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix);

// AddObjectWallForce

extern int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList2(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

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

extern int C2_HOOK_FASTCALL BoundsOverlapTest(br_bounds3* pBounds_1, br_bounds3* pBounds_2);

// GetObjectObjectEdgePoints

// GetSphereNonSphereForceInfo

// GetSphereSphereForceInfo

// GetSphereShapeForceInfo

// GetNonSphereNonSphereForceInfo

// GetNonSphereShapeForceInfo

// GetObjectObjectForceInfo

extern void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex);

// AddOldForces

extern int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2);

extern int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, tPhysics_shape* pCollision_shape_1, tPhysics_shape* pCollision_shape_2);

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

extern void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

extern void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

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

extern void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SetListDoingNothingFlag(tPhysics_object* pObject, int pFlag);

// SetGroupDoingNothingFlag

extern void C2_HOOK_FASTCALL SetHierarchyDoingNothingFlag(tPhysics_object* pObject, int pFlag);

// CheckDoingNothingFlag

// SetGroup

// GroupTogether

// GetPreCalculatedJointForces

extern void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject);

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

// CheckForObjectHierachyTouchingObjectList

// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

// SetObjectTrialPositionType

// FillInTrialWorkspace

extern int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2);

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

extern void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* pObject);

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

extern void C2_HOOK_FASTCALL RotateObjectFirstOrder(tPhysics_object* pObject, br_scalar pTime);

// RotateObject

// TranslateObject

// GetFacesNearObject

extern void C2_HOOK_FASTCALL ResetObjectAndChildren(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects);

// RepositionObjectGroup

// SimpleRotate

// InterpolateSingleObject

// InterpolateObjects

// ApplyGravitySingleObject

// ApplyGravity

// SetGravityDeltaV

// TrialPositionChildren

extern void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object* pParent, tPhysics_object* pRoot);

extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info);

// ApplyTorque

// ApplyDiminishingTorque

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

// RQExpand

// RQSingleExpand

extern int C2_HOOK_FASTCALL RQDcmpV(br_scalar* pMatrix, int pDim, br_scalar* pQ);

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

extern int C2_HOOK_FASTCALL MatrixProcess(br_scalar* pR, br_scalar* pQ, int pDim);

extern void C2_HOOK_FASTCALL RqBackSub(const br_scalar* pR, const br_scalar* pQ, const br_scalar* pRhs, br_scalar* pX, int pDim);

extern void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize);

extern void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize);

extern void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void);

extern tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType);

extern tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType);

extern tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType);

extern tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType);

// GetJointSize

extern tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType);

extern tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType);

extern void C2_HOOK_FASTCALL FreeShape(void* pData);

extern void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape);

extern void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint);

extern void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds);

extern void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);

// UpdateCollisionObject

extern void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);

// PhysicsWarning

extern void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn);

// RepositionAndOrientateChildren

extern void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild);

extern void C2_HOOK_FASTCALL PhysicsAddObjectList(tPhysics_object* pParent, tPhysics_object* pChild);

extern void C2_HOOK_FASTCALL PhysicsRemoveObject(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize);

extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks);

extern float C2_HOOK_FASTCALL GetFrictionFromFace(const tFace_ref* pFace);

extern void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage);

// ResetMan2

extern void C2_HOOK_FASTCALL SetUpTestObjects(void);

// ResetObjectList

// ResetMan

extern void C2_HOOK_FASTCALL InitPhysics(void);

// PointOutOfSightNotAR

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

#endif // GUARD_49_PHYSICS_H
