#ifndef REC2_DEVSETUP_H
#define REC2_DEVSETUP_H

#include <brender/brender.h>

#include "c2_hooks.h"

br_pixelmap* C2_HOOK_CDECL BrDevLastBeginQuery(void);

void C2_HOOK_CDECL BrDevLastBeginSet(br_pixelmap* pm);

br_error C2_HOOK_CDECL BrDevBeginVar(br_pixelmap** ppm, char* setup_string, ...);

br_error C2_HOOK_CDECL BrDevBeginTV(br_pixelmap** ppm, char* setup_string, br_token_value* setup_tv);

br_error C2_HOOK_CDECL BrDevBegin(br_pixelmap** ppm, char* setup_string);

#endif //REC2_DEVSETUP_H
