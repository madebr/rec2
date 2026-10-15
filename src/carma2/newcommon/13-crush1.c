#include "13-crush1.h"

#include "00-car.h"
#include "08-loading1.h"
#include "09-funks.h"
#include "23-piping.h"
#include "41-utility.h"
#include "49-physics.h"
#include "51-smash.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_stdlib.h"
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

// FUNCTION: CARMA2_HW 0x0042ac70
intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* actor, void* pData) {
    tAccumulateSquashVertices_UserData* context = pData;
    tUser_crush_data* crush_data = actor->user;

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

void C2_HOOK_FASTCALL InitModelMasterCrushData(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush = pCar_spec->car_crush_spec;

    if (car_crush != NULL) {
        tAccumulateSquashVertices_UserData data;
        tCar_crush_vertex_data* vertices;

        data.count_vertices = 0;
        data.bens_z_min = car_crush->bend_z_min;
        data.bens_z_max = car_crush->bend_z_max;
        DRActorEnumRecurse(pCar_spec->car_model_actor, AccumulateSquashVertices, &data);
        vertices = BrMemAllocate(data.count_vertices * sizeof(tCar_crush_vertex_data), kMem_crush_data);
        car_crush->field_0x1cc = vertices;
        BrMatrix34Copy(&car_crush->model_actor->t.t.mat, &pCar_spec->car_model_actor->t.t.mat);
    }
}

// InitNetworkShapesStuff

// FUNCTION: CARMA2_HW 0x0042acb0
int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2) {
    float v1 = *(const float*)pValue1;
    float v2 = *(const float*)pValue2;

    if (!(v1 >= v2)) {
        return -1;
    } else if (v1 == v2) {
        return 0;
    } else {
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x0042ace0
int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2) {
    float v1 = *(const float*)pValue1;
    float v2 = *(const float*)pValue2;

    if (v1 > v2) {
        return -1;
    } else if (!(v1 < v2)) {
        return 0;
    } else {
        return 1;
    }
}

// InitPhysMasterCrushData

// FUNCTION: CARMA2_HW 0x0042b0e0
void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec) {
    br_bounds3 wheel_all_bounds;
    const double inverse_world_scale = 1.0 / 6.9;
    int i;

    BrVector3Copy(&wheel_all_bounds.min, &pCar_spec->wpos[0]);
    BrVector3Copy(&wheel_all_bounds.max, &wheel_all_bounds.min);
    for (i = 1; i < 4; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (!(pCar_spec->wpos[i].v[j] >= wheel_all_bounds.min.v[j])) {
                wheel_all_bounds.min.v[j] = pCar_spec->wpos[i].v[j];
            } else if (pCar_spec->wpos[i].v[j] > wheel_all_bounds.max.v[j]) {
                wheel_all_bounds.max.v[j] = pCar_spec->wpos[i].v[j];
            }
        }
    }
    for (i = 0; i < 3; i++) {
        wheel_all_bounds.min.v[i] = (float)(wheel_all_bounds.min.v[i] * inverse_world_scale);
        wheel_all_bounds.max.v[i] = (float)(wheel_all_bounds.max.v[i] * inverse_world_scale);
    }
    for (i = 0; i < (int)REC2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_actor* wheel_actor = pCar_spec->wheel_actors[i];
        tUser_crush_data* user_crush_data;
        br_model* wheel_model;
        br_matrix34 wheel_to_car;
        br_bounds3 wheel_bounds;
        int j;

        if (wheel_actor == NULL) {
            continue;
        }
        user_crush_data = wheel_actor->user;
        if (user_crush_data == NULL) {
            continue;
        }
        wheel_model = user_crush_data->models[0];
        if (wheel_model == NULL) {
            continue;
        }
        BrActorToActorMatrix34(&wheel_to_car, wheel_actor, pCar_spec->car_model_actor);
        for (j = 0; j < 3; j++) {
            wheel_bounds.min.v[j] = wheel_model->bounds.min.v[j] + wheel_to_car.m[3][j];
            wheel_bounds.max.v[j] = wheel_model->bounds.max.v[j] + wheel_to_car.m[3][j];
        }
        for (j = 0; j < 3; j++) {
            if (!(wheel_bounds.min.v[j] >= wheel_all_bounds.min.v[j])) {
                wheel_all_bounds.min.v[j] = wheel_bounds.min.v[j];
            } else if (wheel_bounds.max.v[j] > wheel_all_bounds.max.v[j]) {
                wheel_all_bounds.max.v[j] = wheel_bounds.max.v[j];
            }
        }
    }
    for (i = 0; i < 3; i++) {
        int j;

        for (j = 0; j < pCar_crush->count_limits.counts[i][0]; j++) {
            pCar_crush->field_0xbc.limits[i][0].values[j] = pCar_crush->limits.limits[i][0].values[j];
        }
        pCar_crush->field_0xbc.limits[i][0].values[j] = wheel_all_bounds.max.v[i];
        pCar_crush->field_0xa4.counts[i][0] = j + 1;
        for (j = 0; j < pCar_crush->count_limits.counts[i][1]; j++) {
            pCar_crush->field_0xbc.limits[i][1].values[j] = pCar_crush->limits.limits[i][1].values[j];
        }
        pCar_crush->field_0xbc.limits[i][1].values[j] = wheel_all_bounds.min.v[i];
        pCar_crush->field_0xa4.counts[i][1] = j + 1;
    }
    for (i = 0; i < 3; i++) {
        qsort(pCar_crush->field_0xbc.limits[i][0].values, pCar_crush->field_0xa4.counts[i][0], sizeof(float), DecreasingCompare);
        qsort(pCar_crush->field_0xbc.limits[i][1].values, pCar_crush->field_0xa4.counts[i][1], sizeof(float), IncreasingCompare);
    }
    for (i = 0; i < pCar_crush->count_shapes; i++) {
        int j;
        tCar_crush_shape_info* shape_info = &pCar_crush->field_0x4[i];

        for (j = 0; j < shape_info->count_points; j++) {
            int failed;
            tU16 number;
            float softness;

            number = CrushLimitNumber(&shape_info->field_0x18[j].field_0x0, &pCar_crush->field_0xbc, &pCar_crush->field_0xa4, &failed);
            shape_info->field_0x18[j].field_0x28 = number;
            if (failed == 0) {
                softness = SoftnessOfNearestPoint(pCar_spec, &shape_info->field_0x18[j].field_0x0);
                shape_info->field_0x18[j].field_0x24 = softness;
            } else {
                shape_info->field_0x18[j].field_0x24 = 0.0f;
            }
        }
    }
}

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

// CopyShapePolyhedron

// InitShapeStuff

// FUNCTION: CARMA2_HW 0x0042b7f0
intptr_t C2_HOOK_CDECL InitPhysCrushDataCB(br_actor* actor, void* data) {
    tInitPhysCrushDataCB_Data* context = data;
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    br_model* model;
    tCar_crush_flap_data* flap_data;
    tCar_crush_detach_data* detach_data;
    double volume;

    user_crush_data = actor->user;
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
    volume = ((double)model->bounds.max.v[0] - model->bounds.min.v[0])
           * ((double)model->bounds.max.v[1] - model->bounds.min.v[1])
           * ((double)model->bounds.max.v[2] - model->bounds.min.v[2])
           * context->value;
    crush_data->field_0x24 = (float)volume;
    if (!(volume >= 0.01f)) {
        crush_data->field_0x24 = 0.01f;
    }
    flap_data = crush_data->flap_data;
    if (flap_data != NULL) {
        br_vector3 tv;
        const br_vertex* vertices;
        tCar_crush_flap_data* current_flap;
        tU16 parent_vertex;

        if ((flap_data->kev_o_flap & 0x2) == 0) {
            CheckHingePointOrder(flap_data, model, &context->car->collision_info->cmpos);
        }
        parent_vertex = FindNearestParentVertex(actor, &model->vertices[crush_data->flap_data->hinge0].p);
        crush_data->flap_data->field_0xc = parent_vertex;
        parent_vertex = FindNearestParentVertex(actor, &model->vertices[crush_data->flap_data->hinge2].p);
        crush_data->flap_data->field_0x12 = parent_vertex;
        vertices = model->vertices;
        BrVector3Sub(&tv, &vertices[flap_data->hinge2].p, &vertices[flap_data->hinge1].p);
        current_flap = crush_data->flap_data;
        current_flap->field_0x1c = BrVector3Length(&tv) * 0.5f;
    }
    detach_data = crush_data->detach_data;
    if (detach_data != NULL) {
        detach_data->field_0x2c = context->car;
        SetUpSemiDetachJointStuff(detach_data, model, &context->car->collision_info->bb1);
    }
    return 0;
}

void C2_HOOK_FASTCALL CheckHingePointOrder(tCar_crush_flap_data* pHinge, br_model* pModel, br_vector3* pPos) {
    const br_vertex* vertices = pModel->vertices;
    br_vector3 hinge01;
    br_vector3 hinge02;
    br_vector3 normal;
    br_vector3 center_offset;

    BrVector3Sub(&hinge01, &vertices[pHinge->hinge1].p, &vertices[pHinge->hinge0].p);
    BrVector3Sub(&hinge02, &vertices[pHinge->hinge2].p, &vertices[pHinge->hinge0].p);
    BrVector3Cross(&normal, &hinge02, &hinge01);
    Vector3Average(&center_offset, &pModel->bounds.min, &pModel->bounds.max);
    BrVector3Sub(&center_offset, &center_offset, pPos);
    if (BrVector3Dot(&center_offset, &normal) > 0.0f) {
        tU16 hinge1 = pHinge->hinge1;

        pHinge->hinge1 = pHinge->hinge0;
        pHinge->hinge0 = hinge1;
    }
}

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

tU16 C2_HOOK_FASTCALL FindNearestParentVertex(br_actor* pActor, br_vector3* pPos) {
    br_actor* parent = pActor->parent;
    tUser_crush_data* parent_user;
    br_model* parent_model = NULL;
    br_vector3 point;

    if (parent != NULL) {
        parent_user = parent->user;
        if (parent_user != NULL) {
            parent_model = parent_user->models[0];
        }
    }
    if (parent_model == NULL) {
        PDFatalError("I can't cope flapping bit doesn't have a valid parent");
    }
    BrMatrix34ApplyP(&point, pPos, &pActor->t.t.mat);
    return FindNearestVertex(&point, parent_model);
}

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

void C2_HOOK_FASTCALL InitPhysCrushData(tCar_spec* pCar_spec) {
    tPhysics_object* collision_info = pCar_spec->collision_info;
    tInitPhysCrushDataCB_Data data;

    data.car = pCar_spec;
    data.value = collision_info->M /
                 ((collision_info->bb2.max.v[0] - collision_info->bb2.min.v[0]) *
                  (collision_info->bb2.max.v[1] - collision_info->bb2.min.v[1]) *
                  (collision_info->bb2.max.v[2] - collision_info->bb2.min.v[2]));
    DRActorEnumRecurse(pCar_spec->car_model_actor, InitPhysCrushDataCB, &data);
}

void C2_HOOK_FASTCALL CalculateReferencePoints(br_model* pModel, br_model* pParent_model, tCar_crush_detach_data* pDetach_data, br_actor* pActor) {
    int i;
    float d_min, d_max;
    br_vector3 min_v, max_v;

    if (pModel == NULL && pParent_model == NULL) {
        return;
    }
    d_max = d_min = BR_SCALAR_MAX;
    BrMatrix34ApplyP(&min_v, &pModel->bounds.min, &pActor->t.t.mat);
    BrMatrix34ApplyP(&max_v, &pModel->bounds.max, &pActor->t.t.mat);
    for (i = 0; i < pParent_model->nvertices; i++) {
        float d;

        d = Vector3DistanceSquared(&min_v, &pParent_model->vertices[i].p);
        if (!(d >= d_min)) {
            d_min = d;
            pDetach_data->min_vertex = (tU16)i;
        }
        d = Vector3DistanceSquared(&max_v, &pParent_model->vertices[i].p);
        if (!(d >= d_max)) {
            d_max = d;
            pDetach_data->max_vertex = (tU16)i;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042be20
intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* actor, void* data) {
    tUser_crush_data* user_crush_data;
    tUser_crush_data* parent_user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    br_model* model;
    br_model* parent_model;

    (void)data;
    user_crush_data = actor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    model = user_crush_data->models[0];
    if (model == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    detach_data = crush_data->detach_data;
    if (detach_data == NULL) {
        return 0;
    }
    parent_user_crush_data = actor->parent->user;
    parent_model = parent_user_crush_data->models[0];
    CalculateReferencePoints(model, parent_model, detach_data, actor);
    return 0;
}

void C2_HOOK_FASTCALL InitModelCrushData(tCar_spec* pCar_spec) {
    DRActorEnumRecurse(pCar_spec->car_model_actor, InitModelCrushDataCB, pCar_spec);
}

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

void C2_HOOK_FASTCALL InitModelVertexData(tModel_detail_vertex_data* pVertex_data, br_model* pModel, int pDetail_level, tCar_crush_buffer_entry* pCrush_data, tCar_crush_spec* pCar_crush_spec) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {
        int invalid;
        tU16 number;

        CompressVector3(&pVertex_data[i].p, &pModel->vertices[i].p, -10.0f, 10.0f);
        number = CrushLimitNumber(&pModel->vertices[i].p, &pCar_crush_spec->limits, &pCar_crush_spec->count_limits, &invalid);
        pVertex_data[i].limit_number = (tS16)number;
    }
    if (pDetail_level == 0 && pCrush_data != NULL && pCrush_data->flap_data != NULL) {
        SetFlapCheckVertices(pCrush_data->flap_data, pModel, pVertex_data);
    }
}

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

// FUNCTION: CARMA2_HW 0x0042c8b0
float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices) {
    switch (pCount) {
    case 0:
        return 0.0f;
    case 1:
        return Vector3DistanceSquared(pP, &pVertices[0]);
    case 2:
        return PointEdgeDistSq(pP, &pVertices[0], &pVertices[1]);
    case 3:
        return PointTriangleDistSq(pP, &pVertices[0], &pVertices[1], &pVertices[2]);
    case 4:
        return PointRectDistSq(pP, &pVertices[0], &pVertices[1], &pVertices[2]);
    default:
        return PointSimplexDistSq(pP, pCount, pVertices);
    }
}

// FUNCTION: CARMA2_HW 0x0042ca30
float C2_HOOK_FASTCALL PointTriangleDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
    br_vector3 ab;
    br_vector3 bc;
    br_vector3 ca;
    br_vector3 normal;
    br_vector3 offset;
    br_vector3 projected;
    float distance;

    BrVector3Sub(&ab, pB, pA);
    BrVector3Sub(&bc, pC, pB);
    BrVector3Sub(&ca, pA, pC);
    BrVector3Cross(&normal, &ab, &bc);
    BrVector3Normalise(&normal, &normal);
    BrVector3Sub(&offset, pP, pA);
    distance = BrVector3Dot(&offset, &normal);
    BrVector3Scale(&projected, &normal, distance);
    BrVector3Sub(&projected, pP, &projected);
    if (OutsideEdgeLine(&projected, pA, &ab, pC)) {
        return PointEdgeDistSq(pP, pA, pB);
    }
    if (OutsideEdgeLine(&projected, pB, &bc, pA)) {
        return PointEdgeDistSq(pP, pB, pC);
    }
    if (OutsideEdgeLine(&projected, pA, &ca, pB)) {
        return PointEdgeDistSq(pP, pA, pC);
    }
    return distance * distance;
}

// FUNCTION: CARMA2_HW 0x0042cc40
int C2_HOOK_FASTCALL OutsideEdgeLine(const br_vector3* pPoint, const br_vector3* pA, const br_vector3* pDirection, const br_vector3* pInside) {
    br_vector3 direction;
    br_vector3 offset;
    br_vector3 inside_offset;

    BrVector3Normalise(&direction, pDirection);
    BrVector3Sub(&offset, pPoint, pA);
    /* Each new component changes the dot product used for the next one. */
    direction.v[0] *= BrVector3Dot(&offset, &direction);
    direction.v[1] *= BrVector3Dot(&offset, &direction);
    direction.v[2] *= BrVector3Dot(&offset, &direction);
    BrVector3Sub(&offset, &offset, &direction);
    BrVector3Sub(&inside_offset, pInside, pA);
    return !(BrVector3Dot(&offset, &inside_offset) >= 0.0f);
}

// FUNCTION: CARMA2_HW 0x0042ce00
float C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
    const br_vector3* origin = pA;
    br_vector3 u;
    br_vector3 v;
    br_vector3 offset;
    br_vector3 projected;
    br_vector3 residual;
    float u_scale;
    float v_scale;

    BrVector3Sub(&u, pC, pA);
    BrVector3Sub(&v, pB, pA);
    if (fabs(BrVector3Dot(&u, &v)) > 0.001f) {
        origin = pB;
        BrVector3Sub(&u, pC, pB);
        BrVector3Sub(&v, pA, pB);
        if (fabs(BrVector3Dot(&u, &v)) > 0.001f) {
            origin = pC;
            BrVector3Sub(&u, pB, pC);
            BrVector3Sub(&v, pA, pC);
        }
    }
    BrVector3Sub(&offset, pP, origin);
    v_scale = BrVector3Dot(&offset, &v) / BrVector3LengthSquared(&v);
    u_scale = BrVector3Dot(&offset, &u) / BrVector3LengthSquared(&u);
    if (!(v_scale >= 0.0f)) {
        v_scale = 0.0f;
    } else if (v_scale > 1.0f) {
        v_scale = 1.0f;
    }
    if (!(u_scale >= 0.0f)) {
        u_scale = 0.0f;
    } else if (u_scale > 1.0f) {
        u_scale = 1.0f;
    }
    projected.v[0] = u_scale * u.v[0] + v_scale * v.v[0] + origin->v[0];
    projected.v[1] = u_scale * u.v[1] + v_scale * v.v[1] + origin->v[1];
    projected.v[2] = u_scale * u.v[2] + v_scale * v.v[2] + origin->v[2];
    BrVector3Sub(&residual, &projected, pP);
    return BrVector3LengthSquared(&residual);
}

float C2_HOOK_FASTCALL PointSimplexDistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices) {
    float closest_distance = BR_SCALAR_MAX;
    int i;

    for (i = 0; i < pCount; i++) {
        int j;

        for (j = i + 1; j < pCount; j++) {
            int k;

            for (k = j + 1; k < pCount; k++) {
                float distance = PointTriangleDistSq(pP, &pVertices[i], &pVertices[j], &pVertices[k]);

                if (!(distance >= closest_distance)) {
                    closest_distance = distance;
                }
            }
        }
    }
    return closest_distance;
}

// LimitImpactScale

// GetImpactScale

// FUNCTION: CARMA2_HW 0x0042d0e0
int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq) {
    br_vector3 diagonal;
    float threshold;

    BrVector3Sub(&diagonal, &pModel->bounds.max, &pModel->bounds.min);
    threshold = BrVector3LengthSquared(&diagonal) + pMax_dist_sq;
    if (DistSq(&pModel->bounds.min, pCount, pVertices) > threshold) {
        return 1;
    }
    return DistSq(&pModel->bounds.max, pCount, pVertices) > threshold;
}

int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector) {
    if (fabs(pVector->v[0]) > 0.707107f) {
        return 0;
    }
    if (fabs(pVector->v[1]) > 0.707107f) {
        return 1;
    }
    return 2;
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

// FUNCTION: CARMA2_HW 0x004369c0
void C2_HOOK_FASTCALL ExpandCrush(tCar_crush* pDest, const tCompressed_car_crush* pSrc) {
    tU8 i;
    int count;

    pDest->count = pSrc->count;
    ExpandVector3(&pDest->vectors[0], &pSrc->vectors[0], -2.5f, 2.5f);
    BrVector3Copy(&pDest->field_0x34, &pDest->vectors[0]);
    for (i = 1; i < pSrc->count; i++) {
        ExpandVector3(&pDest->vectors[i], &pSrc->vectors[i], -2.5f, 2.5f);
        BrVector3Accumulate(&pDest->field_0x34, &pDest->vectors[i]);
    }
    count = pDest->count;
    pDest->field_0x34.v[0] /= (float)count;
    pDest->field_0x34.v[1] /= (float)count;
    pDest->field_0x34.v[2] /= (float)count;
    pDest->field_0x40 = DRU16ToScalar(pSrc->field_0x1a, 0.0f, 10.0f);
    ExpandVector3(&pDest->field_0x44, &pSrc->field_0x1c, -1.0f, 1.0f);
    pDest->field_0x50 = pSrc->field_0x24;
}

void C2_HOOK_FASTCALL RemoveDuplicatePoints(int* pCount_out, br_vector3* pPoints_out, int pCount_in, const br_vector3* pPoints_in) {
    int i;

    BrVector3Copy(&pPoints_out[0], &pPoints_in[0]);
    *pCount_out = 1;
    for (i = 1; i < pCount_in; i++) {
        int j;
        int duplicate = 0;

        for (j = 0; j < *pCount_out; j++) {
            if (!(fabs(pPoints_out[j].v[0] - pPoints_in[i].v[0]) >= 0.005f)
                && !(fabs(pPoints_out[j].v[1] - pPoints_in[i].v[1]) >= 0.005f)
                && !(fabs(pPoints_out[j].v[2] - pPoints_in[i].v[2]) >= 0.005f)) {
                duplicate = 1;
                break;
            }
        }
        if (!duplicate) {
            BrVector3Copy(&pPoints_out[*pCount_out], &pPoints_in[i]);
            *pCount_out += 1;
        }
    }
}

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

int C2_HOOK_FASTCALL PointIsOutsideLimits(tCar_crush_shape_info* pShape, int pPoint_index, tCar_crush_limits* pLimits) {
    int i;
    tU16 flags = pShape->field_0x18[pPoint_index].field_0x28;

    for (i = 0; i < 3; i++) {
        br_scalar value = pShape->field_0x18[pPoint_index].field_0x18.v[i];

        if (!(value >= pLimits->limits[i][0].values[flags & 0x3])) {
            return 1;
        }
        if (value > pLimits->limits[i][1].values[(flags >> 2) & 0x3]) {
            return 1;
        }
        flags >>= 4;
    }
    return 0;
}

// ClipPointToPolyhedron

// QuantizeVector3

// MoveCarToSensiblePlace

// SwapShapesIfPossible

// DoShapeSwapping

// BendPoint

int C2_HOOK_FASTCALL EdgeCrossesZPlane(br_scalar pZ, const br_vector3* pPoint1, const br_vector3* pPoint2) {
    if (pPoint1->v[2] >= pZ) {
        return !(pPoint2->v[2] >= pZ);
    }
    return pPoint2->v[2] > pZ;
}

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
    int on_screen;

    (void)pOn_screen;
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
    pModel->flags &= ~BR_MODF_CUSTOM;
    on_screen = (int)BrOnScreenCheck(&pModel->bounds);
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, on_screen, 0);
}

// FUNCTION: CARMA2_HW 0x004315e0
void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel) {
    if ((pModel->flags & BR_MODF_CUSTOM) && pModel->user != NULL) {
        tU32 pending = 1;

        /* Existing custom models mark their metadata as dirty. */
        memcpy(pModel->user, &pending, sizeof(pending));
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

// FUNCTION: CARMA2_HW 0x00439910
intptr_t C2_HOOK_CDECL EnableGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush = pActor->user;

    (void)pData;
    if (user_crush != NULL && user_crush->groove != NULL) {
        PipeSingleGrooveOnOff(1, pActor, 3);
        EnableGroovidelic(pActor, 3);
        PipeSingleTransformType(pActor, BR_TRANSFORM_MATRIX34_LP);
        pActor->t.type = BR_TRANSFORM_MATRIX34_LP;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00439960
void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {
        tCar_crush_shape_info* shape_info = &pCar_spec->car_crush_spec->field_0x4[i];
        int j;

        for (j = 0; j < shape_info->count_points; j++) {
            tCar_crush_reordered_shape_info* point = &shape_info->field_0x18[j];

            memcpy(&point->field_0x18, &point->field_0x0, sizeof(point->field_0x18));
            if (DRVector3TestForNan(&shape_info->field_0x18[j].field_0x18)) {
                PDEnterDebugger("NaN");
            }
        }
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}

// FUNCTION: CARMA2_HW 0x00439fd0
void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape) {
    int i;

    pShape->count_points = pShape->original_count_points;
    for (i = 0; i < pShape->count_points; i++) {
        tCar_crush_reordered_shape_info* point = &pShape->field_0x18[i];

        memcpy(&point->field_0x18, &point->field_0x2c, sizeof(point->field_0x18));
    }
}

void C2_HOOK_FASTCALL CompletelyUnBendCollisionShapes(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {
        CompletelyUnBendCollisionShape(&pCar_spec->car_crush_spec->field_0x4[i]);
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}

// FUNCTION: CARMA2_HW 0x0043a010
void C2_HOOK_FASTCALL CompletelyUnBendWheels(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_actor* actor = pCar_spec->pivot_actors[i];
        tGroovidelic_spec* groove;

        if (actor == NULL) {
            actor = pCar_spec->wheel_actors[i];
        }
        if (actor == NULL) {
            continue;
        }
        groove = ActorsGroove(actor);
        if (groove != NULL && groove->path_type == eGroove_path_straight) {
            br_vector3* centre = &groove->path_data.straight_info.centre;
            br_vector3 delta;

            delta.v[0] = -centre->v[0];
            delta.v[1] = -centre->v[1];
            delta.v[2] = -centre->v[2];
            memcpy(centre, &pCar_spec->car_crush_spec->field_0x1ec[i], sizeof(*centre));
            delta.v[0] += centre->v[0];
            delta.v[1] += centre->v[1];
            delta.v[2] += centre->v[2];
            PipeSingleVector3(centre, &delta);
        }
    }

    for (i = 0; i < (int)REC2_ASIZE(pCar_spec->wpos); i++) {
        memcpy(&pCar_spec->wpos[i], &pCar_spec->car_crush_spec->field_0x234[i], sizeof(pCar_spec->wpos[i]));
    }
    SetCarSuspGiveAndHeight(pCar_spec, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    pCar_spec->field_0x1260 = 0.0f;
}

void C2_HOOK_FASTCALL CompletelyUnBendOtherThings(tCar_spec* pCar_spec) {
    CompletelyUnBendCollisionShapes(pCar_spec);
    CompletelyUnBendWheels(pCar_spec);
    memcpy(&pCar_spec->collision_info->cmpos, &pCar_spec->car_crush_spec->field_0x264, sizeof(pCar_spec->collision_info->cmpos));
}

void C2_HOOK_FASTCALL DoCompletelyUnBentThings(tCar_spec* pCar_spec) {
    pCar_spec->car_crush_spec->field_0x144 = 0;
    CompletelyUnBendOtherThings(pCar_spec);
    DRActorEnumRecurse(pCar_spec->car_model_actor, EnableGroovers, NULL);
}

void C2_HOOK_FASTCALL TotallyRepairModel(br_model* pModel) {
    tUser_detail_level_model* user_detail_model;
    tModel_detail_vertex_data* vertex_detail_data;
    br_vector3 vector_mash[1024];
    int i;

    if (pModel == NULL) {
        return;
    }
    user_detail_model = pModel->user;
    if (user_detail_model == NULL) {
        return;
    }
    vertex_detail_data = user_detail_model->field_0x4;
    if (vertex_detail_data == NULL) {
        return;
    }
    if (pModel->nvertices > REC2_ASIZE(vector_mash)) {
        PDFatalError("Holy polygons Batman! There's a lot of vertices on that model.");
    }
    for (i = 0; i < pModel->nvertices; i++) {
        br_vector3 pos;

        ExpandVector3(&pos, &vertex_detail_data[i].p, -10.0f, 10.0f);
        BrVector3Sub(&vector_mash[i], &pos, &pModel->vertices[i].p);
        BrVector3Copy(&pModel->vertices[i].p, &pos);
        vertex_detail_data[i].flags &= ~0x4;
    }
    AddModelMashToPipingSession(pModel, vector_mash);
    SetModelForUpdate(pModel);
}

// FUNCTION: CARMA2_HW 0x00439a10
int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data) {
    DRActorEnumRecurse(pCollision_info->actor, TotallyRepairModels, pUser_data);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00439a20
intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser) {
    tCar_spec* car_spec;
    tUser_crush_data* user_crush;
    tCar_crush_buffer_entry* crush_data;
    int i;

    car_spec = pUser;
    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 0;
    }
    for (i = 0; i < car_spec->count_detail_levels; i++) {
        TotallyRepairModel(user_crush->models[i]);
    }
    crush_data = user_crush->crush_data;
    if (crush_data != NULL) {
        TotallyRepairSmash(car_spec, crush_data);
        crush_data->field_0x2c = 0;
        if (crush_data->flap_data != NULL && !crush_data->flap_data->kev_o_flap) {
            crush_data->flap_data->field_0x15 = 0;
            crush_data->flap_data->field_0x14 = 1;
            crush_data->flap_data->field_0x18 = 0.f;
        }
        if (crush_data->detach_data != NULL) {
            crush_data->detach_data->field_0x0 = 0.f;
        }
    }
    return 0;
}

// SendReAttachAllBits

// STUB: CARMA2_HW 0x00439510
void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

// TotallyRepairCar

void C2_HOOK_FASTCALL AddTotalDeflectionOfModel(br_model* pModel, float* pDeflection) {
    tUser_detail_level_model* user_detail_model;
    tModel_detail_vertex_data* vertex_detail_data;
    float deflection = 0.0f;
    int i;

    if (pModel == NULL) {
        return;
    }
    user_detail_model = pModel->user;
    if (user_detail_model == NULL) {
        return;
    }
    vertex_detail_data = user_detail_model->field_0x4;
    if (vertex_detail_data == NULL) {
        return;
    }
    for (i = 0; i < pModel->nvertices; i++) {
        br_vector3 pos;

        ExpandVector3(&pos, &vertex_detail_data[i].p, -10.0f, 10.0f);
        deflection = (float)(deflection
            + fabs(pos.v[0] - pModel->vertices[i].p.v[0])
            + fabs(pos.v[1] - pModel->vertices[i].p.v[1])
            + fabs(pos.v[2] - pModel->vertices[i].p.v[2]));
    }
    *pDeflection += deflection;
}

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
br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle) {
    if (pAngle >= BR_ANGLE_DEG(180)) {
        pAngle = (br_angle)-pAngle;
    }
    return BrFixedToScalar(pAngle);
}

br_scalar C2_HOOK_FASTCALL CalculateAmountRepairedFraction(br_angle pOriginal_x, br_angle pOriginal_y, br_angle pOriginal_z, br_angle pRepair_x, br_angle pRepair_y, br_angle pRepair_z, br_scalar* pOriginal_amount) {
    br_scalar original_amount = NormalizedAngle(pOriginal_x) + NormalizedAngle(pOriginal_y) + NormalizedAngle(pOriginal_z);
    br_scalar repaired_amount = NormalizedAngle(pRepair_x) + NormalizedAngle(pRepair_y) + NormalizedAngle(pRepair_z);

    *pOriginal_amount = 2.0f * original_amount;
    return repaired_amount;
}

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

// FUNCTION: CARMA2_HW 0x0043bf30
void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3* pDest, br_model* pModel, br_vector3* pSrc) {
    int i;

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

