#include "spark.h"

#include "errors.h"
#include "globvars.h"
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

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSmoke_on, 0x00660110, 1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gShade_list, 16, 0x006b7840);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int*, gShade_table, 0x00660140, &gShade_list[8]); /* FIXME: rename to gDust_table*/
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_dust_tables, 0x006a82b4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_model*, gShrapnel_model, 2, 0x006aa588);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gBlack_material, 0x006b7880);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tShrapnel, gShrapnel, 30, 0x006a9180);
C2_HOOK_VARIABLE_IMPLEMENT(int, gColumn_flags, 0x006aa59c);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gLollipop_model, 0x006aa380);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gFlame_map, 20, 0x006a8638);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmoke_column, gSmoke_column, 10, 0x006a96a8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSplash_flags, 0x006aa570);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gSplash_model, 0x006a8758);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_splash_types, 0x006aa5a4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gSplash_material, 20, 0x006a9130);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSplash, gSplash, 32, 0x006a82b8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSpark_flags, 0x006aa57c);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gIt_shade_table, 0x006aa5ac);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDust_rotate, 0x006aa568);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tReplay_splash, gReplay_splashes, 10, 0x006aa388);
C2_HOOK_VARIABLE_IMPLEMENT(int, gReplay_splash_flags, 0x006aa574);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gNext_AFE_color_cycle, 0x006b7884);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAFE_color_cycle, 0x006aa5b0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gShrapnel_flags, 0x006aa584);

#define CHARS1_TO_INT(A)            ((A) - '0')
#define CHARS2_TO_INT(A, B)         (10 * CHARS1_TO_INT(A) + CHARS1_TO_INT(B))
#define CHARS3_TO_INT(A, B, C)      (10 * CHARS2_TO_INT(A, B) + CHARS1_TO_INT(C))
#define CHARS4_TO_INT(A, B, C, D)   (10 * CHARS3_TO_INT(A, B, C) + CHARS1_TO_INT(D))

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    C2V(gSmoke_on) = pSmoke_on;
}
C2_HOOK_FUNCTION(0x004fc9d0, SetSmokeOn)

int C2_HOOK_FASTCALL GetSmokeOn(void) {

    return C2V(gSmoke_on);
}
C2_HOOK_FUNCTION(0x004fca30, GetSmokeOn)

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

    C2V(gShade_list)[0] = RGB(rb, gb, bb);
    C2V(gShade_list)[1] = RGB(rl, gl, bl);
    C2V(gShade_list)[3] = RGB(rs, gs, bs);
    C2V(gShade_list)[4] = C2V(gShade_list)[3];
    C2V(gShade_list)[2] = RGB(rm, gm, bm);
    C2V(gShade_list)[7] = RGB(rd, gd, bd);

#undef RGB

}
C2_HOOK_FUNCTION(0x004fb910, GenerateSmokeShades)

void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* f) {
    int i;
    int red;
    int green;
    int blue;
    float quarter;
    float half;
    float three_quarter;

    C2V(gNum_dust_tables) = GetAnInt(f);
    if (C2V(gNum_dust_tables) > 8) {
        C2V(gNum_dust_tables) = 8;
    }
    for (i = 0; i < C2V(gNum_dust_tables); i++) {
        PossibleService();
        GetThreeInts(f, &red, &green, &blue);
        GetThreeScalars(f, &quarter, &half, &three_quarter);
        C2V(gShade_table)[i] = (red << 24) | (green << 8) | (blue << 0);
    }
}
C2_HOOK_FUNCTION(0x004fdb70, GetSmokeShadeTables)

void C2_HOOK_FASTCALL ModelScale(br_model* pModel, float pScale) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {

        BrVector3Scale(&pModel->vertices[i].p, &pModel->vertices[i].p, pScale);
    }
    BrModelUpdate(pModel, BR_MODU_ALL);
}
C2_HOOK_FUNCTION(0x00516240, ModelScale)

float C2_HOOK_FASTCALL DistanceFromFaceND(const br_vector3* pP, const br_vector3* pN, br_scalar pF) {

    return BrVector3Dot(pP, pN) - pF;
}
C2_HOOK_FUNCTION(0x005162a0, DistanceFromFaceND)

void C2_HOOK_FASTCALL ResetSparks(void) {

    C2V(gSpark_flags) = 0;
}
C2_HOOK_FUNCTION(0x004f87a0, ResetSparks)

void C2_HOOK_FASTCALL LoadInShrapnel(void) {

    C2V(gShrapnel_model)[0] = LoadModel("FRAG4.DAT");
    ModelScale(C2V(gShrapnel_model)[0], 0.5f);
    C2V(gShrapnel_model)[1] = LoadModel("FRAG5.DAT");
    ModelScale(C2V(gShrapnel_model)[1], 0.5f);
    BrModelAdd(C2V(gShrapnel_model)[0]);
    BrModelAdd(C2V(gShrapnel_model)[1]);
    C2V(gBlack_material) = GetSimpleMaterial("M14.MAT", 4);
    C2V(gNon_track_actor)->material = C2V(gBlack_material);
}

void C2_HOOK_FASTCALL InitShrapnel(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gShrapnel)); i++) {
        C2V(gShrapnel)[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        C2V(gShrapnel)[i].actor->parent = NULL;
        C2V(gShrapnel)[i].actor->model = C2V(gShrapnel_model)[i % 2];
        C2V(gShrapnel)[i].actor->render_style = BR_RSTYLE_DEFAULT;
        C2V(gShrapnel)[i].actor->t.type = BR_TRANSFORM_MATRIX34;
        C2V(gShrapnel)[i].actor->material = BrMaterialFind("DEBRIS.MAT");
        C2V(gShrapnel)[i].age = 0;
        C2V(gShrapnel)[i].shear1 = FRandomBetween(-2.f, 2.f);
        C2V(gShrapnel)[i].shear2 = FRandomBetween(-2.f, 2.f);
        BrVector3SetFloat(&C2V(gShrapnel)[i].axis,
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f));
        BrVector3Normalise(&C2V(gShrapnel)[i].axis, &C2V(gShrapnel)[i].axis);
    }
}

void C2_HOOK_FASTCALL InitFlame(void) {
    int i;
    int j;
    char name[256];
    br_actor* actor;
    br_material* material;

    C2V(gColumn_flags) = 0;
    C2V(gLollipop_model) = BrModelAllocate("Lollipop", 4, 2);
    c2_strcpy(name, "FLAMES.PIX");
    c2_strcpy(C2V(gCurrent_load_directory), "COMMON");
    if (LoadTextureTryAllLocations(name, C2V(gFlame_map), REC2_ASIZE(C2V(gFlame_map))) != REC2_ASIZE(C2V(gFlame_map))) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, name);
    }
    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gFlame_map)) != 20);
    BrMapAddMany(C2V(gFlame_map), REC2_ASIZE(C2V(gFlame_map)));

    C2_HOOK_BUG_ON(sizeof(tSmoke_column) != 124);
    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gSmoke_column)) != 10);
    for (i = 0; i < REC2_ASIZE(C2V(gSmoke_column)); i++) {

        C2V(gSmoke_column)[i].flame_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        for (j = 0; j < REC2_ASIZE(C2V(gSmoke_column)[i].frame_count); j++) {
            actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            material = BrMaterialAllocate(NULL);
            BrActorAdd(C2V(gSmoke_column)[i].flame_actor, actor);
            actor->model = C2V(gLollipop_model);
            actor->material = material;
            material->flags &= ~BR_MATF_LIGHT;
            material->flags |= BR_MATF_ALWAYS_VISIBLE;
            material->colour_map = C2V(gFlame_map)[0];
            GlorifyMaterial(&material, 1, kRendererShadingType_AmbientOnly);
            BrMaterialAdd(material);
            C2V(gSmoke_column)[i].frame_count[j] = 100;
        }
    }
    C2V(gLollipop_model)->nvertices = 4;
    BrVector3SetFloat(&C2V(gLollipop_model)->vertices[0].p, -.5f, 0.f, .0f);
    BrVector3SetFloat(&C2V(gLollipop_model)->vertices[1].p, .5f, 0.f, .0f);
    BrVector3SetFloat(&C2V(gLollipop_model)->vertices[2].p, .5f, 1.f, .0f);
    BrVector3SetFloat(&C2V(gLollipop_model)->vertices[3].p, -.5f, 1.f, .0f);
    BrVector2Set(&C2V(gLollipop_model)->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&C2V(gLollipop_model)->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&C2V(gLollipop_model)->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&C2V(gLollipop_model)->vertices[3].map, 0.f, 0.f);

    C2V(gLollipop_model)->nfaces = 2;
    C2V(gLollipop_model)->faces[0].vertices[0] = 0;
    C2V(gLollipop_model)->faces[0].vertices[1] = 1;
    C2V(gLollipop_model)->faces[0].vertices[2] = 2;
    C2V(gLollipop_model)->faces[1].vertices[0] = 0;
    C2V(gLollipop_model)->faces[1].vertices[1] = 2;
    C2V(gLollipop_model)->faces[1].vertices[2] = 3;
    C2V(gLollipop_model)->faces[0].smoothing = 1;
    C2V(gLollipop_model)->faces[1].smoothing = 1;
    BrModelAdd(C2V(gLollipop_model));
}
C2_HOOK_FUNCTION(0x004fc3a0, InitFlame)

void C2_HOOK_FASTCALL InitSplash(FILE* pF) {
    int i;
    int num_files;
    int num;
    br_actor* actor;
    char the_path[256];
    char s[256];
    br_pixelmap* splash_maps[20];

    C2V(gSplash_flags) = 0;
    C2V(gSplash_model) = BrModelAllocate("Splash", 4, 2);
    c2_strcpy(C2V(gCurrent_load_directory), "COMMON");
    if (pF != NULL) {
        num = GetAnInt(pF);
        C2V(gNum_splash_types) = 0;
        for (i = 0; i < num; i++) {
            GetAString(pF, s);
#if 0
            PathCat(the_path, the_path, s);
#else
            c2_strcpy(the_path, s);
#endif
            num_files = LoadTextureTryAllLocations(the_path, &splash_maps[C2V(gNum_splash_types)], REC2_ASIZE(splash_maps));
            if (num_files == 0) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, the_path);
            }
            C2V(gNum_splash_types) += num_files;
        }
    } else {
        c2_strcpy(the_path, "SPLSHBLU.PIX");
        C2V(gNum_splash_types) = LoadTextureTryAllLocations(the_path, splash_maps, REC2_ASIZE(splash_maps));
    }
    BrMapAddMany(splash_maps, C2V(gNum_splash_types));
    for (i = 0; i < C2V(gNum_splash_types); i++) {
        C2V(gSplash_material)[i] = BrMaterialAllocate(NULL);
        C2V(gSplash_material)[i]->flags &= ~BR_MATF_LIGHT;
        C2V(gSplash_material)[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        C2V(gSplash_material)[i]->colour_map = splash_maps[i];
        BrMaterialAdd(C2V(gSplash_material)[i]);
    }
    C2V(gSplash_model)->nvertices = 4;
    BrVector3SetFloat(&C2V(gSplash_model)->vertices[0].p, -0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&C2V(gSplash_model)->vertices[1].p,  0.5f, 0.0f, 0.0f);
    BrVector3SetFloat(&C2V(gSplash_model)->vertices[2].p,  0.5f, 1.0f, 0.0f);
    BrVector3SetFloat(&C2V(gSplash_model)->vertices[3].p, -0.5f, 1.0f, 0.0f);
    BrVector2Set(&C2V(gSplash_model)->vertices[0].map, 0.0f, 1.0f);
    BrVector2Set(&C2V(gSplash_model)->vertices[1].map, 1.0f, 1.0f);
    BrVector2Set(&C2V(gSplash_model)->vertices[2].map, 1.0f, 0.0f);
    BrVector2Set(&C2V(gSplash_model)->vertices[3].map, 0.0f, 0.0f);
    C2V(gSplash_model)->nfaces = 2;
    C2V(gSplash_model)->faces[0].vertices[0] = 0;
    C2V(gSplash_model)->faces[0].vertices[1] = 1;
    C2V(gSplash_model)->faces[0].vertices[2] = 2;
    C2V(gSplash_model)->faces[1].vertices[0] = 0;
    C2V(gSplash_model)->faces[1].vertices[1] = 2;
    C2V(gSplash_model)->faces[1].vertices[2] = 3;
    C2V(gSplash_model)->faces[0].smoothing = 1;
    C2V(gSplash_model)->faces[1].smoothing = 1;
    BrModelAdd(C2V(gSplash_model));
    for (i = 0; i < REC2_ASIZE(C2V(gSplash)); i++) {
        C2V(gSplash)[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        actor = C2V(gSplash)[i].actor;
        actor->model = C2V(gSplash_model);
        if (C2V(gNum_splash_types) != 0) {
            actor->material = C2V(gSplash_material)[IRandomBetween(0, C2V(gNum_splash_types) - 1)];
        } else {
            actor->material = NULL;
        }
        C2V(gSplash)[i].scale_x = SRandomBetween(0.9f, 1.1f) * (2 * IRandomBetween(0, 1) - 1);
    }
}
C2_HOOK_FUNCTION(0x004fdde0, InitSplash)

void C2_HOOK_FASTCALL FreeSmokeShadeTables(void) {

}
C2_HOOK_FUNCTION(0x004fdc00, FreeSmokeShadeTables)

void (C2_HOOK_FASTCALL * LoadInKevStuff_original)(FILE* pF);
void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadInKevStuff_original(pF);
#else
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fdc10, LoadInKevStuff, LoadInKevStuff_original)

void (C2_HOOK_FASTCALL * DisposeKevStuff_original)(void);
void C2_HOOK_FASTCALL DisposeKevStuff(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposeKevStuff_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fe180, DisposeKevStuff, DisposeKevStuff_original)

void (C2_HOOK_FASTCALL * StopCarBeingIt_original)(tCar_spec* pCar);
void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar) {

#if defined(C2_HOOKS_ENABLED)
    StopCarBeingIt_original(pCar);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION(0x004fe570, StopCarBeingIt)

void C2_HOOK_FASTCALL MungeSplash(tU32 pTime) {

    if (C2V(gNum_splash_types) == 0) {
        return;
    }
    if (C2V(gAction_replay_mode) && ARGetReplayRate() != 0.f) {
        tVehicle_type type;
        int i;

        for (type = eVehicle_net_player; type <= eVehicle_rozzer; type++) {

            for (i = 0; i < type == eVehicle_self ? 1 : GetCarCount(type); i++) {
                tCar_spec* car;

                if (type == eVehicle_self) {
                    car = &C2V(gProgram_state).current_car;
                } else {
                    car = GetCarSpec(type, i);
                }
                if (car->collision_info->water_d != 10000.f && !(car != NULL && car->driver == eDriver_local_human)) {
                    CreateSplash(car->collision_info, pTime);
                }
            }
        }
        if (C2V(gProgram_state).current_car.collision_info->water_d != 10000.f) {
            CreateSplash(C2V(gProgram_state).current_car.collision_info, pTime);
        }
        for (i = 0; i < REC2_ASIZE(C2V(gReplay_splashes)); i++) {
            tReplay_splash *replay_splash = &C2V(gReplay_splashes)[i];

            if (C2V(gReplay_splash_flags) & (1 << i)) {
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
                        int id = (replay_splash->object.ped - C2V(gPedestrian_array)) | (replay_splash->object.ped->field_0x06 << 16);
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
    } else if (!C2V(gAction_replay_mode)) {
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
    if (C2V(gSplash_flags)) {
        int i;

        for (i = 0; i < REC2_ASIZE(C2V(gSplash)); i++) {
            tSplash *splash = &C2V(gSplash)[i];

            if (!(C2V(gSplash_flags) & (1 << i))) {
                continue;
            }
            if (splash->just_done || (C2V(gAction_replay_mode) && ARGetReplayRate() == 0.f)) {
                br_scalar dt;

                dt = splash->size * splash->scale_x;
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[0], (br_vector3*)C2V(gCamera_to_world).m[0], dt);
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[1], (br_vector3*)C2V(gCamera_to_world).m[1], splash->size);
                BrVector3Scale((br_vector3*)splash->actor->t.t.mat.m[2], (br_vector3*)C2V(gCamera_to_world).m[2], splash->size);
                if (C2V(gProgram_state).cockpit_on) {
                    br_scalar ts;

                    ts = sqrtf(REC2_SQR(C2V(gCamera_to_world).m[0][2]) + REC2_SQR(C2V(gCamera_to_world.m[0][0])));
                    DRMatrix34PreRotateZ(&splash->actor->t.t.mat, -FastScalarArcTan2Angle(C2V(gCamera_to_world).m[0][1], ts));
                }
                splash->just_done = 0;
            } else {
                C2V(gSplash_flags) &= ~(1 << i);
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

    for (i = 0; i < REC2_ASIZE(C2V(gShrapnel)); i++) {
        mat = &C2V(gShrapnel)[i].actor->t.t.mat;
        if (C2V(gShrapnel_flags) & (1 << i)) {
            C2V(gShrapnel)[i].age += (tU32)(ARGetReplayRate() * (float)pTime);
            DrMatrix34Rotate(mat, C2V(gShrapnel)[i].age * BR_ANGLE_DEG(1), &C2V(gShrapnel)[i].axis);
            BrMatrix34PreShearX(mat, C2V(gShrapnel)[i].shear1, C2V(gShrapnel)[i].shear2);
        }
    }
}

void C2_HOOK_FASTCALL KillShrapnel(int i) {

    if (C2V(gShrapnel)[i].actor->parent != NULL) {
        BrActorRemove(gShrapnel[i].actor);
    }
    C2V(gShrapnel_flags) &= ~(1 << i);
}

void (C2_HOOK_FASTCALL * MungeShrapnel_original)(tU32 pTime);
void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungeShrapnel_original(pTime);
#else
    MungeSmokeColumn(pTime);
    MungeSplash(pTime);

    if (C2V(gAFE)) {
        tU32 now = GetTotalTime();
        if (now - C2V(gNext_AFE_color_cycle) > 100) {
            C2V(gAFE_color_cycle) = 1;
            C2V(gNext_AFE_color_cycle) = now;
        } else {
            C2V(gAFE_color_cycle) = 0;
        }
    }
    if (C2V(gAction_replay_mode)) {
        ReplayShrapnel(pTime);
    } else {
        int i;

        ARStartPipingSession(ePipe_chunk_shrapnel);
        for (i = 0; i < REC2_ASIZE(C2V(gShrapnel)); i++) {
            br_matrix34* mat;
            br_scalar ts;

            mat = &C2V(gShrapnel)[i].actor->t.t.mat;
            if (!(C2V(gShrapnel_flags) & (1 << i))) {
                continue;
            }
            if (C2V(gShrapnel)[i].age == -1) {
                KillShrapnel(i);
            } else {
                br_vector3 disp;

                if (C2V(gShrapnel)[i].time_sync != 0) {
                    BrVector3Scale(&disp, &C2V(gShrapnel)[i].v, (float)C2V(gShrapnel)[i].time_sync / 1000.f);
                    C2V(gShrapnel)[i].time_sync = 0;
                } else {
                    BrVector3Scale(&disp, &C2V(gShrapnel)[i].v, pTime / 1000.0f);
                    C2V(gShrapnel)[i].age += pTime;
                }
                BrVector3Accumulate((br_vector3*)mat->m[3], &disp);
                C2V(gShrapnel)[i].v.v[1] -= (float)(10 * pTime) / 1000.f / WORLD_SCALE; /* Fall acceleration */
                DrMatrix34Rotate(mat, BR_ANGLE_DEG(C2V(gShrapnel)[i].age), &C2V(gShrapnel)[i].axis);
                BrMatrix34PreShearX(mat, C2V(gShrapnel)[i].shear1, C2V(gShrapnel)[i].shear2);
                ts = 1.f - BrVector3Length(&C2V(gShrapnel)[i].v) / 1.4f * (float)pTime / 1000.f;
                if (ts < .1f) {
                    ts = .1f;
                }
                BrVector3Scale(&C2V(gShrapnel)[i].v, &C2V(gShrapnel)[i].v, ts);
                AddShrapnelToPipingSession(i + ((gShrapnel[i].age > 1000 || pTime > C2V(gShrapnel)[i].age) << 15), (br_vector3*)mat->m[3], C2V(gShrapnel)[i].age - pTime, C2V(gShrapnel)[i].actor->material);
                if (C2V(gShrapnel)[i].age > 1000) {
                    C2V(gShrapnel)[i].age = -1;
                }
            }
        }
        AREndPipingSession();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f9790, MungeShrapnel, MungeShrapnel_original)

void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void) {

    if (C2V(gIt_shade_table) == NULL) {
        C2V(gIt_shade_table) = GenerateDarkenedShadeTable(16, C2V(gRender_palette), 0, 255, 254, .25f, .5f, .75f, .6f);
    }
}
C2_HOOK_FUNCTION(0x004fb9c0, GenerateItFoxShadeTable)

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
C2_HOOK_FUNCTION(0x004fe8a0, ForEveryActorMaterial)

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
C2_HOOK_FUNCTION(0x004fe930, ForEveryActorMaterialNoGrooves)

void (C2_HOOK_FASTCALL * ForEveryCarMaterial_original)(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves);
void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves) {

#if defined(C2_HOOKS_ENABLED)
    ForEveryCarMaterial_original(pCar_spec, pCallback, pGrooves);
#else
    tUser_crush_data* user_crush_data = pCar_spec->car_model_actor->user;
#if 0
    int i;
#endif

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
    NOT_IMPLEMENTED();
#if 0
    /* FIXME: incomplete */
    for (i = 0; i < pCar_spec->car_crush_spec->field_0x270; i++) {
        ForEveryModelMaterial(pCar_spec->car_crush_spec->field_0x274[i].actor->model, pCallback);
    }
    for (i = 0; i < pCar_spec->car_crush_spec->field_0x2b0; i++) {
        ForEveryModelMaterial(pCar_spec->car_crush_spec->field_0x2b4[i].actor->model, pCallback);
    }
#endif
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fe640, ForEveryCarMaterial, ForEveryCarMaterial_original)

intptr_t C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial) {

    if (GetBlendificatiousnessOfMaterial(pMaterial) == 100) {
        return 0;
    }
    BlendifyMaterial(pMaterial, 100);
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
    return 1;
}
C2_HOOK_FUNCTION(0x004fed50, UnBlendifyMaterialCB)

void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec) {

    ForEveryCarMaterial(pCar_spec, UnBlendifyMaterialCB, 1);
}
C2_HOOK_FUNCTION(0x004fed40, UnBlendifyCar)

intptr_t C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial) {

    if (GetBlendificatiousnessOfMaterial(pMaterial) == 25) {
        return 0;
    }
    BlendifyMaterial(pMaterial, 25);
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
    return 1;
}
C2_HOOK_FUNCTION(0x004fed00, BlendifyMaterialCB)

void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar_spec) {

    ForEveryCarMaterial(pCar_spec, BlendifyMaterialCB, 1);
}
C2_HOOK_FUNCTION(0x004fecf0, BlendifyCar)

void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gSmoke_column)); i++) {
        if (C2V(gSmoke_column)[i].car == pCar_spec) {
            C2V(gSmoke_column)[i].lifetime = 0;
        }
    }
}
C2_HOOK_FUNCTION(0x004fca70, StopCarSmokingInstantly)

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

    if (!C2V(gAction_replay_mode)) {
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
C2_HOOK_FUNCTION(0x004fc740, PipeInstantUnSmudge)

void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gSmoke_column)); i++) {
        if (C2V(gSmoke_column)[i].car == pCar && C2V(gSmoke_column)[i].lifetime > 2000) {
            C2V(gSmoke_column)[i].lifetime = 2000;
        }
    }
}
C2_HOOK_FUNCTION(0x004fca40, StopCarSmoking)

void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tCollision_info* pObject) {
    size_t i;

    for (i = 0; i < REC2_ASIZE(C2V(gSmoke_column)); i++) {

        if (C2V(gSmoke_column)[i].collision_info == pObject) {
            C2V(gSmoke_column)[i].lifetime = 0;
        }
    }
}
C2_HOOK_FUNCTION(0x004fca90, StopObjectSmokingInstantly)

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

    colour = C2V(gDust_rotate) + C2V(gCurrent_race).material_modifiers[pCar->material_index[pWheel]].smoke_type - 2;
    while (colour >= C2V(gNum_dust_tables)) {
        colour -= C2V(gNum_dust_tables);
    }
    CreatePuffOfSmoke(&pos, &v, decay_factor, 2 * decay_factor, colour + 8);
}
C2_HOOK_FUNCTION(0x004fa5b0, GenerateContinuousSmoke)

void (C2_HOOK_FASTCALL * CreatePuffOfSmoke_original)(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);
void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType) {

#if defined(C2_HOOKS_ENABLED)
    CreatePuffOfSmoke_original(pPos, pV, pStrength, pDecay_factor, pType);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fb330, CreatePuffOfSmoke, CreatePuffOfSmoke_original)

void (C2_HOOK_FASTCALL * ClearSplashes_original)(tCollision_info* pObject);
void C2_HOOK_FASTCALL ClearSplashes(tCollision_info* pObject) {

#if defined(C2_HOOKS_ENABLED)
    ClearSplashes_original(pObject);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fee70, ClearSplashes, ClearSplashes_original)

void (C2_HOOK_FASTCALL * CreateSmokeColumn2_original)(undefined4 pArg1, br_actor* pActor, tCar_spec* pCar, tCollision_info* pObject, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7);
void C2_HOOK_FASTCALL CreateSmokeColumn2(undefined4 pArg1, br_actor* pActor, tCar_spec* pCar, tCollision_info* pObject, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7) {

#if defined(C2_HOOKS_ENABLED)
    CreateSmokeColumn2_original(pArg1, pActor, pCar, pObject, pArg5, pArg6, pArg7);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fb630, CreateSmokeColumn2, CreateSmokeColumn2_original)

void (C2_HOOK_FASTCALL * MungeSmokeColumn_original)(tU32 pTime);
void C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    MungeSmokeColumn_original(pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fba40, MungeSmokeColumn, MungeSmokeColumn_original)

void (C2_HOOK_FASTCALL * GeneralCreateSplash_original)(tCollision_info* pObject, br_vector3* pVel,br_vector3* pOmega, br_vector3* pNormal, float pWater_density, br_bounds3* pBounds, br_matrix34* pMat, tU32 pTime);
void C2_HOOK_FASTCALL GeneralCreateSplash(tCollision_info* pObject, br_vector3* pVel,br_vector3* pOmega, br_vector3* pNormal, float pWater_density, br_bounds3* pBounds, br_matrix34* pMat, tU32 pTime) {

#if defined(C2_OOKS_ENABLED)
    GeneralCreateSplash_original(pObject, pVel, pOmega, pNormal, pWater_density, pBounds, pMat, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fcbb0, GeneralCreateSplash, GeneralCreateSplash_original)

void C2_HOOK_FASTCALL CreateSplash(tCollision_info* pObject, tU32 pTime) {

    GeneralCreateSplash(pObject, &pObject->v, &pObject->omega, &pObject->water_normal, pObject->water_d,
        &pObject->bb1, &pObject->actor->t.t.mat, pTime);
}
C2_HOOK_FUNCTION(0x004fda40, CreateSplash)

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
C2_HOOK_FUNCTION(0x004fda70, GetVelocitiesFromMatrices)

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
C2_HOOK_FUNCTION(0x004f9e80, DrMatrix34Rotate)

void (C2_HOOK_FASTCALL * DoSmokeColumn_original)(int i, tU32 pTime, br_vector3* pRet_car_pos);
void C2_HOOK_FASTCALL DoSmokeColumn(int i, tU32 pTime, br_vector3* pRet_car_pos) {

#if defined(C2_HOOKS_ENABLED)
    DoSmokeColumn_original(i, pTime, pRet_car_pos);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fc0e0, DoSmokeColumn, DoSmokeColumn_original)
