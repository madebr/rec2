#include "main.h"

#include "utility.h"

#include "rec2_types.h"

#include "controls.h"
#include "drmem.h"
#include "errors.h"
#include "globvars.h"
#include "init.h"
#include "input.h"
#include "network.h"
#include "sound.h"
#include "structur.h"
#include "utility.h"

#include "platform.h"

#include "c2_string.h"

C2_NORETURN_FUNCPTR void (C2_HOOK_FASTCALL * QuitGame_original)(void);
C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    QuitGame_original();
    C2_HOOK_FINISH();
#else
    SaveOptions();
    if (C2V(gSave_game_out_of_sync)) {
        SaveGame();
    }
    ActionReplayFinishRecording();

#error "Not implemented"
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
