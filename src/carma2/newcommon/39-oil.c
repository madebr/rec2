#include "39-oil.h"

#include "41-utility.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"

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

// PointInSpill

// GetOilFrictionFactors

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

