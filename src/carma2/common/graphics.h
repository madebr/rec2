#ifndef REC2_GRAPHICS_H
#define REC2_GRAPHICS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tWobble_spec, gWobble_array, 5);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gCurrent_palette);

void C2_HOOK_FASTCALL FadePaletteDown(void);

void C2_HOOK_FASTCALL ClearEntireScreen(void);

void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette);

#endif //REC2_GRAPHICS_H
