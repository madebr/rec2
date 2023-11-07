#include "spark.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSplash, gSplash, 20, 0x006a82b8);

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
            AdaptMaterialsForRenderer(&material, 1, kRendererShadingType_AmbientOnly);
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
            num_files = LoadTextureTryAllLocations(the_path, &splash_maps[C2V(gNum_splash_types)], REC2_ASIZE(splash_maps - C2V(gNum_splash_types)));
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

void (C2_HOOK_FASTCALL * LoadInKevStuff_original)(FILE* pF);
void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadInKevStuff_original(pF);
#else
    ApplyPreviousTiffConversion();
    PossibleService();
    LoadInShrapnel();
    PossibleService();
    InitShrapnel();
    PossibleService();
    InitFlame();
    PossibleService();
    InitSplash(pF);
    ApplyTopTiffConversion();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fdc10, LoadInKevStuff, LoadInKevStuff_original)

void (C2_HOOK_FASTCALL * DisposeKevStuff_original)(void);
void C2_HOOK_FASTCALL DisposeKevStuff(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposeKevStuff_original();
#else
#error "Not iplemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fe180, DisposeKevStuff, DisposeKevStuff_original)
