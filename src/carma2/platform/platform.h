#ifndef C2_PLATFORM_H
#define C2_PLATFORM_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gGraf_spec_index);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tGraf_spec, gGraf_specs, 2);

void C2_HOOK_FASTCALL KeyBegin(void);

void C2_HOOK_FASTCALL PDBuildAppPath(char* pThe_path);
void C2_HOOK_FASTCALL PDFatalError(char* pThe_str);
void C2_HOOK_FASTCALL PDSetFileVariables(void);
void C2_HOOK_FASTCALL PDShutdownSystem(void);
void C2_HOOK_FASTCALL PDUnlockRealBackScreen(void);

int C2_HOOK_FASTCALL PDCheckDriveExists2(const char* pThe_path, const char* pFile_name, tU32 pMin_size);

int C2_HOOK_FASTCALL PDReadSourceLocation(tPath_name pPath);

int C2_HOOK_FASTCALL PDDoWeLeadAnAustereExistance(void);

#endif // C2_PLATFORM_H
