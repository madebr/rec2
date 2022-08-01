#include "devsetup.h"

#include <stdarg.h>
#include <stdio.h>

#include "c2_stdio.h"

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, last_begin_screen, 0x006ad9ac);

br_pixelmap* C2_HOOK_CDECL BrDevLastBeginQuery(void) {

    return C2V(last_begin_screen);
}
C2_HOOK_FUNCTION(0x00528d50, BrDevLastBeginQuery)

void C2_HOOK_CDECL BrDevLastBeginSet(br_pixelmap* pm) {

    C2V(last_begin_screen) = pm;
}
C2_HOOK_FUNCTION(0x00528d60, BrDevLastBeginSet)

br_error C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, char* setup_string, ...) {
    va_list vl;
    br_uint_32 i;
    br_token_value tv[64];

    C2_HOOK_START();
    va_start(vl, setup_string);
    for (i = 0; ; i++) {
        if (i == BR_ASIZE(tv)) {
            // FIXME: enable this back!
//            BrFailure("Too many token value pairs");
        }
        tv[i].t = va_arg(vl, br_token);
        if (tv[i].t == 0) {
            break;
        }
        tv[i].v = va_arg(vl, br_value);

        // rec2 hook: disable full screen
        if (tv[i].t == BRT_WINDOW_FULLSCREEN_B) {
//            tv[i].v.i32 = 0;
        }
    }
    va_end(vl);
    tv[i].t = 0;
    tv[i].v.p = NULL;
    br_error res = BrDevBeginTV(ppm, setup_string, tv);
    C2_HOOK_FINISH();
    return res;
}
C2_HOOK_FUNCTION(0x00528d70, BrDevBeginVar)

br_error C2_HOOK_CDECL BrDevBegin(br_pixelmap** ppm, char* setup_string) {

    return BrDevBeginTV(ppm, setup_string, NULL);
}
C2_HOOK_FUNCTION(0x00528df0, BrDevBegin)

br_error (C2_HOOK_CDECL * BrDevBeginTV_original)(br_pixelmap** ppm, char* setup_string, br_token_value* setup_tv);
br_error C2_HOOK_CDECL BrDevBeginTV(br_pixelmap** ppm, char* setup_string, br_token_value* setup_tv) {
#if defined(C2_HOOKS_ENABLED)
    return BrDevBeginTV_original(ppm, setup_string, setup_tv);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528e10, BrDevBeginTV, BrDevBeginTV_original)
