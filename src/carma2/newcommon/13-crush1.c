#include "13-crush1.h"

#include "08-loading1.h"
#include "09-funks.h"
#include "23-piping.h"
#include "41-utility.h"
#include "49-physics.h"
#include "51-smash.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00679698
br_scalar gDistortion_factor;

// GLOBAL: CARMA2_HW 0x006796b0
br_scalar gForce_to_movement_factor;

// GLOBAL: CARMA2_HW 0x00679550
br_scalar gMax_detach_time_ms;

// GLOBAL: CARMA2_HW 0x006796b8
br_scalar gMin_crush_force;

// GLOBAL: CARMA2_HW 0x0067944c
br_scalar gNormal_force_to_detach;

// GLOBAL: CARMA2_HW 0x0067a18c
br_scalar gMin_bend_angle;

// GLOBAL: CARMA2_HW 0x0067b7b0
int gMin_bend_damage;

// GLOBAL: CARMA2_HW 0x0067b7b4
br_scalar gFlap_inertia_fudge_biscuit;

// GLOBAL: CARMA2_HW 0x0067b7d0
int gMax_split_damage;

// GLOBAL: CARMA2_HW 0x0067bac8
br_scalar gMin_force_to_split_XZ_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bacc
br_scalar gMax_crush_dist_sq;

// GLOBAL: CARMA2_HW 0x0067bd5c
br_scalar gMin_force_to_split_Y_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bd64
int gMin_split_damage;

// GLOBAL: CARMA2_HW 0x0067bde8
br_scalar gMin_bend_force;

// GLOBAL: CARMA2_HW 0x0067bdec
br_scalar gMax_crush_force;

// GLOBAL: CARMA2_HW 0x0067bdf4
int gMax_bend_damage;

// GLOBAL: CARMA2_HW 0x0067bdf8
br_scalar gTorque_to_snap_per_tonne;

// GLOBAL: CARMA2_HW 0x0067be00
br_scalar gChance_of_bending;

// GLOBAL: CARMA2_HW 0x0067be04
br_scalar gChance_of_inverse_buckle;

// GLOBAL: CARMA2_HW 0x0067be78
br_vector3 gBatty_gravity;

// GLOBAL: CARMA2_HW 0x0067a190
undefined4 gCrush_array_0067a190[10];

// GLOBAL: CARMA2_HW 0x00679440
undefined4 gDAT_00679440;

// GLOBAL: CARMA2_HW 0x006796a0
tCrush_info_buffer gDetached_bit_crush_info_buffer;

// GLOBAL: CARMA2_HW 0x00679448
tPhysics_object* gDetached_bit_collision_infos;

// GLOBAL: CARMA2_HW 0x006796ac
tDriver gDetached_bit_driver;

// GLOBAL: CARMA2_HW 0x0067bd68
tPhysics_joint* gTrack_crush_joints[32];

// GLOBAL: CARMA2_HW 0x0067b7b8
tCrush_info_buffer gSplit_car_crush_info_buffer;

// GLOBAL: CARMA2_HW 0x0067a188
tPhysics_object* gSplit_car_collision_infos;

// GLOBAL: CARMA2_HW 0x0067b7c4
tDriver gSplit_car_driver;

// GLOBAL: CARMA2_HW 0x006796b4
tPhysics_shape* gGonad_sphere_collision_shape;

// FUNCTION: CARMA2_HW 0x00429bb0
void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF) {
    char s[256];
    char *result;

    while (1) {
        result = GetALineAndDontArgue(pF, s);
        if (result == NULL) {
            break;
        }
        if (strcmp("START OF CRUSH SETTINGS", s) == 0) {
            break;
        }
    }
    if (result == NULL) {
        PDFatalError("Can't find start of CRUSH SETTINGS in .TXT file");
    }
    if (GetAnInt(pF) != 4) {
        PDFatalError("Wrong version of CRUSH SETTINGS");
    }
    /* CRUSHING */
    gDistortion_factor = GetAScalar(pF);
    gMin_crush_force = GetAScalar(pF);
    gMax_crush_force = GetAScalar(pF);
    gForce_to_movement_factor = GetAScalar(pF);
    gMax_crush_dist_sq = GetAScalar(pF);

    /* SPLITTING */
    gMin_force_to_split_XZ_per_tonne = GetAScalar(pF);
    gMin_force_to_split_Y_per_tonne = GetAScalar(pF);
    gMin_split_damage = GetAnInt(pF);
    gMax_split_damage = GetAnInt(pF);

    /* BUCKLING */
    gChance_of_inverse_buckle = GetAScalar(pF);

    /* FLAPPING AND JOINT SNAPPING */
    gFlap_inertia_fudge_biscuit = GetAScalar(pF);
    gBatty_gravity.v[1] = GetAScalar(pF);
    gTorque_to_snap_per_tonne = GetAScalar(pF);

    /* DETACHING */
    gMax_detach_time_ms = GetAScalar(pF);
    gNormal_force_to_detach = GetAScalar(pF);

    /* BENDING */
    gMin_bend_force = GetAScalar(pF);
    gChance_of_bending = GetAScalar(pF);
    gMin_bend_angle = GetAScalar(pF);
    gMin_bend_damage = GetAnInt(pF);
    gMax_bend_damage = GetAnInt(pF);
    while (1) {
        result = GetALineAndDontArgue(pF, s);
        if (result == NULL) {
            break;
        }
        if (strcmp("END OF CRUSH SETTINGS", s) == 0) {
            break;
        }
    }
    if (result == NULL) {
        PDFatalError("Can't find end of CRUSH SETTINGS in .TXT file");
    }
}

// ClearCrushLists

// RemoveCarFromCrushLists

// FUNCTION: CARMA2_HW 0x00429fa0
void C2_HOOK_FASTCALL InitCrushSystems(void) {
    int i;

    memset(gCrush_array_0067a190, 0, sizeof(gCrush_array_0067a190));
    gDAT_00679440 = 0;
    gDetached_bit_crush_info_buffer.capacity = 16;
    gDetached_bit_crush_info_buffer.crush_infos = BrMemAllocate(gDetached_bit_crush_info_buffer.capacity * sizeof(tCrush_info), kMem_crush_data);
    gDetached_bit_collision_infos = BrMemAllocate(gDetached_bit_crush_info_buffer.capacity * sizeof(tPhysics_object), kMem_crush_data);
    for (i = 0; i < gDetached_bit_crush_info_buffer.capacity; i++) {
        gDetached_bit_crush_info_buffer.crush_infos[i].field_0xdc = 1.f;
        gDetached_bit_crush_info_buffer.crush_infos[i].field_0xe0 = 0.28985506f;
        gDetached_bit_crush_info_buffer.crush_infos[i].collision_object = &gDetached_bit_collision_infos[i];
        gDetached_bit_collision_infos[i].shape = (tPhysics_shape*)AllocateShapePolyhedron(16, kMem_crush_data);
    }
    gDetached_bit_driver = eDriver_detached_bit;
    for (i = 0; i < (int)REC2_ASIZE(gTrack_crush_joints); i++) {
        gTrack_crush_joints[i] = AllocatePhysicsJoint(3, kMem_crush_data);
    }
    gSplit_car_crush_info_buffer.capacity = 3;
    gSplit_car_crush_info_buffer.crush_infos = BrMemAllocate(gSplit_car_crush_info_buffer.capacity * sizeof(tCrush_info), kMem_crush_data);
    gSplit_car_collision_infos = BrMemAllocate(gSplit_car_crush_info_buffer.capacity * sizeof(tPhysics_object), kMem_crush_data);
    for (i = 0; i < gSplit_car_crush_info_buffer.capacity; i++) {
        int j;
        tPhysics_shape* shape = NULL;

        gSplit_car_crush_info_buffer.crush_infos[i].field_0xdc = 0.f;
        gSplit_car_crush_info_buffer.crush_infos[i].collision_object = &gSplit_car_collision_infos[i];
        for (j = 0; j < 4; j++) {
            tPhysics_shape* polyhedron = (tPhysics_shape*)AllocateShapePolyhedron(16, kMem_crush_data);

            if (shape != NULL) {
                shape->common.next = polyhedron;
            } else {
                gSplit_car_collision_infos[i].shape = polyhedron;
            }
            shape = polyhedron;
        }
        if (shape != NULL) {
            shape->common.next = NULL;
        }
    }
    gSplit_car_driver = eDriver_split_car;
    gGonad_sphere_collision_shape = (tPhysics_shape*)AllocateShapeSphere(kMem_crush_data);
}

// ResetCrushSystems

// ReadCrushData

// CreateDefaultCrushDataCB

// CreateDefaultCrushData

// ReadMasterCrushData

// LoadCrushDataFile

// LinkCrushDataToActor

// LinkCrushDataToActors

// FUNCTION: CARMA2_HW 0x0042ac70
intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* pActor, void* pData) {
    tAccumulateSquashVertices_UserData* context = pData;
    tUser_crush_data* crush_data = pActor->user;

    if (crush_data != NULL && crush_data->crush_data != NULL) {
        int i;

        for (i = 0; i < REC2_ASIZE(crush_data->models); i++) {
            if (crush_data->models[i] != NULL) {
                context->count_vertices += crush_data->models[i]->nvertices;
            }
        }
    }
    return 0;
}

// InitModelMasterCrushData

// InitNetworkShapesStuff

// IncreasingCompare

// FUNCTION: CARMA2_HW 0x0042ace0
int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2) {
    if (*(const float*)pValue1 > *(const float*)pValue2) {
        return -1;
    } else if (*(const float*)pValue1 == *(const float*)pValue2) {
        return 0;
    } else {
        return 1;
    }
}

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

// FUNCTION: CARMA2_HW 0x0042c420
void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar) {
    int i;

    if (pCar_crush != NULL) {
        BrActorFree(pCar_crush->actor);
        if (pCar_crush->field_0x1cc != NULL) {
            BrMemFree(pCar_crush->field_0x1cc);
        }
        for (i = 0; i < pCar_crush->count_shapes; i++) {
            BrMemFree(pCar_crush->field_0x4[i].field_0x18);
            BrMemFree(pCar_crush->field_0x4[i].field_0x0);
            BrMemFree(pCar_crush->field_0x4[i].field_0x4);
        }
        BrMemFree(pCar_crush->field_0x4);
        pCar->collision_info->shape = pCar_crush->field_0xc;
        if (pCar_crush->network_stuff != NULL) {
            BrMemFree(pCar_crush->network_stuff);
        }
        for (i = 0; i < (int)REC2_ASIZE(pCar_crush->field_0x564); i++) {
            BrMemFree(pCar_crush->field_0x564[i]);
        }
        BrMemFree(pCar_crush);
    }
}

// FUNCTION: CARMA2_HW 0x0042c4f0
void C2_HOOK_FASTCALL DisposeCrushData(tCar_crush_buffer_entry* pCrush_data) {

    if (pCrush_data != NULL) {
        if (pCrush_data->flap_data != NULL) {
            BrMemFree(pCrush_data->flap_data);
        }
        if (pCrush_data->detach_data != NULL) {
            BrMemFree(pCrush_data->detach_data);
        }
        DisposeSmashData(pCrush_data);
        BrMemFree(pCrush_data);
    }
}

// FUNCTION: CARMA2_HW 0x0042c530
void C2_HOOK_FASTCALL DisposeVertexData(tModel_detail_vertex_data* pVertex_data) {

    if (pVertex_data != NULL) {
        BrMemFree(pVertex_data);
    }
}

// BuckleTheBugger

// OutsideEdgeLine

// PointTriangleDistSq

// PointRectDistSq

// PointSimplexDistSq

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0042c8b0
float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// LimitImpactScale

// GetImpactScale

// FUNCTION: CARMA2_HW 0x0042d0e0
int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq) {
    br_vector3 diagonal;
    float threshold;

    BrVector3Sub(&diagonal, &pModel->bounds.max, &pModel->bounds.min);
    threshold = BrVector3LengthSquared(&diagonal) + pMax_dist_sq;
    if (DistSq(&pModel->bounds.min, pCount, pVertices) > threshold
        || DistSq(&pModel->bounds.max, pCount, pVertices) > threshold) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00436890
int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector) {

    if ((float)fabs(pVector->v[0]) > 0.707107f) {
        return 0;
    } else if ((float)fabs(pVector->v[1]) > 0.707107f) {
        return 1;
    } else {
        return 2;
    }
}

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

// FUNCTION: CARMA2_HW 0x00436940
void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc) {
    int i;

    pDest->count = (tU8)pSrc->count;
    for (i = 0; i < pSrc->count; i++) {
        CompressVector3(&pDest->vectors[i], &pSrc->vectors[i], -2.5f, 2.5f);
    }
    pDest->field_0x1a = DRScalarToU16(pSrc->field_0x40, 0.0f, 10.0f);
    CompressVector3(&pDest->field_0x1c, &pSrc->field_0x44, -1.0f, 1.0f);
    pDest->field_0x24 = pSrc->field_0x50;
}

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

// FUNCTION: CARMA2_HW 0x0042d950
void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel) {
    int g;

    if (pModel == NULL) {
        return;
    }
    for (g = 0; g < pModel->prepared->ngroups; g++) {
        br_material* material = pModel->faces[*pModel->prepared->groups[g].face_user].material;

        if (material != NULL && !(material->flags & BR_MATF_TWO_SIDED)) {
            material->flags |= BR_MATF_TWO_SIDED;
            BrMaterialUpdate(material, BR_MATU_RENDERING);
        }
    }
}

// CheckFlapAngles

// FUNCTION: CARMA2_HW 0x0042d9b0
void C2_HOOK_FASTCALL MakeModelMaterialsSingleSided(br_model* pModel) {
    int g;

    if (pModel == NULL) {
        return;
    }
    for (g = 0; g < pModel->prepared->ngroups; g++) {
        br_material* mat = pModel->faces[*pModel->prepared->groups[g].face_user].material;

        if (mat != NULL && (mat->flags & BR_MATF_TWO_SIDED) && mat->identifier != NULL && mat->identifier[0] == 'S') {
            mat->flags &= ~BR_MATF_TWO_SIDED;
            BrMaterialUpdate(mat, BR_MATU_RENDERING);
        }
    }
}

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

// FUNCTION: CARMA2_HW 0x00434910
int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject) {
    br_vector3 world_direction;
    br_vector3 parent_direction;
    const br_vector3* limit_direction;
    br_scalar alignment;
    int i;

    for (i = 0; i < pObject->physics_joint1->count_limits; i++) {
        BrMatrix34ApplyV(&world_direction, &pObject->physics_joint1->limits[i].child, &pObject->actor->t.t.mat);
        BrMatrix34TApplyV(&parent_direction, &world_direction, &pObject->parent->actor->t.t.mat);
        limit_direction = &pObject->physics_joint1->limits[i].parent;
        alignment = BrVector3Dot(limit_direction, &parent_direction);
        if (alignment > 0.707107f) {
            return 1;
        }
    }
    return 0;
}

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

// FUNCTION: CARMA2_HW 0x00430730
intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg) {
    tUser_crush_data* user_crush = pActor->user;
    tCar_spec* car_spec = pArg;
    int i;

    if (user_crush != NULL) {
        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                MakeModelMaterialsDoubleSided(user_crush->models[i]);
            }
        }
    }
    return 0;
}

// SwapCarShapes

// IsAWheelOrPivotActor

// FUNCTION: CARMA2_HW 0x0042deb0
intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg) {
    tUser_crush_data* user_crush = pActor->user;
    tCar_spec* car_spec = pArg;
    int i;

    if (user_crush != NULL) {
        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                MakeModelMaterialsSingleSided(user_crush->models[i]);
            }
        }
    }
    return 0;
}

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

// FUNCTION: CARMA2_HW 0x00431590
void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    (void)pOn_screen;
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
    pModel->flags &= ~BR_MODF_CUSTOM;
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, BrOnScreenCheck(&pModel->bounds), 0);
}

// FUNCTION: CARMA2_HW 0x004315e0
void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel) {
    tUser_detail_level_model* user_detail_model;

    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        user_detail_model = pModel->user;
        user_detail_model->field_0x0 = 1;
    } else {
        pModel->custom = JitModelUpdate;
        pModel->flags |= BR_MODF_CUSTOM;
    }
}

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

// FUNCTION: CARMA2_HW 0x00439910
intptr_t C2_HOOK_CDECL EnableGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL && user_crush->groove != NULL) {
        PipeSingleGrooveOnOff(1, pActor, 0x3);
        EnableGroovidelic(pActor, 0x3);
        PipeSingleTransformType(pActor, BR_TRANSFORM_MATRIX34_LP);
        pActor->t.type = BR_TRANSFORM_MATRIX34_LP;
    }
    return 0;
}

// DoCompletelyUnBentThings

// TotallyRepairCarCollisionShapes

// TotallyRepairModel

// STUB: CARMA2_HW 0x00439a20
intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00439a10
int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data) {
    DRActorEnumRecurse(pCollision_info->actor, TotallyRepairModels, pUser_data);
    return 0;
}

// SendReAttachAllBits

// STUB: CARMA2_HW 0x00439510
void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

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

// FUNCTION: CARMA2_HW 0x0043ac90
// FUNCTION: CARMA2_SW 0x0043ace0
br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle) {
    if (pAngle < BR_ANGLE_DEG(180)) {
        return (br_scalar)(pAngle / (double)BR_ONE_LS);
    } else {
        return (br_scalar)((br_angle)-pAngle / (double)BR_ONE_LS);
    }
}

// CalculateAmountRepairedFraction

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00439c30
br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00439c10
void C2_HOOK_FASTCALL CompletelyUnBendCar(tCar_spec* pCar_spec) {
    br_scalar original_amount;

    UnBendCar(pCar_spec, 2000, &original_amount);
}

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

// FUNCTION: CARMA2_HW 0x0043bf30
void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3* pDest, br_model* pModel, br_vector3* pSrc) {
    tU16 i;

    for (i = 0; i < pModel->nvertices; i++) {
        CompressVector3(&pDest[i], &pSrc[i], -10.0f, 10.0f);
    }
}

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

// FUNCTION: CARMA2_HW 0x0043dfd0
void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes) {
    int i;

    if (pShapes == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id != 0) {
            pShapes->items[i].field_0x4 = 1;
        }
    }
}

