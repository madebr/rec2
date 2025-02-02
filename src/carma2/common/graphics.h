#ifndef REC2_GRAPHICS_H
#define REC2_GRAPHICS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gPalette_changed);
C2_HOOK_VARIABLE_DECLARE(int, gPalette_munged);
C2_HOOK_VARIABLE_DECLARE(char*, gCurrent_palette_pixels);
C2_HOOK_VARIABLE_DECLARE(int, gFaded_palette);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gCurrent_splash);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tWobble_spec, gWobble_array, 5);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gCurrent_palette);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gRender_palette);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tDR_font, gFonts, 24);

C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gOrig_render_palette);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gFlic_palette);
C2_HOOK_VARIABLE_DECLARE(br_colour*, gScratch_pixels);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gScratch_palette);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gPalette_0074a604);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gPalette_0074a600);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_pixelmap*, gMini_map_glowing_line_palettes, 3);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gPalette_0074a66c);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gPalette_0074a5fc);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gPalette_0074a670);

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
C2_HOOK_VARIABLE_DECLARE(float, gCamera_to_horiz_angle);
C2_HOOK_VARIABLE_DECLARE(float, gYon_squared);
C2_HOOK_VARIABLE_DECLARE(int, gMirror_on__graphics);
C2_HOOK_VARIABLE_DECLARE(int, gShadow_clip_plane_count);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_vector3, gShadow_points, 8);
C2_HOOK_VARIABLE_DECLARE(br_matrix34, gIdentity34);
C2_HOOK_VARIABLE_DECLARE(int, gShadow_dim_amount);

void C2_HOOK_FASTCALL ClearWobbles(void);

void C2_HOOK_FASTCALL InitWobbleStuff(void);

void C2_HOOK_FASTCALL FadePaletteDown(void);

void C2_HOOK_FASTCALL BuildColourTable(br_pixelmap* pPalette);

void C2_HOOK_FASTCALL ClearEntireScreen(void);

void C2_HOOK_FASTCALL DRSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount);

void C2_HOOK_FASTCALL DRSetPalette3(br_pixelmap* pThe_palette, int pSet_current_palette);

void C2_HOOK_FASTCALL DRSetPalette2(br_pixelmap* pThe_palette, int pSet_current_palette);

void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette);

void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name);

void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pixelmap, int pixelType);

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

void C2_HOOK_FASTCALL ShadowMode(void);

void C2_HOOK_FASTCALL EnsurePaletteUp(void);

int C2_HOOK_FASTCALL DoMouseCursor(void);

void C2_HOOK_FASTCALL LoadFont(int pFont_ID);

void C2_HOOK_FASTCALL DisposeFont(int pFont_ID);

void C2_HOOK_FASTCALL InitWobbleStuff(void);

void C2_HOOK_FASTCALL InitDRFonts(void);

void C2_HOOK_FASTCALL UpdateMap(void);

void C2_HOOK_FASTCALL Init2DStuffForPolyFonts(void);

void C2_HOOK_FASTCALL InitShadow(void);

br_uint_32 C2_HOOK_CDECL SaveShadeTable(br_pixelmap* pTable, void* pArg);

void C2_HOOK_FASTCALL SaveShadeTables(void);

void C2_HOOK_FASTCALL InitPaletteAnimate(void);

void C2_HOOK_FASTCALL MungePalette(void);

void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight);

void C2_HOOK_FASTCALL PossibleLock(int pValue);

int C2_HOOK_FASTCALL PossibleUnlock(int pValue);

tMaterial_exception* C2_HOOK_FASTCALL FindExceptionInList(const char* pIdentifier, tMaterial_exception* pList);

void C2_HOOK_FASTCALL GlorifyMaterial(br_material** pMaterials, int pCount, tRendererShadingType pShading);

int C2_HOOK_FASTCALL FindBestMatchingPaletteIndex(br_colour pColour);

void C2_HOOK_FASTCALL DisableLights(void);

void C2_HOOK_FASTCALL EnableLights(void);

int C2_HOOK_FASTCALL SwitchToHiresMode(void);

int C2_HOOK_FASTCALL SwitchToLoresMode(void);

void C2_HOOK_FASTCALL AdjustRenderScreenSize(void);

void C2_HOOK_FASTCALL RemovePolyFontActors(void);

void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

void C2_HOOK_FASTCALL DrawNumberAt(br_pixelmap* pSrc, br_pixelmap* pDest, int pX, int pY, int pX_pitch, int pY_pitch, int pValue ,int pDigit_count, int pLeading_zeroes);

void C2_HOOK_FASTCALL DisposeSavedShadeTables(void);

void C2_HOOK_FASTCALL KillSplashScreen(void);

void C2_HOOK_FASTCALL InitTargetLock(void);

void C2_HOOK_FASTCALL SetIntegerMapRenders(void);

void C2_HOOK_FASTCALL VerifyPaletteBlackness(br_pixelmap* pPalette);

void C2_HOOK_FASTCALL ResetPalette(void);

void C2_HOOK_FASTCALL EnsureRenderPalette(void);

void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);

void C2_HOOK_FASTCALL ResetLollipopQueue(void);

void C2_HOOK_FASTCALL RevertPalette(void);

void C2_HOOK_FASTCALL MungeClipPlane(br_vector3* pLight, tCar_spec* pCar, br_vector3* p1, br_vector3* p2, br_vector3* pOffset);

br_scalar C2_HOOK_FASTCALL DistanceFromPlane(br_vector3* pPos, br_scalar arg2, br_scalar pA, br_scalar pB, br_scalar pC, br_scalar pD);

void C2_HOOK_FASTCALL TryThisEdge(tCar_spec* pCar, br_vector3* pLight, int pIndex_1, br_scalar pSign_1, int pIndex_2, br_scalar pSign_2, int pPoint_index_1, int pPoint_index_2, br_vector3* pOffset);

void C2_HOOK_FASTCALL ProcessShadow(tCar_spec* pCar, br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform, br_scalar pDistance_factor);

#endif //REC2_GRAPHICS_H
