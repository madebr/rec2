#ifndef REC2_DEVSETUP_H
#define REC2_DEVSETUP_H

#include <brender/brender.h>

#include "c2_hooks.h"

br_error C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, char* setup_string, ...);
br_error C2_HOOK_CDECL BrDevBeginTV(br_pixelmap** ppm, char* setup_string, br_token_value* setup_tv);

#endif //REC2_DEVSETUP_H
