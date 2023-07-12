#ifndef REC2_TINTED_H
#define REC2_TINTED_H

#include "c2_hooks.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_DECLARE(int, gHud_tinted1);
C2_HOOK_VARIABLE_DECLARE(int, gHud_tinted2);
C2_HOOK_VARIABLE_DECLARE(int, gHud_tinted3);

void C2_HOOK_FASTCALL InitTintedPolys(void);

int C2_HOOK_FASTCALL CreateTintedPolyActor(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3);

br_model* C2_HOOK_FASTCALL CreateInterpolatedQuadModel(int x0, int y0, int width, int height, int nbX, int nbY);

void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex);

void C2_HOOK_FASTCALL FreeAllTintedPolyActors(void);

void C2_HOOK_FASTCALL UpdateTintedPolyActor(int pTintedIndex);

void C2_HOOK_FASTCALL MakeTintedVisible(int pTintedIndex);

void C2_HOOK_FASTCALL MakeTintedInvisible(int pTintedIndex);

void C2_HOOK_FASTCALL RenderTintedActors(void);

void C2_HOOK_FASTCALL SetTintedColour(int pTintedIndex, int pRed, int pGreen, int pBlue);

#endif //REC2_TINTED_H
