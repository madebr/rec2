#include "fontptrs.h"

#include "font3x5f.h"
#include "font4x6p.h"
#include "font7x9p.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_font*, BrFontFixed3x5, 0x0066d7b8, &br_font3x5f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_font*, BrFontProp4x6, 0x0066d7bc, &br_font4x6p);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_font*, BrFontProp7x9, 0x0066d7c0, &br_font7x9p);
