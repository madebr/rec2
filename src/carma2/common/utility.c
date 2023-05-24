#include "utility.h"

#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "platform.h"
#include "world.h"

#include "brender/brender.h"

#include "c2_ctype.h"
#include "c2_string.h"

static br_error (C2_HOOK_FASTCALL * RemoveAllBrenderDevices_original)(void);
br_error C2_HOOK_FASTCALL RemoveAllBrenderDevices(void) {
#if defined(C2_HOOKS_ENABLED)
    return RemoveAllBrenderDevices_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00513400, RemoveAllBrenderDevices, RemoveAllBrenderDevices_original);

void C2_HOOK_FASTCALL StringToUpper(char* dest, const char* src) {
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
C2_HOOK_FUNCTION(0x00513460, StringToUpper)

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
        result = DRfgets(s, sizeof(s), pF);
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
#if defined(C2_HOOKS_ENABLED)
    PossibleService_original();
#else
#error "not implemented"
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
    return LoadTiffOrBrenderTexture_Ex(pathDir, textureName, C2V(gRender_palette), C2V(gPixelFlags), &errorFlags);
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

void C2_HOOK_FASTCALL ExtractPath_Dirname_Stem(const char* path, char* dirPath, char* stemPath) {
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
C2_HOOK_FUNCTION(0x005139a0, ExtractPath_Dirname_Stem)

tU32 C2_HOOK_FASTCALL GetFileLength(FILE* pF) {
    tU32 the_size;

    DRfseek(pF, 0, SEEK_END);
    the_size = DRftell(pF);
    DRrewind(pF);
    return the_size;
}
C2_HOOK_FUNCTION(0x00513790, GetFileLength)

double C2_HOOK_FASTCALL sqr(double pN) {

    return pN * pN;
}
C2_HOOK_FUNCTION(0x00513510, sqr)
