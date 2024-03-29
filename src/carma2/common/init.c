#include "init.h"

#include "c2_stdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gBr_initialized, 0x0068be2c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRender_indent, 0x0068be38);

void (C2_HOOK_FASTCALL * InitialiseApplication_original)(int pArgc, const char** pArgv);
void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char** pArgv) {
#if defined(C2_HOOKS_ENABLED)
    InitialiseApplication_original(pArgc, pArgv);
#else
    KeyBegin();

    C2V(gProgram_state.sausage_eater_mode) = C2V(gSausage_override);
    VideoInit();
    memset(gFontTextureMaps, 0, sizeof(gFontTextureMaps));
    TWT_Init();
    MAMSInitMem();
    C2V(gAusterity_mode) = C2V(gAustere_mode) || PDDoWeLeadAnAustereExistance();
    c2_srand(time(NULL));
    BrV1dbBeginWrapper_Float();
    CreateStainlessClasses();
    InstallDRMemCalls();
    InstallDRFileCalls();

    tTWTVFS twtVfs = TWT_MountEx(C2V(gApplication_path));
    C2V(gApplicationDataTwtMounted) = 1;

    TemporaryMaterialStorageInit();
    PDTimerInit();
    InitWobbleStuff();
    LoadGeneralParameters();
    PrintMemoryDump(0, "AFTER LOADING GENERAL PARAMETERS");
    DefaultNetName();
    strcpy(C2V(gProgram_state_player_name), "MAX DAMAGE");
    RestoreOptions();
    LoadKeyMapping();
    if (!PDInitScreenVars(pArgc, pArgv)) {
        FatalError(kFatalError_InvalidScreenDepthSetting);
    }
    CalcGrafDataIndex();
    // empty_func();
    // empty_func();
    InitializeBRenderEnvironment();
    PrintMemoryDump(0, "AFTER INITIALISING BRENDER");
    CheckIsDemo();

    InitDRFonts();
    InitBRFonts();
    PrintMemoryDump(0, "AFTER LOADING FONTS");
    InitExplosions();
    PrintMemoryDump(0, "AFTER INITIALISING EXPLOSIONS");
    LoadMiscStrings();
    PrintMemoryDump(0, "AFTER LOADING MISC STRING");

    abort(); //Unfinished


#error "not implemented"
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
