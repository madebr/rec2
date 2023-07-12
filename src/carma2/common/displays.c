#include "displays.h"

int (C2_HOOK_FASTCALL * DRTextWidth_original)(const tDR_font* pFont, const char* pText);
int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText) {

#if defined(C2_HOOKS_ENABLED)
    return DRTextWidth_original(pFont, pText);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00465d50, DRTextWidth, DRTextWidth_original)

void (C2_HOOK_FASTCALL *TransDRPixelmapText_original)(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
void C2_HOOK_FASTCALL TransDRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge) {

#if defined(C2_HOOKS_ENABLED)
    TransDRPixelmapText_original(pPixelmap, pX, pY, pFont, pText, pRight_edge);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00465a70, TransDRPixelmapText, TransDRPixelmapText_original)

void (C2_HOOK_FASTCALL * InitHeadups_original)(void);
void C2_HOOK_FASTCALL InitHeadups(void) {

#if defined(C2_HOOKS_ENABLED)
    InitHeadups_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00449090, InitHeadups, InitHeadups_original)
