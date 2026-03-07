#include "41-utility.h"

#include "02-init.h"
#include "globvars.h"

#include "c2_string.h"

// FUNCTION: CARMA2_HW 0x00513400
br_error C2_HOOK_FASTCALL DRBrEnd(void) {
    br_device *dev;

    if (!gBr_initialized) {
        return 0x1006;
    }

    _BrEndHook();
    gBr_initialized = 0;
    while (BrDevFind(&dev, NULL) == 0) {
        if (dev != NULL) {
            BrDevRemove(dev);
        }
    }
    return 0;
}


// Uppercaseificate

// CheckQuit

// sqr

// IRandomBetween

// PercentageChance

// IRandomPosNeg

// FRandomBetween

// FRandomPosNeg

// SRandomBetween

// SRandomPosNeg

// FUNCTION: CARMA2_HW 0x00513690
void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2) {

    if (pDestn_str != pStr_1) { // Added to avoid strcpy overlap checks
        strcpy(pDestn_str, pStr_1);
    }
    if (strlen(pStr_2) != 0) {
        strcat(pDestn_str, gDir_separator);
        strcat(pDestn_str, pStr_2);
    }
}

// tandeg

// GetFileLength

// DRPixelmapAllocate

// DRPixelmapAllocateSub

// DRImageLoad

// DRPixelmapLoad

// SepDirAndFilename

// DRLoadMultiplePix

// DRPixelmapLoadMany

// WaitFor

// DRActorEnumRecurse

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

// GetMiscString

// Flash

// RGBDifferenceSqr

// FindBestMatch

// BuildShadeTablePath

// LoadGeneratedShadeTable

// SaveGeneratedShadeTable

// GenerateShadeTable

// GenerateDarkenedShadeTable

// PossibleService

// DRMatrix34TApplyP

// DRPixelmapRectangleCopy

// NormalSideOfPlane

// DRMaterialClone

// DRStricmp

// DRstrlwr

// PDCheckDriveExists

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

// STUB: CARMA2_HW 0x00516c30
int C2_HOOK_FASTCALL PossibleUnlock(int pValue) {
    NOT_IMPLEMENTED();
    return 0;
}

// PaletteEntry16Bit

// Colour24BitTo16Bit

// PaletteOf16Bits

// Copy8BitTo16Bit

// DRPixelmapCopy

// FindExceptionInList

// NobbleNonzeroBlacks

// GlorifyMaterial

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

// EnsurePixelmapAllowed

