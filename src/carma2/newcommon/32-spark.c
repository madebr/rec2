#include "32-spark.h"

#include "08-loading1.h"
#include "13-crush1.h"
#include "18-graphics2.h"
#include "23-piping.h"
#include "30-opponent.h"
#include "33-depth.h"
#include "36-trig.h"
#include "41-utility.h"
#include "49-physics.h"
#include "52-errors.h"
#include "60-phil.h"
#include "61-pedestrn.h"
#include "63-loading3.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include "c2_string.h"
#include "c2_stdlib.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x00660110
int gSmoke_on = 1;

// GLOBAL: CARMA2_HW 0x006b7840
int gShade_list[16];

// GLOBAL: CARMA2_HW 0x00660140
int* gShade_table = &gShade_list[8];

// GLOBAL: CARMA2_HW 0x006a82b4
int gNum_dust_tables;

// GLOBAL: CARMA2_HW 0x006aa57c
int gSpark_flags;

// GLOBAL: CARMA2_HW 0x006aa5ac
br_pixelmap* gIt_shade_table;

// GLOBAL: CARMA2_HW 0x006a8760
tBRender_smoke* gBR_smoke_pointers[35];

// GLOBAL: CARMA2_HW 0x006aa56c
int gN_BR_smoke_structs;

// GLOBAL: CARMA2_HW 0x00660148
static br_token_value gSmoke_material_tokens[3] = {
    { BRT_BLEND_B, { 1 } },
    { BRT_OPACITY_X, { BR_FIXED_INT(75) } },
    { BR_NULL_TOKEN, { 0 } },
};

// GLOBAL: CARMA2_HW 0x006a8ce0
tSmoke gSmoke[25];

// GLOBAL: CARMA2_HW 0x006aa590
int gSmoke_flags;

// GLOBAL: CARMA2_HW 0x006aa594
int gSmoke_next;

// GLOBAL: CARMA2_HW 0x006aa568
int gDust_rotate;

// GLOBAL: CARMA2_HW 0x006aa59c
int gColumn_flags;

// GLOBAL: CARMA2_HW 0x006aa5a0
int gColumn_next;

// GLOBAL: CARMA2_HW 0x006a96a8
tSmoke_column gSmoke_column[10];

// GLOBAL: CARMA2_HW 0x006b7880
br_material* gBlack_material;

// GLOBAL: CARMA2_HW 0x006aa380
br_model* gLollipop_model;

// GLOBAL: CARMA2_HW 0x00660118
static tU8 gFlame_frame_size[20][2] = {
    { 20, 25 }, { 24, 32 }, { 24, 34 }, { 28, 44 }, { 28, 49 },
    { 32, 53 }, { 28, 61 }, { 28, 69 }, { 24, 61 }, { 28, 64 },
    { 24, 70 }, { 20, 65 }, { 20, 64 }, { 20, 74 }, { 28, 55 },
    { 24, 62 }, { 28, 47 }, { 20, 51 }, { 20, 45 }, { 20, 45 },
};

// GLOBAL: CARMA2_HW 0x006a8638
br_pixelmap* gFlame_map[20];

// GLOBAL: CARMA2_HW 0x006aa570
int gSplash_flags;

// GLOBAL: CARMA2_HW 0x006a8758
br_model* gSplash_model;

// GLOBAL: CARMA2_HW 0x006aa5a4
int gNum_splash_types;

// GLOBAL: CARMA2_HW 0x006a9130
br_material* gSplash_material[20];

// GLOBAL: CARMA2_HW 0x006a82b8
tSplash gSplash[32];

// GLOBAL: CARMA2_HW 0x006aa588
br_model* gShrapnel_model[2];

// GLOBAL: CARMA2_HW 0x006aa584
int gShrapnel_flags;

// GLOBAL: CARMA2_HW 0x006a9180
tShrapnel gShrapnel[30];

// GLOBAL: CARMA2_HW 0x006a9b80
tSpark gSpark[32];

// GLOBAL: CARMA2_HW 0x006a82b0
br_camera* gSpark_cam;

// GLOBAL: CARMA2_HW 0x006a8718
br_matrix4 gCameraToScreen;

// GLOBAL: CARMA2_HW 0x006aa388
tReplay_splash gReplay_splashes[10];

// GLOBAL: CARMA2_HW 0x006aa574
int gReplay_splash_flags;

// GLOBAL: CARMA2_HW 0x006a82ac
int gSplash_next;

// GLOBAL: CARMA2_HW 0x006aa578
int gSpark_next;

// GLOBAL: CARMA2_HW 0x006b7884
tU32 gNext_AFE_color_cycle;

// GLOBAL: CARMA2_HW 0x006aa5b0
int gAFE_color_cycle;

static br_scalar C2_HOOK_CDECL SmokeScalarRound(double pValue);
static tU32 C2_HOOK_CDECL SmokeScalarBits(const br_scalar* pValue);

static int C2_HOOK_CDECL SplashReplayMoving(void) {
    br_scalar rate;
    tU32 bits;

    rate = ARGetReplayRate();
    bits = SmokeScalarBits(&rate) & 0x7fffffffu;
    /* Original FCOMP/TEST AH,40 treats unordered rates as paused. */
    return bits != 0 && bits <= 0x7f800000u;
}

/* These callers consume only the original _ftol result's low DWORD. */
static tS32 C2_HOOK_CDECL SparkFtol(double pValue) {
    tU32 words[2];

    memcpy(words, &pValue, sizeof(words));
    if ((words[1] & 0x7fffffffu) >= 0x43e00000u) {
        return 0;
    }
    return (tS32)(tU32)(br_int_64)pValue;
}

static int C2_HOOK_CDECL SparkClipInteger(int pOrigin, int pOther, int pAmount, int pDivisor) {
    tS32 product;

    product = (tS32)(((tU32)pOrigin - (tU32)pOther) * (tU32)pAmount);
    return (tS32)((tU32)pOrigin - (tU32)(product / pDivisor));
}

static void C2_HOOK_CDECL SparkClipDepth(br_vector3* pPoint, const br_vector3* pOther, int pAmount, int pDivisor) {
    pPoint->v[2] = SmokeScalarRound((double)pOther->v[2]
        - (((double)pOther->v[2] - pPoint->v[2]) * pAmount) / pDivisor);
}

// DrawDot

// SetWorldToScreen

// DrawLine3DThroughBRender

// FUNCTION: CARMA2_HW 0x004f6b80
int C2_HOOK_FASTCALL DrawLine3D(const br_vector3* pStart, const br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_pixelmap* pShade) {
    br_vector3 start;
    br_vector3 end;
    br_vector4 projected_start;
    br_vector4 projected_end;
    double fraction;
    br_scalar near_z;
    int i;

    if (gNo_2d_effects) {
        for (i = 0; i < 3; i++) {
            memcpy(&gLine_model->vertices[0].p.v[i], &pStart->v[i], sizeof(br_scalar));
        }
        for (i = 0; i < 3; i++) {
            memcpy(&gLine_model->vertices[1].p.v[i], &pEnd->v[i], sizeof(br_scalar));
        }
        BrModelUpdate(gLine_model, BR_MODU_VERTEX_POSITIONS);
        BrZbsSceneRenderAdd(gLine_actor);
        return 999;
    }
    /* X is MOV-copied; the other original FLD/FSTP pairs quiet sNaNs. */
    memcpy(&start.v[0], &pStart->v[0], sizeof(br_scalar));
    start.v[1] = SmokeScalarRound(pStart->v[1]);
    start.v[2] = SmokeScalarRound(pStart->v[2]);
    for (i = 0; i < 3; i++) {
        end.v[i] = SmokeScalarRound(pEnd->v[i]);
    }
    near_z = SmokeScalarRound(-(double)gSpark_cam->hither_z);
    if (start.v[2] > near_z || end.v[2] > near_z) {
        if (start.v[2] > near_z && end.v[2] > near_z) {
            return 0;
        }
        fraction = ((double)end.v[2] + gSpark_cam->hither_z) / ((double)end.v[2] - start.v[2]);
        if (start.v[2] > near_z) {
            start.v[0] = SmokeScalarRound((double)end.v[0] - ((double)end.v[0] - start.v[0]) * fraction);
            start.v[1] = SmokeScalarRound((double)end.v[1] - ((double)end.v[1] - start.v[1]) * fraction);
            start.v[2] = SmokeScalarRound(-(double)gSpark_cam->hither_z);
        }
        if (end.v[2] > -gSpark_cam->hither_z) {
            end.v[0] = SmokeScalarRound((double)end.v[0] - ((double)end.v[0] - start.v[0]) * fraction);
            end.v[1] = SmokeScalarRound((double)end.v[1] - ((double)end.v[1] - start.v[1]) * fraction);
            end.v[2] = SmokeScalarRound(-(double)gSpark_cam->hither_z);
        }
    }
    BrMatrix4ApplyP(&projected_start, &start, &gCameraToScreen);
    BrMatrix4ApplyP(&projected_end, &end, &gCameraToScreen);
    for (i = 0; i < 3; i++) {
        start.v[i] = SmokeScalarRound((double)projected_start.v[i] / projected_start.v[3]);
    }
    for (i = 0; i < 3; i++) {
        end.v[i] = SmokeScalarRound((double)projected_end.v[i] / projected_end.v[3]);
    }
    return DrawLine2D(&start, &end, pScreen, pDepth, 1.f, pShade);
}

// FUNCTION: CARMA2_HW 0x004f6de0
int C2_HOOK_FASTCALL DrawLine2D(br_vector3* pStart, br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_scalar pBrightness, br_pixelmap* pShade) {
    tU8* screen_base;
    tU8* shade;
    tU8* pixel;
    tU16* depth;
    int x0;
    int x1;
    int y0;
    int y1;
    int delta_x;
    int delta_y;
    int twice_x;
    int twice_y;
    int step_x;
    int step_y;
    int amount;
    int divisor;
    int shade_count;
    int shade_step;
    int shade_error;
    int error;
    double z;
    double z_step;
    double depth_value;
    tU32 bits[2];

    screen_base = (tU8*)pScreen->pixels + (tU16)pScreen->base_y * pScreen->row_bytes + (tU16)pScreen->base_x;
    depth = (tU16*)pDepth->pixels;
    shade = (tU8*)pShade->pixels + (tU16)pShade->base_y * pShade->row_bytes;
    x0 = (tS32)((tU32)SparkFtol(pStart->v[0]) + ((tU16)pScreen->width >> 1));
    x1 = (tS32)((tU32)SparkFtol(pEnd->v[0]) + ((tU16)pScreen->width >> 1));
    y0 = (tS32)(((tU16)pScreen->height >> 1) - (tU32)SparkFtol(pStart->v[1]));
    y1 = (tS32)(((tU16)pScreen->height >> 1) - (tU32)SparkFtol(pEnd->v[1]));
    if ((SmokeScalarBits(&pBrightness) & 0x7fffffffu) > 0x7f800000u || pBrightness < .001f || pBrightness > 1.f) {
        return 0;
    }
    if (x0 < 0 || x1 < 0) {
        if (x0 < 0 && x1 < 0) {
            return 0;
        }
        if (x0 < 0) {
            divisor = (tS32)((tU32)x1 - (tU32)x0);
            SparkClipDepth(pStart, pEnd, x1, divisor);
            y0 = SparkClipInteger(y1, y0, x1, divisor);
            x0 = 0;
        } else {
            divisor = (tS32)((tU32)x0 - (tU32)x1);
            SparkClipDepth(pEnd, pStart, x0, divisor);
            y1 = SparkClipInteger(y0, y1, x0, divisor);
            x1 = 0;
        }
    }
    if (x0 >= (tU16)pScreen->width || x1 >= (tU16)pScreen->width) {
        if (x0 >= (tU16)pScreen->width && x1 >= (tU16)pScreen->width) {
            return 0;
        }
        if (x0 >= (tU16)pScreen->width) {
            amount = x1 - (tU16)pScreen->width + 1;
            divisor = (tS32)((tU32)x1 - (tU32)x0);
            SparkClipDepth(pStart, pEnd, amount, divisor);
            y0 = SparkClipInteger(y1, y0, amount, divisor);
            x0 = (tU16)pScreen->width - 1;
        } else {
            amount = x0 - (tU16)pScreen->width + 1;
            divisor = (tS32)((tU32)x0 - (tU32)x1);
            SparkClipDepth(pEnd, pStart, amount, divisor);
            y1 = SparkClipInteger(y0, y1, amount, divisor);
            x1 = (tU16)pScreen->width - 1;
        }
    }
    if (y0 < 0 || y1 < 0) {
        if (y0 < 0 && y1 < 0) {
            return 0;
        }
        if (y0 < 0) {
            divisor = (tS32)((tU32)y1 - (tU32)y0);
            SparkClipDepth(pStart, pEnd, y1, divisor);
            x0 = SparkClipInteger(x1, x0, y1, divisor);
            y0 = 0;
        } else {
            divisor = (tS32)((tU32)y0 - (tU32)y1);
            SparkClipDepth(pEnd, pStart, y0, divisor);
            x1 = SparkClipInteger(x0, x1, y0, divisor);
            y1 = 0;
        }
    }
    if (y0 >= (tU16)pScreen->height || y1 >= (tU16)pScreen->height) {
        if (y0 >= (tU16)pScreen->height && y1 >= (tU16)pScreen->height) {
            return 0;
        }
        if (y0 >= (tU16)pScreen->height) {
            amount = y1 - (tU16)pScreen->height + 1;
            divisor = (tS32)((tU32)y1 - (tU32)y0);
            SparkClipDepth(pStart, pEnd, amount, divisor);
            x0 = SparkClipInteger(x1, x0, amount, divisor);
            y0 = (tU16)pScreen->height - 1;
        } else {
            amount = y0 - (tU16)pScreen->height + 1;
            divisor = (tS32)((tU32)y0 - (tU32)y1);
            SparkClipDepth(pEnd, pStart, amount, divisor);
            x1 = SparkClipInteger(x0, x1, amount, divisor);
            y1 = (tU16)pScreen->height - 1;
        }
    }
    delta_x = x1 - x0;
    delta_y = y1 - y0;
    step_x = delta_x >= 0 ? 1 : -1;
    step_y = delta_y >= 0 ? 1 : -1;
    if (delta_x < 0) {
        delta_x = -delta_x;
    }
    if (delta_y < 0) {
        delta_y = -delta_y;
    }
    twice_x = delta_x * 2;
    twice_y = delta_y * 2;
    pixel = screen_base + y0 * pScreen->row_bytes + x0;
    depth += (pDepth->row_bytes / 2) * y0 + x0;
    shade_count = SparkFtol(((double)pBrightness - .001f) * (tU16)pShade->height);
    z = pStart->v[2];
    if (twice_x > twice_y) {
        error = twice_y - twice_x / 2;
        shade_step = (delta_x * 1000) / shade_count;
        shade_error = shade_step;
        z_step = (((double)pEnd->v[2] - pStart->v[2]) * 2.0) / twice_x;
    } else {
        error = twice_x - twice_y / 2;
        shade_step = (delta_y * 1000) / shade_count;
        shade_error = shade_step;
        z_step = (((double)pEnd->v[2] - pStart->v[2]) * 2.0) / twice_y;
    }
    for (;;) {
        depth_value = (1.0 - z) * 32768.f;
        memcpy(bits, &depth_value, sizeof(bits));
        if (((bits[1] & 0x7fffffffu) > 0x7ff00000u || ((bits[1] & 0x7fffffffu) == 0x7ff00000u && bits[0] != 0)) || depth_value < *depth) {
            *depth = (tU16)SparkFtol(depth_value);
            *pixel = shade[*pixel];
        }
        if (twice_x > twice_y) {
            if (x0 == x1) {
                break;
            }
            if (error >= 0) {
                pixel += step_y * pScreen->row_bytes;
                depth += (pDepth->row_bytes / 2) * step_y;
                error -= twice_x;
            }
            x0 += step_x;
            pixel += step_x;
            depth += step_x;
            error += twice_y;
        } else {
            if (y0 == y1) {
                break;
            }
            if (error >= 0) {
                pixel += step_x;
                depth += step_x;
                error -= twice_y;
            }
            y0 += step_y;
            pixel += step_y * pScreen->row_bytes;
            depth += (pDepth->row_bytes / 2) * step_y;
            error += twice_x;
        }
        shade_error -= 1000;
        z += z_step;
        while (shade_error < 1) {
            shade_error += shade_step;
            shade += pShade->row_bytes;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004f7cb0
void C2_HOOK_FASTCALL SetLineModelCols(tU8 pWhite) {
    gLine_model->vertices[0].red = 255;
    gLine_model->vertices[0].grn = pWhite ? 255 : 0;
    gLine_model->vertices[0].blu = pWhite ? 255 : 0;
    gLine_model->vertices[1].red = 255;
    gLine_model->vertices[1].grn = 255;
    gLine_model->vertices[1].blu = pWhite ? 255 : 0;
    BrModelUpdate(gLine_model, BR_MODU_ALL);
}

// ReplaySparks

// FUNCTION: CARMA2_HW 0x004f7450
void C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pScreen, br_pixelmap* pDepth, br_actor* pCamera, const br_matrix34* pCamera_to_world, tU32 pTime) {
    br_matrix4 projection;
    br_matrix4 scale;
    br_matrix34* camera_to_world;
    br_vector3 world_pos;
    br_vector3 world_end;
    br_vector3 delta;
    br_vector3 view_start;
    br_vector3 view_end;
    br_vector3 temp;
    tSpark* spark;
    double dot;
    double interval;
    double factor;
    double new_z;
    double damping;
    br_scalar float_time;
    br_scalar gravity;
    br_scalar random_y;
    br_scalar random_z;
    int trail;
    int replay;
    int i;
    int j;
    tU32 bit;

    (void)pCamera_to_world;
    gSpark_cam = pCamera->type_data;
    BrMatrix4Perspective(&projection, gSpark_cam->field_of_view, gSpark_cam->aspect,
        SmokeScalarRound(-(double)gSpark_cam->hither_z), SmokeScalarRound(-(double)gSpark_cam->yon_z));
    BrMatrix4Scale(&scale, (br_scalar)((tU16)pScreen->width >> 1), (br_scalar)((tU16)pScreen->height >> 1), 1.f);
    BrMatrix4Mul(&gCameraToScreen, &projection, &scale);
    if (gSpark_flags == 0) {
        return;
    }
    if (gNo_2d_effects) {
        BrActorRemove(gLine_actor);
        BrActorAdd(pCamera, gLine_actor);
    }
    replay = gAction_replay_mode;
    if (!replay) {
        ARStartPipingSession(ePipe_chunk_spark);
    }
    for (i = 0; i < 32; i++) {
        bit = 1u << i;
        if (!((tU32)gSpark_flags & bit)) {
            continue;
        }
        spark = &gSpark[i];
        if (replay) {
            if (spark->car != NULL) {
                /* The original first transforms this unwritten private endpoint,
                 * then replaces it below. Preserve the call and its ordering. */
                BrMatrix34ApplyP(&temp, &world_end, &spark->car->car_master_actor->t.t.mat);
                memcpy(&world_end, &temp, sizeof(world_end));
                BrMatrix34ApplyP(&world_pos, &spark->pos, &spark->car->car_master_actor->t.t.mat);
            } else {
                memcpy(&world_pos, &spark->pos, sizeof(world_pos));
            }
            for (j = 0; j < 3; j++) {
                world_end.v[j] = SmokeScalarRound((double)spark->length.v[j] + world_pos.v[j]);
            }
        } else {
            if (spark->count <= 0) {
                spark->count = 0;
                gSpark_flags = (int)((tU32)gSpark_flags & ~bit);
            }
            dot = ((double)spark->normal.v[2] * spark->v.v[2]
                + (double)spark->normal.v[0] * spark->v.v[0])
                + (double)spark->normal.v[1] * spark->v.v[1];
            for (j = 0; j < 3; j++) {
                temp.v[j] = SmokeScalarRound((double)spark->normal.v[j] * dot);
            }
            for (j = 0; j < 3; j++) {
                spark->v.v[j] = SmokeScalarRound((double)spark->v.v[j] - temp.v[j]);
            }
            if (spark->time_sync != 0) {
                interval = (double)spark->time_sync * .001f;
                spark->count = (int)((tU32)spark->count + spark->time_sync - pTime);
                spark->time_sync = 0;
            } else {
                interval = (double)pTime * .001f;
                spark->count = (int)((tU32)spark->count - pTime);
            }
            for (j = 0; j < 3; j++) {
                spark->pos.v[j] = SmokeScalarRound((double)spark->v.v[j] * interval + spark->pos.v[j]);
            }
            trail = (int)(1000u - (tU32)spark->count);
            if (trail > 150) {
                trail = 150;
            }
            factor = (double)(tS32)(0u - (tU32)trail) * .001f;
            if (spark->colour != 0) {
                factor *= .5f;
            }
            for (j = 0; j < 3; j++) {
                spark->length.v[j] = SmokeScalarRound((double)spark->v.v[j] * factor);
            }
            gravity = SmokeScalarRound((double)pTime * 10.f * .00014492753623188405);
            if (spark->car != NULL) {
                BrMatrix34ApplyV(&temp, &spark->length, &spark->car->car_master_actor->t.t.mat);
                for (j = 0; j < 3; j++) {
                    spark->length.v[j] = SmokeScalarRound(temp.v[j]);
                }
                BrMatrix34ApplyP(&world_pos, &spark->pos, &spark->car->car_master_actor->t.t.mat);
                for (j = 0; j < 3; j++) {
                    spark->v.v[j] = SmokeScalarRound((double)spark->v.v[j]
                        - (double)spark->car->car_master_actor->t.t.mat.m[j][1] * gravity);
                }
            } else {
                memcpy(&world_pos, &spark->pos, sizeof(world_pos));
                spark->v.v[1] = SmokeScalarRound((double)spark->v.v[1] - gravity);
            }
            AddSparkToPipingSession((spark->colour << 8) + i, &world_pos, &spark->length);
            for (j = 0; j < 3; j++) {
                world_end.v[j] = SmokeScalarRound((double)world_pos.v[j] + spark->length.v[j]);
            }
        }
        camera_to_world = gRendering_mirror ? &gRearview_camera_to_world : &gCamera_to_world;
        for (j = 0; j < 3; j++) {
            delta.v[j] = SmokeScalarRound((double)world_pos.v[j] - camera_to_world->m[3][j]);
        }
        BrMatrix34TApplyV(&view_start, &delta, camera_to_world);
        for (j = 0; j < 3; j++) {
            delta.v[j] = SmokeScalarRound((double)world_end.v[j] - camera_to_world->m[3][j]);
        }
        BrMatrix34TApplyV(&view_end, &delta, camera_to_world);
        if (!replay) {
            random_z = SmokeScalarRound(FRandomBetween(-.1f, .1f));
            random_y = SmokeScalarRound(FRandomBetween(-.1f, .1f));
            BrVector3SetFloat(&temp, SmokeScalarRound(FRandomBetween(-.1f, .1f)), random_y, random_z);
            spark->v.v[0] = SmokeScalarRound((double)temp.v[0] + spark->v.v[0]);
            spark->v.v[1] = SmokeScalarRound((double)temp.v[1] + spark->v.v[1]);
            new_z = (double)temp.v[2] + spark->v.v[2];
            spark->v.v[2] = SmokeScalarRound(new_z);
            float_time = SmokeScalarRound((double)pTime);
            damping = 1.0 - sqrt(((double)spark->v.v[0] * spark->v.v[0]
                + (double)spark->v.v[1] * spark->v.v[1]) + new_z * spark->v.v[2])
                * .7142857313156128f * float_time * .001f;
            /* The original AH bit test also clamps unordered results. */
            if (!(damping >= .1f)) {
                damping = .1f;
            }
            for (j = 0; j < 3; j++) {
                spark->v.v[j] = SmokeScalarRound((double)spark->v.v[j] * damping);
            }
        }
        if (gNo_2d_effects) {
            SetLineModelCols(spark->colour);
        }
        DrawLine3D(&view_end, &view_start, pScreen, pDepth,
            spark->colour ? gFog_shade_table : gAcid_shade_table);
    }
    if (!replay) {
        AREndPipingSession();
    }
    if (gNo_2d_effects) {
        BrActorRemove(gLine_actor);
        BrActorAdd(gDont_render_actor, gLine_actor);
    }
}

// CreateSingleSpark

// CreateSparks

// CreateSparkShower

// AdjustSpark

// AdjustShrapnel

// FUNCTION: CARMA2_HW 0x004f87a0
void C2_HOOK_FASTCALL ResetSparks(void) {
    gSpark_flags = 0;
}

// ResetShrapnel

// CreateShrapnelShower2

// CreateShrapnelShowerWorld

// CreateShrapnelShower

// random

// InitShrapnel

// LoadInShrapnel

// KillShrapnel

// DisposeShrapnel

// ReplayShrapnel

// ActorFunks

// STUB: CARMA2_HW 0x004f8ca0
void C2_HOOK_FASTCALL MasterDisableCarFunks(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004f8e10
void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// SetPixelmap

// STUB: CARMA2_HW 0x004f8f30
void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// DoSolidGraniteThing

// DoTrueColModelThing

// SetFoxModelCallback

// ResetFoxModelCallback

// ResetFoxLighting

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

// RestorePixelmap

// STUB: CARMA2_HW 0x004f9620
void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

/* The original loads scale_x before multiplying raw size. A signaling NaN
 * in size must remain a memory operand; paired physical probes demonstrate
 * that loading it first changes the chosen NaN sign/payload. */
static double C2_HOOK_CDECL SplashScaleProduct(const tSplash* pSplash) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pSplash
        fld dword ptr [eax + 0x18]
        fmul dword ptr [eax + 0x14]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1; fmuls %2; fstpl %0"
        : "=m"(result) : "m"(pSplash->scale_x), "m"(pSplash->size) : "st");
#else
#error SplashScaleProduct requires the original x86 floating-point behavior
#endif
    return result;
}

// FUNCTION: CARMA2_HW 0x004f9790
void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime) {
    tPhysics_object* object;
    tCar_spec* car;
    tPedestrian* ped;
    tReplay_splash* replay_splash;
    tShrapnel* shrapnel;
    tSplash* splash;
    tPipe_chunk_data* chunk;
    br_actor* actor;
    br_matrix34* matrix;
    br_vector3 velocity;
    br_vector3 omega;
    const char* identifier;
    double interval;
    double scale;
    double damping;
    br_scalar length;
    tU32 time;
    tU32 bit;
    tU32 owner;
    int type;
    int i;
    int j;

    MungeSmokeColumn(pTime);
    if (gNum_splash_types != 0) {
        if (gAction_replay_mode && SplashReplayMoving()) {
            for (type = 1; type <= 3; type++) {
                for (i = 0; ; i++) {
                    if (i >= (type == 0 ? 1 : GetCarCount((tVehicle_type)type))) {
                        break;
                    }
                    car = type == 0 ? &gProgram_state.current_car : GetCarSpec((tVehicle_type)type, i);
                    object = car->collision_info;
                    if (SmokeScalarBits(&object->water_d) != 0x461c4000u
                        && (car == NULL || car->driver != eDriver_local_human)) {
                        GeneralCreateSplash(object, &object->v, &object->omega, &object->water_normal,
                            object->water_d, &object->bb1, &object->actor->t.t.mat, pTime);
                    }
                }
            }
            object = gProgram_state.current_car.collision_info;
            if (SmokeScalarBits(&object->water_d) != 0x461c4000u) {
                CreateSplash(object, pTime);
            }
            for (i = 0; i < 10; i++) {
                if (!((tU32)gReplay_splash_flags & (1u << i))) {
                    continue;
                }
                replay_splash = &gReplay_splashes[i];
                matrix = NULL;
                chunk = NULL;
                if (replay_splash->type == 1) {
                    actor = replay_splash->object.actor;
                    identifier = actor->identifier;
                    owner = (tU32)((((tS8)identifier[4] * 10 + (tS8)identifier[5]) * 10
                        + (tS8)identifier[6]) * 10 + (tS8)identifier[7] - 53328);
                    chunk = FindNextChunk(ePipe_chunk_non_car, owner);
                    if (chunk != NULL) {
                        matrix = &actor->t.t.mat;
                        GetVelocitiesFromMatrices(&velocity, &omega, matrix, &chunk->data.non_car.matrix, pTime);
                    }
                } else if (replay_splash->type == 2) {
                    ped = replay_splash->object.character->ped;
                    actor = GetCharacterActorPtr(replay_splash->object.character);
                    matrix = &actor->t.t.mat;
                    owner = (tU32)((tS32)((tU32)(uintptr_t)ped - (tU32)(uintptr_t)gPedestrian_array) / 84)
                        | ((tU32)(tS16)ped->field_0x06 << 16);
                    chunk = FindNextChunk(ePipe_chunk_ped_physics, owner);
                    if (chunk != NULL) {
                        GetVelocitiesFromMatrices(&velocity, &omega, matrix, &chunk->data.ped_physics.matrix, pTime);
                    }
                } else if (replay_splash->type == 255) {
                    actor = replay_splash->object.phil_object->actor;
                    chunk = FindNextChunk(ePipe_chunk_phil_object, (uintptr_t)replay_splash->object.phil_object);
                    if (chunk != NULL) {
                        matrix = &actor->t.t.mat;
                        GetVelocitiesFromMatrices(&velocity, &omega, matrix, &chunk->data.phil_object.matrix, pTime);
                    }
                }
                if (chunk != NULL) {
                    GeneralCreateSplash(NULL, &velocity, &omega, &replay_splash->normal,
                        replay_splash->water_density, &replay_splash->bb, matrix, pTime);
                }
            }
        } else if (!gAction_replay_mode) {
            for (object = PHILGetFirstObject(); object != NULL; object = PHILGetNextObject(object)) {
                if (SmokeScalarBits(&object->water_d) != 0x461c4000u && object->last_special_volume != NULL
                    && !(object->last_special_volume->gravity_multiplier >= 1.f)) {
                    time = pTime;
                    if (object->owner != NULL && object->flags_0x238 == 1
                        && ((tCar_spec*)object->owner)->driver == eDriver_local_human) {
                        time = 100;
                    }
                    CreateSplash(object, time);
                }
            }
        }
        if (gSplash_flags != 0) {
            for (i = 0; i < 32; i++) {
                bit = 1u << i;
                if (!((tU32)gSplash_flags & bit)) {
                    continue;
                }
                splash = &gSplash[i];
                if (splash->just_done == 0 && (!gAction_replay_mode || SplashReplayMoving())) {
                    gSplash_flags = (int)((tU32)gSplash_flags & ~bit);
                    if (splash->actor->parent != NULL) {
                        BrActorRemove(splash->actor);
                    }
                } else {
                    scale = SplashScaleProduct(splash);
                    for (j = 0; j < 3; j++) {
                        splash->actor->t.t.mat.m[0][j] = SmokeScalarRound(scale * gCamera_to_world.m[0][j]);
                    }
                    for (j = 0; j < 3; j++) {
                        splash->actor->t.t.mat.m[1][j] = SmokeScalarRound((double)splash->size * gCamera_to_world.m[1][j]);
                    }
                    for (j = 0; j < 3; j++) {
                        splash->actor->t.t.mat.m[2][j] = SmokeScalarRound((double)splash->size * gCamera_to_world.m[2][j]);
                    }
                    if (gProgram_state.cockpit_on) {
                        length = SmokeScalarRound(sqrt((double)gCamera_to_world.m[0][0] * gCamera_to_world.m[0][0]
                            + (double)gCamera_to_world.m[0][2] * gCamera_to_world.m[0][2]));
                        DRMatrix34PreRotateZ(&splash->actor->t.t.mat,
                            (br_angle)(0u - FastScalarArcTan2Angle(gCamera_to_world.m[0][1], length)));
                    }
                    splash->just_done = 0;
                }
            }
        }
    }
    if (gAFE) {
        time = GetTotalTime();
        if (time - gNext_AFE_color_cycle <= 100u) {
            gAFE_color_cycle = 0;
        } else {
            gAFE_color_cycle = 1;
            gNext_AFE_color_cycle = time;
        }
    }
    if (gAction_replay_mode) {
        for (i = 0; i < 30; i++) {
            actor = gShrapnel[i].actor;
            matrix = &actor->t.t.mat;
            if ((tU32)gShrapnel_flags & (1u << i)) {
                gShrapnel[i].age = (tU32)SparkFtol((double)ARGetReplayRate() * (tS32)pTime + (tS32)gShrapnel[i].age);
                DrMatrix34Rotate(matrix, (br_angle)((tU16)gShrapnel[i].age * 182), &gShrapnel[i].axis);
                BrMatrix34PreShearX(matrix, gShrapnel[i].shear1, gShrapnel[i].shear2);
            }
        }
        return;
    }
    ARStartPipingSession(ePipe_chunk_shrapnel);
    for (i = 0; i < 30; i++) {
        shrapnel = &gShrapnel[i];
        actor = shrapnel->actor;
        matrix = &actor->t.t.mat;
        bit = 1u << i;
        if (!((tU32)gShrapnel_flags & bit)) {
            continue;
        }
        if (shrapnel->age == 0xffffffffu) {
            if (actor->parent != NULL) {
                BrActorRemove(actor);
            }
            gShrapnel_flags = (int)((tU32)gShrapnel_flags & ~bit);
            continue;
        }
        if (shrapnel->time_sync != 0) {
            interval = (double)shrapnel->time_sync * .001f;
            shrapnel->time_sync = 0;
        } else {
            interval = (double)pTime * .001f;
            shrapnel->age += pTime;
        }
        for (j = 0; j < 3; j++) {
            matrix->m[3][j] = SmokeScalarRound((double)shrapnel->v.v[j] * interval + matrix->m[3][j]);
        }
        shrapnel->v.v[1] = SmokeScalarRound((double)shrapnel->v.v[1]
            - (double)(pTime * 10u) * .00014492752961814404f);
        DrMatrix34Rotate(matrix, (br_angle)((tU16)shrapnel->age * 182), &shrapnel->axis);
        BrMatrix34PreShearX(matrix, shrapnel->shear1, shrapnel->shear2);
        damping = 1.0 - sqrt(((double)shrapnel->v.v[0] * shrapnel->v.v[0]
            + (double)shrapnel->v.v[1] * shrapnel->v.v[1]) + (double)shrapnel->v.v[2] * shrapnel->v.v[2])
            * .7142857313156128f * (double)pTime * .001f;
        if (!(damping >= .1f)) {
            damping = .1f;
        }
        for (j = 0; j < 3; j++) {
            shrapnel->v.v[j] = SmokeScalarRound((double)shrapnel->v.v[j] * damping);
        }
        owner = (tU32)i + ((shrapnel->age > 1000u || shrapnel->age < pTime) ? 0x8000u : 0u);
        AddShrapnelToPipingSession((int)owner, (const br_vector3*)matrix->m[3],
            (tU16)(shrapnel->age - pTime), shrapnel->actor->material);
        if (shrapnel->age > 1000u) {
            shrapnel->age = 0xffffffffu;
        }
    }
    AREndPipingSession();
}


// FUNCTION: CARMA2_HW 0x004f9e80
void C2_HOOK_FASTCALL DrMatrix34Rotate(br_matrix34* pMat, br_angle pAngle, const br_vector3* pAxis) {
    br_scalar scratch[9];
    const br_scalar one = 1.f;

    scratch[4] = SmokeScalarRound(FastScalarSinAngle(pAngle));
    scratch[3] = SmokeScalarRound(FastScalarCosAngle(pAngle));
    /* Retain original intermediate spills, live operands and source reloads
     * while writing only the nine rotation components. */
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov esi, pMat
        mov eax, pAxis
        lea ecx, scratch
        fld one
        fsub dword ptr [ecx + 0xc]
        fld dword ptr [eax]
        fld dword ptr [eax + 8]
        fxch st(2)
        fst dword ptr [ecx + 0x8]
        fmul dword ptr [eax]
        fld dword ptr [eax + 4]
        fld dword ptr [eax + 8]
        fxch st(3)
        fst dword ptr [ecx + 0x20]
        fmul dword ptr [ecx + 0x20]
        fld dword ptr [eax + 4]
        fxch st(3)
        fstp dword ptr [ecx + 0x20]
        fxch st(4)
        fmul dword ptr [ecx + 0x20]
        fxch st(1)
        fmul dword ptr [ecx + 0x20]
        fxch st(3)
        fmul dword ptr [ecx + 0x10]
        fxch st(2)
        fmul dword ptr [ecx + 0x10]
        fxch st(4)
        fmul dword ptr [ecx + 0x8]
        fxch st(1)
        fst dword ptr [ecx + 0x18]
        fxch st(3)
        fst dword ptr [ecx + 0x20]
        fxch st(2)
        fst dword ptr [ecx + 0x14]
        fadd dword ptr [ecx + 0x20]
        fxch st(2)
        fsub dword ptr [ecx + 0x14]
        fxch st(4)
        fstp dword ptr [ecx + 0x14]
        fxch st(2)
        fsub dword ptr [ecx + 0x14]
        fld dword ptr [ecx + 0x10]
        fxch st(3)
        fadd dword ptr [ecx + 0xc]
        fld dword ptr [eax + 8]
        fmul dword ptr [eax + 4]
        fmul dword ptr [ecx + 0x8]
        fstp dword ptr [ecx + 0x10]
        fxch st(3)
        fmul dword ptr [eax]
        fxch st(3)
        fstp dword ptr [esi]
        fxch st(1)
        fstp dword ptr [esi + 4]
        fstp dword ptr [esi + 8]
        fxch st(1)
        fstp dword ptr [esi + 0xc]
        fld dword ptr [eax + 4]
        fst dword ptr [ecx + 0x20]
        fmul dword ptr [ecx + 0x20]
        fxch st(1)
        fstp dword ptr [ecx + 0x20]
        fmul dword ptr [ecx + 0x8]
        fadd dword ptr [ecx + 0xc]
        fstp dword ptr [esi + 0x10]
        fld dword ptr [ecx + 0x20]
        fld dword ptr [ecx + 0x14]
        fld dword ptr [ecx + 0x10]
        fxch st(2)
        fadd dword ptr [ecx + 0x10]
        fxch st(1)
        fadd dword ptr [ecx + 0x18]
        fxch st(2)
        fsub dword ptr [ecx + 0x20]
        fxch st(1)
        fstp dword ptr [esi + 0x14]
        fxch st(1)
        fstp dword ptr [esi + 0x18]
        fstp dword ptr [esi + 0x1c]
        fld dword ptr [eax + 8]
        fld st(0)
        fmul st(0), st(1)
        fmul dword ptr [ecx + 0x8]
        fadd dword ptr [ecx + 0xc]
        fstp dword ptr [esi + 0x20]
        fstp st(0)
    }
#elif defined(__GNUC__) && defined(__i386__)
    {
        br_scalar* scratch_ptr = scratch;
        const br_scalar* one_ptr = &one;
        __asm__ volatile (
            "movl %[matrix], %%esi\n\t"
            "movl %[axis], %%eax\n\t"
            "movl %[scratch], %%ecx\n\t"
            "movl %[one], %%edx\n\t"
            ".intel_syntax noprefix\n\t"
            "fld dword ptr [edx]\n\t"
            "fsub dword ptr [ecx + 0xc]\n\t"
            "fld dword ptr [eax]\n\t"
            "fld dword ptr [eax + 8]\n\t"
            "fxch st(2)\n\t"
            "fst dword ptr [ecx + 0x8]\n\t"
            "fmul dword ptr [eax]\n\t"
            "fld dword ptr [eax + 4]\n\t"
            "fld dword ptr [eax + 8]\n\t"
            "fxch st(3)\n\t"
            "fst dword ptr [ecx + 0x20]\n\t"
            "fmul dword ptr [ecx + 0x20]\n\t"
            "fld dword ptr [eax + 4]\n\t"
            "fxch st(3)\n\t"
            "fstp dword ptr [ecx + 0x20]\n\t"
            "fxch st(4)\n\t"
            "fmul dword ptr [ecx + 0x20]\n\t"
            "fxch st(1)\n\t"
            "fmul dword ptr [ecx + 0x20]\n\t"
            "fxch st(3)\n\t"
            "fmul dword ptr [ecx + 0x10]\n\t"
            "fxch st(2)\n\t"
            "fmul dword ptr [ecx + 0x10]\n\t"
            "fxch st(4)\n\t"
            "fmul dword ptr [ecx + 0x8]\n\t"
            "fxch st(1)\n\t"
            "fst dword ptr [ecx + 0x18]\n\t"
            "fxch st(3)\n\t"
            "fst dword ptr [ecx + 0x20]\n\t"
            "fxch st(2)\n\t"
            "fst dword ptr [ecx + 0x14]\n\t"
            "fadd dword ptr [ecx + 0x20]\n\t"
            "fxch st(2)\n\t"
            "fsub dword ptr [ecx + 0x14]\n\t"
            "fxch st(4)\n\t"
            "fstp dword ptr [ecx + 0x14]\n\t"
            "fxch st(2)\n\t"
            "fsub dword ptr [ecx + 0x14]\n\t"
            "fld dword ptr [ecx + 0x10]\n\t"
            "fxch st(3)\n\t"
            "fadd dword ptr [ecx + 0xc]\n\t"
            "fld dword ptr [eax + 8]\n\t"
            "fmul dword ptr [eax + 4]\n\t"
            "fmul dword ptr [ecx + 0x8]\n\t"
            "fstp dword ptr [ecx + 0x10]\n\t"
            "fxch st(3)\n\t"
            "fmul dword ptr [eax]\n\t"
            "fxch st(3)\n\t"
            "fstp dword ptr [esi]\n\t"
            "fxch st(1)\n\t"
            "fstp dword ptr [esi + 4]\n\t"
            "fstp dword ptr [esi + 8]\n\t"
            "fxch st(1)\n\t"
            "fstp dword ptr [esi + 0xc]\n\t"
            "fld dword ptr [eax + 4]\n\t"
            "fst dword ptr [ecx + 0x20]\n\t"
            "fmul dword ptr [ecx + 0x20]\n\t"
            "fxch st(1)\n\t"
            "fstp dword ptr [ecx + 0x20]\n\t"
            "fmul dword ptr [ecx + 0x8]\n\t"
            "fadd dword ptr [ecx + 0xc]\n\t"
            "fstp dword ptr [esi + 0x10]\n\t"
            "fld dword ptr [ecx + 0x20]\n\t"
            "fld dword ptr [ecx + 0x14]\n\t"
            "fld dword ptr [ecx + 0x10]\n\t"
            "fxch st(2)\n\t"
            "fadd dword ptr [ecx + 0x10]\n\t"
            "fxch st(1)\n\t"
            "fadd dword ptr [ecx + 0x18]\n\t"
            "fxch st(2)\n\t"
            "fsub dword ptr [ecx + 0x20]\n\t"
            "fxch st(1)\n\t"
            "fstp dword ptr [esi + 0x14]\n\t"
            "fxch st(1)\n\t"
            "fstp dword ptr [esi + 0x18]\n\t"
            "fstp dword ptr [esi + 0x1c]\n\t"
            "fld dword ptr [eax + 8]\n\t"
            "fld st(0)\n\t"
            "fmul st(0), st(1)\n\t"
            "fmul dword ptr [ecx + 0x8]\n\t"
            "fadd dword ptr [ecx + 0xc]\n\t"
            "fstp dword ptr [esi + 0x20]\n\t"
            "fstp st(0)\n\t"
            ".att_syntax prefix\n\t"
            :
            : [matrix] "m" (pMat), [axis] "m" (pAxis), [scratch] "m" (scratch_ptr), [one] "m" (one_ptr)
            : "eax", "ecx", "edx", "esi", "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)");
    }
#else
#error DrMatrix34Rotate requires the original x86 floating-point behavior
#endif
}


// FUNCTION: CARMA2_HW 0x004fb2f0
int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2) {
    const tBRender_smoke* a;
    const tBRender_smoke* b;

    a = *(tBRender_smoke* const*)p1;
    b = *(tBRender_smoke* const*)p2;
    /* The original x87 comparison also orders unordered values first. */
    if (!(a->pos.v[2] >= b->pos.v[2])) {
        return -1;
    }
    if (a->pos.v[2] > b->pos.v[2]) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f9fc0
void C2_HOOK_FASTCALL InitSmokeStuff(void) {
    int i;
    char name[256];
    br_pixelmap* map;

    C2_HOOK_BUG_ON(sizeof(tBRender_smoke) != 36);
    gBlend_model = BrModelAllocate("gBlend_model", 4, 2);
    gBlend_model2 = BrModelAllocate("gBlend_model2", 6, 4);
    gBlend_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    for (i = 0; i < (int)REC2_ASIZE(gBR_smoke_structs); i++) {
        gBR_smoke_structs[i].material = BrMaterialAllocate("some smoke");
        if (gBR_smoke_structs[i].material == NULL) {
            break;
        }
    }
    if (gBlend_model == NULL || i != REC2_ASIZE(gBR_smoke_structs) || gBlend_actor == NULL) {
        FatalError(kFatalError_OOM_S);
    }
    gBlend_actor->identifier = "gBlend_actor";
    gBlend_actor->model = gBlend_model;
    gBlend_model->faces[0].vertices[0] = 0;
    gBlend_model->faces[0].vertices[1] = 1;
    gBlend_model->faces[0].vertices[2] = 2;
    gBlend_model->faces[1].vertices[0] = 2;
    gBlend_model->faces[1].vertices[1] = 3;
    gBlend_model->faces[1].vertices[2] = 0;
    gBlend_model2->faces[0].vertices[0] = 0;
    gBlend_model2->faces[0].vertices[1] = 1;
    gBlend_model2->faces[0].vertices[2] = 2;
    gBlend_model2->faces[1].vertices[0] = 2;
    gBlend_model2->faces[1].vertices[1] = 3;
    gBlend_model2->faces[1].vertices[2] = 0;
    gBlend_model2->faces[2].vertices[0] = 1;
    gBlend_model2->faces[2].vertices[1] = 5;
    gBlend_model2->faces[2].vertices[2] = 2;
    gBlend_model2->faces[3].vertices[0] = 1;
    gBlend_model2->faces[3].vertices[1] = 4;
    gBlend_model2->faces[3].vertices[2] = 5;
    BrVector3Set(&gBlend_model->vertices[0].p, -1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model->vertices[1].p, -1.f, -1.f, 0.f);
    BrVector3Set(&gBlend_model->vertices[2].p, 1.f, -1.f, 0.f);
    BrVector3Set(&gBlend_model->vertices[3].p, 1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[0].p, -1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[1].p, -1.f, 0.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[2].p, 1.f, 0.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[3].p, 1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[4].p, -1.f, -.25f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[5].p, 1.f, -.25f, 0.f);
    gBlend_model->flags |= BR_MODF_KEEP_ORIGINAL;
    gBlend_model2->flags |= BR_MODF_KEEP_ORIGINAL;
    strcpy(name, "SMOKE.PIX");
    map = LoadPixelmap(name);
    if (map == NULL) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, name);
    }
    map->map = gRender_palette;
    BrMapAdd(map);
    for (i = 0; i < (int)REC2_ASIZE(gBR_smoke_structs); i++) {
        gBR_smoke_structs[i].material->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_PERSPECTIVE;
        gBR_smoke_structs[i].material->extra_prim = gSmoke_material_tokens;
        gBR_smoke_structs[i].material->colour_map = map;
        BrMaterialAdd(gBR_smoke_structs[i].material);
    }
    /* Round once after the original x87 division and subtraction. */
    BrVector2Set(&gBlend_model->vertices[0].map, 0.f, (br_scalar)(1.0 - 1.0 / map->height));
    BrVector2Set(&gBlend_model->vertices[1].map, 0.f, 0.f);
    BrVector2Set(&gBlend_model->vertices[2].map, (br_scalar)(1.0 - 1.0 / map->width), 0.f);
    BrVector2Set(&gBlend_model->vertices[3].map, (br_scalar)(1.0 - 1.0 / map->width), (br_scalar)(1.0 - 1.0 / map->height));
    BrVector2Set(&gBlend_model2->vertices[0].map, 0.f, (br_scalar)(1.0 - 1.0 / map->height));
    BrVector2Set(&gBlend_model2->vertices[1].map, 0.f, .5f);
    BrVector2Set(&gBlend_model2->vertices[2].map, (br_scalar)(1.0 - 1.0 / map->width), .5f);
    BrVector2Set(&gBlend_model2->vertices[3].map, (br_scalar)(1.0 - 1.0 / map->width), (br_scalar)(1.0 - 1.0 / map->height));
    BrVector2Set(&gBlend_model2->vertices[4].map, 0.f, 0.f);
    BrVector2Set(&gBlend_model2->vertices[5].map, (br_scalar)(1.0 - 1.0 / map->width), 0.f);
    BrModelAdd(gBlend_model);
    BrModelAdd(gBlend_model2);
    BrActorAdd(gDont_render_actor, gBlend_actor);
}

// FUNCTION: CARMA2_HW 0x004fb1b0
void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void) {
    int i;
    int j;
    tBRender_smoke* smoke;
    br_model* model;
    tU8 red;
    tU8 green;
    tU8 blue;

    BrQsort(gBR_smoke_pointers, gN_BR_smoke_structs, sizeof(gBR_smoke_pointers[0]), CmpSmokeZ);
    for (i = gN_BR_smoke_structs - 1; i >= 0; i--) {
        smoke = gBR_smoke_pointers[i];
        BrVector3Copy(&gBlend_actor->t.t.translate.t, &smoke->pos);
        /* The original copies this radius with MOV before dividing it. */
        memcpy(&gBlend_actor->t.t.mat.m[0][0], &smoke->r, sizeof(br_scalar));
        gBlend_actor->t.t.mat.m[1][1] = smoke->r / smoke->aspect;
        gBlend_actor->material = smoke->material;
        /* Preserve extended multiplication and __ftol's low 32 result bits. */
        smoke->material->extra_prim[1].v.x = (br_fixed_ls)(br_int_64)((double)smoke->strength * 150.0 * 65536.0);
        BrMaterialUpdate(smoke->material, BR_MATU_EXTRA_PRIM);
        red = BR_RED(smoke->col);
        green = BR_GRN(smoke->col);
        blue = BR_BLU(smoke->col);
        model = gBR_smoke_pointers[i]->model;
        for (j = 0; j < model->nvertices; j++) {
            model->vertices[j].red = red;
            model->vertices[j].grn = green;
            model->vertices[j].blu = blue;
        }
        BrModelUpdate(model, BR_MODU_VERTEX_COLOURS);
        gBlend_actor->model = model;
        BrZbsSceneRenderAdd(gBlend_actor);
    }
}

/* Force the original float spills even with MSVC5's x87 value reuse. */
static br_scalar C2_HOOK_CDECL SmokeScalarRound(double pValue) {
    volatile union { br_scalar scalar; tU32 bits; } stored;
    union { br_scalar scalar; tU32 bits; } result;

    stored.scalar = (br_scalar)pValue;
    result.bits = stored.bits;
    return result.scalar;
}

/* Preserve raw copied scalar bits across the originally inlined helper calls. */
static tU32 C2_HOOK_CDECL SmokeScalarBits(const br_scalar* pValue) {
    tU32 bits;

    memcpy(&bits, pValue, sizeof(bits));
    return bits;
}

/* These smoke drawing helpers were inlined in the original hardware build. */
static void C2_HOOK_CDECL RecordSmokeCircle(br_vector3* pCent, tU32 pRadius_bits, tU32 pStrength_bits, br_colour pShade, tU32 pAspect_bits) {
    tBRender_smoke* smoke;

    DRMatrix34TApplyP(&gBR_smoke_structs[gN_BR_smoke_structs].pos, pCent,
        gRendering_mirror ? &gRearview_camera_to_world : &gCamera_to_world);
    smoke = &gBR_smoke_structs[gN_BR_smoke_structs];
    memcpy(&smoke->r, &pRadius_bits, sizeof(pRadius_bits));
    memcpy(&smoke->strength, &pStrength_bits, sizeof(pStrength_bits));
    smoke->col = pShade;
    memcpy(&smoke->aspect, &pAspect_bits, sizeof(pAspect_bits));
    gBR_smoke_pointers[gN_BR_smoke_structs] = smoke;
    smoke->model = pShade == (br_colour)gShade_list[7] ? gBlend_model2 : gBlend_model;
    gN_BR_smoke_structs++;
}

static void C2_HOOK_CDECL SmokeCircle3D(br_vector3* pCent, tU32 pRadius_bits, tU32 pStrength_bits, tU32 pAspect_bits, br_pixelmap* pScreen, br_pixelmap* pDepth, br_colour pShade, br_actor* pCamera) {
    (void)pScreen;
    (void)pDepth;
    (void)pCamera;
    if (gNo_2d_effects) {
        RecordSmokeCircle(pCent, pRadius_bits, pStrength_bits, pShade, pAspect_bits);
    }
}

static void C2_HOOK_CDECL DrawTheGlow(br_pixelmap* pScreen, br_pixelmap* pDepth, br_actor* pCamera) {
    int i;
    tU32 seed;
    br_scalar strength;
    br_scalar aspect;
    br_colour shade;
    br_vector3 pos;

    if (gColumn_flags == 0) {
        return;
    }
    seed = rand();
    srand(GetTotalTime());
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1u << i)) != 0 && gSmoke_column[i].colour <= 1) {
            strength = .5f;
            if (gSmoke_column[i].lifetime < 4000) {
                strength = SmokeScalarRound((double)gSmoke_column[i].lifetime * .5f * .00025f);
            }
            memcpy(&pos.v[0], &gSmoke_column[i].pos.v[0], sizeof(br_scalar));
            pos.v[1] = SmokeScalarRound((double)gSmoke_column[i].pos.v[1] + .02);
            memcpy(&pos.v[2], &gSmoke_column[i].pos.v[2], sizeof(br_scalar));
            shade = gShade_list[7];
            aspect = SmokeScalarRound(SRandomBetween(.5f, .99f));
            SmokeCircle3D(&pos, 0x3d8f5c29, SmokeScalarBits(&strength), SmokeScalarBits(&aspect), pScreen, pDepth, shade, pCamera);
        }
    }
    srand(seed);
}

static void C2_HOOK_CDECL ReplaySmoke(br_pixelmap* pScreen, br_pixelmap* pDepth, br_actor* pCamera) {
    int i;
    br_scalar aspect;
    br_scalar radius;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke); i++) {
        if ((gSmoke_flags & (1u << i)) != 0) {
            aspect = SmokeScalarRound(1.0 - ((double)gSmoke[i].radius - .05f) * 4.f * -.5);
            if ((gSmoke[i].type & 0x10) != 0) {
                radius = SmokeScalarRound((double)gSmoke[i].radius / aspect);
                SmokeCircle3D(&gSmoke[i].pos, SmokeScalarBits(&radius),
                    SmokeScalarBits(&gSmoke[i].strength), 0x3f800000, pScreen, pDepth, gShade_list[gSmoke[i].type & 0xf], pCamera);
            } else {
                SmokeCircle3D(&gSmoke[i].pos, SmokeScalarBits(&gSmoke[i].radius), SmokeScalarBits(&gSmoke[i].strength),
                    SmokeScalarBits(&aspect), pScreen, pDepth, gShade_list[gSmoke[i].type & 0xf], pCamera);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fa5b0
void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime) {
    br_vector3 pos;
    br_vector3 v;
    volatile br_vector3 vcs;
    volatile br_vector3 tv;
    volatile br_scalar decay_factor;
    br_scalar alpha;
    double beta;
    double speed_squared;
    int colour;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, dust_time, 0x1284);
    if (pTime < (tU32)pCar->dust_time[pWheel]) {
        pCar->dust_time[pWheel] -= pTime;
        return;
    }
    pCar->dust_time[pWheel] += IRandomBetween(200, 400) - pTime;
    if (pCar->dust_time[pWheel] < 0) {
        pCar->dust_time[pWheel] = 0;
    }
    tv.v[0] = SmokeScalarRound((double)pCar->collision_info->omega.v[1] * pCar->wpos[pWheel].v[2] - (double)pCar->collision_info->omega.v[2] * pCar->wpos[pWheel].v[1]);
    tv.v[1] = SmokeScalarRound((double)pCar->collision_info->omega.v[2] * pCar->wpos[pWheel].v[0] - (double)pCar->collision_info->omega.v[0] * pCar->wpos[pWheel].v[2]);
    tv.v[2] = SmokeScalarRound((double)pCar->collision_info->omega.v[0] * pCar->wpos[pWheel].v[1] - (double)pCar->collision_info->omega.v[1] * pCar->wpos[pWheel].v[0]);
    vcs.v[0] = SmokeScalarRound((double)pCar->collision_info->velocity_car_space.v[0] * WORLD_SCALE);
    vcs.v[1] = SmokeScalarRound((double)pCar->collision_info->velocity_car_space.v[1] * WORLD_SCALE);
    vcs.v[0] = SmokeScalarRound((double)vcs.v[0] + tv.v[0]);
    vcs.v[1] = SmokeScalarRound((double)vcs.v[1] + tv.v[1]);
    vcs.v[2] = SmokeScalarRound((double)pCar->collision_info->velocity_car_space.v[2] * WORLD_SCALE + tv.v[2]);
    speed_squared = (double)vcs.v[0] * vcs.v[0] + (double)vcs.v[1] * vcs.v[1] + (double)vcs.v[2] * vcs.v[2];
    if (!(speed_squared >= 25.f)) {
        return;
    }
    decay_factor = SmokeScalarRound(sqrt(speed_squared) * (double).04f);
    if (decay_factor > 1.f) {
        decay_factor = 1.f;
    }
    tv.v[0] = SmokeScalarRound((double)pCar->wpos[pWheel].v[0] * .14492753148078918f);
    tv.v[1] = SmokeScalarRound((double)pCar->wpos[pWheel].v[1] * .14492753148078918f - (double)pCar->oldd[pWheel] * .14492753623188406);
    tv.v[2] = SmokeScalarRound((double)pCar->wpos[pWheel].v[2] * .14492753148078918f);
    alpha = -1000.f;
    if (!(vcs.v[2] >= 0.f)) {
        alpha = SmokeScalarRound(((double)pCar->collision_info->bb2.max.v[2] - tv.v[2]) / vcs.v[2]);
    } else if (vcs.v[2] > 0.f) {
        alpha = SmokeScalarRound(((double)pCar->collision_info->bb2.min.v[2] - tv.v[2]) / vcs.v[2]);
    }
    beta = -1000.f;
    if (!(vcs.v[0] >= 0.f)) {
        beta = ((double)pCar->collision_info->bb2.max.v[0] - tv.v[0]) / vcs.v[0];
    } else if (vcs.v[0] > 0.f) {
        beta = ((double)pCar->collision_info->bb2.min.v[0] - tv.v[0]) / vcs.v[0];
    }
    if (alpha > beta) {
        beta = alpha;
    }
    pos.v[0] = SmokeScalarRound(vcs.v[0] * beta);
    pos.v[1] = SmokeScalarRound(vcs.v[1] * beta);
    pos.v[2] = SmokeScalarRound(vcs.v[2] * beta);
    BrVector3Accumulate(&tv, &pos);
    BrMatrix34ApplyP(&pos, (const br_vector3*)&tv, &pCar->car_master_actor->t.t.mat);
    BrMatrix34ApplyV(&v, (const br_vector3*)&vcs, &pCar->car_master_actor->t.t.mat);
    colour = gDust_rotate + gCurrent_race.material_modifiers[pCar->material_index[pWheel]].smoke_type - 2;
    while (colour >= gNum_dust_tables) {
        colour -= gNum_dust_tables;
    }
    CreatePuffOfSmoke(&pos, &v, decay_factor, decay_factor + decay_factor, colour + 8);
}

// FUNCTION: CARMA2_HW 0x004fa910
void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    int i;
    int j;
    int component;
    tU32 seed;
    tU32 not_lonely;
    double time_scale;
    double radius_sum;
    union { double scalar; tU32 words[2]; } radius_squared;
    double damping;
    br_scalar aspect;
    br_scalar radius;

    (void)pCamera_to_world;
    not_lonely = 0;
    if (gNo_2d_effects) {
        gBlend_actor->render_style = BR_RSTYLE_FACES;
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(pCamera, gBlend_actor);
        gN_BR_smoke_structs = 0;
    }
    DrawTheGlow(pRender_screen, pDepth_buffer, pCamera);
    if (gSmoke_flags == 0) {
        if (gNo_2d_effects) {
            if (gBlend_actor->parent != NULL) {
                BrActorRemove(gBlend_actor);
            }
            BrActorAdd(gDont_render_actor, gBlend_actor);
        }
        return;
    }
    seed = rand();
    if (!gAction_replay_mode) {
        ARStartPipingSession(ePipe_chunk_smoke);
        for (i = 0; i < (int)REC2_ASIZE(gSmoke); i++) {
            if ((gSmoke_flags & (1u << i)) != 0) {
                if (gSmoke[i].strength > 0.f) {
                    time_scale = (double)(gSmoke[i].time_sync != 0 ? gSmoke[i].time_sync : pTime) * .001f;
                    if (gSmoke[i].time_sync != 0) {
                        gSmoke[i].time_sync = 0;
                    }
                    for (component = 0; component < 3; component++) {
                        gSmoke[i].pos.v[component] = SmokeScalarRound((double)gSmoke[i].v.v[component] * time_scale + gSmoke[i].pos.v[component]);
                    }
                } else {
                    gSmoke_flags &= ~(1u << i);
                }
            }
        }
        for (i = 0; i < (int)REC2_ASIZE(gSmoke); i++) {
            if ((gSmoke_flags & (1u << i)) != 0) {
                if ((gSmoke[i].type & 0xf) == 7) {
                    not_lonely |= 1u << i;
                } else if ((not_lonely & (1u << i)) == 0) {
                    /* The original optimized the self-position subtraction to zero. */
                    for (j = i + 1; j < (int)REC2_ASIZE(gSmoke); j++) {
                        if ((gSmoke_flags & (1u << j)) != 0) {
                            radius_sum = (double)gSmoke[i].radius + gSmoke[j].radius;
                            radius_squared.scalar = radius_sum * radius_sum;
                            /* MSVC5 otherwise folds !<= into ordered > for NaNs. */
                            if (((radius_squared.words[1] & 0x7ff00000) == 0x7ff00000
                                    && ((radius_squared.words[1] & 0x000fffff) != 0 || radius_squared.words[0] != 0))
                                || radius_squared.scalar > 0.f) {
                                not_lonely |= (1u << i) | (1u << j);
                                break;
                            }
                        }
                    }
                }
                if ((not_lonely & (1u << i)) == 0) {
                    gSmoke[i].strength = SmokeScalarRound((double)gSmoke[i].strength * .5f);
                }
                aspect = SmokeScalarRound(1.0 - ((double)gSmoke[i].radius - .05f) * 4.f * -.5);
                if ((gSmoke[i].type & 0x10) != 0) {
                    radius = SmokeScalarRound((double)gSmoke[i].radius / aspect);
                    SmokeCircle3D(&gSmoke[i].pos, SmokeScalarBits(&radius),
                        SmokeScalarBits(&gSmoke[i].strength), 0x3f800000, pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
                } else {
                    SmokeCircle3D(&gSmoke[i].pos, SmokeScalarBits(&gSmoke[i].radius), SmokeScalarBits(&gSmoke[i].strength),
                        SmokeScalarBits(&aspect), pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
                }
                if (gSmoke[i].pipe_me) {
                    AddSmokeToPipingSession(i, gSmoke[i].type, &gSmoke[i].pos, gSmoke[i].radius, gSmoke[i].strength);
                }
                gSmoke[i].radius = SmokeScalarRound((double)pTime * .001f * gSmoke[i].strength * .5000000000000001 + gSmoke[i].radius);
                gSmoke[i].strength = SmokeScalarRound((double)gSmoke[i].strength - (double)gSmoke[i].decay_factor * pTime * .001f);
                if (gSmoke[i].radius > .3f) {
                    gSmoke[i].radius = .3f;
                }
                if (gSmoke[i].strength > 0.f) {
                    damping = 1.0 - (double)pTime * .002f;
                    if (!(damping >= .5f)) {
                        damping = .5f;
                    }
                    for (component = 0; component < 3; component++) {
                        gSmoke[i].v.v[component] = SmokeScalarRound((double)gSmoke[i].v.v[component] * damping);
                    }
                    if (!(fabs(gSmoke[i].v.v[1]) >= .43478259444236755f) && (gSmoke[i].type & 0xf) < 7) {
                        if (gSmoke[i].v.v[1] >= 0.f) {
                            gSmoke[i].v.v[1] = .43478259444236755f;
                        } else {
                            gSmoke[i].v.v[1] = SmokeScalarRound((double)gSmoke[i].v.v[1] + .43478259444236755f);
                        }
                    }
                } else {
                    gSmoke_flags &= ~(1u << i);
                }
            }
        }
        AREndPipingSession();
    } else {
        ReplaySmoke(pRender_screen, pDepth_buffer, pCamera);
    }
    srand(seed);
    if (gNo_2d_effects) {
        RenderRecordedSmokeCircles();
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(gDont_render_actor, gBlend_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004fb330
void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType) {
    double x;
    double y;
    double z;
    br_scalar rounded_x;
    br_scalar rounded_y;
    volatile tSmoke* smoke;
    tU32 strength_bits;

    memcpy(&strength_bits, &pStrength, sizeof(strength_bits));

    C2_HOOK_BUG_ON(sizeof(tSmoke) != 44);
    if (!gSmoke_on) {
        return;
    }
    x = (double)pPos->v[0] - gProgram_state.current_car.pos.v[0];
    y = (double)pPos->v[1] - gProgram_state.current_car.pos.v[1];
    z = (double)pPos->v[2] - gProgram_state.current_car.pos.v[2];
    rounded_x = SmokeScalarRound(x);
    rounded_y = SmokeScalarRound(y);
    if (x * rounded_x + y * rounded_y + z * z > 625.f) {
        if (gCar_to_view == &gProgram_state.current_car) {
            return;
        }
        x = (double)pPos->v[0] - gCar_to_view->pos.v[0];
        y = (double)pPos->v[1] - gCar_to_view->pos.v[1];
        z = (double)pPos->v[2] - gCar_to_view->pos.v[2];
        rounded_x = SmokeScalarRound(x);
        rounded_y = SmokeScalarRound(y);
        if (x * rounded_x + y * rounded_y + z * z > 625.f) {
            return;
        }
    }
    smoke = &gSmoke[gSmoke_next];
    smoke->v.v[0] = SmokeScalarRound((double)pV->v[0] * .14492753148078918f);
    smoke->v.v[1] = SmokeScalarRound((double)pV->v[1] * .14492753148078918f);
    smoke->v.v[2] = SmokeScalarRound((double)pV->v[2] * .14492753148078918f);
    smoke->v.v[1] = SmokeScalarRound((double)smoke->v.v[1] + .14492753623188406);
    memcpy((void*)&smoke->pos.v[0], &pPos->v[0], sizeof(br_scalar));
    memcpy((void*)&smoke->pos.v[1], &pPos->v[1], sizeof(br_scalar));
    memcpy((void*)&smoke->pos.v[2], &pPos->v[2], sizeof(br_scalar));
    smoke->radius = .05f;
    if ((pType & 0xf) == 7) {
        smoke->radius = .1f;
    } else {
        smoke->pos.v[1] = SmokeScalarRound((double)smoke->pos.v[1] + .04f);
    }
    smoke->pos.v[1] = SmokeScalarRound((double)smoke->pos.v[1] + .04f);
    if (pStrength > 1.f) {
        strength_bits = 0x3f800000;
    }
    /* The original x87 load/store quiets signaling NaNs, retaining sign/payload. */
    if ((strength_bits & 0x7fc00000) == 0x7f800000 && (strength_bits & 0x003fffff) != 0) {
        strength_bits |= 0x00400000;
    }
    memcpy((void*)&smoke->strength, &strength_bits, sizeof(strength_bits));
    gSmoke_flags |= 1u << gSmoke_next;
    smoke->time_sync = gPHIL_mechanics_time_sync;
    smoke->type = (tU8)pType;
    memcpy((void*)&smoke->decay_factor, &pDecay_factor, sizeof(pDecay_factor));
    smoke->pipe_me = 1;
    gSmoke_next++;
    if (gSmoke_next >= (int)REC2_ASIZE(gSmoke)) {
        gSmoke_next = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fb510
void C2_HOOK_FASTCALL ResetSmoke(void) {
    gSmoke_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004fb520
void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    gSmoke[pIndex].type = pType;
    memcpy(&gSmoke[pIndex].radius, &pRadius, sizeof(pRadius));
    memcpy(&gSmoke[pIndex].strength, &pStrength, sizeof(pStrength));
    memcpy(&gSmoke[pIndex].pos.v[0], &pPos->v[0], sizeof(br_scalar));
    memcpy(&gSmoke[pIndex].pos.v[1], &pPos->v[1], sizeof(br_scalar));
    memcpy(&gSmoke[pIndex].pos.v[2], &pPos->v[2], sizeof(br_scalar));
    gSmoke_flags |= 1u << pIndex;
}

// ActorError

// FUNCTION: CARMA2_HW 0x004fb580
void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    tSmoke_column* column;
    tU32 bit;

    bit = 1u << (pIndex & 31);
    gColumn_flags ^= bit;
    column = &gSmoke_column[pIndex];
    /* Four separate MOVs preserve the original forward overlap ordering. */
    column->core.field_0x0 = pCore->field_0x0;
    column->core.car = pCore->car;
    column->core.collision_info = pCore->collision_info;
    column->core.car_actor = pCore->car_actor;
    column->vertex_index = pVertex;
    column->colour = pColour;
    column->frame_count[0] = 100;
    column->frame_count[1] = 100;
    column->frame_count[2] = 100;
    if (pColour == 0) {
        if ((gColumn_flags & bit) != 0) {
            BrActorAdd(gNon_track_actor, column->flame_actor);
        } else if (column->flame_actor->depth != 0) {
            BrActorRemove(column->flame_actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fb630
void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime) {
    tSmoke_column* old_column;
    tSmoke_column* column;
    tSmoke_column_core core;
    br_actor* flame;
    int i;

    old_column = &gSmoke_column[gColumn_next];
    if (pObject == NULL) {
        pObject = pCar == NULL ? NULL : pCar->collision_info;
    }
    core.field_0x0 = pType;
    core.car = pCar;
    core.collision_info = pObject;
    core.car_actor = pActor;
    if (!gSmoke_on) {
        return;
    }
    if ((gColumn_flags & (1u << (gColumn_next & 31))) != 0) {
        ARStartPipingSession((tPipe_chunk_type)17);
        column = &gSmoke_column[gColumn_next];
        AddSmokeColumnToPipingSession(gColumn_next, &column->core, column->vertex_index, column->colour);
        AREndPipingSession();
        if (gSmoke_column[gColumn_next].colour == 0) {
            ARStartPipingSession((tPipe_chunk_type)18);
            flame = old_column->flame_actor->children;
            for (i = 0; i < 3; i++) {
                if (flame->type == 1) {
                    AddFlameToPipingSession((gColumn_next << 4) + i, old_column->frame_count[i],
                        old_column->scale_x[i], old_column->scale_y[i], old_column->offset_x[i], old_column->offset_z[i]);
                }
                flame->type = 0;
                flame = flame->next;
            }
            AREndPipingSession();
        }
    }
    if (pColour == 0) {
        if ((gColumn_flags & (1u << (gColumn_next & 31))) == 0 || gSmoke_column[gColumn_next].colour != 0) {
            BrActorAdd(gNon_track_actor, gSmoke_column[gColumn_next].flame_actor);
        }
    } else if ((gColumn_flags & (1u << (gColumn_next & 31))) != 0 && gSmoke_column[gColumn_next].colour == 0) {
        BrActorRemove(gSmoke_column[gColumn_next].flame_actor);
    }
    ARStartPipingSession((tPipe_chunk_type)17);
    AddSmokeColumnToPipingSession(gColumn_next, &core, pVertex, pColour);
    AREndPipingSession();
    column = &gSmoke_column[gColumn_next];
    column->core = core;
    column->colour = pColour;
    column->lifetime = pLifetime;
    column->time = 0;
    column->smudge_timer = 1000;
    column->vertex_index = pVertex;
    column->upright = 1;
    gColumn_flags |= 1u << (gColumn_next & 31);
    column->frame_count[0] = 100;
    column->frame_count[1] = 100;
    column->frame_count[2] = 100;
    gColumn_next++;
    if (gColumn_next >= 10) {
        gColumn_next = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fb890
void C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec* pCar, int pColour, int pVertex, tU32 pLifetime) {
    tSpecial_volume* volume;

    volume = pCar->collision_info->last_special_volume;
    if (volume == NULL || volume->gravity_multiplier >= 1.0f) {
        SmudgeCar(pCar, pVertex);
        if (pCar->knackered) {
            SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 11)]);
        }
        CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pColour, pVertex, pLifetime);
    }
}

// FUNCTION: CARMA2_HW 0x004fb910
void C2_HOOK_FASTCALL GenerateSmokeShades(void) {
    // GLOBAL: CARMA2_HW 0x006aa5b4
    static int rb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5b8
    static int gb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5bc
    static int bb = 0;
    // GLOBAL: CARMA2_HW 0x00660160
    static int rl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660164
    static int gl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660168
    static int bl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660178
    static int rs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066017c
    static int gs = 0xc8;
    // GLOBAL: CARMA2_HW 0x00660180
    static int bs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066016c
    static int rm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660170
    static int gm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660174
    static int bm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660184
    static int rd = 0xfe;
    // GLOBAL: CARMA2_HW 0x00660188
    static int gd = 0xdf;
    // GLOBAL: CARMA2_HW 0x0066018c
    static int bd = 0x43;

    gShade_list[0] = BR_COLOUR_RGB(rb, gb, bb);
    gShade_list[1] = BR_COLOUR_RGB(rl, gl, bl);
    gShade_list[2] = BR_COLOUR_RGB(rm, gm, bm);
    gShade_list[3] = BR_COLOUR_RGB(rs, gs, bs);
    gShade_list[4] = gShade_list[3];
    gShade_list[7] = BR_COLOUR_RGB(rd, gd, bd);
}

// FUNCTION: CARMA2_HW 0x004fb9c0
void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void) {
    if (gIt_shade_table == NULL) {
        gIt_shade_table = GenerateDarkenedShadeTable(16, gRender_palette, 0, 255, 254, .25f, .5f, .75f, .6f);
    }
}

// FUNCTION: CARMA2_HW 0x004fba00
void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    tSmoke_column* column;
    int flame;
    int i;
    tU32 bits[4];
    br_scalar* dest[4];

    column = &gSmoke_column[pIndex >> 4];
    flame = pIndex & 15;
    column->frame_count[flame] = pFrame;
    memcpy(&bits[0], &pScale_x, 4);
    memcpy(&bits[1], &pScale_y, 4);
    memcpy(&bits[2], &pOffset_x, 4);
    memcpy(&bits[3], &pOffset_z, 4);
    dest[0] = &column->scale_x[flame];
    dest[1] = &column->scale_y[flame];
    dest[2] = &column->offset_x[flame];
    dest[3] = &column->offset_z[flame];
    for (i = 0; i < 4; i++) {
        /* Original FLD/FSTP stores quiet sNaNs, preserving sign and payload. */
        if ((bits[i] & 0x7fc00000u) == 0x7f800000u && (bits[i] & 0x003fffffu) != 0) {
            bits[i] |= 0x00400000u;
        }
        memcpy(dest[i], &bits[i], 4);
    }
}

// ReplayFlame

// FUNCTION: CARMA2_HW 0x004fbdd0
void C2_HOOK_FASTCALL FlameAnimate(int pColumn, const br_vector3* pPos, tU32 pTime) {
    tSmoke_column* column;
    br_actor* actor;
    br_actor* flame;
    br_pixelmap* map;
    tPhysics_object* physics;
    tU32 bits;
    int i;
    int width;
    int height;
    int direction;
    br_scalar scale;
    br_scalar sx;
    br_scalar sy;
    double speed_squared;
    int can_start;
    typedef void (C2_HOOK_FASTCALL* raw_flame_fn)(int, int, tU32, tU32, tU32, tU32);
    volatile uintptr_t record_address;

    column = &gSmoke_column[pColumn];
    actor = column->flame_actor;
    DRMatrix34RotateY(&actor->t.t.mat, FastScalarArcTan2Angle(gCamera_to_world.m[2][0], gCamera_to_world.m[2][2]));
    memcpy(&actor->t.t.mat.m[3][0], &pPos->v[0], sizeof(br_scalar));
    memcpy(&actor->t.t.mat.m[3][1], &pPos->v[1], sizeof(br_scalar));
    bits = SmokeScalarBits(&pPos->v[2]);
    flame = actor->children;
    memcpy(&actor->t.t.mat.m[3][2], &bits, sizeof(bits));
    if (gAction_replay_mode) {
        for (i = 0; i < 3; i++) {
            column->frame_count[i] = (tS32)(tU32)(br_int_64)((double)ARGetReplayRate() + column->frame_count[i]);
            if (column->frame_count[i] >= 0 && column->frame_count[i] < 20) {
                flame->type = 1;
                flame->material->colour_map = gFlame_map[column->frame_count[i]];
                BrMaterialUpdate(flame->material, 0x7fff);
                map = gFlame_map[column->frame_count[i]];
                height = (tU16)map->height;
                width = (tU16)map->width;
                sy = SmokeScalarRound((double)height * column->scale_y[i]);
                sx = SmokeScalarRound((double)width * column->scale_x[i]);
                BrMatrix34Scale(&flame->t.t.mat, sx, sy, 1.0f);
                memcpy(&flame->t.t.mat.m[3][0], &column->offset_x[i], sizeof(br_scalar));
                memcpy(&flame->t.t.mat.m[3][2], &column->offset_z[i], sizeof(br_scalar));
            } else {
                flame->type = 0;
            }
            flame = flame->next;
        }
        return;
    }
    record_address = (uintptr_t)AddFlameToPipingSession;
    for (i = 0; i < 3; i++) {
        column->frame_count[i] = (tS32)((tU32)column->frame_count[i] + 1u);
        if (column->frame_count[i] >= 20) {
            ARStartPipingSession((tPipe_chunk_type)18);
            ((raw_flame_fn)record_address)((pColumn << 4) + i, (tS32)((tU32)column->frame_count[i] + 1u), SmokeScalarBits(&column->scale_x[i]), SmokeScalarBits(&column->scale_y[i]), SmokeScalarBits(&column->offset_x[i]), SmokeScalarBits(&column->offset_z[i]));
            AREndPipingSession();
            column->frame_count[i] = IRandomBetween(-5, -1);
            scale = SmokeScalarRound(SRandomBetween(1.0f, 1.5f) * .003000000026077032f);
            direction = IRandomBetween(0, 1) * 2 - 1;
            column->scale_x[i] = SmokeScalarRound((double)direction * scale);
            column->scale_y[i] = SmokeScalarRound(SRandomBetween(.5f, 1.0f) * .003000000026077032f);
            column->offset_x[i] = SmokeScalarRound(SRandomPosNeg(.03f));
            column->offset_z[i] = SmokeScalarRound(SRandomBetween(-.03f, 0.0f));
            flame->type = 0;
        }
        if (column->frame_count[i] == 0) {
            can_start = 1;
            if (column->core.car != NULL) {
                physics = column->core.car->collision_info;
                speed_squared = (double)physics->v.v[0] * physics->v.v[0] + (double)physics->v.v[1] * physics->v.v[1];
                speed_squared += (double)physics->v.v[2] * physics->v.v[2];
                can_start = !(speed_squared >= 11.594202898550725);
            }
            if (can_start && pTime * 30u < column->lifetime) {
                flame->type = 1;
                ARStartPipingSession((tPipe_chunk_type)18);
                ((raw_flame_fn)record_address)((pColumn << 4) + i, (tS32)((tU32)column->frame_count[i] - 1u), SmokeScalarBits(&column->scale_x[i]), SmokeScalarBits(&column->scale_y[i]), SmokeScalarBits(&column->offset_x[i]), SmokeScalarBits(&column->offset_z[i]));
                AREndPipingSession();
            } else {
                column->frame_count[i] = -5;
            }
        }
        if (column->frame_count[i] >= 0) {
            flame->material->colour_map = gFlame_map[column->frame_count[i]];
            BrMaterialUpdate(flame->material, 0x7fff);
            height = gFlame_frame_size[column->frame_count[i]][1];
            width = gFlame_frame_size[column->frame_count[i]][0];
            sy = SmokeScalarRound((double)height * column->scale_y[i]);
            sx = SmokeScalarRound((double)width * column->scale_x[i]);
            BrMatrix34Scale(&flame->t.t.mat, sx, sy, 1.0f);
            memcpy(&flame->t.t.mat.m[3][0], &column->offset_x[i], sizeof(br_scalar));
            memcpy(&flame->t.t.mat.m[3][2], &column->offset_z[i], sizeof(br_scalar));
        }
        flame = flame->next;
    }
}

// FUNCTION: CARMA2_HW 0x004fc0e0
void C2_HOOK_FASTCALL DoSmokeColumn(int pColumn, tU32 pTime, br_vector3* pRet_car_pos) {
    tSmoke_column* column;
    br_actor* actor;
    br_model* volatile* model_slot;
    br_matrix34* local_matrix;
    volatile int* vertex_index;
    const br_vector3* vertex;
    br_scalar value;
    tU32 bits;
    int i;

    (void)pTime;
    column = &gSmoke_column[pColumn];
    vertex_index = &column->vertex_index;
    if (column->core.field_0x0 == 1) {
        if (column->core.car->car_master_actor->t.t.mat.m[1][1] > .1f) {
            column->upright = 1;
        }
        if (!(column->core.car->car_master_actor->t.t.mat.m[1][1] >= -.1f)) {
            column->upright = 0;
        }
        actor = column->core.car->car_actor;
        model_slot = &actor->model;
        local_matrix = &actor->t.t.mat;
        for (i = 0; i < 3; i++) {
            vertex = &(*model_slot)->prepared->groups[0].vertices[*vertex_index].p;
            value = SmokeScalarRound((double)vertex->v[i] + actor->t.t.mat.m[3][i]);
            bits = SmokeScalarBits(&value);
            memcpy(&pRet_car_pos->v[i], &bits, sizeof(bits));
        }
    } else {
        column->upright = 1;
        model_slot = &column->core.car_actor->model;
        local_matrix = (br_matrix34*)((char*)pRet_car_pos + 0x2c);
        for (i = 0; i < 3; i++) {
            vertex = &(*model_slot)->prepared->groups[0].vertices[*vertex_index].p;
            memcpy(&pRet_car_pos->v[i], &vertex->v[i], sizeof(br_scalar));
        }
    }
    if (column->core.field_0x0 == 1 && gProgram_state.cockpit_on && column->core.car != NULL && column->core.car->driver == 8) {
        /* MSVC5's inverse branch for a mixed-width > admits unordered values;
         * the original FCOM/test 0x41 sends them to the subtraction branch. */
        if ((SmokeScalarBits(&column->core.car->driver_z_offset) & 0x7fffffffu) <= 0x7f800000u
                && (SmokeScalarBits(&pRet_car_pos->v[2]) & 0x7fffffffu) <= 0x7f800000u
                && (double)column->core.car->driver_z_offset - (-.2) > pRet_car_pos->v[2]) {
            vertex = &(*model_slot)->prepared->groups[0].vertices[*vertex_index].p;
            BrMatrix34ApplyP(pRet_car_pos, vertex, local_matrix);
        } else {
            pRet_car_pos->v[1] = SmokeScalarRound((double)pRet_car_pos->v[1] - .07);
        }
    }
    if (column->upright == 0) {
        pRet_car_pos->v[1] = SmokeScalarRound((double)column->core.car->collision_info->shape->common.bb.min.v[1] * .14492753148078918f);
    }
    BrMatrix34ApplyP(&column->pos, pRet_car_pos, &column->core.car_actor->t.t.mat);
    column->pos.v[1] = SmokeScalarRound((double)column->pos.v[1] - .03f);
}

// ReplaySmokeColumn

// FUNCTION: CARMA2_HW 0x004fba40
void C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime) {
    tSmoke_column* column;
    tCar_spec* car;
    tPhysics_object* physics;
    br_vector3 car_pos;
    br_vector3 pos;
    br_vector3 velocity;
    br_scalar strength;
    br_scalar linear_x;
    br_scalar linear_y;
    double random;
    tU32 elapsed;
    tU32 bit;
    int colour;
    int i;

    if (gColumn_flags == 0) {
        return;
    }
    if (gAction_replay_mode) {
        for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
            if (gColumn_flags & (1u << i)) {
                DoSmokeColumn(i, pTime, &car_pos);
                if (gSmoke_column[i].colour == 0) {
                    FlameAnimate(i, &gSmoke_column[i].pos, pTime);
                }
            }
        }
        return;
    }
    gPHIL_mechanics_time_sync = 1;
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        bit = 1u << i;
        if ((gColumn_flags & bit) == 0) {
            continue;
        }
        column = &gSmoke_column[i];
        if (column->lifetime < pTime) {
            if (column->core.field_0x0 != 0) {
                ARStartPipingSession((tPipe_chunk_type)17);
                AddSmokeColumnToPipingSession(i, &column->core, column->vertex_index, column->colour);
                AREndPipingSession();
            }
            gColumn_flags &= ~bit;
            if (column->colour == 0) {
                BrActorRemove(column->flame_actor);
            }
            continue;
        }
        column->lifetime -= pTime;
        car = column->core.car;
        physics = column->core.collision_info;
        DoSmokeColumn(i, pTime, &car_pos);
        if (column->core.field_0x0 == 1 && car->car_crush_spec->field_0x4b8 != 0 && IsOnNonCarHalf(car, &car_pos)) {
            column->lifetime = 0;
        }
        if (column->colour == 0) {
            FlameAnimate(i, &column->pos, pTime);
            if (column->core.field_0x0 == 1) {
                if ((tU32)column->smudge_timer < pTime) {
                    column->smudge_timer += 2000u;
                    if (car != NULL) {
                        SmudgeCar(car, column->vertex_index);
                        if (car->knackered) {
                            SmudgeCar(car, car->fire_vertex[IRandomBetween(0, 11)]);
                        }
                    }
                } else {
                    column->smudge_timer -= pTime;
                }
            }
        }
        elapsed = (tU32)column->time + pTime;
        column->time = (tS32)elapsed;
        if (elapsed <= 200u) {
            continue;
        }
        column->time = (tS32)(elapsed - pTime);
        column->count = (tS32)((tU32)column->count + 1u);
        random = SRandomBetween(-.03f, .03f);
        colour = column->colour;
        pos.v[0] = SmokeScalarRound(random + column->pos.v[0]);
        pos.v[1] = SmokeScalarRound((double)column->pos.v[1] - (double)(colour == 0) * -.05f);
        pos.v[2] = SmokeScalarRound(SRandomBetween(-.03f, .03f) + column->pos.v[2]);
        if ((column->whiter & 2) != 0 && IRandomBetween(0, 3) == 0) {
            column->whiter &= 1;
        } else {
            column->whiter = column->whiter > 0 ? 2 : -1;
        }
        strength = SmokeScalarRound(((double)(column->whiter > 0) - (-1.0f)) * .5f);
        if (column->lifetime < 4000u) {
            strength = SmokeScalarRound((double)column->lifetime * strength * .00025f);
        }
        if (column->core.collision_info != NULL) {
            velocity.v[0] = SmokeScalarRound((double)physics->omega.v[1] * car_pos.v[2] - (double)physics->omega.v[2] * car_pos.v[1]);
            velocity.v[1] = SmokeScalarRound((double)physics->omega.v[2] * car_pos.v[0] - (double)physics->omega.v[0] * car_pos.v[2]);
            velocity.v[2] = SmokeScalarRound((double)physics->omega.v[0] * car_pos.v[1] - (double)physics->omega.v[1] * car_pos.v[0]);
            BrMatrix34ApplyV(&car_pos, &velocity, &physics->actor->t.t.mat);
            linear_x = SmokeScalarRound((double)physics->v.v[0] * 6.9);
            linear_y = SmokeScalarRound((double)physics->v.v[1] * 6.9);
            velocity.v[0] = SmokeScalarRound((double)linear_x + car_pos.v[0]);
            velocity.v[1] = SmokeScalarRound((double)linear_y + car_pos.v[1] - (-2.898550724637681));
            velocity.v[2] = SmokeScalarRound((double)physics->v.v[2] * 6.9 + car_pos.v[2]);
        } else {
            BrVector3SetFloat(&velocity, 0.0f, 2.8985507488250732f, 0.0f);
        }
        CreatePuffOfSmoke(&pos, &velocity, strength, strength, (int)((tU32)column->colour + 16u));
    }
}

// FUNCTION: CARMA2_HW 0x004fc2e0
void C2_HOOK_FASTCALL DisposeFlame(void) {
    int i;
    int j;
    br_actor* child;

    for (i = 0; i < (int)REC2_ASIZE(gFlame_map); i++) {
        BrMapRemove(gFlame_map[i]);
        BrPixelmapFree(gFlame_map[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1u << i)) != 0 && gSmoke_column[i].colour == 0) {
            BrActorRemove(gSmoke_column[i].flame_actor);
        }
        child = gSmoke_column[i].flame_actor->children;
        for (j = 0; j < 3; j++) {
            BrMaterialRemove(child->material);
            BrMaterialFree(child->material);
            child = child->next;
        }
        BrActorFree(gSmoke_column[i].flame_actor);
    }
    BrModelRemove(gLollipop_model);
    BrModelFree(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fc3a0
void C2_HOOK_FASTCALL InitFlame(void) {
    int i;
    int j;
    int count;
    char name[256];
    br_actor* child;
    br_material* material;

    gColumn_flags = 0;
    gLollipop_model = BrModelAllocate("Lollipop", 4, 2);
    strcpy(name, "FLAMES.PIX");
    strcpy(gCurrent_load_directory, "COMMON");
    count = LoadTextureTryAllLocations(name, gFlame_map, REC2_ASIZE(gFlame_map));
    if (count != (int)REC2_ASIZE(gFlame_map)) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, name);
    }
    BrMapAddMany(gFlame_map, count);
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        gSmoke_column[i].flame_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        for (j = 0; j < 3; j++) {
            child = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            material = BrMaterialAllocate(NULL);
            BrActorAdd(gSmoke_column[i].flame_actor, child);
            child->model = gLollipop_model;
            child->material = material;
            material->flags &= ~BR_MATF_LIGHT;
            material->flags |= BR_MATF_ALWAYS_VISIBLE;
            material->colour_map = gFlame_map[0];
            GlorifyMaterial(&material, 1, kRendererShadingType_AmbientOnly);
            BrMaterialAdd(material);
            gSmoke_column[i].frame_count[j] = 100;
        }
    }
    gLollipop_model->nvertices = 4;
    BrVector3SetFloat(&gLollipop_model->vertices[0].p, -.5f, 0.f, 0.f);
    BrVector3SetFloat(&gLollipop_model->vertices[1].p, .5f, 0.f, 0.f);
    BrVector3SetFloat(&gLollipop_model->vertices[2].p, .5f, 1.f, 0.f);
    BrVector3SetFloat(&gLollipop_model->vertices[3].p, -.5f, 1.f, 0.f);
    gLollipop_model->vertices[0].map.v[0] = 0.f;
    gLollipop_model->vertices[0].map.v[1] = 1.f;
    gLollipop_model->vertices[1].map.v[0] = 1.f;
    gLollipop_model->vertices[1].map.v[1] = 1.f;
    gLollipop_model->vertices[2].map.v[0] = 1.f;
    gLollipop_model->vertices[2].map.v[1] = 0.f;
    gLollipop_model->vertices[3].map.v[0] = 0.f;
    gLollipop_model->vertices[3].map.v[1] = 0.f;
    gLollipop_model->nfaces = 2;
    gLollipop_model->faces[0].vertices[0] = 0;
    gLollipop_model->faces[0].vertices[1] = 1;
    gLollipop_model->faces[0].vertices[2] = 2;
    gLollipop_model->faces[1].vertices[0] = 0;
    gLollipop_model->faces[1].vertices[1] = 2;
    gLollipop_model->faces[1].vertices[2] = 3;
    gLollipop_model->faces[0].smoothing = 1;
    gLollipop_model->faces[1].smoothing = 1;
    BrModelAdd(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fdde0
void C2_HOOK_FASTCALL InitSplash(FILE* pF) {
    int i;
    int count;
    int loaded;
    int direction;
    br_actor* actor;
    br_pixelmap* maps[20];
    char path[256];
    char name[256];

    gSplash_flags = 0;
    gSplash_model = BrModelAllocate("Splash", 4, 2);
    strcpy(gCurrent_load_directory, "COMMON");
    if (pF != NULL) {
        count = GetAnInt(pF);
        gNum_splash_types = 0;
        for (i = 0; i < count; i++) {
            GetAString(pF, name);
            /* Preserve the original private path call and subsequent overwrite. */
            PathCat(path, path, name);
            strcpy(path, name);
            loaded = LoadTextureTryAllLocations(path, &maps[gNum_splash_types], (int)(20u - (tU32)gNum_splash_types));
            if (loaded == 0) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, path);
            }
            gNum_splash_types = (int)((tU32)gNum_splash_types + (tU32)loaded);
        }
    } else {
        strcpy(path, "SPLSHBLU.PIX");
        gNum_splash_types = LoadTextureTryAllLocations(path, maps, REC2_ASIZE(maps));
    }
    BrMapAddMany(maps, gNum_splash_types);
    for (i = 0; i < gNum_splash_types; i++) {
        gSplash_material[i] = BrMaterialAllocate(NULL);
        gSplash_material[i]->flags &= ~BR_MATF_LIGHT;
        gSplash_material[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        gSplash_material[i]->colour_map = maps[i];
        BrMaterialAdd(gSplash_material[i]);
    }
    gSplash_model->nvertices = 4;
    BrVector3SetFloat(&gSplash_model->vertices[0].p, -.5f, 0.f, 0.f);
    BrVector3SetFloat(&gSplash_model->vertices[1].p, .5f, 0.f, 0.f);
    BrVector3SetFloat(&gSplash_model->vertices[2].p, .5f, 1.f, 0.f);
    BrVector3SetFloat(&gSplash_model->vertices[3].p, -.5f, 1.f, 0.f);
    gSplash_model->vertices[0].map.v[0] = 0.f;
    gSplash_model->vertices[0].map.v[1] = 1.f;
    gSplash_model->vertices[1].map.v[0] = 1.f;
    gSplash_model->vertices[1].map.v[1] = 1.f;
    gSplash_model->vertices[2].map.v[0] = 1.f;
    gSplash_model->vertices[2].map.v[1] = 0.f;
    gSplash_model->vertices[3].map.v[0] = 0.f;
    gSplash_model->vertices[3].map.v[1] = 0.f;
    gSplash_model->nfaces = 2;
    gSplash_model->faces[0].vertices[0] = 0;
    gSplash_model->faces[0].vertices[1] = 1;
    gSplash_model->faces[0].vertices[2] = 2;
    gSplash_model->faces[1].vertices[0] = 0;
    gSplash_model->faces[1].vertices[1] = 2;
    gSplash_model->faces[1].vertices[2] = 3;
    gSplash_model->faces[0].smoothing = 1;
    gSplash_model->faces[1].smoothing = 1;
    BrModelAdd(gSplash_model);
    for (i = 0; i < (int)REC2_ASIZE(gSplash); i++) {
        actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gSplash[i].actor = actor;
        actor->model = gSplash_model;
        if (gNum_splash_types != 0) {
            actor->material = gSplash_material[IRandomBetween(0, gNum_splash_types - 1)];
        } else {
            actor->material = NULL;
        }
        direction = 2 * IRandomBetween(0, 1) - 1;
        gSplash[i].scale_x = SmokeScalarRound(SRandomBetween(.9f, 1.1f) * (br_scalar)direction);
    }
}

// DisposeSplash

// DrawTheGlow

// PipeInstantUnSmudge

// FUNCTION: CARMA2_HW 0x004fc9c0
void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int pFire_point) {
    (void)pCar;
    (void)pFire_point;
}

// FUNCTION: CARMA2_HW 0x004fc9d0
void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    gSmoke_on = pSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fca30
int C2_HOOK_FASTCALL GetSmokeOn(void) {

    return gSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fca40
void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar && gSmoke_column[i].lifetime > 2000u) {
            gSmoke_column[i].lifetime = 2000;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca70
void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca90
void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.collision_info == pObject) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fcab0
void C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int pFire_point, int pColour) {
    int flags;
    int i;
    int vertex;
    tSpecial_volume* volume;

    if (pColour == 0 && (pCar == NULL || (int)pCar->driver < 7)) {
        pColour = 1;
    }
    flags = gColumn_flags;
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((flags & (1u << i)) && gSmoke_column[i].core.car == pCar) {
            break;
        }
    }
    if (i < (int)REC2_ASIZE(gSmoke_column)) {
        for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
            if (gSmoke_column[i].core.car == pCar) {
                if ((flags & (1u << i)) && gSmoke_column[i].colour <= pColour && gSmoke_column[i].lifetime != 0) {
                    return;
                }
                gSmoke_column[i].lifetime = 2000;
            }
        }
    }
    vertex = pCar->fire_vertex[pFire_point];
    volume = pCar->collision_info->last_special_volume;
    if (volume == NULL || volume->gravity_multiplier >= 1.0f) {
        SmudgeCar(pCar, vertex);
        if (pCar->knackered) {
            SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 11)]);
        }
        CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pColour, vertex, 10000000);
    }
}

/* FSUBR selects its live x87 NaN operand before the scalar speed operand. */
static double C2_HOOK_CDECL SplashSubtractFromScalar(double pValue, const br_scalar* pScalar) {
    double result;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov ecx, pScalar
        fld pValue
        fsubr dword ptr [ecx]
        fstp result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile (
        "fldl %[value]\n\t"
        "fsubrs %[scalar]\n\t"
        "fstpl %[result]\n\t"
        : [result] "=m" (result)
        : [value] "m" (pValue), [scalar] "m" (*pScalar)
        : "st", "memory");
#else
#error SplashSubtractFromScalar requires the original x86 floating-point behavior
#endif
    return result;
}

// FUNCTION: CARMA2_HW 0x004fd530
void C2_HOOK_FASTCALL SingleSplash(tPhysics_object* pObject, const br_matrix34* pMatrix, const br_vector3* pVelocity, const br_vector3* pNormal_velocity, const br_vector3* pOmega, const br_vector3* pPoint, const br_vector3* pNormal, tU32 pTime) {
    br_vector3 cross;
    br_vector3 velocity;
    br_vector3 recoil;
    br_scalar scaled;
    br_scalar speed;
    br_scalar size;
    br_scalar range;
    double dot;
    double random;
    double wide;
    tCar_spec* car;
    int index;
    int i;

    BrMatrix34ApplyP((br_vector3*)gSplash[gSplash_next].actor->t.t.mat.m[3], pPoint, pMatrix);
    cross.v[0] = SmokeScalarRound((double)pOmega->v[1] * pPoint->v[2] - (double)pOmega->v[2] * pPoint->v[1]);
    cross.v[1] = SmokeScalarRound((double)pOmega->v[2] * pPoint->v[0] - (double)pOmega->v[0] * pPoint->v[2]);
    cross.v[2] = SmokeScalarRound((double)pOmega->v[0] * pPoint->v[1] - (double)pOmega->v[1] * pPoint->v[0]);
    BrMatrix34ApplyV(&velocity, &cross, pMatrix);
    for (i = 0; i < 3; i++) {
        scaled = SmokeScalarRound((double)pVelocity->v[i] * 6.9);
        velocity.v[i] = SmokeScalarRound((double)velocity.v[i] + scaled);
    }
    speed = SmokeScalarRound(sqrt(((double)velocity.v[0] * velocity.v[0]
        + (double)velocity.v[1] * velocity.v[1]) + (double)velocity.v[2] * velocity.v[2]));
    dot = ((double)pNormal->v[1] * velocity.v[1] + (double)pNormal->v[2] * velocity.v[2])
        + (double)pNormal->v[0] * velocity.v[0];
    wide = SplashSubtractFromScalar(fabs(dot) * -5.f, &speed) * .006666666828095913f - -.04782608523964882f;
    size = SmokeScalarRound(wide);
    if (wide > .5f) {
        size = .5f;
    }
    dot = ((double)pNormal_velocity->v[2] * pPoint->v[2] + (double)pNormal_velocity->v[1] * pPoint->v[1])
        + (double)pNormal_velocity->v[0] * pPoint->v[0];
    if (!(dot >= 0.0)) {
        size = SmokeScalarRound((double)size * .5f);
    }
    index = gSplash_next;
    gSplash[index].size = SmokeScalarRound(SRandomBetween(SmokeScalarRound((double)size * .5f), size));
    if (!((tU32)gSplash_flags & (1u << (gSplash_next & 31)))) {
        BrActorAdd(gNon_track_actor, gSplash[index].actor);
    }
    gSplash_flags = (int)((tU32)gSplash_flags | (1u << (gSplash_next & 31)));
    gSplash[gSplash_next].just_done = 1;
    if (pObject != NULL && !gAction_replay_mode) {
        car = pObject->flags_0x238 == 1 ? (tCar_spec*)pObject->owner : NULL;
        if (car != NULL) {
            random = SRandomBetween(0.f, 1.f);
            if (random < (double)pTime * .003f) {
                range = SmokeScalarRound(sqrt((double)speed * .014285714365541935f) * 15.f);
                for (i = 0; i < 3; i++) {
                    wide = (double)velocity.v[i] * .14492753148078918f;
                    velocity.v[i] = SmokeScalarRound(wide);
                    recoil.v[i] = SmokeScalarRound(wide * -.1f);
                }
                if (range > 15.f) {
                    range = 15.f;
                }
                random = SRandomBetween(5.f, range);
                recoil.v[1] = SmokeScalarRound((double)recoil.v[1] - random * -.14492753623188406);
                BrMatrix34TApplyV(&velocity, &recoil, &pObject->actor->t.t.mat);
                cross.v[0] = SmokeScalarRound(((double)pObject->water_normal.v[2] * velocity.v[1]
                    - (double)pObject->water_normal.v[1] * velocity.v[2]) * .5f);
                cross.v[1] = SmokeScalarRound(((double)pObject->water_normal.v[0] * velocity.v[2]
                    - (double)pObject->water_normal.v[2] * velocity.v[0]) * .5f);
                cross.v[2] = SmokeScalarRound(((double)pObject->water_normal.v[1] * velocity.v[0]
                    - (double)pObject->water_normal.v[0] * velocity.v[1]) * .5f);
                dot = ((double)pPoint->v[1] * cross.v[1] + (double)pPoint->v[2] * cross.v[2])
                    + (double)pPoint->v[0] * cross.v[0];
                for (i = 0; i < 3; i++) {
                    velocity.v[i] = SmokeScalarRound(dot > 0.0 ? (double)velocity.v[i] + cross.v[i]
                        : (double)velocity.v[i] - cross.v[i]);
                }
                index = gSpark_next;
                for (i = 0; i < 3; i++) {
                    memcpy(&gSpark[index].pos.v[i], &pPoint->v[i], 4);
                }
                BrVector3SetFloat(&gSpark[index].normal, 0.f, 0.f, 0.f);
                index = gSpark_next;
                memcpy(&gSpark[index].v, &velocity, sizeof(velocity));
                gSpark[index].count = 500;
                gSpark[index].car = car;
                gSpark[index].time_sync = 1;
                gSpark[index].colour = 1;
                gSpark_flags = (int)((tU32)gSpark_flags | (1u << (index & 31)));
                gSpark_next = (int)((tU32)index + 1u);
                if (gSpark_next >= 32) {
                    gSpark_next = 0;
                }
            }
        }
    }
    gSplash_next = (int)((tU32)gSplash_next + 1u);
    if ((tU32)gSplash_next >= 32u) {
        gSplash_next = 0;
    }
}


#if defined(_MSC_VER)
#if _MSC_VER >= 1300
#pragma warning(push)
#endif
/* The original copies the unwritten private minimum point before assigning
 * its first candidate. Keep that stack-history copy; do not initialize it. */
#pragma warning(disable: 4700)
#endif
// FUNCTION: CARMA2_HW 0x004fcbb0
void C2_HOOK_FASTCALL GeneralCreateSplash(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, const br_vector3* pNormal, br_scalar pWater_distance, const br_bounds3* pBounds, const br_matrix34* pMatrix, tU32 pTime) {
    br_vector3 local_normal;
    br_vector3 world_min;
    br_vector3 local_velocity;
    /* Original spills this projection, but passes local_velocity below. */
    volatile br_vector3 normal_velocity;
    br_vector3 point;
    br_vector3 minimum_point;
    br_vector3 maximum_point;
    br_vector3 span;
    br_vector3 side;
    br_vector3 offset;
    br_bounds3 bounds;
    tCar_spec* car;
    br_scalar plane_distance;
    br_scalar minimum_projection;
    br_scalar maximum_projection;
    br_scalar projection;
    br_scalar length;
    br_scalar near_distance;
    br_scalar far_distance;
    br_scalar height;
    double side_wide[3];
    double wide_height;
    double wide_near;
    double coordinate;
    double wide_projection;
    double lower;
    double upper;
    double dot;
    double value;
    double random;
    double second_random;
    double multiplier;
    int start;
    int axis;
    int a;
    int b;
    int corner;
    int pattern;
    int i;

    value = ((double)pVelocity->v[1] * pVelocity->v[1] + (double)pVelocity->v[2] * pVelocity->v[2])
        + (double)pVelocity->v[0] * pVelocity->v[0];
    if (!(value * 47.61000000000001 >= 1.0)) {
        return;
    }
    BrMatrix34TApplyV(&local_normal, pNormal, pMatrix);
    BrMatrix34ApplyP(&world_min, &pBounds->min, pMatrix);
    lower = ((double)pNormal->v[1] * world_min.v[1] + (double)pNormal->v[2] * world_min.v[2])
        + (double)pNormal->v[0] * world_min.v[0] - pWater_distance;
    upper = lower;
    for (i = 0; i < 3; i++) {
        value = ((double)pBounds->max.v[i] - pBounds->min.v[i]) * local_normal.v[i];
        if (local_normal.v[i] > 0.f) {
            upper += value;
        } else {
            lower += value;
        }
    }
    if (lower * upper > 0.0) {
        return;
    }
    dot = ((double)pVelocity->v[1] * pNormal->v[1] + (double)pVelocity->v[2] * pNormal->v[2])
        + (double)pVelocity->v[0] * pNormal->v[0];
    for (i = 0; i < 3; i++) {
        normal_velocity.v[i] = SmokeScalarRound(dot * local_normal.v[i]);
    }
    (void)&normal_velocity;
    memcpy(&bounds, pBounds, sizeof(bounds));
    bounds.min.v[1] = .01f;
    BrMatrix34TApplyV(&local_velocity, pVelocity, pMatrix);
    plane_distance = SmokeScalarRound((double)pWater_distance
        - (((double)pMatrix->m[3][2] * pNormal->v[2] + (double)pMatrix->m[3][1] * pNormal->v[1])
            + (double)pMatrix->m[3][0] * pNormal->v[0]));
    minimum_projection = 0.f;
    maximum_projection = 0.f;
    start = IRandomBetween(0, 3);
    for (axis = 0; axis < 3; axis++) {
        a = (axis + 1) % 3;
        b = (axis + 2) % 3;
        for (corner = 0; corner < 4; corner++) {
            pattern = start ^ corner;
            point.v[a] = SmokeScalarRound((pattern & 1) ? bounds.min.v[a] : bounds.max.v[a]);
            point.v[b] = SmokeScalarRound((pattern & 2) ? bounds.min.v[b] : bounds.max.v[b]);
            coordinate = (((double)plane_distance - (double)point.v[a] * local_normal.v[a])
                - (double)point.v[b] * local_normal.v[b]) / local_normal.v[axis];
            value = SmokeScalarRound(coordinate);
            if (!(coordinate >= bounds.min.v[axis]) || value > bounds.max.v[axis]) {
                continue;
            }
            point.v[axis] = SmokeScalarRound(value);
            wide_projection = ((double)local_velocity.v[0] * point.v[0] + (double)local_velocity.v[1] * point.v[1])
                + (double)local_velocity.v[2] * point.v[2];
            projection = SmokeScalarRound(wide_projection);
            if (!(wide_projection >= minimum_projection)) {
                if (!(maximum_projection >= 0.f)) {
                    SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &maximum_point, &local_normal, pTime);
                }
                maximum_projection = minimum_projection;
                minimum_projection = projection;
                memcpy(&maximum_point, &minimum_point, sizeof(maximum_point));
                memcpy(&minimum_point, &point, sizeof(minimum_point));
            } else if (!(projection >= maximum_projection)) {
                if (!(maximum_projection >= 0.f)) {
                    SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &maximum_point, &local_normal, pTime);
                }
                maximum_projection = projection;
                memcpy(&maximum_point, &point, sizeof(maximum_point));
            } else {
                SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &point, &local_normal, pTime);
            }
        }
    }
    if (!(maximum_projection >= 0.f)) {
        for (i = 0; i < 3; i++) {
            span.v[i] = SmokeScalarRound((double)maximum_point.v[i] - minimum_point.v[i]);
        }
        random = SRandomBetween(.4f, .6f);
        point.v[0] = SmokeScalarRound((double)minimum_point.v[0] + random * span.v[0]);
        point.v[1] = SmokeScalarRound((double)minimum_point.v[1] + SmokeScalarRound(random * span.v[1]));
        point.v[2] = SmokeScalarRound((double)minimum_point.v[2] + SmokeScalarRound(random * span.v[2]));
        random = SRandomBetween(.2f, .3f);
        for (i = 0; i < 3; i++) {
            minimum_point.v[i] = SmokeScalarRound(random * span.v[i] + minimum_point.v[i]);
        }
        second_random = SRandomBetween(.2f, .3f);
        random = SmokeScalarRound(-second_random);
        dot = -(((double)local_velocity.v[2] * local_normal.v[2] + (double)local_velocity.v[1] * local_normal.v[1])
            + (double)local_velocity.v[0] * local_normal.v[0]);
        for (i = 0; i < 3; i++) {
            side_wide[i] = (double)local_velocity.v[i] + SmokeScalarRound((double)local_normal.v[i] * dot);
            side.v[i] = SmokeScalarRound(side_wide[i]);
        }
        length = SmokeScalarRound(sqrt((side_wide[0] * side.v[0] + side_wide[1] * side.v[1])
            + (double)side.v[2] * side.v[2]));
        if (!(length > 2.384185791015625e-7f)) {
            side.v[0] = 1.f;
            side.v[1] = 0.f;
            side.v[2] = 0.f;
            side_wide[0] = 1.0;
            side_wide[1] = 0.0;
            side_wide[2] = 0.0;
        } else {
            multiplier = 1.0 / length;
            for (i = 0; i < 3; i++) {
                side_wide[i] = multiplier * side.v[i];
                side.v[i] = SmokeScalarRound(side_wide[i]);
                if (i == 2) {
                    side_wide[i] = side.v[i];
                }
            }
        }
        for (i = 0; i < 3; i++) {
            value = side_wide[i] * -.028985507786273956f;
            offset.v[i] = SmokeScalarRound(value * .5f);
            point.v[i] = SmokeScalarRound((double)point.v[i] + SmokeScalarRound(value));
            minimum_point.v[i] = SmokeScalarRound((double)minimum_point.v[i] + offset.v[i]);
            maximum_point.v[i] = SmokeScalarRound((double)maximum_point.v[i]
                + (i == 0 ? -second_random : random) * span.v[i]);
            maximum_point.v[i] = SmokeScalarRound((double)maximum_point.v[i] + offset.v[i]);
        }
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &point, &local_normal, pTime);
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &minimum_point, &local_normal, pTime);
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &maximum_point, &local_normal, pTime);
    } else if (!(minimum_projection >= 0.f)) {
        SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &minimum_point, &local_normal, pTime);
    }
    plane_distance = SmokeScalarRound((double)plane_distance * 6.900000095367432f);
    if (pObject != NULL && pObject->flags_0x238 == 1 && pObject->owner != NULL) {
        car = pObject->owner;
        if (car->driver > 5) {
            for (i = 0; i < 4; i++) {
                value = (i & 1) ? pObject->shape->common.bb.max.v[0] : pObject->shape->common.bb.min.v[0];
                wide_height = (double)pObject->shape->common.bb.max.v[1] * 6.9;
                height = SmokeScalarRound(wide_height);
                wide_near = (double)plane_distance
                    - (((double)car->wpos[i].v[2] * local_normal.v[2] + wide_height * local_normal.v[1])
                        + value * 6.9 * local_normal.v[0]);
                near_distance = SmokeScalarRound(wide_near);
                far_distance = SmokeScalarRound(((double)pObject->shape->common.bb.max.v[1] - .01f)
                    * local_normal.v[1] * 6.900000095367432f + near_distance);
                if ((wide_near > 0.f) != (far_distance > 0.f)) {
                    point.v[0] = SmokeScalarRound(value * 6.9 * .14492753148078918f);
                    point.v[1] = SmokeScalarRound(((double)height - (wide_height - .01)
                        * ((double)near_distance / ((double)near_distance - far_distance))) * .14492753148078918f);
                    point.v[2] = SmokeScalarRound((double)car->wpos[i].v[2] * .14492753148078918f);
                    SingleSplash(pObject, pMatrix, pVelocity, &local_velocity, pOmega, &point, &local_normal, pTime);
                }
            }
        }
    }
}


#if defined(_MSC_VER)
#if _MSC_VER >= 1300
#pragma warning(pop)
#else
#pragma warning(default: 4700)
#endif
#endif

// FUNCTION: CARMA2_HW 0x004fda40
void C2_HOOK_FASTCALL CreateSplash(tPhysics_object* pObject, tU32 pTime) {
    GeneralCreateSplash(pObject, &pObject->v, &pObject->omega, &pObject->water_normal,
        pObject->water_d, &pObject->bb1, &pObject->actor->t.t.mat, pTime);
}


// FUNCTION: CARMA2_HW 0x004fda70
void C2_HOOK_FASTCALL GetVelocitiesFromMatrices(br_vector3* pVelocity, br_vector3* pOmega, const br_matrix34* pMatrix, const br_matrix34* pNext_matrix, tU32 pTime) {
    br_quat current;
    br_quat next;
    br_quat difference;
    br_scalar interval;
    int i;

    pVelocity->v[0] = SmokeScalarRound((double)pNext_matrix->m[3][0] - pMatrix->m[3][0]);
    pVelocity->v[1] = SmokeScalarRound((double)pNext_matrix->m[3][1] - pMatrix->m[3][1]);
    interval = SmokeScalarRound((double)pTime * .001);
    pVelocity->v[2] = SmokeScalarRound((double)pNext_matrix->m[3][2] - pMatrix->m[3][2]);
    for (i = 0; i < 3; i++) {
        pVelocity->v[i] = SmokeScalarRound((double)pVelocity->v[i] / interval);
    }
    /* Both original calls deliberately read the current matrix. */
    BrMatrix34ToQuat(&current, pMatrix);
    BrMatrix34ToQuat(&next, pMatrix);
    BrQuatInvert(&current, &current);
    BrQuatMul(&difference, &current, &next);
    pOmega->v[0] = SmokeScalarRound(((double)difference.x + difference.x) / interval);
    pOmega->v[1] = SmokeScalarRound(((double)difference.y + difference.y) / interval);
    pOmega->v[2] = SmokeScalarRound(((double)difference.z + difference.z) / interval);
}


// MungeSplash

// FUNCTION: CARMA2_HW 0x004fdb70
void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* pF) {
    int i;
    int red;
    int green;
    int blue;
    br_scalar quarter;
    br_scalar half;
    br_scalar three_quarter;

    gNum_dust_tables = GetAnInt(pF);
    if (gNum_dust_tables > 8) {
        gNum_dust_tables = 8;
    }
    for (i = 0; i < gNum_dust_tables; i++) {
        PossibleService();
        GetThreeInts(pF, &red, &green, &blue);
        GetThreeScalars(pF, &quarter, &half, &three_quarter);
        gShade_table[i] = BR_COLOUR_RGB(red, green, blue);
    }
}

// FUNCTION: CARMA2_HW 0x004fdc00
void C2_HOOK_FASTCALL FreeSmokeShadeTables(void) {
}

// LoadInKevStuff

// FUNCTION: CARMA2_HW 0x004fe180
void C2_HOOK_FASTCALL DisposeKevStuff(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gShrapnel); i++) {
        if ((gShrapnel_flags & (1u << i)) != 0 && gShrapnel[i].actor->parent != NULL) {
            BrActorRemove(gShrapnel[i].actor);
        }
        BrActorFree(gShrapnel[i].actor);
    }
    gShrapnel_flags = 0;
    BrModelRemove(gShrapnel_model[0]);
    BrModelRemove(gShrapnel_model[1]);
    BrModelFree(gShrapnel_model[0]);
    BrModelFree(gShrapnel_model[1]);
    DisposeFlame();
    for (i = 0; i < gNum_splash_types; i++) {
        BrMapRemove(gSplash_material[i]->colour_map);
        BrPixelmapFree(gSplash_material[i]->colour_map);
        BrMaterialRemove(gSplash_material[i]);
        BrMaterialFree(gSplash_material[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gSplash); i++) {
        if ((gSplash_flags & (1u << i)) != 0 && gSplash[i].actor->parent != NULL) {
            BrActorRemove(gSplash[i].actor);
        }
        BrActorFree(gSplash[i].actor);
    }
    BrModelRemove(gSplash_model);
    BrModelFree(gSplash_model);
}

// DisposeKevStuffCar

// MakeCarIt

// StopCarBeingIt

// ForEveryModelMaterial

// ForEveryActorMaterial

// ForEveryActorMaterialNoGrooves

// ForEveryCarMaterial

// ForEveryCarModelCB

// ForEveryCarModel

// MightBlendifyMaterial

// BlendifyMaterialCB

// STUB: CARMA2_HW 0x004fecf0
void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// UnBlendifyMaterialCB

// STUB: CARMA2_HW 0x004fed40
void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fed90
int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1u << i)) != 0 && gSmoke_column[i].core.car == pCar) {
            return 1;
        }
    }
    return 0;
}

// ClearSplashReplay

// GetSplashIndex

// ClearSplashes

// AdjustSplashReplay

// InitReplaySplashes
