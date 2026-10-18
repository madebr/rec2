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

// RemoveCarFromCrushLists

extern void C2_HOOK_FASTCALL InitCrushSystems(void);

// ResetCrushSystems

// ReadCrushData

// CreateDefaultCrushDataCB

// CreateDefaultCrushData

// ReadMasterCrushData

// LoadCrushDataFile

// LinkCrushDataToActor

// LinkCrushDataToActors

// AccumulateSquashVertices

// InitModelMasterCrushData

// InitNetworkShapesStuff

// IncreasingCompare

// DecreasingCompare

// CrushLimitNumber

// SoftnessOfNearestPointCB

// DRActorEnumRecurseWithTranslation

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

// InitModelCrushDataCB

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

// DistSq

// LimitImpactScale

// GetImpactScale

// TooFarToCrush

// NearestAxis

// CrushModel

// PossibleShockSmash

// CrushModels

// CrushPoint

// ClipPointToLimits

// CrushCollisionShapes

// SimpleCrushCarModelsCB

// SimpleCrushCar

// CrushCar

// SendCrush

extern void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc);

// ExpandCrush

// RemoveDuplicatePoints

// SimpleCrushModel

// CrushDrone

// CrushDroneObject

// DoDroneCrushing

// AddDroneCrush

// QueueDroneCrush

// DoCrushing

// SetCrush

// QueueCarCrush

// SetBitForDetachment

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

// GetSDBJointPosAndBounds

// SendSemiDetachBit

// SendDetachBit

// SendAllSemiDetachedBits

// AddNonCar

// DetachBit

// DoDetaching

// FlapBit

// FlapBits

// StartFlapping

// MungeDetachLists

// SendReAttachBit

// StopFlapping

// DoFlapping

// ToggleFlapsCB2

// ToggleFlapsCB

// SendToggleFlaps

// ToggleFlaps

// BitObjectIsSufficientlyOutsideCarObjectToDetach

// FullyDetachBit

// SendFullyDetachBit

// DoFullyDetaching

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

// JitModelUpdate

// SetModelForUpdate

// RandomlyXZSplitCar

// RandomlyBendCar

// TotallySpamTheCar

// DRActorEnumRecurseUltraPlus

// FireOffBitsCB

// FireSomeBitsOffCar

// DoSpams

// CrushBendFlapRend

// SphericizeModel

// SphericizePhysics

// SphericizeCar

// BattenDownTheHatches

// BattenDownTheObjects

// CompletelyUnBendCollisionShape

// CompletelyUnBendCollisionShapes

// CompletelyUnBendWheels

// CompletelyUnBendOtherThings

// EnableGroovers

// DoCompletelyUnBentThings

// TotallyRepairCarCollisionShapes

// TotallyRepairModel

// TotallyRepairModels

// TotallyRepairObject

// SendReAttachAllBits

extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);

// TotallyRepairCar

// AddTotalDeflectionOfModel

// AddTotalDeflection

// RepairModel

// RepairModels

// UnBendModel

// UnBendModels

// UnBendActor

// UnBendCollisionShape

// UnBendCollisionShapes

// UnBendWheels

// UnBendOtherThings

// NormalizedAngle

// CalculateAmountRepairedFraction

// UnBendCar

// CompletelyUnBendCar

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

// ARCompressModelMash

// ARApplyModelMash

// ARUndoModelMash

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

// EncodeNetShapeData

// DecodeNetShapeData

// CopyBendInMessage

// Vector4Equal

// CompressPlane

// ExpandPlane

// CopySplitInMessage

// SetAllShapeStatusFlags

#endif // GUARD_13_CRUSH1_H
