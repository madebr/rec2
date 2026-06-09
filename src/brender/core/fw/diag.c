#include "diag.h"

#include "brprintf.h"
#include "fwsetup.h"

#include "core/std/brstdlib.h"

#include <stdarg.h>
#include <stdlib.h>

// Global variables

// GLOBAL: CARMA2_HW 0x006ad928
char _diag_scratch[128];

// FUNCTION: CARMA2_HW 0x00527c80
void C2_HOOK_CDECL BrFailure(const char* s, ...) {
    va_list args;

    BrStrCpy(_diag_scratch, "Failure: ");

    va_start(args, s);
    BrVSprintf(&_diag_scratch[sizeof("Failure: ") - 1], s, args);
    va_end(args);

    if (fw.diag->failure == NULL) {
        BrAbort();
    }

    fw.diag->failure(_diag_scratch);
#ifdef BRENDER_FIX_BUGS
    abort();
#endif
}

// FUNCTION: CARMA2_HW 0x00527cd0
void C2_HOOK_CDECL BrWarning(const char* s, ...) {
    va_list args;

    va_start(args, s);
    BrStrCpy(_diag_scratch, "Warning: ");
    BrVSprintf(&_diag_scratch[sizeof("Warning: ") - 1], s, args);

    if (fw.diag->warning == NULL) {
        BrAbort();
    }

    fw.diag->warning(_diag_scratch);
    va_end(args);
}

// FUNCTION: CARMA2_HW 0x00527d20
void C2_HOOK_CDECL BrFatal(const char* name, int line, const char* s, ...) {
    va_list args;
    int n;

    va_start(args, s);
    n = BrSprintf(_diag_scratch, "FATAL %s:%d\n", name, line);
    BrVSprintf(&_diag_scratch[n], s, args);
    if (fw.diag->failure == NULL) {
        BrAbort();
    }

    fw.diag->failure(_diag_scratch);
    va_end(args);
}

// FUNCTION: CARMA2_HW 0x00527d80
void C2_HOOK_CDECL _BrAssert(const char* condition, const char* file, unsigned int line) {
    if (fw.diag->failure == NULL) {
        BrAbort();
    }

    BrSprintf(_diag_scratch, "ASSERTION FAILED %s:%d: \"%s\"\n", file, line, condition);
    fw.diag->failure(_diag_scratch);
}

// FUNCTION: CARMA2_HW 0x00527dd0
void C2_HOOK_CDECL _BrUAssert(const char* condition, const char* file, unsigned int line) {
    if (fw.diag->failure == NULL) {
        BrAbort();
    }

    BrSprintf(_diag_scratch, "ASSERTION FAILED %s:%d: \"%s\"\n", file, line, condition);
    fw.diag->failure(_diag_scratch);
}
