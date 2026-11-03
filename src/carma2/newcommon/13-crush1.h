/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_13_CRUSH1_H
#define GUARD_13_CRUSH1_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include <stdio.h>

extern undefined4 gCrush_array_0067a190[10];
extern undefined4 gDAT_00679440;
extern tCrush_info_buffer gDetached_bit_crush_info_buffer;
extern tPhysics_object* gDetached_bit_collision_infos;
extern tDriver gDetached_bit_driver;
extern tPhysics_joint* gTrack_crush_joints[32];
extern tCrush_info_buffer gSplit_car_crush_info_buffer;
extern tPhysics_object* gSplit_car_collision_infos;
extern tDriver gSplit_car_driver;
extern tPhysics_shape* gGonad_sphere_collision_shape;

extern void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF);

// ClearCrushLists

extern void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL InitCrushSystems(void);

// ResetCrushSystems

// ReadCrushData

extern intptr_t C2_HOOK_CDECL CreateDefaultCrushDataCB(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL CreateDefaultCrushData(br_actor* pActor, tCar_spec* pCar_spec);

// ReadMasterCrushData

// LoadCrushDataFile

// LinkCrushDataToActor

// LinkCrushDataToActors

extern void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor* pActor, tCar_crush_buffer* pBuffer);

extern intptr_t C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* pData);

extern intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* pActor, void* pData);

// InitModelMasterCrushData

extern void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush);

extern int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2);

extern int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2);

extern tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid);

// SoftnessOfNearestPointCB

extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* pCallback, void* pContext);

// SoftnessOfNearestPoint

// SetUpShapeLimitingStuff

// CopyShapePolyhedron

// InitShapeStuff

// InitPhysMasterCrushData

// CheckHingePointOrder

// FindNearestVertex

// DistanceToBounds

// FindNearestParentVertex

// SetUpSemiDetachJointStuff

// InitPhysCrushDataCB

// InitPhysCrushData

// CalculateReferencePoints

extern intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* pActor, void* pData);

// InitModelCrushData

// PointEdgeDistSq

// SetFlapCheckVertices

// InitModelVertexData

// InitVertexDataCB

// InitVertexData

// CheckWheelPositions

// InitPhysModCrushData

extern void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DisposeCrushData(tCar_crush_buffer_entry* pCrush_data);

extern void C2_HOOK_FASTCALL DisposeVertexData(tModel_detail_vertex_data* pVertex_data);

// BuckleTheBugger

// OutsideEdgeLine

// PointTriangleDistSq

// PointRectDistSq

// PointSimplexDistSq

extern float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices);

// LimitImpactScale

extern float C2_HOOK_FASTCALL GetImpactScale(const br_vector3* pP, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

extern int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

extern int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector);

// CrushModel

// PossibleShockSmash

// CrushModels

// CrushPoint

extern void C2_HOOK_FASTCALL ClipPointToLimits(br_vector3* pPoint, tU16 pLimit_bits, const tCar_crush_limits* pLimits);

// CrushCollisionShapes

// SimpleCrushCarModelsCB

// SimpleCrushCar

// CrushCar

// SendCrush

extern void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc);

extern void C2_HOOK_FASTCALL ExpandCrush(tCar_crush* pDest, const tCompressed_car_crush* pSrc);

// RemoveDuplicatePoints

// SimpleCrushModel

// CrushDrone

// CrushDroneObject

// DoDroneCrushing

extern void C2_HOOK_FASTCALL AddDroneCrush(tQueued_drone_crush_item* pCrush, const br_vector3* pVector, float pScale, int pCount, const br_vector3* pPoints);

// QueueDroneCrush

// DoCrushing

extern void C2_HOOK_FASTCALL SetCrush(tCar_crush* pCrush, int pCount_vectors, br_vector3* pVectors, br_vector3* pVector1, br_vector3* pVector2, float pScalar, tU32 pValue);

// QueueCarCrush

extern void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4);

// SetFlappage

// GetUniqueID

// InitializeCollisionInfo

// RemoveBit

// RemoveNonCarChildren

// RemoveNonCar

// GetNonCar

// GetNonCarObject

// SetRelationsDoingNothingFlags

// SetUpDetachedBitObject

// SetUpSemiDetachedBitJoint

// BitIsInBentPartOfCar

// SetUpFlapJoint

extern void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel);

// CheckFlapAngles

extern void C2_HOOK_FASTCALL MakeModelMaterialsSingleSided(br_model* pModel);

// ReAttachBit

// CentreModelOnPoint

// SemiDetachBit

extern int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP3, br_bounds3* pBounds, br_actor* pActor);

extern void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, tNet_game_player_info* pPlayer);

// SendDetachBit

extern void C2_HOOK_FASTCALL SendAllSemiDetachedBits(tNet_game_player_info* pPlayer);

// AddNonCar

// DetachBit

// DoDetaching

// FlapBit

// FlapBits

// StartFlapping

extern void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush);

extern void C2_HOOK_FASTCALL SendReAttachBit(tCar_spec* pCar, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec);

// DoFlapping

// ToggleFlapsCB2

extern int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data);

// SendToggleFlaps

extern void C2_HOOK_FASTCALL ToggleFlaps(void);

extern int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec *pCar, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL DoFullyDetaching(void);

// CalculateCollisionObjectStuffForSplitHalves

// SetUpCollisionInfoStuffForSplitHalf

// SetFace

// AddSplitVertexIndex

// CreateSplitVertex

// GetSplitVertex

// SplitModel

// CreateSplitPoints

// SplitCollisionShape

// SplitCarCollisionShapes

// QuantizeWithinBounds

// SaveVitalBitsOfObject

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg);

// SwapCarShapes

// IsAWheelOrPivotActor

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg);

// IsOnNonCarHalf

// SplitCar

// QuantizePlane

// WithinBounds

// SplitCheck

// FindForceEdge

// DoXZSplitting

// DoYSplitting

// DoSplitting

// DoJointSnapping

// PointIsOutsideShape

// PointIsOutsideLimits

// ClipPointToPolyhedron

// QuantizeVector3

// MoveCarToSensiblePlace

// SwapShapesIfPossible

// DoShapeSwapping

// BendPoint

// EdgeCrossesZPlane

// BendCollisionShape

// BendCollisionShapes

// BendModel

// BendModels

// BrAngleToSignedDegree

// BendWheels

// BendActor

// BendOtherThings

// ReAttachOrFullyDetachDifficultBits

// BendCar

// QuantizeAngle

// BendCarOneForce

// DoBending

extern void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

extern void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel);

// RandomlyXZSplitCar

// RandomlyBendCar

// TotallySpamTheCar

extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseUltraPlus(br_actor* pActor, br_actor_enum_cbfn* pCallback, void* pArg);

// FireOffBitsCB

// FireSomeBitsOffCar

// DoSpams

// CrushBendFlapRend

// SphericizeModel

// SphericizePhysics

// SphericizeCar

extern intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data);

extern void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape);

// CompletelyUnBendCollisionShapes

// CompletelyUnBendWheels

// CompletelyUnBendOtherThings

extern intptr_t C2_HOOK_CDECL EnableGroovers(br_actor* pActor, void* pData);

// DoCompletelyUnBentThings

extern void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec* pCar_spec);

// TotallyRepairModel

extern intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data);

extern void C2_HOOK_FASTCALL SendReAttachAllBits(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL TotallyRepairCar(void);

// AddTotalDeflectionOfModel

// AddTotalDeflection

// RepairModel

// RepairModels

// UnBendModel

// UnBendModels

// UnBendActor

typedef struct tBend_data_local {
    br_scalar field_0x0;
    br_scalar field_0x4;
    undefined field_0x8[4];
    br_scalar field_0xc;
    br_scalar field_0x10;
    undefined field_0x14[4];
    br_vector3 field_0x18;
    br_vector3 field_0x24;
    br_vector3 field_0x30;
    br_angle field_0x3c;
    br_angle field_0x3e;
    br_angle field_0x40;
    undefined field_0x42[2];
    br_vector3 field_0x44;
    br_vector3 field_0x50;
    br_vector3 field_0x5c;
    br_vector3 field_0x68;
    int field_0x74;
    tCar_crush_vertex_data* field_0x78;
} tBend_data_local;

typedef struct tBend_point_context_local {
    tCar_spec* car;
    br_matrix34 matrix;
    tBend_data_local* bend;
} tBend_point_context_local;

extern void C2_HOOK_FASTCALL UnBendCollisionShape(tCar_crush_shape_info* pShape, const tBend_point_context_local* pContext);

// UnBendCollisionShapes

// UnBendWheels

// UnBendOtherThings

extern br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle);

// CalculateAmountRepairedFraction

extern br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount);

extern void C2_HOOK_FASTCALL CompletelyUnBendCar(tCar_spec* pCar_spec);

// ReAttachBits

// RepairObject

// RepairCarCollisionShapes

// RepairCar2

// RepairCar

// RestoreVitalBitsOfObject

// WeldCar

// ARCompressCrush

// ARApplyCrush

// ARUndoCrush

extern void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3* pDest, br_model* pModel, br_vector3* pSrc);

extern void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, tCompressed_vector3* pSrc);

extern void C2_HOOK_FASTCALL ARUndoModelMash(br_model* pModel, tCompressed_vector3* pSrc);

// ARApplyFlap

// ARApplySplitWeld

// ARUndoSplitWeld

// ReceivedCrush

// FindBitWithID

// AttachedBitFromID

// ReceivedSemiDetachBit

// ReceivedDetachBit

// SemiDetachedBitObjectFromID

// ReceivedFullyDetachBit

// DetachedBitFromID

// SemiDetachedBitFromID

// ReceivedReAttachBit

// ReceivedReAttachAllBits

// DoQueuedNetDetaching

// DBNonCarOwner

// SendDBNonCar

// ReceivedDBNonCar

// SendAllDBNonCars

// ReceivedDBNonCarPosition

// ReceivedToggleFlaps

// SHNonCarOwner

// SendSHNonCar

// SendAllSHNonCars

// ReceivedSHNonCar

// GetNetShapeDataLengthFromShape

// GetNetShapeDataLengthFromData

extern void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar);

// DecodeNetShapeData

// CopyBendInMessage

// Vector4Equal

extern void C2_HOOK_FASTCALL CompressPlane(tU16* pDest, const br_vector4* pSrc);

extern void C2_HOOK_FASTCALL ExpandPlane(br_vector4* pDest, const tU16* pSrc);

// CopySplitInMessage

extern void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes);

extern float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar_spec);
extern int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3);
extern int C2_HOOK_FASTCALL FUN_00424140(tPhysics_object* pObject, tPhysics_shape* pBounds_shape, tPhysics_shape* pRay_shape, int pCollision_count, undefined4 pArg5, tU8* pPolyhedron_data, undefined4 pArg7);
extern void C2_HOOK_FASTCALL AddToggledDoor(br_actor* pActor, tCar_spec* pCar, int pFlag, float pDelay);
extern int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data);
extern intptr_t C2_HOOK_FASTCALL CrushCarModelCbfn(br_actor* pActor, br_matrix34* pMat, void* pContext);
extern intptr_t C2_HOOK_FASTCALL FUN_004358a0(br_actor* pActor, br_vector3* pDelta, void* pContext);
extern void C2_HOOK_FASTCALL FUN_00439500(void);
extern intptr_t C2_HOOK_FASTCALL UnBendActorCB(br_actor* pActor, br_vector3* pDelta, void* pData);
extern int C2_HOOK_CDECL FindActorByCrushEntryID(br_actor* pActor, void* pArg);
extern void C2_HOOK_FASTCALL ApplyRandomDamageToCar(tCar_spec* pCar_spec, int pMin_damage, int pMax_damage);
extern void C2_HOOK_FASTCALL PHILDetachObjectState(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL FUN_004c7f40(br_model* pModel, br_vector3* pSrc, const float* pScalars);
extern void C2_HOOK_FASTCALL FUN_004c7fa0(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates);
extern void C2_HOOK_FASTCALL FUN_004c86b0(uintptr_t pId, br_actor* pActor);
extern void C2_HOOK_FASTCALL FUN_004c86f0(int pOwner, void* pData, tU32 pArg3);
extern void C2_HOOK_FASTCALL FUN_004ee250(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPos, void* pArg4, br_actor* pActor, int pArg6, tCar_crush_smashable_part* pSmashable, float pDamage, int pArg9);
extern void C2_HOOK_FASTCALL FUN_004f5a60(tCar_spec* pCar_spec);

#endif // GUARD_13_CRUSH1_H
