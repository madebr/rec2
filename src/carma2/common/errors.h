#ifndef REC2_ERRORS_H
#define REC2_ERRORS_H

#include "brender/br_types.h"

#include "c2_hooks.h"

extern const char* gError_messages[186];
extern int gError_code;

void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);
//
//void NonFatalError(int pStr_index, ...);
//
void C2_HOOK_FASTCALL CloseDiagnostics();

void C2_HOOK_FASTCALL OpenDiagnostics(void);

void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);

//int DoErrorInterface(int pMisc_text_index);

#endif // REC2_ERRORS_H
