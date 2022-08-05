#include "utility.h"

#include "globvars.h"
#include "platform.h"

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

int C2_HOOK_FASTCALL PDCheckDriveExists(char* pThe_path) {
    return PDCheckDriveExists2(pThe_path, NULL, 0);
}
C2_HOOK_FUNCTION(0x00515950, PDCheckDriveExists)

char* (C2_HOOK_FASTCALL * GetALineWithNoPossibleService_original)(tTWTFILE* pF, char* pS);
char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(tTWTFILE* pF, char* pS) {
#if defined(C2_HOOKS_ENABLED)
    return GetALineWithNoPossibleService_original(pF, pS);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00490f30, GetALineWithNoPossibleService, GetALineWithNoPossibleService_original)

char* C2_HOOK_FASTCALL GetALineAndDontArgue(tTWTFILE* pF, char* pS) {

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
