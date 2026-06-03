#include "08-loading1.h"

#include "69-sound.h"
#include "70-packfile.h"
#include "41-utility.h"
#include "platform.h"
#include "globvars.h"

#include <string.h>

#ifdef REC2_MATCHING
#include <windows.h>
#endif

// GLOBAL: CARMA2_HW 0x00655e30
int gDecode_thing = '@';

// GLOBAL: CARMA2_HW 0x00658610
int gAllow_open_to_fail = 1;

// GLOBAL: CARMA2_HW 0x00658614
int g_source_exists = 1;

// FUNCTION: CARMA2_HW 0x0048f830
tU32 C2_HOOK_FASTCALL ReadU32(FILE* pF) {
    tU32 raw_long;

    PFfread(&raw_long, sizeof(raw_long), 1, pF);
    return raw_long;
}

// ReadF32

// ReadS32

// WriteU32L

// WriteU16L

// WriteU8L

// MemReadU32

// MemReadU16

// MemReadU8

// MemReadS16

// MemReadS8

// MemSkipBytes

// GetALineAndInterpretCommand

// GetAnInt

// GetAFloat

// GetPairOfFloats

// GetThreeFloats

// GetAVector3

// GetPairOfInts

// GetThreeInts

// GetFourInts

// GetAScalar

// GetPairOfScalars

// GetThreeScalars

// GetFourScalars

// GetNScalars

// GetPairOfFloatPercents

// GetThreeFloatPercents

// GetAString

// StripCR

// SubsStringJob

// STUB: CARMA2_HW 0x00490f30
char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(FILE* pF, char* pS) {
    NOT_IMPLEMENTED();
}

// GetALineAndDontArgue

FILE* OldDRfopen(const char* pFilename, const char* pMode) {
    FILE* fp;

    fp = PFfopen(pFilename, pMode);
    if (fp == NULL) {
        const char* data_pos;
        char source_check[256];

        if (gCD_fully_installed) {
            return NULL;
        }
        if (g_source_exists == 1) {
            char general_file[256];
            char paths_file[256];
            char source_check[256];

            strcpy(general_file, "DATA");
            strcat(general_file, gDir_separator);
            strcat(general_file, "GENERAL.TXT");
            strcpy(paths_file, gApplication_path);
            strcat(paths_file, gDir_separator);
            strcat(paths_file, "PATHS.TXT");
            if (!PDCheckDriveExists(paths_file)) {
                g_source_exists = 0;
                return NULL;
            }
            fp = PFfopen(paths_file, "rt");
            if (fp == NULL) {
                g_source_exists = 0;
                return NULL;
            }
            source_check[0] = '\0';
            PossibleService();
            GetALineWithNoPossibleService(fp, source_check);
            if (strchr(gDir_separator, source_check[strlen(source_check) - 1]) == NULL) {
                strcat(source_check, gDir_separator);
            }
            strcat(source_check, general_file);
            PFfclose(fp);
            if (!PDCheckDriveExists(source_check)) {
                PDFatalError("Carmageddon CD not in drive.");
                if (gCD_fully_installed) {
                    g_source_exists = 0;
                }
            } else {
                g_source_exists += 1;
            }
        }
        if (g_source_exists == 0) {
            return NULL;
        }
        data_pos = strstr(pFilename, "DATA");
        if (data_pos == NULL) {
            return NULL;
        }

        if (!GetRegisterSourceLocation(source_check)) {
            return NULL;
        }
        if (strchr(gDir_separator, source_check[strlen(source_check) - 1]) == NULL) {
            strcat(source_check, gDir_separator);
        }
        strcat(source_check, data_pos);
        if (!PDCheckDriveExists(source_check)) {
            return NULL;
        }
        return PFfopen(source_check, pMode);
    } else {
        int len;
        char ch;

        if (!gDecode_thing) {
            return fp;
        }
        len = strlen(pFilename);
        if (strcmp(&pFilename[len - 4], ".TXT") != 0
                || strcmp(&pFilename[len - 12], "DKEYMAP0.TXT") == 0
                || strcmp(&pFilename[len - 12], "DKEYMAP1.TXT") == 0
                || strcmp(&pFilename[len - 12], "DKEYMAP2.TXT") == 0
                || strcmp(&pFilename[len - 12], "DKEYMAP3.TXT") == 0
                || strcmp(&pFilename[len - 12], "KEYMAP_0.TXT") == 0
                || strcmp(&pFilename[len - 12], "KEYMAP_1.TXT") == 0
                || strcmp(&pFilename[len - 12], "KEYMAP_2.TXT") == 0
                || strcmp(&pFilename[len - 12], "KEYMAP_3.TXT") == 0
                || strcmp(&pFilename[len - 11], "OPTIONS.TXT") == 0
                || strcmp(&pFilename[len - 12], "KEYNAMES.TXT") == 0
                || strcmp(&pFilename[len - 10], "KEYMAP.TXT") == 0
                || strcmp(&pFilename[len - 9], "PATHS.TXT") == 0
                || strcmp(&pFilename[len - 11], "PRATCAM.TXT") == 0) {
            return fp;
        }
        ch = PFfgetc(fp);
        if (ch == gDecode_thing) {
            PFungetc(ch, fp);
            return fp;
        }
        PFfclose(fp);
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004910b0
void C2_HOOK_FASTCALL AllowOpenToFail(void) {

    gAllow_open_to_fail = 1;
}

// FUNCTION: CARMA2_HW 0x004910c0
void C2_HOOK_FASTCALL DoNotAllowOpenToFail(void) {

}

// GetCDPathFromPathsTxtFile

// FUNCTION: CARMA2_HW 0x00491170
FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode) {
    FILE *result;

    result = OldDRfopen(pFilename, pMode);
    if (result == NULL && !gAllow_open_to_fail) {
        char buffer[256];

        if (GetRegisterSourceLocation(buffer)) {
            if (!PDCheckDriveExists(buffer)) {
                if (gMisc_strings[0] != NULL) {
                    PDFatalError(GetMiscString(eMiscString_could_not_find_carmageddon_cd));
                } else {
                    PDFatalError("Could not find the Carmageddon CD");
                }
            }
        }
        sprintf(buffer, "DRfopen( \"%s\", \"%s\" ) failed", pFilename, pMode);
        PDFatalError(buffer);
    }
    return result;
}

// UnlockInterfaceStuff

// DisposePhysicsObject

// DisposeCarActModDataCB

// DisposeCar

// DisposeRaceInfo

// DisposeOpponentsCars

// DisposeRace

// DisposeGameIfNecessary

// DisposeTrack

// FUNCTION: CARMA2_HW 0x004910d0
int GetRegisterSourceLocation(char* pSource_location) {
#ifdef REC2_MATCHING
    HKEY hKey;
    LONG status;
    char message[256];

    DWORD size = 255;
    status = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\SCI\\CARMAGEDDON2", 0, KEY_ALL_ACCESS, &hKey);
    if (status == ERROR_SUCCESS) {
        status = RegQueryValueExA(hKey, "SourceLocation", NULL, NULL, (LPBYTE)pSource_location, &size);
        if (status == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return 1;
        } else {
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, status, 0, message, size, NULL);
            RegCloseKey(hKey);
        }
    }
    return 0;
#else
    strcpy(pSource_location, "DATA");
    return 1;
#endif
}
