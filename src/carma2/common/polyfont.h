#ifndef REC2_POLYFONT_H
#define REC2_POLYFONT_H

#include "c2_hooks.h"

void C2_HOOK_FASTCALL LoadPolyFont(const char* pName, int pSize, int pIndex);

void C2_HOOK_FASTCALL InitPolyFonts(void);

#endif //REC2_POLYFONT_H
