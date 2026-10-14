#include "13-crush1.h"

#include "08-loading1.h"
#include "49-physics.h"
#include "51-smash.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_string.h"

#include "brender/brender.h"

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

// AccumulateSquashVertices

// InitModelMasterCrushData

// InitNetworkShapesStuff

// IncreasingCompare

// DecreasingCompare

// FUNCTION: CARMA2_HW 0x0042b4c0
tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid) {
    tU16 number;
    int i;

    *pInvalid = 1;
    number = 0;
    for (i = 2; i >= 0; i--) {
        int j;
        int count;
        tU16 mi = 0;
        tU16 ma = 0;

        count = pCount_limits->counts[i][1] - 1;
        for (j = 0; j < count; j++) {
            if (!(pPoint->v[i] >= pLimits->limits[i][1].values[j])) {
                *pInvalid = 0;
                break;
            }
            ma += 1;
        }
        number = (tU16)((number << 2) | ma);

        count = pCount_limits->counts[i][0] - 1;
        for (j = 0; j < count; j++) {
            if (!(pLimits->limits[i][0].values[j] >= pPoint->v[i])) {
                *pInvalid = 0;
                break;
            }
            mi += 1;
        }
        number = (tU16)((number << 2) | mi);
    }
    return number;
}

// FUNCTION: CARMA2_HW 0x0042b590
intptr_t C2_HOOK_FASTCALL SoftnessOfNearestPointCB(br_actor* pActor, br_vector3* pPoint, void* pData) {
    tUser_crush_data* user_crush_data = pActor->user;
    tCar_crush_buffer_entry* crush_data;
    br_model* model;
    tSoftnessOfNearestPointCB_Context* context;
    const br_vector3* point;
    br_vector3 delta_point;
    float dist_bb_min;
    float dist_bb_max;
    float dist_bb_diagonal;
    float dist_p;
    int i;

    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    model = user_crush_data->models[0];
    if (model == NULL) {
        return 0;
    }
    context = pData;
    point = context->point;
    BrVector3Sub(&delta_point, point, pPoint);
    dist_bb_diagonal = Vector3DistanceSquared(&model->bounds.max, &model->bounds.min);
    dist_bb_min = Vector3DistanceSquared(&delta_point, &model->bounds.min);
    dist_bb_max = Vector3DistanceSquared(&delta_point, &model->bounds.max);
    dist_p = dist_bb_min > dist_bb_max ? dist_bb_min : dist_bb_max;
    if (!((double)dist_p - dist_bb_diagonal > context->distance)) {
        for (i = 0; i < model->nvertices; i++) {
            float d;

            d = Vector3DistanceSquared(&model->vertices[i].p, &delta_point);
            if (!(d >= context->distance)) {
                context->distance = d;
                context->softness_factor = crush_data->softness_factor;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042b760
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* cbfn, void* pContext) {
    br_vector3 delta;
    intptr_t result;
    br_actor* a;

    if (pDelta == NULL) {
        BrVector3Set(&delta, 0.0f, 0.0f, 0.0f);
    } else {
        delta.v[0] = pActor->t.t.mat.m[3][0] + pDelta->v[0];
        delta.v[1] = pActor->t.t.mat.m[3][1] + pDelta->v[1];
        delta.v[2] = pActor->t.t.mat.m[3][2] + pDelta->v[2];
    }
    result = cbfn(pActor, &delta, pContext);
    if (result != 0) {
        return result;
    }
    for (a = pActor->children; a != NULL; a = a->next) {
        result = DRActorEnumRecurseWithTranslation(a, &delta, cbfn, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

float C2_HOOK_FASTCALL SoftnessOfNearestPoint(tCar_spec* pCar_spec, br_vector3* pPoint) {
    tSoftnessOfNearestPointCB_Context softcb_data;

    softcb_data.point = pPoint;
    softcb_data.distance = BR_SCALAR_MAX;
    softcb_data.softness_factor = 1.0f;
    DRActorEnumRecurseWithTranslation(pCar_spec->car_model_actor, NULL, SoftnessOfNearestPointCB, &softcb_data);
    return softcb_data.softness_factor;
}

// SetUpShapeLimitingStuff

// CopyShapePolyhedron

// InitShapeStuff

// InitPhysMasterCrushData

// CheckHingePointOrder

// FUNCTION: CARMA2_HW 0x0042bb00
tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel) {
    tU16 closest_vertex = 0;
    br_scalar closest_dist = BR_SCALAR_MAX;
    tU16 count;
    const br_vertex* vertices;
    tU16 i;
    br_scalar d;

    count = pModel->nvertices;
    if (count == 0) {
        return closest_vertex;
    }
    vertices = pModel->vertices;
    for (i = 0; i < count; i++) {
        d = Vector3DistanceSquared(pPoint, &vertices[i].p);
        if (!(d >= closest_dist)) {
            closest_dist = d;
            closest_vertex = i;
        }
    }
    return closest_vertex;
}

float C2_HOOK_FASTCALL DistanceToBounds(const br_vector3* pPoint, const br_bounds3* pBounds) {
    float closest_distance = BR_SCALAR_MAX;
    double ds;
    int i;

    /* Find the nearest of the six coordinate planes. */
    for (i = 0; i < 3; i++) {
        ds = fabs((double)pPoint->v[i] - pBounds->min.v[i]);
        if (!(ds >= closest_distance)) {
            closest_distance = (float)ds;
        }
        ds = fabs((double)pPoint->v[i] - pBounds->max.v[i]);
        if (!(ds >= closest_distance)) {
            closest_distance = (float)ds;
        }
    }
    return closest_distance;
}

// FindNearestParentVertex

// FUNCTION: CARMA2_HW 0x0042bb90
void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds) {
    const br_vector3* model_bounds[2];
    int closest_corner_index = 0;

    model_bounds[0] = &pModel->bounds.min;
    model_bounds[1] = &pModel->bounds.max;
    if (pDetach_data->type == eDetachType_joint_index) {
        float closest_corner_distance = BR_SCALAR_MAX;
        br_vector3 vp;
        int i;

        BrVector3Copy(&vp, &pModel->vertices[pDetach_data->field_0x30].p);
        for (i = 0; i < 8; i++) {
            br_vector3 corner;
            float dist_bb;

            BrVector3Set(&corner,
                model_bounds[i & 1]->v[0],
                model_bounds[(i >> 1) & 1]->v[1],
                model_bounds[(i >> 2) & 1]->v[2]);
            dist_bb = Vector3DistanceSquared(&corner, &vp);
            if (!(dist_bb >= closest_corner_distance)) {
                closest_corner_distance = dist_bb;
                closest_corner_index = i;
            }
        }
    } else {
        float closest_distance = BR_SCALAR_MAX;
        br_vector3 closest_corner;
        int i;

        BrVector3Copy(&closest_corner, &pModel->bounds.min);
        for (i = 0; i < 8; i++) {
            br_vector3 corner;
            float dist_closest_side;

            BrVector3Set(&corner,
                model_bounds[i & 1]->v[0],
                model_bounds[(i >> 1) & 1]->v[1],
                model_bounds[(i >> 2) & 1]->v[2]);
            dist_closest_side = DistanceToBounds(&corner, pBounds);
            if (!(dist_closest_side >= closest_distance)) {
                closest_distance = dist_closest_side;
                BrVector3Copy(&closest_corner, &corner);
                closest_corner_index = i;
            }
        }
        pDetach_data->field_0x30 = FindNearestVertex(&closest_corner, pModel);
    }
    pDetach_data->field_0x32 = (tU8)closest_corner_index;
    {
        br_vector3 center;
        const br_vector3* bounds_points[2];
        float closest = BR_SCALAR_MAX;
        int closest_side = 0;
        double d;
        int i;

        bounds_points[0] = &pBounds->min;
        bounds_points[1] = &pBounds->max;
        Vector3Average(&center, &pModel->bounds.min, &pModel->bounds.max);
        for (i = 0; i < 6; i++) {
            if (i != 2) {
                d = fabs((double)center.v[i / 2] - bounds_points[i & 1]->v[i / 2]);
                if (!(d >= closest)) {
                    closest = (float)d;
                    closest_side = i;
                }
            }
        }
        pDetach_data->field_0x33 = (tU8)closest_side;
    }
}

// InitPhysCrushDataCB

// InitPhysCrushData

// CalculateReferencePoints

// InitModelCrushDataCB

// InitModelCrushData

// FUNCTION: CARMA2_HW 0x0042c0e0
void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data* pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data) {
    const br_vector3 *p0, *p1, *p2;
    br_vector3 hinge01;
    br_vector3 hinge02;
    br_vector3 hinge_normal;
    br_scalar hinge_dot;
    br_scalar distance;
    br_scalar plane;
    int i;

    p0 = &pModel->vertices[pFlap_data->hinge0].p;
    p1 = &pModel->vertices[pFlap_data->hinge1].p;
    p2 = &pModel->vertices[pFlap_data->hinge2].p;
    BrVector3Sub(&hinge02, p2, p0);
    BrVector3Sub(&hinge01, p1, p0);
    BrVector3Cross(&hinge_normal, &hinge01, &hinge02);
    BrVector3Normalise(&hinge_normal, &hinge_normal);
    hinge_dot = BrVector3Dot(&hinge_normal, p0);

    for (i = 0; i < pModel->nvertices; i++) {
        distance = PointEdgeDistSq(&pModel->vertices[i].p, &pModel->vertices[pFlap_data->hinge0].p, &pModel->vertices[pFlap_data->hinge1].p);
        plane = BrVector3Dot(&hinge_normal, &pModel->vertices[i].p) - hinge_dot;
        if (distance > 0.00021003991f && plane > -0.0014492754f) {
            pVertex_data[i].flags |= 0x2;
        } else {
            pVertex_data[i].flags &= ~0x2;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042c300
float C2_HOOK_FASTCALL PointEdgeDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB) {
    br_vector3 pAB;
    br_vector3 pAP;
    br_vector3 pProj;
    float proj;

    BrVector3Sub(&pAB, pB, pA);
    BrVector3Sub(&pAP, pP, pA);
    proj = BrVector3Dot(&pAP, &pAB) / BrVector3Dot(&pAB, &pAB);
    if (!(proj >= 0.0f)) {
        proj = 0.0f;
    } else if (proj > 1.0f) {
        proj = 1.0f;
    }
    BrVector3Scale(&pProj, &pAB, proj);
    BrVector3Add(&pProj, &pProj, pA);
    BrVector3Sub(&pProj, pP, &pProj);
    return BrVector3LengthSquared(&pProj);
}

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

// CompressCrush

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

// MakeModelMaterialsDoubleSided

// CheckFlapAngles

// MakeModelMaterialsSingleSided

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

// MakeCarModelsMaterialsDoubleSided

// SwapCarShapes

// IsAWheelOrPivotActor

// MakeCarModelsMaterialsSingleSided

// FUNCTION: CARMA2_HW 0x0042df70
int C2_HOOK_FASTCALL IsOnNonCarHalf(tCar_spec* pCar, const br_vector3* pPos) {
    tCar_crush_spec* crush;
    double distance;

    crush = pCar->car_crush_spec;
    if (crush == NULL || crush->field_0x4b8 == 0) {
        return 0;
    }
    distance = (double)crush->field_0x548.v[1] * pPos->v[1] + (double)pPos->v[2] * crush->field_0x548.v[2];
    distance += (double)crush->field_0x548.v[0] * pPos->v[0];
    distance -= crush->field_0x554;
    return !(distance >= 0.0f);
}

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

// FUNCTION: CARMA2_HW 0x00439fd0
void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape) {
    const volatile tCar_crush_shape_info* live = pShape;
    tCar_crush_reordered_shape_info* points;
    tU32 bits;
    int count = pShape->original_count_points;
    int i;

    pShape->count_points = count;
    if (count <= 0) {
        return;
    }
    i = 0;
    do {
        /* Original reloads the array pointer after each raw coordinate store. */
        points = live->field_0x18;
        memcpy(&bits, &points[i].field_0x2c.v[0], sizeof(bits));
        memcpy(&points[i].field_0x18.v[0], &bits, sizeof(bits));
        points = live->field_0x18;
        memcpy(&bits, &points[i].field_0x2c.v[1], sizeof(bits));
        memcpy(&points[i].field_0x18.v[1], &bits, sizeof(bits));
        points = live->field_0x18;
        memcpy(&bits, &points[i].field_0x2c.v[2], sizeof(bits));
        memcpy(&points[i].field_0x18.v[2], &bits, sizeof(bits));
        i++;
    } while (i < live->count_points);
}

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

