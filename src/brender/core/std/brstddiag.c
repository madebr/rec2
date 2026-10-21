#include "brstddiag.h"

#include "core/fw/brbegin.h"

#include <stdio.h>
#include "c2_stdlib.h"


// GLOBAL: CARMA2_HW 0x0066fe48
br_diaghandler BrStdioDiagHandler = {

    "Stdio DiagHandler",
    BrStdioWarning,
    BrStdioFailure,
};


// GLOBAL: CARMA2_HW 0x0066fe54
// GLOBAL: CARMA2_SW 0x0066a5e4
br_diaghandler* _BrDefaultDiagHandler = &BrStdioDiagHandler;

// FUNCTION: CARMA2_HW 0x0053fa00
// FUNCTION: CARMA2_SW 0x0053b0a0
void C2_HOOK_CDECL BrStdioWarning(char* message) {

    fflush(stdout);
    fputs(message, stderr);
    fputc('\n', stderr);
    fflush(stderr);
}

// FUNCTION: CARMA2_HW 0x0053fa40
// FUNCTION: CARMA2_SW 0x0053b0e0
void C2_HOOK_CDECL BrStdioFailure(char* message) {

    fflush(stdout);
    fputs(message, stderr);
    fputc('\n', stderr);
    fflush(stderr);
#ifdef REC2_MATCHING
    exit(10);
#else
    abort();
#endif
}
