#ifndef REC2_UTILITY_H
#define REC2_UTILITY_H

#include "brender/br_types.h"
#include "rec2_types.h"

#include "c2_hooks.h"

#include <stdint.h>

br_error C2_HOOK_FASTCALL RemoveAllBrenderDevices(void);

void C2_HOOK_FASTCALL StringToUpper(char* dest, const char* src);

int C2_HOOK_FASTCALL PDCheckDriveExists(char* pThe_path);

char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(tTWTFILE* pF, char* pS);

char* C2_HOOK_FASTCALL GetALineAndDontArgue(tTWTFILE* pF, char* pS);

void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);

int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);

tU32 C2_HOOK_FASTCALL GetTotalTime(void);

void C2_HOOK_FASTCALL PossibleService(void);

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags);

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocateSub(br_pixelmap* pPm, br_uint_16 pX, br_uint_16 pY, br_uint_16 pW, br_uint_16 pH);

br_pixelmap* C2_HOOK_FASTCALL DRLoadUpdatePixelmapFromTif(const char* path);

void C2_HOOK_FASTCALL DRPixelmapRectangleCopy(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_uint_16 w, br_uint_16 h);

intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);

void C2_HOOK_FASTCALL ExtractPath_Dirname_Stem(const char* path, char* dirPath, char* stemPath);

tU32 C2_HOOK_FASTCALL GetFileLength(tTWTFILE* pF);

#endif // REC2_UTILITY_H
