#ifndef C2_PLATFORM_H
#define C2_PLATFORM_H

#include "c2_hooks.h"

#include "rec2_types.h"

void C2_HOOK_FASTCALL KeyBegin(void);

void C2_HOOK_FASTCALL PDBuildAppPath(char* pThe_path);
void C2_HOOK_FASTCALL PDFatalError(char* pThe_str);
void C2_HOOK_FASTCALL PDSetFileVariables(void);
C2_NORETURN void C2_HOOK_FASTCALL PDShutdownSystem(void);
void C2_HOOK_FASTCALL PDUnlockRealBackScreen(void);

int C2_HOOK_FASTCALL PDCheckDriveExists2(const char* pThe_path, const char* pFile_name, tU32 pMin_size);

#endif // C2_PLATFORM_H
