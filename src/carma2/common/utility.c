#include "utility.h"

#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "main.h"
#include "mainloop.h"
#include "mainmenu.h"
#include "network.h"
#include "platform.h"
#include "sound.h"
#include "world.h"

#include "brender/brender.h"

#include "rec2_macros.h"

#include "c2_ctype.h"
#include "c2_stdlib.h"
#include "c2_string.h"

#include <float.h>
#include <math.h>


// GLOBAL: CARMA2_HW 0x006abef4
tU32 gLost_time;

// GLOBAL: CARMA2_HW 0x006abee0
int gIn_check_quit;

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

// GLOBAL: CARMA2_HW 0x00655e30
int gDecode_thing = '@';

// GLOBAL: CARMA2_HW 0x006abef8
tU32 last_service;

// GLOBAL: CARMA2_HW 0x00655df0
br_vector3 y_unit_vector = { { 0.f, 1.f, 0.f } };

// GLOBAL: CARMA2_HW 0x006b63f4
br_pixelmap* g16bit_palette;

// GLOBAL: CARMA2_HW 0x006b63f0
br_pixelmap* gPalette_source;

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

// FUNCTION: CARMA2_HW 0x00515910
void C2_HOOK_FASTCALL StringTransformToLower(char* pStr) {
    int i;
    int len;

    len = c2_strlen(pStr);
    for (i = 0; i < len; i++) {
        pStr[i] = c2_tolower(pStr[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00513460
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

// FUNCTION: CARMA2_HW 0x00515950
int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path) {
    return PDCheckDriveExists2(pThe_path, NULL, 0);
}

// FUNCTION: CARMA2_HW 0x00490f30
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

// FUNCTION: CARMA2_HW 0x00491090
char* C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS) {

    PossibleService();
    return GetALineWithNoPossibleService(pF, pS);
}

// FUNCTION: CARMA2_HW 0x00513690
void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2) {

    if (pDestn_str != pStr_1) { // Added to avoid strcpy overlap checks
        c2_strcpy(pDestn_str, pStr_1);
    }
    if (c2_strlen(pStr_2) != 0) {
        c2_strcat(pDestn_str, gDir_separator);
        c2_strcat(pDestn_str, pStr_2);
    }
}

void C2_HOOK_FASTCALL DecodeLine2(char* pS) {
    int len;
    int seed;
    int i;
    unsigned char c;
    const tU8* key;

    len = c2_strlen(pS);
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
    key = gLong_key;
    while (len > 0 && (pS[len - 1] == '\r' || pS[len - 1] == '\n')) {
        len--;
        pS[len] = '\0';
    }

    seed = len % 16;

    for (i = 0; i < len; i++) {
        if (count == 2) {
            key = gOther_long_key;
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

    len = c2_strlen(pThe_path);
    c2_strcpy(new_file, pThe_path);
    c2_strcpy(&new_file[len - 3], "ENC");

    f = PFfopen(pThe_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantOpen_S, pThe_path);
    }

    ch = DRfgetc(f);
    DRungetc(ch, f);

    if (gDecode_thing == '@' && ch == '@') {
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
}

// FUNCTION: CARMA2_HW 0x00515870
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

// FUNCTION: CARMA2_HW 0x00514c30
tU32 C2_HOOK_FASTCALL GetTotalTime(void) {

    if (gAction_replay_mode) {
        return gLast_replay_frame_time;
    }
    if (gNet_mode != eNet_mode_none) {
        return gLast_tick_count + gFrame_period;
    }
    return gFrame_period + gLast_tick_count - gLost_time;
}

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
        the_map->origin_y = 0;
        the_map->origin_x = 0;
    }
    return the_map;
}

// FUNCTION: CARMA2_HW 0x00513870
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
        if (pathBuffer[fileStart] == *gDir_separator) {
            break;
        }
    }
    c2_memcpy(pathDir, pathBuffer, fileStart);
    pathDir[fileStart] = '\0';
    if (pathDir[0] != '\0') {
        fileStart += 1;
    }
    for (i = 0; i < pathLen - fileStart; i++) {
        textureName[i] = pathBuffer[fileStart + i];
    }
    textureName[pathLen - fileStart] = '\0';
    return DRLdImg(pathDir, textureName, gRender_palette, gPixelFlags, &errorFlags);
}

// FUNCTION: CARMA2_HW 0x005191b0
void C2_HOOK_FASTCALL DRPixelmapRectangleCopy(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_uint_16 w, br_uint_16 h) {
    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
}

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

// FUNCTION: CARMA2_HW 0x005139a0
void C2_HOOK_FASTCALL SepDirAndFilename(const char* path, char* dirPath, char* stemPath) {
    size_t pathLen;
    size_t dirLen;
    size_t stemIndex;

    pathLen = c2_strlen(path);
    for (dirLen = pathLen - 1; dirLen != 0; dirLen--) {
        if (path[dirLen] == gDir_separator[0]) {
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

// FUNCTION: CARMA2_HW 0x00513790
tU32 C2_HOOK_FASTCALL GetFileLength(FILE* pF) {
    tU32 the_size;

    PFfseek(pF, 0, SEEK_END);
    the_size = PFftell(pF);
    PFrewind(pF);
    return the_size;
}

// FUNCTION: CARMA2_HW 0x00513510
double C2_HOOK_FASTCALL sqr(double pN) {

    return pN * pN;
}

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
    PathCat(pThe_path, gApplication_path, "SHADETAB");
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

// FUNCTION: CARMA2_HW 0x00514eb0
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

// FUNCTION: CARMA2_HW 0x00514e40
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

// FUNCTION: CARMA2_HW 0x00513520
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

// FUNCTION: CARMA2_HW 0x005135b0
float C2_HOOK_STDCALL FRandomBetween(float pA, float pB) {

    return (float)rand() * (pB - pA) / (float)RAND_MAX + pA;
}

// FUNCTION: CARMA2_HW 0x00513620
br_scalar C2_HOOK_STDCALL SRandomBetween(br_scalar pA, br_scalar pB) {

    return FRandomBetween(pA, pB);
}

// FUNCTION: CARMA2_HW 0x00514d70
const char* C2_HOOK_FASTCALL GetMiscString(int pIndex) {

    return gMisc_strings[pIndex];
}

// FUNCTION: CARMA2_HW 0x00514c80
void C2_HOOK_FASTCALL AddLostTime(tU32 pLost_time) {

    gLost_time += pLost_time;
}

// FUNCTION: CARMA2_HW 0x005134b0
int C2_HOOK_FASTCALL CheckQuit(void) {

    if (gIn_check_quit) {
        return 0;
    }
    if (KeyIsDown(1) && KeyIsDown(7)) {
        gIn_check_quit = 1;
        while (AnyKeyDown()) {
        }
        if (DoVerifyQuit(1)) {
            QuitGame();
        }
        gIn_check_quit = 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00513770
float C2_HOOK_STDCALL tandeg(float pAngle) {

    pAngle = (float)DEG_TO_RAD(pAngle);
    return sinf(pAngle) / cosf(pAngle);
}

// FUNCTION: CARMA2_HW 0x005147b0
intptr_t C2_HOOK_CDECL CompareActorID(br_actor* pActor, void* pArg) {

    if (pActor->identifier != NULL && c2_strcmp(pActor->identifier, (const char*)pArg) == 0) {
        return (intptr_t)pActor;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00514730
br_actor* C2_HOOK_FASTCALL DRActorFindRecurse(br_actor* pSearch_root, const char* pName) {

    return (br_actor*)DRActorEnumRecurse(pSearch_root, CompareActorID, (void*)pName);
}

// FUNCTION: CARMA2_HW 0x005149a0
FILE* C2_HOOK_FASTCALL OpenUniqueFileB(char* pPrefix, char* pExtension) {
    int index;
    FILE* f;
    tPath_name the_path;

    for (index = 0; index < 10000; index++) {
        PathCat(the_path, gApplication_path, pPrefix);
        c2_sprintf(the_path + c2_strlen(the_path), "%04d.%s", index, pExtension);
        f = DRfopen(the_path, "rt");
        if (f == NULL) {
            return DRfopen(the_path, "wb");
        }
        PFfclose(f);
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00514ab0
void C2_HOOK_FASTCALL PrintScreenFile(FILE* pF) {
    int i;
    int j;
    int bit_map_size;
    int offset;

    bit_map_size = gBack_screen->height * gBack_screen->row_bytes;

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
    WriteU32L(pF, gBack_screen->row_bytes);
    //    3. Height of bitmap in pixels
    WriteU32L(pF, gBack_screen->height);
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
        WriteU8L(pF, ((tU8*)gCurrent_palette->pixels)[4 * i + 0]);
        WriteU8L(pF, ((tU8*)gCurrent_palette->pixels)[4 * i + 1]);
        WriteU8L(pF, ((tU8*)gCurrent_palette->pixels)[4 * i + 2]);
        WriteU8L(pF, 0);
    }

    // 4. Pixel Data (=LUT)
    offset = bit_map_size - gBack_screen->row_bytes;
    for (i = 0; i < gBack_screen->height; i++) {
        for (j = 0; j < gBack_screen->row_bytes; j++) {
            WriteU8L(pF, ((tU8*)gBack_screen->pixels)[offset]);
            offset++;
        }
        offset -= 2 * gBack_screen->row_bytes;
    }
    WriteU16L(pF, 0);
}

void C2_HOOK_FASTCALL PrintScreenFile16(FILE* pF) {
    int i;
    int j;
    int bit_map_size;
    int offset;

    if (gBack_screen->pixels == NULL) {
        BrPixelmapDirectLock(gBack_screen, 1);
    }

    bit_map_size = gBack_screen->height * 3 * gBack_screen->width;

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
    WriteU32L(pF, gBack_screen->width);
    //    3. Height of bitmap in pixels
    WriteU32L(pF, gBack_screen->height);
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
    offset = gBack_screen->row_bytes * (gBack_screen->height - 1);
    for (i = 0; i < gBack_screen->height; i++) {
        for (j = 0; j < gBack_screen->width; j++) {
            tU8 r, g, b;
            tU16 pixel = *(tU16*)&((tU8*)gBack_screen->pixels)[offset];
            if (gBack_screen->type == BR_PMT_RGB_565) {
                b = pixel << 3;
                g = (pixel >> 3) & 0xf8;
                r = (pixel >> 8) & 0xf8;
            } else if (gBack_screen->type == BR_PMT_RGB_555) {
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
        offset -= 2 * gBack_screen->width + gBack_screen->row_bytes;
    }
    WriteU16L(pF, 0);

    if (gBack_screen->pixels != NULL) {
        BrPixelmapDirectUnlock(gBack_screen);
    }
}

// FUNCTION: CARMA2_HW 0x00518780
void C2_HOOK_FASTCALL PrintScreen(void) {
    FILE* f;

    f = OpenUniqueFileB("DUMP", "BMP");
    if (f != NULL) {
        if (gBack_screen->type == BR_PMT_INDEX_8) {
            PrintScreenFile(f);
        } else {
            PrintScreenFile16(f);
        }
        PFfclose(f);
    }
}

// FUNCTION: CARMA2_HW 0x00518b00
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
        c2_printf("\tAddress:\t%p\n", pActor);
        c2_printf("\tParent:\t\t%p\n", pActor->parent);
        c2_printf("\tDepth:\t\t%i\n", pActor->depth);
        c2_printf("\tModelPtr:\t0x%p", pActor->model);
        if (pActor->model == NULL) {
            c2_printf("\n");
        } else {
            str = pActor->model->identifier;
            if (str == NULL) {
                str = "No Identifier";
            }
            c2_printf(" (\"%s\")\n", str);
        }
        c2_printf("\tMaterialPtr:\t0x%p", pActor->material);
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

// FUNCTION: CARMA2_HW 0x00518ac0
void C2_HOOK_FASTCALL DumpVisibleActors(br_actor* pActor, const char* pMsg) {

    c2_printf("MSG:\"%s\"\n", pMsg);
    BrActorEnum(pActor, DumpVisibleActorsCB, NULL);
    c2_printf("----------------------------------\n\n");
    c2_fflush(c2_stdout);
}

// FUNCTION: CARMA2_HW 0x00518d60
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

// FUNCTION: CARMA2_HW 0x00513a30
int C2_HOOK_FASTCALL LoadTextureTryAllLocations(char* pName, br_pixelmap** pMaps, int pCapacity) {
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

    PathCat(path1, gApplication_path, gCurrent_load_directory);
    if (c2_strcmp(gCurrent_load_directory, "COMMON") != 0 && c2_strcmp(gCurrent_load_directory, "INTRFACE") != 0) {
        PathCat(path1, path1, gCurrent_load_name);
    }
    PathCat(path1, path1, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(path1, path1, pName);

    PathCat(path3, gApplication_path, gCurrent_load_directory);
    if (c2_strcmp(gCurrent_load_directory, "COMMON") != 0 && c2_strcmp(gCurrent_load_directory, "INTRFACE") != 0) {
        PathCat(path3, path3, gCurrent_load_name);
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
            pMaps[i] = DRLdImg(path3, list.items[i], gRender_palette, gPixelFlags, &error);
        }
    } else {
        for (i = 0; i < list.size; i++) {
            pMaps[i] = DRLdImg(path1, list.items[i], gRender_palette, gPixelFlags, &error);
        }
    }
    return list.size;
}

// FUNCTION: CARMA2_HW 0x00513550
int C2_HOOK_FASTCALL PercentageChance(int pC) {

    return IRandomBetween(0, 100) < pC;
}

// FUNCTION: CARMA2_HW 0x00514c70
tU32 C2_HOOK_FASTCALL GetRaceTime(void) {

    return PDGetTotalTime() - gRace_start;
}

// FUNCTION: CARMA2_HW 0x00515d90
intptr_t C2_HOOK_CDECL FindMaterialCB(br_actor* pActor, void* data) {
    const char* name = data;
    br_model* model = pActor->model;
    int face_i;

    if (model != NULL) {
        for (face_i = 0; face_i < model->nfaces; face_i++) {
            br_face *face = &model->faces[face_i];

            if (face->material != NULL
                    && face->material->identifier != NULL
                    && c2_strcmp(face->material->identifier, name) == 0) {
                return (intptr_t)face->material;
            }
        }
    }
    return (intptr_t)NULL;
}

// FUNCTION: CARMA2_HW 0x00515c40
br_material* C2_HOOK_FASTCALL FindMaterial(const char* pName, br_actor* pActor, int pRecursive) {

    if (pRecursive) {
        return (br_material*)DRActorEnumRecurse(pActor, FindMaterialCB, (void*)pName);
    } else {
        return (br_material*)FindMaterialCB(pActor, (void*)pName);
    }
}

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
        pMaterial->index_blend = LoadSingleShadeTable(&gTrack_storage_space, s);
    }
}

void C2_HOOK_FASTCALL BlendifyMaterialPrimitively(br_material* pMaterial, int pPercent) {

    // GLOBAL: CARMA2_HW 0x00661688
    static br_token_value alpha25[] = {
        { BRT_BLEND_B, { .b = 1 } },
        { BRT_OPACITY_X, { .x = 0x400000 } },
        { 0 },
    };
    // GLOBAL: CARMA2_HW 0x006616a0
    static br_token_value alpha50[] = {
        { BRT_BLEND_B, { .b = 1 } },
        { BRT_OPACITY_X, { .x = 0x800000 } },
        { 0 },
    };
    // GLOBAL: CARMA2_HW 0x006616b8
    static br_token_value alpha75[] = {
        { BRT_BLEND_B, { .b = 1 } },
        { BRT_OPACITY_X, { .x = 0xc00000 } },
        { 0 },
    };

    switch (pPercent) {
    case 25:
        pMaterial->extra_prim = alpha25;
        break;
    case 50:
        pMaterial->extra_prim = alpha50;
        break;
    case 75:
        pMaterial->extra_prim = alpha75;
        break;
    case 0:
    case 1000:
        pMaterial->extra_prim = NULL;
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00515e70
void C2_HOOK_FASTCALL BlendifyMaterial(br_material* pMaterial, int pPercent) {

    if (gScreen->type == BR_PMT_INDEX_8) {
        BlendifyMaterialTablishly(pMaterial, pPercent);
    } else {
        BlendifyMaterialPrimitively(pMaterial, pPercent);
    }
}

// FUNCTION: CARMA2_HW 0x00515fa0
void C2_HOOK_FASTCALL DRModelUpdateAndKevificateMaterials(br_model* pModel, br_uint_16 pFlags) {

    C2_HOOK_BUG_ON(sizeof(v11group) != 0x24);

    if (pModel->nvertices != 0 && pModel->nfaces != 0) {
        int i;

        BrModelUpdate(pModel, pFlags);
        for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {
            v11group* v11g;

            v11g = &V11MODEL(pModel)->groups[i];
            *v11g->face_colours.materials = pModel->faces[*v11g->face_user].material;
        }
    }
}

// FUNCTION: CARMA2_HW 0x005135e0
float C2_HOOK_STDCALL FRandomPosNeg(float pN) {

    return FRandomBetween(-pN, pN);
}

// FUNCTION: CARMA2_HW 0x00513650
br_scalar C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN) {

    return SRandomBetween(-pN, pN);
}

// FUNCTION: CARMA2_HW 0x005191f0
void C2_HOOK_FASTCALL PixelmapSwapByteOrder(br_pixelmap* pMap) {

}

// FUNCTION: CARMA2_HW 0x00514cb0
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

// FUNCTION: CARMA2_HW 0x00514db0
int C2_HOOK_FASTCALL Flash(tU32 pPeriod, tU32* pLast_change, int* pCurrent_state) {
    tU32 the_time;

    the_time = PDGetTotalTime();
    if (the_time - *pLast_change > pPeriod) {
        *pCurrent_state = !*pCurrent_state;
        *pLast_change = the_time;
    }
    return *pCurrent_state;
}

// FUNCTION: CARMA2_HW 0x00514800
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

// FUNCTION: CARMA2_HW 0x00514850
br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithTrans(br_actor* pActor, br_matrix34* pMatrix, recurse_with_trans_cbfn* pCall_back, void* pArg) {
    br_uint_32 result;
    br_matrix34 combined_transform;

    if (pMatrix != NULL) {
        BrMatrix34Mul(&combined_transform, pMatrix, &pActor->t.t.mat);
    } else {
        BrMatrix34Copy(&combined_transform, &pActor->t.t.mat);
    }
    result = pCall_back(pActor, &combined_transform, pArg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurseWithTrans(pActor, &combined_transform, pCall_back, pArg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00515700
int C2_HOOK_FASTCALL NormalSideOfPlane(br_vector3* pPoint, br_vector3* pNormal, br_scalar pD) {

    return BrVector3Dot(pNormal, pNormal) * (BrVector3Dot(pNormal, pPoint) - pD) >= 0.f;
}

// FUNCTION: CARMA2_HW 0x00515780
br_material* C2_HOOK_FASTCALL DRMaterialClone(br_material* pMaterial, int pSet_identifier) {
    br_material* the_material;
    char s[256];
// GLOBAL: CARMA2_HW 0x006abefc
    static int name_suffix;

    the_material = BrMaterialAllocate(NULL);
    the_material->flags = pMaterial->flags;
    the_material->ka = pMaterial->ka;
    the_material->kd = pMaterial->kd;
    the_material->ks = pMaterial->ks;
    the_material->power = pMaterial->power;
    the_material->colour = pMaterial->colour;
    the_material->index_base = pMaterial->index_base;
    the_material->index_range = pMaterial->index_range;
    the_material->index_shade = pMaterial->index_shade;
    the_material->index_blend = pMaterial->index_blend;
    the_material->colour_map = pMaterial->colour_map;
    c2_memcpy(&the_material->map_transform, &pMaterial->map_transform, sizeof(the_material->map_transform));
    if (pSet_identifier) {
        c2_sprintf(s, "%s(%d)", pMaterial->identifier, name_suffix);
        name_suffix += 1;
        the_material->identifier = BrResStrDup(the_material, s);
    }
    BrMaterialAdd(the_material);
    return the_material;
}

int C2_HOOK_FASTCALL GetBlendificatiousnessOfMaterialTablishly(br_material *pMaterial) {

    if (pMaterial->index_blend == BrTableFind("BLEND75.TAB")) {
        return 25;
    }
    if (pMaterial->index_blend == BrTableFind("BLEND50.TAB")) {
        return 50;
    }
    if (pMaterial->index_blend == BrTableFind("BLEND25.TAB")) {
        return 75;
    }
    return 100;
}

int C2_HOOK_FASTCALL GetBlendificatiousnessOfMaterialPrimitively(br_material *pMaterial) {
    br_token_value* prims = pMaterial->extra_prim;

    if (prims != NULL && prims[0].t == BRT_BLEND_B && prims[0].v.b && prims[1].t == BRT_OPACITY_X) {
        return (100 * BrFixedToInt(prims[1].v.x)) >> 8;
    }
    return 100;
}

// FUNCTION: CARMA2_HW 0x00518e70
int C2_HOOK_FASTCALL GetBlendificatiousnessOfMaterial(br_material *pMaterial) {

    if (gScreen->type == BR_PMT_INDEX_8) {
        return GetBlendificatiousnessOfMaterialTablishly(pMaterial);
    }
    else {
        return GetBlendificatiousnessOfMaterialPrimitively(pMaterial);
    }
}

// FUNCTION: CARMA2_HW 0x00516fd0
tU16 C2_HOOK_FASTCALL PaletteEntry16Bit(br_pixelmap* pPal, int pEntry) {
    tU32* src_entry;
    int red;
    int green;
    int blue;

    src_entry = pPal->pixels;
    switch (gBack_screen->type) {
    case BR_PMT_RGB_555:
        red = (src_entry[pEntry] >> 9) & 0x7c00;
        green = (src_entry[pEntry] >> 6) & 0x03e0;
        blue = (src_entry[pEntry] >> 3) & 0x001f;
        break;
    case BR_PMT_RGB_565:
        red = (src_entry[pEntry] >> 8) & 0xf800;
        green = (src_entry[pEntry] >> 5) & 0x07e0;
        blue = (src_entry[pEntry] >> 3) & 0x001f;
        break;
    default:
        BrFailure("Unsupported back buffer type.");
    }
    return red | green | blue;
}

// FUNCTION: CARMA2_HW 0x00517050
tU16 C2_HOOK_FASTCALL Colour24BitTo16Bit(br_colour pColour) {
    int red;
    int green;
    int blue;

    switch (gBack_screen->type) {
    case BR_PMT_RGB_555:
        red = (pColour >> 9) & 0xf800;
        green = (pColour >> 6) & 0x03e0;
        blue = (pColour >> 3) & 0x001f;
        break;
    case BR_PMT_RGB_565:
        red = (pColour >> 8) & 0xf800;
        green = (pColour >> 5) & 0x07e0;
        blue = (pColour >> 3) & 0x001f;
        break;
    default:
        BrFailure("Unsupported back buffer type.");
    }
    return red | green | blue;
}

// FUNCTION: CARMA2_HW 0x005170c0
br_pixelmap* C2_HOOK_FASTCALL PaletteOf16Bits(br_pixelmap* pSrc) {
    tU16* dst_entry;
    int value;

    if (g16bit_palette == NULL) {
        g16bit_palette = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 256, NULL, 0);
        if (g16bit_palette == NULL) {
            FatalError(kFatalError_OOM_S, "16-bit palette");
        }
    }
    if (!gPalette_changed || pSrc != gPalette_source) {
        value = 0;
        dst_entry = g16bit_palette->pixels;
        for (value = 0; value < 256; value++) {
            *dst_entry = PaletteEntry16Bit(pSrc, value);
            dst_entry++;
        }
        gPalette_changed = 1;
        gPalette_source = pSrc;
    }
    return g16bit_palette;
}

void C2_HOOK_FASTCALL Copy8BitTo16Bit(br_pixelmap* pDst, br_pixelmap* pSrc, br_pixelmap* pPalette) {
    int x;
    int y;
    tU8* src_start;
    tU16* dst_start;
    tU16* palette_entry;

    palette_entry = PaletteOf16Bits(pPalette)->pixels;
    for (y = 0; y < pDst->height; y++) {
        src_start = (tU8*)pSrc->pixels + pSrc->row_bytes * y;
        dst_start = (tU16*)((tU8*)pDst->pixels + pDst->row_bytes * y);
        for (x = 0; x < pDst->width; x++) {
            *dst_start = palette_entry[*src_start];
            src_start++;
            dst_start++;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00517d90
void C2_HOOK_FASTCALL DRPixelmapCopy(br_pixelmap* dst, br_pixelmap* src) {

    if (dst->type == src->type) {
        BrPixelmapCopy(dst, src);
    } else if (dst->type != BR_PMT_INDEX_8 && src->type == BR_PMT_INDEX_8) {
        Copy8BitTo16Bit(dst, src, gCurrent_palette);
    }
}

// FUNCTION: CARMA2_HW 0x00514930
int C2_HOOK_FASTCALL sign(int pNumber) {

    if (pNumber > 0) {
        return 1;
    } else if (pNumber < 0) {
        return -1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x005146c0
void C2_HOOK_FASTCALL WaitFor(tU32 pDelay) {
    tU32 start_time;

    start_time = PDGetTotalTime();
    while (start_time + pDelay > PDGetTotalTime() && !AnyKeyDown()) {
        SoundService();
    }
}

// FUNCTION: CARMA2_HW 0x00515610
void C2_HOOK_FASTCALL DRMatrix34TApplyP(br_vector3* pA, br_vector3* pB, br_matrix34* pC) {
    br_scalar t1;
    br_scalar t2;
    br_scalar t3;

    t1 = pB->v[0] - pC->m[3][0];
    t2 = pB->v[1] - pC->m[3][1];
    t3 = pB->v[2] - pC->m[3][2];

    pA->v[0] = pC->m[0][2] * t3 + pC->m[0][1] * t2 + pC->m[0][0] * t1;
    pA->v[1] = pC->m[1][0] * t1 + pC->m[1][2] * t3 + pC->m[1][1] * t2;
    pA->v[2] = pC->m[2][0] * t1 + pC->m[2][1] * t2 + pC->m[2][2] * t3;
}
