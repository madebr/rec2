#ifndef REC2_GRAPHICS_H
#define REC2_GRAPHICS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tWobble_spec, gWobble_array, 5);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gCurrent_palette);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gRender_palette);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tDR_font, gFonts, 24);

C2_HOOK_VARIABLE_DECLARE(int, gGraf_data_index);

C2_HOOK_VARIABLE_DECLARE(int, gNoTransients);

C2_HOOK_VARIABLE_DECLARE(float, gMap_render_x);
C2_HOOK_VARIABLE_DECLARE(float, gMap_render_y);
C2_HOOK_VARIABLE_DECLARE(float, gMap_render_width);
C2_HOOK_VARIABLE_DECLARE(float, gMap_render_height);

C2_HOOK_VARIABLE_DECLARE(int, gHeadup_map_x);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_map_y);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_map_w);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_map_h);
C2_HOOK_VARIABLE_DECLARE(int, gHeadup_detail_level);

C2_HOOK_VARIABLE_DECLARE(int, gMap_trans);
C2_HOOK_VARIABLE_DECLARE(int, gCamera_type);
C2_HOOK_VARIABLE_DECLARE(int, gAR_camera_type);
C2_HOOK_VARIABLE_DECLARE(int, gMini_map_visible);

void C2_HOOK_FASTCALL FadePaletteDown(void);

void C2_HOOK_FASTCALL BuildColourTable(br_pixelmap* pPalette);

void C2_HOOK_FASTCALL ClearEntireScreen(void);

void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount);

void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette);

void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name);

void C2_HOOK_FASTCALL DRPixelmapConvertRGB565ToRGB555IfNeeded(br_pixelmap* pixelmap, int pixelType);

void C2_HOOK_FASTCALL AllocateCursorActors(void);

void C2_HOOK_FASTCALL StartMouseCursor(void);

void C2_HOOK_FASTCALL EndMouseCursor(void);

void C2_HOOK_FASTCALL InitTransientBitmaps(void);

void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);

void C2_HOOK_FASTCALL DeallocateTransientBitmap(int pIndex);

void C2_HOOK_FASTCALL DeallocateAllTransientBitmaps(void);

void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel);

tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void);

void C2_HOOK_FASTCALL ToggleShadow(void);

void C2_HOOK_FASTCALL EnsurePaletteUp(void);

int C2_HOOK_FASTCALL DoMouseCursor(void);

void C2_HOOK_FASTCALL LoadFont(int pFont_ID);

void C2_HOOK_FASTCALL InitWobbleStuff(void);

void C2_HOOK_FASTCALL InitDRFonts(void);

void C2_HOOK_FASTCALL UpdateMinimap(void);

void C2_HOOK_FASTCALL InitHUDActor(void);

void C2_HOOK_FASTCALL InitShadows(void);

void C2_HOOK_FASTCALL InitPaletteAnimate(void);

void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight);

void C2_HOOK_FASTCALL LockBackScreen(int pValue);

int C2_HOOK_FASTCALL UnlockBackScreen(int pValue);

#endif //REC2_GRAPHICS_H
