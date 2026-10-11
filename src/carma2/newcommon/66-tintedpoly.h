#ifndef GUARD_66_TINTEDPOLY_H
#define GUARD_66_TINTEDPOLY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tTintedPoly gTintedPolys[10];
extern br_actor* gTintedPolyCamera;
extern br_colour* gTinted_poly_palette;
extern br_uint_32 gTintedColourMap_grn;
extern br_uint_32 gTintedColourMap_blu;
extern int gTinted_fire_phase;
extern int gTinted_fire_frame;
extern int gTinted_fire_save;
extern int gTinted_fire_cooling;
extern br_token_value gTinted_poly_tokens[9];
#define gTintedColourMap_red (gTinted_poly_tokens[6].t)
#define gDefaultOpacity_TintedPoly (gTinted_poly_tokens[6].v.i32)

extern void C2_HOOK_FASTCALL InitTintedPolyStuff(void);

extern br_model* C2_HOOK_FASTCALL BuildTintedPolyModel(int x0, int y0, int width, int height, int nbX, int nbY);

// BuildTintedPolyMaterial

// GetRangeOfValuesFromPixelmap

extern int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3);

extern void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex);

extern void C2_HOOK_FASTCALL FreeAllTintedPolyActor(void);

extern void C2_HOOK_FASTCALL UpdateTintedPolyByMaterial(int pIndex);

extern void C2_HOOK_FASTCALL UpdatePolyVertexColours(int pIndex);

extern void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex);

extern void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex);

extern void C2_HOOK_FASTCALL RenderTintedPolys(void);

extern void C2_HOOK_FASTCALL SetTintedPolyColour(int pTintedIndex, int pRed, int pGreen, int pBlue);

// SetTintedPolyRefMaterial

extern br_material* C2_HOOK_FASTCALL InWater(int pTintedIndex);

// ClearTheFlame is inlined in UpdateFirePoly.

// GetUniqueFileName is inlined in UpdateFirePoly.

extern void C2_HOOK_FASTCALL UpdateFirePoly(int pIndex);

extern void C2_HOOK_FASTCALL UpdatePulsePoly(int pIndex);

extern void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex);

extern void C2_HOOK_FASTCALL UpdateTintedPolys(void);

extern void C2_HOOK_FASTCALL SetTintedPolySize(int pTintedIndex, int x0, int y0, int width, int height);

extern int C2_HOOK_FASTCALL TintedPolyIsOn(int pIndex);

#endif // GUARD_66_TINTEDPOLY_H
