#include "52-errors.h"

#include "platform.h"
#include "rec2_types.h"

#include <stdarg.h>
#include <stdio.h>

FILE* gDiagnostic_file;

// STUB: CARMA2_HW 0x0044c230
void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0044c570
void C2_HOOK_FASTCALL CloseDiagnostics(void) {

}

// FUNCTION: CARMA2_HW 0x0044c580
void C2_HOOK_FASTCALL OpenDiagnostics(void) {
#ifndef REC2_MATCHING
    // FIXME: use c2_stdio functions
    //    gDiagnostic_file = fopen("DIAGNOST.TXT", "w");
    gDiagnostic_file = fopen("DIAGNOST.TXT", "w");
    if (gDiagnostic_file == NULL) {
        return;
    }
    fputs("DIAGNOSTIC OUTPUT\n", gDiagnostic_file);
    // todo: generate a real date
    fprintf(gDiagnostic_file, "Date / time : %s\n\n\n", __DATE__ " : " __TIME__);
#endif
}

// Renamed from dprintf to avoid collisions to stdio
// This function is stripped from the retail binary, we've guessed at the implementation
// FUNCTION: CARMA2_HW 0x0044c590
void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...) {
    va_list args;
    tU32 the_time;

#ifndef REC2_MATCHING
    if (gDiagnostic_file == NULL) {
        OpenDiagnostics();
    }
    the_time = PDGetTotalTime();
    fprintf(gDiagnostic_file, "%7d.%03d: ", the_time / 1000, the_time % 1000);

    va_start(args, fmt_string);
    vfprintf(gDiagnostic_file, fmt_string, args);
    va_end(args);

    fputs("\n", gDiagnostic_file);

    fflush(gDiagnostic_file);
#endif
}


