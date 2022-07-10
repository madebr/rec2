#ifndef REC2_STDDIAG_H
#define REC2_STDDIAG_H

#include "c2_hooks.h"

#include "brender/br_types.h"

C2_HOOK_VARIABLE_DECLARE(br_diaghandler, BrStdioDiagHandler);
C2_HOOK_VARIABLE_DECLARE(br_diaghandler*, _BrDefaultDiagHandler);

void C2_HOOK_CDECL BrStdioWarning(char* message);

void C2_HOOK_CDECL BrStdioFailure(char* message);

#endif // REC2_STDDIAG_H
