#include "main.h"

#include "rec2_types.h"

#include <stdlib.h>

//#include "controls.h"
//#include "cutscene.h"
//#include "drmem.h"
//#include "errors.h"
//#include "globvars.h"
//#include "harness/config.h"
//#include "harness/trace.h"
//#include "init.h"
//#include "input.h"
//#include "loading.h"
//#include "loadsave.h"
//#include "network.h"
//#include "pd/sys.h"
//#include "s3/s3.h"
//#include "sound.h"
//#include "structur.h"
//#include "utility.h"
//
void (C2_HOOK_FASTCALL * QuitGame_original)(void);
void C2_HOOK_FASTCALL QuitGame() {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    QuitGame_original();
    C2_HOOK_FINISH();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491f70, QuitGame, QuitGame_original)

//
//// IDA: tU32 __cdecl TrackCount(br_actor *pActor, tU32 *pCount)
//tU32 TrackCount(br_actor* pActor, tU32* pCount) {
//    unsigned int x;
//    unsigned int z;
//    int ad;
//    float e;
//    LOG_TRACE("(%p, %p)", pActor, pCount);
//    NOT_IMPLEMENTED();
//}
//
//// IDA: void __cdecl CheckNumberOfTracks()
//void CheckNumberOfTracks() {
//    tU32 track_count;
//    LOG_TRACE("()");
//    NOT_IMPLEMENTED();
//}
//
//// IDA: void __usercall ServiceTheGame(int pRacing@<EAX>)
//void ServiceTheGame(int pRacing) {
//
//    CheckMemory();
//    if (!pRacing) {
//        CyclePollKeys();
//    }
//    PollKeys();
//    rand();
//    if (PDServiceSystem(gFrame_period)) {
//        QuitGame();
//    }
//    if (!pRacing) {
//        CheckSystemKeys(0);
//    }
//    if (!pRacing && gSound_enabled) {
//        if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0) {
//            S3Service(1, 2);
//        } else {
//            S3Service(0, 2);
//        }
//    }
//    NetService(pRacing);
//}
//
//// IDA: void __cdecl ServiceGame()
//void ServiceGame() {
//    ServiceTheGame(0);
//}
//
//// IDA: void __cdecl ServiceGameInRace()
//void ServiceGameInRace() {
//    LOG_TRACE("()");
//
//    ServiceTheGame(1);
//    CheckKevKeys();
//}
//

void (C2_HOOK_FASTCALL * GameMain_original)(int pArgc, char** pArgv);
void C2_HOOK_FASTCALL GameMain(int pArgc, char** pArgv) {
    C2_HOOK_START();
#if defined(C2_HOOKS_ENABLED)
    GameMain_original(pArgc, pArgv);
#else
    tPath_name location;

    PDSetFileVariables();
    PDBuildAppPath(C2V(gApplicationPath);
    OpenDiagnostics();

    strcat(C2V(gApplicationPath), "DATA");
    UsePathFileToDetermineIfFullInstallation();
    if (!C2V(gCD_fully_installed)) {
        if (PDReadSourceLocation(location)) {
            if (!PDCheckDriveExists(location)) {

            }
        }
    }

//    tPath_name CD_dir;
//
//    PDSetFileVariables();
//#if defined(DETHRACE_VFS)
//    gApplication_path[0] = '\0';
//#else
//    PDBuildAppPath(gApplication_path);
//#endif
//    OpenDiagnostics();
//
//    strcat(gApplication_path, "DATA");
//
//    UsePathFileToDetermineIfFullInstallation();
//    if (!gCD_fully_installed && GetCDPathFromPathsTxtFile(CD_dir) && !PDCheckDriveExists(CD_dir)) {
//        PDInitialiseSystem();
//        fprintf(stderr, "Can't find the Carmageddon CD\n");
//        exit(1);
//    }
//    InitialiseDeathRace(pArgc, pArgv);
//    DoProgram();
//    QuitGame();
//}
#endif
    C2_HOOK_FINISH();
}
C2_HOOK_FUNCTION_ORIGINAL(0x004924a0, GameMain, GameMain_original)
