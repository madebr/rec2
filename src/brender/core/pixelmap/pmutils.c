#include "pmutils.h"

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
br_uint_16 C2_HOOK_CDECL GetSysQual(void) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        mov ax, ds;
        ret 0;
    }
#else
    __asm__(
        "mov %ds, %ax\n"
        "ret\n"
    );
#endif
#else
    return 0;
#endif
}
C2_NO_HOOK_FUNCTION(0x0053e682, GetSysQual)
