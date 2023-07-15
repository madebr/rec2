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
#include "grafdata.h"
#include "graphics.h"
#include "loading.h"
#include "netgame.h"
#include "oil.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "powerups.h"
#include "platform.h"
#include "raycast.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "temp.h"
#include "tinted.h"
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

#if defined(C2_HOOKS_ENABLED)
    AllocateSelf_original();
#else
#error "Not implemented"
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

void (C2_HOOK_FASTCALL * Init2DStuff_original)(void);
void C2_HOOK_FASTCALL Init2DStuff(void) {

#if defined(C2_HOOKS_ENABLED)
    Init2DStuff_original();
#else
#error "Not implemented"
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

#if defined(C2_HOOKS_ENABLED)
    InitGame_original(pStart_race);
#else
#error "Not implemented"
#endif

}
C2_HOOK_FUNCTION_ORIGINAL(0x004816b0, InitGame, InitGame_original)
