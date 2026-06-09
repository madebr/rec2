#include "41-utility.h"

#include "01-network.h"
#include "02-init.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"

#include "c2_string.h"
#include <ctype.h>


// GLOBAL: CARMA2_HW 0x006abef8
tU32 last_service;

// GLOBAL: CARMA2_HW 0x006b5f40
char* gMisc_strings[300];

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

// STUB: CARMA2_HW 0x00513520
int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB) {
    NOT_IMPLEMENTED();
}

// PercentageChance

// IRandomPosNeg

// FRandomBetween

// FRandomPosNeg

// SRandomBetween

// SRandomPosNeg

// FUNCTION: CARMA2_HW 0x00513690
void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2) {

#ifdef REC2_FIX_BUGS
    if (pDestn_str != pStr_1) { // Added to avoid strcpy overlap checks
        strcpy(pDestn_str, pStr_1);
    }
#else
    strcpy(pDestn_str, pStr_1);
#endif

    if (strlen(pStr_2) != 0) {
        strcat(pDestn_str, gDir_separator);
        strcat(pDestn_str, pStr_2);
    }
}

// tandeg

// GetFileLength

// FUNCTION: CARMA2_HW 0x005137d0
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags) {
    br_pixelmap* pm;

    pm = BrPixelmapAllocate(pType, pW, pH, pPixels, pFlags);
    if (pm != NULL) {
        pm->origin_x = 0;
        pm->origin_y = 0;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00513820
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocateSub(br_pixelmap* pPm, br_uint_16 pX, br_uint_16 pY, br_uint_16 pW, br_uint_16 pH) {
    br_pixelmap* the_map;

    the_map = BrPixelmapAllocateSub(pPm, pX, pY, pW, pH);
    if (the_map != NULL) {
        the_map->origin_x = 0;
        the_map->origin_y = 0;
    }
    return the_map;
}

// DRImageLoad

// DRPixelmapLoad

// SepDirAndFilename

// DRLoadMultiplePix

// DRPixelmapLoadMany

// WaitFor

// FUNCTION: CARMA2_HW 0x005146f0
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg) {
    intptr_t result;

    result = callback(pActor, arg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurse(pActor, callback, arg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

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

// FUNCTION: CARMA2_HW 0x00514d70
const char* C2_HOOK_FASTCALL GetMiscString(int pIndex) {

    return gMisc_strings[pIndex];
}

// Flash

// RGBDifferenceSqr

// FindBestMatch

// BuildShadeTablePath

// LoadGeneratedShadeTable

// SaveGeneratedShadeTable

// GenerateShadeTable

// GenerateDarkenedShadeTable

// FUNCTION: CARMA2_HW 0x005155d0
void C2_HOOK_FASTCALL PossibleService(void) {
    tU32 time;

    time = PDGetTotalTime();
    if (time - last_service > 200 && !gProgram_state.racing) {
        SoundService();
        NetService(gProgram_state.racing);
        last_service = time;
    }
}

// DRMatrix34TApplyP

// DRPixelmapRectangleCopy

// NormalSideOfPlane

// DRMaterialClone

// FUNCTION: CARMA2_HW 0x00515870
int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2) {
    int val;

    val = tolower(*p1);
    val -= tolower(*p2);
    while (val == 0) {
        if (*p1++ == '\0') {
            break;
        }
        if (*p2++ == '\0') {
            break;
        }
        val = tolower(*p1);
        val -= tolower(*p2);
    }
    return val;
}

// FUNCTION: CARMA2_HW 0x005158d0
void C2_HOOK_FASTCALL DRstrlwr(char* s) {
    int l = (int)strlen(s);
    int i;

    for (i = 0; i < l; i++) {
        s[i] = toupper(s[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00515950
int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path) {
    return PDCheckDriveExists2(pThe_path, NULL, 0);
}

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

// FUNCTION: CARMA2_HW 0x005193f0
void C2_HOOK_FASTCALL EnsurePixelmapAllowed(br_pixelmap* pMap, undefined4 pArg2) {

}

