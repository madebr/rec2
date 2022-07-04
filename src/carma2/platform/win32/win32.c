#include "win32.h"

#include "errors.h"
#include "init.h"
#include "utility.h"

#include "c2_stdio.h"

#include <windows.h>

C2_HOOK_VARIABLE_IMPLEMENT(char*, gFatalErrorMessage, 0x006acc88)
C2_HOOK_VARIABLE_IMPLEMENT(int, gIsFatalError, 0x006ad498)
C2_HOOK_VARIABLE_IMPLEMENT(int, gExitCode, 0x006ad494)
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gReal_back_screen, 0x0074d360)


void PDFatalError(char* pThe_str) {
    char* str1;
    char* str2;
    dr_dprintf("FATAL ERROR: %s", pThe_str);

    str1 = pThe_str;
    if (str1 == NULL) {
        str1 = "NULL str1";
    }
    str2 = "";
    if (str2 == NULL) {
        str2 = "NULL str2";
    }
    C2V(gIsFatalError) = 1;
    c2_sprintf(C2V(gFatalErrorMessage), "%s %s", str1, str2);

    C2V(gExitCode) = 700;

    if (C2V(gReal_back_screen) != NULL && C2V(gReal_back_screen)->pixels != NULL) {
        PDUnlockRealBackScreen();
    }
    if (C2V(gBr_initialized)) {
        RemoveAllBrenderDevices();
    }
    PDShutdownSystem();
}
C2_HOOK_FUNCTION(0x0051af20, PDFatalError)


static void(* C2_HOOK_FASTCALL PDShutdownSystem_original)(void);
void PDShutdownSystem() {
    PDShutdownSystem_original();
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051c110, PDShutdownSystem, PDShutdownSystem_original)


static void(* C2_HOOK_FASTCALL PDUnlockRealBackScreen_original)(void);
void PDUnlockRealBackScreen() {
    PDUnlockRealBackScreen_original();
}
C2_HOOK_FUNCTION_ORIGINAL(0x00516c30, PDUnlockRealBackScreen, PDUnlockRealBackScreen_original)
