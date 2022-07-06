#include "loading.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include "c2_hooks.h"

void C2_HOOK_FASTCALL StripCRNL(char* line) {
    char* loc;

    loc = c2_strchr(line, '\n');
    if (loc != NULL) {
        *loc = '\0';
    }

    loc = c2_strchr(line, '\r');
    if (loc != NULL) {
        *loc = '\0';
    }
}
C2_HOOK_FUNCTION(0x00490690, StripCRNL)

DRFILE* (C2_HOOK_FASTCALL * DRfopen_original)(const char* pFilename, const char* pMode);
DRFILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_STARTF("(%s %s)", pFilename, pMode);
    DRFILE* res = DRfopen_original(pFilename, pMode);
    C2_HOOK_FINISH();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4780, DRfopen, DRfopen_original)

char* (C2_HOOK_FASTCALL * DRreadline_original)(DRFILE* pFile, char* pBuffer);
char* C2_HOOK_FASTCALL DRreadline(DRFILE* pFile, char* pBuffer) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    char* res = DRreadline_original(pFile, pBuffer);
    C2_HOOK_FINISH();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00490f30, DRreadline, DRreadline_original)

void (C2_HOOK_FASTCALL * DRfclose_original)(DRFILE* pFile);
void C2_HOOK_FASTCALL DRfclose(DRFILE* pFile) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    DRfclose_original(pFile);
    C2_HOOK_FINISH();
#else
#error "not implemented"
#endif
}
