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

// UpdateTintedPolyByMaterial

// UpdatePolyVertexColours

// TurnTintedPolyOn

// TurnTintedPolyOff

// RenderTintedPolys

extern void C2_HOOK_FASTCALL SetTintedPolyColour(int pTintedIndex, int pRed, int pGreen, int pBlue);

// SetTintedPolyRefMaterial

// InWater

// ClearTheFlame

// GetUniqueFileName

// UpdateFirePoly

// UpdatePulsePoly

// ProcessTintedPoly

// UpdateTintedPolys

// SetTintedPolySize

// TintedPolyIsOn

#endif // GUARD_66_TINTEDPOLY_H
