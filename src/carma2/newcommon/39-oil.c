/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "39-oil.h"

#include "01-network.h"
#include "41-utility.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x0065a398
// GLOBAL: CARMA2_SW 0x00654cb0
const char* gOil_pixie_names[1] = {
    "OIL.PIX",
};

// GLOBAL: CARMA2_HW 0x00690c88
// GLOBAL: CARMA2_SW 0x0068ace0
br_pixelmap* gOil_pixies[1];

// GLOBAL: CARMA2_HW 0x00690c90
// GLOBAL: CARMA2_SW 0x0068ace8
tOil_spill_info gOily_spills[32];

// GLOBAL: CARMA2_HW 0x0074a68c
// GLOBAL: CARMA2_SW 0x0074114c
br_material* oily_material;

// GLOBAL: CARMA2_HW 0x00691710
int gNext_oil_pixie;

// FUNCTION: CARMA2_HW 0x004a6a10
// FUNCTION: CARMA2_SW 0x004a2b90
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
#ifdef REC2_HW
        GlorifyMaterial(&oily_material, 1, kRendererShadingType_Specular);
#endif
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a6c80
// STUB: CARMA2_SW 0x004a2df0
void C2_HOOK_FASTCALL QueueOilSpill2(tCar_spec* pCar, br_actor* pCar_actor, br_model* pCar_model, tU32 pTime_delay, br_scalar pMin_size, br_scalar pMax_size, br_scalar pMin_grow_rate, br_scalar pMax_grow_rate, br_scalar pInitial_size, br_scalar pField_0x48, br_scalar pField_0x4c, br_scalar pField_0x40, br_scalar pField_0x44, br_material* pMaterial, br_material* pField_0x50) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a6dd0
// FUNCTION: CARMA2_SW 0x004a2f40
void C2_HOOK_FASTCALL QueueOilSpill(tCar_spec* pCar) {
    QueueOilSpill2(pCar, pCar->car_master_actor, pCar->car_actor->model, 500,
        0.35f, 0.6f, 0.00003f, 0.0001f, 0.1f, 1.5f, 2.5f, 0.01f, 0.15f,
        oily_material, NULL);
}

// OKToSpillOil

/* MungeOilsHeightAboveGround: inlined into its callers in the original */
static void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil) {

    pOil->actor->t.t.translate.t = pOil->position;
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

// FUNCTION: CARMA2_HW 0x004a74e0
// FUNCTION: CARMA2_SW 0x004a3650
int C2_HOOK_FASTCALL GetOilSpillCount(void) {
    return (int)REC2_ASIZE(gOily_spills);
}

// FUNCTION: CARMA2_HW 0x004a74f0
// FUNCTION: CARMA2_SW 0x004a3660
void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize) {
    if (gOily_spills[pIndex].car_actor != NULL) {
        *pActor = gOily_spills[pIndex].actor;
        *pSize = gOily_spills[pIndex].full_size;
    } else {
        *pActor = NULL;
    }
}

// STUB: CARMA2_HW 0x004a77d0
int C2_HOOK_FASTCALL PointInSpill(br_vector3* pPoint, int pIndex) {
    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004a7530
void C2_HOOK_FASTCALL GetOilFrictionFactors(tCar_spec* pCar, br_scalar* pFl, br_scalar* pFr, br_scalar* pRl, br_scalar* pRr) {
    int i;
    br_vector3 v;
    br_scalar a, b, c, d;

    *pFl = 1.f;
    *pFr = 1.f;
    *pRl = 1.f;
    *pRr = 1.f;
    if (pCar != NULL && (int)pCar->driver > 5) {
        if (pCar->shadow_intersection_flags != 0) {
            for (i = 0; i < (int)REC2_ASIZE(gOily_spills); i++) {
                c = gOily_spills[i].field_0x48;
                d = gOily_spills[i].field_0x4c;
                if (d != 0.f && (pCar->shadow_intersection_flags & (1 << i))) {
                    a = gOily_spills[i].field_0x40;
                    b = gOily_spills[i].field_0x44;
                    BrMatrix34ApplyP(&v, &pCar->wpos[2], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&v, i)) {
                        pCar->oil_remaining[2] = SRandomBetween(c, d);
                        pCar->maybe_blood_remaining[2] = SRandomBetween(a, b);
                        pCar->field_0x16fc[2] = (br_material*)gOily_spills[i].field_0x50;
                    }
                    BrMatrix34ApplyP(&v, &pCar->wpos[3], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&v, i)) {
                        pCar->oil_remaining[3] = SRandomBetween(c, d);
                        pCar->maybe_blood_remaining[3] = SRandomBetween(a, b);
                        pCar->field_0x16fc[3] = (br_material*)gOily_spills[i].field_0x50;
                    }
                    BrMatrix34ApplyP(&v, &pCar->wpos[0], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&v, i)) {
                        pCar->oil_remaining[0] = SRandomBetween(c, d);
                        pCar->maybe_blood_remaining[0] = SRandomBetween(a, b);
                        pCar->field_0x16fc[0] = (br_material*)gOily_spills[i].field_0x50;
                    }
                    BrMatrix34ApplyP(&v, &pCar->wpos[1], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&v, i)) {
                        pCar->oil_remaining[1] = SRandomBetween(c, d);
                        pCar->maybe_blood_remaining[1] = SRandomBetween(a, b);
                        pCar->field_0x16fc[1] = (br_material*)gOily_spills[i].field_0x50;
                    }
                }
            }
        }
        if (pCar->oil_remaining[2] != 0.f) {
            *pFl = pCar->maybe_blood_remaining[2];
        }
        if (pCar->oil_remaining[3] != 0.f) {
            *pFr = pCar->maybe_blood_remaining[3];
        }
        if (pCar->oil_remaining[0] != 0.f) {
            *pRl = pCar->maybe_blood_remaining[0];
        }
        if (pCar->oil_remaining[1] != 0.f) {
            *pRr = pCar->maybe_blood_remaining[1];
        }
    }
}

// FUNCTION: CARMA2_HW 0x004a7860
void C2_HOOK_FASTCALL AdjustOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap) {

    BrMatrix34Copy(&gOily_spills[pIndex].actor->t.t.mat, pMat);
    gOily_spills[pIndex].full_size = pFull_size;
    gOily_spills[pIndex].grow_rate = pGrow_rate;
    gOily_spills[pIndex].spill_time = pSpill_time;
    gOily_spills[pIndex].stop_time = 0;
    gOily_spills[pIndex].car = pCar;
    BrVector3Copy(&gOily_spills[pIndex].original_pos, pOriginal_pos);
    gOily_spills[pIndex].actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004a78e0
// FUNCTION: CARMA2_SW 0x004a3a50
void C2_HOOK_FASTCALL ReceivedOilSpill(tNet_message_chunk* pContents) {
    tCar_spec* car;

    car = NetCarFromPlayerID(pContents->oil_spill.player);
    if (car == NULL) {
        return;
    }
    QueueOilSpill2(car, car->car_master_actor, car->car_actor->model, 500,
        0.35f, 0.6f, 0.00003f, 0.0001f, 0.1f, 1.5f, 2.5f, 0.01f, 0.15f,
        oily_material, NULL);
}

// RemoveCarsOilSlicks

// PipeSingleOilSpill

// STUB: CARMA2_HW 0x004a6e50
void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period) { NOT_IMPLEMENTED(); }
