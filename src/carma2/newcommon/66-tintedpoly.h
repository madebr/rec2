/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_66_TINTEDPOLY_H
#define GUARD_66_TINTEDPOLY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tTintedPoly gTintedPolys[10];
extern br_actor* gTintedPolyCamera;
extern const char* gForm_non_humanoid_names[1];

extern void C2_HOOK_FASTCALL InitTintedPolyStuff(void);

extern br_model* C2_HOOK_FASTCALL BuildTintedPolyModel(int x0, int y0, int width, int height, int nbX, int nbY);

// static br_material* C2_HOOK_FASTCALL BuildTintedPolyMaterial(int pOpacity);

// void C2_HOOK_FASTCALL GetRangeOfValuesFromPixelmap(br_pixelmap* pMap, int* pDarkest, int* pBrightest);

extern int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3);

// static void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex);

extern void C2_HOOK_FASTCALL FreeAllTintedPolyActor(void);

extern void C2_HOOK_FASTCALL UpdateTintedPalette(int pIndex);

extern void C2_HOOK_FASTCALL UpdateTintedPolyByMaterial(int pIndex);

extern void C2_HOOK_FASTCALL UpdatePolyVertexColours(int pIndex);

extern void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex);

extern void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex);

extern void C2_HOOK_FASTCALL RenderTintedPolys(void);

extern void C2_HOOK_FASTCALL SetTintedPolyColour(int pTintedIndex, int pRed, int pGreen, int pBlue);

extern void C2_HOOK_FASTCALL SetTintedPolyRefMaterial(int pTintedIndex, br_vector3* pPosition);

extern br_material* C2_HOOK_FASTCALL InWater(int pTintedIndex);

extern void C2_HOOK_FASTCALL ClearTheFlame(int pIndex);

extern void C2_HOOK_FASTCALL GetUniqueFileName(const char *pDirectory, const char *pExtension, char *pDest);

extern void C2_HOOK_FASTCALL UpdateFirePoly(int pIndex);


extern void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex);

extern void C2_HOOK_FASTCALL UpdateTintedPolys(void);

extern void C2_HOOK_FASTCALL SetTintedPolySize(int pTintedIndex, int x0, int y0, int width, int height);

extern int C2_HOOK_FASTCALL TintedPolyIsOn(int pIndex);

#endif // GUARD_66_TINTEDPOLY_H
