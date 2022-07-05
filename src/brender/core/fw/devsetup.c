#include "devsetup.h"

#include <stdarg.h>

br_error C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, char* setup_string, ...) {
    va_list vl;
    br_uint_32 i;
    br_uint_32 n;
    br_token_value tv[64];

    va_start(vl, setup_string);
    for (i = 0; ; i++) {
        if (i == BR_ASIZE(tv)) {
            BrFailure("Too many token value pairs");
        }
        tv[i].t = va_arg(vl, br_token);
        if (tv[i].t  == 0) {
            break;
        }
        tv[i].v = va_arg(vl, br_value);

        // rec2 hook: disable full screen
        if (tv[i].t == BRT_WINDOW_FULLSCREEN_B) {
            tv[i].v.b = 0;
        }
    }
    va_end(vl);
    tv[i].t = 0;
    tv[i].v.p = NULL;
    return BrDevBeginTV(ppm, setup_string, tv);
}
C2_HOOK_FUNCTION(0x00528d70, BrDevBeginVar)

br_error (C2_HOOK_CDECL * BrDevBeginTV_original)(br_pixelmap** ppm, char* setup_string, br_token_value* setup_tv);
br_error C2_HOOK_CDECL BrDevBeginTV(br_pixelmap** ppm, char* setup_string, br_token_value* setup_tv) {
    return BrDevBeginTV_original(ppm, setup_string, setup_tv);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528e10, BrDevBeginTV, BrDevBeginTV_original)
