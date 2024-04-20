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
C2_HOOK_VARIABLE_DECLARE_ARRAY(tTransient_bm, gTransient_bitmaps, 50);
C2_HOOK_VARIABLE_DECLARE(int, gMouse_started);
C2_HOOK_VARIABLE_DECLARE(int, gNoTransients);
C2_HOOK_VARIABLE_DECLARE(int, gNext_transient);

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

C2_HOOK_VARIABLE_DECLARE(tU32, gLast_palette_change);
C2_HOOK_VARIABLE_DECLARE(int, gPalette_index);

C2_HOOK_VARIABLE_DECLARE(int, gWidth);
C2_HOOK_VARIABLE_DECLARE(int, gHeight);
C2_HOOK_VARIABLE_DECLARE(int, gX_offset);
C2_HOOK_VARIABLE_DECLARE(int, gY_offset);
C2_HOOK_VARIABLE_DECLARE(int, gCount_polyfont_glyph_actors);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_actor*, gPolyfont_glyph_actors, 256);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSaved_table, gSaved_shade_tables, 100);
C2_HOOK_VARIABLE_DECLARE(int, gSaved_table_count);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gCurrent_splash);
C2_HOOK_VARIABLE_DECLARE(int, gTarget_lock_enabled);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gLit_op_stat);

C2_HOOK_VARIABLE_DECLARE(int, gCursor_tinted_top);
C2_HOOK_VARIABLE_DECLARE(int, gCursor_tinted_left);
C2_HOOK_VARIABLE_DECLARE(int, gCursor_tinted_bott);
C2_HOOK_VARIABLE_DECLARE(int, gCursor_tinted_right);
C2_HOOK_VARIABLE_DECLARE(int, gCursor_tinted_cent);
C2_HOOK_VARIABLE_DECLARE(int, gCursor_line_width);

C2_HOOK_VARIABLE_DECLARE(int, gTransient_bitmap_index);
C2_HOOK_VARIABLE_DECLARE(tMouse_coord, gMouse_last_pos);
C2_HOOK_VARIABLE_DECLARE(int, gMouse_in_use);
C2_HOOK_VARIABLE_DECLARE(int, gCurrent_cursor_index);

void C2_HOOK_FASTCALL ClearWobbles(void);

void C2_HOOK_FASTCALL InitWobbleStuff(void);

void C2_HOOK_FASTCALL FadePaletteDown(void);

void C2_HOOK_FASTCALL BuildColourTable(br_pixelmap* pPalette);

void C2_HOOK_FASTCALL ClearEntireScreen(void);

void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount);

void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette);

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

void C2_HOOK_FASTCALL DisposeFont(int pFont_ID);

void C2_HOOK_FASTCALL InitWobbleStuff(void);

void C2_HOOK_FASTCALL InitDRFonts(void);

void C2_HOOK_FASTCALL UpdateMap(void);

void C2_HOOK_FASTCALL InitHUDActor(void);

void C2_HOOK_FASTCALL InitShadows(void);

br_uint_32 C2_HOOK_CDECL SaveShadeTable(br_pixelmap* pTable, void* pArg);

void C2_HOOK_FASTCALL SaveShadeTables(void);

void C2_HOOK_FASTCALL InitPaletteAnimate(void);

void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight);

void C2_HOOK_FASTCALL LockBackScreen(int pValue);

int C2_HOOK_FASTCALL UnlockBackScreen(int pValue);

void C2_HOOK_FASTCALL AdaptMaterialsForRenderer(br_material** pMaterials, int pCount, tRendererShadingType pShading);

int C2_HOOK_FASTCALL FindBestMatchingPaletteIndex(br_colour pColour);

void C2_HOOK_FASTCALL DisableLights(void);

void C2_HOOK_FASTCALL EnableLights(void);

int C2_HOOK_FASTCALL SwitchToRealResolution(void);

int C2_HOOK_FASTCALL SwitchToLoresMode(void);

void C2_HOOK_FASTCALL AdjustRenderScreenSize(void);

void C2_HOOK_FASTCALL RemovePolyFontActors(void);

void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

void C2_HOOK_FASTCALL DrawNumberAt(br_pixelmap* pSrc, br_pixelmap* pDest, int pX, int pY, int pX_pitch, int pY_pitch, int pValue ,int pDigit_count, int pLeading_zeroes);

void C2_HOOK_FASTCALL DisposeSavedShadeTables(void);

void C2_HOOK_FASTCALL KillSplashScreen(void);

void C2_HOOK_FASTCALL ResetTargetLock(void);

void C2_HOOK_FASTCALL SetIntegerMapRenders(void);

#endif //REC2_GRAPHICS_H
