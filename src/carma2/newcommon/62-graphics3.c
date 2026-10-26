#include "62-graphics3.h"

#include "08-loading1.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "53-controls.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include <limits.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x00655dfc
// GLOBAL: CARMA2_SW 0x00650ce4
int gMini_map_visible = 2;

// GLOBAL: CARMA2_HW 0x0074a780
tCar_icon gCar_icons[128];

// GLOBAL: CARMA2_HW 0x0074ab84
br_actor* gCar_icons_model_actor;

// GLOBAL: CARMA2_HW 0x0068d8c8
int gTarget_lock_enabled;

// GLOBAL: CARMA2_HW 0x0068d8cc
// GLOBAL: CARMA2_SW 0x00687924
int gInitMap_done;

// GLOBAL: CARMA2_HW 0x0074abdc
// GLOBAL: CARMA2_SW 0x00741288
float gHeadup_map_x_float;

// GLOBAL: CARMA2_HW 0x0074abd8
// GLOBAL: CARMA2_SW 0x00741284
float gHeadup_map_y_float;

// GLOBAL: CARMA2_HW 0x0074abcc
// GLOBAL: CARMA2_SW 0x00741278
float gHeadup_map_w_float;

// GLOBAL: CARMA2_HW 0x0074ab80
// GLOBAL: CARMA2_SW 0x00741230
float gHeadup_map_h_float;

// GLOBAL: CARMA2_HW 0x0074ab88
// GLOBAL: CARMA2_SW 0x00741234
int gINT_0074ab88;

// GLOBAL: CARMA2_HW 0x0074ab8c
// GLOBAL: CARMA2_SW 0x00741238
int gINT_0074ab8c;

// GLOBAL: CARMA2_HW 0x0074ab94
// GLOBAL: CARMA2_SW 0x00741240
int gINT_0074ab94;

// GLOBAL: CARMA2_HW 0x0074ab98
// GLOBAL: CARMA2_SW 0x00741244
int gHeadup_map_half_height;

// GLOBAL: CARMA2_HW 0x0074abe4
// GLOBAL: CARMA2_SW 0x00741290
int gHeadup_map_half_width;

// GLOBAL: CARMA2_HW 0x0074abec
// GLOBAL: CARMA2_SW 0x00741298
int gINT_0074abec;

// GLOBAL: CARMA2_HW 0x0068d8d8
br_pixelmap* gMap_overlay;

// GLOBAL: CARMA2_HW 0x0074aba0
// GLOBAL: CARMA2_SW 0x0074124c
int gMini_map_x;

// GLOBAL: CARMA2_HW 0x0074aba4
// GLOBAL: CARMA2_SW 0x00741250
int gMini_map_y;

// GLOBAL: CARMA2_HW 0x0074abd4
// GLOBAL: CARMA2_SW 0x00741280
int gINT_0074abd4;

// GLOBAL: CARMA2_HW 0x0074abd0
// GLOBAL: CARMA2_SW 0x0074127c
int gINT_0074abd0;

// GLOBAL: CARMA2_HW 0x0068d8dc
// GLOBAL: CARMA2_SW 0x00687934
br_pixelmap* gMini_map;

// GLOBAL: CARMA2_HW 0x00655f04
int gINT_00655f04;

// GLOBAL: CARMA2_HW 0x00655f08
int gINT_00655f08;

// GLOBAL: CARMA2_HW 0x0068d8a0
// GLOBAL: CARMA2_SW 0x006878f8
float gFLOAT_0068d8a0;

// GLOBAL: CARMA2_HW 0x0068d8a4
// GLOBAL: CARMA2_SW 0x006878fc
float gFLOAT_0068d8a4;

// GLOBAL: CARMA2_HW 0x0068d8a8
// GLOBAL: CARMA2_SW 0x00687900
float gMini_map_arrow_z;

// GLOBAL: CARMA2_HW 0x0074ab9c
// GLOBAL: CARMA2_SW 0x00741248
float gFLOAT_0074ab9c;

// GLOBAL: CARMA2_HW 0x0074abb8
// GLOBAL: CARMA2_SW 0x00741264
float gFLOAT_0074abb8;

// GLOBAL: CARMA2_HW 0x0074abbc
// GLOBAL: CARMA2_SW 0x00741268
int gMap_render_y_i;

// GLOBAL: CARMA2_HW 0x0074abc0
// GLOBAL: CARMA2_SW 0x0074126c
int gMap_render_x_i;

// GLOBAL: CARMA2_HW 0x0074abc8
// GLOBAL: CARMA2_SW 0x00741274
int gMap_render_height_i;

// GLOBAL: CARMA2_HW 0x0074abe8
// GLOBAL: CARMA2_SW 0x00741294
int gMap_render_width_i;

// GLOBAL: CARMA2_HW 0x0068d6e4
// GLOBAL: CARMA2_SW 0x0068773c
br_pixelmap* gCheckpoint_numbers;

// GLOBAL: CARMA2_HW 0x0068d884
// GLOBAL: CARMA2_SW 0x006878dc
br_pixelmap* gSmashy_dot;

// GLOBAL: CARMA2_HW 0x0068c87c
// GLOBAL: CARMA2_SW 0x006868d4
br_pixelmap* gAlt_meter;

// GLOBAL: CARMA2_HW 0x0068c880
// GLOBAL: CARMA2_SW 0x006868d8
br_matrix23 gMatrix23_0068c880;

// GLOBAL: CARMA2_HW 0x0068c868
// GLOBAL: CARMA2_SW 0x006868c0
br_vector3 gOrigin_map;

// GLOBAL: CARMA2_HW 0x0068d898
// GLOBAL: CARMA2_SW 0x006878f0
br_vector2 gOrigin_headup_map;

// GLOBAL: CARMA2_HW 0x0068c860
// GLOBAL: CARMA2_SW 0x006868b8
int gCheckpoint_digit_height;

// GLOBAL: CARMA2_HW 0x0068d8bc
// GLOBAL: CARMA2_SW 0x00687914
int gCheckpoint_digit_center_x;

// GLOBAL: CARMA2_HW 0x0068c85c
// GLOBAL: CARMA2_SW 0x006868b4
tU32 gCheckpoint_digit_center_y;

// GLOBAL: CARMA2_HW 0x0068d6f8
// GLOBAL: CARMA2_SW 0x00687750
tOppo_status_messages gOppo_status_messages[9];

// GLOBAL: CARMA2_HW 0x0074abe0
// GLOBAL: CARMA2_SW 0x0074128c
int gCount_smashable_race_targets;

// GLOBAL: CARMA2_HW 0x0068c898
// GLOBAL: CARMA2_SW 0x006868f0
tSmashable_race_target gSmashable_race_targets[300];

// CarArrowColour

// ToggleTargetLock

// CycleTargetLock

// CycleMapOptions

// FUNCTION: CARMA2_HW 0x00494880
// FUNCTION: CARMA2_SW 0x00491b80
void C2_HOOK_FASTCALL ToggleMapTransparency(void) {

    gMap_trans = !gMap_trans;
    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x004948a0
void C2_HOOK_FASTCALL InitTargetLock(void) {

    gTarget_lock_enabled = 0;
}

// FUNCTION: CARMA2_HW 0x004948b0
void C2_HOOK_FASTCALL SetIntegerMapRenders(void) {

    gMap_render_width = 128.f;
    gMap_render_height = 80.f;
    gMap_render_x_i = ((int)gMap_render_x) & ~0x3;
    gMap_render_y_i = ((int)gMap_render_y) & ~0x1;
    gMap_render_width_i = (int)gMap_render_width;
    gMap_render_height_i = (int)gMap_render_height;
}

// OppositeColour

// DrawArrow

// DrawMapBlip

// DrawSmashBlip

// DrawMapSmallBlip

// CopyMapToScreen

// DrawCarArrow

// FUNCTION: CARMA2_HW 0x00495ba0
void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev) {
    int i;
    int checkpoint;
    int checkpoints[10];
    int checkpoint_flags;

    C2_HOOK_ASSERT(gCurrent_race.check_point_count <= REC2_ASIZE(checkpoints));
    for (i = 0; i < gCurrent_race.check_point_count; i++) {
        checkpoints[i] = 0;
    }
    if (gNet_mode != eNet_mode_none) {
        if (gCurrent_net_game->type == eNet_game_type_checkpoint) {
            checkpoint_flags = gNet_players[gThis_net_player_index].score2;
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                if (checkpoint_flags & 0x1) {
                    pCallback(pMap, i, pTime, 1);
                    checkpoints[i] = 1;
                }
                checkpoint_flags >>= 1;
            }
        } else if (gCurrent_net_game->type == eNet_game_type_5 || gCurrent_net_game->type == eNet_game_type_4) {
            if (gNet_players[gThis_net_player_index].score2 >= 0) {
                checkpoint = gNet_players[gThis_net_player_index].score2 % gCurrent_race.check_point_count;
                pCallback(pMap, checkpoint, pTime, 1);
                checkpoints[checkpoint] = 1;
            }
        }
    } else {
        pCallback(pMap, gCheckpoint - 1, pTime, 1);
        checkpoints[gCheckpoint - 1] = 1;
    }
    if (pDraw_prev) {
        if (gNet_mode == eNet_mode_none
                || gCurrent_net_game->type == eNet_game_type_checkpoint
                || gCurrent_net_game->type == eNet_game_type_4
                || gCurrent_net_game->type == eNet_game_type_5) {
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                if (!checkpoints[i]) {
                    pCallback(pMap, i, pTime, 0);
                }
            }
        }
    }
}

// CalcMapCheckpoint

// FUNCTION: CARMA2_HW 0x00496940
void C2_HOOK_FASTCALL CalcMapCheckpoint2(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    br_vector3 pos_mapspace;
    br_vector2 p;
    tRace_checkpoint_spec* checkpoint;

    checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
    BrMatrix34ApplyP(&pos_mapspace, &checkpoint->pos, &gCurrent_race.map_transformation);
    BrVector2Set(&p, pos_mapspace.v[0] - gOrigin_map.v[0] + gOrigin_headup_map.v[0], pos_mapspace.v[1] - gOrigin_map.v[1] + gOrigin_headup_map.v[1]);
    BrMatrix23TApplyV(&checkpoint->map_position, &p, &gMatrix23_0068c880);
    checkpoint->map_position.v[0] += (float)gINT_0074abd4;
    checkpoint->map_position.v[1] += (float)gINT_0074abd0;
}

// DrawCheckpoint

// DrawOffsetCheckpoint

// DoMapOverlays

// StartMap

// FinishMap

// FUNCTION: CARMA2_HW 0x00496b10
void C2_HOOK_FASTCALL CheckpointLine(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    int p2_x;
    int p2_y;
    tRace_checkpoint_spec* checkpoint;

    if (pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count) {
        checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
        if (checkpoint->map_position.v[0] < 0.f) {
            p2_x = 8;
            p2_y = (int)((checkpoint->map_position.v[1] - (float)gINT_0074abd0)
                * ((float)gINT_0074abd4 / ((float)gINT_0074abd4 - checkpoint->map_position.v[0]))
                + (float)gINT_0074abd0);
        } else {
            p2_x = (int)(checkpoint->map_position.v[0] + 8.f);
            p2_y = (int)checkpoint->map_position.v[1];
        }
        FancyDrawLine(pMap, gINT_0074abd4 + 8, gINT_0074abd0, p2_x, p2_y, 132, 100);
    }
}

// DoDirectionFinderStuff

// MungeMoodMessage

// DoOpponentStatusHeadup

// AllocateOppoBarActor

// DoNearestOpponentRelativeHeightMarkerStuff

// MapOverlay

// InitNearestCar

// MapStuffBeforeRender

// MapStuffAfterRender

static void C2_HOOK_FASTCALL ReadMoodMessages(void) {
    tPath_name path;
    FILE *f;
    int mood;
    char s[256];

    PathCat(path, gApplication_path, "STATUS.TXT");
    f = PFfopen(path, "rt");
    if (f != NULL) {
        for (mood = 0; mood < (int)REC2_ASIZE(gOppo_status_messages); mood += 1) {
            int i;

            C2_HOOK_ASSERT(mood < REC2_ASIZE(gOppo_status_messages));

            gOppo_status_messages[mood].count = GetAnInt(f);
            C2_HOOK_ASSERT(gOppo_status_messages[mood].count < REC2_ASIZE(gOppo_status_messages[mood].messages));
            for (i = 0; i < gOppo_status_messages[mood].count; i++) {


                GetALineAndDontArgue(f, s);
                gOppo_status_messages[mood].messages[i] = (char *) BrMemAllocate(strlen(s) + 1, kMem_misc_string);
                strcpy(gOppo_status_messages[mood].messages[i], s);
            }
        }
        PFfclose(f);
    } else {
        FatalError(kFatalError_CantOpen_S, "STATUS.TXT");
    }
}

// FUNCTION: CARMA2_HW 0x00496e30
// FUNCTION: CARMA2_SW 0x00493ca0
void C2_HOOK_FASTCALL InitMap(void) {

    if (!gInitMap_done) {
        gHeadup_map_x_float = (float)gHeadup_map_x;
        gHeadup_map_y_float = (float)gHeadup_map_y;
        gHeadup_map_w_float = (float)gHeadup_map_w;
        gHeadup_map_h_float = (float)gHeadup_map_h;
    } else {
        gHeadup_map_x = ((int)gHeadup_map_x_float + 3) & (tU16)~3;
        gHeadup_map_y = (int)gHeadup_map_y_float;
        gHeadup_map_w = ((int)gHeadup_map_w_float + 3) & (tU16)~3;
        gHeadup_map_h = (int)gHeadup_map_h_float;
    }
    if (gHeadup_map_x % 4 != 0) {
        gHeadup_map_x -= gHeadup_map_x % 4;
        gHeadup_map_x_float = (float)gHeadup_map_x;
    }
    if (gHeadup_map_w % 4 != 0) {
        gHeadup_map_w -= gHeadup_map_w % 4;
        gHeadup_map_w_float = (float)gHeadup_map_w;
    }
    if (gHeadup_map_w > 64) {
        gHeadup_map_w = 64;
        gHeadup_map_w_float = 64.0f;
    }
    if (gHeadup_map_h > 64) {
        gHeadup_map_h = 64;
        gHeadup_map_h_float = 64.0f;
    }
    gINT_0074ab8c = (int)(3.0 + 0.3 * gHeadup_map_w) & (tU16)~3;
    gINT_0074ab88 = (int)(3.0 + 0.3 * gHeadup_map_h) & (tU16)~3;
    if (gINT_0074ab8c > gINT_0074ab88) {
        gINT_0074ab88 = gINT_0074ab8c;
    } else {
        gINT_0074ab8c = gINT_0074ab88;
    }
    gINT_0074ab94 = gHeadup_map_w + 2 * gINT_0074ab88;
    gINT_0074abec = gHeadup_map_h + 2 * gINT_0074ab88;
    gHeadup_map_half_width = gHeadup_map_w / 2;
    gHeadup_map_half_height = gHeadup_map_h / 2;
#ifdef REC2_SW
    if (gMap_overlay != NULL) {
        BrPixelmapFree(gMap_overlay);
    }
#endif
    if (gMap_overlay == NULL) {
        gMap_overlay = DRPixelmapAllocate(gBack_screen->type, MAX(gINT_0074abec, gINT_0074ab94), MAX(gINT_0074abec, gINT_0074ab94), NULL, 0);
    }
    gMini_map_x = gHeadup_map_x + gHeadup_map_w / 16;
    gMini_map_y = gHeadup_map_y + gHeadup_map_h / 16;
    if (gMini_map_x % 2 != 0) {
        gMini_map_x -= 1;
    }
    gINT_0074abd4 = gHeadup_map_w / 2 - gMini_map_x + gHeadup_map_x;
    gINT_0074abd0 = gHeadup_map_h / 2 - gMini_map_y + gHeadup_map_y;
    if (gMini_map != NULL) {
        BrPixelmapFree(gMini_map);
    }
#ifdef REC2_HW
    gINT_00655f04 = gHeadup_map_x;
    gINT_00655f08 = gHeadup_map_y;
#endif
    gMini_map = DRPixelmapAllocateSub(gBack_screen,
        gMini_map_x,
        gMini_map_y,
        gHeadup_map_w - (gHeadup_map_w + 8) / 8 + 8,
        gHeadup_map_h - gHeadup_map_h / 8);
    gMini_map->pixels = (br_uint_8*)gMini_map->pixels + gMini_map_y * gMini_map->row_bytes + gMini_map_x;
    gMini_map->base_x = gMini_map_x;
    gMini_map->base_y = 0;
    gFLOAT_0068d8a0 = (float)gINT_0074abd4;
    gFLOAT_0068d8a4 = (float)gINT_0074abd0;
    gMini_map_arrow_z = 0.0f;
    gFLOAT_0074ab9c = (float)((double)(gHeadup_map_half_height + gHeadup_map_half_width) * 0.45);
    gFLOAT_0074ab9c = REC2_SQR(gFLOAT_0074ab9c);
    gFLOAT_0074abb8 = (float)REC2_SQR(gCurrent_graf_data->field_0x51c + 1);
    if (!gInitMap_done) {
        gCheckpoint_numbers = LoadPixelmap("CPNUMB.PIX");
        if (gCheckpoint_numbers == NULL) {
            FatalError(kFatalError_CantFindFile_S, "CPNUMB.PIX");
        }
        BRPM_convert(gCheckpoint_numbers, gBack_screen->type);
        gSmashy_dot = LoadPixelmap("SMASHY.PIX");
        if (gSmashy_dot == NULL) {
            FatalError(kFatalError_CantFindFile_S, "SMASHY.PIX");
        }
        BRPM_convert(gSmashy_dot, gBack_screen->type);
        gAlt_meter = LoadPixelmap("ALTMETER.PIX");
        if (gAlt_meter == NULL) {
            FatalError(kFatalError_CantFindFile_S, "ALTMETER.PIX");
        }
        gCheckpoint_digit_height = gCheckpoint_numbers->height / 10;
        gCheckpoint_digit_center_y = gCheckpoint_digit_height / 2;
        gCheckpoint_digit_center_x = gCheckpoint_numbers->width / 2;
#ifdef REC2_SW
        gIcons_low_pix = LoadPixelmap("CARICONSLO.pix");
        if (gIcons_low_pix != NULL) {
            BrMapAdd(gIcons_low_pix);
        }
#endif
        gIcons_pix = LoadPixelmap("CARICONS.PIX");
        if (gIcons_pix != NULL) {
            BRPM_convert(gIcons_pix, gBack_screen->type);
            BrMapAdd(gIcons_pix);
        }
        ReadMoodMessages();
    }
    gInitMap_done = 1;
}

// FUNCTION: CARMA2_HW 0x004973b0
void C2_HOOK_FASTCALL ConvertCarIcons(br_pixelmap* pMap) {
    int i;
    int j;
    int count_car_icons;
    int icon_width;
    int icon_height;
    int texture_x;
    int texture_y;
    tCar_icon* icon;
    int car_icon_height;
    br_model* model;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_icons) != 128);
    memset(gCar_icons, 0, sizeof(gCar_icons));
    car_icon_height = gCurrent_graf_data->car_icon_height;
    count_car_icons = gIcons_pix->height / car_icon_height;

    icon_width = NearestPowerOfTwo(pMap->width);
    icon_height = NearestPowerOfTwo(car_icon_height);

    texture_x = 0;
    texture_y = 0;
    for (i = 0; i < count_car_icons; i++) {
        icon = &gCar_icons[i];

        if (gTexture_maps[gSize_font_texture_pages] == NULL) {
            gTexture_maps[gSize_font_texture_pages] = BrPixelmapAllocate(pMap->type, 64, 64, NULL, 0);
            if (gTexture_maps[gSize_font_texture_pages] == NULL) {
                FatalError(kFatalError_CouldNotCreateTexturesPages_S, "Car Icons");
            }
            BrMapAdd(gTexture_maps[gSize_font_texture_pages]);
        }
        DRPixelmapRectangleCopy(
            gTexture_maps[gSize_font_texture_pages],
            texture_x,
            texture_y,
            pMap,
            0,
            car_icon_height * i,
            icon_width,
            car_icon_height);
        BrMapUpdate(gTexture_maps[gSize_font_texture_pages], BR_MAPU_ALL);
        icon->index = gSize_font_texture_pages;
        icon->model = model = CreateCharacterModel(icon_width, icon_height, texture_x, texture_y, "CAR ICONS");
        for (j = 0; j < 4; j++) {
            model->vertices[j].red = 0xff;
            model->vertices[j].grn = 0xff;
            model->vertices[j].blu = 0xff;
        }
        model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
        BrModelUpdate(model, BR_MODU_ALL);
        texture_x += icon_width;
        if (texture_x >= 64) {
            texture_x = 0;
            texture_y += icon_height;
            if (texture_y >= 64) {
                gSize_font_texture_pages++;
                if (gSize_font_texture_pages >= (int)REC2_ASIZE(gTexture_maps)) {
                    FatalError(kFatalError_CouldNotCreateTexturesPages_S, "CAR ICONS");
                }
                texture_x = 0;
                texture_y = 0;
            }
        }
    }
    gSize_font_texture_pages++;
    gCar_icons_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gCar_icons_model_actor->material = BrMaterialAllocate("Car Icon Material");
    gCar_icons_model_actor->material->flags = BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_ALWAYS_VISIBLE;
    BrMaterialAdd(gCar_icons_model_actor->material);
}

static void C2_HOOK_FASTCALL MapChanged(void) {

    InitMap();
    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x00497620
// FUNCTION: CARMA2_SW 0x00494250
void C2_HOOK_FASTCALL CheckMapMoveKeys(int pKey0) {

    if (KeyIsDown(31) && pKey0) {
        gHeadup_map_y_float = (float)((double)gHeadup_map_y_float - (double)gFrame_period / 20.0);
        if (gHeadup_map_y_float < 0.0f) {
            gHeadup_map_y_float = 0.0f;
        }
        MapChanged();
    } else if (KeyIsDown(32) && pKey0) {
        gHeadup_map_y_float = (float)((double)gHeadup_map_y_float + (double)gFrame_period / 20.0);
        if (gHeadup_map_y_float + gHeadup_map_h_float > (float)gBack_screen->height) {
            gHeadup_map_y_float = (float)(gBack_screen->height - gHeadup_map_h);
        }
        MapChanged();
    }
    if (KeyIsDown(33) && pKey0) {
        gHeadup_map_x_float = (float)((double)gHeadup_map_x_float - (double)gFrame_period / 20.0);
        if (gHeadup_map_x_float < 0.0f) {
            gHeadup_map_x_float = 0.0f;
        }
        MapChanged();
    } else if (KeyIsDown(34) && pKey0) {
        gHeadup_map_x_float = (float)((double)gHeadup_map_x_float + (double)gFrame_period / 20.0);
        if (gHeadup_map_x_float + gHeadup_map_w_float + 8.0f > (float)gBack_screen->width) {
            gHeadup_map_x_float = (float)(gBack_screen->width - gHeadup_map_w - 8);
        }
        MapChanged();
    }
}

// FUNCTION: CARMA2_HW 0x004977e0
void C2_HOOK_FASTCALL InitSmashTargets(void) {
    gCount_smashable_race_targets = 0;
}

// FUNCTION: CARMA2_HW 0x004977f0
// FUNCTION: CARMA2_SW 0x00494420
void C2_HOOK_FASTCALL AddSmashTarget(br_model* pModel, br_actor* pActor, int pUnknown) {
    int index;

    index = gCount_smashable_race_targets;
    if (index < (int)REC2_ASIZE(gSmashable_race_targets)) {
        gSmashable_race_targets[index].model = pModel;
        gSmashable_race_targets[index].actor = pActor;
        gSmashable_race_targets[index].field_0x8 = pUnknown;
        gCount_smashable_race_targets = index + 1;
    }
}

int NearestPowerOfTwo(int pValue) {
    int power;

    for (power = 1; power < 32; power++) {
        if (pValue <= 1 << power) {
            return 1 << power;
        }
    }
    return INT_MIN;
}

