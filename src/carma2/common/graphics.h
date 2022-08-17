#ifndef REC2_GRAPHICS_H
#define REC2_GRAPHICS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tWobble_spec, gWobble_array, 5);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gCurrent_palette);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gRender_palette);


void C2_HOOK_FASTCALL ClearWobbles(void);

void C2_HOOK_FASTCALL InitWobbleStuff(void);

void C2_HOOK_FASTCALL FadePaletteDown(void);

void C2_HOOK_FASTCALL ClearEntireScreen(void);

void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount);

void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette);

void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name);

void C2_HOOK_FASTCALL DRPixelmapConvertRGB565ToRGB555IfNeeded(br_pixelmap* pixelmap, int pixelType);

br_model* C2_HOOK_FASTCALL CreateInterpolatedQuadModel(int x, int y, int width, int height, int nbX, int nbY);

void C2_HOOK_FASTCALL TintedPolyActor_SetColour(int index, int red, int green, int blue);

int C2_HOOK_FASTCALL CreateTintedPolyActor(int x, int y, int width, int height, int type, int arg1, int arg2, int arg3);

void C2_HOOK_FASTCALL AllocateCursorActors(void);

void C2_HOOK_FASTCALL StartMouseCursor(void);

void C2_HOOK_FASTCALL EndMouseCursor(void);

void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);

void C2_HOOK_FASTCALL DeallocateTransientBitmap(int pIndex);

void C2_HOOK_FASTCALL DeallocateAllTransientBitmaps(void);

void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel);

tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void);

void C2_HOOK_FASTCALL ToggleShadow(void);

void C2_HOOK_FASTCALL EnsurePaletteUp(void);

int C2_HOOK_FASTCALL DoMouseCursor(void);

void C2_HOOK_FASTCALL LoadFont(int pFont_ID);

#endif //REC2_GRAPHICS_H
