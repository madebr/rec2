#include "16-graphics1.h"

#include "02-init.h"
#include "19-font.h"
#include "23-piping.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "brender/brender.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_types.h"
#include "rec2_macros.h"
#include "c2_string.h"
#include "c2_math.h"

#include <limits.h>

// GLOBAL: CARMA2_HW 0x00703e24
int gWidth;

// GLOBAL: CARMA2_HW 0x00703e20
int gHeight;

// GLOBAL: CARMA2_HW 0x00705060
int gX_offset;

// GLOBAL: CARMA2_HW 0x006baa2c
int gY_offset;

// GLOBAL: CARMA2_HW 0x00705188
int gScreen_wobble_x;

// GLOBAL: CARMA2_HW 0x00705184
int gScreen_wobble_y;

// GLOBAL: CARMA2_HW 0x0068be30
int gBrZb_initialized;

// GLOBAL: CARMA2_HW 0x006a22bc
br_pixelmap* gRear_pixelmap;

// GLOBAL: CARMA2_HW 0x006a22b8
br_pixelmap* gEvalu;

// GLOBAL: CARMA2_HW 0x006a22c4
int gNumber_of_lollipops;

// GLOBAL: CARMA2_HW 0x006a22f8
tWobble_spec gWobble_array[5];

// GLOBAL: CARMA2_HW 0x00705080
float gCosine_array[65];

// GLOBAL: CARMA2_HW 0x0065fdc8
tShadow_level gShadow_level = eShadow_us_only;

// GLOBAL: CARMA2_HW 0x0068be38
int gRender_indent;

// GLOBAL: CARMA2_HW 0x006baa40
tU8 gTemporary_physics_render_buffer[300000];

// GLOBAL: CARMA2_HW 0x006a2448
tClip_details gShadow_clip_planes[8];

// GLOBAL: CARMA2_HW 0x006a23d8
int gFancy_shadow;

// GLOBAL: CARMA2_HW 0x006a27e0
br_material* gShadow_material;

// GLOBAL: CARMA2_HW 0x006a27d0
br_vector3 gShadow_light_ray;

// GLOBAL: CARMA2_HW 0x006a27c0
br_vector3 gShadow_light_z;

// GLOBAL: CARMA2_HW 0x006a27b0
br_vector3 gShadow_light_x;

// GLOBAL: CARMA2_HW 0x006a27e4
br_model* gShadow_model;

// GLOBAL: CARMA2_HW 0x006a2444
br_actor* gShadow_actor;

// GLOBAL: CARMA2_HW 0x006a2488
tSaved_table gSaved_shade_tables[100];

// GLOBAL: CARMA2_HW 0x006a27a8
int gSaved_table_count;

// GLOBAL: CARMA2_HW 0x00703e28
int gHud_actor_storage_size;

// GLOBAL: CARMA2_HW 0x00704e60
br_actor* gHud_actor_storage[128];

// GLOBAL: CARMA2_HW 0x00762100
br_scalar gYon_squared;

// MungeClipPlane

// TryThisEdge

// ProcessShadow

// RenderShadows

// FUNCTION: CARMA2_HW 0x004e9940
void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel) {

    gShadow_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x004e9950
tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void) {
    return gShadow_level;
}

// ToggleShadow

// FUNCTION: CARMA2_HW 0x004e99d0
void C2_HOOK_FASTCALL InitShadow(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(gShadow_clip_planes[0]) != 8);

    for (i = 0; i < (int)REC2_ASIZE(gShadow_clip_planes); i++) {
        gShadow_clip_planes[i].clip = BrActorAllocate(BR_ACTOR_CLIP_PLANE, NULL);
        BrActorAdd(gUniverse_actor, gShadow_clip_planes[i].clip);
        BrClipPlaneDisable(gShadow_clip_planes[i].clip);
        BrMatrix34Identity(&gShadow_clip_planes[i].clip->t.t.mat);
    }
    gFancy_shadow = 1;
    gShadow_material = BrMaterialFind("SHADOW.MAT");
    BrVector3Set(&gShadow_light_ray, 0.f, -1.f, 0.f);
    BrVector3Set(&gShadow_light_z, 0.f, 0.f, -1.f);
    BrVector3Set(&gShadow_light_x, 1.f, 0.f, 0.f);

    gShadow_model = BrModelAllocate("", 0, 0);
    gShadow_model->flags = BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL;
    gShadow_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gShadow_actor->model = gShadow_model;
    BrActorAdd(gUniverse_actor, gShadow_actor);
}

// FUNCTION: CARMA2_HW 0x004e9ae0
void C2_HOOK_FASTCALL SaveShadeTables(void) {
    PossibleService();
    gSaved_table_count = 0;
    BrTableEnum("*", SaveShadeTable, NULL);
}

// FUNCTION: CARMA2_HW 0x004e9b10
br_uint_32 C2_HOOK_CDECL SaveShadeTable(br_pixelmap* pTable, void* pArg) {
    br_pixelmap* copy;

    (void)pArg;
    if (gSaved_table_count == (int)REC2_ASIZE(gSaved_shade_tables)) {
        return 1;
    }
    gSaved_shade_tables[gSaved_table_count].original = pTable;
    copy = BrMemAllocate(sizeof(br_pixelmap), kMem_misc);
    gSaved_shade_tables[gSaved_table_count].copy = copy;
    gSaved_table_count++;
    memcpy(copy, pTable, sizeof(br_pixelmap));
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e9b60
void C2_HOOK_FASTCALL DisposeSavedShadeTables(void) {
    int i;

    for (i = 0; i < gSaved_table_count; i++) {
        BrMemFree(gSaved_shade_tables[i].copy);
    }
}

// ShadowMode

// FUNCTION: CARMA2_HW 0x004e4650
void C2_HOOK_FASTCALL LollipopizeActor(br_actor* pSubject_actor, br_matrix34* ref_to_world, tLollipop_mode pWhich_axis) {
    br_vector3 ref_to_subject;
    br_vector3 fixed_axis;
    br_vector3 vector_a;
    br_vector3 vector_b;
    br_matrix34 subject_to_world;
    br_matrix34 mat;

    BrActorToActorMatrix34(&subject_to_world, pSubject_actor, gNon_track_actor);
    BrVector3Sub(&ref_to_subject, (br_vector3*)ref_to_world->m[3], (br_vector3*)subject_to_world.m[3]);
    switch (pWhich_axis) {
    case eLollipop_x_match:
        BrVector3SetFloat(&vector_a, 1.0f, 0.0f, 0.0f);
        break;
    case eLollipop_y_match:
        BrVector3SetFloat(&vector_a, 0.0f, 1.0f, 0.0f);
        break;
    case eLollipop_z_match:
        BrVector3SetFloat(&vector_a, 0.0f, 0.0f, 1.0f);
        break;
    default:
        break;
    }
    BrVector3Cross(&vector_b, &ref_to_subject, &vector_a);
    BrVector3Normalise(&vector_b, &vector_b);
    BrVector3Cross(&fixed_axis, &vector_a, &vector_b);
    switch (pWhich_axis) {
    case eLollipop_x_match:
        mat.m[0][0] = vector_a.v[0];
        mat.m[1][0] = vector_a.v[1];
        mat.m[2][0] = vector_a.v[2];
        mat.m[0][1] = vector_b.v[0];
        mat.m[1][1] = vector_b.v[1];
        mat.m[2][1] = vector_b.v[2];
        mat.m[0][2] = fixed_axis.v[0];
        mat.m[1][2] = fixed_axis.v[1];
        mat.m[2][2] = fixed_axis.v[2];
        mat.m[3][0] = 0.0f;
        mat.m[3][1] = 0.0f;
        mat.m[3][2] = 0.0f;
        break;
    case eLollipop_y_match:
        mat.m[0][0] = vector_b.v[0];
        mat.m[1][0] = vector_b.v[1];
        mat.m[2][0] = vector_b.v[2];
        mat.m[0][1] = vector_a.v[0];
        mat.m[1][1] = vector_a.v[1];
        mat.m[2][1] = vector_a.v[2];
        mat.m[0][2] = fixed_axis.v[0];
        mat.m[1][2] = fixed_axis.v[1];
        mat.m[2][2] = fixed_axis.v[2];
        mat.m[3][0] = 0.0f;
        mat.m[3][1] = 0.0f;
        mat.m[3][2] = 0.0f;
        break;
    case eLollipop_z_match:
        mat.m[0][0] = vector_b.v[0];
        mat.m[1][0] = vector_b.v[1];
        mat.m[2][0] = vector_b.v[2];
        mat.m[0][1] = fixed_axis.v[0];
        mat.m[1][1] = fixed_axis.v[1];
        mat.m[2][1] = fixed_axis.v[2];
        mat.m[0][2] = vector_a.v[0];
        mat.m[1][2] = vector_a.v[1];
        mat.m[2][2] = vector_a.v[2];
        mat.m[3][0] = 0.0f;
        mat.m[3][1] = 0.0f;
        mat.m[3][2] = 0.0f;
        break;
    default:
        break;
    }
    BrMatrix34Pre(&pSubject_actor->t.t.mat, &mat);
}

// FUNCTION: CARMA2_HW 0x004e4900
void C2_HOOK_FASTCALL ResetLollipopQueue(void) {
    gNumber_of_lollipops = 0;
}

// RenderLollipops

// FUNCTION: CARMA2_HW 0x004e4940
void C2_HOOK_FASTCALL SetupDepthBuffer(void) {
    if (gDepth_buffer != NULL) {
        BrPixelmapFree(gDepth_buffer);
        gDepth_buffer = NULL;
    }
    gDepth_buffer = BrPixelmapMatch(gBack_screen, BR_PMMATCH_DEPTH_16);
    if (gDepth_buffer == NULL) {
        FatalError(kFatalError_AllocateZBuffer);
    }
}

// FUNCTION: CARMA2_HW 0x004e4980
void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight) {

    PDAllocateScreenAndBack();
    if (pWidth == 0) {
        pWidth = gBack_screen->width;
    }
    if (pHeight == 0) {
        pHeight = gBack_screen->height;
    }
    gRender_screen = DRPixelmapAllocateSub(gBack_screen, (br_uint_16)pX_offset, (br_uint_16)pY_offset, (br_uint_16)pWidth, (br_uint_16)pHeight);
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

    SetupDepthBuffer();

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

// FUNCTION: CARMA2_HW 0x004e4db0
void C2_HOOK_FASTCALL NewScreenWobble(double pAmplitude_x, double pAmplitude_y, double pPeriod) {
    int i;
    int index = -1;
    int oldest_time = INT_MAX;

    for (i = 0; i < (int)REC2_ASIZE(gWobble_array); ++i) {
        if (gWobble_array[i].time_started == 0) {
            index = i;
            break;
        }
        if (gWobble_array[i].time_started < oldest_time) {
            oldest_time = gWobble_array[i].time_started;
            index = i;
        }
    }
    gWobble_array[index].time_started = (int)GetTotalTime();
    gWobble_array[index].amplitude_x = (float)pAmplitude_x;
    gWobble_array[index].amplitude_y = (float)pAmplitude_y;
    gWobble_array[index].period = (float)pPeriod;
}

// FUNCTION: CARMA2_HW 0x004e4e20
void C2_HOOK_FASTCALL SetScreenWobble(int pX, int pY) {
    gScreen_wobble_x = pX;
    gScreen_wobble_y = pY;
}

// FUNCTION: CARMA2_HW 0x004e4e30
void C2_HOOK_FASTCALL ResetScreenWobble(void) {
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
}

void C2_HOOK_FASTCALL CalculateWobblitude(tU32 pThe_time) {
    int i;
    tU32 time_going;
    double mod_angle;
    double cosine_over_angle;
    double wobble_factor;
    tWobble_spec* wobble;
    tGraf_data* graf_data;

    if (gProgram_state.new_view != eView_undefined) {
        return;
    }
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
    for (i = 0; i < (int)REC2_ASIZE(gWobble_array); ++i) {
        wobble = &gWobble_array[i];
        if (wobble->time_started != 0) {
            time_going = pThe_time - (tU32)wobble->time_started;
            if (time_going > 1000) {
                wobble->time_started = 0;
            } else {
                mod_angle = fmod((double)time_going / wobble->period, REC2_TAU);
                if (mod_angle > REC2_3PI_OVER_2) {
                    cosine_over_angle = gCosine_array[(int)((REC2_TAU - mod_angle) * (128.0 / REC2_PI))];
                } else if (mod_angle > REC2_PI) {
                    cosine_over_angle = -gCosine_array[(int)((mod_angle - REC2_PI) * (128.0 / REC2_PI))];
                } else if (mod_angle > REC2_PI_OVER_2) {
                    cosine_over_angle = -gCosine_array[(int)((REC2_PI - mod_angle) * (128.0 / REC2_PI))];
                } else {
                    cosine_over_angle = gCosine_array[(int)(mod_angle * (128.0 / REC2_PI))];
                }
                wobble_factor = cosine_over_angle / (1.0 + time_going * 0.0035);
                /* Truncate the accumulated displacement after each contribution. */
                gScreen_wobble_x = (int)(gScreen_wobble_x + wobble->amplitude_x * wobble_factor);
                gScreen_wobble_y = (int)(gScreen_wobble_y + wobble->amplitude_y * wobble_factor);
            }
        }
    }
    graf_data = gCurrent_graf_data;
    if (gScreen_wobble_x > graf_data->cock_margin_x) {
        gScreen_wobble_x = graf_data->cock_margin_x;
    } else if (gScreen_wobble_x < -graf_data->cock_margin_x) {
        gScreen_wobble_x = -graf_data->cock_margin_x;
    }
    if (gScreen_wobble_y > graf_data->cock_margin_y) {
        gScreen_wobble_y = graf_data->cock_margin_y;
    } else if (gScreen_wobble_y < -graf_data->cock_margin_y) {
        gScreen_wobble_y = -graf_data->cock_margin_y;
    }
    PipeSingleScreenWobble(gScreen_wobble_x, gScreen_wobble_y);
}

// ProcessNonTrackActors

// ConditionallyFillWithSky

// FoxyStuff

// DoARenderPass

// DoACompleteRenderPass

// RenderAFrame

// FUNCTION: CARMA2_HW 0x004e5a70
void C2_HOOK_FASTCALL StartRenderingHeadups(void) {
    CleanPolyFontDanglers();
    gHud_actor_storage_size = 0;
    gRender_poly_text = 0;
}

// FUNCTION: CARMA2_HW 0x004e5ad0
void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor) {
    if (gHud_actor_storage_size >= (int)REC2_ASIZE(gHud_actor_storage)) {
        BrFailure("Not enough HUD actor storage");
    }
    gHud_actor_storage[gHud_actor_storage_size] = pActor;
    gHud_actor_storage_size++;
}

// FUNCTION: CARMA2_HW 0x004e5b00
void C2_HOOK_FASTCALL StopRenderingHeadups(void) {
    br_int_16 original_origin_x;
    br_int_16 original_origin_y;
    br_uint_16 original_base_x;
    br_uint_16 original_base_y;
    br_actor* actor;
    int i;

    if (gCount_polyfont_glyph_actors != 0) {
        BrActorAdd(gHUD_root, gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        actor = gHud_actor_storage[i];
        if (actor != NULL && actor->prev == NULL) {
            BrActorAdd(gHUD_root, actor);
        }
    }
    original_origin_x = gRender_screen->origin_x;
    original_origin_y = gRender_screen->origin_y;
    original_base_x = gRender_screen->base_x;
    original_base_y = gRender_screen->base_y;
    gRender_screen->origin_x = 0;
    gRender_screen->origin_y = 0;
    gRender_screen->base_x = 0;
    gRender_screen->base_y = 0;
    BrZbSceneRender(gHUD_root, gHUD_camera, gRender_screen, gDepth_buffer);
    gRender_screen->origin_x = original_origin_x;
    gRender_screen->origin_y = original_origin_y;
    gRender_screen->base_x = original_base_x;
    gRender_screen->base_y = original_base_y;
    if (gString_root_actor->parent != NULL) {
        BrActorRemove(gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        actor = gHud_actor_storage[i];
        if (actor != NULL && actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gHud_actor_storage_size = 0;
    CleanPolyFontDanglers();
    gRender_poly_text = 1;
}

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
