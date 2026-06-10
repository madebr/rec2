#ifndef GUARD_41_UTILITY_H
#define GUARD_41_UTILITY_H

#include "brender/br_types.h"
#include "c2_hooks.h"
#include "rec2_types.h"

extern char* gMisc_strings[300];

extern br_error C2_HOOK_FASTCALL DRBrEnd(void);

// Uppercaseificate

// CheckQuit

// sqr

extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);

// PercentageChance

// IRandomPosNeg

// FRandomBetween

// FRandomPosNeg

// SRandomBetween

// SRandomPosNeg

extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);

// tandeg

// GetFileLength

extern br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags);

extern br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocateSub(br_pixelmap* pPm, br_uint_16 pX, br_uint_16 pY, br_uint_16 pW, br_uint_16 pH);

// DRImageLoad

// DRPixelmapLoad

// SepDirAndFilename

// DRLoadMultiplePix

// DRPixelmapLoadMany

// WaitFor

extern intptr_t DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);

// CompareActorID

// DRActorFindRecurse

// DRActorEnumRecurseWithMat

// DRActorEnumRecurseWithTrans

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

// NobbleNonzeroBlacks

extern void C2_HOOK_FASTCALL GlorifyMaterial(br_material** pMaterials, int pCount, tRendererShadingType pShading);

// FindBestColourMatch

// WhitenVertexRGB

// ArenaOpenFile

// BRPM_convert

// PrintScreen

// FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport

// GetBlenficatiousnessOfMaterialTablishly

// GetBlenficatiousnessOfMaterialPrimitively

// GetBlenficatiousnessOfMaterial

// MungeCommas

// MungeMetaCharacters

// MungeMetaCharactersChar

// MungeMetaCharactersNum

// DrPixelmapRectangleCopyPossibleLock

// PixelmapSwapByteOrder

extern void C2_HOOK_FASTCALL EnsurePixelmapAllowed(br_pixelmap* pMap, undefined4 pArg2);

#endif // GUARD_41_UTILITY_H
