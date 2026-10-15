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
extern tCrush_list_item gCrush_lists[8];
extern int gCount_queued_drone_crushes;
extern int gCount_crush_detach_list;
extern int gINT_0067bdfc;
extern int gCount_toggled_doors;
extern tU8 gCrush_data_entry_counter;
extern int gCount_net_crush_detach_list;
extern int gCount_net_crush_semi_detach_bit_list;
extern int gCount_net_crush_full_detach_bit_list;
extern int gCount_net_crush_reattach_bit_list;
extern int gCount_car_damage_crush_list;

extern void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF);

extern void C2_HOOK_FASTCALL ClearCrushLists(void);

// RemoveCarFromCrushLists

extern void C2_HOOK_FASTCALL InitCrushSystems(void);

extern void C2_HOOK_FASTCALL ResetCrushSystems(void);

// ReadCrushData

// CreateDefaultCrushDataCB

// CreateDefaultCrushData

// ReadMasterCrushData

// LoadCrushDataFile

// LinkCrushDataToActor

// LinkCrushDataToActors

extern intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* actor, void* pData);

extern void C2_HOOK_FASTCALL InitModelMasterCrushData(tCar_spec* pCar_spec);

// InitNetworkShapesStuff

extern int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2);

extern int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2);

// InitPhysMasterCrushData

extern void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec);

extern tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid);

extern intptr_t C2_HOOK_FASTCALL SoftnessOfNearestPointCB(br_actor* pActor, br_vector3* pPoint, void* pData);

extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* cbfn, void* pContext);

extern float C2_HOOK_FASTCALL SoftnessOfNearestPoint(tCar_spec* pCar_spec, br_vector3* pPoint);

// CopyShapePolyhedron

// InitShapeStuff

extern intptr_t C2_HOOK_CDECL InitPhysCrushDataCB(br_actor* actor, void* data);

extern void C2_HOOK_FASTCALL CheckHingePointOrder(tCar_crush_flap_data* pHinge, br_model* pModel, br_vector3* pPos);

extern tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel);

extern float C2_HOOK_FASTCALL DistanceToBounds(const br_vector3* pPoint, const br_bounds3* pBounds);

extern tU16 C2_HOOK_FASTCALL FindNearestParentVertex(br_actor* pActor, br_vector3* pPos);

extern void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds);

extern void C2_HOOK_FASTCALL InitPhysCrushData(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL CalculateReferencePoints(br_model* pModel, br_model* pParent_model, tCar_crush_detach_data* pDetach_data, br_actor* pActor);

extern intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* actor, void* data);

extern void C2_HOOK_FASTCALL InitModelCrushData(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data* pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data);

extern float C2_HOOK_FASTCALL PointEdgeDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB);

extern void C2_HOOK_FASTCALL InitModelVertexData(tModel_detail_vertex_data* pVertex_data, br_model* pModel, int pDetail_level, tCar_crush_buffer_entry* pCrush_data, tCar_crush_spec* pCar_crush_spec);

// InitVertexDataCB

// InitVertexData

// CheckWheelPositions

// InitPhysModCrushData

extern void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DisposeCrushData(tCar_crush_buffer_entry* pCrush_data);

extern void C2_HOOK_FASTCALL DisposeVertexData(tModel_detail_vertex_data* pVertex_data);

// BuckleTheBugger

extern float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices);

extern float C2_HOOK_FASTCALL PointTriangleDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC);

extern int C2_HOOK_FASTCALL OutsideEdgeLine(const br_vector3* pPoint, const br_vector3* pA, const br_vector3* pDirection, const br_vector3* pInside);

extern float C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC);

extern float C2_HOOK_FASTCALL PointSimplexDistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices);

// LimitImpactScale

// GetImpactScale

extern int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

extern int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector);

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

extern void C2_HOOK_FASTCALL ExpandCrush(tCar_crush* pDest, const tCompressed_car_crush* pSrc);

extern void C2_HOOK_FASTCALL RemoveDuplicatePoints(int* pCount_out, br_vector3* pPoints_out, int pCount_in, const br_vector3* pPoints_in);

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

extern void C2_HOOK_FASTCALL SaveVitalBitsOfObject(tSaved_vital_bits* pDest, const tPhysics_object* pSrc, const tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL SwapCarShapes(tCar_spec* pCar_spec, tPhysics_shape* pCollision_shape);

extern int C2_HOOK_FASTCALL IsAWheelOrPivotActor(br_actor* pActor, tCar_spec* pCar);

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg);

extern int C2_HOOK_FASTCALL IsOnNonCarHalf(tCar_spec* pCar, const br_vector3* pPos);

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg);

// SplitCar

extern void C2_HOOK_FASTCALL QuantizePlane(br_vector4* pPlane);

extern int C2_HOOK_FASTCALL WithinBounds(br_scalar pValue, br_scalar pMin, br_scalar pMax);

extern int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane);

extern int C2_HOOK_FASTCALL QuantizeWithinBounds(br_scalar pQuant, br_scalar pMin, br_scalar pMax, br_scalar* pValue);

extern int C2_HOOK_FASTCALL FindForceEdge(const br_vector3* pPoint, const tPhysics_object* pObject, br_vector3* pEdge, br_scalar* pNormal_dot);

// DoXZSplitting

// DoYSplitting

// DoSplitting

// DoJointSnapping

extern int C2_HOOK_FASTCALL PointIsOutsideShape(br_vector3* pPoint, tCollision_shape_polyhedron_data* pPolyhedron);

extern int C2_HOOK_FASTCALL PointIsOutsideLimits(tCar_crush_shape_info* pShape, int pPoint_index, tCar_crush_limits* pLimits);

extern void C2_HOOK_FASTCALL ClipPointToPolyhedron(br_vector3* pPoint, tCollision_shape_polyhedron_data* pPolyhedron);

extern void C2_HOOK_FASTCALL QuantizeVector3(br_vector3* pV);

// MoveCarToSensiblePlace

// SwapShapesIfPossible

// DoShapeSwapping

extern void C2_HOOK_FASTCALL BendPoint(br_vector3* pPoint, const tBend_point_context* pContext);

extern int C2_HOOK_FASTCALL EdgeCrossesZPlane(br_scalar pZ, const br_vector3* pPoint1, const br_vector3* pPoint2);

// BendCollisionShape

// BendCollisionShapes

// BendModel

// BendModels

extern br_scalar C2_HOOK_FASTCALL BrAngleToSignedDegree(br_angle pAngle);

// BendWheels

// BendActor

// BendOtherThings

// ReAttachOrFullyDetachDifficultBits

// BendCar

extern br_angle C2_HOOK_FASTCALL QuantizeAngle(br_angle pAngle, br_angle pMax, br_angle pMin, br_angle pQuant);

// BendCarOneForce

// DoBending

extern void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

extern void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel);

extern intptr_t C2_HOOK_CDECL CheckFlapAngles(br_actor* pActor, void* pArg);

extern int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject);

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

extern intptr_t C2_HOOK_CDECL EnableGroovers(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL CompletelyUnBendCar(tCar_spec* pCar_spec);

extern br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount);

extern void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape);

extern void C2_HOOK_FASTCALL CompletelyUnBendCollisionShapes(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL CompletelyUnBendWheels(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL CompletelyUnBendOtherThings(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL DoCompletelyUnBentThings(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL TotallyRepairModel(br_model* pModel);

extern int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data);

extern intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser);

// SendReAttachAllBits

extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);

// TotallyRepairCar

extern void C2_HOOK_FASTCALL AddTotalDeflectionOfModel(br_model* pModel, float* pDeflection);

extern void C2_HOOK_FASTCALL UnBendActor(br_actor* pActor, tCar_bend_data* bend, br_angle pRepair_x, br_angle pRepair_y, br_angle pRepair_z, tCar_spec* pCar_spec);

extern intptr_t C2_HOOK_FASTCALL UnBendModels(br_actor* pActor, br_vector3* pTranslation, void* pUser);

extern void C2_HOOK_FASTCALL UnBendModel(br_model* pModel, const br_vector3* pTranslation, const tUnbend_model_context* pContext);

extern void C2_HOOK_FASTCALL UnBendCollisionShape(tCar_crush_shape_info* pShape, const tBend_point_context* pContext);

extern void C2_HOOK_FASTCALL UnBendCollisionShapes(tCar_spec* pCar_spec, const tBend_point_context* pContext);

extern void C2_HOOK_FASTCALL UnBendWheels(tCar_spec* pCar_spec, const tBend_point_context* pContext);

extern void C2_HOOK_FASTCALL UnBendOtherThings(tCar_spec* pCar_spec, tCar_bend_data* bend);

extern br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle);

extern br_scalar C2_HOOK_FASTCALL CalculateAmountRepairedFraction(br_angle pOriginal_x, br_angle pOriginal_y, br_angle pOriginal_z, br_angle pRepair_x, br_angle pRepair_y, br_angle pRepair_z, br_scalar* pOriginal_amount);

// ReAttachBits

extern intptr_t C2_HOOK_CDECL AddTotalDeflection(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL RepairObject(tPhysics_object* pCollision_info, void* pUser_data);

extern intptr_t C2_HOOK_CDECL RepairModels(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL RepairModel(br_model* pModel, tRepair_model_context* pContext);

extern void C2_HOOK_FASTCALL RepairCarCollisionShapes(tCar_spec* pCar_spec, br_scalar pRepair_step);

// RepairCar2

// RepairCar

extern void C2_HOOK_FASTCALL RestoreVitalBitsOfObject(tPhysics_object* pDest, const tSaved_vital_bits* pSrc);

// WeldCar

// ARCompressCrush

// ARApplyCrush

// ARUndoCrush

extern void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3* pDest, br_model* pModel, br_vector3* pSrc);

extern void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, const tCompressed_vector3* pMash);

extern void C2_HOOK_FASTCALL ARUndoModelMash(br_model* pModel, const tCompressed_vector3* pMash);

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

extern void C2_HOOK_FASTCALL CompressPlane(tU16 pDest[4], const br_vector4* pSrc);

extern void C2_HOOK_FASTCALL ExpandPlane(br_vector4* pDest, const tU16 pSrc[4]);

// CopySplitInMessage

extern void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes);

#endif // GUARD_13_CRUSH1_H
