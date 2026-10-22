#include "45-cutscene.h"

#include "18-graphics2.h"
#include "42-input.h"

// FUNCTION: CARMA2_HW 0x0043e750
void C2_HOOK_FASTCALL DoSCILogo(void) {
}

// FUNCTION: CARMA2_HW 0x0043e760
void C2_HOOK_STDCALL DoStainlessLogo(void) {
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0043e770
void C2_HOOK_FASTCALL PlaySmackerFile(char* pSmack_name) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DoOpeningAnimation

// FUNCTION: CARMA2_HW 0x0043f370
void C2_HOOK_FASTCALL DoEndRaceAnimation(void) {
    FadePaletteDown();
}

// FUNCTION: CARMA2_HW 0x0043f390
void C2_HOOK_FASTCALL DoGameCompletedAnimation(void) {

    PlaySmackerFile("TOPRANK.SMK");
    WaitForNoKeys();
}

// StartLoadingScreen

