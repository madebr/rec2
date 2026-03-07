#include "brstddiag.h"

#include "core/fw/brbegin.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"


// GLOBAL: CARMA2_HW 0x0066fe48
br_diaghandler BrStdioDiagHandler = {
    "Stdio DiagHandler",
    BrStdioWarning,
    BrStdioFailure,
};


// GLOBAL: CARMA2_HW 0x0066fe54
br_diaghandler* _BrDefaultDiagHandler = &BrStdioDiagHandler;

// FUNCTION: CARMA2_HW 0x0053fa00
void C2_HOOK_CDECL BrStdioWarning(char* message) {
    fflush(stdout);
    fputs(message, stderr);
    fputc('\n', stderr);
    fflush(stderr);
}

// FUNCTION: CARMA2_HW 0x0053fa40
void C2_HOOK_CDECL BrStdioFailure(char* message) {

#if 0
    // FIXME: 'real' implementation ends BRender
    BrEnd();
#endif
    fflush(stdout);
    fputs(message, stderr);
    fputc('\n', stderr);
    fflush(stderr);
#if 0
    // FIXME: 'real' implementation unconditionally exits.
    c2_exit(10);
#else
    c2_abort();
#endif
}