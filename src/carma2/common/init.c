#include "init.h"

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
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
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
#include "raycast.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
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

C2_HOOK_VARIABLE_IMPLEMENT(tU32, gAustere_time, 0x0068be24);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBr_initialized, 0x0068be2c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBrZb_initialized, 0x0068be30);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRender_indent, 0x0068be38);
C2_HOOK_VARIABLE_IMPLEMENT(int, gApp_initialized, 0x0068be34);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gDefault_track_material, 0x0074b74c);

void (C2_HOOK_FASTCALL * InitialiseApplication_original)(int pArgc, const char** pArgv);
void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char** pArgv) {
#if 0//defined(C2_HOOKS_ENABLED)
    InitialiseApplication_original(pArgc, pArgv);
#else
    C2_HOOK_BUG_ON(sizeof(int) != 4);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, complete_race_data) != 184);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, field44_0xf4) != 244);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, follow_path_data__corner_width) != 268);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, time_last_away_from_pursuee) != 296);
    C2_HOOK_BUG_ON(offsetof(tOpponent_spec, field58_0x178) != 376);
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
    C2_HOOK_BUG_ON(offsetof(tCar_spec, softness_factor) != 6352);
    C2_HOOK_BUG_ON(sizeof(tCar_spec) != 6500);

    C2_HOOK_BUG_ON(offsetof(tProgram_state, sausage_eater_mode) != 16);
    C2_HOOK_BUG_ON(offsetof(tProgram_state, special_screens_count) != 7192);
    C2_HOOK_ASSERT((uintptr_t)&C2V(gProgram_state).sausage_eater_mode == 0x0075bb90);
    C2_HOOK_BUG_ON(sizeof(tProgram_state) != 24272);
    C2_HOOK_BUG_ON(sizeof(C2V(gTextureMaps)) != 0x1000);

    PDInitialiseSystem();

    C2V(gProgram_state).sausage_eater_mode = C2V(gSausage_override);
    InitQuickTimeStuff();
    c2_memset(C2V(gTextureMaps), 0, sizeof(C2V(gTextureMaps)));
    TWT_Init();
    MAMSInitMem();
    C2V(gAusterity_mode) = C2V(gAustere_override) || PDDoWeLeadAnAustereExistance();
    c2_srand(c2_time(NULL));
    BrV1dbBeginWrapper_Float();
    CreateStainlessClasses();
    PDInstallErrorHandlers();
    InstallDRMemCalls();
    InstallDRFileCalls();
    tTWTVFS twt = TWT_MountEx(C2V(gApplication_path));
    C2V(gApplicationDataTwtMounted) = 1;
    TemporaryMaterialStorageInit();
    PDInitTimer();
    InitWobbleStuff();
    LoadGeneralParameters();
    PrintMemoryDump(0, "AFTER LOADING GENERAL PARAMETERS");
    DefaultNetName();
    c2_strcpy(C2V(gProgram_state).player_name, "MAX DAMAGE");
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
    CheckDemo();
    InitDRFonts();
    C2V(gBig_font) = LoadBRFont("BIGFONT.FNT");
    C2V(gFont_7) = LoadBRFont("FONT7.FNT");
    C2V(gHeadup_font) = LoadBRFont("HEADUP.FNT");
    PrintMemoryDump(0, "AFTER LOADING FONTS");
    InitExplosions();
    PrintMemoryDump(0, "AFTER INITIALISING EXPLOSIONS");
    LoadMiscStrings();
    PrintMemoryDump(0, "AFTER LOADING MISC STRING");
    InitialiseStorageSpace(0, &C2V(gMisc_storage_space), 1000, 5, 1000, 1000, 10);
    LoadInRegistees();
    UpdateMap();
    PrintMemoryDump(0, "AFTER INITIALISING GENERAL STORAGE AND LOADING IN REG");
    InitHUDActor();
    InitLineStuff();
    InitSmokeStuff();
    PrintMemoryDump(0, "AFTER INITING LINE AND SMOKE STUFF");
    Init2DStuff();
    InitTintedPolys();
    C2V(gHud_tinted1) = CreateTintedPolyActor(0, 0, 640, 480, 2, 0, 0, 0);
    C2V(gHud_tinted2) = CreateTintedPolyActor(0, 0, 640, 480, 3, 2, 0, 0);
    PrintMemoryDump(0, "AFTER INITING 2D STUFF");
    // FUN_00486d80(); /* NOP */
    LoadInterfaceStrings();
    PrintMemoryDump(0, "AFTER INITING INTERFACE STRINGS");
    PrintMemoryDump(0, "AFTER INITING ACTION REPLAY");
    InitPhysics();
    PrintMemoryDump(0, "AFTER INITING PHYSICS");
    InitCrush();
    PrintMemoryDump(0, "AFTER INITING CRUSH SYSTEMS");
    LoadTreeSurgery();
    FlicPaletteAllocate();
    // FUN_00487eb0(); /* NOP */
    InitTransientBitmaps();
    PrintMemoryDump(0, "AFTER INITING OTHER INTERFACE STUFF");
    InitSound();
    PrintMemoryDump(0, "AFTER INITING SOUND");
    InitHeadups();
    PrintMemoryDump(0, "AFTER INITING HEADUPS");
    C2V(gDefault_track_material) = BrMaterialAllocate("gDefault_track_material");
    C2V(gDefault_track_material)->index_base = 227;
    C2V(gDefault_track_material)->index_range = 1;
    C2V(gDefault_track_material)->colour = ((br_colour*)C2V(gRender_palette)->pixels)[227];
    C2V(gDefault_track_material)->ka = 1.f;
    C2V(gDefault_track_material)->kd = 0.f;
    C2V(gDefault_track_material)->ks = 0.f;
    BrMaterialAdd(C2V(gDefault_track_material));
    InitPowerups();
    PrintMemoryDump(0, "AFTER INITING POWERUPS");
    InitShadows();
    PrintMemoryDump(0, "AFTER INITING SHADOW");
    InitFlics();
    AllocateStandardLamp();
    InitAIWorld();
    PrintMemoryDump(0, "AFTER INITING AI WORLD");
    LoadPowerups();
    PrintMemoryDump(0, "AFTER LOADING POWERUPS");
    LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), -1);
    PrintMemoryDump(0, "AFTER LOADING RACES");
    InitPaletteAnimate();
    InitRayCasting();
    InitDepthEffects();
    PrintMemoryDump(0, "AFTER INITING DEPTH EFFECTS");
    InitSmashing();
    PrintMemoryDump(0, "AFTER INITING SMASHING");
    InitialiseStorageSpace(0, &C2V(gOur_car_storage_space), 200, 2, 200, 200, 10);
    InitialiseStorageSpace(0, &C2V(gTheir_cars_storage_space), 1500, 50, 1500, 1500, 50);
    InitialiseStorageSpace(0, &C2V(gDroneStorage), 500, 50, 500, 228, 10);
    InitialiseStorageSpace(0, &C2V(gPedStorage), 200, 10, 100, 500, 200);
    InitialiseStorageSpace(0, &C2V(gTrack_storage_space), 1000, 50, 1000, 1000, 50);
    FinishLoadGeneralParameters();
    PrintMemoryDump(0, "AFTER FINISHING LOADING GENERAL");
    InitOilSpills();
    if (C2V(gAustere_time)) {
        while (PDGetTotalTime() - C2V(gAustere_time) < 2000) {
        }
    }
    ClearEntireScreen();
    InitSkids();
    PrintMemoryDump(0, "AFTER INITING SPILLS AND SKIDS");
    InitPeds();
    C2V(gProgram_state).track_spec.the_actor = NULL;
    C2V(gCD_is_in_drive) = TestForOriginalCarmaCDinDrive();
    LoadHeadupImages();
    InitGame(C2V(gDev_initial_race));
    BrMatrix34Identity(&C2V(gCamera_to_world));
    BrMatrix34Identity(&C2V(gRearview_camera_to_world));
    PrintMemoryDump(0, "AFTER APPLICATION INITIALISATION (JUST INITED PED SYSTEM)");
    c2_memset(C2V(gPickedup_powerups), 0, sizeof(C2V(gPickedup_powerups)));
    c2_memset(C2V(gUnknown_00705b80), 0, sizeof(C2V(gUnknown_00705b80)));
    TWT_UnmountEx(twt);
    C2V(gApplicationDataTwtMounted) = 0;
    C2V(gApp_initialized) = 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0047dd20, InitialiseApplication, InitialiseApplication_original)

int C2_HOOK_FASTCALL GetScreenSize(void) {

    return C2V(gRender_indent);
}
C2_HOOK_FUNCTION(0x00481e90, GetScreenSize)

void C2_HOOK_FASTCALL SetScreenSize(int pNew_size) {

    C2V(gRender_indent) = pNew_size;
}
C2_HOOK_FUNCTION(0x00481ea0, SetScreenSize)

void C2_HOOK_FASTCALL InitBRFonts(void) {

    C2V(gBig_font) = LoadBRFont("BIGFONT.FNT");
    C2V(gFont_7) = LoadBRFont("FONT7.FNT");
    C2V(gHeadup_font) = LoadBRFont("HEADUP.FNT");
}

void C2_HOOK_FASTCALL InitializeBRenderEnvironment(void) {

    C2V(gBr_initialized) = 1;
    SetBRenderScreenAndBuffers(0, 0, 0, 0);
    C2V(gUniverse_actor) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (C2V(gUniverse_actor) == NULL) {
        FatalError(3);
    }
    C2V(gUniverse_actor)->identifier = BrResStrDup(C2V(gUniverse_actor), "Root");
    BrEnvironmentSet(C2V(gUniverse_actor));
    C2V(gNon_track_actor) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (C2V(gNon_track_actor) == NULL) {
        FatalError(3);
    }
    BrActorAdd(C2V(gUniverse_actor), C2V(gNon_track_actor));
    C2V(gDont_render_actor) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (C2V(gDont_render_actor) == NULL) {
        FatalError(3);
    }
    C2V(gDont_render_actor)->render_style = BR_RSTYLE_NONE;
    BrActorAdd(C2V(gUniverse_actor), C2V(gDont_render_actor));
    AllocateSelf();
    AllocateCamera();
}

void (C2_HOOK_FASTCALL * AllocateSelf_original)(void);
void C2_HOOK_FASTCALL AllocateSelf(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    AllocateSelf_original();
#else
    int i;

    C2V(gSelf) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (C2V(gSelf) == NULL) {
        FatalError(kFatalError_CannotAllocateSelf);
    }
    C2V(gSelf) = BrActorAdd(C2V(gUniverse_actor), C2V(gSelf));
    if (C2V(gSelf) == NULL) {
        FatalError(kFatalError_CannotAllocateSelf);
    }

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gOther_selfs)) != 4);

    for (i = 0; i < REC2_ASIZE(C2V(gOther_selfs)); i++) {
        C2V(gOther_selfs)[i] = BrActorAllocate(BR_ACTOR_NONE, NULL);
        if (C2V(gOther_selfs)[i] == NULL) {
            FatalError(kFatalError_CannotAllocateSelf);
        }
        BrActorAdd(C2V(gSelf), C2V(gOther_selfs)[i]);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0047e320, AllocateSelf, AllocateSelf_original)

void (C2_HOOK_FASTCALL * AllocateCamera_original)(void);
void C2_HOOK_FASTCALL AllocateCamera(void) {

#if defined(C2_HOOKS_ENABLED)
    AllocateCamera_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0047e3b0, AllocateCamera, AllocateCamera_original)

static void inline Prepare2DModelAndMaterial(br_model* pModel, br_material* pMaterial, int pConfigure_vertices, br_colour pColour, int pFadeAlpha) {
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

static void inline Prepare2DModelToDim(br_model* pModel, int pX, int pY, int pW, int pH) {

    pModel->vertices[0].p.v[0] = pModel->vertices[1].p.v[0] = (float)pX;
    pModel->vertices[0].p.v[1] = pModel->vertices[3].p.v[1] = (float)-pY;
    pModel->vertices[2].p.v[0] = pModel->vertices[3].p.v[0] = pModel->vertices[1].p.v[0] + pW;
    pModel->vertices[1].p.v[1] = pModel->vertices[2].p.v[1] = pModel->vertices[3].p.v[1] - pH;
}

C2_HOOK_VARIABLE_DECLARE(br_actor*, gPrat_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, g2d_camera);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gPowerupHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gStatbarHUD1_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHeadup2_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHUDsquare_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gMapHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gTimerRightHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gOffense_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gTimerLeftHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gPower_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHUDcurve_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gStatbarRightHUD_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHeadup_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gStatbarHUD5_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gDim_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gArmour_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gHUD_root_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gStatbarHUD3_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gTestFont_actor);

C2_HOOK_VARIABLE_DECLARE(br_model*, gPrat_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gMapHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gStatbarRightHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gTestFont_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gHeadup_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gHUDsquare_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gTimerRightHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gOffence_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gTimerLeftHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gPower_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gStatbarHUD5_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gStatbarHUD3_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gStatbarHUD1_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gPowerupHUD_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gHeadup2_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gDim_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gHUDcurve_model);
C2_HOOK_VARIABLE_DECLARE(br_model*, gArmour_model);

C2_HOOK_VARIABLE_DECLARE(br_material*, gPowerupHUD_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gTestFont_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gArmour_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gStatbarHUD1_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gStatbarHUD5_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gHeadup_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gStatbarHUD3_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gOffence_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gPower_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gStatbarRightHUD_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gPrat_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gHUDcurve_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gMapHUD_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gHeadup2_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gTimerRightHUD_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gDim_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gHUDsquare_material);
C2_HOOK_VARIABLE_DECLARE(br_material*, gTimerLeftHUD_material);

C2_HOOK_VARIABLE_DECLARE(int, gHeadup_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gHeadup2_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup2_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup2_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup2_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gPowerupHUD_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gPowerupHUD_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gPowerupHUD_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gPowerupHUD_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD1_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD1_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD1_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD1_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD3_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD3_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD3_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD3_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gHUDsquare_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gHUDsquare_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gHUDsquare_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gHUDsquare_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD5_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD5_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD5_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gStatbarHUD5_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gTimerLeftHUD_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gTimerLeftHUD_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gTimerLeftHUD_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gTimerLeftHUD_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gTimerRightHUD_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gArmour_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gArmour_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gArmour_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gArmour_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gPower_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gPower_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gPower_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gPower_dim_y);

C2_HOOK_VARIABLE_DECLARE(int, gOffence_dim_w);
C2_HOOK_VARIABLE_DECLARE(int, gOffence_dim_h);
C2_HOOK_VARIABLE_DECLARE(int, gOffence_dim_x);
C2_HOOK_VARIABLE_DECLARE(int, gOffence_dim_y);

int C2_HOOK_FASTCALL Fix2DTextureWidth(int pWidth);
int C2_HOOK_FASTCALL Fix2DTextureHeight(int pHeight);

void (C2_HOOK_FASTCALL * Init2DStuff_original)(void);
void C2_HOOK_FASTCALL Init2DStuff(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    Init2DStuff_original();
#else
    tPath_name the_path;
    br_camera* camera_data;
    float w, h, w_fixed, h_fixed;

    C2V(gHUD_root_actor) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrMatrix34Translate(&C2V(gHUD_root_actor)->t.t.mat, 0.f, 0.f, 2.f);
    C2V(g2d_camera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    C2V(gDim_model) = BrModelAllocate("gDim_model", 4, 2);
    C2V(gDim_material) = BrMaterialAllocate("gDim_material");
    C2V(gDim_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    C2V(gPrat_model) = BrModelAllocate("gPrat_model", 4, 2);
    C2V(gPrat_material) = BrMaterialAllocate("gPrat_material");
    C2V(gPrat_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    PathCat(the_path, C2V(gApplication_path), "INTRFACE");
    PathCat(the_path, the_path, "BOTHUP");
    c2_strcat(the_path, ".DAT");

    C2V(gHeadup_model) = BrModelLoad(the_path);
    C2V(gHeadup_material) = BrMaterialAllocate("gHeadup_material");
    C2V(gHeadup_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gHeadup2_model) = BrModelAllocate("gHeadup2_model", 4, 2);
    C2V(gHeadup2_material) = BrMaterialAllocate("gHeadup2_material");
    C2V(gHeadup2_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gPowerupHUD_model) = BrModelAllocate("gPowerupHUD_model", 4, 2);
    C2V(gPowerupHUD_material) = BrMaterialAllocate("gPowerupHUD_material");
    C2V(gPowerupHUD_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gStatbarHUD1_model) = BrModelAllocate("gStatbarHUD1_model", 4, 2);
    C2V(gStatbarHUD1_material) = BrMaterialAllocate("gStatbarHUD1_material");
    C2V(gStatbarHUD1_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gHUDcurve_model) = BrModelAllocate("gHUDcurve_model", 4, 2);
    C2V(gHUDcurve_material) = BrMaterialAllocate("gHUDcurve_material");
    C2V(gHUDcurve_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gStatbarHUD3_model) = BrModelAllocate("gStatbarHUD3_model", 4, 2);
    C2V(gStatbarHUD3_material) = BrMaterialAllocate("gStatbarHUD3");
    C2V(gStatbarHUD3_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gHUDsquare_model) = BrModelAllocate("gHUDsquare_model", 4, 2);
    C2V(gHUDsquare_material) = BrMaterialAllocate("gHUDsquare_material");
    C2V(gHUDsquare_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gStatbarHUD5_model) = BrModelAllocate("gStatbarHUD5_model", 4, 2);
    C2V(gStatbarHUD5_material) = BrMaterialAllocate("gStatbarHUD5_material");
    C2V(gStatbarHUD5_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gTimerLeftHUD_model) = BrModelAllocate("gTimerLeftHUD_model", 4, 2);
    C2V(gTimerLeftHUD_material) = BrMaterialAllocate("gTimerLeftHUD_material");
    C2V(gTimerLeftHUD_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gTimerRightHUD_model) = BrModelAllocate("gTimerRightHUD_model", 4, 2);
    C2V(gTimerRightHUD_material) = BrMaterialAllocate("gTimerRightHUD_material");
    C2V(gTimerRightHUD_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gStatbarRightHUD_model) = BrModelAllocate("gStatbarRightHUD_model", 4, 2);
    C2V(gStatbarRightHUD_material) = BrMaterialAllocate("gStatbarRightHUD_material");
    C2V(gStatbarRightHUD_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gMapHUD_model) = BrModelAllocate("gMapHUD_model", 4, 2);
    C2V(gMapHUD_material) = BrMaterialAllocate("gMapHUD_material");
    C2V(gMapHUD_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gTestFont_model) = BrModelAllocate("gTestFont_model", 4, 2);
    C2V(gTestFont_material) = BrMaterialAllocate("gTestFont_material");
    C2V(gTestFont_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gArmour_model) = BrModelAllocate("gArmour_model", 4, 2);
    C2V(gArmour_material) = BrMaterialAllocate("gArmour_material");
    C2V(gArmour_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gPower_model) = BrModelAllocate("gPower_model", 4, 2);
    C2V(gPower_material) = BrMaterialAllocate("gPower_material");
    C2V(gPower_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    C2V(gOffence_model) = BrModelAllocate("gOffence_model", 4, 2);
    C2V(gOffence_material) = BrMaterialAllocate("gOffence_material");
    C2V(gOffense_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);

    if (C2V(gHUD_root_actor) == NULL
            || C2V(g2d_camera) == NULL
            || C2V(gDim_model) == NULL
            || C2V(gDim_material) == NULL
            || C2V(gDim_actor) == NULL
            || C2V(gPrat_model) == NULL
            || C2V(gPrat_material) == NULL
            || C2V(gPrat_actor) == NULL
            || C2V(gHeadup_model) == NULL
            || C2V(gHeadup_material) == NULL
            || C2V(gHeadup_actor) == NULL
            || C2V(gHeadup2_model) == NULL
            || C2V(gHeadup2_material) == NULL
            || C2V(gHeadup2_actor) == NULL
            || C2V(gPowerupHUD_model) == NULL
            || C2V(gPowerupHUD_material) == NULL
            || C2V(gPowerupHUD_actor) == NULL
            || C2V(gStatbarHUD1_model) == NULL
            || C2V(gStatbarHUD1_material) == NULL
            || C2V(gStatbarHUD1_actor) == NULL
            || C2V(gHUDcurve_model) == NULL
            || C2V(gHUDcurve_material) == NULL
            || C2V(gHUDcurve_actor) == NULL
            || C2V(gStatbarHUD3_model) == NULL
            || C2V(gStatbarHUD3_material) == NULL
            || C2V(gStatbarHUD3_actor) == NULL
            || C2V(gHUDsquare_model) == NULL
            || C2V(gHUDsquare_material) == NULL
            || C2V(gHUDsquare_actor) == NULL
            || C2V(gStatbarHUD5_model) == NULL
            || C2V(gStatbarHUD5_material) == NULL
            || C2V(gStatbarHUD5_actor) == NULL
            || C2V(gTimerLeftHUD_model) == NULL
            || C2V(gTimerLeftHUD_material) == NULL
            || C2V(gTimerLeftHUD_actor) == NULL
            || C2V(gTimerRightHUD_model) == NULL
            || C2V(gTimerRightHUD_material) == NULL
            || C2V(gTimerRightHUD_actor) == NULL
            || C2V(gStatbarRightHUD_model) == NULL
            || C2V(gStatbarRightHUD_material) == NULL
            || C2V(gStatbarRightHUD_actor) == NULL
            || C2V(gMapHUD_model) == NULL
            || C2V(gMapHUD_material) == NULL
            || C2V(gMapHUD_actor) == NULL
            || C2V(gTestFont_model) == NULL
            || C2V(gTestFont_material) == NULL
            || C2V(gTestFont_actor) == NULL
            || C2V(gArmour_model) == NULL
            || C2V(gArmour_material) == NULL
            || C2V(gArmour_actor) == NULL
            || C2V(gPower_model) == NULL
            || C2V(gPower_material) == NULL
            || C2V(gPower_actor) == NULL
            || C2V(gOffence_model) == NULL
            || C2V(gOffence_material) == NULL
            || C2V(gOffense_actor) == NULL) {
        FatalError(kFatalError_OOM_S);
    }

    C2V(g2d_camera)->identifier = "A 2D camera, possibly even g2d_camera";
    camera_data = C2V(g2d_camera)->type_data;
    camera_data->type = BR_CAMERA_PARALLEL;
    camera_data->hither_z = 1.f;
    camera_data->yon_z = 3.f;
    camera_data->width = (float)C2V(gScreen)->width;
    camera_data->height = (float)C2V(gScreen)->height;

    C2V(gDim_actor)->identifier = "gDim_actor";
    C2V(gDim_actor)->model = C2V(gDim_model);
    C2V(gDim_actor)->material = C2V(gDim_material);
    Prepare2DModelAndMaterial(C2V(gDim_model), C2V(gDim_material), 1, BR_COLOUR_RGB(0, 0, 0), 1);
    BrModelAdd(C2V(gDim_model));
    BrMaterialAdd(C2V(gDim_material));

    C2V(gPrat_actor)->identifier ="gPrat_actor";
    C2V(gPrat_actor)->model = C2V(gPrat_model);
    C2V(gPrat_actor)->material = C2V(gPrat_material);
    Prepare2DModelAndMaterial(C2V(gPrat_model), C2V(gPrat_material), 1, BR_COLOUR_RGB(0xff, 0xff, 0xff), 0);
    BrVector2Set(&C2V(gPrat_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gPrat_model)->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&C2V(gPrat_model)->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&C2V(gPrat_model)->vertices[3].map, 1.f, 0.f);
    BrModelAdd(C2V(gPrat_model));
    BrMaterialAdd(C2V(gPrat_material));

    C2V(gHeadup_actor)->identifier = "gHeadup_actor";
    C2V(gHeadup_actor)->model = C2V(gHeadup_model);
    C2V(gHeadup_actor)->material = C2V(gHeadup_material);
    Prepare2DModelAndMaterial(C2V(gHeadup_model), C2V(gHeadup_material), 0, BR_COLOUR_RGB(0x04, 0x0e, 0x4a), 1);
    w = (float)C2V(gHeadup_dim_w);
    h = (float)C2V(gHeadup_dim_w);
    w_fixed = (float)Fix2DTextureWidth(C2V(gHeadup_dim_w));
    h_fixed = (float)Fix2DTextureHeight(C2V(gHeadup_dim_w));
    BrVector2Set(&C2V(gHeadup_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gHeadup_model)->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&C2V(gHeadup_model)->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&C2V(gHeadup_model)->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(C2V(gHeadup_model));
    BrMaterialAdd(C2V(gHeadup_material));
    BrMatrix34Translate(&C2V(gHeadup_actor)->t.t.mat,
            (float)(C2V(gHeadup_dim_x) + 10 + C2V(gHeadup_dim_w)),
            (float)(-C2V(gHeadup_dim_h) / 2 - C2V(gHeadup_dim_y)),
            -2.f);

    C2V(gHeadup2_actor)->identifier = "gHeadup2_actor";
    C2V(gHeadup2_actor)->model = C2V(gHeadup2_model);
    C2V(gHeadup2_actor)->material = C2V(gHeadup2_material);
    Prepare2DModelAndMaterial(C2V(gHeadup2_model), C2V(gHeadup2_material), 1, BR_COLOUR_RGB(0x04, 0x0e, 0x4a), 1);
    w = (float)C2V(gHeadup2_dim_w);
    h = (float)C2V(gHeadup2_dim_h);
    w_fixed = (float)Fix2DTextureWidth(C2V(gHeadup2_dim_w));
    h_fixed = (float)Fix2DTextureHeight(C2V(gHeadup2_dim_h));
    BrVector2Set(&C2V(gHeadup2_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gHeadup2_model)->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&C2V(gHeadup2_model)->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&C2V(gHeadup2_model)->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(C2V(gHeadup2_model));
    BrMaterialAdd(C2V(gHeadup2_material));

    C2V(gPowerupHUD_actor)->identifier = "gPowerupHUD_actor";
    C2V(gPowerupHUD_actor)->model = C2V(gPowerupHUD_model);
    C2V(gPowerupHUD_actor)->material = C2V(gPowerupHUD_material);
    Prepare2DModelAndMaterial(C2V(gPowerupHUD_model), C2V(gPowerupHUD_material), 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)C2V(gPowerupHUD_dim_w);
    h = (float)C2V(gPowerupHUD_dim_h);
    w_fixed = (float)Fix2DTextureWidth(C2V(gPowerupHUD_dim_w));
    h_fixed = (float)Fix2DTextureHeight(C2V(gPowerupHUD_dim_h));
    BrVector2Set(&C2V(gPowerupHUD_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gPowerupHUD_model)->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&C2V(gPowerupHUD_model)->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&C2V(gPowerupHUD_model)->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(C2V(gPowerupHUD_model));
    BrMaterialAdd(C2V(gPowerupHUD_material));

    C2V(gStatbarHUD1_actor)->identifier = "gStatbarHUD1_actor";
    C2V(gStatbarHUD1_actor)->model = C2V(gStatbarHUD1_model);
    C2V(gStatbarHUD1_actor)->material = C2V(gStatbarHUD1_material);
    Prepare2DModelAndMaterial(C2V(gStatbarHUD1_model), C2V(gStatbarHUD1_material), 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)C2V(gStatbarHUD1_dim_w);
    h = (float)C2V(gStatbarHUD1_dim_h);
    w_fixed = (float)Fix2DTextureWidth(C2V(gStatbarHUD1_dim_w));
    h_fixed = (float)Fix2DTextureHeight(C2V(gStatbarHUD1_dim_h));
    BrVector2Set(&C2V(gStatbarHUD1_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gStatbarHUD1_model)->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&C2V(gStatbarHUD1_model)->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&C2V(gStatbarHUD1_model)->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(C2V(gStatbarHUD1_model));
    BrMaterialAdd(C2V(gStatbarHUD1_material));

    C2V(gHUDcurve_actor)->identifier = "gHUDcurve_actor";
    C2V(gHUDcurve_actor)->model = C2V(gHUDcurve_model);
    C2V(gHUDcurve_actor)->material = C2V(gHUDcurve_material);
    Prepare2DModelAndMaterial(C2V(gHUDcurve_model), C2V(gHUDcurve_material), 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    BrVector2Set(&C2V(gPrat_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gPrat_model)->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&C2V(gPrat_model)->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&C2V(gPrat_model)->vertices[3].map, 1.f, 0.f);
    BrModelAdd(C2V(gHUDcurve_model));
    BrMaterialAdd(C2V(gHUDcurve_material));

    C2V(gStatbarHUD3_actor)->identifier = "gStatbarHUD3_actor";
    C2V(gStatbarHUD3_actor)->model = C2V(gStatbarHUD3_model);
    C2V(gStatbarHUD3_actor)->material = C2V(gStatbarHUD3_material);
    Prepare2DModelAndMaterial(C2V(gStatbarHUD3_model), C2V(gStatbarHUD3_material), 1, BR_COLOUR_RGB(0x04, 0x0e, 0x4a), 1);
    w = (float)C2V(gStatbarHUD3_dim_w);
    h = (float)C2V(gStatbarHUD3_dim_h);
    w_fixed = (float)Fix2DTextureWidth(C2V(gStatbarHUD3_dim_w));
    h_fixed = (float)Fix2DTextureHeight(C2V(gStatbarHUD3_dim_h));
    BrVector2Set(&C2V(gStatbarHUD3_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gStatbarHUD3_model)->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&C2V(gStatbarHUD3_model)->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&C2V(gStatbarHUD3_model)->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(C2V(gStatbarHUD3_model));
    BrMaterialAdd(C2V(gStatbarHUD3_material));

    C2V(gHUDsquare_actor)->identifier = "gHUDsquare_actor";
    C2V(gHUDsquare_actor)->model = C2V(gHUDsquare_model);
    C2V(gHUDsquare_actor)->material = C2V(gHUDsquare_material);
    Prepare2DModelAndMaterial(C2V(gHUDsquare_model), C2V(gHUDsquare_material), 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)C2V(gHUDsquare_dim_w);
    h = (float)C2V(gHUDsquare_dim_h);
    w_fixed = (float)Fix2DTextureWidth(C2V(gHUDsquare_dim_w));
    h_fixed = (float)Fix2DTextureHeight(C2V(gHUDsquare_dim_h));
    BrVector2Set(&C2V(gHUDsquare_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gHUDsquare_model)->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&C2V(gHUDsquare_model)->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&C2V(gHUDsquare_model)->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(C2V(gHUDsquare_model));
    BrMaterialAdd(C2V(gHUDsquare_material));

    C2V(gStatbarHUD5_actor)->identifier = "gStatbarHUD5_actor";
    C2V(gStatbarHUD5_actor)->model = C2V(gStatbarHUD5_model);
    C2V(gStatbarHUD5_actor)->material = C2V(gStatbarHUD5_material);
    Prepare2DModelAndMaterial(C2V(gStatbarHUD5_model), C2V(gStatbarHUD5_material), 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)C2V(gStatbarHUD5_dim_w);
    h = (float)C2V(gStatbarHUD5_dim_h);
    w_fixed = (float)Fix2DTextureWidth(C2V(gStatbarHUD5_dim_w));
    h_fixed = (float)Fix2DTextureHeight(C2V(gStatbarHUD5_dim_h));
    BrVector2Set(&C2V(gStatbarHUD5_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gStatbarHUD5_model)->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&C2V(gStatbarHUD5_model)->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&C2V(gStatbarHUD5_model)->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(C2V(gStatbarHUD5_model));
    BrMaterialAdd(C2V(gStatbarHUD5_material));

    C2V(gTimerLeftHUD_actor)->identifier = "gTimerLeftHUD_actor";
    C2V(gTimerLeftHUD_actor)->model = C2V(gTimerLeftHUD_model);
    C2V(gTimerLeftHUD_actor)->material = C2V(gTimerLeftHUD_material);
    Prepare2DModelAndMaterial(C2V(gTimerLeftHUD_model), C2V(gTimerLeftHUD_material), 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    BrVector2Set(&C2V(gTimerLeftHUD_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gTimerLeftHUD_model)->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&C2V(gTimerLeftHUD_model)->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&C2V(gTimerLeftHUD_model)->vertices[3].map, 1.f, 0.f);
    BrModelAdd(C2V(gTimerLeftHUD_model));
    BrMaterialAdd(C2V(gTimerLeftHUD_material));

    C2V(gTimerRightHUD_actor)->identifier = "gTimerRightHUD_actor";
    C2V(gTimerRightHUD_actor)->model = C2V(gTimerRightHUD_model);
    C2V(gTimerRightHUD_actor)->material = C2V(gTimerRightHUD_material);
    Prepare2DModelAndMaterial(C2V(gTimerRightHUD_model), C2V(gTimerRightHUD_material), 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    w = (float)C2V(gTimerRightHUD_dim_w);
    h = (float)C2V(gTimerRightHUD_dim_h);
    w_fixed = (float)Fix2DTextureWidth(C2V(gTimerRightHUD_dim_w));
    h_fixed = (float)Fix2DTextureHeight(C2V(gTimerRightHUD_dim_h));
    BrVector2Set(&C2V(gTimerRightHUD_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gTimerRightHUD_model)->vertices[1].map, 0.f, h / h_fixed);
    BrVector2Set(&C2V(gTimerRightHUD_model)->vertices[2].map, w / w_fixed, h / h_fixed);
    BrVector2Set(&C2V(gTimerRightHUD_model)->vertices[3].map, w / w_fixed, 0.f);
    BrModelAdd(C2V(gTimerRightHUD_model));
    BrMaterialAdd(C2V(gTimerRightHUD_material));

    C2V(gStatbarRightHUD_actor)->identifier = "gStatbarRightHUD_actor";
    C2V(gStatbarRightHUD_actor)->model = C2V(gStatbarRightHUD_model);
    C2V(gStatbarRightHUD_actor)->material = C2V(gStatbarRightHUD_material);
    Prepare2DModelAndMaterial(C2V(gStatbarRightHUD_model), C2V(gStatbarRightHUD_material), 1, BR_COLOUR_RGB(0x04, 0x0e, 0x4e), 1);
    BrModelAdd(C2V(gStatbarRightHUD_model));
    BrMaterialAdd(C2V(gStatbarRightHUD_material));

    C2V(gMapHUD_actor)->identifier = "gMapHUD_actor";
    C2V(gMapHUD_actor)->model = C2V(gMapHUD_model);
    C2V(gMapHUD_actor)->material = C2V(gMapHUD_material);
    Prepare2DModelAndMaterial(C2V(gMapHUD_model), C2V(gMapHUD_material), 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    BrModelAdd(C2V(gMapHUD_model));
    BrMaterialAdd(C2V(gMapHUD_material));

    C2V(gTestFont_actor)->identifier = "gTestFont_actor";
    C2V(gTestFont_actor)->model = C2V(gTestFont_model);
    C2V(gTestFont_actor)->material = C2V(gTestFont_material);
    Prepare2DModelAndMaterial(C2V(gTestFont_model), C2V(gTestFont_material), 1, BR_COLOUR_RGB(0x22, 0x35, 0x51), 0);
    BrModelAdd(C2V(gTestFont_model));
    BrMaterialAdd(C2V(gTestFont_material));

    C2V(gArmour_actor)->identifier = "gArmour_actor";
    C2V(gArmour_actor)->model = C2V(gArmour_model);
    C2V(gArmour_actor)->material = C2V(gArmour_material);
    Prepare2DModelAndMaterial(C2V(gArmour_model), C2V(gArmour_material), 1, BR_COLOUR_RGB(0x96, 0x96, 0x96), 0);
    BrVector2Set(&C2V(gArmour_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gArmour_model)->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&C2V(gArmour_model)->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&C2V(gArmour_model)->vertices[3].map, 1.f, 0.f);
    BrModelAdd(C2V(gArmour_model));
    BrMaterialAdd(C2V(gArmour_material));

    C2V(gPower_actor)->identifier = "gPower_actor";
    C2V(gPower_actor)->model = C2V(gPower_model);
    C2V(gPower_actor)->material = C2V(gPower_material);
    Prepare2DModelAndMaterial(C2V(gPower_model), C2V(gPower_material), 1, BR_COLOUR_RGB(0x22, 0x35, 0x51), 0);
    BrVector2Set(&C2V(gPower_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gPower_model)->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&C2V(gPower_model)->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&C2V(gPower_model)->vertices[3].map, 1.f, 0.f);
    BrModelAdd(C2V(gPower_model));
    BrMaterialAdd(C2V(gPower_material));

    C2V(gOffense_actor)->identifier = "gOffense_actor";
    C2V(gOffense_actor)->model = C2V(gOffence_model);
    C2V(gOffense_actor)->material = C2V(gOffence_material);
    Prepare2DModelAndMaterial(C2V(gOffence_model), C2V(gOffence_material), 1, BR_COLOUR_RGB(0x22, 0x35, 0x51), 0);
    BrVector2Set(&C2V(gOffence_model)->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&C2V(gOffence_model)->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&C2V(gOffence_model)->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&C2V(gOffence_model)->vertices[3].map, 1.f, 0.f);
    BrModelAdd(C2V(gOffence_model));
    BrMaterialAdd(C2V(gOffence_material));

    Prepare2DModelToDim(C2V(gHeadup2_model), C2V(gHeadup2_dim_x), C2V(gHeadup2_dim_y), C2V(gHeadup2_dim_w), C2V(gHeadup2_dim_h));
    BrModelUpdate(C2V(gHeadup2_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gPowerupHUD_model), C2V(gPowerupHUD_dim_x), C2V(gPowerupHUD_dim_y), C2V(gPowerupHUD_dim_w), C2V(gPowerupHUD_dim_h));
    BrModelUpdate(C2V(gPowerupHUD_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gStatbarHUD1_model), C2V(gStatbarHUD1_dim_x), C2V(gStatbarHUD1_dim_y), C2V(gStatbarHUD1_dim_w), C2V(gStatbarHUD1_dim_h));
    BrModelUpdate(C2V(gStatbarHUD1_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gStatbarHUD3_model), C2V(gStatbarHUD3_dim_x), C2V(gStatbarHUD3_dim_y), C2V(gStatbarHUD3_dim_w), C2V(gStatbarHUD3_dim_h));
    BrModelUpdate(C2V(gStatbarHUD3_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gHUDsquare_model), C2V(gHUDsquare_dim_x), C2V(gHUDsquare_dim_y), C2V(gHUDsquare_dim_w), C2V(gHUDsquare_dim_h));
    BrModelUpdate(C2V(gHUDsquare_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gStatbarHUD5_model), C2V(gStatbarHUD5_dim_x), C2V(gStatbarHUD5_dim_y), C2V(gStatbarHUD5_dim_w), C2V(gStatbarHUD5_dim_h));
    BrModelUpdate(C2V(gStatbarHUD5_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gTimerLeftHUD_model), C2V(gTimerLeftHUD_dim_x), C2V(gTimerLeftHUD_dim_y), C2V(gTimerLeftHUD_dim_w), C2V(gTimerLeftHUD_dim_h));
    BrModelUpdate(C2V(gTimerLeftHUD_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gTimerRightHUD_model), C2V(gTimerRightHUD_dim_x), C2V(gTimerRightHUD_dim_y), C2V(gTimerRightHUD_dim_w), C2V(gTimerRightHUD_dim_h));
    BrModelUpdate(C2V(gTimerRightHUD_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gStatbarRightHUD_model), C2V(gStatbarRightHUD_dim_x), C2V(gStatbarRightHUD_dim_y), C2V(gStatbarRightHUD_dim_w), C2V(gStatbarRightHUD_dim_h));
    BrModelUpdate(C2V(gStatbarRightHUD_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gArmour_model), C2V(gArmour_dim_x), C2V(gArmour_dim_y), C2V(gArmour_dim_w), C2V(gArmour_dim_h));
    BrModelUpdate(C2V(gArmour_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gPower_model), C2V(gPower_dim_x), C2V(gPower_dim_y), C2V(gPower_dim_w), C2V(gPower_dim_h));
    BrModelUpdate(C2V(gPower_model), BR_MODU_VERTEX_POSITIONS);

    Prepare2DModelToDim(C2V(gOffence_model), C2V(gOffence_dim_x), C2V(gOffence_dim_y), C2V(gOffence_dim_w), C2V(gOffence_dim_h));
    BrModelUpdate(C2V(gOffence_model), BR_MODU_VERTEX_POSITIONS);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0047e720, Init2DStuff, Init2DStuff_original)

void (C2_HOOK_FASTCALL * InitLineStuff_original)(void);
void C2_HOOK_FASTCALL InitLineStuff(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitLineStuff_original();
#else

    C2V(gLine_model) = BrModelAllocate("gLine_model", 2, 1);
    C2V(gLine_material) = BrMaterialAllocate("gLine_material");
    C2V(gLine_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    if (C2V(gLine_model) == NULL || C2V(gLine_material) == NULL || C2V(gLine_actor) == NULL) {
        FatalError(kFatalError_OOM_S);
    }
    C2V(gLine_actor)->identifier = "gLine_actor";
    C2V(gLine_actor)->model = C2V(gLine_model);
    C2V(gLine_actor)->material = C2V(gLine_material);
    C2V(gLine_actor)->render_style = BR_RSTYLE_EDGES;
    C2V(gLine_model)->flags = BR_MODF_KEEP_ORIGINAL | BR_MODF_QUICK_UPDATE;
    C2V(gLine_model)->faces->vertices[0] = 0;
    C2V(gLine_model)->faces->vertices[1] = 0;
    C2V(gLine_model)->faces->vertices[2] = 1;
    C2V(gLine_material)->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_TWO_SIDED;
    C2V(gLine_model)->faces->flags = 5;
    BrModelAdd(C2V(gLine_model));
    BrMaterialAdd(C2V(gLine_material));
    BrActorAdd(C2V(gDont_render_actor), C2V(gLine_actor));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0047e610, InitLineStuff, InitLineStuff_original)

void (C2_HOOK_FASTCALL * InitSmokeStuff_original)(void);
void C2_HOOK_FASTCALL InitSmokeStuff(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    InitSmokeStuff_original();
#else
    int i;
    br_pixelmap* smoke_pm;
    static br_token_value fadealpha[3] = {
        { BRT_BLEND_B, { 1 } },
        { BRT_OPACITY_X, { 0x4b0000 } }, /* ~ 29.296875% = 75 / 256 */
        { 0 },
    };

    C2V(gBlend_model) = BrModelAllocate("gBlend_model", 4, 2);
    C2V(gBlend_model2) = BrModelAllocate("gBlend_model2", 6, 4);
    C2V(gBlend_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    for (i = 0; i < REC2_ASIZE(C2V(gSmoke_infos)); i++) {
        C2V(gSmoke_infos)[i].material = BrMaterialAllocate("some smoke");
    }
    if (C2V(gBlend_model) == NULL || C2V(gBlend_actor) == NULL) {
        FatalError(kFatalError_OOM_S);
    }
    C2V(gBlend_actor)->identifier = "gBlend_actor";
    C2V(gBlend_actor)->model = C2V(gBlend_model);

    C2V(gBlend_model)->faces[0].vertices[0] = 0;
    C2V(gBlend_model)->faces[0].vertices[1] = 1;
    C2V(gBlend_model)->faces[0].vertices[2] = 2;
    C2V(gBlend_model)->faces[1].vertices[0] = 2;
    C2V(gBlend_model)->faces[1].vertices[1] = 3;
    C2V(gBlend_model)->faces[1].vertices[2] = 0;

    C2V(gBlend_model2)->faces[0].vertices[0] = 0;
    C2V(gBlend_model2)->faces[0].vertices[1] = 1;
    C2V(gBlend_model2)->faces[0].vertices[2] = 2;
    C2V(gBlend_model2)->faces[1].vertices[0] = 2;
    C2V(gBlend_model2)->faces[1].vertices[1] = 3;
    C2V(gBlend_model2)->faces[1].vertices[2] = 0;
    C2V(gBlend_model2)->faces[2].vertices[0] = 1;
    C2V(gBlend_model2)->faces[2].vertices[1] = 5;
    C2V(gBlend_model2)->faces[2].vertices[2] = 2;
    C2V(gBlend_model2)->faces[3].vertices[0] = 1;
    C2V(gBlend_model2)->faces[3].vertices[1] = 4;
    C2V(gBlend_model2)->faces[3].vertices[2] = 5;

    BrVector3Set(&C2V(gBlend_model)->vertices[0].p, -1.f,  1.f, 0.f);
    BrVector3Set(&C2V(gBlend_model)->vertices[1].p, -1.f, -1.f, 0.f);
    BrVector3Set(&C2V(gBlend_model)->vertices[2].p,  1.f, -1.f, 0.f);
    BrVector3Set(&C2V(gBlend_model)->vertices[3].p,  1.f,  1.f, 0.f);

    BrVector3Set(&C2V(gBlend_model2)->vertices[0].p, -1.f, 1.f, 0.f);
    BrVector3Set(&C2V(gBlend_model2)->vertices[1].p, -1.f, 0.f, 0.f);
    BrVector3Set(&C2V(gBlend_model2)->vertices[2].p,  1.f, 0.f, 0.f);
    BrVector3Set(&C2V(gBlend_model2)->vertices[3].p,  1.f, 1.f, 0.f);
    BrVector3Set(&C2V(gBlend_model2)->vertices[4].p, -1.f, -.25f, 0.f);
    BrVector3Set(&C2V(gBlend_model2)->vertices[5].p,  1.f, -.25f, 0.f);
    C2V(gBlend_model)->flags |= BR_MODF_KEEP_ORIGINAL;
    C2V(gBlend_model2)->flags |= BR_MODF_KEEP_ORIGINAL;

    smoke_pm = DRLoadPixelmap("SMOKE.PIX");
    if (smoke_pm == NULL) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, "SMOKE.PIX");
    }
    smoke_pm->map = C2V(gRender_palette);
    BrMapAdd(smoke_pm);

    for (i = 0; i < REC2_ASIZE(C2V(gSmoke_infos)); i++) {
        C2V(gSmoke_infos)[i].material->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_PERSPECTIVE;
        C2V(gSmoke_infos)[i].material->extra_prim = fadealpha;
        C2V(gSmoke_infos)[i].material->colour_map = smoke_pm;
        BrMaterialAdd(C2V(gSmoke_infos)[i].material);
    }

    BrVector2Set(&C2V(gBlend_model)->vertices[0].map, 0.f, 1.f - 1.f / (float)smoke_pm->height);
    BrVector2Set(&C2V(gBlend_model)->vertices[1].map, 0.f, 0.f);
    BrVector2Set(&C2V(gBlend_model)->vertices[2].map, 1.f - 1.f / (float)smoke_pm->width, 0.f);
    BrVector2Set(&C2V(gBlend_model)->vertices[3].map, 1.f - 1.f / (float)smoke_pm->width, 1.f - 1.f / (float)smoke_pm->height);

    BrVector2Set(&C2V(gBlend_model2)->vertices[0].map, 0.f, 1.f - 1.f / (float)smoke_pm->height);
    BrVector2Set(&C2V(gBlend_model2)->vertices[1].map, 0.f, 0.5f);
    BrVector2Set(&C2V(gBlend_model2)->vertices[2].map, 1.f - 1.f / (float)smoke_pm->width, .5f);
    BrVector2Set(&C2V(gBlend_model2)->vertices[3].map, 1.f - 1.f / (float)smoke_pm->width, 1.f - 1.f / (float)smoke_pm->height);
    BrVector2Set(&C2V(gBlend_model2)->vertices[4].map, 0.f, 0.f);
    BrVector2Set(&C2V(gBlend_model2)->vertices[5].map, 1.f - 1.f / (float)smoke_pm->width, 0.f);

    BrModelAdd(C2V(gBlend_model));
    BrModelAdd(C2V(gBlend_model2));

    BrActorAdd(C2V(gDont_render_actor), C2V(gBlend_actor));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f9fc0, InitSmokeStuff, InitSmokeStuff_original)

void (C2_HOOK_FASTCALL * AllocateStandardLamp_original)(void);
void C2_HOOK_FASTCALL AllocateStandardLamp(void) {

#if defined(C2_HOOKS_ENABLED)
    AllocateStandardLamp_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0047e500, AllocateStandardLamp, AllocateStandardLamp_original)

void (C2_HOOK_FASTCALL * InitGame_original)(int pStart_race);
void C2_HOOK_FASTCALL InitGame(int pStart_race) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitGame_original(pStart_race);
#else
    int i;

#ifndef _MSC_VER
    C2_HOOK_BUG_ON((uintptr_t)&C2V(gCurrent_race).number_of_racers != 0x00762430);
#endif

    C2V(gProgram_state).field_0x2c = 0;
    C2V(gNo_current_game) = 0;
    C2V(gWaiting_for_unpause) = 1;
    C2V(gWait_for_it) = 0;
    C2V(gGame_to_load) = -1;
    C2V(gCurrent_race).number_of_racers = 0;
    TemporaryMaterialStorageInit();
    DisableMaterialAdapt();
    C2V(gGame_initialized) = 1;
    C2V(gNet_mode_of_last_game) = C2V(gNet_mode);
    C2V(gNo_races_yet) = 1;
    NetPlayerStatusChanged(ePlayer_status_loading);
    C2V(gCurrent_race_group) = C2V(gRace_list)[pStart_race].group;
    C2V(gIs_boundary_race) = C2V(gRace_list)[pStart_race].is_boundary;
    C2V(gProgram_state).current_race_index = pStart_race;
    for (i = 0; i < C2V(gNumber_of_races); i++) {
        C2V(gRace_list)[i].count_opponents = 0;
    }
    C2_HOOK_BUG_ON(sizeof(tOpponent) != 448);
    for (i = 0; i < C2V(gNumber_of_racers); i++) {
        C2V(gOpponents)[i].dead = 0;
    }
    C2V(gProgram_state).rank = C2V(gInitial_rank);
    C2V(gProgram_state).number_of_cars = 1;
    C2V(gProgram_state).credits_per_rank = C2V(gCredits_per_rank)[C2V(gProgram_state).skill_level];
    C2V(gProgram_state).credits = C2V(gStarting_money)[C2V(gProgram_state).skill_level];
    C2V(gProgram_state).redo_race_index = -1;
    C2V(gProgram_state).cars_available[0] = 0;
    C2V(gProgram_state).current_car_index = 0;
    C2V(gProgram_state).game_completed = 0;
    for (i = 0; i < REC2_ASIZE(C2V(gInitial_APO)); i++) {
        C2V(gCurrent_APO_levels)[i] = C2V(gNet_mode) == eNet_mode_none ? C2V(gInitial_APO)[i].initial[C2V(gProgram_state).skill_level] : C2V(gInitial_APO)[i].initial_network[C2V(gCurrent_net_game)->type];
        C2V(gCurrent_APO_potential_levels)[i] = C2V(gNet_mode) == eNet_mode_none ? C2V(gInitial_APO_potential)[i].initial[C2V(gProgram_state).skill_level] : C2V(gInitial_APO_potential)[i].initial_network[C2V(gCurrent_net_game)->type];
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004816b0, InitGame, InitGame_original)

int C2_HOOK_FASTCALL Fix2DTextureWidth(int pWidth) {
    int new_width;

    if (C2V(gUse_actor_dimming) && C2V(gTexture_power_of_2)) {
        new_width = 1;
        while (pWidth > new_width) {
            new_width *= 2;
        }
    } else {
        new_width = pWidth;
    }
    return new_width;
}
C2_HOOK_FUNCTION(0x0044ba60, Fix2DTextureWidth)

int C2_HOOK_FASTCALL Fix2DTextureHeight(int pHeight) {
    int new_height;

    if (C2V(gUse_actor_dimming) && C2V(gTexture_power_of_2)) {
        new_height = 1;
        while (pHeight > new_height) {
            new_height *= 2;
        }
    } else {
        new_height = pHeight;
    }
    return new_height;

}
C2_HOOK_FUNCTION(0x0044ba90, Fix2DTextureHeight)

void C2_HOOK_FASTCALL DisposeGameIfNecessary(void) {
    int i;

    dr_dprintf("DisposeGameIfNecessary() - START");
    if (C2V(gNet_mode) != eNet_mode_none) {
        NetLeaveGame(C2V(gCurrent_net_game));
    }
    dr_dprintf("DisposeGameIfNecessary() - Point 1");
    if (C2V(gGame_initialized)) {
        C2V(gGame_initialized) = 0;
        dr_dprintf("DisposeGameIfNecessary() - Point 2");
        if (C2V(gNet_mode_of_last_game) != eNet_mode_none) {
            for (i = 0; i < C2V(gNumber_of_net_players); i++) {
                DisposeCar(C2V(gNet_players)[i].car, C2V(gNet_players)[i].car_index);
            }
            dr_dprintf("DisposeGameIfNecessary() - Point 3");
            PossibleService();
            LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), -1);
            DisposeStorageSpace(&C2V(gNet_cars_storage_space));
            C2V(gNo_current_game) = 1;
            dr_dprintf("DisposeGameIfNecessary() - Point 4");
        }
        dr_dprintf("DisposeGameIfNecessary() - Point 5");
    }
    ShutdownNetIfRequired();
    dr_dprintf("DisposeGameIfNecessary() - END");
}
C2_HOOK_FUNCTION(0x0044c130, DisposeGameIfNecessary)

void C2_HOOK_FASTCALL ReinitialiseRenderStuff(void) {
    int x_diff;
    int y_diff;

    if (C2V(gProgram_state).cockpit_on) {
        C2V(gProgram_state).current_render_left = C2V(gProgram_state).current_car.render_left[C2V(gProgram_state).cockpit_image_index];
        C2V(gProgram_state).current_render_top = C2V(gProgram_state).current_car.render_top[C2V(gProgram_state).cockpit_image_index];
        C2V(gProgram_state).current_render_right = C2V(gProgram_state).current_car.render_right[C2V(gProgram_state).cockpit_image_index];
        C2V(gProgram_state).current_render_bottom = C2V(gProgram_state).current_car.render_bottom[C2V(gProgram_state).cockpit_image_index];
    } else {
        if (C2V(gNo_render_indent)) {
            C2V(gProgram_state).current_render_right = C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width;
            C2V(gProgram_state).current_render_bottom = C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height;
            C2V(gProgram_state).current_render_left = 0;
            C2V(gProgram_state).current_render_top = 0;
        } else {
            C2V(gProgram_state).current_render_top = (C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height / 18 & ~1) * C2V(gRender_indent);
            C2V(gProgram_state).current_render_left = (C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width / 18 & ~3) * C2V(gRender_indent);
            x_diff = C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width - C2V(gProgram_state).current_render_left;
            y_diff = C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height - C2V(gProgram_state).current_render_top;
            C2V(gProgram_state).current_render_right = x_diff;
            C2V(gProgram_state).current_render_bottom = y_diff;

        }
    }
}
C2_HOOK_FUNCTION(0x0047dc30, ReinitialiseRenderStuff)

void C2_HOOK_FASTCALL ReinitialiseForwardCamera(void) {
    br_camera* camera_ptr;
    float the_angle;
    float d;
    float w;

    camera_ptr = C2V(gCamera)->type_data;
    if (C2V(gProgram_state).cockpit_on) {
        the_angle = C2V(gCamera_angle) / 2.f;

        d = atanf(
                tandeg(the_angle)
                * (float)C2V(gRender_screen)->height
                / (float)(C2V(gProgram_state).current_car.render_bottom[0] - C2V(gProgram_state).current_car.render_top[0]))
            * (br_scalar)(2 * 180. / PI);
        camera_ptr->field_of_view = BrDegreeToAngle(d);
        BrMatrix34Identity(&C2V(gCamera)->t.t.mat);
        BrVector3Set(&C2V(gCamera)->t.t.translate.t,
            C2V(gProgram_state).current_car.driver_x_offset,
            C2V(gProgram_state).current_car.driver_y_offset,
            C2V(gProgram_state).current_car.driver_z_offset);
        w = (float)(C2V(gRender_screen)->base_y
                    + (C2V(gRender_screen)->height / 2)
                    - (C2V(gProgram_state).current_car.render_bottom[0] + C2V(gProgram_state).current_car.render_top[0]) / 2);

        C2V(gCamera)->t.t.mat.m[2][1] = tandeg(d / 2.0f) * w * 2.0f / (float)C2V(gRender_screen)->height;
        camera_ptr->aspect = (float)C2V(gWidth) / C2V(gHeight);
        camera_ptr->yon_z = C2V(gYon_multiplier) * C2V(gCamera_yon);
        if (C2V(gProgram_state).which_view == eView_left) {
            DRMatrix34PostRotateY(
                    &C2V(gCamera)->t.t.mat,
                    BrDegreeToAngle(C2V(gProgram_state).current_car.head_left_angle));
        } else if (C2V(gProgram_state).which_view == eView_right) {
            DRMatrix34PostRotateY(
                    &C2V(gCamera)->t.t.mat,
                    BrDegreeToAngle(C2V(gProgram_state).current_car.head_right_angle));
        }
        BrVector3Set(&C2V(gCamera)->t.t.translate.t,
                     C2V(gProgram_state).current_car.driver_x_offset,
                     C2V(gProgram_state).current_car.driver_y_offset,
                     C2V(gProgram_state).current_car.driver_z_offset);
        SetSightDistance(camera_ptr->yon_z);
        MungeForwardSky();
    }
    AssertYons();
}
C2_HOOK_FUNCTION(0x0047d8d0, ReinitialiseForwardCamera)

void C2_HOOK_FASTCALL AllocateRearviewPixelmap(void) {

    if (C2V(gRearview_screen) != NULL) {
        BrPixelmapFree(C2V(gRearview_screen));
        C2V(gRearview_screen) = NULL;
    }
    if (C2V(gProgram_state).mirror_on) {
        C2V(gRearview_screen) = BrPixelmapAllocateSub(
                C2V(gBack_screen),
                C2V(gProgram_state).current_car.mirror_left,
                C2V(gProgram_state).current_car.mirror_top,
                C2V(gProgram_state).current_car.mirror_right - C2V(gProgram_state).current_car.mirror_left,
                C2V(gProgram_state).current_car.mirror_bottom - C2V(gProgram_state).current_car.mirror_top);
        C2V(gRearview_depth_buffer) = C2V(gDepth_buffer);
        C2V(gRearview_screen)->origin_x = C2V(gRearview_screen)->width / 2;
        C2V(gRearview_screen)->origin_y = C2V(gRearview_screen)->height / 2;
    }
}
C2_HOOK_FUNCTION(0x0047dab0, AllocateRearviewPixelmap)

void C2_HOOK_FASTCALL ReinitialiseRearviewCamera(void) {
    br_camera* camera_ptr;

    camera_ptr = C2V(gRearview_camera)->type_data;
    camera_ptr->field_of_view = BrDegreeToAngle(C2V(gProgram_state).current_car.rearview_camera_angle);
    camera_ptr->aspect = (C2V(gProgram_state).current_car.mirror_right - C2V(gProgram_state).current_car.mirror_left) / (float)(C2V(gProgram_state).current_car.mirror_bottom - C2V(gProgram_state).current_car.mirror_top);
    BrVector3Set(&C2V(gRearview_camera)->t.t.translate.t,
        C2V(gProgram_state).current_car.mirror_x_offset,
        C2V(gProgram_state).current_car.mirror_y_offset,
        C2V(gProgram_state).current_car.mirror_z_offset);
    AllocateRearviewPixelmap();
    MungeRearviewSky();
}
C2_HOOK_FUNCTION(0x0047db30, ReinitialiseRearviewCamera)

void (C2_HOOK_FASTCALL * InitRace_original)(void);
void C2_HOOK_FASTCALL InitRace(void) {

#if defined(C2_HOOKS_ENABLED)
    InitRace_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00481830, InitRace, InitRace_original)

void C2_HOOK_FASTCALL DisposeRace(void) {

    DisposeMutantTail();
    PossibleService();
    DisposeSmashingAndExplosions();
    DisposeReplay();
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
    // FUN_00486d70();
    DisposeOpponentsAndDrones();
    PossibleService();
    DisposePedestrians();
    PossibleService();
    DisposePratcam();
    PossibleService();
}
C2_HOOK_FUNCTION(0x0044c070, DisposeRace)

void C2_HOOK_FASTCALL DisposeTrack(void) {

    FreeTrack(&C2V(gProgram_state).track_spec);
}
C2_HOOK_FUNCTION(0x0044c220, DisposeTrack)
