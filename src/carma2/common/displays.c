#include "displays.h"

#include "platform.h"

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tQueued_headup, gQueued_headups, 4, 0x0067f890);
C2_HOOK_VARIABLE_IMPLEMENT(int, gQueued_headup_count, 0x0067f888);

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

int (C2_HOOK_FASTCALL * MungeHeadupWidth_original)(tHeadup* pHeadup);
int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup) {

    C2_HOOK_BUG_ON(sizeof(tHeadup) != 356);

#if defined(C2_HOOKS_ENABLED)
    return MungeHeadupWidth_original(pHeadup);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044a220, MungeHeadupWidth, MungeHeadupWidth_original)

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void) {

    C2_HOOK_BUG_ON(sizeof(tQueued_headup) != 0x10c);

    C2V(gQueued_headup_count) -= 1;
    c2_memmove(&C2V(gQueued_headups)[0], &C2V(gQueued_headups)[1], C2V(gQueued_headup_count) * sizeof(tQueued_headup));
}

void C2_HOOK_FASTCALL ClearQueuedHeadups(void) {

    while (C2V(gQueued_headup_count) != 0) {
        KillOldestQueuedHeadup();
    }
}
C2_HOOK_FUNCTION(0x004497b0, ClearQueuedHeadups)
