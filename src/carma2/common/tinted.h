#ifndef REC2_TINTED_H
#define REC2_TINTED_H

#include "c2_hooks.h"

#include <brender/brender.h>
#include <rec2_types.h>

C2_HOOK_VARIABLE_DECLARE(int, gHud_tinted1);
C2_HOOK_VARIABLE_DECLARE(int, gHud_tinted2);
C2_HOOK_VARIABLE_DECLARE(int, gHud_tinted3);
C2_HOOK_VARIABLE_DECLARE(int, gHud_tinted4);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tTintedPoly, gTintedPolys, 10);

C2_HOOK_VARIABLE_DECLARE(int, gINT_006a0440);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006a0444);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006a0448);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006a044c);

void C2_HOOK_FASTCALL InitTintedPolyStuff(void);

br_material* C2_HOOK_FASTCALL BuildTintedPolyMaterial(int pOpacity);

void C2_HOOK_FASTCALL GetRangeOfValuesFromPixelmap(br_pixelmap* pMap, br_uint_32* pDarkest, br_uint_32* pBrightest);

int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3);

br_model* C2_HOOK_FASTCALL BuildTintedPolyModel(int x0, int y0, int width, int height, int nbX, int nbY);

void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex);

void C2_HOOK_FASTCALL FreeAllTintedPolyActors(void);

void C2_HOOK_FASTCALL UpdateTintedPolyActor(int pTintedIndex);

void C2_HOOK_FASTCALL MakeTintedVisible(int pTintedIndex);

void C2_HOOK_FASTCALL MakeTintedInvisible(int pTintedIndex);

void C2_HOOK_FASTCALL RenderTintedActors(void);

void C2_HOOK_FASTCALL SetTintedColour(int pTintedIndex, int pRed, int pGreen, int pBlue);

void C2_HOOK_FASTCALL ResetTintedVertices(int pTintedIndex, int x0, int y0, int width, int height);

void C2_HOOK_FASTCALL SetTintedFromSpecialVolume(int pIndex, br_vector3* pPosition);

void C2_HOOK_FASTCALL UpdateTinted(int pIndex);

void C2_HOOK_FASTCALL TintedAnimateSawToothColor(int pIndex);

void C2_HOOK_FASTCALL FUN_004d86e0(int pIndex);

int C2_HOOK_FASTCALL IsTintedVisible(int pIndex);

#endif //REC2_TINTED_H
