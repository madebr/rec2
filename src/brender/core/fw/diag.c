#include "diag.h"

#include "brprintf.h"
#include "fwsetup.h"

#include "core/std/brstdlib.h"

#include <stdarg.h>

// Global variables
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, _diag_scratch, 128, 0x006ad928);

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

void C2_HOOK_CDECL BrWarning(const char* s, ...) {
    va_list args;
    const char warning_header[10] = "Warning: ";

    va_start(args, s);
    BrStrCpy(C2V(_diag_scratch), warning_header);
    BrVSprintf(&C2V(_diag_scratch)[sizeof(warning_header) - 1], s, args);

    if (C2V(fw).diag->warning == NULL) {
        BrAbort();
    }

    C2V(fw).diag->warning(C2V(_diag_scratch));
    va_end(args);
}
C2_HOOK_FUNCTION(0x00527cd0, BrWarning)

void C2_HOOK_CDECL BrFatal(const char* name, int line, const char* s, ...) {
    va_list args;
    int n;

    va_start(args, s);
    n = BrSprintf(C2V(_diag_scratch), "FATAL %s:%d\n", name, line);
    BrVSprintf(&C2V(_diag_scratch)[n], s, args);
    if (C2V(fw).diag->failure == NULL) {
        BrAbort();
    }

    C2V(fw).diag->failure(C2V(_diag_scratch));
    va_end(args);
}
C2_HOOK_FUNCTION(0x00527d20, BrFatal)

void C2_HOOK_CDECL _BrAssert(const char* condition, const char* file, unsigned int line) {
    if (C2V(fw).diag->failure == NULL) {
        BrAbort();
    }

    BrSprintf(C2V(_diag_scratch), "ASSERTION FAILED %s:%d: \"%s\"\n", file, line, condition);
    C2V(fw).diag->failure(C2V(_diag_scratch));
}
C2_HOOK_FUNCTION(0x00527d80, _BrAssert)

void C2_HOOK_CDECL _BrUAssert(const char* condition, const char* file, unsigned int line) {
    if (C2V(fw).diag->failure == NULL) {
        BrAbort();
    }

    BrSprintf(C2V(_diag_scratch), "ASSERTION FAILED %s:%d: \"%s\"\n", file, line, condition);
    C2V(fw).diag->failure(C2V(_diag_scratch));
}
C2_HOOK_FUNCTION(0x00527dd0, _BrUAssert)
