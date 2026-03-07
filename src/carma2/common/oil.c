#include "oil.h"

#include "finteray.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "network.h"
#include "piping.h"
#include "utility.h"

#include <brender/brender.h>

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

    for (i = 0; i < REC2_ASIZE(gOil_pixie_names); i++) {
        gOil_pixies[i] = LoadPixelmap(gOil_pixie_names[i]);
        BrMapAdd(gOil_pixies[i]);
    }

    for (i = 0; i < REC2_ASIZE(gOily_spills); i++) {
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

    for (i = 0; i < REC2_ASIZE(gOily_spills); i++) {
        gOily_spills[i].actor->render_style = BR_RSTYLE_NONE;
        gOily_spills[i].car = NULL;
        gOily_spills[i].car_actor = NULL;
        gOily_spills[i].stop_time = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004a7460
void C2_HOOK_FASTCALL  SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel) {

    BrVector2Set(&pModel->vertices[0].p, -pOil->field_0x20, -pOil->field_0x20);
    BrVector2Set(&pModel->vertices[1].p,  pOil->field_0x20, -pOil->field_0x20);
    BrVector2Set(&pModel->vertices[2].p,  pOil->field_0x20,  pOil->field_0x20);
    BrVector2Set(&pModel->vertices[3].p, -pOil->field_0x20,  pOil->field_0x20);
    pOil->actor->render_style = BR_RSTYLE_FACES;
    BrMaterialUpdate(pOil->actor->material, BR_MATU_ALL);
    BrModelUpdate(pModel, BR_MODU_ALL);
}

void C2_HOOK_FASTCALL MungeOilsHeightAeGround(tOil_spill_info* pThe_spill) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, actor, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, position, 0x34);

    BrVector3Copy(&pThe_spill->actor->t.t.translate.t, &pThe_spill->position);
}

int C2_HOOK_FASTCALL OKToSpillOil(tOil_spill_info* pOil) {
    br_scalar distance;
    br_scalar mr_dotty;
    br_vector3 pos;
    br_vector3 v;
    br_vector3 ray_pos;
    br_vector3 ray_dir;
    br_vector3 normal;
    tCar_spec* car;
    int i;
    int face_count;
    int found_one;
    br_angle angle_to_rotate_by;
    tBounds kev_bounds;
    tFace_ref the_list[10];
    tFace_ref* face_ref;

    car = pOil->car;
    if (car != NULL && car->driver >= 7 && car->damage_units[eDamage_engine].damage_level < 99 && car->damage_units[eDamage_transmission].damage_level < 99) {
        return 0;
    }
    angle_to_rotate_by = IRandomBetween(0, 0xffff);
    BrVector3Set(&kev_bounds.original_bounds.min,
        -pOil->full_size,
        pOil->car_model->bounds.min.v[1] - 1.f / WORLD_SCALE,
        -pOil->full_size);
    BrVector3Set(&kev_bounds.original_bounds.max,
        pOil->full_size,
        pOil->car_model->bounds.max.v[1],
        pOil->full_size);
    BrMatrix34PreRotateY(&pOil->actor->t.t.mat, angle_to_rotate_by);
    kev_bounds.mat = &pOil->car_actor->t.t.mat;
    face_count = FindFacesInBox(&kev_bounds, the_list, REC2_ASIZE(the_list), NULL);
    BrVector3Set(&v, .0f, .2f, .0f);
    BrVector3Add(&ray_pos, &pOil->car_actor->t.t.translate.t, &v);
    if (ray_dir.v[1] * ray_dir.v[1] <= 2 * BR_SCALAR_EPSILON) {
        BrVector3Set(&ray_dir, 1.f, 0.f, 0.f);
    } else {
        BrVector3Set(&ray_dir, 0.f, 1.f, 0.f);
    }
    if (face_count == 0) {
        return 0;
    }
    found_one = 0;
    for (i = 0; i < face_count; i++) {
        face_ref = &the_list[i];
        if (!found_one) {
            CheckSingleFace(face_ref, &ray_pos, &ray_dir, &normal, &distance, &pos);
            if (distance < 100.f) {
                found_one = 1;
                BrVector3Copy(&pOil->actor->t.t.look_up.up, &normal);
                BrVector3Set(&v, 0.f, 0.f, 1.f);
                BrVector3Cross(&pOil->actor->t.t.look_up.look, &normal, &v);
                BrVector3Set(&v, -1.f, 0.f, 0.f);
                BrVector3Cross((br_vector3*)pOil->actor->t.t.mat.m[2], &normal, &v);
                BrVector3Copy(&pOil->position, &pos);
                BrMatrix34PreRotateY(&pOil->actor->t.t.mat, angle_to_rotate_by);
            }
        }
    }
    if (!found_one || normal.v[1] < .97f) {
        return 0;
    }
    for (i = 0; i < face_count; i++) {
        face_ref = &the_list[i];
        mr_dotty = BrVector3Dot(&face_ref->normal, &normal);
        if (mr_dotty < .98f && (mr_dotty > .8f || !NormalSideOfPlane(&pOil->actor->t.t.translate.t, &face_ref->normal, face_ref->d))) {
            return 0;
        }
    }
    return 1;
}

void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil) {

    BrVector3Copy(&pOil->actor->t.t.translate.t, &pOil->position);
}

// FUNCTION: CARMA2_HW 0x004a6e50
void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period) {
    int i;
    tU32 time;
    br_scalar grow_amount;
    br_scalar this_size;
    br_vector3 v;
    tNet_message* message;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, position, 0x34);
    /* FIXME: unconditional assert */
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.player, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.full_size, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.grow_rate, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.current_size, 0x28);

    time = GetTotalTime();
    for (i = 0; i < REC2_ASIZE(gOily_spills); i++) {
        br_model* the_model;
        tOil_spill_info* oil = &gOily_spills[i];

        if (oil->car_actor == NULL) {
            oil->actor->render_style = BR_RSTYLE_NONE;
        } else {
            the_model = oil->actor->model;
            if (oil->actor->render_style == BR_RSTYLE_NONE
                    && time >= oil->spill_time
                    && (oil->car == NULL || (fabsf(oil->car->collision_info->v.v[0]) < 1 / WORLD_SCALE / 100.f
                                          && fabsf(oil->car->collision_info->v.v[1]) < 1 / WORLD_SCALE / 100.f
                                          && fabsf(oil->car->collision_info->v.v[2]) < 1 / WORLD_SCALE / 100.f))) {
                if (gAction_replay_mode) {
                    SetInitialOilStuff(oil, the_model);
                } else {
                    if (!OKToSpillOil(oil)) {
                        oil->car = NULL;
                        oil->car_actor = NULL;
                    } else {
                        oil->spill_time = time;
                        if (gNext_oil_pixie > 0) { /* REC2_ASIZE(gOil_pixies) */
                            gNext_oil_pixie = 0;
                        }
                        if (oil->car == NULL) {
                            BrVector3Copy(&oil->original_pos, &oil->car_actor->t.t.translate.t);
                        } else {
                            BrVector3Copy(&oil->original_pos, &oil->car->pos);
                        }
                        PipeSingleOilSpill(i,
                            &oil->actor->t.t.mat,
                            oil->full_size,
                            oil->grow_rate,
                            oil->spill_time,
                            oil->stop_time,
                            oil->car,
                            &oil->original_pos,
                            oil->actor->material->colour_map);
                        oil->stop_time = 0;
                        SetInitialOilStuff(oil, the_model);
                        if (gNet_mode == eNet_mode_host && oil->car != NULL) {
                            message = NetBuildGuaranteedMessage(31, 0);
                            message->contents.oil_spill.player = NetPlayerFromCar(oil->car)->ID;
                            message->contents.oil_spill.full_size = oil->full_size;
                            message->contents.oil_spill.grow_rate = oil->grow_rate;
                            message->contents.oil_spill.current_size = oil->current_size;
                            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
                        }
                    }
                }
            } else {
                if (oil->actor->render_style == BR_RSTYLE_FACES
                        && (oil->stop_time == 0 || time < oil->stop_time)) {

                    int grow = 1;
                    if (oil->car != NULL) {
                        BrVector3Sub(&v, &oil->original_pos, &oil->car->pos);
                        grow_amount = BrVector3LengthSquared(&v);
                        grow = oil->stop_time == 0 || grow_amount <= 0.2f;
                    }
                    if (grow) {
                        this_size = oil->field_0x20 + (time - oil->spill_time) * oil->grow_rate;
                        if (this_size >= oil->field_0x20) {
                            oil->actor->render_style = BR_RSTYLE_FACES;
                            if (this_size <= oil->full_size) {
                                the_model->vertices[0].p.v[0] = -this_size;
                                the_model->vertices[0].p.v[2] = -this_size;
                                the_model->vertices[1].p.v[0] =  this_size;
                                the_model->vertices[1].p.v[2] = -this_size;
                                the_model->vertices[2].p.v[0] =  this_size;
                                the_model->vertices[2].p.v[2] =  this_size;
                                the_model->vertices[3].p.v[0] = -this_size;
                                the_model->vertices[3].p.v[2] =  this_size;
                                oil->current_size = this_size;
                            } else {
                                the_model->vertices[0].p.v[0] = -oil->full_size;
                                the_model->vertices[0].p.v[2] = -oil->full_size;
                                the_model->vertices[1].p.v[0] =  oil->full_size;
                                the_model->vertices[1].p.v[2] = -oil->full_size;
                                the_model->vertices[2].p.v[0] =  oil->full_size;
                                the_model->vertices[2].p.v[2] =  oil->full_size;
                                the_model->vertices[3].p.v[0] = -oil->full_size;
                                the_model->vertices[3].p.v[2] =  oil->full_size;
                                oil->current_size = oil->full_size;
                            }
                            BrModelUpdate(the_model, BR_MODU_ALL);
                        } else {
                            oil->actor->render_style = BR_RSTYLE_NONE;
                        }
                    } else {
                        oil->stop_time = time;
                        continue;
                    }
                }
            }
        }
        if (oil->actor->render_style == BR_RSTYLE_FACES) {
            MungeOilsHeightAboveGround(oil);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004a74e0
int C2_HOOK_FASTCALL GetOilSpillCount(void) {

    return REC2_ASIZE(gOily_spills);
}

// FUNCTION: CARMA2_HW 0x004a74f0
void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize) {

    if (gOily_spills[pIndex].car != NULL) {
        *pActor = gOily_spills[pIndex].actor;
        *pSize = gOily_spills[pIndex].full_size;
    } else {
        *pActor = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004a6e20
void C2_HOOK_FASTCALL MungeIndexedOilsHeightAboveGround(int pIndex) {

    MungeOilsHeightAboveGround(&gOily_spills[pIndex]);
}
