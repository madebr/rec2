#include "main.h"

#include "rec2_types.h"

#include "controls.h"
#include "drmem.h"
#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "loadsave.h"
#include "network.h"
#include "physics.h"
#include "replay.h"
#include "sound.h"
#include "structur.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include <s3/s3.h>

#include "c2_stdlib.h"
#include "c2_string.h"

C2_NORETURN_FUNCPTR void (C2_HOOK_FASTCALL * QuitGame_original)(void);
C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    QuitGame_original();
    C2_HOOK_FINISH();
#else
    SaveOptions();
    if (C2V(gSave_game_out_of_sync)) {
        WriteSaveGame();
    }
    ActionReplayFinishRecording();
    DisableRecursiveCollisions();
    dr_dprintf("QuitGame() - Point 1");
    if (C2V(gProgram_state).racing && C2V(gAdditional_actors) != NULL && C2V(gAdditional_actors)->children != NULL) {
        dr_dprintf("QuitGame() - Point 2");
        SaveAdditionalStuff();
        dr_dprintf("QuitGame() - Point 3");
    }
    C2V(gProgram_state).racing = 0;
    SaveOptions();
    if (C2V(gNet_mode) != eNet_mode_none) {
        NetLeaveGame(C2V(gCurrent_net_game));
    }
    ShutdownNetIfRequired();
    if (C2V(gSound_available)) {
        DRS3DisableSound();
    }
    if (C2V(gBr_initialized)) {
        ClearEntireScreen();
    }
    PDRevertPalette();
    StopMusic();
    if (C2V(gBr_initialized)) {
        RemoveAllBrenderDevices();
    }
    PDShutdownSystem();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491f70, QuitGame, QuitGame_original)

C2_NORETURN_FUNCPTR void (C2_HOOK_FASTCALL * GameMain_original)(int pArgc, const char** pArgv);
void C2_HOOK_FASTCALL GameMain(int pArgc, const char** pArgv) {
    C2_HOOK_START();
#if 0//defined(C2_HOOKS_ENABLED)
    GameMain_original(pArgc, pArgv);
#else
    tPath_name location;

    PDSetFileVariables();
    PDBuildAppPath(C2V(gApplication_path));
    OpenDiagnostics();

    c2_strcat(C2V(gApplication_path), "DATA");
    UsePathFileToDetermineIfFullInstallation();
    if (!C2V(gCD_fully_installed)) {
        if (PDReadSourceLocation(location)) {
            if (!PDCheckDriveExists(location)) {
                PDInitialiseSystem();
                PDFatalError("Can't find the Carmageddon CD\n");
            }
        }
    }
    InitialiseApplication(pArgc, pArgv);
    DoProgram();
    QuitGame();
#endif
    C2_HOOK_FINISH();
}
C2_HOOK_FUNCTION_ORIGINAL(0x004924a0, GameMain, GameMain_original)

void (C2_HOOK_FASTCALL * ServiceGame_original)(void);
void C2_HOOK_FASTCALL ServiceGame(void) {

#if defined(C2_HOOKS_ENABLED)
    ServiceGame_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00492180, ServiceGame, ServiceGame_original)

void C2_HOOK_FASTCALL ServiceTheGame(int pRacing) {

    CheckMemory();
    if (!pRacing) {
        CyclePollKeys();
    }
    PollKeys();
    c2_rand();
    if (PDServiceSystem(C2V(gFrame_period))) {
        QuitGame();
    }
    if (!pRacing) {
        CheckSystemKeys(0);
    }
    if (!pRacing && C2V(gSound_enabled)) {
        SoundService();
    }
    NetService(pRacing);
}
C2_HOOK_FUNCTION(0x00492050, ServiceTheGame)

void C2_HOOK_FASTCALL ServiceGameInRace(void) {

    ServiceTheGame(1);
    CheckKevKeys();
}
