#ifndef REC2_FONT_H
#define REC2_FONT_H

#include "c2_hooks.h"

#include "brender/brender.h"
#include "rec2_types.h"

br_material* C2_HOOK_FASTCALL CreateFontCharacterMaterial(int textureIdx);

#endif //REC2_FONT_H
