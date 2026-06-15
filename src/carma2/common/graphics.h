#ifndef REC2_GRAPHICS_H
#define REC2_GRAPHICS_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern int gPalette_changed;
extern int gPalette_munged;
extern char* gCurrent_palette_pixels;
extern int gFaded_palette;
extern br_pixelmap* gCurrent_splash;
extern tWobble_spec gWobble_array[5];
extern float gCosine_array[64];
extern br_pixelmap* gCurrent_palette;
extern br_pixelmap* gRender_palette;
extern tDR_font gFonts[24];

extern br_pixelmap* gOrig_render_palette;
extern br_pixelmap* gFlic_palette;
extern br_colour* gScratch_pixels;
extern br_pixelmap* gScratch_palette;
extern br_pixelmap* gPalette_0074a604;
extern br_pixelmap* gPalette_0074a600;
extern br_pixelmap* gMini_map_glowing_line_palettes[3];
extern br_pixelmap* gPalette_0074a66c;
extern br_pixelmap* gPalette_0074a5fc;
extern br_pixelmap* gPalette_0074a670;

extern int gGraf_data_index;

extern int gNoTransients;
extern tTransient_bm gTransient_bitmaps[50];
extern int gMouse_started;
extern int gNoTransients;
extern int gNext_transient;

extern float gMap_render_x;
extern float gMap_render_y;
extern int gMap_render_x_i;
extern int gMap_render_y_i;
extern int gMap_render_width_i;
extern int gMap_render_height_i;
extern float gMap_render_width;
extern float gMap_render_height;

extern int gHeadup_map_x;
extern int gHeadup_map_y;
extern int gHeadup_map_w;
extern int gHeadup_map_h;
extern int gHeadup_detail_level;

extern int gMap_trans;
extern int gCamera_type;
extern int gAR_camera_type;
extern int gMini_map_visible;

extern tU32 gLast_palette_change;
extern int gPalette_index;

extern int gWidth;
extern int gHeight;
extern int gX_offset;
extern int gY_offset;
extern int gCount_polyfont_glyph_actors;
extern br_actor* gPolyfont_glyph_actors[256];
extern tSaved_table gSaved_shade_tables[100];
extern int gSaved_table_count;
extern br_pixelmap* gCurrent_splash;
extern int gTarget_lock_enabled;
extern br_pixelmap* gLit_op_stat;

extern int gCursor_tinted_top;
extern int gCursor_tinted_left;
extern int gCursor_tinted_bott;
extern int gCursor_tinted_right;
extern int gCursor_tinted_cent;
extern int gCursor_line_width;

extern int gTransient_bitmap_index;
extern tMouse_coord gMouse_last_pos;
extern int gMouse_in_use;
extern int gCurrent_cursor_index;
extern float gCamera_to_horiz_angle;
extern float gYon_squared;
extern int gMirror_on__graphics;
extern int gShadow_clip_plane_count;
extern br_vector3 gShadow_points[8];
extern br_matrix34 gIdentity34;
extern int gShadow_dim_amount;
extern tOppo_status_messages gOppo_status_messages[9];
extern int gInitMap_done;
extern float gHeadup_map_x_float;
extern float gHeadup_map_y_float;
extern float gHeadup_map_w_float;
extern float gHeadup_map_h_float;
extern tU32 gUINT_0074ab88;
extern tU32 gUINT_0074ab8c;
extern int gINT_0074ab94;
extern int gINT_0074abec;
extern br_pixelmap* gMap_overlay;
extern br_pixelmap* gMini_map;
extern br_pixelmap* gCheckpoint_numbers;
extern br_pixelmap* gSmashy_dot;
extern br_pixelmap* gAlt_meter;
extern int gCheckpoint_digit_height;
extern int gCheckpoint_digit_center_x;
extern tU32 gCheckpoint_digit_center_y;
extern int gCheckpoint_digit_height;
extern int gMini_map_x;
extern int gMini_map_y;
extern int gINT_0074abd4;
extern int gINT_0074abd0;
extern int gINT_00655f04;
extern int gINT_00655f08;
extern float gFLOAT_0068d8a0;
extern float gFLOAT_0068d8a4;
extern float gFLOAT_0074ab9c;
extern float gFLOAT_0074abb8;
extern float gMini_map_arrow_z;
extern int gNumber_of_lollipops;
extern int gScreen_wobble_x;
extern int gScreen_wobble_y;
extern int gHud_actor_storage_size;
extern br_matrix34 gSheer_mat;
extern int gAR_fudge_headups;
extern const int gArrows[2][4][60];
extern br_matrix34 gCar_in_map_space;
extern int gHeadup_map_half_width;
extern int gHeadup_map_half_height;
extern int gINT_0068c858;
extern int gINT_0068d890;
extern float gFLOAT_0068d6e0;
extern float gFLOAT_0074ab90;
extern br_vector2 gVector2_0068d6d8;
extern const int gMini_map_glowing_line_animation_indices[5];
extern br_matrix34 gSheer_mat;
extern int gAR_fudge_headups;
extern tOpponent_Status gPrevious_opponent_status;
extern tU32 gTime_oppobar_target_wasted;
extern float gPrevious_rear_yon;

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

void C2_HOOK_FASTCALL Darken(tU8* pPtr, unsigned int pDarken_amount);

void C2_HOOK_FASTCALL SetFadedPalette(int pDegree);

void C2_HOOK_FASTCALL FadePaletteUp(void);

void C2_HOOK_FASTCALL EnsurePaletteUp(void);

int C2_HOOK_FASTCALL DoMouseCursor(void);

void C2_HOOK_FASTCALL LoadFont(int pFont_ID);

void C2_HOOK_FASTCALL DisposeFont(int pFont_ID);

void C2_HOOK_FASTCALL InitWobbleStuff(void);

void C2_HOOK_FASTCALL InitDRFonts(void);

void C2_HOOK_FASTCALL ReadMoodMessages(void);

void C2_HOOK_FASTCALL InitMap(void);

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

void C2_HOOK_FASTCALL CleanPolyFontDanglers(void);

void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);

void C2_HOOK_FASTCALL DrawNumberAt(br_pixelmap* pSrc, br_pixelmap* pDest, int pX, int pY, int pX_pitch, int pY_pitch, int pValue ,int pDigit_count, int pLeading_zeroes);

void C2_HOOK_FASTCALL DisposeSavedShadeTables(void);

void C2_HOOK_FASTCALL KillSplashScreen(void);

void C2_HOOK_FASTCALL InitTargetLock(void);

void C2_HOOK_FASTCALL SetIntegerMapRenders(void);

void C2_HOOK_FASTCALL NobbleNonzeroBlacks(br_pixelmap* pPalette);

void C2_HOOK_FASTCALL ResetPalette(void);

void C2_HOOK_FASTCALL EnsureRenderPalette(void);

void C2_HOOK_FASTCALL CleanPolyFontDanglers(void);

void C2_HOOK_FASTCALL StartRenderingHeadups(void);

void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);

void C2_HOOK_FASTCALL ResetLollipopQueue(void);

void C2_HOOK_FASTCALL RevertPalette(void);

void C2_HOOK_FASTCALL MungeClipPlane(br_vector3* pLight, tCar_spec* pCar, br_vector3* p1, br_vector3* p2, br_vector3* pOffset);

br_scalar C2_HOOK_FASTCALL DistanceFromPlane(br_vector3* pPos, br_scalar arg2, br_scalar pA, br_scalar pB, br_scalar pC, br_scalar pD);

void C2_HOOK_FASTCALL DoTestHeadup(void);

void C2_HOOK_FASTCALL TryThisEdge(tCar_spec* pCar, br_vector3* pLight, int pIndex_1, br_scalar pSign_1, int pIndex_2, br_scalar pSign_2, int pPoint_index_1, int pPoint_index_2, br_vector3* pOffset);

void C2_HOOK_FASTCALL ProcessShadow(tCar_spec* pCar, br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform, br_scalar pDistance_factor);

void C2_HOOK_FASTCALL InitNearestCar(void);

void C2_HOOK_FASTCALL StartMap(void);

void C2_HOOK_FASTCALL CopyMapToScreen(void);

void C2_HOOK_FASTCALL MapStuffBeforeRender(void);

void C2_HOOK_FASTCALL FoxyStuff(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth);

int C2_HOOK_FASTCALL ConditionallyFillWithSky(br_pixelmap* pPixelmap);

void C2_HOOK_FASTCALL FixificateClipulatingPlaneyThings(br_actor* pCamera);

void C2_HOOK_FASTCALL ProcessNonTrackActors(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);

void C2_HOOK_FASTCALL CancelificateClipulatingPlaneyThings(void);

void C2_HOOK_FASTCALL DoARenderPass(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth, float pYon_factor, int pShadows, int pEffects);

void C2_HOOK_FASTCALL DoACompleteRenderPass(int pMirror, br_matrix34* pCamera_to_world, br_actor* pCamera, br_pixelmap* pScreen, br_pixelmap* pDepth);

void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDestn, int pX1, int pY1, int pX2, int pY2, int pColour);

void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev);

void C2_HOOK_FASTCALL CalcMapCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);

void C2_HOOK_FASTCALL DrawCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);

void C2_HOOK_FASTCALL CalcMapCheckpoint2(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);

void C2_HOOK_FASTCALL DrawMapSmallBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3 *pPos, int pColour);

br_uint_32 C2_HOOK_FASTCALL OppositeColour(br_uint_32 pColour);

void C2_HOOK_FASTCALL DrawArrow(br_pixelmap* pScreen, br_uint_32 pArrow_index, const br_vector3 *pPos, br_uint_32 pColour);

void C2_HOOK_FASTCALL DrawMapBlip(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, br_matrix34* pTrans, const br_vector3* pPos, br_uint_32 pColour);

void C2_HOOK_FASTCALL DRPixelmapBlendedLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, br_pixelmap* pPalette);

void C2_HOOK_FASTCALL DR8BitFancyDrawLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period);

void C2_HOOK_FASTCALL FancyDrawLine(br_pixelmap *pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period);

void C2_HOOK_FASTCALL DR16BitPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height);

void C2_HOOK_FASTCALL DR8BitPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans);

void C2_HOOK_FASTCALL DRPixelmapRotatedAndFeatheredCopy(br_matrix23* pMat, br_pixelmap* pDest, tS16 pDest_x, tS16 pDest_y, br_pixelmap* pSrc, tS16 pSrc_x, tS16 pSrc_y, tS16 pSrc_width, tS16 pSrc_height, int pTrans);

void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack8Bit(br_pixelmap* pDest, br_pixelmap* pSrc);

void C2_HOOK_FASTCALL DRPixelmapCopyMapBlack(br_pixelmap* pDest, br_pixelmap* pSrc);

void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform);

#endif //REC2_GRAPHICS_H
