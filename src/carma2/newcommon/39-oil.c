#include "39-oil.h"

#include "41-utility.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x0065a398
const char* gOil_pixie_names[1] = {
    "OIL.PIX",
};

// GLOBAL: CARMA2_HW 0x00690c88
br_pixelmap* gOil_pixies[1];

// GLOBAL: CARMA2_HW 0x00690c90
tOil_spill_info gOily_spills[32];

// GLOBAL: CARMA2_HW 0x0074a68c
br_material* oily_material;

// GLOBAL: CARMA2_HW 0x00691710
int gNext_oil_pixie;

// FUNCTION: CARMA2_HW 0x004a6a10
void C2_HOOK_FASTCALL InitOilSpills(void) {
    int i;
    br_model* the_model;

    C2_HOOK_BUG_ON(sizeof(tOil_spill_info) != 0x54);

    for (i = 0; i < (int)REC2_ASIZE(gOil_pixie_names); i++) {
        gOil_pixies[i] = LoadPixelmap(gOil_pixie_names[i]);
        BrMapAdd(gOil_pixies[i]);
    }

    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        oily_material = BrMaterialAllocate(NULL);
        BrMaterialAdd(oily_material);
        oily_material->flags |= BR_MATF_LIGHT;
        oily_material->flags |= BR_MATF_PERSPECTIVE;
        oily_material->flags |= BR_MATF_SMOOTH;
        oily_material->ka = 0.99f;
        oily_material->kd = 0.0f;
        oily_material->ks = 0.0f;
        oily_material->power = 0.0f;
        oily_material->index_base = 0;
        oily_material->index_range = 0;
        oily_material->colour_map = gOil_pixies[0];
        BrMatrix23Identity(&oily_material->map_transform);
        oily_material->index_shade = BrTableFind("IDENTITY.TAB");
        GlorifyMaterial(&oily_material, 1, kRendererShadingType_Specular);
        BrMaterialUpdate(oily_material, BR_MATU_ALL);
        the_model = BrModelAllocate(NULL, 4, 2);
        the_model->flags |= BR_MODF_KEEP_ORIGINAL;

        the_model->faces[0].vertices[0] = 2;
        the_model->faces[0].vertices[1] = 1;
        the_model->faces[0].vertices[2] = 0;
        the_model->faces[0].material = NULL;
        the_model->faces[0].smoothing = 1;

        the_model->faces[1].vertices[0] = 3;
        the_model->faces[1].vertices[1] = 2;
        the_model->faces[1].vertices[2] = 0;
        the_model->faces[1].material = NULL;
        the_model->faces[1].smoothing = 1;

        BrVector3Set(&the_model->vertices[0].p, -1.f, 0.f, -1.f);
        BrVector2Set(&the_model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&the_model->vertices[1].p, 1.f, 0.f, 1.f);
        BrVector2Set(&the_model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&the_model->vertices[2].p, 1.f, 0.f, -1.f);
        BrVector2Set(&the_model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&the_model->vertices[3].p, -1.f, 0.f, 1.f);
        BrVector2Set(&the_model->vertices[3].map, 1.f, 1.f);
        gOily_spills[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gOily_spills[i].actor->model = the_model;
        gOily_spills[i].actor->render_style = BR_RSTYLE_NONE;
        gOily_spills[i].actor->material = oily_material;
        BrActorAdd(gOther_selfs[2], gOily_spills[i].actor);
    }
}

// FUNCTION: CARMA2_HW 0x004a6c50
void C2_HOOK_FASTCALL ResetOilSpills(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        gOily_spills[i].actor->render_style = BR_RSTYLE_NONE;
        gOily_spills[i].car = NULL;
        gOily_spills[i].car_actor = NULL;
        gOily_spills[i].stop_time = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004a6c80
void C2_HOOK_FASTCALL QueueOilSpill2(tCar_spec* pCar, br_actor* pCar_actor, br_model* pCar_model, tU32 pTime_delay, br_scalar pMin_size, br_scalar pMax_size, br_scalar pMin_grow_rate, br_scalar pMax_grow_rate, br_scalar pInitial_size, br_scalar pField_0x48, br_scalar pField_0x4c, br_scalar pField_0x40, br_scalar pField_0x44, br_material* pMaterial, br_material* pField_0x50) {
    int i;
    int free_index;
    int oldest_index;
    tU32 the_time;
    tU32 oldest_time;
    tOil_spill_info* oil;

    free_index = -1;
    oldest_index = 0;
    the_time = GetTotalTime();
    oldest_time = GetTotalTime();
    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        if (pCar != NULL && gOily_spills[i].car_actor == pCar_actor
            && the_time < gOily_spills[i].spill_time + 5000) {
            return;
        }
    }
    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        if (gOily_spills[i].car_actor == NULL) {
            free_index = i;
            break;
        }
        if (gOily_spills[i].spill_time < oldest_time) {
            oldest_time = gOily_spills[i].spill_time;
            oldest_index = i;
        }
    }
    if (free_index < 0) {
        free_index = oldest_index;
    }
    oil = &gOily_spills[free_index];
    oil->actor->material = pMaterial;
    oil->car = pCar;
    oil->car_actor = pCar_actor;
    oil->car_model = pCar_model;
    oil->spill_time = the_time + pTime_delay;
    oil->field_0x20 = pInitial_size;
    oil->full_size = (float)SRandomBetween(pMin_size, pMax_size);
    oil->grow_rate = (float)SRandomBetween(pMin_grow_rate, pMax_grow_rate);
    oil->current_size = pInitial_size;
    oil->actor->render_style = BR_RSTYLE_NONE;
    oil->field_0x40 = pField_0x40;
    oil->field_0x44 = pField_0x44;
    oil->field_0x48 = pField_0x48;
    oil->field_0x4c = pField_0x4c;
    oil->field_0x50 = pField_0x50;
}

// FUNCTION: CARMA2_HW 0x004a6dd0
void C2_HOOK_FASTCALL QueueOilSpill(tCar_spec* pCar) {
    QueueOilSpill2(pCar, pCar->car_master_actor, pCar->car_actor->model, 500,
        0.35f, 0.6f, 0.00003f, 0.0001f, 0.1f, 1.5f, 2.5f, 0.01f, 0.15f,
        oily_material, NULL);
}

// OKToSpillOil

// MungeOilsHeightAboveGround
void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil) {
    BrVector3Copy(&pOil->actor->t.t.translate.t, &pOil->position);
}

// FUNCTION: CARMA2_HW 0x004a6e20
void C2_HOOK_FASTCALL MungeIndexedOilsHeightAboveGround(int pIndex) {
    MungeOilsHeightAboveGround(&gOily_spills[pIndex]);
}

// FUNCTION: CARMA2_HW 0x004a7460
void C2_HOOK_FASTCALL SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel) {
    /* Resize the spill in the ground plane without changing vertex heights. */
    pModel->vertices[0].p.v[0] = -pOil->field_0x20;
    pModel->vertices[0].p.v[2] = -pOil->field_0x20;
    pModel->vertices[1].p.v[0] = pOil->field_0x20;
    pModel->vertices[1].p.v[2] = -pOil->field_0x20;
    pModel->vertices[2].p.v[0] = pOil->field_0x20;
    pModel->vertices[2].p.v[2] = pOil->field_0x20;
    pModel->vertices[3].p.v[0] = -pOil->field_0x20;
    pModel->vertices[3].p.v[2] = pOil->field_0x20;
    pOil->actor->render_style = BR_RSTYLE_FACES;
    BrMaterialUpdate(pOil->actor->material, BR_MATU_ALL);
    BrModelUpdate(pModel, BR_MODU_ALL);
}

// ProcessOilSpills

// FUNCTION: CARMA2_HW 0x004a74e0
int C2_HOOK_FASTCALL GetOilSpillCount(void) {
    return (int)REC2_ASIZE(gOily_spills);
}

// FUNCTION: CARMA2_HW 0x004a74f0
void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize) {
    if (gOily_spills[pIndex].car_actor != NULL) {
        *pActor = gOily_spills[pIndex].actor;
        *pSize = gOily_spills[pIndex].full_size;
    } else {
        *pActor = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004a77d0
int C2_HOOK_FASTCALL PointInSpill(br_vector3* pPoint, int pIndex) {
    const br_scalar inverse_world_scale = 1.f / WORLD_SCALE;
    tOil_spill_info* oil;
    br_actor* actor;
    br_scalar range_squared;
    br_scalar delta;

    oil = &gOily_spills[pIndex];
    actor = oil->actor;
    range_squared = oil->current_size * oil->current_size * 0.8f;
    delta = pPoint->v[0] * inverse_world_scale - actor->t.t.mat.m[3][0];
    if (delta * delta >= range_squared) {
        return 0;
    }
    delta = pPoint->v[2] * inverse_world_scale - actor->t.t.mat.m[3][2];
    if (delta * delta >= range_squared) {
        return 0;
    }
    delta = pPoint->v[1] * inverse_world_scale - actor->t.t.mat.m[3][1];
    if (fabs(delta) >= 0.1f) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004a7530
void C2_HOOK_FASTCALL GetOilFrictionFactors(tCar_spec* pCar, br_scalar* pFactor_1, br_scalar* pFactor_2, br_scalar* pFactor_3, br_scalar* pFactor_4) {
    const int wheel_order[4] = { 2, 3, 0, 1 };
    br_scalar* factors[4];
    br_vector3 world_pos;
    tOil_spill_info* oil;
    br_scalar min_remaining;
    br_scalar max_remaining;
    br_scalar min_friction;
    br_scalar max_friction;
    int i;
    int j;
    int wheel;

    *pFactor_1 = 1.f;
    *pFactor_2 = 1.f;
    *pFactor_3 = 1.f;
    *pFactor_4 = 1.f;
    if (pCar == NULL || (int)pCar->driver < eDriver_oppo) {
        return;
    }
    if (pCar->shadow_intersection_flags != 0) {
        for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
            oil = &gOily_spills[i];
            max_remaining = oil->field_0x4c;
            min_remaining = oil->field_0x48;
            /* Zero and NaN duration limits disable this spill. */
            if (!(max_remaining > 0.f || max_remaining < 0.f)) {
                continue;
            }
            if (((tU32)pCar->shadow_intersection_flags & (1u << i)) == 0) {
                continue;
            }
            /* Keep the limits from this spill across all wheel callbacks. */
            min_friction = oil->field_0x40;
            max_friction = oil->field_0x44;
            for (j = 0; j < (int)REC2_ASIZE(wheel_order); j++) {
                wheel = wheel_order[j];
                BrMatrix34ApplyP(&world_pos, &pCar->wpos[wheel], &pCar->car_master_actor->t.t.mat);
                if (PointInSpill(&world_pos, i)) {
                    pCar->oil_remaining[wheel] = (br_scalar)SRandomBetween(min_remaining, max_remaining);
                    pCar->maybe_blood_remaining[wheel] = (br_scalar)SRandomBetween(min_friction, max_friction);
                    pCar->field_0x16fc[wheel] = oil->field_0x50;
                }
            }
        }
    }
    factors[0] = pFactor_1;
    factors[1] = pFactor_2;
    factors[2] = pFactor_3;
    factors[3] = pFactor_4;
    for (j = 0; j < (int)REC2_ASIZE(wheel_order); j++) {
        wheel = wheel_order[j];
        if (pCar->oil_remaining[wheel] > 0.f || pCar->oil_remaining[wheel] < 0.f) {
            *factors[j] = pCar->maybe_blood_remaining[wheel];
        }
    }
}

// FUNCTION: CARMA2_HW 0x004a7860
void C2_HOOK_FASTCALL AdjustOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap) {
    tOil_spill_info* oil;

    (void)pStop_time;
    (void)pPixelmap;
    oil = &gOily_spills[pIndex];
    BrMatrix34Copy(&oil->actor->t.t.mat, pMat);
    oil->full_size = pFull_size;
    oil->grow_rate = pGrow_rate;
    oil->spill_time = pSpill_time;
    oil->stop_time = 0;
    oil->car = pCar;
    BrVector3Copy(&oil->original_pos, pOriginal_pos);
    oil->actor->render_style = BR_RSTYLE_NONE;
}

// ReceivedOilSpill

// FUNCTION: CARMA2_HW 0x004a7940
void C2_HOOK_FASTCALL RemoveCarsOilSlicks(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
        if (gOily_spills[i].car_actor != NULL && gOily_spills[i].car == pCar) {
            gOily_spills[i].car = NULL;
            gOily_spills[i].car_actor = NULL;
        }
    }
}

