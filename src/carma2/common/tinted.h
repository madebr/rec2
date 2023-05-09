#ifndef REC2_TINTED_H
#define REC2_TINTED_H

#include "c2_hooks.h"

#include <brender/brender.h>

void C2_HOOK_FASTCALL InitTintedPolys(void);

void C2_HOOK_FASTCALL SetTintedColour(int pTintedIndex, int pRed, int pGreen, int pBlue);

br_model* C2_HOOK_FASTCALL CreateInterpolatedQuadModel(int x0, int y0, int width, int height, int nbX, int nbY);

void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex);

#endif //REC2_TINTED_H
