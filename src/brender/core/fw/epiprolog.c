#include "epiprolog.h"
/*
 * Entry/exit tracking hooks for compiler
 */

void C2_HOOK_CDECL _EPI(void) {
}
C2_HOOK_FUNCTION(0x0052d050, _EPI)

void C2_HOOK_CDECL _PRO(void) {
    _EPI();
}
