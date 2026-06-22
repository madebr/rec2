#include "16-graphics1.h"

#include "02-init.h"
#include "19-font.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "brender/brender.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_types.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00703e24
int gWidth;

// GLOBAL: CARMA2_HW 0x00703e20
int gHeight;

// GLOBAL: CARMA2_HW 0x00705060
int gX_offset;

// GLOBAL: CARMA2_HW 0x006baa2c
int gY_offset;

// GLOBAL: CARMA2_HW 0x0068be30
int gBrZb_initialized;

// GLOBAL: CARMA2_HW 0x006a22bc
br_pixelmap* gRear_pixelmap;

// GLOBAL: CARMA2_HW 0x006a22b8
br_pixelmap* gEvalu;

// GLOBAL: CARMA2_HW 0x006a22f8
tWobble_spec gWobble_array[5];

// GLOBAL: CARMA2_HW 0x00705080
float gCosine_array[64];

// GLOBAL: CARMA2_HW 0x0065fdc8
tShadow_level gShadow_level = eShadow_us_only;

// GLOBAL: CARMA2_HW 0x0068be38
int gRender_indent;

// GLOBAL: CARMA2_HW 0x006baa40
tU8 gTemporary_physics_render_buffer[300000];

// MungeClipPlane

// TryThisEdge

// ProcessShadow

// RenderShadows

// FUNCTION: CARMA2_HW 0x004e9940
void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel) {

    gShadow_level = pLevel;
}

// GetShadowLevel

// ToggleShadow

// STUB: CARMA2_HW 0x004e99d0
void C2_HOOK_FASTCALL InitShadow(void) {
    NOT_IMPLEMENTED();
}

// SaveShadeTable

// SaveShadeTables

// STUB: CARMA2_HW 0x004e9b60
void C2_HOOK_FASTCALL DisposeSavedShadeTables(void) {
    NOT_IMPLEMENTED();
}

// ShadowMode

// LollipopizeActor

// ResetLollipopQueue

// RenderLollipops

// SetupDepthBuffer

// FUNCTION: CARMA2_HW 0x004e4980
void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight) {

    PDAllocateScreenAndBack();
    if (pWidth == 0) {
        pWidth = gBack_screen->width;
    }
    if (pHeight == 0) {
        pHeight = gBack_screen->height;
    }
    gRender_screen = DRPixelmapAllocateSub(gBack_screen, pX_offset, pY_offset, pWidth, pHeight);
    gWidth = pWidth;
    gHeight = pHeight;
    gX_offset = pX_offset;
    gY_offset = pY_offset;
    if (gGraf_specs[gGraf_spec_index].doubled) {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - 2 * gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - 2 * gGraf_specs[gGraf_spec_index].total_height) / 2;
    } else {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - gGraf_specs[gGraf_spec_index].total_height) / 2;
    }

    gScreen->origin_x = 0;
    gScreen->origin_y = 0;
    if (gBack_screen == NULL) {
        FatalError(kFatalError_AllocateOffScreenBuffer);
    }

    if (gDepth_buffer != NULL) {
        BrPixelmapFree(gDepth_buffer);
        gDepth_buffer = NULL;
    }
    gDepth_buffer = BrPixelmapMatch(gBack_screen, BR_PMMATCH_DEPTH_16);
    if (gDepth_buffer == NULL) {
        FatalError(kFatalError_AllocateZBuffer);
    }

    BrZbsBegin(gRender_screen->type, gDepth_buffer->type, gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
    gBrZb_initialized = 1;
    gRear_pixelmap = DRPixelmapAllocate(gScreen->type, 64, 64, NULL, 0);
    BrMapAdd(gRear_pixelmap);
}

// AdjustRenderScreenSize

// ScreenSmaller

// ScreenLarger

// FUNCTION: CARMA2_HW 0x004e4d30
void C2_HOOK_FASTCALL ClearWobbles(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gWobble_array); ++i) {
        gWobble_array[i].time_started = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e4d50
void C2_HOOK_FASTCALL InitWobbleStuff(void) {
    int i;

    ClearWobbles();
    for (i = 0; i < (int)REC2_ASIZE(gCosine_array); i++) {
        gCosine_array[i] = (float)cos((double)i / 64.0 * 3.141592653589793 / 2.0);
    }

    gEvalu = LoadPixelmap("Evalu01.PIX");
}

// NewScreenWobble

// SetScreenWobble

// ResetScreenWobble

// CalculateWobblitude

// ProcessNonTrackActors

// ConditionallyFillWithSky

// FoxyStuff

// DoARenderPass

// DoACompleteRenderPass

// RenderAFrame

// StartRenderingHeadups

// RenderThisHeadup

// StopRenderingHeadups

// FUNCTION: CARMA2_HW 0x004e5c70
void C2_HOOK_FASTCALL CleanPolyFontDanglers(void) {
    int i;

    for (i = 0; i < gCount_polyfont_glyph_actors; i++) {
        br_actor* actor = gPolyfont_glyph_actors[i];
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gCount_polyfont_glyph_actors = 0;
}
