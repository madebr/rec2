/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "37-brucetrk.h"

#include "41-utility.h"

#include "globvars.h"
#include "rec2_macros.h"

#include <math.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x0058f490
// GLOBAL: CARMA2_SW 0x0058a490
int gRender_alternative_track_actors = 1;

// GLOBAL: CARMA2_HW 0x00655e60
// GLOBAL: CARMA2_SW 0x00650d48
br_scalar gYon_factor = 0.25f;

// GLOBAL: CARMA2_HW 0x0074b74c
// GLOBAL: CARMA2_SW 0x00741e0c
br_material* gDefault_track_material;

// GLOBAL: CARMA2_HW 0x00679260
// GLOBAL: CARMA2_SW 0x006739d0
int gCount_track_non_cars;

// GLOBAL: CARMA2_HW 0x00679268
// GLOBAL: CARMA2_SW 0x006739d8
br_vector3 gAverage_grid_position;

// AssertNoncars

// AllocateActorMatrix

// DisposeActorMatrix

// DisposeRuntimeBuiltModels

// FUNCTION: CARMA2_HW 0x0040ca90
// FUNCTION: CARMA2_SW 0x0040ca80
void C2_HOOK_FASTCALL DisposeColumns(tTrack_spec* pTrack_spec) {
    int z;
    int x;
    tTrack_square** columns;
    tU16 row;
    int i;

    for (z = 0; z != pTrack_spec->ncolumns_z; z++) {
        for (x = 0; x != pTrack_spec->ncolumns_x; x++) {
            br_actor* actor = pTrack_spec->columns[z][x].actor_0x8;

            if (actor != NULL && actor->model != NULL) {
                BrModelRemove(actor->model);
                BrModelFree(actor->model);
            }
        }
    }
    columns = pTrack_spec->columns;
    if (columns != NULL) {
        for (row = 0; row != pTrack_spec->ncolumns_z; row++) {
            BrMemFree(columns[row]);
        }
        BrMemFree(columns);
    }
    if (pTrack_spec->non_car_list != NULL) {
        for (i = gCount_track_non_cars; i < pTrack_spec->count_non_cars; i++) {
            if (pTrack_spec->non_car_list[i] != NULL && pTrack_spec->non_car_list[i]->parent != gNon_track_actor) {
                BrActorRemove(pTrack_spec->non_car_list[i]);
                BrActorFree(pTrack_spec->non_car_list[i]);
            }
        }
        BrMemFree(pTrack_spec->non_car_list);
    }
}

// FUNCTION: CARMA2_HW 0x0040cb90
// FUNCTION: CARMA2_SW 0x0040cb80
void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec) {
    br_scalar x;
    br_scalar z;
    br_scalar size_x = pTrack_spec->column_size_x;
    br_scalar size_z = pTrack_spec->column_size_z;

    x = (pX - pTrack_spec->origin_x) / size_x;
    z = (pZ - pTrack_spec->origin_z) / size_z;
    if (x < 0.0f) {
        x = 0.0f;
    }
    if (x >= pTrack_spec->ncolumns_x) {
        x = pTrack_spec->ncolumns_x - 1.0f;
    }
    if (z < 0.0f) {
        z = 0.0f;
    }
    if (z >= pTrack_spec->ncolumns_z) {
        z = pTrack_spec->ncolumns_z - 1.0f;
    }
    *pColumn_x = (tU8)x;
    *pColumn_z = (tU8)z;
}

// FindNonCarsCB

// MungeFaces

// ProcessModelsCB

// ProcessModels

// ExtractColumns

// FUNCTION: CARMA2_HW 0x0040d7c0
// FUNCTION: CARMA2_SW 0x0040d7b0
void C2_HOOK_FASTCALL RenderTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world) {
    tU8 column_x;
    tU8 column_z;
    tU8 min_x;
    tU8 max_x;
    tU8 min_z;
    tU8 max_z;
    br_vector3 edge_before;
    br_vector3 edge_after;
    br_camera* camera;
    br_scalar tan_fov_ish;
    br_scalar scale;
    tU8 x;
    tU8 z;
    tU8 idx_x;
    tU8 idx_z;
    br_actor* a;

    camera = (br_camera*)pCamera->type_data;
    if (pTrack_spec->columns == NULL) {
        BrZbsSceneRenderAdd(pWorld);
    } else {
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0], pCamera_to_world->m[3][2], pTrack_spec);
        min_x = column_x;
        max_x = column_x;
        min_z = column_z;
        max_z = column_z;
        tan_fov_ish = BR_SIN(camera->field_of_view / 2) / BR_COS(camera->field_of_view / 2);

        edge_after.v[1] = tan_fov_ish;
        edge_after.v[0] = tan_fov_ish * camera->aspect;
        edge_after.v[2] = -1.0;
        scale = camera->yon_z * gYon_factor;
        BrVector3Scale(&edge_before, &edge_after, scale);
        BrMatrix34ApplyV(&edge_after, &edge_before, pCamera_to_world);
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0] + edge_after.v[0], pCamera_to_world->m[3][2] + edge_after.v[2], pTrack_spec);
        if (column_x < min_x) {
            min_x = column_x;
        } else if (column_x > max_x) {
            max_x = column_x;
        }
        if (column_z < min_z) {
            min_z = column_z;
        } else if (column_z > max_z) {
            max_z = column_z;
        }
        edge_before.v[0] = -edge_before.v[0];
        BrMatrix34ApplyV(&edge_after, &edge_before, pCamera_to_world);
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0] + edge_after.v[0], pCamera_to_world->m[3][2] + edge_after.v[2], pTrack_spec);
        if (column_x < min_x) {
            min_x = column_x;
        } else if (column_x > max_x) {
            max_x = column_x;
        }
        if (column_z < min_z) {
            min_z = column_z;
        } else {
            if (column_z > max_z) {
                max_z = column_z;
            }
        }
        edge_before.v[1] = -edge_before.v[1];
        BrMatrix34ApplyV(&edge_after, &edge_before, pCamera_to_world);
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0] + edge_after.v[0], pCamera_to_world->m[3][2] + edge_after.v[2], pTrack_spec);
        if (column_x < min_x) {
            min_x = column_x;
        } else if (column_x > max_x) {
            max_x = column_x;
        }
        if (column_z < min_z) {
            min_z = column_z;
        } else if (column_z > max_z) {
            max_z = column_z;
        }
        edge_before.v[0] = -edge_before.v[0];
        BrMatrix34ApplyV(&edge_after, &edge_before, pCamera_to_world);
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0] + edge_after.v[0], pCamera_to_world->m[3][2] + edge_after.v[2], pTrack_spec);
        if (column_x < min_x) {
            min_x = column_x;
        } else if (column_x > max_x) {
            max_x = column_x;
        }
        if (column_z < min_z) {
            min_z = column_z;
        } else if (column_z > max_z) {
            max_z = column_z;
        }
        if (min_x > 0) {
            min_x--;
        }
        if (max_x < pTrack_spec->ncolumns_x - 1) {
            max_x++;
        }
        if (min_z > 0) {
            min_z--;
        }
        if (max_z < pTrack_spec->ncolumns_z - 1) {
            max_z++;
        }
        if ((float)fabs(pCamera_to_world->m[2][0]) > (float)fabs(pCamera_to_world->m[2][2])) {
            for (x = min_x; x <= max_x; x++) {
                for (z = min_z; z <= max_z; z++) {
                    idx_x = (tU8)(pCamera_to_world->m[2][0] > 0.f ? x : min_x + max_x - x);
                    idx_z = (tU8)(pCamera_to_world->m[2][2] > 0.f ? z : min_z + max_z - z);
                    a = pTrack_spec->columns[idx_z][idx_x].actor_0x0;
                    if (a != NULL) {
                        if (gRender_alternative_track_actors) {
                            BrZbsSceneRenderAdd(a);
                        } else {
                            BrZbsSceneRenderAdd(pTrack_spec->columns[idx_z][idx_x].actor_0x4);
                        }
                    }
                }
            }
        } else {
            for (z = min_z; z <= max_z; z++) {
                for (x = min_x; x <= max_x; x++) {
                    idx_x = (tU8)(pCamera_to_world->m[2][0] > 0.f ? x : min_x + max_x - x);
                    idx_z = (tU8)(pCamera_to_world->m[2][2] > 0.f ? z : min_z + max_z - z);
                    a = pTrack_spec->columns[idx_z][idx_x].actor_0x0;
                    if (a != NULL) {
                        if (gRender_alternative_track_actors) {
                            BrZbsSceneRenderAdd(a);
                        } else {
                            BrZbsSceneRenderAdd(pTrack_spec->columns[idx_z][idx_x].actor_0x4);
                        }
                    }
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0040dfe0
// FUNCTION: CARMA2_SW 0x0040dfc0
br_scalar C2_HOOK_STDCALL GetYonFactor(void) {

    return gYon_factor;
}

// FUNCTION: CARMA2_HW 0x0040dff0
// FUNCTION: CARMA2_SW 0x0040dfd0
void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew) {

    gYon_factor = pNew;
}

// FUNCTION: CARMA2_HW 0x0040e290
// FUNCTION: CARMA2_SW 0x0040e270
intptr_t C2_HOOK_CDECL FoundAnActor(br_actor* pActor, void* pContext) {
    tFoundAnActor_context* context = pContext;
    char* identifier;

    if ((float)fabs(pActor->t.t.mat.m[3][0] - context->pos->v[0]) > context->max_dist_axis
            || (float)fabs(pActor->t.t.mat.m[3][1] - context->pos->v[1]) > context->max_dist_axis
            || (float)fabs(pActor->t.t.mat.m[3][2] - context->pos->v[2]) > context->max_dist_axis) {
        return 0;
    }
    if (pActor->type != BR_ACTOR_MODEL && (context->match_flags & 0x1000)) {
        return 0;
    }
    identifier = pActor->identifier;
    if (identifier == NULL && !(context->match_flags & 0x4)) {
        return 0;
    }
    if (identifier != NULL && identifier[0] == '&' && !(context->match_flags & 0x3)) {
        return 0;
    }
    if (identifier != NULL && identifier[0] != '&' && !(context->match_flags & 0x4)) {
        return 0;
    }
    if (context->match_type == 1) {
        if (pActor->type != BR_ACTOR_MODEL || pActor->model == NULL || pActor->model->identifier == NULL) {
            return 0;
        }
        if (context->identifier_index >= 0 && pActor->model->identifier[context->identifier_index] != context->identifier_value) {
            return 0;
        }
        if (context->identifier_index < 0 && pActor->model->identifier[strlen(pActor->model->identifier) + context->identifier_index] != context->identifier_value) {
            return 0;
        }
    }
    if (context->match_type == 2) {
        if (identifier == NULL) {
            return 0;
        }
        if (context->identifier_index >= 0 && identifier[context->identifier_index] != context->identifier_value) {
            return 0;
        }
        if (context->identifier_index < 0 && pActor->identifier[strlen(pActor->identifier) + context->identifier_index] != context->identifier_value) {
            return 0;
        }
    }
    if (Vector3DistanceSquared(context->pos, (br_vector3*)pActor->t.t.mat.m[3]) >= context->max_dist_squared) {
        return 0;
    }
    return context->callback(pActor, context->callback_context);
}

// FUNCTION: CARMA2_HW 0x0040e000
// FUNCTION: CARMA2_SW 0x0040dfe0
int C2_HOOK_FASTCALL ProcessNearbyActors(tTrack_spec* pTrack, br_vector3* pPos, float pMax_dist, int pMatch_type, char pIdentifier_value, tS8 pIdentifier_index, tU16 pMatch_flags, tNearbyActors_cbfn* pCallback, void* pContext) {

    tFoundAnActor_context found_an_actor_context;
    float p;
    float column_origin;
    int x, xmin, xmax;
    int z, zmin, zmax;

    found_an_actor_context.match_type = pMatch_type;
    found_an_actor_context.identifier_value = pIdentifier_value;
    found_an_actor_context.identifier_index = pIdentifier_index;
    found_an_actor_context.match_flags = pMatch_flags;
    found_an_actor_context.pos = pPos;
    found_an_actor_context.callback_context = pContext;
    found_an_actor_context.callback = pCallback;
    found_an_actor_context.max_dist_axis = pMax_dist;
    x = (int)((pPos->v[0] - pTrack->origin_x) / pTrack->column_size_x);
    found_an_actor_context.max_dist_squared = pMax_dist * pMax_dist;
    column_origin = pTrack->origin_x + (float)x * pTrack->column_size_x;
    p = column_origin;
    xmin = x;
    while (p + pMax_dist > pPos->v[0]) {
        if (xmin == 0) {
            break;
        }
        p -= pTrack->column_size_x;
        xmin -= 1;
    }
    p = pTrack->column_size_x + column_origin;
    xmax = x;
    while (pPos->v[0] + pMax_dist > p) {
        if (xmax + 1 >= pTrack->ncolumns_x) {
            break;
        }
        p += pTrack->column_size_x;
        xmax += 1;
    }

    z = (int)((pPos->v[2] - pTrack->origin_z) / pTrack->column_size_z);
    column_origin = pTrack->origin_z + (float)z * pTrack->column_size_z;
    p = column_origin;
    zmin = z;
    while (p + pMax_dist > pPos->v[2]) {
        if (zmin == 0) {
            break;
        }
        p -= pTrack->column_size_z;
        zmin -= 1;
    }
    p = pTrack->column_size_z + column_origin;
    zmax = z;
    while (pPos->v[2] + pMax_dist > p) {
        if (zmax + 1 >= pTrack->ncolumns_z) {
            break;
        }
        p += pTrack->column_size_z;
        zmax += 1;
    }
    if (xmin < 0) {
        xmin = 0;
    } else if (xmin >= pTrack->ncolumns_x) {
        xmin = pTrack->ncolumns_x - 1;
    }
    if (xmax < 0) {
        xmax = 0;
    } else if (xmax >= pTrack->ncolumns_x) {
        xmax = pTrack->ncolumns_x - 1;
    }
    if (zmin < 0) {
        zmin = 0;
    } else if (zmin >= pTrack->ncolumns_z) {
        zmin = pTrack->ncolumns_z - 1;
    }
    if (zmax < 0) {
        zmax = 0;
    } else if (zmax >= pTrack->ncolumns_z) {
        zmax = pTrack->ncolumns_z - 1;
    }

    for (x = xmin; x <= xmax; x++) {
        for (z = zmin; z <= zmax; z++) {
            if (pTrack->columns[z][x].actor_0x0 != NULL) {
                p = (float)(tU32)DRActorEnumRecurse(pTrack->columns[z][x].actor_0x0, FoundAnActor, &found_an_actor_context);
                if (p != 0.0f) {
                    return (int)p;
                }
            }
        }
    }
    return 0;
}

