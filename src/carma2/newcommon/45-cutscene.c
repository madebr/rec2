#include "45-cutscene.h"

#include "18-graphics2.h"

// FUNCTION: CARMA2_HW 0x0043e750
void C2_HOOK_FASTCALL DoSCILogo(void) {
}

// FUNCTION: CARMA2_HW 0x0043e760
void C2_HOOK_STDCALL DoStainlessLogo(void) {
}

// PlaySmackerFile

// DoOpeningAnimation

// FUNCTION: CARMA2_HW 0x0043f370
void C2_HOOK_FASTCALL DoEndRaceAnimation(void) {
    FadePaletteDown();
}

// DoGameCompletedAnimation

// StartLoadingScreen

