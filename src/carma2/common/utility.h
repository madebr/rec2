#ifndef REC2_UTILITY_H
#define REC2_UTILITY_H

#include "brender/br_types.h"
#include "rec2_types.h"

#include "c2_hooks.h"

#include "c2_stdio.h"

#include <stdint.h>

C2_HOOK_VARIABLE_DECLARE_ARRAY(const tU8, gLong_key, 16);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const tU8, gOther_long_key, 16);
C2_HOOK_VARIABLE_DECLARE(int, gDecode_thing);
C2_HOOK_VARIABLE_DECLARE(tU32, last_service);
C2_HOOK_VARIABLE_DECLARE(br_vector3, y_unit_vector);

br_error C2_HOOK_FASTCALL DRBrEnd(void);

void C2_HOOK_FASTCALL StringTransformToLower(char* pStr);

void C2_HOOK_FASTCALL Uppercaseificate(char* dest, const char* src);

int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path);

char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(FILE* pF, char* pS);

char* C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);

void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);

void C2_HOOK_FASTCALL DecodeLine2(char* pS);

void C2_HOOK_FASTCALL EncodeLine2(char* pS);

void C2_HOOK_FASTCALL EncodeFile(char* pThe_path);

int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);

tU32 C2_HOOK_FASTCALL GetTotalTime(void);

void C2_HOOK_FASTCALL PossibleService(void);

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags);

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocateSub(br_pixelmap* pPm, br_uint_16 pX, br_uint_16 pY, br_uint_16 pW, br_uint_16 pH);

br_pixelmap* C2_HOOK_FASTCALL DRLoadUpdatePixelmapFromTif(const char* path);

void C2_HOOK_FASTCALL DRPixelmapRectangleCopy(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_uint_16 w, br_uint_16 h);

intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);

void C2_HOOK_FASTCALL SepDirAndFilename(const char* path, char* dirPath, char* stemPath);

tU32 C2_HOOK_FASTCALL GetFileLength(FILE* pF);

double C2_HOOK_FASTCALL sqr(double pN);

br_pixelmap* C2_HOOK_FASTCALL GenerateShadeTable(int pHeight, br_pixelmap* pPalette, int pRed_mix, int pGreen_mix, int pBlue_mix, float pQuarter, float pHalf, float pThree_quarter);

int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);

float C2_HOOK_STDCALL FRandomBetween(float pA, float pB);

br_scalar C2_HOOK_STDCALL SRandomBetween(br_scalar pA, br_scalar pB);

const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);

void C2_HOOK_FASTCALL AddLostTime(tU32 pLost_time);

int C2_HOOK_FASTCALL CheckQuit(void);

float C2_HOOK_STDCALL tandeg(float pAngle);

intptr_t C2_HOOK_CDECL CompareActorID(br_actor* pActor, void* pArg);

br_actor* C2_HOOK_FASTCALL DRActorFindRecurse(br_actor* pSearch_root, const char* pName);

FILE* C2_HOOK_FASTCALL OpenUniqueFileB(char* pPrefix, char* pExtension);

void C2_HOOK_FASTCALL PrintScreenFile(FILE* pF);

void C2_HOOK_FASTCALL PrintScreenFile16(FILE* pF);

void C2_HOOK_FASTCALL PrintScreen(void);

int C2_HOOK_CDECL DumpVisibleActorsCB(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL DumpActorTree(br_actor* pActor, const char* pMsg);

tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp);

int C2_HOOK_FASTCALL LoadTextureTryAllLocations(char* pName, br_pixelmap** pMaps, int pCapacity);

int C2_HOOK_FASTCALL PercentageChance(int pC);

tU32 C2_HOOK_FASTCALL GetRaceTime(void);

void C2_HOOK_FASTCALL BlendifyMaterialTablishly(br_material* pMaterial, int pPercent);

void C2_HOOK_FASTCALL BlendifyMaterialPrimitively(br_material* pMaterial, int pPercent);

void C2_HOOK_FASTCALL BlendifyMaterial(br_material* pMaterial, int pPercent);

float C2_HOOK_STDCALL FRandomPosNeg(float pN);

br_scalar C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN);

void C2_HOOK_FASTCALL PixelmapSwapByteOrder(br_pixelmap* pMap);

#endif // REC2_UTILITY_H
