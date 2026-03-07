#include "init.h"

#include "car.h"
#include "controls.h"
#include "crush.h"
#include "demo.h"
#include "depth.h"
#include "displays.h"
#include "drmem.h"
#include "errors.h"
#include "explosions.h"
#include "flicplay.h"
#include "font.h"
#include "globvars.h"
#include "globvrbm.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "joystick.h"
#include "loading.h"
#include "netgame.h"
#include "network.h"
#include "oil.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "powerups.h"
#include "pratcam.h"
#include "platform.h"
#include "racemem.h"
#include "raycast.h"
#include "replay.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "temp.h"
#include "tinted.h"
#include "trig.h"
#include "utility.h"
#include "video.h"
#include "world.h"

#include "brender/brender.h"

#include "rec2_macros.h"

#include "c2_stdlib.h"
#include "c2_string.h"
#include "c2_time.h"


// GLOBAL: CARMA2_HW 0x0068be24
tU32 gAustere_time;

// GLOBAL: CARMA2_HW 0x0068be2c
int gBr_initialized;

// GLOBAL: CARMA2_HW 0x0068be30
int gBrZb_initialized;

// GLOBAL: CARMA2_HW 0x0068be38
int gRender_indent;

// GLOBAL: CARMA2_HW 0x0068be34
int gApp_initialized;

// GLOBAL: CARMA2_HW 0x0074b74c
br_material* gDefault_track_material;

// GLOBAL: CARMA2_HW 0x00762130
int gInitial_powerup_slots[3];

// GLOBAL: CARMA2_HW 0x0074c6d0
undefined4 gDAT_0074c6d0;

void C2_HOOK_FASTCALL DoDevelopmentThings(void) {

}

// FUNCTION: CARMA2_HW 0x00486d80
void C2_HOOK_FASTCALL InitLightingEffects(void) {

}

// FUNCTION: CARMA2_HW 0x00486d70
void C2_HOOK_FASTCALL KillLightingEffects(void) {

}

// FUNCTION: CARMA2_HW 0x0047dd20
void C2_HOOK_FASTCALL InitialiseDeathRace(int pArgc, const char** pArgv) {
    PDInitialiseSystem();
    DoDevelopmentThings();
    InitialiseApplication(pArgc, pArgv);
    gApp_initialized = 1;
}

void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char **pArgv) {
    tTWTVFS twt;

#ifndef REC2_MATCHING
    C2_HOOK_BUG_ON(sizeof(int) != 4);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, complete_race_data) != 184);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, time_for_this_objective_to_finish) != 0x74);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, follow_path_data.corner_width) != 0xbc + 0x50);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, pursue_car_data) != 0x114);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, pursue_car_data.time_last_away_from_pursuee) != 0x114 + 0x14);
    C2_HOOK_BUG_ON(sizeof(tOpponent_spec) != 420);

    C2_HOOK_BUG_ON(sizeof(tIntelligent_vehicles) != 17064);
    C2_HOOK_BUG_ON(sizeof(tPursuee_trail) != 324);

    C2_HOOK_BUG_ON(offsetof(tCar_spec, force_torque_ratio) != 4972);
    C2_HOOK_BUG_ON(offsetof(tCar_spec, pre_car_col_velocity_car_space) != 5056);
    C2_HOOK_BUG_ON(offsetof(tCar_spec, time_last_hit) != 5092);
    C2_HOOK_BUG_ON(offsetof(tCar_spec, no_of_processes_recording_my_trail) != 5108);
    C2_HOOK_BUG_ON(offsetof(tCar_spec, my_trail) != 5112);
    C2_HOOK_BUG_ON(offsetof(tCar_spec, last_person_we_hit) != 5476);
    C2_HOOK_BUG_ON(offsetof(tCar_spec, maybe_blood_remaining) != 5852);
    C2_HOOK_BUG_ON(offsetof(tCar_spec, softness_factor) != 0x18d0);
    C2_HOOK_BUG_ON(sizeof(tCar_spec) != 6500);

    C2_HOOK_BUG_ON(offsetof(tProgram_state, sausage_eater_mode) != 16);
    C2_HOOK_BUG_ON(offsetof(tProgram_state, special_screens_count) != 7192);
    C2_HOOK_ASSERT((uintptr_t) &gProgram_state.sausage_eater_mode == 0x0075bb90);
    C2_HOOK_BUG_ON(sizeof(tProgram_state) != 24272);
    C2_HOOK_BUG_ON(sizeof(gTextureMaps) != 0x1000);
#endif

    gProgram_state.sausage_eater_mode = gSausage_override;
    InitQuickTimeStuff();
    c2_memset(gTextureMaps, 0, sizeof(gTextureMaps));
    InitPackFiles();
    MAMSInitMem();
    gAusterity_mode = gAustere_override || PDDoWeLeadAnAustereExistance();
    c2_srand((unsigned int)c2_time(NULL));
    BrV1dbBeginWrapper_Float();
    CreateStainlessClasses();
    PDInstallErrorHandlers();
    InstallFindFailedHooks();
    InstallDRMemCalls();
    InstallDRFileCalls();
    twt = OpenPackFileAndSetTiffLoading(gApplication_path);
    gApplicationDataTwtMounted = 1;
    InitFogificateMaterials();
    PDInitTimer();
    InitWobbleStuff();
    LoadGeneralParameters();
    PrintMemoryDump(0, "AFTER LOADING GENERAL PARAMETERS");
    DefaultNetName();
    c2_strcpy(gProgram_state.player_name, "MAX DAMAGE");
    RestoreOptions();
    LoadKeyMapping();
    if (!PDInitScreenVars(pArgc, pArgv)) {
        FatalError(0);
    }
    CalcGrafDataIndex();
    PDSaveOriginalPalette();
    PDInitScreen();
    InitializeBRenderEnvironment();
    PrintMemoryDump(0, "AFTER INITIALISING BRENDER");
    CheckTimedDemo();
    InitDRFonts();
    gBig_font = LoadBRFont("BIGFONT.FNT");
    gFont_7 = LoadBRFont("FONT7.FNT");
    gHeadup_font = LoadBRFont("HEADUP.FNT");
    PrintMemoryDump(0, "AFTER LOADING FONTS");
    InitExplosions();
    PrintMemoryDump(0, "AFTER INITIALISING EXPLOSIONS");
    LoadMiscStrings();
    PrintMemoryDump(0, "AFTER LOADING MISC STRING");
    InitialiseStorageSpace(0, &gMisc_storage_space, 1000, 5, 1000, 1000, 10);
    LoadInRegistees();
    InitMap();
    PrintMemoryDump(0, "AFTER INITIALISING GENERAL STORAGE AND LOADING IN REG");
    Init2DStuffForPolyFonts();
    InitLineStuff();
    InitSmokeStuff();
    PrintMemoryDump(0, "AFTER INITING LINE AND SMOKE STUFF");
    Init2DStuff();
    InitTintedPolyStuff();
    gHud_tinted1 = CreateTintedPoly(0, 0, 640, 480, 2, 0, 0, 0);
    gHud_tinted2 = CreateTintedPoly(0, 0, 640, 480, 3, 2, 0, 0);
    PrintMemoryDump(0, "AFTER INITING 2D STUFF");
    InitLightingEffects();
    // FUN_00486d80(); /* NOP */
    LoadInterfaceStrings();
    PrintMemoryDump(0, "AFTER INITING INTERFACE STRINGS");
    PrintMemoryDump(0, "AFTER INITING ACTION REPLAY");
    InitPhysics();
    PrintMemoryDump(0, "AFTER INITING PHYSICS");
    InitCrushSystems();
    PrintMemoryDump(0, "AFTER INITING CRUSH SYSTEMS");
    InitTreeSurgery();
    FlicPaletteAllocate();
    InitInterfaceLoadState();
    InitTransientBitmaps();
    PrintMemoryDump(0, "AFTER INITING OTHER INTERFACE STUFF");
    InitSound();
    PrintMemoryDump(0, "AFTER INITING SOUND");
    InitHeadups();
    PrintMemoryDump(0, "AFTER INITING HEADUPS");
    gDefault_track_material = BrMaterialAllocate("gDefault_track_material");
    gDefault_track_material->index_base = 227;
    gDefault_track_material->index_range = 1;
    gDefault_track_material->colour = ((br_colour*)gRender_palette->pixels)[227];
    gDefault_track_material->ka = 1.f;
    gDefault_track_material->kd = 0.f;
    gDefault_track_material->ks = 0.f;
    BrMaterialAdd(gDefault_track_material);
    InitPowerups();
    PrintMemoryDump(0, "AFTER INITING POWERUPS");
    InitShadow();
    PrintMemoryDump(0, "AFTER INITING SHADOW");
    InitFlics();
    AllocateStandardLamp();
    InitPanGameAIWorld();
    PrintMemoryDump(0, "AFTER INITING AI WORLD");
    LoadPowerups();
    PrintMemoryDump(0, "AFTER LOADING POWERUPS");
    LoadRaces(gRace_list, &gNumber_of_races, -1);
    PrintMemoryDump(0, "AFTER LOADING RACES");
    InitPaletteAnimate();
    InitRayCasting();
    InitDepthEffects();
    PrintMemoryDump(0, "AFTER INITING DEPTH EFFECTS");
    InitSmashing();
    PrintMemoryDump(0, "AFTER INITING SMASHING");
    InitialiseStorageSpace(0, &gOur_car_storage_space, 200, 2, 200, 200, 10);
    InitialiseStorageSpace(0, &gTheir_cars_storage_space, 1500, 50, 1500, 1500, 50);
    InitialiseStorageSpace(0, &gDroneStorage, 500, 50, 500, 228, 10);
    InitialiseStorageSpace(0, &gPedStorage, 200, 10, 100, 500, 200);
    InitialiseStorageSpace(0, &gTrack_storage_space, 1000, 50, 1000, 1000, 50);
    FinishLoadGeneralParameters();
    PrintMemoryDump(0, "AFTER FINISHING LOADING GENERAL");
    InitOilSpills();
    if (gAustere_time) {
        while (PDGetTotalTime() - gAustere_time < 2000) {
        }
    }
    ClearEntireScreen();
    InitSkids();
    PrintMemoryDump(0, "AFTER INITING SPILLS AND SKIDS");
    InitPolyPedSystem();
    gProgram_state.track_spec.the_actor = NULL;
    gCD_is_in_drive = TestForOriginalCarmaCDinDrive();
    LoadHeadupImages();
    InitGame(gDev_initial_race);
    BrMatrix34Identity(&gCamera_to_world);
    BrMatrix34Identity(&gRearview_camera_to_world);
    PrintMemoryDump(0, "AFTER APPLICATION INITIALISATION (JUST INITED PED SYSTEM)");
    c2_memset(gPickedup_powerups, 0, sizeof(gPickedup_powerups));
    c2_memset(gUnknown_00705b80, 0, sizeof(gUnknown_00705b80));
    ClosePackFileAndSetTiffLoading(twt);
    gApplicationDataTwtMounted = 0;
}

// FUNCTION: CARMA2_HW 0x00481e90
int C2_HOOK_FASTCALL GetScreenSize(void) {

    return gRender_indent;
}

// FUNCTION: CARMA2_HW 0x00481ea0
void C2_HOOK_FASTCALL SetScreenSize(int pNew_size) {

    gRender_indent = pNew_size;
}

void C2_HOOK_FASTCALL InitBRFonts(void) {

    gBig_font = LoadBRFont("BIGFONT.FNT");
    gFont_7 = LoadBRFont("FONT7.FNT");
    gHeadup_font = LoadBRFont("HEADUP.FNT");
}

void C2_HOOK_FASTCALL InitializeBRenderEnvironment(void) {

    gBr_initialized = 1;
    SetBRenderScreenAndBuffers(0, 0, 0, 0);
    gUniverse_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (gUniverse_actor == NULL) {
        FatalError(3);
    }
    gUniverse_actor->identifier = BrResStrDup(gUniverse_actor, "Root");
    BrEnvironmentSet(gUniverse_actor);
    gNon_track_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (gNon_track_actor == NULL) {
        FatalError(3);
    }
    BrActorAdd(gUniverse_actor, gNon_track_actor);
    gDont_render_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (gDont_render_actor == NULL) {
        FatalError(3);
    }
    gDont_render_actor->render_style = BR_RSTYLE_NONE;
    BrActorAdd(gUniverse_actor, gDont_render_actor);
    AllocateActors();
    AllocateCamera();
}

// FUNCTION: CARMA2_HW 0x0047e320
void C2_HOOK_FASTCALL AllocateActors(void) {
    int i;

    gSelf = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (gSelf == NULL) {
        FatalError(kFatalError_CannotAllocateSelf);
    }
    gSelf = BrActorAdd(gUniverse_actor, gSelf);
    if (gSelf == NULL) {
        FatalError(kFatalError_CannotAllocateSelf);
    }

    C2_HOOK_BUG_ON(REC2_ASIZE(gOther_selfs) != 4);

    for (i = 0; i < REC2_ASIZE(gOther_selfs); i++) {
        gOther_selfs[i] = BrActorAllocate(BR_ACTOR_NONE, NULL);
        if (gOther_selfs[i] == NULL) {
            FatalError(kFatalError_CannotAllocateSelf);
        }
        BrActorAdd(gSelf, gOther_selfs[i]);
    }
}

// FUNCTION: CARMA2_HW 0x0047e3b0
void C2_HOOK_FASTCALL AllocateCamera(void) {
    br_camera* camera_ptr;
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCamera_list) != 2);

    for (i = 0; i < REC2_ASIZE(gCamera_list); i++) {
        gCamera_list[i] = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
        if (gCamera_list[i] == NULL) {
            FatalError(kFatalError_CannotAllocateCamera);
        }

        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->type = BR_CAMERA_PERSPECTIVE_FOV;
        camera_ptr->field_of_view = BrDegreeToAngle(gCamera_angle);
        camera_ptr->hither_z = gCamera_hither;
        camera_ptr->yon_z = gCamera_yon;
        camera_ptr->aspect = (float)gWidth / (float)gHeight;
    }

    if (gCamera_list[0] == NULL) {
        FatalError(kFatalError_CannotAllocateCamera);
    }
    gCamera_list[1] = BrActorAdd(gUniverse_actor, gCamera_list[1]);
    if (gCamera_list[1] == NULL) {
        FatalError(kFatalError_CannotAllocateCamera);
    }
    gCamera = gCamera_list[1];
    gRearview_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    if (gRearview_camera == NULL) {
        FatalError(kFatalError_CannotAllocateCamera);
    }

    gRearview_camera->t.t.mat.m[2][2] = -1.0f;
    camera_ptr = (br_camera*)gRearview_camera->type_data;
    camera_ptr->hither_z = gCamera_hither;
    camera_ptr->type = BR_CAMERA_PERSPECTIVE_FOV;
    camera_ptr->yon_z = .5f * gCamera_yon;
    camera_ptr->field_of_view = BrDegreeToAngle(gCamera_angle);
    camera_ptr->aspect = (float)gWidth / (float)gHeight;
    if (gRearview_camera == NULL) {
        FatalError(kFatalError_CannotAllocateCamera);
    }
    SetSightDistance(camera_ptr->yon_z);
}

static void Prepare2DModelAndMaterial(br_model* pModel, br_material* pMaterial, int pConfigure_vertices, br_colour pColour, int pFadeAlpha) {
    static br_token_value fadealpha[3] = {
            { BRT_BLEND_B , {1}},
            { BRT_OPACITY_X, {0x800000} }, /* 50%*/
            { 0 },
    };

    if (pConfigure_vertices) {
        pModel->faces[0].vertices[0] = 0;
        pModel->faces[0].vertices[1] = 1;
        pModel->faces[0].vertices[2] = 2;
        pModel->faces[1].vertices[0] = 2;
        pModel->faces[1].vertices[1] = 3;
        pModel->faces[1].vertices[2] = 0;
        BrVector3Set(&pModel->vertices[0].p, 150.f, -20.f, -2.f);
        BrVector3Set(&pModel->vertices[1].p, 150.0, -100.f, -2.f);
        BrVector3Set(&pModel->vertices[2].p, 200.f, -100.f, -2.f);
        BrVector3Set(&pModel->vertices[3].p, 200.f, -20.f, -2.f);
    }
    pMaterial->colour = pColour;
    pMaterial->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    if (pFadeAlpha) {
        pMaterial->extra_prim = fadealpha;
    }
    pModel->flags |= BR_MODF_KEEP_ORIGINAL;
}

static void Prepare2DModelToDim(br_model* pModel, int pX, int pY, int pW, int pH) {

    pModel->vertices[0].p.v[0] = pModel->vertices[1].p.v[0] = (float)pX;
    pModel->vertices[0].p.v[1] = pModel->vertices[3].p.v[1] = (float)-pY;
    pModel->vertices[2].p.v[0] = pModel->vertices[3].p.v[0] = pModel->vertices[1].p.v[0] + pW;
    pModel->vertices[1].p.v[1] = pModel->vertices[2].p.v[1] = pModel->vertices[3].p.v[1] - pH;
}

extern br_actor* gPrat_actor;
extern br_actor* g2d_camera;
extern br_actor* gPowerupHUD_actor;
extern br_actor* gStatbarHUD1_actor;
extern br_actor* gHeadup2_actor;
extern br_actor* gHUDsquare_actor;
extern br_actor* gMapHUD_actor;
extern br_actor* gTimerRightHUD_actor;
extern br_actor* gOffense_actor;
extern br_actor* gTimerLeftHUD_actor;
extern br_actor* gPower_actor;
extern br_actor* gHUDcurve_actor;
extern br_actor* gStatbarRightHUD_actor;
extern br_actor* gHeadup_actor;
extern br_actor* gStatbarHUD5_actor;
extern br_actor* gDim_actor;
extern br_actor* gArmour_actor;
extern br_actor* gHUD_root_actor;
extern br_actor* gStatbarHUD3_actor;
extern br_actor* gTestFont_actor;

extern br_model* gPrat_model;
extern br_model* gMapHUD_model;
extern br_model* gStatbarRightHUD_model;
extern br_model* gTestFont_model;
extern br_model* gHeadup_model;
extern br_model* gHUDsquare_model;
extern br_model* gTimerRightHUD_model;
extern br_model* gOffence_model;
extern br_model* gTimerLeftHUD_model;
extern br_model* gPower_model;
extern br_model* gStatbarHUD5_model;
extern br_model* gStatbarHUD3_model;
extern br_model* gStatbarHUD1_model;
extern br_model* gPowerupHUD_model;
extern br_model* gHeadup2_model;
extern br_model* gDim_model;
extern br_model* gHUDcurve_model;
extern br_model* gArmour_model;

extern br_material* gPowerupHUD_material;
extern br_material* gTestFont_material;
extern br_material* gArmour_material;
extern br_material* gStatbarHUD1_material;
extern br_material* gStatbarHUD5_material;
extern br_material* gHeadup_material;
extern br_material* gStatbarHUD3_material;
extern br_material* gOffence_material;
extern br_material* gPower_material;
extern br_material* gStatbarRightHUD_material;
extern br_material* gPrat_material;
extern br_material* gHUDcurve_material;
extern br_material* gMapHUD_material;
extern br_material* gHeadup2_material;
extern br_material* gTimerRightHUD_material;
extern br_material* gDim_material;
extern br_material* gHUDsquare_material;
extern br_material* gTimerLeftHUD_material;

extern int gHeadup_dim_w;
extern int gHeadup_dim_h;
extern int gHeadup_dim_x;
extern int gHeadup_dim_y;

extern int gHeadup2_dim_w;
extern int gHeadup2_dim_h;
extern int gHeadup2_dim_x;
extern int gHeadup2_dim_y;

extern int gPowerupHUD_dim_w;
extern int gPowerupHUD_dim_h;
extern int gPowerupHUD_dim_x;
extern int gPowerupHUD_dim_y;

extern int gStatbarHUD1_dim_w;
extern int gStatbarHUD1_dim_h;
extern int gStatbarHUD1_dim_x;
extern int gStatbarHUD1_dim_y;

extern int gStatbarHUD3_dim_w;
extern int gStatbarHUD3_dim_h;
extern int gStatbarHUD3_dim_x;
extern int gStatbarHUD3_dim_y;

extern int gHUDsquare_dim_w;
extern int gHUDsquare_dim_h;
extern int gHUDsquare_dim_x;
extern int gHUDsquare_dim_y;

extern int gStatbarHUD5_dim_w;
extern int gStatbarHUD5_dim_h;
extern int gStatbarHUD5_dim_x;
extern int gStatbarHUD5_dim_y;

extern int gTimerRightHUD_dim_w;
extern int gTimerRightHUD_dim_h;
extern int gTimerRightHUD_dim_x;
extern int gTimerRightHUD_dim_y;

extern int gTimerLeftHUD_dim_w;
extern int gTimerLeftHUD_dim_h;
extern int gTimerLeftHUD_dim_x;
extern int gTimerLeftHUD_dim_y;

extern int gTimerRightHUD_dim_w;
extern int gTimerRightHUD_dim_h;
extern int gTimerRightHUD_dim_x;
extern int gTimerRightHUD_dim_y;

extern int gArmour_dim_w;
extern int gArmour_dim_h;
extern int gArmour_dim_x;
extern int gArmour_dim_y;

extern int gPower_dim_w;
extern int gPower_dim_h;
extern int gPower_dim_x;
extern int gPower_dim_y;

extern int gOffence_dim_w;
extern int gOffence_dim_h;
extern int gOffence_dim_x;
extern int gOffence_dim_y;

// FUNCTION: CARMA2_HW 0x0047e720
void C2_HOOK_FASTCALL Init2DStuff(void) {
    tPath_name the_path;
    br_camera* camera_data;
    float w, h, w_fixed, h_fixed;

    gHUD_root_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrMatrix34Translate(&gHUD_root_actor->t.t.mat, 0.f, 0.f, 2.f);
    g2d_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    gDim_model = BrModelAllocate("gDim_model", 4, 2);
    gDim_material = BrMaterialAllocate("gDim_material");
    gDim_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gPrat_model = BrModelAllocate("gPrat_model", 4, 2);
    gPrat_material = BrMaterialAllocate("gPrat_material");
    gPrat_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    PathCat(the_path, gApplication_path, "INTRFACE");
    PathCat(the_path, the_path, "BOTHUP");
    c2_strcat(the_path, ".DAT");

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

    if (gHUD_root_actor == NULL
            || g2d_camera == NULL
            || gDim_model == NULL
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
            || gOffense_actor == NULL) {
        FatalError(kFatalError_OOM_S);
    }

    g2d_camera->identifier = "A 2D camera, possibly even g2d_camera";
    camera_data = g2d_camera->type_data;
    camera_data->type = BR_CAMERA_PARALLEL;
    camera_data->hither_z = 1.f;
    camera_data->yon_z = 3.f;
    camera_data->width = (float)gScreen->width;
    camera_data->height = (float)gScreen->height;

    gDim_actor->identifier = "gDim_actor";
    gDim_actor->model = gDim_model;
    gDim_actor->material = gDim_material;
    Prepare2DModelAndMaterial(gDim_model, gDim_material, 1, BR_COLOUR_RGB(0, 0, 0), 1);
    BrModelAdd(gDim_model);
    BrMaterialAdd(gDim_material);

    gPrat_actor->identifier ="gPrat_actor";
    gPrat_actor->model = gPrat_model;
    gPrat_actor->material = gPrat_material;
    Prepare2DModelAndMaterial(gPrat_model, gPrat_material, 1, BR_COLOUR_RGB(0xff, 0xff, 0xff), 0);
    BrVector2Set(&gPrat_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gPrat_model->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&gPrat_model->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&gPrat_model->vertices[3].map, 1.f, 0.f);
    BrModelAdd(gPrat_model);
    BrMaterialAdd(gPrat_material);

    gHeadup_actor->identifier = "gHeadup_actor";
    gHeadup_actor->model = gHeadup_model;
    gHeadup_actor->material = gHeadup_material;
    Prepare2DModelAndMaterial(gHeadup_model, gHeadup_material, 0, BR_COLOUR_RGB(0x04, 0x0e, 0x4a), 1);
    w = (float)gHeadup_dim_w;
    h = (float)gHeadup_dim_w;
    w_fixed = (float)HighResHeadupWidth(gHeadup_dim_w);
    h_fixed = (float)HighResHeadupHeight(gHeadup_dim_w);
    BrVector2Set(&gHeadup_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gHeadup_model->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&gHeadup_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gHeadup_model->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(gHeadup_model);
    BrMaterialAdd(gHeadup_material);
    BrMatrix34Translate(&gHeadup_actor->t.t.mat,
            (float)(gHeadup_dim_x + 10 + gHeadup_dim_w),
            (float)(-gHeadup_dim_h / 2 - gHeadup_dim_y),
            -2.f);

    gHeadup2_actor->identifier = "gHeadup2_actor";
    gHeadup2_actor->model = gHeadup2_model;
    gHeadup2_actor->material = gHeadup2_material;
    Prepare2DModelAndMaterial(gHeadup2_model, gHeadup2_material, 1, BR_COLOUR_RGB(0x04, 0x0e, 0x4a), 1);
    w = (float)gHeadup2_dim_w;
    h = (float)gHeadup2_dim_h;
    w_fixed = (float)HighResHeadupWidth(gHeadup2_dim_w);
    h_fixed = (float)HighResHeadupHeight(gHeadup2_dim_h);
    BrVector2Set(&gHeadup2_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gHeadup2_model->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&gHeadup2_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gHeadup2_model->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(gHeadup2_model);
    BrMaterialAdd(gHeadup2_material);

    gPowerupHUD_actor->identifier = "gPowerupHUD_actor";
    gPowerupHUD_actor->model = gPowerupHUD_model;
    gPowerupHUD_actor->material = gPowerupHUD_material;
    Prepare2DModelAndMaterial(gPowerupHUD_model, gPowerupHUD_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)gPowerupHUD_dim_w;
    h = (float)gPowerupHUD_dim_h;
    w_fixed = (float)HighResHeadupWidth(gPowerupHUD_dim_w);
    h_fixed = (float)HighResHeadupHeight(gPowerupHUD_dim_h);
    BrVector2Set(&gPowerupHUD_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gPowerupHUD_model->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&gPowerupHUD_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gPowerupHUD_model->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(gPowerupHUD_model);
    BrMaterialAdd(gPowerupHUD_material);

    gStatbarHUD1_actor->identifier = "gStatbarHUD1_actor";
    gStatbarHUD1_actor->model = gStatbarHUD1_model;
    gStatbarHUD1_actor->material = gStatbarHUD1_material;
    Prepare2DModelAndMaterial(gStatbarHUD1_model, gStatbarHUD1_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)gStatbarHUD1_dim_w;
    h = (float)gStatbarHUD1_dim_h;
    w_fixed = (float)HighResHeadupWidth(gStatbarHUD1_dim_w);
    h_fixed = (float)HighResHeadupHeight(gStatbarHUD1_dim_h);
    BrVector2Set(&gStatbarHUD1_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gStatbarHUD1_model->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&gStatbarHUD1_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gStatbarHUD1_model->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(gStatbarHUD1_model);
    BrMaterialAdd(gStatbarHUD1_material);

    gHUDcurve_actor->identifier = "gHUDcurve_actor";
    gHUDcurve_actor->model = gHUDcurve_model;
    gHUDcurve_actor->material = gHUDcurve_material;
    Prepare2DModelAndMaterial(gHUDcurve_model, gHUDcurve_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    BrVector2Set(&gPrat_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gPrat_model->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&gPrat_model->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&gPrat_model->vertices[3].map, 1.f, 0.f);
    BrModelAdd(gHUDcurve_model);
    BrMaterialAdd(gHUDcurve_material);

    gStatbarHUD3_actor->identifier = "gStatbarHUD3_actor";
    gStatbarHUD3_actor->model = gStatbarHUD3_model;
    gStatbarHUD3_actor->material = gStatbarHUD3_material;
    Prepare2DModelAndMaterial(gStatbarHUD3_model, gStatbarHUD3_material, 1, BR_COLOUR_RGB(0x04, 0x0e, 0x4a), 1);
    w = (float)gStatbarHUD3_dim_w;
    h = (float)gStatbarHUD3_dim_h;
    w_fixed = (float)HighResHeadupWidth(gStatbarHUD3_dim_w);
    h_fixed = (float)HighResHeadupHeight(gStatbarHUD3_dim_h);
    BrVector2Set(&gStatbarHUD3_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gStatbarHUD3_model->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&gStatbarHUD3_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gStatbarHUD3_model->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(gStatbarHUD3_model);
    BrMaterialAdd(gStatbarHUD3_material);

    gHUDsquare_actor->identifier = "gHUDsquare_actor";
    gHUDsquare_actor->model = gHUDsquare_model;
    gHUDsquare_actor->material = gHUDsquare_material;
    Prepare2DModelAndMaterial(gHUDsquare_model, gHUDsquare_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)gHUDsquare_dim_w;
    h = (float)gHUDsquare_dim_h;
    w_fixed = (float)HighResHeadupWidth(gHUDsquare_dim_w);
    h_fixed = (float)HighResHeadupHeight(gHUDsquare_dim_h);
    BrVector2Set(&gHUDsquare_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gHUDsquare_model->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&gHUDsquare_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gHUDsquare_model->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(gHUDsquare_model);
    BrMaterialAdd(gHUDsquare_material);

    gStatbarHUD5_actor->identifier = "gStatbarHUD5_actor";
    gStatbarHUD5_actor->model = gStatbarHUD5_model;
    gStatbarHUD5_actor->material = gStatbarHUD5_material;
    Prepare2DModelAndMaterial(gStatbarHUD5_model, gStatbarHUD5_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)gStatbarHUD5_dim_w;
    h = (float)gStatbarHUD5_dim_h;
    w_fixed = (float)HighResHeadupWidth(gStatbarHUD5_dim_w);
    h_fixed = (float)HighResHeadupHeight(gStatbarHUD5_dim_h);
    BrVector2Set(&gStatbarHUD5_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gStatbarHUD5_model->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&gStatbarHUD5_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gStatbarHUD5_model->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(gStatbarHUD5_model);
    BrMaterialAdd(gStatbarHUD5_material);

    gTimerLeftHUD_actor->identifier = "gTimerLeftHUD_actor";
    gTimerLeftHUD_actor->model = gTimerLeftHUD_model;
    gTimerLeftHUD_actor->material = gTimerLeftHUD_material;
    Prepare2DModelAndMaterial(gTimerLeftHUD_model, gTimerLeftHUD_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    BrVector2Set(&gTimerLeftHUD_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gTimerLeftHUD_model->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&gTimerLeftHUD_model->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&gTimerLeftHUD_model->vertices[3].map, 1.f, 0.f);
    BrModelAdd(gTimerLeftHUD_model);
    BrMaterialAdd(gTimerLeftHUD_material);

    gTimerRightHUD_actor->identifier = "gTimerRightHUD_actor";
    gTimerRightHUD_actor->model = gTimerRightHUD_model;
    gTimerRightHUD_actor->material = gTimerRightHUD_material;
    Prepare2DModelAndMaterial(gTimerRightHUD_model, gTimerRightHUD_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)gTimerRightHUD_dim_w;
    h = (float)gTimerRightHUD_dim_h;
    w_fixed = (float)HighResHeadupWidth(gTimerRightHUD_dim_w);
    h_fixed = (float)HighResHeadupHeight(gTimerRightHUD_dim_h);
    BrVector2Set(&gTimerRightHUD_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gTimerRightHUD_model->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&gTimerRightHUD_model->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&gTimerRightHUD_model->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(gTimerRightHUD_model);
    BrMaterialAdd(gTimerRightHUD_material);

    gStatbarRightHUD_actor->identifier = "gStatbarRightHUD_actor";
    gStatbarRightHUD_actor->model = gStatbarRightHUD_model;
    gStatbarRightHUD_actor->material = gStatbarRightHUD_material;
    Prepare2DModelAndMaterial(gStatbarRightHUD_model, gStatbarRightHUD_material, 1, BR_COLOUR_RGB(0x04, 0x0e, 0x4e), 1);
    BrModelAdd(gStatbarRightHUD_model);
    BrMaterialAdd(gStatbarRightHUD_material);

    gMapHUD_actor->identifier = "gMapHUD_actor";
    gMapHUD_actor->model = gMapHUD_model;
    gMapHUD_actor->material = gMapHUD_material;
    Prepare2DModelAndMaterial(gMapHUD_model, gMapHUD_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    BrModelAdd(gMapHUD_model);
    BrMaterialAdd(gMapHUD_material);

    gTestFont_actor->identifier = "gTestFont_actor";
    gTestFont_actor->model = gTestFont_model;
    gTestFont_actor->material = gTestFont_material;
    Prepare2DModelAndMaterial(gTestFont_model, gTestFont_material, 1, BR_COLOUR_RGB(0x22, 0x35, 0x51), 0);
    BrModelAdd(gTestFont_model);
    BrMaterialAdd(gTestFont_material);

    gArmour_actor->identifier = "gArmour_actor";
    gArmour_actor->model = gArmour_model;
    gArmour_actor->material = gArmour_material;
    Prepare2DModelAndMaterial(gArmour_model, gArmour_material, 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    BrVector2Set(&gArmour_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gArmour_model->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&gArmour_model->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&gArmour_model->vertices[3].map, 1.f, 0.f);
    BrModelAdd(gArmour_model);
    BrMaterialAdd(gArmour_material);

    gPower_actor->identifier = "gPower_actor";
    gPower_actor->model = gPower_model;
    gPower_actor->material = gPower_material;
    Prepare2DModelAndMaterial(gPower_model, gPower_material, 1, BR_COLOUR_RGB(0x22, 0x35, 0x51), 0);
    BrVector2Set(&gPower_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gPower_model->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&gPower_model->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&gPower_model->vertices[3].map, 1.f, 0.f);
    BrModelAdd(gPower_model);
    BrMaterialAdd(gPower_material);

    gOffense_actor->identifier = "gOffense_actor";
    gOffense_actor->model = gOffence_model;
    gOffense_actor->material = gOffence_material;
    Prepare2DModelAndMaterial(gOffence_model, gOffence_material, 1, BR_COLOUR_RGB(0x22, 0x35, 0x51), 0);
    BrVector2Set(&gOffence_model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&gOffence_model->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&gOffence_model->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&gOffence_model->vertices[3].map, 1.f, 0.f);
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

// FUNCTION: CARMA2_HW 0x004f9fc0
void C2_HOOK_FASTCALL InitSmokeStuff(void) {

    int i;
    br_pixelmap* smoke_pm;
    static br_token_value fadealpha[3] = {
        { BRT_BLEND_B, { 1 } },
        { BRT_OPACITY_X, { 0x4b0000 } }, /* ~ 29.296875% = 75 / 256 */
        { 0 },
    };

    gBlend_model = BrModelAllocate("gBlend_model", 4, 2);
    gBlend_model2 = BrModelAllocate("gBlend_model2", 6, 4);
    gBlend_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    for (i = 0; i < REC2_ASIZE(gBR_smoke_structs); i++) {
        gBR_smoke_structs[i].material = BrMaterialAllocate("some smoke");
    }
    if (gBlend_model == NULL || gBlend_actor == NULL) {
        FatalError(kFatalError_OOM_S);
    }
    gBlend_actor->identifier = "gBlend_actor";
    gBlend_actor->model = gBlend_model;

    gBlend_model->faces[0].vertices[0] = 0;
    gBlend_model->faces[0].vertices[1] = 1;
    gBlend_model->faces[0].vertices[2] = 2;
    gBlend_model->faces[1].vertices[0] = 2;
    gBlend_model->faces[1].vertices[1] = 3;
    gBlend_model->faces[1].vertices[2] = 0;

    gBlend_model2->faces[0].vertices[0] = 0;
    gBlend_model2->faces[0].vertices[1] = 1;
    gBlend_model2->faces[0].vertices[2] = 2;
    gBlend_model2->faces[1].vertices[0] = 2;
    gBlend_model2->faces[1].vertices[1] = 3;
    gBlend_model2->faces[1].vertices[2] = 0;
    gBlend_model2->faces[2].vertices[0] = 1;
    gBlend_model2->faces[2].vertices[1] = 5;
    gBlend_model2->faces[2].vertices[2] = 2;
    gBlend_model2->faces[3].vertices[0] = 1;
    gBlend_model2->faces[3].vertices[1] = 4;
    gBlend_model2->faces[3].vertices[2] = 5;

    BrVector3Set(&gBlend_model->vertices[0].p, -1.f,  1.f, 0.f);
    BrVector3Set(&gBlend_model->vertices[1].p, -1.f, -1.f, 0.f);
    BrVector3Set(&gBlend_model->vertices[2].p,  1.f, -1.f, 0.f);
    BrVector3Set(&gBlend_model->vertices[3].p,  1.f,  1.f, 0.f);

    BrVector3Set(&gBlend_model2->vertices[0].p, -1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[1].p, -1.f, 0.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[2].p,  1.f, 0.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[3].p,  1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[4].p, -1.f, -.25f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[5].p,  1.f, -.25f, 0.f);
    gBlend_model->flags |= BR_MODF_KEEP_ORIGINAL;
    gBlend_model2->flags |= BR_MODF_KEEP_ORIGINAL;

    smoke_pm = LoadPixelmap("SMOKE.PIX");
    if (smoke_pm == NULL) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, "SMOKE.PIX");
    }
    smoke_pm->map = gRender_palette;
    BrMapAdd(smoke_pm);

    for (i = 0; i < REC2_ASIZE(gBR_smoke_structs); i++) {
        gBR_smoke_structs[i].material->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_PERSPECTIVE;
        gBR_smoke_structs[i].material->extra_prim = fadealpha;
        gBR_smoke_structs[i].material->colour_map = smoke_pm;
        BrMaterialAdd(gBR_smoke_structs[i].material);
    }

    BrVector2Set(&gBlend_model->vertices[0].map, 0.f, 1.f - 1.f / (float)smoke_pm->height);
    BrVector2Set(&gBlend_model->vertices[1].map, 0.f, 0.f);
    BrVector2Set(&gBlend_model->vertices[2].map, 1.f - 1.f / (float)smoke_pm->width, 0.f);
    BrVector2Set(&gBlend_model->vertices[3].map, 1.f - 1.f / (float)smoke_pm->width, 1.f - 1.f / (float)smoke_pm->height);

    BrVector2Set(&gBlend_model2->vertices[0].map, 0.f, 1.f - 1.f / (float)smoke_pm->height);
    BrVector2Set(&gBlend_model2->vertices[1].map, 0.f, 0.5f);
    BrVector2Set(&gBlend_model2->vertices[2].map, 1.f - 1.f / (float)smoke_pm->width, .5f);
    BrVector2Set(&gBlend_model2->vertices[3].map, 1.f - 1.f / (float)smoke_pm->width, 1.f - 1.f / (float)smoke_pm->height);
    BrVector2Set(&gBlend_model2->vertices[4].map, 0.f, 0.f);
    BrVector2Set(&gBlend_model2->vertices[5].map, 1.f - 1.f / (float)smoke_pm->width, 0.f);

    BrModelAdd(gBlend_model);
    BrModelAdd(gBlend_model2);

    BrActorAdd(gDont_render_actor, gBlend_actor);
}

void C2_HOOK_FASTCALL InstallFindFailedHooks(void) {

}

// FUNCTION: CARMA2_HW 0x0047e500
void C2_HOOK_FASTCALL AllocateStandardLamp(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        br_actor *light = gLight_array[i];
        BrActorAdd(gUniverse_actor, light);
        if (light->children != NULL) {
            br_actor *child = light->children;
            BrActorRemove(child);
            light->children = NULL;
            BrActorFree(child);
        }
        BrLightEnable(light);
    }
}

void C2_HOOK_FASTCALL InitGameAccordingToSkillLevel(void) {
    int i;

    gProgram_state.rank = gInitial_rank;
    gProgram_state.credits_per_rank = gCredits_per_rank[gProgram_state.skill_level];
    gProgram_state.credits = gStarting_money[gProgram_state.skill_level];
    for (i = 0; i < REC2_ASIZE(gInitial_APO); i++) {
        gCurrent_APO_levels[i] = gNet_mode == eNet_mode_none ? gInitial_APO[i].initial[gProgram_state.skill_level] : gInitial_APO[i].initial_network[gCurrent_net_game->type];
        gCurrent_APO_potential_levels[i] = gNet_mode == eNet_mode_none ? gInitial_APO_potential[i].initial[gProgram_state.skill_level] : gInitial_APO_potential[i].initial_network[gCurrent_net_game->type];
    }
}

// FUNCTION: CARMA2_HW 0x004816b0
void C2_HOOK_FASTCALL InitGame(int pStart_race) {
    int i;

#ifndef _MSC_VER
    C2_HOOK_BUG_ON((uintptr_t)&gCurrent_race.number_of_racers != 0x00762430);
#endif

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
    C2_HOOK_BUG_ON(sizeof(tOpponent) != 448);
    for (i = 0; i < gNumber_of_racers; i++) {
        gOpponents[i].dead = 0;
    }
    gProgram_state.number_of_cars = 1;
    gProgram_state.redo_race_index = -1;
    gProgram_state.cars_available[0] = 0;
    gProgram_state.current_car_index = 0;
    gProgram_state.game_completed = 0;

    InitGameAccordingToSkillLevel();
}

// FUNCTION: CARMA2_HW 0x0044ba60
int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth) {
    int new_width;

    if (gDevious_2d && gTexture_power_of_2) {
        new_width = 1;
        while (pWidth > new_width) {
            new_width *= 2;
        }
    } else {
        new_width = pWidth;
    }
    return new_width;
}

// FUNCTION: CARMA2_HW 0x0044ba90
int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight) {
    int new_height;

    if (gDevious_2d && gTexture_power_of_2) {
        new_height = 1;
        while (pHeight > new_height) {
            new_height *= 2;
        }
    } else {
        new_height = pHeight;
    }
    return new_height;

}

// FUNCTION: CARMA2_HW 0x0044c130
void C2_HOOK_FASTCALL DisposeGameIfNecessary(void) {
    int i;

    dr_dprintf("DisposeGameIfNecessary() - START");
    if (gNet_mode != eNet_mode_none) {
        NetLeaveGame(gCurrent_net_game);
    }
    dr_dprintf("DisposeGameIfNecessary() - Point 1");
    if (gGame_initialized) {
        gGame_initialized = 0;
        dr_dprintf("DisposeGameIfNecessary() - Point 2");
        if (gNet_mode_of_last_game != eNet_mode_none) {
            for (i = 0; i < gNumber_of_net_players; i++) {
                DisposeCar(gNet_players[i].car, gNet_players[i].car_index);
            }
            dr_dprintf("DisposeGameIfNecessary() - Point 3");
            PossibleService();
            LoadRaces(gRace_list, &gNumber_of_races, -1);
            DisposeStorageSpace(&gNet_cars_storage_space);
            gNo_current_game = 1;
            dr_dprintf("DisposeGameIfNecessary() - Point 4");
        }
        dr_dprintf("DisposeGameIfNecessary() - Point 5");
    }
    ShutdownNetIfRequired();
    dr_dprintf("DisposeGameIfNecessary() - END");
}

// FUNCTION: CARMA2_HW 0x0047dc30
void C2_HOOK_FASTCALL ReinitialiseRenderStuff(void) {
    int x_diff;
    int y_diff;

    if (gProgram_state.cockpit_on) {
        gProgram_state.current_render_left = gProgram_state.current_car.render_left[gProgram_state.cockpit_image_index];
        gProgram_state.current_render_top = gProgram_state.current_car.render_top[gProgram_state.cockpit_image_index];
        gProgram_state.current_render_right = gProgram_state.current_car.render_right[gProgram_state.cockpit_image_index];
        gProgram_state.current_render_bottom = gProgram_state.current_car.render_bottom[gProgram_state.cockpit_image_index];
    } else {
        if (gNo_render_indent) {
            gProgram_state.current_render_right = gGraf_specs[gGraf_spec_index].total_width;
            gProgram_state.current_render_bottom = gGraf_specs[gGraf_spec_index].total_height;
            gProgram_state.current_render_left = 0;
            gProgram_state.current_render_top = 0;
        } else {
            gProgram_state.current_render_top = (gGraf_specs[gGraf_spec_index].total_height / 18 & ~1) * gRender_indent;
            gProgram_state.current_render_left = (gGraf_specs[gGraf_spec_index].total_width / 18 & ~3) * gRender_indent;
            x_diff = gGraf_specs[gGraf_spec_index].total_width - gProgram_state.current_render_left;
            y_diff = gGraf_specs[gGraf_spec_index].total_height - gProgram_state.current_render_top;
            gProgram_state.current_render_right = x_diff;
            gProgram_state.current_render_bottom = y_diff;

        }
    }
}

// FUNCTION: CARMA2_HW 0x0047d8d0
void C2_HOOK_FASTCALL ReinitialiseForwardCamera(void) {
    br_camera* camera_ptr;
    float the_angle;
    float d;
    float w;

    camera_ptr = gCamera->type_data;
    if (gProgram_state.cockpit_on) {
        the_angle = gCamera_angle / 2.f;

        d = atanf(
                tandeg(the_angle)
                * (float)gRender_screen->height
                / (float)(gProgram_state.current_car.render_bottom[0] - gProgram_state.current_car.render_top[0]))
            * (br_scalar)(2 * 180. / PI);
        camera_ptr->field_of_view = BrDegreeToAngle(d);
        BrMatrix34Identity(&gCamera->t.t.mat);
        BrVector3Set(&gCamera->t.t.translate.t,
            gProgram_state.current_car.driver_x_offset,
            gProgram_state.current_car.driver_y_offset,
            gProgram_state.current_car.driver_z_offset);
        w = (float)(gRender_screen->base_y
                    + (gRender_screen->height / 2)
                    - (gProgram_state.current_car.render_bottom[0] + gProgram_state.current_car.render_top[0]) / 2);

        gCamera->t.t.mat.m[2][1] = tandeg(d / 2.0f) * w * 2.0f / (float)gRender_screen->height;
        camera_ptr->aspect = (float)gWidth / gHeight;
        camera_ptr->yon_z = gYon_multiplier * gCamera_yon;
        if (gProgram_state.which_view == eView_left) {
            DRMatrix34PostRotateY(
                    &gCamera->t.t.mat,
                    BrDegreeToAngle(gProgram_state.current_car.head_left_angle));
        } else if (gProgram_state.which_view == eView_right) {
            DRMatrix34PostRotateY(
                    &gCamera->t.t.mat,
                    BrDegreeToAngle(gProgram_state.current_car.head_right_angle));
        }
        BrVector3Set(&gCamera->t.t.translate.t,
                     gProgram_state.current_car.driver_x_offset,
                     gProgram_state.current_car.driver_y_offset,
                     gProgram_state.current_car.driver_z_offset);
        SetSightDistance(camera_ptr->yon_z);
        MungeForwardSky();
    }
    AssertYons();
}

// FUNCTION: CARMA2_HW 0x0047dab0
void C2_HOOK_FASTCALL AllocateRearviewPixelmap(void) {

    if (gRearview_screen != NULL) {
        BrPixelmapFree(gRearview_screen);
        gRearview_screen = NULL;
    }
    if (gProgram_state.mirror_on) {
        gRearview_screen = BrPixelmapAllocateSub(
                gBack_screen,
                gProgram_state.current_car.mirror_left,
                gProgram_state.current_car.mirror_top,
                gProgram_state.current_car.mirror_right - gProgram_state.current_car.mirror_left,
                gProgram_state.current_car.mirror_bottom - gProgram_state.current_car.mirror_top);
        gRearview_depth_buffer = gDepth_buffer;
        gRearview_screen->origin_x = gRearview_screen->width / 2;
        gRearview_screen->origin_y = gRearview_screen->height / 2;
    }
}

// FUNCTION: CARMA2_HW 0x0047db30
void C2_HOOK_FASTCALL ReinitialiseRearviewCamera(void) {
    br_camera* camera_ptr;

    camera_ptr = gRearview_camera->type_data;
    camera_ptr->field_of_view = BrDegreeToAngle(gProgram_state.current_car.rearview_camera_angle);
    camera_ptr->aspect = (gProgram_state.current_car.mirror_right - gProgram_state.current_car.mirror_left) / (float)(gProgram_state.current_car.mirror_bottom - gProgram_state.current_car.mirror_top);
    BrVector3Set(&gRearview_camera->t.t.translate.t,
        gProgram_state.current_car.mirror_x_offset,
        gProgram_state.current_car.mirror_y_offset,
        gProgram_state.current_car.mirror_z_offset);
    AllocateRearviewPixelmap();
    MungeRearviewSky();
}

static void C2_HOOK_FASTCALL InitRaceHeadups(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gRace_head_ups) != 22);

    for (i = 0; i < REC2_ASIZE(gRace_head_ups); i++) {
        gRace_head_ups[i] = NewTextHeadupSlot(16 + i, 0, 0, -1, "");
    }
    if (gNet_mode == eNet_mode_none) {
        gCredits_won_headup = NewTextHeadupSlot(1, 0, 0, -6, "");
        gPed_kill_count_headup = NewTextHeadupSlot(2, 0, 0, -6, "");
        gCar_kill_count_headup = NewTextHeadupSlot(12, 0, 0, -6, "");
        gTimer_headup = NewTextHeadupSlot(7, 0, 0, -5, "");
        gTime_awarded_headup = NewTextHeadupSlot(11, 0, 0, -2, "");
        gLaps_headup = NewTextHeadupSlot(8, 0, 0, -6, "");

        C2_HOOK_BUG_ON(REC2_ASIZE(gInitial_powerup_slots) != 3);
        for (i = 0; i < REC2_ASIZE(gInitial_powerup_slots); i++) {
            int current, max;

            if (gNet_mode == eNet_mode_none) {
                max = gMax_APO[i].initial[gProgram_state.skill_level];
                current = gInitial_APO_potential[i].initial[gProgram_state.skill_level];
            } else {
                max = gMax_APO[i].initial_network[gCurrent_net_game->type];
                current = gInitial_APO_potential[i].initial_network[gCurrent_net_game->type];
            }
            gInitial_powerup_slots[i] = current + 1 + (max - current - 1) * gCurrent_race.index / (gNumber_of_races - 1);
        }
    } else {
        gNet_cash_headup = NewTextHeadupSlot(13, 0, 0, -6, "");
        gNet_ped_headup = NewTextHeadupSlot(14, 0, 0, -6, "");

        C2_HOOK_BUG_ON(REC2_ASIZE(gInitial_powerup_slots) != 3);
        for (i = 0; i < REC2_ASIZE(gInitial_powerup_slots); i++) {
            if (gNet_mode == eNet_mode_none) {
                gInitial_powerup_slots[i] = gMax_APO[i].initial[gProgram_state.skill_level];
            } else {
                gInitial_powerup_slots[i] = gMax_APO[i].initial_network[gCurrent_net_game->type];
            }
        }
    }
}

void C2_HOOK_FASTCALL Modify2DCopyPixelmaps(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(gFonts); i++) {
        BRPM_convert(gFonts[i].images, gBack_screen->type);
    }
    BRPM_convert(gIcons_pix, gBack_screen->type);
    BRPM_convert(gLit_op_stat, gBack_screen->type);
}

// FUNCTION: CARMA2_HW 0x00481830
void C2_HOOK_FASTCALL InitRace(void) {

    gMap_view = 1;
    InitFogificateMaterials();
    InitSmashTargets();
    ClearAllRuntimeVariables();
    PHILInit();
    SetUpTestObjects();
    InitTargetLock();
    ReinitSmashing();
    InitRepairAnimations();
    ResetExplosions();
    ClearWobbles();
    ClearHeadups();
    ResetOilSpills();
    HideSkids();
    ResetPedNearness();
    PossibleService();
    InitPedsForRace();
    BuildColourTable(gRender_palette);
    PossibleService();

    gDAT_0074c6d0 = 0;
    gINT_0074b780 = 0;
    gDAT_0079ec44 = NULL;
    if (!TranslationMode()) {
        LoadFont(1);
        LoadFont(2);
    }
    LoadFont(3);
    LoadFont(4);
    LoadFont(5);
    PossibleService();
    LoadFont(6);
    LoadFont(7);
    LoadFont(8);
    LoadFont(23);
    PossibleService();
    ResetRecoveryVouchers();
    gMap_time = 0;
    gProgram_state.cockpit_image_index = 0;
    PossibleService();
    gProgram_state.which_view = eView_forward;
    gProgram_state.new_view = eView_undefined;
    gProgram_state.pratcam_move_start = 0;
    gAction_replay_mode = 0;
    InitPratcam();
    gProgram_state.peds_killed = 0;
    gProgram_state.revs = 2000;
    gProgram_state.current_car.speed = 0.0f;
    gProgram_state.current_car.steering_angle = 0.0f;
    gProgram_state.current_car.lf_sus_position = 0.0f;
    gProgram_state.current_car.rf_sus_position = 0.0f;
    gProgram_state.current_car.lr_sus_position = 0.0f;
    gProgram_state.current_car.rr_sus_position = 0.0f;
    PossibleService();
    gAuto_repair = 0;
    SetIntegerMapRenders();
    AdjustRenderScreenSize();
    PrintMemoryDump(0, "DIRECTLY BEFORE LOADING IN TRACK");
    LoadInTrack();
    if (gYon_multiplier != 1.0f) {
        AdjustRenderScreenSize();
    }
    PrintMemoryDump(0, "DIRECTLY AFTER LOADING IN TRACK");
    ResetCrushSystems();
    LoadAIWorldForRace(&gCurrent_race);

    InitRaceHeadups();

    InstantDepthChange(
        gProgram_state.default_depth_effect.type,
        gProgram_state.default_depth_effect.sky_texture,
        gProgram_state.default_depth_effect.start,
        gProgram_state.default_depth_effect.end,
        gProgram_state.default_depth_effect.colour.red,
        gProgram_state.default_depth_effect.colour.green,
        gProgram_state.default_depth_effect.colour.blue,
        1);
    gSky_texture_0079ec1c = NULL;
    if (!GetSkyTextureOn()) {
        ToggleSkyQuietly();
    }
    gSwap_depth_effect_type = eDepth_effect_none;
    if (!GetDepthCueingOn()) {
        ToggleDepthCueingQuietly();
    }
    PrintMemoryDump(0, "AFTER LOADING IN AI STUFF");
    SaveShadeTables();
    gCountdown = 7;
    gLap = 1;
    gTimer = 1000 * gCurrent_race.initial_timer[gProgram_state.skill_level];
    gTotal_laps = gCurrent_race.total_laps;

    if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_5) {
        gTotal_laps = gCurrent_net_game->options.starting_target;
    }
    gCheckpoint = 1;
    gCheckpoint_count = gCurrent_race.check_point_count;
    gFreeze_timer = 0;
    gFree_repairs = 0;
    if (gNet_mode == eNet_mode_none) {
        gShow_opponents = 1;
    } else {
        gShow_opponents = gCurrent_net_game->options.show_players_on_map;
    }
    gOn_drugs = 0;
    gRace_finished = 0;
    gOpponent_speed_factor = 1.f;
    gCop_speed_factor = 1.f;
    gGravity_multiplier = gDefault_gravity;
    gPinball_factor = 0.f;
    gInstant_handbrake = 0;
    gShow_peds_on_map = 0;

    PossibleService();
    SetCarSuspGiveAndHeight(&gProgram_state.current_car REC2_THISCALL_EDX, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    ResetPowerups();
    PossibleService();
    ResetSparks();
    PossibleService();
    PossibleService();
    InitPlayers();
    if (gNet_mode != eNet_mode_none) {
        gProgram_state.credits = gCurrent_net_game->options.starting_credits;
        InitNetGameplayStuff();
    }
    gInitialised_grid = 0;
    SetCameraType(gAction_replay_camera_mode);
    Modify2DCopyPixelmaps();

    SetupFFBValues();
    PossibleLock(1);
    PossibleUnlock(1);
}

// FUNCTION: CARMA2_HW 0x0044c070
void C2_HOOK_FASTCALL DisposeRace(void) {

    RemoveTail();
    PossibleService();
    CleanUpSmashStuff();
    DisposeActionReplay();
    PossibleService();
    CloseDownPowerUps();
    PossibleService();
    if (!TranslationMode()) {
        DisposeFont(1);
        DisposeFont(2);
    }
    PossibleService();
    DisposeFont(6);
    DisposeFont(7);
    DisposeFont(8);
    DisposeFont(3);
    DisposeFont(4);
    DisposeFont(5);
    PossibleService();
    DisposeSavedShadeTables();
    PossibleService();
    DisposeSoundSources();
    PossibleService();
    KillLightingEffects();
    DisposeAIWorldRaceStuff();
    PossibleService();
    DisposePedestrians();
    PossibleService();
    DisposePratcam();
    PossibleService();
}

void C2_HOOK_FASTCALL LoadInTrack(void) {

    LoadTrack(
        gProgram_state.track_file_name,
        &gProgram_state.track_spec,
        &gCurrent_race);
}

// FUNCTION: CARMA2_HW 0x0044c220
void C2_HOOK_FASTCALL DisposeTrack(void) {

    FreeTrack(&gProgram_state.track_spec);
}
