#ifndef GUARD_41_UTILITY_H
#define GUARD_41_UTILITY_H

#include "brender/br_types.h"
#include "c2_hooks.h"
#include "rec2_types.h"

extern char* gMisc_strings[300];

extern br_error C2_HOOK_FASTCALL DRBrEnd(void);

extern void C2_HOOK_FASTCALL Uppercaseificate(char* dest, const char* src);

extern int C2_HOOK_FASTCALL CheckQuit(void);

extern double C2_HOOK_FASTCALL sqr(double pN);

extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);

extern int C2_HOOK_FASTCALL PercentageChance(int pC);

extern int C2_HOOK_FASTCALL IRandomPosNeg(int pN);

extern float C2_HOOK_STDCALL FRandomBetween(float pA, float pB);

extern float C2_HOOK_STDCALL FRandomPosNeg(float pN);

extern br_scalar C2_HOOK_STDCALL SRandomBetween(br_scalar pA, br_scalar pB);

extern br_scalar C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN);

extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);

// tandeg

extern tU32 C2_HOOK_FASTCALL GetFileLength(FILE* pF);

extern br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags);

extern br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocateSub(br_pixelmap* pPm, br_uint_16 pX, br_uint_16 pY, br_uint_16 pW, br_uint_16 pH);

// DRImageLoad

// DRPixelmapLoad

extern void C2_HOOK_FASTCALL SepDirAndFilename(const char* path, char* dirPath, char* stemPath);

// DRLoadMultiplePix

extern int C2_HOOK_FASTCALL DRPixelmapLoadMany(const char* texturePathNoExt, br_pixelmap** pixelmaps, size_t capacity);

// WaitFor

extern intptr_t DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);

extern intptr_t C2_HOOK_CDECL CompareActorID(br_actor* pActor, void* pArg);

extern br_actor* C2_HOOK_FASTCALL DRActorFindRecurse(br_actor* pSearch_root, const char* pName);

extern br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithMat(br_actor* pActor, br_material* pMat, recurse_with_mat_cbfn* pCall_back, void* pArg);

extern br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithTrans(br_actor* pActor, br_matrix34* pMatrix, recurse_with_trans_cbfn* pCall_back, void* pArg);

// DRActorEnumRecurseWithSnart

// sign

// OpenUniqueFileB

// PrintScreenFile

// PrintScreenFile16

// GetTotalTime

// GetRaceTime

// AddLostTime

// AssertThisTimeAsCurrentTime

// TimerString

extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);

// Flash

// RGBDifferenceSqr

// FindBestMatch

// BuildShadeTablePath

// LoadGeneratedShadeTable

// SaveGeneratedShadeTable

// GenerateShadeTable

// GenerateDarkenedShadeTable

extern void C2_HOOK_FASTCALL PossibleService(void);

// DRMatrix34TApplyP

// DRPixelmapRectangleCopy

// NormalSideOfPlane

// DRMaterialClone

extern int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);

extern void C2_HOOK_FASTCALL DRstrlwr(char* s);

int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path);

// CloneActor

// CalcActorGlobalPos

// frac

// FindMaterialCB

// FindMaterial

// BlendifyMaterialTablishly

// BlendifyMaterialPrimitively

// BlendifyMaterial

// DRModelUpdateAndKevificateMaterials

// DRModelUpdateDeluxTurbo

// DistanceFromFace

// DRBoundsCopy

// TestForNan

// DRVector3TestForNan

// DRScaleModel

// DistanceFromFaceND

// DRVector3NonZero

// DRVector3Diminish

// DRScalarToU16

// DRU16ToScalar

// CompressVector3

// ExpandVector3

// CompressMatrix34

// ExpandMatrix34

// PossibleLock

extern int C2_HOOK_FASTCALL PossibleUnlock(int pValue);

// PaletteEntry16Bit

// Colour24BitTo16Bit

// PaletteOf16Bits

// Copy8BitTo16Bit

// DRPixelmapCopy

// tMaterial_exception* C2_HOOK_FASTCALL FindExceptionInList(const char* pIdentifier, tMaterial_exception* pList)

extern void C2_HOOK_FASTCALL NobbleNonzeroBlacks(br_pixelmap* pPalette);

extern void C2_HOOK_FASTCALL GlorifyMaterial(br_material** pMaterials, int pCount, tRendererShadingType pShading);

// FindBestColourMatch

extern void C2_HOOK_FASTCALL WhitenVertexRGB(br_model** pModels, int pCount);

// ArenaOpenFile

// BRPM_convert

// PrintScreen

// FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport

// GetBlenficatiousnessOfMaterialTablishly

// GetBlenficatiousnessOfMaterialPrimitively

// GetBlenficatiousnessOfMaterial

extern char* C2_HOOK_FASTCALL MungeCommas(int pValue);

extern void C2_HOOK_FASTCALL MungeMetaCharacters(char* pText, char pMeta, const char* pRepl);

extern void C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pMeta, char pChar);

extern void C2_HOOK_FASTCALL MungeMetaCharactersNum(char* pText, char pMeta, int pNum);

extern void C2_HOOK_FASTCALL DrPixelmapRectangleCopyPossibleLock(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 w, br_int_32 h);

extern void C2_HOOK_FASTCALL PixelmapSwapByteOrder(br_pixelmap* pMap);

extern void C2_HOOK_FASTCALL EnsurePixelmapAllowed(br_pixelmap* pMap, undefined4 pArg2);

#endif // GUARD_41_UTILITY_H
