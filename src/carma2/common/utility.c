#include "utility.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "main.h"
#include "mainmenu.h"
#include "network.h"
#include "platform.h"
#include "sound.h"
#include "world.h"

#include "brender/brender.h"

#include "rec2_macros.h"

#include "c2_ctype.h"
#include "c2_string.h"

#include <float.h>
#include <math.h>

C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLost_time, 0x006abef4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gIn_check_quit, 0x006abee0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const tU8, gLong_key, 16, 0x006585f0, {
    0x6c, 0x1b, 0x99, 0x5f, 0xb9, 0xcd, 0x5f, 0x13,
    0xcb, 0x04, 0x20, 0x0e, 0x5e, 0x1c, 0xa1, 0x0e,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const tU8, gOther_long_key, 16, 0x00658600, {
    0x67, 0xa8, 0xd6, 0x26, 0xb6, 0xdd, 0x45, 0x1b,
    0x32, 0x7e, 0x22, 0x13, 0x15, 0xc2, 0x94, 0x37,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDecode_thing, 0x00655e30, '@');
C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_service, 0x006abef8);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_vector3, y_unit_vector, 0x00655df0, { { 0.f, 1.f, 0.f } });

static br_error (C2_HOOK_FASTCALL * DRBrEnd_original)(void);
br_error C2_HOOK_FASTCALL DRBrEnd(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    return DRBrEnd_original();
#else
    br_device *dev;

    if (!C2V(gBr_initialized)) {
        return 0x1006;
    }

    _BrEndHook();
    C2V(gBr_initialized) = 0;
    while (BrDevFind(&dev, NULL) == 0) {
        if (dev != NULL) {
            BrDevRemove(dev);
        }
    }
    return 0;

#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00513400, DRBrEnd, DRBrEnd_original);

void C2_HOOK_FASTCALL StringTransformToLower(char* pStr) {
    int i;
    int len;

    len = c2_strlen(pStr);
    for (i = 0; i < len; i++) {
        pStr[i] = c2_tolower(pStr[i]);
    }
}
C2_HOOK_FUNCTION(0x00515910, StringTransformToLower)

void C2_HOOK_FASTCALL Uppercaseificate(char* dest, const char* src) {
    size_t lenSrc;
    size_t nbLeft;

    lenSrc = c2_strlen(src);
    nbLeft = lenSrc;
    while (nbLeft != 0) {
        *dest = c2_toupper(*src);
        dest++;
        src++;
        nbLeft--;
    }
    dest[0] = '\0';
}
C2_HOOK_FUNCTION(0x00513460, Uppercaseificate)

int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path) {
    return PDCheckDriveExists2(pThe_path, NULL, 0);
}
C2_HOOK_FUNCTION(0x00515950, PDCheckDriveExists)

char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(FILE* pF, char* pS) {
    char* result;
    char s[256];
    int ch;
    size_t len;

    do {
        result = PFfgets(s, sizeof(s), pF);
        if (result == NULL) {
            pS[0] = '\0';
            return pS;
        }
        while (*result == ' ' || *result == '\t') {
            result++;
        }

        while (1) {
            ch = DRfgetc(pF);
            if (ch != '\r' && ch != '\n') {
                break;
            }
        }
        if (ch != -1) {
            DRungetc(ch, pF);
        }
    } while (!c2_isalnum(*result)
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
             && *result >= 0);

    len = c2_strlen(result);
    while (len > 0 && (result[len - 1] == '\r' || result[len - 1] == '\n')) {
        len--;
    }
    result[len] = '\0';
    c2_strncpy(pS, result, len + 1);
    return pS;
}
C2_HOOK_FUNCTION(0x00490f30, GetALineWithNoPossibleService)

char* C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS) {

    PossibleService();
    return GetALineWithNoPossibleService(pF, pS);
}
C2_HOOK_FUNCTION(0x00491090, GetALineAndDontArgue)

void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2) {

    if (pDestn_str != pStr_1) { // Added to avoid strcpy overlap checks
        c2_strcpy(pDestn_str, pStr_1);
    }
    if (c2_strlen(pStr_2) != 0) {
        c2_strcat(pDestn_str, C2V(gDir_separator));
        c2_strcat(pDestn_str, pStr_2);
    }
}
C2_HOOK_FUNCTION(0x00513690, PathCat)

void C2_HOOK_FASTCALL DecodeLine2(char* pS) {
    int len;
    int seed;
    int i;
    unsigned char c;
    const tU8* key;

    len = c2_strlen(pS);
    key = C2V(gLong_key);
    while (len > 0 && (pS[len - 1] == '\r' || pS[len - 1] == '\n')) {
        len--;
        pS[len] = '\0';
    }
    seed = len % 16;
    for (i = 0; i < len; i++) {
        c = pS[i];
        if (i >= 2) {
            if (pS[i - 1] == '/' && pS[i - 2] == '/') {
                key = C2V(gOther_long_key);
            }
        }
        if (c == '\t') {
            c = 0x9f;
        }

        c -= 0x20;
        c ^= key[seed];
        c &= 0x7f;
        c += 0x20;

        seed += 7;
        seed %= 16;

        if (c == 0x9f) {
            c = '\t';
        }
        pS[i] = c;
    }
}

void C2_HOOK_FASTCALL EncodeLine2(char* pS) {
    int len;
    int seed;
    int i;
    int count;
    unsigned char c;
    const tU8* key;

    len = c2_strlen(pS);
    count = 0;
    key = C2V(gLong_key);
    while (len > 0 && (pS[len - 1] == '\r' || pS[len - 1] == '\n')) {
        len--;
        pS[len] = '\0';
    }

    seed = len % 16;

    for (i = 0; i < len; i++) {
        if (count == 2) {
            key = C2V(gOther_long_key);
        }
        if (pS[i] == '/') {
            count++;
        } else {
            count = 0;
        }
        if (pS[i] == '\t') {
            pS[i] = 0x9f;
        }

        c = pS[i] - 0x20;
        c ^= key[seed];
        c &= 0x7f;
        c += 0x20;

        seed += 7;
        seed %= 16;

        if (c == 0x9f) {
            c = '\t';
        }
        pS[i] = c;
    }
}

void (C2_HOOK_FASTCALL * EncodeFile_original)(char* pThe_path);
void C2_HOOK_FASTCALL EncodeFile(char* pThe_path) {

#if 0//defined(C2_HOOKS_ENABLED)
    EncodeFile_original(pThe_path);
#else
    FILE* f;
    FILE* d;
    char line[257];
    char new_file[256];
    char* result;
    int ch;
    int decode;
    int len;
    int count;

    len = c2_strlen(pThe_path);
    c2_strcpy(new_file, pThe_path);
    c2_strcpy(&new_file[len - 3], "ENC");

    f = PFfopen(pThe_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantOpen_S, pThe_path);
    }

    ch = DRfgetc(f);
    DRungetc(ch, f);

    if (C2V(gDecode_thing) == '@' && ch == '@') {
        PFfclose(f);
        return;
    }

    d = PFfopen(new_file, "wb");
    if (d == NULL) {
        FatalError(kFatalError_CantOpen_S, new_file);
    }

    result = &line[1];

    while (PFfgets(result, 256, f) || !PFfeof(f)) {

        if (result[0] == '@') {
            decode = 1;
        } else {
            decode = 0;
            // Strip leading whitespace
            while (result[0] == ' ' || result[0] == '\t') {
                c2_memmove(result, &result[1], c2_strlen(result) - 1);
            }
        }

        if (decode) {
            DecodeLine2(&result[decode]);
        } else {
            EncodeLine2(&result[decode]);
        }

        line[0] = '@';
        c2_fputs(&line[decode * 2], d);
        count = -1;
        while (1) {
            count++;
            ch = DRfgetc(f);
            if (ch == '\r' || ch == '\n') {
                continue;
            }
        }
        if (count > 2) {
            c2_fputc('\r', d);
            c2_fputc('\n', d);
        }
        c2_fputc('\r', d);
        c2_fputc('\n', d);

        if (ch != -1) {
            DRungetc(ch, f);
        }
    }
    PFfclose(f);
    PFfclose(d);

    PDFileUnlock(pThe_path);
    c2_unlink(pThe_path);
    c2_rename(new_file, pThe_path);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00490840, EncodeFile, EncodeFile_original)

int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2) {
    int val;

    while (1) {
        val = c2_tolower(*p1) - c2_tolower(*p2);
        if (val != 0 || *p1 == '\0' || *p2 == '\0') {
            return val;
        }
        p1++;
        p2++;
    }
}
C2_HOOK_FUNCTION(0x00515870, DRStricmp)

static tU32 (C2_HOOK_FASTCALL * GetTotalTime_original)(void);
tU32 C2_HOOK_FASTCALL GetTotalTime(void) {
#if defined(C2_HOOKS_ENABLED)
    return GetTotalTime_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00514c30, GetTotalTime, GetTotalTime_original)

void (C2_HOOK_FASTCALL * PossibleService_original)(void);
void C2_HOOK_FASTCALL PossibleService(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    PossibleService_original();
#else
    tU32 time;

    time = PDGetTotalTime();
    if (time - C2V(last_service) > 200 && !C2V(gProgram_state).racing) {
        SoundService();
        NetService(C2V(gProgram_state).racing);
        C2V(last_service) = time;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005155d0, PossibleService, PossibleService_original)

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags) {
    br_pixelmap* pm;

    pm = BrPixelmapAllocate(pType, pW, pH, pPixels, pFlags);
    if (pm != NULL) {
        pm->origin_x = 0;
        pm->origin_y = 0;
    }
    return pm;
}
C2_HOOK_FUNCTION(0x005137d0, DRPixelmapAllocate)

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocateSub(br_pixelmap* pPm, br_uint_16 pX, br_uint_16 pY, br_uint_16 pW, br_uint_16 pH) {
    br_pixelmap* the_map;

    the_map = BrPixelmapAllocateSub(pPm, pX, pY, pW, pH);
    if (the_map != NULL) {
        the_map->origin_y = 0;
        the_map->origin_x = 0;
    }
    return the_map;
}
C2_HOOK_FUNCTION(0x00513820, DRPixelmapAllocateSub)

br_pixelmap* C2_HOOK_FASTCALL DRLoadUpdatePixelmapFromTif(const char* path) {
    int errorFlags;
    tPath_name pathBuffer;
    tPath_name pathDir;
    tPath_name textureName;
    size_t fileStart;
    size_t pathLen;
    size_t i;

    c2_strcpy(pathBuffer, path);
    pathLen = c2_strlen(pathBuffer);
    for (fileStart = pathLen - 1; fileStart != 0; fileStart--) {
        if (pathBuffer[fileStart] == *C2V(gDir_separator)) {
            break;
        }
    }
    c2_strncpy(pathDir, pathBuffer, fileStart);
    pathDir[fileStart] = '\0';
    if (pathDir[0] != '\0') {
        fileStart += 1;
    }
    for (i = 0; i < pathLen - fileStart; i++) {
        textureName[i] = pathBuffer[fileStart + i];
    }
    textureName[pathLen - fileStart] = '\0';
    return DRLdImg(pathDir, textureName, C2V(gRender_palette), C2V(gPixelFlags), &errorFlags);
}
C2_HOOK_FUNCTION(0x00513870, DRLoadUpdatePixelmapFromTif)

void C2_HOOK_FASTCALL DRPixelmapRectangleCopy(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_uint_16 w, br_uint_16 h) {
    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
}
C2_HOOK_FUNCTION(0x005191b0, DRPixelmapRectangleCopy)

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
C2_HOOK_FUNCTION(0x005146f0, DRActorEnumRecurse)

void C2_HOOK_FASTCALL SepDirAndFilename(const char* path, char* dirPath, char* stemPath) {
    size_t pathLen;
    size_t dirLen;
    size_t stemIndex;

    pathLen = c2_strlen(path);
    for (dirLen = pathLen - 1; dirLen != 0; dirLen--) {
        if (path[dirLen] == C2V(gDir_separator)[0]) {
            break;
        }
    }
    c2_strncpy(dirPath, path, dirLen);
    dirPath[dirLen] = '\0';
    if (*dirPath != '\0') {
        dirLen += 1;
    }
    stemIndex = 0;
    while (path[dirLen + stemIndex] != '.' && dirLen + stemIndex != pathLen) {
        stemPath[stemIndex] = path[dirLen + stemIndex];
        stemIndex += 1;
    }
    stemPath[stemIndex] = '\0';
}
C2_HOOK_FUNCTION(0x005139a0, SepDirAndFilename)

tU32 C2_HOOK_FASTCALL GetFileLength(FILE* pF) {
    tU32 the_size;

    PFfseek(pF, 0, SEEK_END);
    the_size = PFftell(pF);
    PFrewind(pF);
    return the_size;
}
C2_HOOK_FUNCTION(0x00513790, GetFileLength)

double C2_HOOK_FASTCALL sqr(double pN) {

    return pN * pN;
}
C2_HOOK_FUNCTION(0x00513510, sqr)

void C2_HOOK_FASTCALL BuildShadeTablePath(char* pThe_path, int pR, int pG, int pB) {
    char s[32];

    s[0] = 's';
    s[1] = 't';
    s[2] = 'A' + ((pR & 0xf0) >> 4);
    s[3] = 'A' + ((pR & 0x0f) >> 0);
    s[4] = 'A' + ((pG & 0xf0) >> 4);
    s[5] = 'A' + ((pG & 0x0f) >> 0);
    s[6] = 'A' + ((pB & 0xf0) >> 4);
    s[7] = 'A' + ((pB & 0x0f) >> 0);
    s[8] = '\0';
    c2_strcat(s, ".TAB");
    PathCat(pThe_path, C2V(gApplication_path), "SHADETAB");
    PathCat(pThe_path, pThe_path, s);
}

br_pixelmap* C2_HOOK_FASTCALL LoadGeneratedShadeTable(int pR, int pG, int pB) {
    char the_path[256];

    BuildShadeTablePath(the_path, pR, pG, pB);
    return BrPixelmapLoad(the_path);
}

void C2_HOOK_FASTCALL SaveGeneratedShadeTable(br_pixelmap* pThe_table, int pR, int pG, int pB) {
    char the_path[256];

    BuildShadeTablePath(the_path, pR, pG, pB);
    BrPixelmapSave(the_path, pThe_table);
}

double C2_HOOK_FASTCALL RGBDifferenceSqr(tRGB_colour* pColour_1, tRGB_colour* pColour_2) {

    return ((pColour_1->red - pColour_2->red) * (pColour_1->red - pColour_2->red))
           + ((pColour_1->green - pColour_2->green) * (pColour_1->green - pColour_2->green))
           + ((pColour_1->blue - pColour_2->blue) * (pColour_1->blue - pColour_2->blue));
}

int C2_HOOK_FASTCALL FindBestMatch(tRGB_colour* pRGB_colour, br_pixelmap* pPalette) {
    int n;
    int near_c;
    double min_d;
    double d;
    tRGB_colour trial_RGB;
    br_colour* dp;

    near_c = 127;
    min_d = DBL_MAX;
    dp = pPalette->pixels;
    for (n = 0; n < 256; n++) {
        trial_RGB.red = (dp[n] >> 16) & 0xff;
        trial_RGB.green = (dp[n] >> 8) & 0xff;
        trial_RGB.blue = (dp[n] >> 0) & 0xff;
        d = RGBDifferenceSqr(pRGB_colour, &trial_RGB);
        if (d < min_d) {
            min_d = d;
            near_c = n;
        }
    }
    return near_c;
}

br_pixelmap* C2_HOOK_FASTCALL GenerateDarkenedShadeTable(int pHeight, br_pixelmap* pPalette, int pRed_mix, int pGreen_mix, int pBlue_mix, float pQuarter, float pHalf, float pThree_quarter, br_scalar pDarken) {
    br_pixelmap* the_table;
    tRGB_colour the_RGB;
    tRGB_colour new_RGB;
    tRGB_colour ref_col;
    br_colour* cp;
    char* tab_ptr;
    char* shade_ptr;
    double f_i;
    double f_total_minus_1;
    double ratio1;
    double ratio2;
    int i;
    int c;

    the_table = LoadGeneratedShadeTable(pRed_mix, pGreen_mix, pBlue_mix);
    if (the_table == NULL) {
        the_table = BrPixelmapAllocate(BR_PMT_INDEX_8, 256, pHeight, NULL, 0);
        if (the_table == NULL) {
            FatalError(kFatalError_CannotLoadAGeneratedShadeTable);
        }
        cp = pPalette->pixels;

        ref_col.red = pRed_mix;
        ref_col.green = pGreen_mix;
        ref_col.blue = pBlue_mix;

        for (c = 0, tab_ptr = the_table->pixels; c < 256; c++, tab_ptr++) {
            the_RGB.red = (int)(((cp[c] >> 16) & 0xff) * pDarken);
            the_RGB.green = (int)(((cp[c] >> 8) & 0xff) * pDarken);
            the_RGB.blue = (int)(((cp[c] >> 0) & 0xff) * pDarken);

            if (pHeight == 1) {
                f_total_minus_1 = 1.;
            } else {
                f_total_minus_1 = pHeight - 1;
            }
            shade_ptr = tab_ptr;
            for (i = 0, shade_ptr = tab_ptr; i < pHeight; i++, shade_ptr += 0x100) {
                if (pHeight == 1) {
                    f_i = 1.f;
                } else {
                    f_i = (float)i;
                }
                ratio1 = f_i / f_total_minus_1;
                if (ratio1 < .5f) {
                    if (ratio1 < .25) {
                        ratio2 = pQuarter * ratio1 * 4.;
                    } else {
                        ratio2 = (ratio1 - .25f) * (pHalf - pQuarter) * 4. + pQuarter;
                    }
                } else {
                    if (ratio1 < 0.75f) {
                        ratio2 = (ratio1 - .5f) * (pThree_quarter - pHalf) * 4. + pHalf;
                    } else {
                        ratio2 = 1.f - (1.f - pThree_quarter) * (1.f - ratio1) * 4.f;
                    }
                }
                new_RGB.red = (int)(ref_col.red * ratio2 + the_RGB.red * (1. - ratio2));
                new_RGB.green = (int)(ref_col.green * ratio2 + the_RGB.green * (1. - ratio2));
                new_RGB.blue = (int)(ref_col.blue * ratio2 + the_RGB.blue * (1. - ratio2));
                *shade_ptr = FindBestMatch(&new_RGB, pPalette);
            }
        }
        SaveGeneratedShadeTable(the_table, pRed_mix, pGreen_mix, pBlue_mix);
    }
    BrTableAdd(the_table);
    return the_table;
}
C2_HOOK_FUNCTION(0x00514eb0, GenerateDarkenedShadeTable)

br_pixelmap* C2_HOOK_FASTCALL GenerateShadeTable(int pHeight, br_pixelmap* pPalette, int pRed_mix, int pGreen_mix, int pBlue_mix, float pQuarter, float pHalf, float pThree_quarter) {

    PossibleService();
    return GenerateDarkenedShadeTable(
            pHeight,
            pPalette,
            pRed_mix,
            pGreen_mix,
            pBlue_mix,
            pQuarter,
            pHalf,
            pThree_quarter,
            1.0f);
}
C2_HOOK_FUNCTION(0x00514e40, GenerateShadeTable)

int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB) {
    int num;

    num = c2_rand();
#if RAND_MAX == 0x7fff
    //  If RAND_MAX == 0x7fff, then `num` can be seen as a fixed point number with 15 fractional and 17 integral bits
    return pA + ((num * (pB + 1 - pA)) >> 15);
#else
    //  If RAND_MAX != 0x7fff, then use floating numbers (alternative is using modulo)
    return pA + (int)((pB + 1 - pA) * (num / ((float)RAND_MAX + 1)));
#endif
}
C2_HOOK_FUNCTION(0x00513520, IRandomBetween)

float C2_HOOK_STDCALL FRandomBetween(float pA, float pB) {

    return (float)rand() * (pB - pA) / (float)RAND_MAX + pA;
}
C2_HOOK_FUNCTION(0x005135b0, FRandomBetween)

br_scalar C2_HOOK_STDCALL SRandomBetween(br_scalar pA, br_scalar pB) {

    return FRandomBetween(pA, pB);
}
C2_HOOK_FUNCTION(0x00513620, SRandomBetween)

const char* C2_HOOK_FASTCALL GetMiscString(int pIndex) {

    return C2V(gMisc_strings)[pIndex];
}
C2_HOOK_FUNCTION(0x00514d70, GetMiscString)

void C2_HOOK_FASTCALL AddLostTime(tU32 pLost_time) {

    C2V(gLost_time) += pLost_time;
}
C2_HOOK_FUNCTION(0x00514c80, AddLostTime)

int C2_HOOK_FASTCALL CheckQuit(void) {

    if (C2V(gIn_check_quit)) {
        return 0;
    }
    if (KeyIsDown(1) && KeyIsDown(7)) {
        C2V(gIn_check_quit) = 1;
        while (AnyKeyDown()) {
        }
        if (DoVerifyQuit(1)) {
            QuitGame();
        }
        C2V(gIn_check_quit) = 0;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x005134b0, CheckQuit)

float C2_HOOK_STDCALL tandeg(float pAngle) {

    pAngle = (float)DEG_TO_RAD(pAngle);
    return sinf(pAngle) / cosf(pAngle);
}
C2_HOOK_FUNCTION(0x00513770, tandeg)

intptr_t C2_HOOK_CDECL CompareActorID(br_actor* pActor, void* pArg) {

    if (pActor->identifier != NULL && c2_strcmp(pActor->identifier, (const char*)pArg) == 0) {
        return (intptr_t)pActor;
    } else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x005147b0, CompareActorID)

br_actor* (C2_HOOK_FASTCALL * DRActorFindRecurse_original)(br_actor* pSearch_root, const char* pName);
br_actor* C2_HOOK_FASTCALL DRActorFindRecurse(br_actor* pSearch_root, const char* pName) {

#if 0//defined(C2_HOOKS_ENABLED)
    return DRActorFindRecurse_original(pSearch_root, pName);
#else
    return (br_actor*)DRActorEnumRecurse(pSearch_root, CompareActorID, (void*)pName);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00514730, DRActorFindRecurse, DRActorFindRecurse_original)

FILE* C2_HOOK_FASTCALL OpenUniqueFileB(char* pPrefix, char* pExtension) {
    int index;
    FILE* f;
    tPath_name the_path;

    for (index = 0; index < 10000; index++) {
        PathCat(the_path, C2V(gApplication_path), pPrefix);
        c2_sprintf(the_path + c2_strlen(the_path), "%04d.%s", index, pExtension);
        f = DRfopen(the_path, "rt");
        if (f == NULL) {
            return DRfopen(the_path, "wb");
        }
        PFfclose(f);
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x005149a0, OpenUniqueFileB)

void C2_HOOK_FASTCALL PrintScreenFile(FILE* pF) {
    int i;
    int j;
    int bit_map_size;
    int offset;

    bit_map_size = C2V(gBack_screen)->height * C2V(gBack_screen)->row_bytes;

    // 1. BMP Header
    //    1. 'BM' Signature
    WriteU8L(pF, 'B');
    WriteU8L(pF, 'M');
    //    2. File size in bytes (header = 0xe bytes; infoHeader = 0x28 bytes; colorTable = 0x400 bytes; pixelData = xxx)
    WriteU32L(pF, bit_map_size + 0x436);
    //    3. unused
    WriteU16L(pF, 0);
    //    4. unused
    WriteU16L(pF, 0);
    //    5. pixelData offset (from beginning of file)
    WriteU32L(pF, 0x436);

    // 2. Info Header
    //    1. InfoHeader Size
    WriteU32L(pF, 0x28);
    //    2. Width of bitmap in pixels
    WriteU32L(pF, C2V(gBack_screen)->row_bytes);
    //    3. Height of bitmap in pixels
    WriteU32L(pF, C2V(gBack_screen)->height);
    //    4. Number of planes
    WriteU16L(pF, 1);
    //    5. Bits per pixels / palletization (8 -> 8bit palletized ==> #colors = 256)
    WriteU16L(pF, 8);
    //    6. Compression (0 = BI_RGB -> no compression)
    WriteU32L(pF, 0);
    //    7. Image Size (0 --> no compression)
    WriteU32L(pF, 0);
    //    8. Horizontal Pixels Per Meter
    WriteU32L(pF, 0);
    //    9. Vertical Pixels Per Meter
    WriteU32L(pF, 0);
    //    10. # Actually used colors
    WriteU32L(pF, 0);
    //    11. Number of important colors
    WriteU32L(pF, 256);

    // 3. Color table (=palette)
    for (i = 0; i < 256; i++) {
        // red, green, blue, unused
        WriteU8L(pF, ((tU8*)C2V(gCurrent_palette)->pixels)[4 * i + 0]);
        WriteU8L(pF, ((tU8*)C2V(gCurrent_palette)->pixels)[4 * i + 1]);
        WriteU8L(pF, ((tU8*)C2V(gCurrent_palette)->pixels)[4 * i + 2]);
        WriteU8L(pF, 0);
    }

    // 4. Pixel Data (=LUT)
    offset = bit_map_size - C2V(gBack_screen)->row_bytes;
    for (i = 0; i < C2V(gBack_screen)->height; i++) {
        for (j = 0; j < C2V(gBack_screen)->row_bytes; j++) {
            WriteU8L(pF, ((tU8*)C2V(gBack_screen)->pixels)[offset]);
            offset++;
        }
        offset -= 2 * C2V(gBack_screen)->row_bytes;
    }
    WriteU16L(pF, 0);
}
C2_HOOK_FUNCTION(0x00514ab0, PrintScreenFile)

void C2_HOOK_FASTCALL PrintScreenFile16(FILE* pF) {
    int i;
    int j;
    int bit_map_size;
    int offset;

    if (C2V(gBack_screen)->pixels == NULL) {
        BrPixelmapDirectLock(C2V(gBack_screen), 1);
    }

    bit_map_size = C2V(gBack_screen)->height * 3 * C2V(gBack_screen)->width;

    // 1. BMP Header
    //    1. 'BM' Signature
    WriteU8L(pF, 'B');
    WriteU8L(pF, 'M');
    //    2. File size in bytes (header = 0xe bytes; infoHeader = 0x28 bytes; colorTable = 0x400 bytes; pixelData = xxx)
    WriteU32L(pF, 0x36 + bit_map_size);
    //    3. unused
    WriteU16L(pF, 0);
    //    4. unused
    WriteU16L(pF, 0);
    //    5. pixelData offset (from beginning of file)
    WriteU32L(pF, 0x36);

    // 2. Info Header
    //    1. InfoHeader Size
    WriteU32L(pF, 0x28);
    //    2. Width of bitmap in pixels
    WriteU32L(pF, C2V(gBack_screen)->width);
    //    3. Height of bitmap in pixels
    WriteU32L(pF, C2V(gBack_screen)->height);
    //    4. Number of planes
    WriteU16L(pF, 1);
    //    5. Bits per pixels / palletization (0x18 -> 24bit colours ==> #colors = 2^24)
    WriteU16L(pF, 0x18);
    //    6. Compression (0 = BI_RGB -> no compression)
    WriteU32L(pF, 0);
    //    7. Image Size (0 --> no compression)
    WriteU32L(pF, 0);
    //    8. Horizontal Pixels Per Meter
    WriteU32L(pF, 0);
    //    9. Vertical Pixels Per Meter
    WriteU32L(pF, 0);
    //    10. # Actually used colors
    WriteU32L(pF, 0);
    //    11. Number of important colors
    WriteU32L(pF, 256);

    // 4. Pixel Data (=LUT)
    offset = C2V(gBack_screen)->row_bytes * (C2V(gBack_screen)->height - 1);
    for (i = 0; i < C2V(gBack_screen)->height; i++) {
        for (j = 0; j < C2V(gBack_screen)->width; j++) {
            tU8 r, g, b;
            tU16 pixel = *(tU16*)&((tU8*)C2V(gBack_screen)->pixels)[offset];
            if (C2V(gBack_screen)->type == BR_PMT_RGB_565) {
                b = pixel << 3;
                g = (pixel >> 3) & 0xf8;
                r = (pixel >> 8) & 0xf8;
            } else if (C2V(gBack_screen)->type == BR_PMT_RGB_555) {
                b = pixel << 3;
                g = (pixel >> 2) & 0xf8;
                r = (pixel >> 7) & 0xf8;
            } else {
                b = 0;
                g = 0;
                r = 0;
            }
            WriteU8L(pF, b);
            WriteU8L(pF, g);
            WriteU8L(pF, r);
            offset += 2;
        }
        offset -= 2 * C2V(gBack_screen)->width + C2V(gBack_screen)->row_bytes;
    }
    WriteU16L(pF, 0);

    if (C2V(gBack_screen)->pixels != NULL) {
        BrPixelmapDirectUnlock(C2V(gBack_screen));
    }
}

void C2_HOOK_FASTCALL PrintScreen(void) {
    FILE* f;

    f = OpenUniqueFileB("DUMP", "BMP");
    if (f != NULL) {
        if (C2V(gBack_screen)->type == BR_PMT_INDEX_8) {
            PrintScreenFile(f);
        } else {
            PrintScreenFile16(f);
        }
        PFfclose(f);
    }
}
C2_HOOK_FUNCTION(0x00518780, PrintScreen)

int C2_HOOK_CDECL DumpVisibleActorsCB(br_actor* pActor, void* pData) {

    if (pActor->render_style != BR_RSTYLE_NONE) {
        const char* str;

        switch (pActor->type) {
        case BR_ACTOR_NONE:
            str = "DUMMY ACTOR\n";
            break;
        case BR_ACTOR_MODEL:
            str = "MODEL ACTOR\n";
            break;
        case BR_ACTOR_LIGHT:
            str = "LIGHT ACTOR\n";
            break;
        case BR_ACTOR_CAMERA:
            str = "CAMERA ACTOR\n";
            break;
        case BR_ACTOR_BOUNDS:
            str = "BOUNDS ACTOR\n";
            break;
        case BR_ACTOR_BOUNDS_CORRECT:
            str = "BOUNDS-CORRECT ACTOR\n";
            break;
        case BR_ACTOR_CLIP_PLANE:
            str = "CLIP-PLANE ACTOR\n";
            break;
        default:
            str = "******** UNKNOWN ********\n";
            break;
        }
        c2_printf(str);
        str = pActor->identifier;
        if (str == NULL) {
            str = "No Identifier";
        }
        c2_printf("\tName:\t\t\"%s\"\n", str);
        c2_printf("\tAddress:\t%08x\n", pActor);
        c2_printf("\tParent:\t\t%08x\n", pActor->parent);
        c2_printf("\tDepth:\t\t%i\n", pActor->depth);
        c2_printf("\tModelPtr:\t0x%08x", pActor->model);
        if (pActor->model == NULL) {
            c2_printf("\n");
        } else {
            str = pActor->model->identifier;
            if (str == NULL) {
                str = "No Identifier";
            }
            c2_printf(" (\"%s\")\n", str);
        }
        c2_printf("\tMaterialPtr:\t0x%08x", pActor->material);
        if (pActor->material == NULL) {
            c2_printf("\n");
        } else {
            str = pActor->material->identifier;
            if (str == NULL) {
                str = "No Identifier";
            }
            c2_printf(" (\"%s\")\n", str);
        }
        switch (pActor->render_style) {
        case BR_RSTYLE_DEFAULT:
            str = "BR_RSTYLE_DEFAULT";
            break;
        case BR_RSTYLE_NONE:
            str = "BR_RSTYLE_NONE";
            break;
        case BR_RSTYLE_POINTS:
            str = "BR_RSTYLE_POINTS";
            break;
        case BR_RSTYLE_EDGES:
            str = "BR_RSTYLE_EDGES";
            break;
        case BR_RSTYLE_FACES:
            str = "BR_RSTYLE_FACES";
            break;
        case BR_RSTYLE_BOUNDING_POINTS:
            str = "BR_RSTYLE_BOUNDING_POINTS";
            break;
        case BR_RSTYLE_BOUNDING_EDGES:
            str = "BR_RSTYLE_BOUNDING_EDGES";
            break;
        case BR_RSTYLE_BOUNDING_FACES:
            str = "BR_RSTYLE_BOUNDING_FACES";
            break;
        default:
            str = "******** UNKNOWN ********";
            break;
        }
        c2_printf("\tRenderStyle:\t%s\n", str);
        c2_printf("\n");
    }
    return BrActorEnum(pActor, DumpVisibleActorsCB, pData);
}
C2_HOOK_FUNCTION(0x00518b00, DumpVisibleActorsCB)

void C2_HOOK_FASTCALL DumpVisibleActors(br_actor* pActor, const char* pMsg) {

    c2_printf("MSG:\"%s\"\n", pMsg);
    BrActorEnum(pActor, DumpVisibleActorsCB, NULL);
    c2_printf("----------------------------------\n\n");
    c2_fflush(c2_stdout);
}
C2_HOOK_FUNCTION(0x00518ac0, DumpVisibleActors)

tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp) {

    switch (pm->type) {

    case BR_PMT_RGB_555:
        return (((red & 0xf8) >> 3) << 10) | (((grn & 0xf8) >> 3) << 5) | (((blu & 0xf8) >> 3) << 0);
    case BR_PMT_RGB_565:
        return (((red & 0xf8) >> 3) << 11) | (((grn & 0xfc) >> 2) << 5) | (((blu & 0xf8) >> 3) << 0);
    case BR_PMT_RGBA_8888:
        return (alp << 24) | (red << 16) | (red << 8) | (blu << 0);
    case BR_PMT_RGBA_4444:
        return (((red & 0xf0) >> 4) << 8) | (((grn & 0xf0) >> 4) << 4) | (((blu & 0xf0) >> 4) << 0) | (((alp & 0xf0) >> 4) << 12);
    case BR_PMT_ARGB_1555:
        return (((red & 0xf8) >> 3) << 10) | (((grn & 0xf8) >> 3) << 5) | (((blu & 0xf8) >> 3) << 0) | (((alp & 0x80) >> 7) << 15);
    case BR_PMT_RGB_888:
    default:
        return (red << 16) | (red << 8) | (blu << 0);
    }
}
C2_HOOK_FUNCTION(0x00518d60, FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport)

int (C2_HOOK_FASTCALL * LoadTextureTryAllLocations_original)(char* pName, br_pixelmap** pMaps, int pCapacity);
int C2_HOOK_FASTCALL LoadTextureTryAllLocations(char* pName, br_pixelmap** pMaps, int pCapacity) {

#if 0//defined(C2_HOOKS_ENABLED)
    return LoadTextureTryAllLocations_original(pName, pMaps, pCapacity);
#else
    char path1[256];
    char path2[256];
    char path3[256];
    char path4[256];
    tName_list list;
    size_t i;
    int error;

    list.size = 0;
    C2_HOOK_ASSERT(pName[c2_strlen(pName) - 4] == '.');
    pName[c2_strlen(pName) - 4] = '\0';

    PathCat(path1, C2V(gApplication_path), C2V(gCurrent_load_directory));
    if (c2_strcmp(C2V(gCurrent_load_directory), "COMMON") != 0 && c2_strcmp(C2V(gCurrent_load_directory), "INTRFACE") != 0) {
        PathCat(path1, path1, C2V(gCurrent_load_name));
    }
    PathCat(path1, path1, C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
    PathCat(path1, path1, pName);

    PathCat(path3, C2V(gApplication_path), C2V(gCurrent_load_directory));
    if (c2_strcmp(C2V(gCurrent_load_directory), "COMMON") != 0 && c2_strcmp(C2V(gCurrent_load_directory), "INTRFACE") != 0) {
        PathCat(path3, path3, C2V(gCurrent_load_name));
    }
    PathCat(path3, path3, pName);

    PathCat(path2, path1, "TIFFX");
    PFForEveryFile2(path2, (tEnumPathCallback)AddTexturePixTifFileStemToList, &list);

    PathCat(path4, path1, "PIX8");
    PFForEveryFile2(path4, (tEnumPathCallback)AddTextureFileStemToList, &list);

    PathCat(path2, path1, "TIFFRGB");
    PFForEveryFile2(path2, (tEnumPathCallback)AddTextureFileStemToList, &list);

    PathCat(path4, path1, "PIX16");
    PFForEveryFile2(path4, (tEnumPathCallback)AddTextureFileStemToList, &list);

    if (list.size == 0) {
        PathCat(path2, path3, "TIFFX");
        PFForEveryFile2(path2, (tEnumPathCallback)AddTexturePixTifFileStemToList, &list);

        PathCat(path4, path3, "PIX8");
        PFForEveryFile2(path4, (tEnumPathCallback)AddTextureFileStemToList, &list);

        PathCat(path2, path3, "TIFFRGB");
        PFForEveryFile2(path2, (tEnumPathCallback)AddTextureFileStemToList, &list);

        PathCat(path4, path3, "PIX16");
        PFForEveryFile2(path4, (tEnumPathCallback)AddTextureFileStemToList, &list);

        for (i = 0; i < list.size; i++) {
            pMaps[i] = DRLdImg(path3, list.items[i], C2V(gRender_palette), C2V(gPixelFlags), &error);
        }
    } else {
        for (i = 0; i < list.size; i++) {
            pMaps[i] = DRLdImg(path1, list.items[i], C2V(gRender_palette), C2V(gPixelFlags), &error);
        }
    }
    return list.size;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00513a30, LoadTextureTryAllLocations, LoadTextureTryAllLocations_original)

int C2_HOOK_FASTCALL PercentageChance(int pC) {

    return IRandomBetween(0, 100) < pC;
}
C2_HOOK_FUNCTION(0x00513550, PercentageChance)

tU32 C2_HOOK_FASTCALL GetRaceTime(void) {

    return PDGetTotalTime() - C2V(gRace_start);
}
C2_HOOK_FUNCTION(0x00514c70, GetRaceTime)

void C2_HOOK_FASTCALL BlendifyMaterialTablishly(br_material* pMaterial, int pPercent) {
    char* s = NULL;

    switch (pPercent) {
    case 0:
    case 100:
        pMaterial->index_blend = NULL;
        return;
    case 25:
        s = "BLEND75.TAB";
        break;
    case 50:
        s = "BLEND50.TAB";
        break;
    case 75:
        s = "BLEND25.TAB";
        break;
    default:
        return;
    }
    pMaterial->index_blend = BrTableFind(s);
    if (pMaterial->index_blend == NULL) {
        pMaterial->index_blend = LoadSingleShadeTable(&C2V(gTrack_storage_space), s);
    }
}

void C2_HOOK_FASTCALL BlendifyMaterialPrimitively(br_material* pMaterial, int pPercent) {

    static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_token_value, alpha25, 3, 0x00661688, {
        { BRT_BLEND_B, { .b = 1 } },
        { BRT_OPACITY_X, { .x = 0x400000 } },
        { 0 },
    });
    static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_token_value, alpha50, 3, 0x006616a0, {
        { BRT_BLEND_B, { .b = 1 } },
        { BRT_OPACITY_X, { .x = 0x800000 } },
        { 0 },
    });
    static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_token_value, alpha75, 3, 0x006616b8, {
        { BRT_BLEND_B, { .b = 1 } },
        { BRT_OPACITY_X, { .x = 0xc00000 } },
        { 0 },
    });

    switch (pPercent) {
    case 25:
        pMaterial->extra_prim = C2V(alpha25);
        break;
    case 50:
        pMaterial->extra_prim = C2V(alpha50);
        break;
    case 75:
        pMaterial->extra_prim = C2V(alpha75);
        break;
    case 0:
    case 1000:
        pMaterial->extra_prim = NULL;
        break;
    }
}

void C2_HOOK_FASTCALL BlendifyMaterial(br_material* pMaterial, int pPercent) {

    if (C2V(gScreen)->type == BR_PMT_INDEX_8) {
        BlendifyMaterialTablishly(pMaterial, pPercent);
    } else {
        BlendifyMaterialPrimitively(pMaterial, pPercent);
    }
}
C2_HOOK_FUNCTION(0x00515e70, BlendifyMaterial)

float C2_HOOK_STDCALL FRandomPosNeg(float pN) {

    return FRandomBetween(-pN, pN);
}
C2_HOOK_FUNCTION(0x005135e0, FRandomPosNeg)

br_scalar C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN) {

    return SRandomBetween(-pN, pN);
}
C2_HOOK_FUNCTION(0x00513650, SRandomPosNeg)

void C2_HOOK_FASTCALL PixelmapSwapByteOrder(br_pixelmap* pMap) {

}
C2_HOOK_FUNCTION(0x005191f0, PixelmapSwapByteOrder)

void C2_HOOK_FASTCALL TimerString(tU32 pTime, char* pStr, undefined4 pArg3, int pFudge_colon, int pFloat) {
    int seconds;

    seconds = (pTime + 500) / 1000;
    if (pFudge_colon || seconds >= 60) {
        c2_sprintf(pStr, "%d:%02d", seconds / 60, seconds % 60);
    } else if (pFloat) {
        c2_sprintf(pStr, "%.1f", (float) pTime / 1000.f);
    } else {
        c2_sprintf(pStr, "%d", seconds);
    }
}
C2_HOOK_FUNCTION(0x00514cb0, TimerString)

int C2_HOOK_FASTCALL Flash(tU32 pPeriod, tU32* pLast_change, int* pCurrent_state) {
    tU32 the_time;

    the_time = PDGetTotalTime();
    if (the_time - *pLast_change > pPeriod) {
        *pCurrent_state = !*pCurrent_state;
        *pLast_change = the_time;
    }
    return *pCurrent_state;
}
C2_HOOK_FUNCTION(0x00514db0, Flash)

br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithMat(br_actor* pActor, br_material* pMat, recurse_with_mat_cbfn* pCall_back, void* pArg) {
    br_uint_32 result;

    if (pActor->material != NULL) {
        pMat = pActor->material;
    }
    result = pCall_back(pActor, pMat, pArg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurseWithMat(pActor, pMat, pCall_back, pArg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00514800, DRActorEnumRecurseWithMat)
