#include "18-graphics2.h"

#include "41-utility.h"
#include "52-errors.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_types.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x0074ca28
int gNoTransients;

// GLOBAL: CARMA2_HW 0x0074a674
br_pixelmap* gRender_palette;

// GLOBAL: CARMA2_HW 0x0079ec14
int gDim_amount;

// GLOBAL: CARMA2_HW 0x0074a680
char* gCurrent_palette_pixels;

// GLOBAL: CARMA2_HW 0x0074cf04
int gPalette_changed;

// GLOBAL: CARMA2_HW 0x0074a678
br_pixelmap* gCurrent_palette;

// GLOBAL: CARMA2_HW 0x006923ac
br_pixelmap* gOrig_render_palette;

// GLOBAL: CARMA2_HW 0x0074a67c
br_pixelmap* gFlic_palette;

// GLOBAL: CARMA2_HW 0x006923c8
int gFaded_palette;

// GLOBAL: CARMA2_HW 0x006923c0
int gPalette_munged;

// GLOBAL: CARMA2_HW 0x006923b8
br_colour* gScratch_pixels;

// GLOBAL: CARMA2_HW 0x006923a8
br_pixelmap* gScratch_palette;

// GLOBAL: CARMA2_HW 0x0074a660
br_pixelmap* gMini_map_glowing_line_palettes[3];

// GLOBAL: CARMA2_HW 0x0074a604
br_pixelmap* gPalette_0074a604;

// GLOBAL: CARMA2_HW 0x0074a600
br_pixelmap* gPalette_0074a600;

// GLOBAL: CARMA2_HW 0x0074a66c
br_pixelmap* gPalette_0074a66c;

// GLOBAL: CARMA2_HW 0x0074a5fc
br_pixelmap* gPalette_0074a5fc;

// GLOBAL: CARMA2_HW 0x0074a670
br_pixelmap* gPalette_0074a670;

// DRSetPaletteEntries

// DRSetPalette3

// DRSetPalette2

// DRSetPalette

// FUNCTION: CARMA2_HW 0x004b5090
void C2_HOOK_FASTCALL InitializePalettes(void) {
    br_pixelmap* render_palette;

    gCurrent_palette_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    gPalette_changed = 0;
    gCurrent_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gCurrent_palette_pixels, 0);
    gRender_palette = BrTableFind("DRRENDER.PAL");
    if (gRender_palette == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    NobbleNonzeroBlacks(gRender_palette);
    gOrig_render_palette = BrPixelmapAllocateSub(gRender_palette, 0, 0, gRender_palette->width, gRender_palette->height);
    gOrig_render_palette->pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    memcpy(gOrig_render_palette->pixels, gRender_palette->pixels, 256 * sizeof(br_uint_32));
    gFlic_palette = BrTableFind("DRACEFLC.PAL");
    if (gFlic_palette == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    render_palette = gRender_palette;
    ((br_uint_32*)render_palette->pixels)[0] = 0;
    memcpy(gCurrent_palette_pixels, render_palette->pixels, 256 * sizeof(br_uint_32));
    gPalette_changed = 0;
    if (!gFaded_palette) {
        PDSetPalette(render_palette);
    }
    gPalette_munged |= render_palette != gRender_palette;
    gScratch_pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    gScratch_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gScratch_pixels, 0);
    gMini_map_glowing_line_palettes[0] = (br_pixelmap*)(intptr_t)-1; /* FIXME: invalid pointer! */
    gMini_map_glowing_line_palettes[1] = gPalette_0074a604;
    gMini_map_glowing_line_palettes[2] = gPalette_0074a600;
    gPalette_0074a66c = gPalette_0074a5fc;
    gPalette_0074a670 = NULL;
}

// STUB: CARMA2_HW 0x004b52a0
void C2_HOOK_FASTCALL InitPaletteAnimate(void) {
    NOT_IMPLEMENTED();
}

// RevertPalette

// MungePalette

// ResetPalette

// Darken

// SetFadedPalette

// FadePaletteDown

// FadePaletteUp

// EnsurePaletteUp

// EnsureRenderPalette

// STUB: CARMA2_HW 0x0043dff0
void C2_HOOK_FASTCALL InitTransientBitmaps(void) {
    NOT_IMPLEMENTED();
}

// DeallocateTransientBitmap

// DeallocateAllTransientBitmaps

// RemoveTransientBitmaps

// SaveTransient

// DoMouseCursor

// AllocateCursorTransient

// InitMouseTargetLines

// StartMouseCursor

// EndMouseCursor

// DRDrawLine

// STUB: CARMA2_HW 0x0047b880
void C2_HOOK_FASTCALL ClearEntireScreen(void) {
    NOT_IMPLEMENTED();
}

// DistanceFromPlane

// KillSplashScreen

// SplashScreenWith

// DRPixelmapRectangleMaskedCopy

// DRMaskedStamp

// SwitchToHiresMode

// SwitchToLoresMode

// PoshDrawLine

// DeviouslyDimRectangle

// DimRectangle

// DimRectangleClipped

// DR8BitPixelmapRotatedAndFeatheredCopy

// DR16BitPixelmapRotatedAndFeatheredCopy

// DRPixelmapRotatedAndFeatheredCopy

// DRPixelmapBlendedLine

// DR8BitFancyDrawLine

// FancyDrawLine

// DRPixelmapCopyMapBlack8Bit

// DRPixelmapCopyMapBlack

// DisableLights

// FUNCTION: CARMA2_HW 0x0047d6d0
void C2_HOOK_FASTCALL EnableLights(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        BrLightEnable(gLight_array[i]);
    }
}
