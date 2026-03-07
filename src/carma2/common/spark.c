#include "spark.h"

#include "depth.h"
#include "52-errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "replay.h"
#include "trig.h"
#include "utility.h"

#include "rec2_macros.h"

#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x00660110
int gSmoke_on = 1;

// GLOBAL: CARMA2_HW 0x006b7840
int gShade_list[16];

// GLOBAL: CARMA2_HW 0x00660140
int* gShade_table = &gShade_list[8]; /* FIXME: rename to gDust_table*/

// GLOBAL: CARMA2_HW 0x006a82b4
int gNum_dust_tables;

// GLOBAL: CARMA2_HW 0x006aa588
br_model* gShrapnel_model[2];

// GLOBAL: CARMA2_HW 0x006b7880
br_material* gBlack_material;

// GLOBAL: CARMA2_HW 0x006a9180
tShrapnel gShrapnel[30];

// GLOBAL: CARMA2_HW 0x006aa59c
int gColumn_flags;

// GLOBAL: CARMA2_HW 0x006aa380
br_model* gLollipop_model;

// GLOBAL: CARMA2_HW 0x006a8638
br_pixelmap* gFlame_map[20];

// GLOBAL: CARMA2_HW 0x006a96a8
tSmoke_column gSmoke_column[10];

// GLOBAL: CARMA2_HW 0x006aa570
int gSplash_flags;

// GLOBAL: CARMA2_HW 0x006a8758
br_model* gSplash_model;

// GLOBAL: CARMA2_HW 0x006aa5a4
int gNum_splash_types;

// GLOBAL: CARMA2_HW 0x006a9130
br_material* gSplash_material[20];

// GLOBAL: CARMA2_HW 0x006a82b8
tSplash gSplash[32];

// GLOBAL: CARMA2_HW 0x006aa57c
int gSpark_flags;

// GLOBAL: CARMA2_HW 0x006aa5ac
br_pixelmap* gIt_shade_table;

// GLOBAL: CARMA2_HW 0x006aa568
int gDust_rotate;

// GLOBAL: CARMA2_HW 0x006aa388
tReplay_splash gReplay_splashes[10];

// GLOBAL: CARMA2_HW 0x006aa574
int gReplay_splash_flags;

// GLOBAL: CARMA2_HW 0x006b7884
tU32 gNext_AFE_color_cycle;

// GLOBAL: CARMA2_HW 0x006aa5b0
int gAFE_color_cycle;

// GLOBAL: CARMA2_HW 0x006aa584
int gShrapnel_flags;

// GLOBAL: CARMA2_HW 0x006a82b0
br_camera* gSpark_cam;

// GLOBAL: CARMA2_HW 0x006a8718
br_matrix4 gCameraToScreen;

// GLOBAL: CARMA2_HW 0x006a9b80
tSpark gSparks[32];

// GLOBAL: CARMA2_HW 0x006a8760
tBRender_smoke* gBR_smoke_pointers[35];

// GLOBAL: CARMA2_HW 0x006aa56c
int gN_BR_smoke_structs;

// GLOBAL: CARMA2_HW 0x006a8ce0
tSmoke gSmoke[25];

// GLOBAL: CARMA2_HW 0x006aa590
int gSmoke_flags;

#define CHARS1_TO_INT(A)            ((A) - '0')
#define CHARS2_TO_INT(A, B)         (10 * CHARS1_TO_INT(A) + CHARS1_TO_INT(B))
#define CHARS3_TO_INT(A, B, C)      (10 * CHARS2_TO_INT(A, B) + CHARS1_TO_INT(C))
#define CHARS4_TO_INT(A, B, C, D)   (10 * CHARS3_TO_INT(A, B, C) + CHARS1_TO_INT(D))

// FUNCTION: CARMA2_HW 0x004fc9d0
void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    gSmoke_on = pSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fca30
int C2_HOOK_FASTCALL GetSmokeOn(void) {

    return gSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fb910
void C2_HOOK_FASTCALL GenerateSmokeShades(void) {
    static int rb = 0x00;
    static int gb = 0x00;
    static int bb = 0x00;

    static int rl = 0x40;
    static int gl = 0x40;
    static int bl = 0x40;

    static int rs = 0xc8;
    static int gs = 0xc8;
    static int bs = 0xc8;

    static int rm = 0x80;
    static int gm = 0x80;
    static int bm = 0x80;

    static int rd = 0xfe;
    static int gd = 0xdf;
    static int bd = 0x43;

#define RGB(R, G, B) (((R) << 16) | ((G) << 8) | ((B) << 0))

    gShade_list[0] = RGB(rb, gb, bb);
    gShade_list[1] = RGB(rl, gl, bl);
    gShade_list[3] = RGB(rs, gs, bs);
    gShade_list[4] = gShade_list[3];
    gShade_list[2] = RGB(rm, gm, bm);
    gShade_list[7] = RGB(rd, gd, bd);

#undef RGB

}

// FUNCTION: CARMA2_HW 0x004fdb70
void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* f) {
    int i;
    int red;
    int green;
    int blue;
    float quarter;
    float half;
    float three_quarter;

    gNum_dust_tables = GetAnInt(f);
    if (gNum_dust_tables > 8) {
        gNum_dust_tables = 8;
    }
    for (i = 0; i < gNum_dust_tables; i++) {
        PossibleService();
        GetThreeInts(f, &red, &green, &blue);
        GetThreeScalars(f, &quarter, &half, &three_quarter);
        gShade_table[i] = (red << 24) | (green << 8) | (blue << 0);
    }
}

// FUNCTION: CARMA2_HW 0x00516240
void C2_HOOK_FASTCALL ModelScale(br_model* pModel, float pScale) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {

        BrVector3Scale(&pModel->vertices[i].p, &pModel->vertices[i].p, pScale);
    }
    BrModelUpdate(pModel, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x005162a0
float C2_HOOK_FASTCALL DistanceFromFaceND(const br_vector3* pP, const br_vector3* pN, br_scalar pF) {

    return BrVector3Dot(pP, pN) - pF;
}

// FUNCTION: CARMA2_HW 0x004f87a0
void C2_HOOK_FASTCALL ResetSparks(void) {

    gSpark_flags = 0;
}

void C2_HOOK_FASTCALL LoadInShrapnel(void) {

    gShrapnel_model[0] = LoadModel("FRAG4.DAT");
    ModelScale(gShrapnel_model[0], 0.5f);
    gShrapnel_model[1] = LoadModel("FRAG5.DAT");
    ModelScale(gShrapnel_model[1], 0.5f);
    BrModelAdd(gShrapnel_model[0]);
    BrModelAdd(gShrapnel_model[1]);
    gBlack_material = GetSimpleMaterial("M14.MAT", 4);
    gNon_track_actor->material = gBlack_material;
}

void C2_HOOK_FASTCALL InitShrapnel(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(gShrapnel); i++) {
        gShrapnel[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gShrapnel[i].actor->parent = NULL;
        gShrapnel[i].actor->model = gShrapnel_model[i % 2];
        gShrapnel[i].actor->render_style = BR_RSTYLE_DEFAULT;
        gShrapnel[i].actor->t.type = BR_TRANSFORM_MATRIX34;
        gShrapnel[i].actor->material = BrMaterialFind("DEBRIS.MAT");
        gShrapnel[i].age = 0;
        gShrapnel[i].shear1 = FRandomBetween(-2.f, 2.f);
        gShrapnel[i].shear2 = FRandomBetween(-2.f, 2.f);
        BrVector3SetFloat(&gShrapnel[i].axis,
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f));
        BrVector3Normalise(&gShrapnel[i].axis, &gShrapnel[i].axis);
    }
}

// FUNCTION: CARMA2_HW 0x004fc3a0
void C2_HOOK_FASTCALL InitFlame(void) {
    int i;
    int j;
    char name[256];
    br_actor* actor;
    br_material* material;

    gColumn_flags = 0;
    gLollipop_model = BrModelAllocate("Lollipop", 4, 2);
    strcpy(name, "FLAMES.PIX");
    strcpy(gCurrent_load_directory, "COMMON");
    if (LoadTextureTryAllLocations(name, gFlame_map, REC2_ASIZE(gFlame_map)) != REC2_ASIZE(gFlame_map)) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, name);
    }
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlame_map) != 20);
    BrMapAddMany(gFlame_map, REC2_ASIZE(gFlame_map));

    C2_HOOK_BUG_ON(sizeof(tSmoke_column) != 124);
    C2_HOOK_BUG_ON(REC2_ASIZE(gSmoke_column) != 10);
    for (i = 0; i < REC2_ASIZE(gSmoke_column); i++) {

        gSmoke_column[i].flame_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        for (j = 0; j < REC2_ASIZE(gSmoke_column[i].frame_count); j++) {
            actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            material = BrMaterialAllocate(NULL);
            BrActorAdd(gSmoke_column[i].flame_actor, actor);
            actor->model = gLollipop_model;
            actor->material = material;
            material->flags &= ~BR_MATF_LIGHT;
            material->flags |= BR_MATF_ALWAYS_VISIBLE;
            material->colour_map = gFlame_map[0];
            GlorifyMaterial(&material, 1, kRendererShadingType_AmbientOnly);
            BrMaterialAdd(material);
            gSmoke_column[i].frame_count[j] = 100;
        }
    }
    gLollipop_model->nvertices = 4;
    BrVector3SetFloat(&gLollipop_model->vertices[0].p, -.5f, 0.f, .0f);
    BrVector3SetFloat(&gLollipop_model->vertices[1].p, .5f, 0.f, .0f);
    BrVector3SetFloat(&gLollipop_model->vertices[2].p, .5f, 1.f, .0f);
    BrVector3SetFloat(&gLollipop_model->vertices[3].p, -.5f, 1.f, .0f);
    BrVector2Set(&gLollipop_model->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gLollipop_model->vertices[3].map, 0.f, 0.f);

    gLollipop_model->nfaces = 2;
    gLollipop_model->faces[0].vertices[0] = 0;
    gLollipop_model->faces[0].vertices[1] = 1;
    gLollipop_model->faces[0].vertices[2] = 2;
    gLollipop_model->faces[1].vertices[0] = 0;
    gLollipop_model->faces[1].vertices[1] = 2;
    gLollipop_model->faces[1].vertices[2] = 3;
    gLollipop_model->faces[0].smoothing = 1;
    gLollipop_model->faces[1].smoothing = 1;
    BrModelAdd(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fdde0
void C2_HOOK_FASTCALL InitSplash(FILE* pF) {
    int i;
    int num_files;
    int num;
    br_actor* actor;
    char the_path[256];
    char s[256];
    br_pixelmap* splash_maps[20];

    gSplash_flags = 0;
    gSplash_model = BrModelAllocate("Splash", 4, 2);
    strcpy(gCurrent_load_directory, "COMMON");
    if (pF != NULL) {
        num = GetAnInt(pF);
        gNum_splash_types = 0;
        for (i = 0; i < num; i++) {
            GetAString(pF, s);
#if 0
            PathCat(the_path, the_path, s);
#else
            strcpy(the_path, s);
#endif
            num_files = LoadTextureTryAllLocations(the_path, &splash_maps[gNum_splash_types], REC2_ASIZE(splash_maps));
            if (num_files == 0) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, the_path);
            }
            gNum_splash_types += num_files;
        }
    } else {
        strcpy(the_path, "SPLSHBLU.PIX");
        gNum_splash_types = LoadTextureTryAllLocations(the_path, splash_maps, REC2_ASIZE(splash_maps));
    }
    BrMapAddMany(splash_maps, gNum_splash_types);
    for (i = 0; i < gNum_splash_types; i++) {
        gSplash_material[i] = BrMaterialAllocate(NULL);
        gSplash_material[i]->flags &= ~BR_MATF_LIGHT;
        gSplash_material[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        gSplash_material[i]->colour_map = splash_maps[i];
        BrMaterialAdd(gSplash_material[i]);
    }
    gSplash_model->nvertices = 4;
    BrVector3SetFloat(&gSplash_model->vertices[0].p, -0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[1].p,  0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[2].p,  0.5f, 1.0f, 0.0f);
    BrVector3SetFloat(&gSplash_model->vertices[3].p, -0.5f, 1.0f, 0.0f);
    BrVector2Set(&gSplash_model->vertices[0].map, 0.0f, 1.0f);
    BrVector2Set(&gSplash_model->vertices[1].map, 1.0f, 1.0f);
    BrVector2Set(&gSplash_model->vertices[2].map, 1.0f, 0.0f);
    BrVector2Set(&gSplash_model->vertices[3].map, 0.0f, 0.0f);
    gSplash_model->nfaces = 2;
    gSplash_model->faces[0].vertices[0] = 0;
    gSplash_model->faces[0].vertices[1] = 1;
    gSplash_model->faces[0].vertices[2] = 2;
    gSplash_model->faces[1].vertices[0] = 0;
    gSplash_model->faces[1].vertices[1] = 2;
    gSplash_model->faces[1].vertices[2] = 3;
    gSplash_model->faces[0].smoothing = 1;
    gSplash_model->faces[1].smoothing = 1;
    BrModelAdd(gSplash_model);
    for (i = 0; i < REC2_ASIZE(gSplash); i++) {
        gSplash[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        actor = gSplash[i].actor;
        actor->model = gSplash_model;
        if (gNum_splash_types != 0) {
            actor->material = gSplash_material[IRandomBetween(0, gNum_splash_types - 1)];
        } else {
            actor->material = NULL;
        }
        gSplash[i].scale_x = SRandomBetween(0.9f, 1.1f) * (2 * IRandomBetween(0, 1) - 1);
    }
}

// FUNCTION: CARMA2_HW 0x004fdc00
void C2_HOOK_FASTCALL FreeSmokeShadeTables(void) {

}

// FUNCTION: CARMA2_HW 0x004fdc10
void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF) {

    PackFileRevertTiffLoading();
    PossibleService();
    LoadInShrapnel();
    PossibleService();
    InitShrapnel();
    PossibleService();
    InitFlame();
    PossibleService();
    InitSplash(pF);
    PackFileRerevertTiffLoading();
}

// FUNCTION: CARMA2_HW 0x004fe180
void C2_HOOK_FASTCALL DisposeKevStuff(void) {

    NOT_IMPLEMENTED();
}

void (C2_HOOK_FASTCALL * StopCarBeingIt_original)(tCar_spec* pCar);
// FUNCTION: CARMA2_HW 0x004fe570
void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL MungeSplash(tU32 pTime) {

    if (gNum_splash_types == 0) {
        return;
    }
    if (gAction_replay_mode && ARGetReplayRate() != 0.f) {
        tVehicle_type type;
        int i;

        for (type = eVehicle_net_player; type <= eVehicle_rozzer; type++) {

            for (i = 0; i < (type == eVehicle_self ? 1 : GetCarCount(type)); i++) {
                tCar_spec* car;

                if (type == eVehicle_self) {
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(type, i);
                }
                if (car->collision_info->water_d != 10000.f && !(car != NULL && car->driver == eDriver_local_human)) {
                    CreateSplash(car->collision_info, pTime);
                }
            }
        }
        if (gProgram_state.current_car.collision_info->water_d != 10000.f) {
            CreateSplash(gProgram_state.current_car.collision_info, pTime);
        }
        for (i = 0; i < REC2_ASIZE(gReplay_splashes); i++) {
            tReplay_splash *replay_splash = &gReplay_splashes[i];

            if (gReplay_splash_flags & (1 << i)) {
                switch (replay_splash->type) {
                case 1:
                    {
                        const char* ident = replay_splash->object.actor->identifier;
                        tPipe_chunk_data *chunk_non_car = FindNextChunk(ePipe_chunk_non_car, CHARS4_TO_INT(ident[4], ident[5], ident[6], ident[7]));
                        if (chunk_non_car != NULL) {
                            br_vector3 vel;
                            br_vector3 rot;

                            GetVelocitiesFromMatrices(&vel, &rot, &replay_splash->object.actor->t.t.mat, &chunk_non_car->data.non_car.matrix, pTime);
                            GeneralCreateSplash(NULL, &vel, &rot, &replay_splash->normal, replay_splash->water_density, &replay_splash->bb, &replay_splash->object.actor->t.t.mat, pTime);
                        }
                    }
                    break;
                case 2:
                    {
                        int id = (replay_splash->object.ped - gPedestrian_array) | (replay_splash->object.ped->field_0x06 << 16);
                        br_actor* actor = GetCharacterActorPtr(replay_splash->object.ped->character);
                        tPipe_chunk_data *chunk_ped_physics = FindNextChunk(ePipe_chunk_ped_physics, id);
                        if (chunk_ped_physics != NULL) {
                            br_vector3 vel;
                            br_vector3 rot;

                            GetVelocitiesFromMatrices(&vel, &rot, &actor->t.t.mat, &chunk_ped_physics->data.ped_physics.matrix, pTime);
                            GeneralCreateSplash(NULL, &vel, &rot, &replay_splash->normal, replay_splash->water_density, &replay_splash->bb, &replay_splash->object.actor->t.t.mat, pTime);
                        }
                    }
                    break;
                case 255:
                    {
                        tPipe_chunk_data *chunk_ped_phil_object = FindNextChunk(ePipe_chunk_phil_object, (uintptr_t)replay_splash->object.phil_object);
                        if (chunk_ped_phil_object != NULL) {
                            br_vector3 vel;
                            br_vector3 rot;

                            GetVelocitiesFromMatrices(&vel, &rot, &replay_splash->object.actor->t.t.mat, &chunk_ped_phil_object->data.phil_object.matrix, pTime);
                            GeneralCreateSplash(NULL, &vel, &rot, &replay_splash->normal, replay_splash->water_density, &replay_splash->bb, &replay_splash->object.actor->t.t.mat, pTime);
                        }
                    }
                    break;
                }
            }
        }
    } else if (!gAction_replay_mode) {
        tCollision_info* object;

        for (object = PHILGetFirstObject(); object != NULL; object = PHILGetNextObject(object)) {

            if (object->water_d != 10000.f && object->last_special_volume != NULL && object->last_special_volume->gravity_multiplier < 1.f) {
                if (object != NULL && object->owner != NULL && object->flags_0x238 == 1 && object->owner != NULL && ((tCar_spec*)object->owner)->driver == eDriver_local_human) {
                    CreateSplash(object, 100);
                } else {
                    CreateSplash(object, pTime);
                }
            }
        }
    }
    if (gSplash_flags) {
        int i;

        for (i = 0; i < REC2_ASIZE(gSplash); i++) {
            tSplash *splash = &gSplash[i];

            if (!(gSplash_flags & (1 << i))) {
                continue;
            }
            if (splash->just_done || (gAction_replay_mode && ARGetReplayRate() == 0.f)) {
                br_scalar dt;

                dt = splash->size * splash->scale_x;
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[0], (br_vector3*)gCamera_to_world.m[0], dt);
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[1], (br_vector3*)gCamera_to_world.m[1], splash->size);
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[2], (br_vector3*)gCamera_to_world.m[2], splash->size);
                if (gProgram_state.cockpit_on) {
                    br_scalar ts;

                    ts = sqrtf(REC2_SQR(gCamera_to_world.m[0][2]) + REC2_SQR(gCamera_to_world.m[0][0]));
                    DRMatrix34PreRotateZ(&splash->actor->t.t.mat, -FastScalarArcTan2Angle(gCamera_to_world.m[0][1], ts));
                }
                splash->just_done = 0;
            } else {
                gSplash_flags &= ~(1 << i);
                if (splash->actor->parent != NULL) {
                    BrActorRemove(splash->actor);
                }
            }
        }
    }
}

void C2_HOOK_FASTCALL ReplayShrapnel(tU32 pTime) {
    int i;
    br_matrix34* mat;

    for (i = 0; i < REC2_ASIZE(gShrapnel); i++) {
        mat = &gShrapnel[i].actor->t.t.mat;
        if (gShrapnel_flags & (1 << i)) {
            gShrapnel[i].age += (tU32)(ARGetReplayRate() * (float)pTime);
            DrMatrix34Rotate(mat, gShrapnel[i].age * BR_ANGLE_DEG(1), &gShrapnel[i].axis);
            BrMatrix34PreShearX(mat, gShrapnel[i].shear1, gShrapnel[i].shear2);
        }
    }
}

void C2_HOOK_FASTCALL KillShrapnel(int i) {

    if (gShrapnel[i].actor->parent != NULL) {
        BrActorRemove(gShrapnel[i].actor);
    }
    gShrapnel_flags &= ~(1 << i);
}

// FUNCTION: CARMA2_HW 0x004f9790
void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime) {

    MungeSmokeColumn(pTime);
    MungeSplash(pTime);

    if (gAFE) {
        tU32 now = GetTotalTime();
        if (now - gNext_AFE_color_cycle > 100) {
            gAFE_color_cycle = 1;
            gNext_AFE_color_cycle = now;
        } else {
            gAFE_color_cycle = 0;
        }
    }
    if (gAction_replay_mode) {
        ReplayShrapnel(pTime);
    } else {
        int i;

        ARStartPipingSession(ePipe_chunk_shrapnel);
        for (i = 0; i < REC2_ASIZE(gShrapnel); i++) {
            br_matrix34* mat;
            br_scalar ts;

            mat = &gShrapnel[i].actor->t.t.mat;
            if (!(gShrapnel_flags & (1 << i))) {
                continue;
            }
            if (gShrapnel[i].age == -1) {
                KillShrapnel(i);
            } else {
                br_vector3 disp;

                if (gShrapnel[i].time_sync != 0) {
                    BrVector3Scale(&disp, &gShrapnel[i].v, (float)gShrapnel[i].time_sync / 1000.f);
                    gShrapnel[i].time_sync = 0;
                } else {
                    BrVector3Scale(&disp, &gShrapnel[i].v, pTime / 1000.0f);
                    gShrapnel[i].age += pTime;
                }
                BrVector3Accumulate((br_vector3*)mat->m[3], &disp);
                gShrapnel[i].v.v[1] -= (float)(10 * pTime) / 1000.f / WORLD_SCALE; /* Fall acceleration */
                DrMatrix34Rotate(mat, BR_ANGLE_DEG(gShrapnel[i].age), &gShrapnel[i].axis);
                BrMatrix34PreShearX(mat, gShrapnel[i].shear1, gShrapnel[i].shear2);
                ts = 1.f - BrVector3Length(&gShrapnel[i].v) / 1.4f * (float)pTime / 1000.f;
                if (ts < .1f) {
                    ts = .1f;
                }
                BrVector3Scale(&gShrapnel[i].v, &gShrapnel[i].v, ts);
                AddShrapnelToPipingSession(i + ((gShrapnel[i].age > 1000 || pTime > gShrapnel[i].age) << 15), (br_vector3*)mat->m[3], gShrapnel[i].age - pTime, gShrapnel[i].actor->material);
                if (gShrapnel[i].age > 1000) {
                    gShrapnel[i].age = -1;
                }
            }
        }
        AREndPipingSession();
    }
}

// FUNCTION: CARMA2_HW 0x004fb9c0
void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void) {

    if (gIt_shade_table == NULL) {
        gIt_shade_table = GenerateDarkenedShadeTable(16, gRender_palette, 0, 255, 254, .25f, .5f, .75f, .6f);
    }
}

void C2_HOOK_FASTCALL ForEveryModelMaterial(br_model* pModel, tMaterialMaybeUpdate_cbfn* pCallback) {
    int i;
    int need_update;

    if (pModel == NULL) {
        return;
    }
    need_update = 0;
    for (i = 0; i < pModel->prepared->ngroups; i++) {
        br_material *material = pModel->faces[pModel->prepared->groups[i].face_user[0]].material;

        if (material != NULL) {
            need_update |= pCallback(material);
        }
    }
    if (need_update) {
        BrModelUpdate(pModel, BR_MODU_PRIMITIVE_COLOURS);
    }
}

// FUNCTION: CARMA2_HW 0x004fe8a0
intptr_t C2_HOOK_CDECL ForEveryActorMaterial(br_actor* pActor, void *pContext) {
    tUser_crush_data* user_crush_data;
    tMaterialMaybeUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (pActor->material != NULL) {
        callback(pActor->material);
    }
    ForEveryModelMaterial(pActor->model, callback);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fe930
intptr_t C2_HOOK_CDECL ForEveryActorMaterialNoGrooves(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush_data;
    tMaterialMaybeUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (user_crush_data->groove != NULL) {
        return 0;
    }
    if (pActor->material != NULL) {
        callback(pActor->material);
    }
    ForEveryModelMaterial(pActor->model, callback);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fe640
void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves) {
#if 0
    int i;
#endif

    tUser_crush_data* user_crush_data = pCar_spec->car_model_actor->user;
    NOT_IMPLEMENTED();

    if (pCar_spec->car_master_actor->material != NULL) {
        pCallback(pCar_spec->car_master_actor->material);
    }
    ForEveryModelMaterial(pCar_spec->shell_model, pCallback);
    ForEveryModelMaterial(user_crush_data->models[1], pCallback);
    if (pGrooves) {
        DRActorEnumRecurse(pCar_spec->car_model_actor, ForEveryActorMaterial, pCallback);
    } else {
        DRActorEnumRecurse(pCar_spec->car_model_actor, ForEveryActorMaterialNoGrooves, pCallback);
    }
#if 0
    /* FIXME: incomplete */
    for (i = 0; i < pCar_spec->car_crush_spec->field_0x270; i++) {
        ForEveryModelMaterial(pCar_spec->car_crush_spec->field_0x274[i].actor->model, pCallback);
    }
    for (i = 0; i < pCar_spec->car_crush_spec->field_0x2b0; i++) {
        ForEveryModelMaterial(pCar_spec->car_crush_spec->field_0x2b4[i].actor->model, pCallback);
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004fed50
intptr_t C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial) {

    if (GetBlendificatiousnessOfMaterial(pMaterial) == 100) {
        return 0;
    }
    BlendifyMaterial(pMaterial, 100);
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004fed40
void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec) {

    ForEveryCarMaterial(pCar_spec, UnBlendifyMaterialCB, 1);
}

// FUNCTION: CARMA2_HW 0x004fed00
intptr_t C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial) {

    if (GetBlendificatiousnessOfMaterial(pMaterial) == 25) {
        return 0;
    }
    BlendifyMaterial(pMaterial, 25);
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004fecf0
void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar_spec) {

    ForEveryCarMaterial(pCar_spec, BlendifyMaterialCB, 1);
}

// FUNCTION: CARMA2_HW 0x004fca70
void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar_spec) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fc740
void C2_HOOK_FASTCALL PipeInstantUnSmudge(tCar_spec* pCar_spec) {
    br_model* model;
    br_model* b_model;
    br_actor* actor;
    br_actor* bonny;
    int j;
    int n;
    int v;
    int group;
    tSmudged_vertex data[1000];

    if (!gAction_replay_mode) {
        return;
    }
    actor = pCar_spec->car_actor;
    model = actor->model;
    bonny = pCar_spec->car_actor;
    n = 0;
    if ((model->flags & BR_MODF_KEEP_ORIGINAL) || (model->flags & BR_MODF_UPDATEABLE)) {
        ARStartPipingSession(ePipe_chunk_smudge);
        j = 0;
        for (group = 0; group < model->prepared->ngroups; group++) {
            for (v = 0; v < model->prepared->groups[group].nvertices; v++) {
                if ((model->prepared->groups[group].vertex_colours[v] >> 24) != 0) {
                    data[n].vertex_index = j;
                    data[n].light_index = -((tS32)(model->prepared->groups[group].vertex_colours[v] >> 24));
                    n += 1;
                    model->prepared->groups[group].vertex_colours[v] = 0;
                    if (model->flags & BR_MODF_UPDATEABLE) {
                        model->vertices[model->prepared->groups[group].vertex_user[v]].index = 0;
                    }
                    if (n >= REC2_ASIZE(data)) {
                        group = model->prepared->ngroups;
                        break;
                    }
                }
                j += 1;
            }
        }
        if (n != 0) {
            AddSmudgeToPipingSession(pCar_spec->car_ID, 0, n, data);
        }
        if (bonny != actor) {
            /* This branch is never taken */
            b_model = bonny->model;
            n = 0;
            j = 0;
            for (group = 0; group < b_model->prepared->ngroups; group++) {
                for (v = 0; v < b_model->prepared->groups[group].nvertices; v++) {
                    if ((b_model->prepared->groups[group].vertex_colours[v] >> 24) != 0) {
                        data[n].vertex_index = j;
                        data[n].light_index = -b_model->prepared->groups[group].nvertices;
                        n += 1;
                        b_model->prepared->groups[group].vertex_colours[v] = 0;
                        if (b_model->flags & BR_MODF_UPDATEABLE) {
                            b_model->vertices[b_model->prepared->groups[group].vertex_user[v]].index = 0;
                        }
                        if (n >= REC2_ASIZE(data)) {
                            group = b_model->prepared->groups[group].nvertices;
                            break;
                        }
                    }
                    j += 1;
                }
            }
            if (n != 0) {
                AddSmudgeToPipingSession(pCar_spec->car_ID, 0, n, data);
            }
        }
        AREndPipingSession();
    }
}

// FUNCTION: CARMA2_HW 0x004fca40
void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar && gSmoke_column[i].lifetime > 2000) {
            gSmoke_column[i].lifetime = 2000;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca90
void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tCollision_info* pObject) {
    size_t i;

    for (i = 0; i < REC2_ASIZE(gSmoke_column); i++) {

        if (gSmoke_column[i].core.collision_info == pObject) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fa5b0
void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime) {
    br_vector3 pos;
    br_vector3 v;
    br_vector3 vcs;
    br_vector3 tv;
    br_scalar decay_factor;
    br_scalar ts;
    br_scalar alpha;
    br_scalar beta;
    int colour;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, dust_time, 0x1284);

    if (pTime < (tU32)pCar->dust_time[pWheel]) {
        pCar->dust_time[pWheel] -= pTime;
        return;
    }
    pCar->dust_time[pWheel] += IRandomBetween(200, 400) - pTime;
    if (pCar->dust_time[pWheel] < 0) {
        pCar->dust_time[pWheel] = 0;
    }
    BrVector3Cross(&tv, &pCar->collision_info->omega, &pCar->wpos[pWheel]);
    BrVector3Scale(&vcs, &pCar->collision_info->velocity_car_space, WORLD_SCALE);
    BrVector3Accumulate(&vcs, &tv);
    ts = BrVector3LengthSquared(&vcs);
    if (ts < 25.f) {
        return;
    }
    decay_factor = sqrtf(ts) / 25.f;
    if (decay_factor > 1.f) {
        decay_factor = 1.f;
    }
    BrVector3InvScale(&tv, &pCar->wpos[pWheel], WORLD_SCALE);
    tv.v[1] -= pCar->oldd[pWheel] / WORLD_SCALE;

    alpha = -1000.0f;
    if (vcs.v[2] < 0.0f) {
        alpha = (pCar->collision_info->bb2.max.v[2] - tv.v[2]) / vcs.v[2];
    } else if (vcs.v[2] > 0.0f) {
        alpha = (pCar->collision_info->bb2.min.v[2] - tv.v[2]) / vcs.v[2];
    }

    beta = -1000.0f;
    if (vcs.v[0] < 0.0f) {
        beta = (pCar->collision_info->bb2.max.v[0] - tv.v[0]) / vcs.v[0];
    } else if (vcs.v[0] < 0.0f) {
        beta = (pCar->collision_info->bb2.min.v[0] - tv.v[0]) / vcs.v[0];
    }

    ts = MAX(alpha, beta);
    BrVector3Scale(&pos, &vcs, ts);
    BrVector3Accumulate(&tv, &pos);
    BrMatrix34ApplyP(&pos, &tv, &pCar->car_master_actor->t.t.mat);
    BrMatrix34ApplyV(&v, &vcs, &pCar->car_master_actor->t.t.mat);

    colour = gDust_rotate + gCurrent_race.material_modifiers[pCar->material_index[pWheel]].smoke_type - 2;
    while (colour >= gNum_dust_tables) {
        colour -= gNum_dust_tables;
    }
    CreatePuffOfSmoke(&pos, &v, decay_factor, 2 * decay_factor, colour + 8);
}

// FUNCTION: CARMA2_HW 0x004fb330
void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fee70
void C2_HOOK_FASTCALL ClearSplashes(tCollision_info* pObject) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fb630
void C2_HOOK_FASTCALL CreateSmokeColumn2(undefined4 pArg1, br_actor* pActor, tCar_spec* pCar, tCollision_info* pObject, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL ReplaySmokeColumn(tU32 pTime) {
    int i;

    for (i = 0; i < REC2_ASIZE(gSmoke_column); i++) {
        if (gColumn_flags & (1 << i)) {
            br_vector3 dummy;

            DoSmokeColumn(i, pTime, &dummy);
            if (gSmoke_column[i].colour == 0) {
                FlameAnimate(i, &gSmoke_column[i].pos, pTime);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fba40
void C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime) {
    int i;

    if (gColumn_flags == 0) {
        return;
    }
    if (gAction_replay_mode) {
        ReplaySmokeColumn(pTime);
        return;
    }

    gPHIL_mechanics_time_sync = 1;
    for (i = 0; i < REC2_ASIZE(gSmoke_column); i++) {
        if (!(gColumn_flags & (1 << i))) {
            continue;
        }
        if (gSmoke_column[i].lifetime >= pTime) {
            tCollision_info* object;
            tCar_spec* c;
            br_vector3 car_pos;

            gSmoke_column[i].lifetime -= pTime;
            object = gSmoke_column[i].core.collision_info;
            c = gSmoke_column[i].core.car;
            DoSmokeColumn(i, pTime, &car_pos);
            if (gSmoke_column[i].core.field_0x0 == 1 && c->car_crush_spec->field_0x4b8 != 0) {
                if (IsOnNonCarHalf(c, &car_pos)) {
                    gSmoke_column[i].lifetime = 0;
                }
            }
            if (gSmoke_column[i].colour == 0) {
                FlameAnimate(i, &gSmoke_column[i].pos, pTime);
                if (gSmoke_column[i].core.field_0x0 == 1) {
                    if (gSmoke_column[i].smudge_timer >= pTime) {
                        gSmoke_column[i].smudge_timer -= pTime;
                    } else {
                        gSmoke_column[i].smudge_timer += 2000;
                        SmudgeCar(c, gSmoke_column[i].vertex_index);
                        if (c->knackered) {
                            int plane;

                            plane = IRandomBetween(0, REC2_ASIZE(gSmoke_column[i].core.car->fire_vertex) - 1);
                            SmudgeCar(c, gSmoke_column[i].core.car->fire_vertex[plane]);
                        }
                    }
                }
            }
            gSmoke_column[i].time += pTime;
            if (gSmoke_column[i].time > 200) {
                br_vector3 pos;
                br_vector3 v;
                br_scalar decay_factor;

                gSmoke_column[i].time -= pTime;
                gSmoke_column[i].count += 1;
                pos.v[0] = gSmoke_column[i].pos.v[0] + SRandomBetween(-.03f, .03f);
                pos.v[1] = gSmoke_column[i].pos.v[1] + (gSmoke_column[i].colour == 0) * .05f;
                pos.v[2] = gSmoke_column[i].pos.v[2] + SRandomBetween(-.03f, .03f);
                if (!(gSmoke_column[i].whiter & 0x2) || IRandomBetween(0, 3)) {
                    if (gSmoke_column[i].whiter > 0) {
                        gSmoke_column[i].whiter = 2;
                    } else {
                        gSmoke_column[i].whiter = -2;
                    }
                } else {
                    gSmoke_column[i].whiter &= 0x1;
                }
                decay_factor = ((float)(gSmoke_column[i].whiter > 0) + 1.f) / 2.f;
                if (gSmoke_column[i].lifetime < 4000) {
                    decay_factor = gSmoke_column[i].lifetime * decay_factor / 4000.0f;
                }
                if (gSmoke_column[i].core.collision_info == NULL) {
                    BrVector3SetFloat(&v, 0.f, 2.8985507f, 0.f);
                } else {
                    BrVector3Cross(&v, &object->omega, &car_pos);
                    BrMatrix34ApplyV(&car_pos, &v, &object->actor->t.t.mat);
                    v.v[0] = car_pos.v[0] + WORLD_SCALE * object->v.v[0];
                    v.v[1] = car_pos.v[1] + WORLD_SCALE * object->v.v[1];
                    v.v[2] = car_pos.v[2] + WORLD_SCALE * object->v.v[2];
                }
                CreatePuffOfSmoke(&pos, &v, decay_factor, decay_factor, gSmoke_column[i].colour + 16);
            }
        } else {
            if (gSmoke_column[i].core.field_0x0 != 0) {
                ARStartPipingSession(ePipe_chunk_smoke_column);
                AddSmokeColumnToPipingSession(i, &gSmoke_column[i], gSmoke_column[i].vertex_index, gSmoke_column[i].colour);
                AREndPipingSession();
            }
            gColumn_flags &= ~(1 << i);
            if (gSmoke_column[i].colour == 0) {
                BrActorRemove(gSmoke_column[i].flame_actor);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fcbb0
void C2_HOOK_FASTCALL GeneralCreateSplash(tCollision_info* pObject, br_vector3* pVel,br_vector3* pOmega, br_vector3* pNormal, float pWater_density, br_bounds3* pBounds, br_matrix34* pMat, tU32 pTime) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fda40
void C2_HOOK_FASTCALL CreateSplash(tCollision_info* pObject, tU32 pTime) {

    GeneralCreateSplash(pObject, &pObject->v, &pObject->omega, &pObject->water_normal, pObject->water_d,
        &pObject->bb1, &pObject->actor->t.t.mat, pTime);
}

// FUNCTION: CARMA2_HW 0x004fda70
void C2_HOOK_FASTCALL GetVelocitiesFromMatrices(br_vector3* pVel, br_vector3* pRot, br_matrix34* pMat_now, br_matrix34* pMat_prev, tU32 pTime) {
    br_quat quat_now;
    br_quat quat_prev;
    br_quat quat_speed;
    br_scalar ts;

    ts = (float)pTime / 1000.f;
    BrVector3Sub(pVel, (br_vector3*)pMat_prev->m[3], (br_vector3*)pMat_now->m[3]);
    BrVector3InvScale(pVel, pVel, ts);
    BrMatrix34ToQuat(&quat_now, pMat_now);
    BrMatrix34ToQuat(&quat_prev, pMat_now);
    BrQuatInvert(&quat_now, &quat_now);
    BrQuatMul(&quat_speed, &quat_now, &quat_prev);
    pRot->v[0] = 2 * quat_speed.x / ts;
    pRot->v[1] = 2 * quat_speed.y / ts;
    pRot->v[2] = 2 * quat_speed.z / ts;
}

// FUNCTION: CARMA2_HW 0x004f9e80
void C2_HOOK_FASTCALL DrMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a) {
    br_scalar t;
    br_scalar s;
    br_scalar c;
    br_scalar txy;
    br_scalar txz;
    br_scalar tyz;
    br_scalar sx;
    br_scalar sy;
    br_scalar sz;

    s = FastScalarSinAngle(r);
    c = FastScalarCosAngle(r);
    t = 1.0f - c;
    txy = t * a->v[0] * a->v[1];
    txz = t * a->v[0] * a->v[2];
    tyz = t * a->v[1] * a->v[2];
    sx = a->v[0] * s;
    sy = a->v[1] * s;
    sz = a->v[2] * s;
    mat->m[0][0] = a->v[0] * a->v[0] * t + c;
    mat->m[0][1] = sz + txy;
    mat->m[0][2] = txz - sy;
    mat->m[1][0] = txy - sz;
    mat->m[1][1] = a->v[1] * a->v[1] * t + c;
    mat->m[1][2] = sx + tyz;
    mat->m[2][0] = sy + txz;
    mat->m[2][1] = tyz - sx;
    mat->m[2][2] = a->v[2] * a->v[2] * t + c;
}

// FUNCTION: CARMA2_HW 0x004fc0e0
void C2_HOOK_FASTCALL DoSmokeColumn(int i, tU32 pTime, br_vector3* pRet_car_pos) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fbdd0
void C2_HOOK_FASTCALL FlameAnimate(int c, br_vector3* pPos, tU32 pTime) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0042df70
int C2_HOOK_FASTCALL IsOnNonCarHalf(tCar_spec* pCar, br_vector3 *pPos) {
    tCar_crush_spec* car_crush;

    car_crush = pCar->car_crush_spec;
    return car_crush != NULL
        && car_crush->field_0x4b8 != 0
        && BrVector3Dot(&car_crush->field_0x548, pPos) - car_crush->field_0x554 < 0.f;
}

// FUNCTION: CARMA2_HW 0x004fc9c0
void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int fire_point) {

    /* empty */
}

// FUNCTION: CARMA2_HW 0x004f7cb0
void C2_HOOK_FASTCALL SetLineModelCols(tU8 pCol) {

    if (pCol != 0) {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0xff;
        gLine_model->vertices[0].blu = 0xff;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0xff;
    } else {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0x00;
        gLine_model->vertices[0].blu = 0x00;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0x00;
    }
    BrModelUpdate(gLine_model, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x004f6b80
int C2_HOOK_FASTCALL DrawLine3D(br_vector3* start, br_vector3* end, br_pixelmap* pScreen, br_pixelmap* pDepth_buffer, br_pixelmap* shade_table) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL SetWorldToScreen(br_pixelmap* pScreen) {
    br_matrix4 mat;
    br_matrix4 mat2;

    BrMatrix4Perspective(&mat, gSpark_cam->field_of_view, gSpark_cam->aspect, -gSpark_cam->hither_z, -gSpark_cam->yon_z);
    BrMatrix4Scale(&mat2, (float)(pScreen->width / 2), (float)(pScreen->height / 2), 1.0f);
    BrMatrix4Mul(&gCameraToScreen, &mat, &mat2);
}

void C2_HOOK_FASTCALL ReplaySparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, tU32 pTime) {
    int i;
    br_vector3 pos;
    br_vector3 o;
    br_vector3 p;
    br_vector3 tv;
    br_vector3 new_pos;

    for (i = 0; i < REC2_ASIZE(gSparks); i++) {
        if (gSpark_flags & (1 << i)) {
            if (gSparks[i].car != NULL) {
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
                BrMatrix34ApplyP(&tv, &o, &gSparks[i].car->car_master_actor->t.t.mat);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
                BrVector3Copy(&o, &tv);
                BrMatrix34ApplyP(&pos, &gSparks[i].pos, &gSparks[i].car->car_master_actor->t.t.mat);
            } else {
                BrVector3Copy(&pos, &gSparks[i].pos);
            }
            BrVector3Add(&o, &pos, &gSparks[i].length);
            if (gRendering_mirror) {
                BrVector3Sub(&tv, &pos, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&new_pos, &tv, &gRearview_camera_to_world);
                BrVector3Sub(&tv, &o, (br_vector3*)gRearview_camera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gRearview_camera_to_world);
            } else {
                BrVector3Sub(&tv, &pos, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&new_pos, &tv, &gCamera_to_world);
                BrVector3Sub(&tv, &o, (br_vector3*)gCamera_to_world.m[3]);
                BrMatrix34TApplyV(&p, &tv, &gCamera_to_world);
            }
            if (gNo_2d_effects) {
                SetLineModelCols(gSparks[i].colour);
            }
            if (gSparks[i].colour != 0) {
                DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gFog_shade_table);
            } else {
                DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gAcid_shade_table);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f7450
void C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    int i;
    int time;
    br_vector3 tv;
    br_vector3 o;
    br_vector3 p;
    br_vector3 pos;
    br_vector3 new_pos;
    br_scalar ts;

    C2_HOOK_BUG_ON(sizeof(tSpark) != 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, count, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, pos, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, v, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, length, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, normal, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, time_sync, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, car, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSpark, colour, 0x3c);

    gSpark_cam = pCamera->type_data;
    SetWorldToScreen(pRender_screen);

    if (gSpark_flags == 0) {
        return;
    }

    if (gNo_2d_effects) {
        BrActorRemove(gLine_actor);
        BrActorAdd(pCamera, gLine_actor);
    }

    if (gAction_replay_mode) {
        ReplaySparks(pRender_screen, pDepth_buffer, pCamera, pTime);
        if (gNo_2d_effects) {
            BrActorRemove(gLine_actor);
            BrActorAdd(gDont_render_actor, gLine_actor);
        }
        return;
    }
    ARStartPipingSession(ePipe_chunk_spark);
    for (i = 0; i < REC2_ASIZE(gSparks); i++) {
        if (!((1u << i) & gSpark_flags)) {
            continue;
        }
        if (gSparks[i].count <= 0) {
            gSparks[i].count = 0;
            gSpark_flags &= ~(1u << i);
        }
        ts = BrVector3Dot(&gSparks[i].normal, &gSparks[i].v);
        BrVector3Scale(&tv, &gSparks[i].normal, ts);
        BrVector3Sub(&gSparks[i].v, &gSparks[i].v, &tv);
        if (gSparks[i].time_sync) {
            BrVector3Scale(&o, &gSparks[i].v, gSparks[i].time_sync / 1000.f);
            gSparks[i].count += gSparks[i].time_sync - pTime;
            gSparks[i].time_sync = 0;
        } else {
            BrVector3Scale(&o, &gSparks[i].v, pTime / 1000.f);
            gSparks[i].count -= pTime;
        }
        BrVector3Accumulate(&gSparks[i].pos, &o);
        time = 1000 - gSparks[i].count;
        if (time > 150) {
            time = 150;
        }
        ts = -time / 1000.f;
        if (gSparks[i].colour) {
            ts /= 2.0;
        }
        BrVector3Scale(&gSparks[i].length, &gSparks[i].v, ts);
        ts = pTime * 10.f / (WORLD_SCALE * 100.f);
        if (gSparks[i].car != NULL) {
            BrMatrix34ApplyV(&tv, &gSparks[i].length, &gSparks[i].car->car_master_actor->t.t.mat);
            BrVector3Copy(&gSparks[i].length, &tv);
            BrMatrix34ApplyP(&pos, &gSparks[i].pos, &gSparks[i].car->car_master_actor->t.t.mat);
            BrVector3Copy(&o, &tv);
            gSparks[i].v.v[0] -= gSparks[i].car->car_master_actor->t.t.mat.m[0][1] * ts;
            gSparks[i].v.v[1] -= gSparks[i].car->car_master_actor->t.t.mat.m[1][1] * ts;
            gSparks[i].v.v[2] -= gSparks[i].car->car_master_actor->t.t.mat.m[2][1] * ts;
        } else {
            BrVector3Copy(&pos, &gSparks[i].pos);
            gSparks[i].v.v[1] -= ts;
        }
        AddSparkToPipingSession(i + (gSparks[i].colour << 8), &pos, &gSparks[i].length);
        BrVector3Add(&o, &gSparks[i].length, &pos);
        if (gRendering_mirror) {
            BrVector3Sub(&tv, &pos, (br_vector3*)gRearview_camera_to_world.m[3]);
            BrMatrix34TApplyV(&new_pos, &tv, &gRearview_camera_to_world);
            BrVector3Sub(&tv, &o, (br_vector3*)gRearview_camera_to_world.m[3]);
            BrMatrix34TApplyV(&p, &tv, &gRearview_camera_to_world);
        } else {
            BrVector3Sub(&tv, &pos, (br_vector3*)gCamera_to_world.m[3]);
            BrMatrix34TApplyV(&new_pos, &tv, &gCamera_to_world);
            BrVector3Sub(&tv, &o, (br_vector3*)gCamera_to_world.m[3]);
            BrMatrix34TApplyV(&p, &tv, &gCamera_to_world);
        }
        BrVector3SetFloat(&tv, FRandomBetween(-0.1f, 0.1f), FRandomBetween(-0.1f, 0.1f), FRandomBetween(-0.1f, 0.1f));
        BrVector3Accumulate(&gSparks[i].v, &tv);
        ts = 1.0f - BrVector3Length(&gSparks[i].v) / 1.4f * pTime / 1000.0f;
        if (ts < 0.1f) {
            ts = 0.1f;
        }
        BrVector3Scale(&gSparks[i].v, &gSparks[i].v, ts);
        if (gNo_2d_effects) {
            SetLineModelCols(gSparks[i].colour);
        }
        if (gSparks[i].colour != 0) {
            DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gFog_shade_table);
        } else {
            DrawLine3D(&p, &new_pos, pRender_screen, pDepth_buffer, gAcid_shade_table);
        }
    }
    AREndPipingSession();
    if (gNo_2d_effects) {
        BrActorRemove(gLine_actor);
        BrActorAdd(gDont_render_actor, gLine_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004fb2f0
int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2) {
    tBRender_smoke** a;
    tBRender_smoke** b;

    a = (tBRender_smoke**)p1;
    b = (tBRender_smoke**)p2;
    if ((*a)->pos.v[2] < (*b)->pos.v[2]) {
        return -1;
    } else if ((*a)->pos.v[2] > (*b)->pos.v[2]) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fb1b0
void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void) {
    int i;
    int j;
    tBRender_smoke* smoke;
    tU8 red;
    tU8 grn;
    tU8 blu;

    BrQsort(gBR_smoke_pointers, gN_BR_smoke_structs, sizeof(void*), CmpSmokeZ);

    for (i = 0; i < gN_BR_smoke_structs; i++) {
        smoke = gBR_smoke_pointers[gN_BR_smoke_structs - 1 - i];
        BrVector3Copy(&gBlend_actor->t.t.translate.t, &smoke->pos);
        gBlend_actor->t.t.mat.m[0][0] = smoke->r;
        gBlend_actor->t.t.mat.m[1][1] = smoke->r / smoke->aspect;
        gBlend_actor->material = smoke->material;
        gBlend_actor->material->extra_prim[1].v.x = BrFloatToFixed(smoke->strength * 150.f);
        BrMaterialUpdate(gBlend_actor->material, BR_MATU_EXTRA_PRIM);

        red = BR_RED(smoke->col);
        grn = BR_GRN(smoke->col);
        blu = BR_BLU(smoke->col);
        for (j = 0; j < smoke->model->nvertices; j++) {
            smoke->model->vertices[j].red = red;
            smoke->model->vertices[j].grn = grn;
            smoke->model->vertices[j].blu = blu;
        }
        BrModelUpdate(smoke->model, BR_MODU_VERTEX_COLOURS);
        gBlend_actor->model = smoke->model;
        BrZbsSceneRenderAdd(gBlend_actor);
    }
}

void C2_HOOK_FASTCALL RecordSmokeCircle(br_vector3* pCent, br_scalar pR, br_scalar pStrength, br_colour pShade, br_scalar pAspect) {

    if (gRendering_mirror) {
        DRMatrix34TApplyP(&gBR_smoke_structs[gN_BR_smoke_structs].pos, pCent, &gRearview_camera_to_world);
    } else {
        DRMatrix34TApplyP(&gBR_smoke_structs[gN_BR_smoke_structs].pos, pCent, &gCamera_to_world);
    }

    gBR_smoke_structs[gN_BR_smoke_structs].r = pR;
    gBR_smoke_structs[gN_BR_smoke_structs].strength = pStrength;
    gBR_smoke_structs[gN_BR_smoke_structs].col = pShade;
    gBR_smoke_structs[gN_BR_smoke_structs].aspect = pAspect;
    gBR_smoke_pointers[gN_BR_smoke_structs] = &gBR_smoke_structs[gN_BR_smoke_structs];
    if (pShade == gShade_list[7]) {
        gBR_smoke_structs[gN_BR_smoke_structs].model = gBlend_model2;
    } else {
        gBR_smoke_structs[gN_BR_smoke_structs].model = gBlend_model;
    }
    gN_BR_smoke_structs += 1;
}

void C2_HOOK_FASTCALL SmokeCircle3D(br_vector3* o, br_scalar r, br_scalar strength, br_scalar pAspect, br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_colour pShade, br_actor* pCamera) {

    if (gNo_2d_effects) {
        RecordSmokeCircle(o, r, strength, pShade, pAspect);
    }
}

void C2_HOOK_FASTCALL DrawTheGlow(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera) {
    int i;
    br_scalar strength;
    br_vector3 tv;
    tU32 seed;

    if (gColumn_flags != 0) {
        seed = rand();
        srand(GetTotalTime());
        for (i = 0; i < REC2_ASIZE(gSmoke_column); i++) {
            if (((1u << i) & gColumn_flags) != 0 && gSmoke_column[i].colour <= 1) {
                strength = 0.5f;
                if (gSmoke_column[i].lifetime < 4000) {
                    strength = gSmoke_column[i].lifetime * 0.5f / 4000.f;
                }
                BrVector3Set(&tv, gSmoke_column[i].pos.v[0], gSmoke_column[i].pos.v[1] + 0.02f, gSmoke_column[i].pos.v[2]);
                SmokeCircle3D(&tv, 0.07f, strength, SRandomBetween(0.5f, 0.99f), pRender_screen, pDepth_buffer, gShade_list[7], pCamera);
            }
        }
        srand(seed);
    }
}

void C2_HOOK_FASTCALL ReplaySmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera) {
    br_scalar aspect;
    int i;

    for (i = 0; i < REC2_ASIZE(gSmoke); i++) {
        if (gSmoke_flags & (1u << i)) {
            aspect = 1.f + (gSmoke[i].radius - .05f) * 4.f / 2.f;
            if (gSmoke[i].type & 0x10) {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius / aspect, gSmoke[i].strength, 1.f,
                    pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            } else {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius, gSmoke[i].strength, aspect,
                    pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fa910
void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    int i;
    int j;
    br_vector3 tv;
    br_scalar aspect;
    br_scalar ts;
    tU32 seed;
    tU32 not_lonely;

    not_lonely = 0;
    if (gNo_2d_effects) {
        gBlend_actor->render_style = BR_RSTYLE_FACES;
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(pCamera, gBlend_actor);
        gN_BR_smoke_structs = 0;
    }
    DrawTheGlow(pRender_screen, pDepth_buffer, pCamera);

    if (gSmoke_flags == 0) {
        if (gNo_2d_effects) {
            if (gBlend_actor->parent != NULL) {
                BrActorRemove(gBlend_actor);
            }
            BrActorAdd(gDont_render_actor, gBlend_actor);
        }
        return;
    }

    seed = rand();
    if (gAction_replay_mode) {
        ReplaySmoke(pRender_screen, pDepth_buffer, pCamera);
        srand(seed);
        if (gNo_2d_effects) {
            RenderRecordedSmokeCircles();
            if (gBlend_actor->parent != NULL) {
                BrActorRemove(gBlend_actor);
            }
            BrActorAdd(gDont_render_actor, gBlend_actor);
        }
        return;
    }
    ARStartPipingSession(ePipe_chunk_smoke);
    for (i = 0; i < REC2_ASIZE(gSmoke); i++) {
        if (gSmoke_flags & (1u << i)) {
            if (gSmoke[i].strength > 0.0) {
                if (gSmoke[i].time_sync != 0) {
                    BrVector3Scale(&tv, &gSmoke[i].v, gSmoke[i].time_sync / 1000.0f);
                    gSmoke[i].time_sync = 0;
                } else {
                    BrVector3Scale(&tv, &gSmoke[i].v, pTime / 1000.0f);
                }
                BrVector3Accumulate(&gSmoke[i].pos, &tv);
            } else {
                gSmoke_flags &= ~(1u << i);
            }
        }
    }
    for (i = 0; i < REC2_ASIZE(gSmoke); i++) {
        if (gSmoke_flags & (1u << i)) {
            if ((gSmoke[i].type & 0xf) == 7) {
                not_lonely |= 1u << i;
            } else if (!(not_lonely & (1u << i))) {
                for (j = i + 1; j < REC2_ASIZE(gSmoke); j++) {
                    if ((gSmoke_flags & (1u << j))) {
                        BrVector3Sub(&tv, &gSmoke[i].pos, &gSmoke[i].pos);
                        ts = BrVector3LengthSquared(&tv);
                        if (REC2_SQR(gSmoke[i].radius + gSmoke[j].radius) > ts) {
                            not_lonely |= (1u << j) | (1u << i);
                            break;
                        }
                    }
                }
            }
            if (!((1u << i) & not_lonely)) {
                gSmoke[i].strength = gSmoke[i].strength / 2.f;
            }
            aspect = 1.f + (gSmoke[i].radius - .05f) * 4.f / 2.f;
            if ((gSmoke[i].type & 0x10)) {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius / aspect, gBR_smoke_structs[i].strength, 1.0, pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            } else {
                SmokeCircle3D(&gSmoke[i].pos, gSmoke[i].radius, gBR_smoke_structs[i].strength, aspect, pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
            }
            if (gSmoke[i].pipe_me) {
                AddSmokeToPipingSession(i, gSmoke[i].type, &gSmoke[i].pos, gSmoke[i].radius, gSmoke[i].strength);
            }
            gSmoke[i].radius = pTime / 1000.0f * gSmoke[i].strength * .5f + gSmoke[i].radius;
            gSmoke[i].strength -= pTime * gSmoke[i].decay_factor / 1000.0f;
            if (gSmoke[i].radius > 0.3f) {
                gSmoke[i].radius = 0.3f;
            }
            if (gSmoke[i].strength > 0.0f) {
                ts = 1.0f - pTime * 0.002f;
                if (ts < 0.5f) {
                    ts = 0.5f;
                }
                BrVector3Scale(&gSmoke[i].v, &gSmoke[i].v, ts);
                if (fabs(gSmoke[i].v.v[1]) < 0.43478259f && (gSmoke[i].type & 0xfu) < 7) {
                    if (gSmoke[i].v.v[1] >= 0.0) {
                        gSmoke[i].v.v[1] = 0.43478259f;
                    } else {
                        gSmoke[i].v.v[1] += 0.43478259f;
                    }
                }
            } else {
                gSmoke_flags &= ~(1u << i);
            }
        }
    }
    AREndPipingSession();
    srand(seed);
    if (gNo_2d_effects) {
        RenderRecordedSmokeCircles();
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(gDont_render_actor, gBlend_actor);
    }
}
