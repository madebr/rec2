#include "brucetrk.h"

#include "globvars.h"
#include "globvrpb.h"
#include "init.h"
#include "pedestrn.h"
#include "smashing.h"
#include "world.h"

#include "platform.h"

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_scalar, gYon_factor, 0x00655e60, 0.25f);

br_scalar C2_HOOK_STDCALL GetYonFactor(void) {

    return C2V(gYon_factor);
}
C2_HOOK_FUNCTION(0x0040dfe0, GetYonFactor)

void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew) {

    C2V(gYon_factor) = pNew;
}
C2_HOOK_FUNCTION(0x0040dff0, SetYonFactor)

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gMr_blendy, 0x00679264);

void (C2_HOOK_FASTCALL * StripBlendedFaces_original)(br_actor* pActor, br_model* pModel);
void C2_HOOK_FASTCALL StripBlendedFaces(br_actor* pActor, br_model* pModel) {

#if 0//defined(C2_HOOKS_ENABLED)
    StripBlendedFaces_original(pActor, pModel);
#else

    int i;
    int j;
    int k;
    br_face* face;
    int changed_one;

    changed_one = 0;

    for (i = 0; i < pModel->nfaces; i++) {
        face = &pModel->faces[i];

        MaybeSpawnPedestrian(face, pModel);
        if (MaterialIsSmashableTrigger(face->material)) {
            if (C2V(gMr_blendy) == NULL) {
                C2V(gMr_blendy) = BrActorAllocate(BR_ACTOR_MODEL, NULL);
                C2V(gMr_blendy)->render_style = BR_RSTYLE_FACES;
                C2V(gMr_blendy)->model = BrModelAllocate(NULL, 2000, 2000);
                C2V(gMr_blendy)->model->nfaces = 0;
                C2V(gMr_blendy)->model->nvertices = 0;
                C2V(gMr_blendy)->model->flags |= BR_MODF_UPDATEABLE;
            }
            br_face *blendy_face = &C2V(gMr_blendy)->model->faces[C2V(gMr_blendy)->model->nfaces];
            C2V(gMr_blendy)->model->nfaces += 1;
            c2_memcpy(blendy_face, face, sizeof(br_face));
            for (j = 0; j < 3; j++) {
                br_vertex *vertex_j = &pModel->vertices[face->vertices[j]];
                for (k = 0; k < C2V(gMr_blendy)->model->nvertices; k++) {
                    if (Vector3Equals(&C2V(gMr_blendy)->model->vertices[k].p, &vertex_j->p)
                            && Vector2Equals(&C2V(gMr_blendy)->model->vertices[k].map, &vertex_j->map)) {
                        blendy_face->vertices[j] = k;
                        vertex_j = NULL;
                        break;
                    }
                }
                if (vertex_j != NULL) {
                    blendy_face->vertices[j] = C2V(gMr_blendy)->model->nvertices;
                    c2_memcpy(&C2V(gMr_blendy)->model->vertices[C2V(gMr_blendy)->model->nvertices], vertex_j, sizeof(br_vertex));
                    C2V(gMr_blendy)->model->nvertices += 1;
                }
            }
            if (i < pModel->nfaces - 1) {
                c2_memmove(&pModel->faces[i], &pModel->faces[i + 1], (pModel->nfaces - (i + 1)) * sizeof(br_vertex));
            }
            pModel->nfaces -= 1;
            i -= 1;
            changed_one = 1;
        }
    }
    if (changed_one) {
        if (pModel->nfaces != 0) {
            BrModelUpdate(pModel, BR_MODU_ALL);
        } else {
            pActor->model = NULL;
            pActor->render_style = BR_RSTYLE_NONE;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040d530, StripBlendedFaces, StripBlendedFaces_original)

intptr_t (C2_HOOK_CDECL * FindNonCarsCB_original)(br_actor* pActor, tTrack_spec* pTrack_spec);
intptr_t C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, tTrack_spec* pTrack_spec) {

#if defined(C2_HOOKS_ENABLED)
    return FindNonCarsCB_original(pActor, pTrack_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040d1f0, FindNonCarsCB, FindNonCarsCB_original)

void (C2_HOOK_FASTCALL * GetModelTextureArea_original)(br_model* pModel, int* pArea_1, int* pArea_2);
void C2_HOOK_FASTCALL GetModelTextureArea(br_model* pModel, int* pArea_1, int* pArea_2) {
    int i;

    *pArea_1 = 0;
    *pArea_2 = 0;

    for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {
        int j;
        v11group* v11g = &V11MODEL(pModel)->groups[i];

        for (j = 0; j < v11g->nfaces; j++) {
            int k;
            float min_x =  10000.f;
            float max_x = -10000.f;
            float min_y =  10000.f;
            float max_y = -10000.f;
            v11face* v11f = &v11g->faces[j];
            int dx;
            int dy;

            for (k = 0; k < 3; k++) {
                br_uint_16 vertex_i = v11f->vertices[k];

                if (v11g->vertices[vertex_i].map.v[0] < min_x) {
                    min_x = v11g->vertices[vertex_i].map.v[0];
                }
                if (v11g->vertices[vertex_i].map.v[0] > max_x) {
                    max_x = v11g->vertices[vertex_i].map.v[0];
                }
                if (v11g->vertices[vertex_i].map.v[1] < min_y) {
                    min_y = v11g->vertices[vertex_i].map.v[1];
                }
                if (v11g->vertices[vertex_i].map.v[1] > max_y) {
                    max_y = v11g->vertices[vertex_i].map.v[1];
                }
            }
            dy = (int)(max_y - min_y + 0.9f);
            dx = (int)(max_x - min_x + 0.9f);
            *pArea_1 += dx * dy;
            *pArea_2 += dx * dy;
        }
    }
    *pArea_1 += 50;
    *pArea_2 += 50;
    if (*pArea_1 > 250) {
        *pArea_1 = 250;
    }
    if (*pArea_2 > 250) {
        *pArea_2 = 250;
    }

#if C2_HOOKS_ENABLED
    {
        int a1, a2;

        GetModelTextureArea_original(pModel, &a1, &a2);
        C2_HOOK_ASSERT(a1 == *pArea_1);
        C2_HOOK_ASSERT(a2 == *pArea_2);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f5ae0, GetModelTextureArea, GetModelTextureArea_original)

void C2_HOOK_FASTCALL FixModelPointer(br_model* pModel, br_uint_16 pFlags) {

    C2_HOOK_BUG_ON(sizeof(v11group) != 0x24);

    if (pModel->nvertices != 0 && pModel->nfaces != 0) {
        int i;

        BrModelUpdate(pModel, pFlags);
        for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {
            v11group* v11g;

            v11g = &V11MODEL(pModel)->groups[i];
            *v11g->face_colours.materials = pModel->faces[*v11g->face_user].material;
        }
    }
}
C2_HOOK_FUNCTION(0x00515fa0, FixModelPointer)

void C2_HOOK_FASTCALL ProcessSmashableActorModel(br_actor* pActor) {
    int i;

    if (pActor->model == NULL || pActor->model->identifier == NULL) {
        return;
    }
    for (i = 0; i < C2V(gCount_track_smashable_environment_specs); i++) {
        tSmashable_item_spec* spec;

        spec = &C2V(gTrack_smashable_environment_specs)[i];
        if (spec->trigger_type == kSmashableTrigger_Model && spec->trigger_object.model == pActor->model) {
            char s[64];
            char s2[64];
            size_t len;

            c2_strcpy(s, pActor->identifier);
            len = c2_strlen(s);
            if (len < 4) {
                sprintf(s, "Smash material %s has a name that is less than 4 characters long", pActor->identifier);
                PDFatalError(s);
            }
            c2_strcpy(s2, &s[len - 4]);
            c2_sprintf(&s[len - 4], "        ");
            c2_sprintf(&s[5], "%cx", '|');
            s[6] = i + 1;
            c2_strcat(s, s2);
            s[11] = '\0';
            if (c2_strlen(pActor->identifier) < 12) {
                BrResFree(pActor->identifier);
                pActor->identifier = BrResStrDup(pActor, s);
            } else {
                c2_strcpy(pActor->identifier, s);
            }

            if ((C2V(gCurrent_race).race_spec->race_type > 3 && spec->mode_data.field_0x14 == C2V(gCurrent_race).race_spec->options.cars.count_opponents)
                    || (C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_net_game)->type == eNet_game_type_2 && spec->mode_data.field_0x14 == 1)) {
                AddSmashableRaceTarget(pActor->model, pActor, -1);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x004f5cb0, ProcessSmashableActorModel)

intptr_t C2_HOOK_CDECL ProcessModelsCB(br_actor* pActor, void* data) {
    unsigned int x;
    unsigned int z;
    tTrack_spec* pTrack_spec = data;

    if (c2_sscanf(pActor->identifier, "%u%u", &x, &z) == 2 && x < pTrack_spec->ncolumns_x && z < pTrack_spec->ncolumns_z) {
        pActor->material = C2V(gDefault_track_material);
        pTrack_spec->columns[z][x].actor_0x0 = BrActorAllocate(BR_ACTOR_NONE, NULL);
        BrActorAdd(pActor->parent, pTrack_spec->columns[z][x].actor_0x0);
        pTrack_spec->columns[z][x].actor_0x4 = pActor;
        pTrack_spec->columns[z][x].actor_0x8 = NULL;
        BrActorRelink(pTrack_spec->columns[z][x].actor_0x0, pActor);
        C2V(gMr_blendy) = NULL;
        if (pActor->model != NULL && !C2V(gAusterity_mode)) {
            StripBlendedFaces(pActor, pActor->model);
        } else {
            br_actor *child;

            for (child = pActor->children; child != NULL; child = child->next) {
                if (child->identifier[0] != '&') {
                    StripBlendedFaces(child, child->model);
                    break;
                }
            }
        }
        BrActorEnum(pActor, (br_actor_enum_cbfn*)FindNonCarsCB, pTrack_spec);
        if (C2V(gMr_blendy) != NULL) {
            int area1, area2;
            br_vertex* new_vertices;
            br_face* new_faces;

            BrActorAdd(pTrack_spec->columns[z][x].actor_0x0, C2V(gMr_blendy));
            C2V(gMr_blendy)->model->flags |= BR_MODU_FACES;
            BrModelAdd(C2V(gMr_blendy)->model);
            GetModelTextureArea(C2V(gMr_blendy)->model, &area1, &area2);

            new_vertices = BrResAllocate(C2V(gMr_blendy)->model,
                (C2V(gMr_blendy)->model->nvertices + area2) * sizeof(br_vertex), BR_MEMORY_VERTICES);
            c2_memcpy(new_vertices, C2V(gMr_blendy)->model->vertices,
                C2V(gMr_blendy)->model->nvertices * sizeof(br_vertex));
            BrResFree(C2V(gMr_blendy)->model->vertices);
            C2V(gMr_blendy)->model->vertices = new_vertices;

            new_faces = BrResAllocate(C2V(gMr_blendy)->model,
                (C2V(gMr_blendy)->model->nfaces + area1) * sizeof(br_face), BR_MEMORY_FACES);
            c2_memcpy(new_faces, C2V(gMr_blendy)->model->faces, C2V(gMr_blendy)->model->nfaces * sizeof(br_face));
            BrResFree(C2V(gMr_blendy)->model->faces);
            C2V(gMr_blendy)->model->faces = new_faces;

            FixModelPointer(C2V(gMr_blendy)->model, BR_MODU_ALL);
            pTrack_spec->columns[z][x].actor_0x8 = C2V(gMr_blendy);
        }
    } else {
        BrActorEnum(pActor, ProcessModelsCB, pTrack_spec);
    }
    ProcessSmashableActorModel(pActor);
    return 0;
}
C2_HOOK_FUNCTION(0x0040cf10, ProcessModelsCB)
