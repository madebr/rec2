#include "brstddiag.h"

#include "core/fw/brbegin.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_diaghandler, BrStdioDiagHandler, 0x0066fe48, {
    "Stdio DiagHandler",
    BrStdioWarning,
    BrStdioFailure,
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_diaghandler*, _BrDefaultDiagHandler, 0x0066fe54, &C2V(BrStdioDiagHandler));

void C2_HOOK_CDECL BrStdioWarning(char* message) {
    fflush(stdout);
    fputs(message, stderr);
    fputc('\n', stderr);
    fflush(stderr);
}
C2_HOOK_FUNCTION(0x0053fa00, BrStdioWarning)

void BrStdioFailure(char* message) {

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
C2_HOOK_FUNCTION(0x0053fa40, BrStdioFailure)
