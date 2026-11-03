/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "11-world1.h"

#include "28-world3.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006ab7a8
// GLOBAL: CARMA2_SW 0x006a51f8
int gDelete_count;

// GLOBAL: CARMA2_HW 0x006aafc8
// GLOBAL: CARMA2_SW 0x006a4a18
br_actor* gDelete_list[500];

// GLOBAL: CARMA2_HW 0x006b6400
char gAdditional_model_path[256];

// GLOBAL: CARMA2_HW 0x006b6500
char gAdditional_actor_path[256];

// GLOBAL: CARMA2_HW 0x006b6620
br_model* gAdditional_models[1000];

// GLOBAL: CARMA2_HW 0x006ab948
int gNumber_of_additional_models;

// GLOBAL: CARMA2_HW 0x006b6600
br_actor* gLast_actor;

// FUNCTION: CARMA2_HW 0x00507210
// FUNCTION: CARMA2_SW 0x00502660
br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* pActor, br_matrix34* pMatrix, void* pArg) {
    int i;
    int parent_already_doomed;

    if ((pActor != gAdditional_actors
            && (pActor->identifier == NULL || pActor->identifier[0] == '&')
            && Vector3IsZero((br_vector3*)pMatrix->m[3]))
        || (pActor->model == NULL && pActor->type == BR_ACTOR_MODEL)) {
        parent_already_doomed = 0;
        for (i = 0; i < gDelete_count; i++) {
            if (gDelete_list[i] == pActor) {
                parent_already_doomed = 1;
                break;
            }
        }
        if (!parent_already_doomed) {
            gDelete_list[gDelete_count] = pActor;
            gDelete_count++;
        }
    }
    return 0;
}

// DeleteAnyZeroBastards

// FUNCTION: CARMA2_HW 0x005072b0
// FUNCTION: CARMA2_SW 0x00502700
br_uint_32 C2_HOOK_FASTCALL ApplyTransToModels(br_actor* pActor, br_matrix34* pMatrix, void* pArg) {
    int i;
    br_vector3 temp_point;

    if (pActor->identifier != NULL && pActor->identifier[0] != '&') {
        if (pActor->model != NULL) {
            for (i = 0; i < pActor->model->nvertices; i++) {
                temp_point = pActor->model->vertices[i].p;
                BrMatrix34ApplyP(&pActor->model->vertices[i].p, &temp_point, pMatrix);
            }
            BrModelUpdate(pActor->model, BR_MODU_ALL);
        }
        BrMatrix34Identity(&pActor->t.t.mat);
    }
    return 0;
}

// FindSpecVolIndex

// FUNCTION: CARMA2_HW 0x005074d0
// FUNCTION: CARMA2_SW 0x00502920
void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* pMat, br_material* pMat_1, br_material* pMat_2, int pAxis_0, int pAxis_1) {
    pMat_1->map_transform.m[0][0] = 6.f * BrVector3Length((br_vector3*)pMat->m[pAxis_0]);
    pMat_1->map_transform.m[1][1] = 6.f * BrVector3Length((br_vector3*)pMat->m[pAxis_1]);
    BrMatrix23Copy(&pMat_2->map_transform, &pMat_1->map_transform);
}

// SetSpecVolMatSize

// UpdateSpecVol

// PutAdditionalNonCarsOnToAdditionalActors

// PutAdditionalNonCarsOnToNonTrackActor

// SaveAdditionalActors

// DoSaveAdditionalStuff

// STUB: CARMA2_HW 0x00507360
// STUB: CARMA2_SW 0x005027b0
void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00507580
void C2_HOOK_FASTCALL LoadAdditionalStuff(void) {
    extern int gNum_active_non_cars;
    extern int gNumber_of_additional_models;
    extern br_model* gAdditional_models[1000];
    extern char gAdditional_model_path[256];
    extern char gAdditional_actor_path[256];
    extern br_actor* gLast_actor;
    extern br_actor* gUniverse_actor;
    extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
    int i;

    if (gNum_active_non_cars != 0) {
        NewTextHeadupSlot(4, 0, 2000, -2, "Can't load with active non cars");
        return;
    }
    NewTextHeadupSlot(4, 0, 2000, -2, "Loading Additional Stuff");
    for (i = 0; i < gNumber_of_additional_models; i++) {
        BrModelRemove(gAdditional_models[i]);
        BrModelFree(gAdditional_models[i]);
    }
    BrActorRemove(gAdditional_actors);
    BrActorFree(gAdditional_actors);
    gLast_actor = NULL;
    gNumber_of_additional_models = BrModelLoadMany(gAdditional_model_path, gAdditional_models, 1000);
    for (i = 0; i < gNumber_of_additional_models; i++) {
        gAdditional_models[i]->flags = 0x80;
    }
    BrModelAddMany(gAdditional_models, gNumber_of_additional_models);
    gAdditional_actors = BrActorLoad(gAdditional_actor_path);
    if (gAdditional_actors == NULL) {
        gAdditional_actors = BrActorAllocate(0, NULL);
    }
    BrActorAdd(gUniverse_actor, gAdditional_actors);
}

// FUNCTION: CARMA2_HW 0x00508e50
intptr_t C2_HOOK_CDECL MoveModelVertices(br_actor* pActor, br_vector3* pOffset) {
    int i;

    if (pActor->model != NULL) {
        i = 0;
        if (pActor->model->nvertices > 0) {
            do {
                pActor->model->vertices[i].p.v[0] += pOffset->v[0];
                pActor->model->vertices[i].p.v[1] += pOffset->v[1];
                pActor->model->vertices[i].p.v[2] += pOffset->v[2];
                i++;
            } while (i < pActor->model->nvertices);
        }
    }
    return 0;
}

