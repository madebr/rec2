#include "diag.h"

#include "core/fw/fwsetup.h"

#include "core/std/brstdlib.h"

#include <stdarg.h>

#if defined(C2_HOOKS_ENABLED)
C2_HOOK_VARIABLE_IMPLEMENT(char*, _diag_scratch, 0x006ad928);
#else
// Global variables
char _diag_scratch[128];
#endif

void C2_HOOK_CDECL BrFailure(const char* s, ...) {
    va_list args;
    const char failure_header[10] = "Failure: ";
    C2_HOOK_START();

    BrStrCpy(C2V(_diag_scratch), failure_header);

    va_start(args, s);
    BrVSprintf(&C2V(_diag_scratch)[sizeof(failure_header) - 1], s, args);
    va_end(args);

    if (C2V(fw).diag->failure == NULL) {
        BrAbort();
    }

    C2V(fw).diag->failure(C2V(_diag_scratch));
    C2_HOOK_FINISH();
}
C2_HOOK_FUNCTION(0x00527c80, BrFailure)
