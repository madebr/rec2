#ifndef _ERRORS_H_
#define _ERRORS_H_

#include "brender/br_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(char*, gError_messages);
C2_HOOK_VARIABLE_DECLARE(int, gError_code);

#if defined(C2_HOOKS_ENABLED)
#else

extern char* gError_messages[126];
//extern char* gPalette_copy__errors; // suffix added to avoid duplicate symbol
//extern int gPixel_buffer_size__errors; // suffix added to avoid duplicate symbol
//extern int gMouse_was_started__errors; // suffix added to avoid duplicate symbol
//extern char* gPixels_copy__errors; // suffix added to avoid duplicate symbol

#endif
//
void C2_HOOK_CDECL FatalError(int pStr_index, ...);
//
//void NonFatalError(int pStr_index, ...);
//
//void CloseDiagnostics();
//
//void OpenDiagnostics();

void C2_HOOK_CDECL dr_dprintf(char* fmt_string, ...);

//int DoErrorInterface(int pMisc_text_index);

#endif
