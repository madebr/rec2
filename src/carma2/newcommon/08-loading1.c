#include "08-loading1.h"

#include "52-errors.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "41-utility.h"
#include "platform.h"
#include "globvars.h"

#include <ctype.h>
#include <stdarg.h>
#include "c2_string.h"

#ifdef REC2_MATCHING
#include <windows.h>
#endif

// GLOBAL: CARMA2_HW 0x00655e30
int gDecode_thing = '@';

// GLOBAL: CARMA2_HW 0x00658610
int gAllow_open_to_fail = 1;

// GLOBAL: CARMA2_HW 0x00658614
int g_source_exists = 1;

// GLOBAL: CARMA2_HW 0x006585f0
const tU8 gLong_key[16] = {
    0x6c, 0x1b, 0x99, 0x5f, 0xb9, 0xcd, 0x5f, 0x13,
    0xcb, 0x04, 0x20, 0x0e, 0x5e, 0x1c, 0xa1, 0x0e,
};

// GLOBAL: CARMA2_HW 0x00658600
const tU8 gOther_long_key[16] = {
    0x67, 0xa8, 0xd6, 0x26, 0xb6, 0xdd, 0x45, 0x1b,
    0x32, 0x7e, 0x22, 0x13, 0x15, 0xc2, 0x94, 0x37,
};

// FUNCTION: CARMA2_HW 0x0048f830
tU32 C2_HOOK_FASTCALL ReadU32(FILE* pF) {
    tU32 raw_long;

    PFfread(&raw_long, sizeof(raw_long), 1, pF);
    // FIXME: byteswap for BE (introduce LittleEndianToNative macro)
    return raw_long;
}

// FUNCTION: CARMA2_HW 0x0048f890
float C2_HOOK_FASTCALL ReadF32(FILE* pF) {
    float f;

    PFfread(&f, sizeof(f), 1, pF);
    // FIXME: byteswap for BE
    return f;
}

// FUNCTION: CARMA2_HW 0x0048f8b0
tS32 C2_HOOK_FASTCALL ReadS32(FILE* pF) {
    tS32 raw_long;

    PFfread(&raw_long, sizeof(raw_long), 1, pF);
    // FIXME: byteswap for BE
    return raw_long;
}

// FUNCTION: CARMA2_HW 0x0048f910
void C2_HOOK_FASTCALL WriteU32L(FILE* pF, tU32 pNumber) {
    tU32 raw_long;

    raw_long = pNumber;
    PFfwrite(&raw_long, sizeof(raw_long), 1, pF);
    // FIXME: byteswap for BE
}

// FUNCTION: CARMA2_HW 0x0048f930
void C2_HOOK_FASTCALL WriteU16L(FILE* pF, tU16 pNumber) {
    tU16 raw_short;

    raw_short = pNumber;
    PFfwrite(&raw_short, sizeof(raw_short), 1, pF);
    // FIXME: byteswap for BE
}

// FUNCTION: CARMA2_HW 0x0048f960
void C2_HOOK_FASTCALL WriteU8L(FILE* pF, tU8 pNumber) {
    tU8 raw_byte;

    raw_byte = pNumber;
    PFfwrite(&raw_byte, sizeof(raw_byte), 1, pF);
}

// FUNCTION: CARMA2_HW 0x0048fa00
tU32 C2_HOOK_FASTCALL MemReadU32(char** pPtr) {
    tU32 raw_long;

    memcpy(&raw_long, *pPtr, sizeof(raw_long));
    *pPtr += sizeof(raw_long);
    // FIXME: byteswap for BE
    return raw_long;
}

// FUNCTION: CARMA2_HW 0x0048fa10
tU16 C2_HOOK_FASTCALL MemReadU16(char** pPtr) {
    tU16 raw_short;

    memcpy(&raw_short, *pPtr, sizeof(raw_short));
    *pPtr += sizeof(raw_short);
    // FIXME: byteswap for BE
    return raw_short;
}

// FUNCTION: CARMA2_HW 0x0048fa20
tU8 C2_HOOK_FASTCALL MemReadU8(char** pPtr) {
    tU8 raw_byte;

    memcpy(&raw_byte, *pPtr, sizeof(raw_byte));
    *pPtr += sizeof(raw_byte);
    return raw_byte;
}

// FUNCTION: CARMA2_HW 0x0048fa40
tS16 C2_HOOK_FASTCALL MemReadS16(char** pPtr) {
    tS16 raw_short;

    memcpy(&raw_short, *pPtr, sizeof(raw_short));
    *pPtr += sizeof(raw_short);
    // FIXME: byteswap for BE
    return raw_short;
}

// FUNCTION: CARMA2_HW 0x0048fa50
tS8 C2_HOOK_FASTCALL MemReadS8(char** pPtr) {
    tS8 raw_byte;

    memcpy(&raw_byte, *pPtr, sizeof(raw_byte));
    *pPtr += sizeof(raw_byte);
    return raw_byte;
}

// FUNCTION: CARMA2_HW 0x0048fa60
void C2_HOOK_FASTCALL MemSkipBytes(char** pPtr, int pBytes_to_skip) {

    *pPtr += pBytes_to_skip;
}

// FUNCTION: CARMA2_HW 0x0048fa70
int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount) {
    int i;
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < pCount; i++) {
        if (DRStricmp(str, pString_list[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0048fb00
int C2_HOOK_FASTCALL GetAnInt(FILE* pF) {
    char s[256];
    int value;
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &value);
    return value;
}

// FUNCTION: CARMA2_HW 0x004900e0
float C2_HOOK_FASTCALL GetAFloat(FILE* pF) {
    char s[256];
    char *str;
    float f;

    PossibleService();
    GetALineWithNoPossibleService(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", &f);
    return f;
}

// FUNCTION: CARMA2_HW 0x0048fc20
void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", pF1);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pF2);
}

// FUNCTION: CARMA2_HW 0x0048fc90
void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", pF1);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pF2);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pF3);
}

// FUNCTION: CARMA2_HW 0x0048fd30
void C2_HOOK_FASTCALL GetAVector3(FILE* pF, br_vector3* pV) {

    GetThreeFloats(pF, &pV->v[0], &pV->v[1], &pV->v[2]);
}

// FUNCTION: CARMA2_HW 0x0048fdc0
void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", pF1);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%d", pF2);
}

// FUNCTION: CARMA2_HW 0x0048fe30
void C2_HOOK_FASTCALL GetThreeInts(FILE* pF, int* pF1, int* pF2, int* pF3) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", pF1);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%d", pF2);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%d", pF3);
}

// FUNCTION: CARMA2_HW 0x00490020
void C2_HOOK_FASTCALL GetFourInts(FILE* pF, int* pF1, int* pF2, int* pF3, int* pF4) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", pF1);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%d", pF2);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%d", pF3);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%d", pF4);
}

// FUNCTION: CARMA2_HW 0x0048fb80
br_scalar C2_HOOK_FASTCALL GetAScalar(FILE* pF) {

    return GetAFloat(pF);
}

// FUNCTION: CARMA2_HW 0x00490130
void GetPairOfScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2) {
    float f1;
    float f2;

    GetPairOfFloats(pF, &f1, &f2);
    *pS1 = f1;
    *pS2 = f2;
}

// FUNCTION: CARMA2_HW 0x004901c0
void C2_HOOK_FASTCALL GetThreeScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3) {
    float f1;
    float f2;
    float f3;

    GetThreeFloats(pF, &f1, &f2, &f3);
    *pS1 = f1;
    *pS2 = f2;
    *pS3 = f3;
}

// FUNCTION: CARMA2_HW 0x00490270
void C2_HOOK_FASTCALL GetFourScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4) {
    char s[256];
    char* str;
    float f1;
    float f2;
    float f3;
    float f4;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", &f1);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", &f2);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", &f3);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", &f4);
    *pS1 = f1;
    *pS2 = f2;
    *pS3 = f3;
    *pS4 = f4;
}

// GetNScalars

// GetPairOfFloatPercents

// GetThreeFloatPercents

// FUNCTION: CARMA2_HW 0x00490630
void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString) {
    char s[256];
    char* str;

    PossibleService();
    GetALineWithNoPossibleService(pF, s);
    str = strtok(s, "\t ,/");
    strcpy(pString, str);
}

// FUNCTION: CARMA2_HW 0x00490690
void C2_HOOK_FASTCALL StripCRNL(char* line) {
    char* loc;

    loc = strchr(line, '\n');
    if (loc != NULL) {
        *loc = '\0';
    }

    loc = strchr(line, '\r');
    if (loc != NULL) {
        *loc = '\0';
    }
}

// FUNCTION: CARMA2_HW 0x004906c0
void C2_HOOK_CDECL SubsStringJob(char *pFormat, ...) {
    char *str;
    char temp[256];
    va_list ap;

    va_start(ap, pFormat);
    while ((str = strchr(pFormat, '%')) != NULL) {
        char *arg = va_arg(ap, char*);
        StripCRNL(arg);
        strcpy(temp, str + 1);
        strcpy(str, arg);
        strcat(pFormat, temp);
    }
    va_end(ap);
}

static void StripCRNLPrefix(char *line) {

    while (line[0] == ' ' || line[0] == '\t') {
        memmove(line, &line[1], strlen(line) - 1);
    }
}

static void C2_HOOK_FASTCALL DecodeLine2(char* pS) {
    int len;
    int seed;
    int i;
    unsigned char c;
    const tU8* key;

    len = strlen(pS);
    key = gLong_key;
    while (len > 0 && (pS[len - 1] == '\r' || pS[len - 1] == '\n')) {
        len--;
        pS[len] = '\0';
    }
    seed = len % 16;
    for (i = 0; i < len; i++) {
        c = pS[i];
        if (i >= 2) {
            if (pS[i - 1] == '/' && pS[i - 2] == '/') {
                key = gOther_long_key;
            }
        }
        if (c == '\t') {
            c = 0x9f;
        }

        c -= 0x20;
        c ^= key[seed];
        c &= 0x7f;
        c += 0x20;
        pS[i] = c;

        seed += 7;
        seed %= 16;

        if (c == 0x9f) {
            pS[i] = '\t';
        }
    }
}

static void C2_HOOK_FASTCALL EncodeLine2(char* pS) {
    int len;
    int seed;
    int i;
    int comment_count;
    unsigned char c;
    const tU8* key;

    len = strlen(pS);
    comment_count = 0;
    key = gLong_key;
    while (len > 0 && (pS[len - 1] == '\r' || pS[len - 1] == '\n')) {
        len--;
        pS[len] = '\0';
    }

    seed = len % 16;

    for (i = 0; i < len; i++) {
        if (comment_count == 2) {
            key = gOther_long_key;
        }
        if (pS[i] == '/') {
            comment_count += 1;
        } else {
            comment_count = 0;
        }
        if (pS[i] == '\t') {
            pS[i] = (char)0x9f;
        }

        c = pS[i] - 0x20;
        c ^= key[seed];
        c &= 0x7f;
        c += 0x20;
        pS[i] = c;

        seed += 7;
        seed %= 16;

        if (c == 0x9f) {
            pS[i] = '\t';
        }
    }
}

// FUNCTION: CARMA2_HW 0x00490840
void C2_HOOK_FASTCALL EncodeFile(char* pThe_path) {
    FILE* f;
    FILE* d;
    char line[257];
    char new_file[256];
    char* result;
    int ch;
    int decode;
    int len;
    int count;

    len = strlen(pThe_path);
    strcpy(new_file, pThe_path);
    memcpy(&new_file[len - 3], "ENC", 4);

    f = PFfopen(pThe_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantOpen_S, pThe_path);
    }

    ch = PFfgetc(f);
    PFungetc(ch, f);

    if (gDecode_thing == '@' && ch == '@') {
        PFfclose(f);
        return;
    }

    d = PFfopen(new_file, "wb");
    if (d == NULL) {
        FatalError(kFatalError_CantOpen_S, new_file);
    }

    result = &line[1];

    for (;;) {
        if (PFfgets(result, 256, f) != NULL) {
            if (result[0] == '@') {
                decode = 1;
            } else {
                decode = 0;
                StripCRNLPrefix(result);
            }

            if (decode) {
                DecodeLine2(&result[decode]);
            } else {
                EncodeLine2(&result[decode]);
            }

            line[0] = '@';
            fputs(&line[decode * 2], d);
            count = -1;
            while (1) {
                count++;
                ch = PFfgetc(f);
                if (ch != '\r' && ch != '\n') {
                    break;
                }
            }
            if (count > 2) {
                fputc('\r', d);
                fputc('\n', d);
            }
            fputc('\r', d);
            fputc('\n', d);

            if (ch != EOF) {
                PFungetc(ch, f);
            }
        }
        if (PFfeof(f)) {
            break;
        }
    }
    PFfclose(f);
    PFfclose(d);

    PDFileUnlock(pThe_path);
    unlink(pThe_path);
    rename(new_file, pThe_path);
}

// FUNCTION: CARMA2_HW 0x00490f30
char * C2_HOOK_FASTCALL GetALineWithNoPossibleService(FILE* pF, char* pS) {
    char* result;
    size_t len;
    char s[256];
    int ch;

    do {
        result = PFfgets(s, sizeof(s), pF);
        if (result == NULL) {
            break;
        }
        while (*result == ' ' || *result == '\t') {
            result++;
        }

        while (1) {
            ch = PFfgetc(pF);
            if (ch != '\r' && ch != '\n') {
                break;
            }
        }
        if (ch != -1) {
            PFungetc(ch, pF);
        }
    } while (!isalnum(*result)
             && *result != '*'
             && *result != '-'
             && *result != '+'
             && *result != '.'
             && *result != '!'
             && *result != '&'
             && *result != '}'
             && *result != '{'
             && *result != '~'
             && *result != '('
             && *result != '\''
             && *result != '\"'
             && (unsigned char)*result < 0x80);

    if (result != NULL) {
        len = strlen(result);
        if (len > 0 && (result[len - 1] == '\n' || result[len - 1] == '\r')) {
            result[len - 1] = '\0';
            len--;
        }
        if (len > 0 && (result[len - 1] == '\n' || result[len - 1] == '\r')) {
            result[len - 1] = '\0';
            len--;
        }
        memcpy(pS, result, len + 1);
#ifdef REC2_FIX_BUGS
        return pS;
#else
        return result;
#endif
    }
    pS[0] = '\0';
    return result;

}

// FUNCTION: CARMA2_HW 0x00491090
char * C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS) {

    PossibleService();
    return GetALineWithNoPossibleService(pF, pS);
}

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

// FUNCTION: CARMA2_HW 0x0044bb80
void C2_HOOK_FASTCALL UnlockInterfaceStuff(void) {

}

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
