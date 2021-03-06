#include "utility.h"

#include "globvars.h"
#include "platform.h"

#include "brender/brender.h"

#include "c2_ctype.h"
#include "c2_string.h"

static br_error (C2_HOOK_FASTCALL * RemoveAllBrenderDevices_original)(void);
br_error C2_HOOK_FASTCALL RemoveAllBrenderDevices(void) {
    return RemoveAllBrenderDevices_original();
}
C2_HOOK_FUNCTION_ORIGINAL(0x00513400, RemoveAllBrenderDevices, RemoveAllBrenderDevices_original);

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

void C2_HOOK_FASTCALL PathCat(char* pDestn_str, char* pStr_1, const char* pStr_2) {

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
