#ifndef REC2_TINTED_H
#define REC2_TINTED_H

#include "c2_hooks.h"

void C2_HOOK_FASTCALL InitTintedPolys(void);

void C2_HOOK_FASTCALL SetTintedColour(int pTintedIndex, int pRed, int pGreen, int pBlue);

void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex);

#endif //REC2_TINTED_H
