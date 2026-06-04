#include "02-init.h"

#include "01-network.h"
#include "04-netgame.h"
#include "05-drmem.h"
#include "07-structur.h"
#include "09-funks.h"
#include "10-loading2.h"
#include "13-crush1.h"
#include "14-drfile.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "20-skidmark.h"
#include "25-grafdata.h"
#include "27-powerup.h"
#include "28-world3.h"
#include "29-raycast.h"
#include "32-spark.h"
#include "33-depth.h"
#include "38-flicplay.h"
#include "39-oil.h"
#include "41-utility.h"
#include "49-physics.h"
#include "64-movie.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "50-fog.h"
#include "51-smash.h"
#include "52-errors.h"
#include "56-aiworld.h"
#include "61-pedestrn.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "66-tintedpoly.h"
#include "67-lighting.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrbm.h"
#include "globvrpb.h"
#include "platform.h"

#include "rec2_macros.h"

#include <string.h>
#include <time.h>

// GLOBAL: CARMA2_HW 0x0068be2c
int gBr_initialized;

// GLOBAL: CARMA2_HW 0x0068be34
int gInitialisation_finished;

// GLOBAL: CARMA2_HW 0x0074b74c
br_material* gDefault_track_material;

// GLOBAL: CARMA2_HW 0x0068be24
tU32 gAustere_time;

// GLOBAL: CARMA2_HW 0x0074b760
int gCredits_per_rank[3];

// GLOBAL: CARMA2_HW 0x0074b76c
int gInitial_rank;

// GLOBAL: CARMA2_HW 0x00762110
int gStarting_money[3];

// GLOBAL: CARMA2_HW 0x00762160
int gNet_starting_money[8];

// GLOBAL: CARMA2_HW 0x0074d4c0
tSlot_info gInitial_APO[3];

// GLOBAL: CARMA2_HW 0x007622a0
tSlot_info gInitial_APO_potential[3];

// GLOBAL: CARMA2_HW 0x0074cfa0
tSlot_info gMax_APO[3];


// Modify2DCopyPixelmaps

// SetupFFBValues

// AllocateActors

// AllocateCamera

// ReinitialiseForwardCamera

// AllocateRearviewPixelmap

// ReinitialiseRearviewCamera

// ReinitialiseRenderStuff

void C2_HOOK_FASTCALL InstallFindFailedHooks(void) {

}

// AllocateStandardLamp

#define InitializeBRenderEnvironment() \
    do { \
        gBr_initialized = 1; \
        SetBRenderScreenAndBuffers(0, 0, 0, 0); \
        gUniverse_actor = BrActorAllocate(BR_ACTOR_NONE, NULL); \
        if (gUniverse_actor == NULL) { \
            FatalError(kFatalError_FailToOpenGeneralSettings); \
        } \
        gUniverse_actor->identifier = BrResStrDup(gUniverse_actor, "Root"); \
        BrEnvironmentSet(gUniverse_actor); \
        gNon_track_actor = BrActorAllocate(BR_ACTOR_NONE, NULL); \
        if (gNon_track_actor == NULL) { \
            FatalError(kFatalError_FailToOpenGeneralSettings); \
        } \
        BrActorAdd(gUniverse_actor, gNon_track_actor); \
        gDont_render_actor = BrActorAllocate(BR_ACTOR_NONE, NULL); \
        if (gDont_render_actor == NULL) { \
            FatalError(kFatalError_FailToOpenGeneralSettings); \
        } \
        gDont_render_actor->render_style = BR_RSTYLE_NONE; \
        BrActorAdd(gUniverse_actor, gDont_render_actor); \
        AllocateActors(); \
        AllocateCamera(); \
    } while (0)

// InitBRFonts

// Init2DStuffForPolyFonts

// FUNCTION: CARMA2_HW 0x0047e610
void C2_HOOK_FASTCALL InitLineStuff(void) {

    gLine_model = BrModelAllocate("gLine_model", 2, 1);
    gLine_material = BrMaterialAllocate("gLine_material");
    gLine_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    if (gLine_model == NULL || gLine_material == NULL || gLine_actor == NULL) {
        FatalError(kFatalError_OOM_S);
    }
    gLine_actor->identifier = "gLine_actor";
    gLine_actor->model = gLine_model;
    gLine_actor->material = gLine_material;
    gLine_actor->render_style = BR_RSTYLE_EDGES;
    gLine_model->flags = BR_MODF_KEEP_ORIGINAL | BR_MODF_QUICK_UPDATE;
    gLine_model->faces->vertices[0] = 0;
    gLine_model->faces->vertices[1] = 0;
    gLine_model->faces->vertices[2] = 1;
    gLine_material->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_TWO_SIDED;
    gLine_model->faces->flags = 5;
    BrModelAdd(gLine_model);
    BrMaterialAdd(gLine_material);
    BrActorAdd(gDont_render_actor, gLine_actor);
}

#define Prepare2DModelAndMaterial(pModel, pMaterial, pConfigure_vertices, pColour, pPrims) \
    do { \
        if (pConfigure_vertices) { \
            (pModel)->faces[0].vertices[0] = 0; \
            (pModel)->faces[0].vertices[1] = 1; \
            (pModel)->faces[0].vertices[2] = 2; \
            (pModel)->faces[1].vertices[0] = 2; \
            (pModel)->faces[1].vertices[1] = 3; \
            (pModel)->faces[1].vertices[2] = 0; \
            BrVector3Set(&(pModel)->vertices[0].p, 150.0f, -20.0f, -2.0f); \
            BrVector3Set(&(pModel)->vertices[1].p, 150.0f, -100.0f, -2.0f); \
            BrVector3Set(&(pModel)->vertices[2].p, 200.0f, -100.0f, -2.0f); \
            BrVector3Set(&(pModel)->vertices[3].p, 200.0f, -20.0f, -2.0f); \
        } \
        (pMaterial)->colour = pColour; \
        (pMaterial)->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT; \
        if ((pPrims) != NULL) { \
            (pMaterial)->extra_prim = pPrims; \
        } \
        (pModel)->flags |= BR_MODF_KEEP_ORIGINAL; \
    } while (0)

#define Prepare2DModelToDim(pModel, pX, pY, pW, pH) \
    do { \
        (pModel)->vertices[0].p.v[0] = (pModel)->vertices[1].p.v[0] = (float)(pX); \
        (pModel)->vertices[0].p.v[1] = (pModel)->vertices[3].p.v[1] = (float)(-pY); \
        (pModel)->vertices[2].p.v[0] = (pModel)->vertices[3].p.v[0] = (pModel)->vertices[1].p.v[0] + (pW); \
        (pModel)->vertices[1].p.v[1] = (pModel)->vertices[2].p.v[1] = (pModel)->vertices[3].p.v[1] - (pH); \
    } while (0)

// FUNCTION: CARMA2_HW 0x0047e720
void C2_HOOK_FASTCALL Init2DStuff(void) {
    // GLOBAL: CARMA2_HW 0x006569c8
    static br_token_value fadealpha_opacity0x80[3] = {
        { BRT_BLEND_B , {1}},
        { BRT_OPACITY_X, {0x800000} },
        { 0 },
    };
    // GLOBAL: CARMA2_HW 0x006569e0
    static br_token_value fadealpha_opacity0x70[3] = {
        { BRT_BLEND_B , {1}},
        { BRT_OPACITY_X, {0x700000} },
        { 0 },
    };
    tPath_name the_path;
    br_camera* camera_data;
    float w, h, w_fixed, h_fixed;

    gHUD_root_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrMatrix34Translate(&gHUD_root_actor->t.t.mat, 0.0f, 0.0f, 2.f);
    g2d_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    gDim_model = BrModelAllocate("gDim_model", 4, 2);
    gDim_material = BrMaterialAllocate("gDim_material");
    gDim_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gPrat_model = BrModelAllocate("gPrat_model", 4, 2);
    gPrat_material = BrMaterialAllocate("gPrat_material");
    gPrat_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    PathCat(the_path, gApplication_path, "INTRFACE");
    PathCat(the_path, the_path, "BOTHUP");
    strcat(the_path, ".DAT");

    gHeadup_model = BrModelLoad(the_path);
    gHeadup_material = BrMaterialAllocate("gHeadup_material");
    gHeadup_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gHeadup2_model = BrModelAllocate("gHeadup2_model", 4, 2);
    gHeadup2_material = BrMaterialAllocate("gHeadup2_material");
    gHeadup2_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gPowerupHUD_model = BrModelAllocate("gPowerupHUD_model", 4, 2);
    gPowerupHUD_material = BrMaterialAllocate("gPowerupHUD_material");
    gPowerupHUD_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gStatbarHUD1_model = BrModelAllocate("gStatbarHUD1_model", 4, 2);
    gStatbarHUD1_material = BrMaterialAllocate("gStatbarHUD1_material");
    gStatbarHUD1_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gHUDcurve_model = BrModelAllocate("gHUDcurve_model", 4, 2);
    gHUDcurve_material = BrMaterialAllocate("gHUDcurve_material");
    gHUDcurve_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gStatbarHUD3_model = BrModelAllocate("gStatbarHUD3_model", 4, 2);
    gStatbarHUD3_material = BrMaterialAllocate("gStatbarHUD3");
    gStatbarHUD3_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gHUDsquare_model = BrModelAllocate("gHUDsquare_model", 4, 2);
    gHUDsquare_material = BrMaterialAllocate("gHUDsquare_material");
    gHUDsquare_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gStatbarHUD5_model = BrModelAllocate("gStatbarHUD5_model", 4, 2);
    gStatbarHUD5_material = BrMaterialAllocate("gStatbarHUD5_material");
    gStatbarHUD5_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gTimerLeftHUD_model = BrModelAllocate("gTimerLeftHUD_model", 4, 2);
    gTimerLeftHUD_material = BrMaterialAllocate("gTimerLeftHUD_material");
    gTimerLeftHUD_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gTimerRightHUD_model = BrModelAllocate("gTimerRightHUD_model", 4, 2);
    gTimerRightHUD_material = BrMaterialAllocate("gTimerRightHUD_material");
    gTimerRightHUD_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gStatbarRightHUD_model = BrModelAllocate("gStatbarRightHUD_model", 4, 2);
    gStatbarRightHUD_material = BrMaterialAllocate("gStatbarRightHUD_material");
    gStatbarRightHUD_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gMapHUD_model = BrModelAllocate("gMapHUD_model", 4, 2);
    gMapHUD_material = BrMaterialAllocate("gMapHUD_material");
    gMapHUD_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gTestFont_model = BrModelAllocate("gTestFont_model", 4, 2);
    gTestFont_material = BrMaterialAllocate("gTestFont_material");
    gTestFont_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gArmour_model = BrModelAllocate("gArmour_model", 4, 2);
    gArmour_material = BrMaterialAllocate("gArmour_material");
    gArmour_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gPower_model = BrModelAllocate("gPower_model", 4, 2);
    gPower_material = BrMaterialAllocate("gPower_material");
    gPower_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    gOffence_model = BrModelAllocate("gOffence_model", 4, 2);
    gOffence_material = BrMaterialAllocate("gOffence_material");
    gOffense_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    if (gDim_model == NULL
            || gDim_material == NULL
            || gDim_actor == NULL
            || gPrat_model == NULL
            || gPrat_material == NULL
            || gPrat_actor == NULL
            || gHeadup_model == NULL
            || gHeadup_material == NULL
            || gHeadup_actor == NULL
            || gHeadup2_model == NULL
            || gHeadup2_material == NULL
            || gHeadup2_actor == NULL
            || gPowerupHUD_model == NULL
            || gPowerupHUD_material == NULL
            || gPowerupHUD_actor == NULL
            || gStatbarHUD1_model == NULL
            || gStatbarHUD1_material == NULL
            || gStatbarHUD1_actor == NULL
            || gHUDcurve_model == NULL
            || gHUDcurve_material == NULL
            || gHUDcurve_actor == NULL
            || gStatbarHUD3_model == NULL
            || gStatbarHUD3_material == NULL
            || gStatbarHUD3_actor == NULL
            || gHUDsquare_model == NULL
            || gHUDsquare_material == NULL
            || gHUDsquare_actor == NULL
            || gStatbarHUD5_model == NULL
            || gStatbarHUD5_material == NULL
            || gStatbarHUD5_actor == NULL
            || gTimerLeftHUD_model == NULL
            || gTimerLeftHUD_material == NULL
            || gTimerLeftHUD_actor == NULL
            || gTimerRightHUD_model == NULL
            || gTimerRightHUD_material == NULL
            || gTimerRightHUD_actor == NULL
            || gStatbarRightHUD_model == NULL
            || gStatbarRightHUD_material == NULL
            || gStatbarRightHUD_actor == NULL
            || gMapHUD_model == NULL
            || gMapHUD_material == NULL
            || gMapHUD_actor == NULL
            || gTestFont_model == NULL
            || gTestFont_material == NULL
            || gTestFont_actor == NULL
            || gArmour_model == NULL
            || gArmour_material == NULL
            || gArmour_actor == NULL
            || gPower_model == NULL
            || gPower_material == NULL
            || gPower_actor == NULL
            || gOffence_model == NULL
            || gOffence_material == NULL
            || gOffense_actor == NULL
            || g2d_camera == NULL
            || gHUD_root_actor == NULL) {
        FatalError(kFatalError_OOM_S);
    }

    g2d_camera->identifier = "A 2D camera, possibly even g2d_camera";
    camera_data = g2d_camera->type_data;
    camera_data->type = BR_CAMERA_PARALLEL;
    camera_data->hither_z = 1.0f;
    camera_data->yon_z = 3.0f;
    camera_data->width = (br_scalar)gScreen->width;
    camera_data->height = (br_scalar)gScreen->height;

    gDim_actor->identifier = "gDim_actor";
    gDim_actor->model = gDim_model;
    gDim_actor->material = gDim_material;
    Prepare2DModelAndMaterial(gDim_model, gDim_material, 1, BR_COLOUR_RGB(0, 0, 0), fadealpha_opacity0x80);
    BrModelAdd(gDim_model);
    BrMaterialAdd(gDim_material);

    gPrat_actor->identifier ="gPrat_actor";
    gPrat_actor->model = gPrat_model;
    gPrat_actor->material = gPrat_material;
    Prepare2DModelAndMaterial(gPrat_model, gPrat_material, 1, BR_COLOUR_RGB(0xff, 0xff, 0xff), NULL);
    BrVector2Set(&gPrat_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gPrat_model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&gPrat_model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&gPrat_model->vertices[3].map, 1.0f, 0.0f);
    BrModelAdd(gPrat_model);
    BrMaterialAdd(gPrat_material);

    gHeadup_actor->identifier = "gHeadup_actor";
    gHeadup_actor->model = gHeadup_model;
    gHeadup_actor->material = gHeadup_material;
    Prepare2DModelAndMaterial(gHeadup_model, gHeadup_material, 0, BR_COLOUR_RGB(0x04, 0x0e, 0x4a), fadealpha_opacity0x70);
    w = (float)gHeadup_dim_w;
    h = (float)gHeadup_dim_h;
    w_fixed = (float)HighResHeadupWidth(gHeadup_dim_w);
    h_fixed = (float)HighResHeadupHeight(gHeadup_dim_h);
    BrVector2Set(&gHeadup_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gHeadup_model->vertices[1].map, 0.0f, h / h_fixed);
    BrVector2Set(&gHeadup_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gHeadup_model->vertices[3].map, w / w_fixed, 0.0f);
    BrModelAdd(gHeadup_model);
    BrMaterialAdd(gHeadup_material);
    BrMatrix34Translate(&gHeadup_actor->t.t.mat,
            (float)(gHeadup_dim_x + 10 + gHeadup_dim_w),
            (float)(-gHeadup_dim_h / 2 - gHeadup_dim_y),
            -2.f);

    gHeadup2_actor->identifier = "gHeadup2_actor";
    gHeadup2_actor->model = gHeadup2_model;
    gHeadup2_actor->material = gHeadup2_material;
    Prepare2DModelAndMaterial(gHeadup2_model, gHeadup2_material, 1, BR_COLOUR_RGB(0x04, 0x0e, 0x4a), fadealpha_opacity0x70);
    w = (float)gHeadup2_dim_w;
    h = (float)gHeadup2_dim_h;
    w_fixed = (float)HighResHeadupWidth(gHeadup2_dim_w);
    h_fixed = (float)HighResHeadupHeight(gHeadup2_dim_h);
    BrVector2Set(&gHeadup2_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gHeadup2_model->vertices[1].map, 0.0f, h / h_fixed);
    BrVector2Set(&gHeadup2_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gHeadup2_model->vertices[3].map, w / w_fixed, 0.0f);
    BrModelAdd(gHeadup2_model);
    BrMaterialAdd(gHeadup2_material);

    gPowerupHUD_actor->identifier = "gPowerupHUD_actor";
    gPowerupHUD_actor->model = gPowerupHUD_model;
    gPowerupHUD_actor->material = gPowerupHUD_material;
    Prepare2DModelAndMaterial(gPowerupHUD_model, gPowerupHUD_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), NULL);
    w = (float)gPowerupHUD_dim_w;
    h = (float)gPowerupHUD_dim_h;
    w_fixed = (float)HighResHeadupWidth(gPowerupHUD_dim_w);
    h_fixed = (float)HighResHeadupHeight(gPowerupHUD_dim_h);
    BrVector2Set(&gPowerupHUD_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gPowerupHUD_model->vertices[1].map, 0.0f, h / h_fixed);
    BrVector2Set(&gPowerupHUD_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gPowerupHUD_model->vertices[3].map, w / w_fixed, 0.0f);
    BrModelAdd(gPowerupHUD_model);
    BrMaterialAdd(gPowerupHUD_material);

    gStatbarHUD1_actor->identifier = "gStatbarHUD1_actor";
    gStatbarHUD1_actor->model = gStatbarHUD1_model;
    gStatbarHUD1_actor->material = gStatbarHUD1_material;
    Prepare2DModelAndMaterial(gStatbarHUD1_model, gStatbarHUD1_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), NULL);
    w = (float)gStatbarHUD1_dim_w;
    h = (float)gStatbarHUD1_dim_h;
    w_fixed = (float)HighResHeadupWidth(gStatbarHUD1_dim_w);
    h_fixed = (float)HighResHeadupHeight(gStatbarHUD1_dim_h);
    BrVector2Set(&gStatbarHUD1_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gStatbarHUD1_model->vertices[1].map, 0.0f, h / h_fixed);
    BrVector2Set(&gStatbarHUD1_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gStatbarHUD1_model->vertices[3].map, w / w_fixed, 0.0f);
    BrModelAdd(gStatbarHUD1_model);
    BrMaterialAdd(gStatbarHUD1_material);

    gHUDcurve_actor->identifier = "gHUDcurve_actor";
    gHUDcurve_actor->model = gHUDcurve_model;
    gHUDcurve_actor->material = gHUDcurve_material;
    Prepare2DModelAndMaterial(gHUDcurve_model, gHUDcurve_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), NULL);
    BrVector2Set(&gPrat_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gPrat_model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&gPrat_model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&gPrat_model->vertices[3].map, 1.0f, 0.0f);
    BrModelAdd(gHUDcurve_model);
    BrMaterialAdd(gHUDcurve_material);

    gStatbarHUD3_actor->identifier = "gStatbarHUD3_actor";
    gStatbarHUD3_actor->model = gStatbarHUD3_model;
    gStatbarHUD3_actor->material = gStatbarHUD3_material;
    Prepare2DModelAndMaterial(gStatbarHUD3_model, gStatbarHUD3_material, 1, BR_COLOUR_RGB(0x04, 0x0e, 0x4a), fadealpha_opacity0x70);
    w = (float)gStatbarHUD3_dim_w;
    h = (float)gStatbarHUD3_dim_h;
    w_fixed = (float)HighResHeadupWidth(gStatbarHUD3_dim_w);
    h_fixed = (float)HighResHeadupHeight(gStatbarHUD3_dim_h);
    BrVector2Set(&gStatbarHUD3_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gStatbarHUD3_model->vertices[1].map, 0.0f, h / h_fixed);
    BrVector2Set(&gStatbarHUD3_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gStatbarHUD3_model->vertices[3].map, w / w_fixed, 0.0f);
    BrModelAdd(gStatbarHUD3_model);
    BrMaterialAdd(gStatbarHUD3_material);

    gHUDsquare_actor->identifier = "gHUDsquare_actor";
    gHUDsquare_actor->model = gHUDsquare_model;
    gHUDsquare_actor->material = gHUDsquare_material;
    Prepare2DModelAndMaterial(gHUDsquare_model, gHUDsquare_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), NULL);
    w = (float)gHUDsquare_dim_w;
    h = (float)gHUDsquare_dim_h;
    w_fixed = (float)HighResHeadupWidth(gHUDsquare_dim_w);
    h_fixed = (float)HighResHeadupHeight(gHUDsquare_dim_h);
    BrVector2Set(&gHUDsquare_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gHUDsquare_model->vertices[1].map, 0.0f, h / h_fixed);
    BrVector2Set(&gHUDsquare_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gHUDsquare_model->vertices[3].map, w / w_fixed, 0.0f);
    BrModelAdd(gHUDsquare_model);
    BrMaterialAdd(gHUDsquare_material);

    gStatbarHUD5_actor->identifier = "gStatbarHUD5_actor";
    gStatbarHUD5_actor->model = gStatbarHUD5_model;
    gStatbarHUD5_actor->material = gStatbarHUD5_material;
    Prepare2DModelAndMaterial(gStatbarHUD5_model, gStatbarHUD5_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), NULL);
    w = (float)gStatbarHUD5_dim_w;
    h = (float)gStatbarHUD5_dim_h;
    w_fixed = (float)HighResHeadupWidth(gStatbarHUD5_dim_w);
    h_fixed = (float)HighResHeadupHeight(gStatbarHUD5_dim_h);
    BrVector2Set(&gStatbarHUD5_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gStatbarHUD5_model->vertices[1].map, 0.0f, h / h_fixed);
    BrVector2Set(&gStatbarHUD5_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gStatbarHUD5_model->vertices[3].map, w / w_fixed, 0.0f);
    BrModelAdd(gStatbarHUD5_model);
    BrMaterialAdd(gStatbarHUD5_material);

    gTimerLeftHUD_actor->identifier = "gTimerLeftHUD_actor";
    gTimerLeftHUD_actor->model = gTimerLeftHUD_model;
    gTimerLeftHUD_actor->material = gTimerLeftHUD_material;
    Prepare2DModelAndMaterial(gTimerLeftHUD_model, gTimerLeftHUD_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), NULL);
    BrVector2Set(&gTimerLeftHUD_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gTimerLeftHUD_model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&gTimerLeftHUD_model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&gTimerLeftHUD_model->vertices[3].map, 1.0f, 0.0f);
    BrModelAdd(gTimerLeftHUD_model);
    BrMaterialAdd(gTimerLeftHUD_material);

    gTimerRightHUD_actor->identifier = "gTimerRightHUD_actor";
    gTimerRightHUD_actor->model = gTimerRightHUD_model;
    gTimerRightHUD_actor->material = gTimerRightHUD_material;
    Prepare2DModelAndMaterial(gTimerRightHUD_model, gTimerRightHUD_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), NULL);
    w = (float)gTimerRightHUD_dim_w;
    h = (float)gTimerRightHUD_dim_h;
    w_fixed = (float)HighResHeadupWidth(gTimerRightHUD_dim_w);
    h_fixed = (float)HighResHeadupHeight(gTimerRightHUD_dim_h);
    BrVector2Set(&gTimerRightHUD_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gTimerRightHUD_model->vertices[1].map, 0.0f, h / h_fixed);
    BrVector2Set(&gTimerRightHUD_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gTimerRightHUD_model->vertices[3].map, w / w_fixed, 0.0f);
    BrModelAdd(gTimerRightHUD_model);
    BrMaterialAdd(gTimerRightHUD_material);

    gStatbarRightHUD_actor->identifier = "gStatbarRightHUD_actor";
    gStatbarRightHUD_actor->model = gStatbarRightHUD_model;
    gStatbarRightHUD_actor->material = gStatbarRightHUD_material;
    Prepare2DModelAndMaterial(gStatbarRightHUD_model, gStatbarRightHUD_material, 1, BR_COLOUR_RGB(0x04, 0x0e, 0x4e), fadealpha_opacity0x70);
    BrModelAdd(gStatbarRightHUD_model);
    BrMaterialAdd(gStatbarRightHUD_material);

    gMapHUD_actor->identifier = "gMapHUD_actor";
    gMapHUD_actor->model = gMapHUD_model;
    gMapHUD_actor->material = gMapHUD_material;
    Prepare2DModelAndMaterial(gMapHUD_model, gMapHUD_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), NULL);
    BrModelAdd(gMapHUD_model);
    BrMaterialAdd(gMapHUD_material);

    gTestFont_actor->identifier = "gTestFont_actor";
    gTestFont_actor->model = gTestFont_model;
    gTestFont_actor->material = gTestFont_material;
    Prepare2DModelAndMaterial(gTestFont_model, gTestFont_material, 1, BR_COLOUR_RGB(0x22, 0x35, 0x51), NULL);
    BrModelAdd(gTestFont_model);
    BrMaterialAdd(gTestFont_material);

    gArmour_actor->identifier = "gArmour_actor";
    gArmour_actor->model = gArmour_model;
    gArmour_actor->material = gArmour_material;
    Prepare2DModelAndMaterial(gArmour_model, gArmour_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), NULL);
    BrVector2Set(&gArmour_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gArmour_model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&gArmour_model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&gArmour_model->vertices[3].map, 1.0f, 0.0f);
    BrModelAdd(gArmour_model);
    BrMaterialAdd(gArmour_material);

    gPower_actor->identifier = "gPower_actor";
    gPower_actor->model = gPower_model;
    gPower_actor->material = gPower_material;
    Prepare2DModelAndMaterial(gPower_model, gPower_material, 1, BR_COLOUR_RGB(0x22, 0x35, 0x51), NULL);
    BrVector2Set(&gPower_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gPower_model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&gPower_model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&gPower_model->vertices[3].map, 1.0f, 0.0f);
    BrModelAdd(gPower_model);
    BrMaterialAdd(gPower_material);

    gOffense_actor->identifier = "gOffense_actor";
    gOffense_actor->model = gOffence_model;
    gOffense_actor->material = gOffence_material;
    Prepare2DModelAndMaterial(gOffence_model, gOffence_material, 1, BR_COLOUR_RGB(0x22, 0x35, 0x51), NULL);
    BrVector2Set(&gOffence_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gOffence_model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&gOffence_model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&gOffence_model->vertices[3].map, 1.0f, 0.0f);
    BrModelAdd(gOffence_model);
    BrMaterialAdd(gOffence_material);

    Prepare2DModelToDim(gHeadup2_model, gHeadup2_dim_x, gHeadup2_dim_y, gHeadup2_dim_w, gHeadup2_dim_h);
    BrModelUpdate(gHeadup2_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gPowerupHUD_model, gPowerupHUD_dim_x, gPowerupHUD_dim_y, gPowerupHUD_dim_w, gPowerupHUD_dim_h);
    BrModelUpdate(gPowerupHUD_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gStatbarHUD1_model, gStatbarHUD1_dim_x, gStatbarHUD1_dim_y, gStatbarHUD1_dim_w, gStatbarHUD1_dim_h);
    BrModelUpdate(gStatbarHUD1_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gStatbarHUD3_model, gStatbarHUD3_dim_x, gStatbarHUD3_dim_y, gStatbarHUD3_dim_w, gStatbarHUD3_dim_h);
    BrModelUpdate(gStatbarHUD3_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gHUDsquare_model, gHUDsquare_dim_x, gHUDsquare_dim_y, gHUDsquare_dim_w, gHUDsquare_dim_h);
    BrModelUpdate(gHUDsquare_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gStatbarHUD5_model, gStatbarHUD5_dim_x, gStatbarHUD5_dim_y, gStatbarHUD5_dim_w, gStatbarHUD5_dim_h);
    BrModelUpdate(gStatbarHUD5_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gTimerLeftHUD_model, gTimerLeftHUD_dim_x, gTimerLeftHUD_dim_y, gTimerLeftHUD_dim_w, gTimerLeftHUD_dim_h);
    BrModelUpdate(gTimerLeftHUD_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gTimerRightHUD_model, gTimerRightHUD_dim_x, gTimerRightHUD_dim_y, gTimerRightHUD_dim_w, gTimerRightHUD_dim_h);
    BrModelUpdate(gTimerRightHUD_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gStatbarRightHUD_model, gStatbarRightHUD_dim_x, gStatbarRightHUD_dim_y, gStatbarRightHUD_dim_w, gStatbarRightHUD_dim_h);
    BrModelUpdate(gStatbarRightHUD_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gArmour_model, gArmour_dim_x, gArmour_dim_y, gArmour_dim_w, gArmour_dim_h);
    BrModelUpdate(gArmour_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gPower_model, gPower_dim_x, gPower_dim_y, gPower_dim_w, gPower_dim_h);
    BrModelUpdate(gPower_model, BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(gOffence_model, gOffence_dim_x, gOffence_dim_y, gOffence_dim_w, gOffence_dim_h);
    BrModelUpdate(gOffence_model, BR_MODU_VERTEX_POSITIONS);
}

// CheckTimedDemo

// void InitialiseApplication(int argc, char **argv);
#define InitialiseApplication(pArgc, pArgv) \
    do { \
        tTWTVFS twt; \
        \
        gProgram_state.sausage_eater_mode = gSausage_override; \
        InitQuickTimeStuff(); \
        memset(gTextureMaps, 0, sizeof(gTextureMaps)); \
        InitPackFiles(); \
        MAMSInitMem(); \
        if (gAustere_override || PDDoWeLeadAnAustereExistance()) { \
            gAusterity_mode = 1; \
        } else { \
            gAusterity_mode = 0; \
        } \
        srand((unsigned int)time(NULL)); \
        BrV1dbBeginWrapper_Float(); \
        CreateStainlessClasses(); \
        PDInstallErrorHandlers(); \
        InstallFindFailedHooks(); \
        InstallDRMemCalls(); \
        InstallDRFileCalls(); \
        twt = OpenPackFileAndSetTiffLoading(gApplication_path); \
        gApplicationDataTwtMounted = 1; \
        InitFogificateMaterials(); \
        PDInitTimer(); \
        InitWobbleStuff(); \
        LoadGeneralParameters(); \
        PrintMemoryDump(0, "AFTER LOADING GENERAL PARAMETERS"); \
        DefaultNetName(); \
        strcpy(gProgram_state.player_name, "MAX DAMAGE"); \
        RestoreOptions(); \
        LoadKeyMapping(); \
        if (!PDInitScreenVars(pArgc, pArgv)) { \
            FatalError(0); \
        } \
        CalcGrafDataIndex(); \
        PDSaveOriginalPalette(); \
        PDInitScreen(); \
        InitializeBRenderEnvironment(); \
        PrintMemoryDump(0, "AFTER INITIALISING BRENDER"); \
        CheckTimedDemo(); \
        InitDRFonts(); \
        gBig_font = LoadBRFont("BIGFONT.FNT"); \
        gFont_7 = LoadBRFont("FONT7.FNT"); \
        gHeadup_font = LoadBRFont("HEADUP.FNT"); \
        PrintMemoryDump(0, "AFTER LOADING FONTS"); \
        InitExplosions(); \
        PrintMemoryDump(0, "AFTER INITIALISING EXPLOSIONS"); \
        LoadMiscStrings(); \
        PrintMemoryDump(0, "AFTER LOADING MISC STRING"); \
        InitialiseStorageSpace(0, &gMisc_storage_space, 1000, 5, 1000, 1000, 10); \
        LoadInRegistees(); \
        InitMap(); \
        PrintMemoryDump(0, "AFTER INITIALISING GENERAL STORAGE AND LOADING IN REG"); \
        Init2DStuffForPolyFonts(); \
        InitLineStuff(); \
        InitSmokeStuff(); \
        PrintMemoryDump(0, "AFTER INITING LINE AND SMOKE STUFF"); \
        Init2DStuff(); \
        InitTintedPolyStuff(); \
        gHud_tinted1 = CreateTintedPoly(0, 0, 640, 480, 2, 0, 0, 0); \
        gHud_tinted2 = CreateTintedPoly(0, 0, 640, 480, 3, 2, 0, 0); \
        PrintMemoryDump(0, "AFTER INITING 2D STUFF"); \
        InitLightingEffects(); \
        LoadInterfaceStrings(); \
        PrintMemoryDump(0, "AFTER INITING INTERFACE STRINGS"); \
        PrintMemoryDump(0, "AFTER INITING ACTION REPLAY"); \
        InitPhysics(); \
        PrintMemoryDump(0, "AFTER INITING PHYSICS"); \
        InitCrushSystems(); \
        PrintMemoryDump(0, "AFTER INITING CRUSH SYSTEMS"); \
        InitTreeSurgery(); \
        FlicPaletteAllocate(); \
        InitInterfaceLoadState(); \
        InitTransientBitmaps(); \
        PrintMemoryDump(0, "AFTER INITING OTHER INTERFACE STUFF"); \
        InitSound(); \
        PrintMemoryDump(0, "AFTER INITING SOUND"); \
        InitHeadups(); \
        PrintMemoryDump(0, "AFTER INITING HEADUPS"); \
        gDefault_track_material = BrMaterialAllocate("gDefault_track_material"); \
        gDefault_track_material->index_base = 227; \
        gDefault_track_material->index_range = 1; \
        gDefault_track_material->colour = ((br_colour*)gRender_palette->pixels)[227]; \
        gDefault_track_material->ka = 1.0f; \
        gDefault_track_material->kd = 0.0f; \
        gDefault_track_material->ks = 0.0f; \
        BrMaterialAdd(gDefault_track_material); \
        InitPowerups(); \
        PrintMemoryDump(0, "AFTER INITING POWERUPS"); \
        InitShadow(); \
        PrintMemoryDump(0, "AFTER INITING SHADOW"); \
        InitFlics(); \
        AllocateStandardLamp(); \
        InitPanGameAIWorld(); \
        PrintMemoryDump(0, "AFTER INITING AI WORLD"); \
        LoadPowerups(); \
        PrintMemoryDump(0, "AFTER LOADING POWERUPS"); \
        LoadRaces(gRace_list, &gNumber_of_races, -1); \
        PrintMemoryDump(0, "AFTER LOADING RACES"); \
        InitPaletteAnimate(); \
        InitRayCasting(); \
        InitDepthEffects(); \
        PrintMemoryDump(0, "AFTER INITING DEPTH EFFECTS"); \
        InitSmashing(); \
        PrintMemoryDump(0, "AFTER INITING SMASHING"); \
        InitialiseStorageSpace(0, &gOur_car_storage_space, 200, 2, 200, 200, 10); \
        InitialiseStorageSpace(0, &gTheir_cars_storage_space, 1500, 50, 1500, 1500, 50); \
        InitialiseStorageSpace(0, &gDroneStorage, 500, 50, 500, 228, 10); \
        InitialiseStorageSpace(0, &gPedStorage, 200, 10, 100, 500, 200); \
        InitialiseStorageSpace(0, &gTrack_storage_space, 1000, 50, 1000, 1000, 50); \
        FinishLoadingGeneral(); \
        PrintMemoryDump(0, "AFTER FINISHING LOADING GENERAL"); \
        InitOilSpills(); \
        if (gAustere_time) { \
            while (PDGetTotalTime() - gAustere_time < 2000) { \
            } \
        } \
        ClearEntireScreen(); \
        InitSkids(); \
        PrintMemoryDump(0, "AFTER INITING SPILLS AND SKIDS"); \
        InitPolyPedSystem(); \
        gProgram_state.track_spec.the_actor = NULL; \
        gCD_is_in_drive = TestForOriginalCarmaCDinDrive(); \
        LoadHeadupImages(); \
        InitGame(gDev_initial_race); \
        BrMatrix34Identity(&gCamera_to_world); \
        BrMatrix34Identity(&gRearview_camera_to_world); \
        PrintMemoryDump(0, "AFTER APPLICATION INITIALISATION (JUST INITED PED SYSTEM)"); \
        memset(gPickedup_powerups, 0, sizeof(gPickedup_powerups)); \
        memset(gUnknown_00705b80, 0, sizeof(gUnknown_00705b80)); \
        ClosePackFileAndSetTiffLoading(twt); \
        gApplicationDataTwtMounted = 0; \
    } while (0)

void C2_HOOK_FASTCALL DoDevelopmentThings(void) {

}

// FUNCTION: CARMA2_HW 0x0047dd20
void C2_HOOK_FASTCALL InitialiseDeathRace(int pArgc, const char** pArgv) {

    PDInitialiseSystem();
    DoDevelopmentThings();
    InitialiseApplication(pArgc, pArgv);
    gInitialisation_finished = 1;
}

void C2_HOOK_FASTCALL InitGameAccordingToSkillLevel(void) {
    int i;

    gProgram_state.rank = gInitial_rank;
    gProgram_state.credits_per_rank = gCredits_per_rank[gProgram_state.skill_level];
    gProgram_state.credits = gStarting_money[gProgram_state.skill_level];
    for (i = 0; i < (int)REC2_ASIZE(gInitial_APO); i++) {
        gCurrent_APO_levels[i] = gNet_mode == eNet_mode_none ? gInitial_APO[i].initial[gProgram_state.skill_level] : gInitial_APO[i].initial_network[gCurrent_net_game->type];
        gCurrent_APO_potential_levels[i] = gNet_mode == eNet_mode_none ? gInitial_APO_potential[i].initial[gProgram_state.skill_level] : gInitial_APO_potential[i].initial_network[gCurrent_net_game->type];
    }
}

// FUNCTION: CARMA2_HW 0x004816b0
void C2_HOOK_FASTCALL InitGame(int pStart_race) {
    int i;

    gProgram_state.field_0x2c = 0;
    gNo_current_game = 0;
    gWaiting_for_unpause = 1;
    gWait_for_it = 0;
    gGame_to_load = -1;
    gCurrent_race.number_of_racers = 0;
    InitFogificateMaterials();
    DisableAutoFogification();
    gGame_initialized = 1;
    gNet_mode_of_last_game = gNet_mode;
    gNo_races_yet = 1;
    NetPlayerStatusChanged(ePlayer_status_loading);
    gCurrent_race_group = gRace_list[pStart_race].group;
    gIs_boundary_race = gRace_list[pStart_race].is_boundary;
    gProgram_state.current_race_index = pStart_race;
    for (i = 0; i < gNumber_of_races; i++) {
        gRace_list[i].count_opponents = 0;
    }
    for (i = 0; i < gNumber_of_racers; i++) {
        gOpponents[i].dead = 0;
    }
    gProgram_state.number_of_cars = 1;
    gProgram_state.cars_available[0] = 0;
    gProgram_state.current_car_index = 0;
    gProgram_state.game_completed = 0;
    gProgram_state.redo_race_index = -1;

    InitGameAccordingToSkillLevel();
}

// LoadInTrack

// InitRace

// GetScreenSize

// SetScreenSize

