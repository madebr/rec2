#include "spark.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "piping.h"
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

void (C2_HOOK_FASTCALL * MungeShrapnel_original)(tU32 pTime);
void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    MungeShrapnel_original(pTime);
#else
    NOT_IMPLEMENTED();
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

void (C2_HOOK_FASTCALL * CreatePuffOfSmoke_original)(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);
void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType) {

#if defined(C2_HOOKS_ENABLED)
    CreatePuffOfSmoke_original(pPos, pV, pStrength, pDecay_factor, pType);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fb330, CreatePuffOfSmoke, CreatePuffOfSmoke_original)
