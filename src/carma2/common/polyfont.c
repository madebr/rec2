#include "polyfont.h"

void (C2_HOOK_FASTCALL * LoadPolyFont_original)(const char* pName, int pSize, int pIndex);
void C2_HOOK_FASTCALL LoadPolyFont(const char* pName, int pSize, int pIndex) {

#if defined(C2_HOOKS_ENABLED)
    LoadPolyFont_original(pName, pSize, pIndex);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004643f0, LoadPolyFont, LoadPolyFont_original)

void (C2_HOOK_FASTCALL * InitPolyFonts_original)(void);
void C2_HOOK_FASTCALL InitPolyFonts(void) {
#if defined(C2_HOOKS_ENABLED)
    InitPolyFonts_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00463d40, InitPolyFonts, InitPolyFonts_original)
