#include "main.h"

#include "utility.h"

#include "rec2_types.h"

#include "controls.h"
#include "drmem.h"
#include "52-errors.h"
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
#include "timers.h"
#include "utility.h"
#include "video.h"
#include "world.h"

#include "platform.h"

#include <s3/s3.h>

#include "c2_stdlib.h"
#include "c2_string.h"

// FUNCTION: CARMA2_HW 0x00491f70
C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void) {

    SaveOptions();
    if (gSave_game_out_of_sync) {
        DoSaveGame();
    }
    MovieStopRecordingIfNecessary();
    PHILDisable();
    dr_dprintf("QuitGame() - Point 1");
    if (gProgram_state.racing && gAdditional_actors != NULL && gAdditional_actors->children != NULL) {
        dr_dprintf("QuitGame() - Point 2");
        AutoSaveAdditionalStuff();
        dr_dprintf("QuitGame() - Point 3");
    }
    gProgram_state.racing = 0;
    SaveOptions();
    if (gNet_mode != eNet_mode_none) {
        NetLeaveGame(gCurrent_net_game);
    }
    ShutdownNetIfRequired();
    if (gSound_available) {
        DRS3ShutDown();
    }
    if (gBr_initialized) {
        ClearEntireScreen();
    }
    PDRevertPalette();
    StopMusic();
    if (gBr_initialized) {
        DRBrEnd();
    }
    PDShutdownSystem();
    CloseDiagnostics();
    exit(0);
}

// FUNCTION: CARMA2_HW 0x004924a0
void C2_HOOK_FASTCALL GameMain(int pArgc, const char** pArgv) {
    tPath_name location;

    PDSetFileVariables();
    PDBuildAppPath(gApplication_path);
    OpenDiagnostics();

    strcat(gApplication_path, "DATA");
    UsePathFileToDetermineIfFullInstallation();
    if (!gCD_fully_installed) {
        if (PDReadSourceLocation(location)) {
            if (!PDCheckDriveExists(location)) {
                PDInitialiseSystem();
                PDFatalError("Can't find the Carmageddon CD\n");
            }
        }
    }
    InitialiseDeathRace(pArgc, pArgv);
    DoProgram();
    QuitGame();
}

// FUNCTION: CARMA2_HW 0x00492180
void C2_HOOK_FASTCALL ServiceGame(void) {

    ServiceTheGame(0);
}

// FUNCTION: CARMA2_HW 0x00492050
void C2_HOOK_FASTCALL ServiceTheGame(int pRacing) {

    CheckMemory();
    if (!pRacing) {
        CyclePollKeys();
    }
    PollKeys();
    rand();
    if (PDServiceSystem(gFrame_period)) {
        QuitGame();
    }
    if (!pRacing) {
        CheckSystemKeys(0);
    }
    if (!pRacing && gSound_enabled) {
        SoundService();
    }
    NetService(pRacing);
}

// FUNCTION: CARMA2_HW 0x004922a0
void C2_HOOK_FASTCALL ServiceGameInRace(void) {

    Timers_Push(TIMER_SER);
    ServiceTheGame(1);
    CheckKevKeys();
    Timers_Pop(TIMER_SER);
}
