#ifndef REC2_POLYFONT_H
#define REC2_POLYFONT_H

#include <brender/brender.h>

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gInitial_count_font_texture_pages);
C2_HOOK_VARIABLE_DECLARE(int, gSize_font_texture_pages);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gString_root_actor);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_actor*, gPolyFont_glyph_actors, 256);

void C2_HOOK_FASTCALL LoadPolyFont(const char* pName, int pSize, int pIndex);

void C2_HOOK_FASTCALL InitCarIcons(br_pixelmap* pMap);

void C2_HOOK_FASTCALL InitPolyFonts(void);

#endif //REC2_POLYFONT_H
