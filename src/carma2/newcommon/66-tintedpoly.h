#ifndef GUARD_66_TINTEDPOLY_H
#define GUARD_66_TINTEDPOLY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tTintedPoly gTintedPolys[10];
extern br_actor* gTintedPolyCamera;

extern void C2_HOOK_FASTCALL InitTintedPolyStuff(void);

extern br_model* C2_HOOK_FASTCALL BuildTintedPolyModel(int x0, int y0, int width, int height, int nbX, int nbY);

// BuildTintedPolyMaterial

// GetRangeOfValuesFromPixelmap

extern int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3);

// static void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex);

extern void C2_HOOK_FASTCALL FreeAllTintedPolyActor(void);

// UpdateTintedPolyByMaterial

// UpdatePolyVertexColours

extern void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex);

extern void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex);

extern void C2_HOOK_FASTCALL RenderTintedPolys(void);

extern void C2_HOOK_FASTCALL SetTintedPolyColour(int pTintedIndex, int pRed, int pGreen, int pBlue);

// SetTintedPolyRefMaterial

extern br_material* C2_HOOK_FASTCALL InWater(int pTintedIndex);

// ClearTheFlame

// GetUniqueFileName

// UpdateFirePoly

// UpdatePulsePoly

// ProcessTintedPoly

// UpdateTintedPolys

extern void C2_HOOK_FASTCALL SetTintedPolySize(int pTintedIndex, int x0, int y0, int width, int height);

extern int C2_HOOK_FASTCALL TintedPolyIsOn(int pIndex);

#endif // GUARD_66_TINTEDPOLY_H
