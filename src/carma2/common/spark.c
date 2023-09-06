#include "spark.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSmoke_on, 0x00660110, 1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gShade_list, 16, 0x006b7840);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int*, gShade_table, 0x00660140, &gShade_list[8]); /* FIXME: rename to gDust_table*/
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_dust_tables, 0x006a82b4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_model*, gShrapnel_model, 2, 0x006aa588);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gBlack_material, 0x006b7880);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tShrapnel, gShrapnel, 30, 0x006a9180);

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

void (C2_HOOK_FASTCALL * LoadInKevStuff_original)(FILE* pF);
void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF) {

#if defined(C2_HOOKS_ENABLED)
    LoadInKevStuff_original(pF);
#else
#error "Not implemented"
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
