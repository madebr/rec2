#include "brstdlib.h"

#include "c2_hooks.h"

#include <stdio.h>
#include <string.h>

void BrAbort(void) {
#if defined(C2_HOOKS_ENABLED)
    void (C2_HOOK_CDECL * abort_original)(void) = (void(C2_HOOK_CDECL *)(void))0x00578aa0;
    abort_original();
#else
    abort();
#endif
}
C2_HOOK_FUNCTION(0x0053f230, BrAbort);

char* BrStrCpy(char* s1, const char* s2) {
    return strcpy(s1, s2);
}
C2_HOOK_FUNCTION(0x0053f160, BrStrCpy);

int BrVSprintf(char* buf, const char* fmt, va_list args) {
    return vsprintf(buf, fmt, args);
}
C2_HOOK_FUNCTION(0x0053f390, BrVSprintf)