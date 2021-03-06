#ifndef REC2_UTILITY_H
#define REC2_UTILITY_H

#include "brender/br_types.h"
#include "rec2_types.h"

#include "c2_hooks.h"

#include <stdint.h>

br_error C2_HOOK_FASTCALL RemoveAllBrenderDevices(void);

int C2_HOOK_FASTCALL PDCheckDriveExists(char* pThe_path);

char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(tTWTFILE* pF, char* pS);

char* C2_HOOK_FASTCALL GetALineAndDontArgue(tTWTFILE* pF, char* pS);

void C2_HOOK_FASTCALL PathCat(char* pDestn_str, char* pStr_1, const char* pStr_2);

int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);

void C2_HOOK_FASTCALL PossibleService(void);

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags);

#endif // REC2_UTILITY_H
