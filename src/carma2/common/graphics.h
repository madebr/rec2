#ifndef REC2_GRAPHICS_H
#define REC2_GRAPHICS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tWobble_spec, gWobble_array, 5);


void C2_HOOK_FASTCALL ClearWobbles(void);

void C2_HOOK_FASTCALL InitWobbleStuff(void);

#endif //REC2_GRAPHICS_H
