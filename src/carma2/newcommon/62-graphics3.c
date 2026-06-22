#include "62-graphics3.h"

#include "08-loading1.h"
#include "10-loading2.h"
#include "19-font.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "53-controls.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "rec2_macros.h"

#include <limits.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x00655dfc
int gMini_map_visible = 2;

// GLOBAL: CARMA2_HW 0x0074a780
tCar_icon gCar_icons[128];

// GLOBAL: CARMA2_HW 0x0074ab84
br_actor* gCar_icons_model_actor;

// GLOBAL: CARMA2_HW 0x0068d8cc
int gInitMap_done;

// GLOBAL: CARMA2_HW 0x0074abdc
float gHeadup_map_x_float;

// GLOBAL: CARMA2_HW 0x0074abd8
float gHeadup_map_y_float;

// GLOBAL: CARMA2_HW 0x0074abcc
float gHeadup_map_w_float;

// GLOBAL: CARMA2_HW 0x0074ab80
float gHeadup_map_h_float;

// GLOBAL: CARMA2_HW 0x0074ab88
int gINT_0074ab88;

// GLOBAL: CARMA2_HW 0x0074ab8c
int gINT_0074ab8c;

// GLOBAL: CARMA2_HW 0x0074ab94
int gINT_0074ab94;

// GLOBAL: CARMA2_HW 0x0074ab98
int gHeadup_map_half_height;

// GLOBAL: CARMA2_HW 0x0074abe4
int gHeadup_map_half_width;

// GLOBAL: CARMA2_HW 0x0074abec
int gINT_0074abec;

// GLOBAL: CARMA2_HW 0x0068d8d8
br_pixelmap* gMap_overlay;

// GLOBAL: CARMA2_HW 0x0074aba0
int gMini_map_x;

// GLOBAL: CARMA2_HW 0x0074aba4
int gMini_map_y;

// GLOBAL: CARMA2_HW 0x0074abd4
int gINT_0074abd4;

// GLOBAL: CARMA2_HW 0x0074abd0
int gINT_0074abd0;

// GLOBAL: CARMA2_HW 0x0068d8dc
br_pixelmap* gMini_map;

// GLOBAL: CARMA2_HW 0x00655f04
int gINT_00655f04;

// GLOBAL: CARMA2_HW 0x00655f08
int gINT_00655f08;

// GLOBAL: CARMA2_HW 0x0068d8a0
float gFLOAT_0068d8a0;

// GLOBAL: CARMA2_HW 0x0068d8a4
float gFLOAT_0068d8a4;

// GLOBAL: CARMA2_HW 0x0068d8a8
float gMini_map_arrow_z;

// GLOBAL: CARMA2_HW 0x0074ab9c
float gFLOAT_0074ab9c;

// GLOBAL: CARMA2_HW 0x0074abb8
float gFLOAT_0074abb8;

// GLOBAL: CARMA2_HW 0x0068d6e4
br_pixelmap* gCheckpoint_numbers;

// GLOBAL: CARMA2_HW 0x0068d884
br_pixelmap* gSmashy_dot;

// GLOBAL: CARMA2_HW 0x0068c87c
br_pixelmap* gAlt_meter;

// GLOBAL: CARMA2_HW 0x0068c860
int gCheckpoint_digit_height;

// GLOBAL: CARMA2_HW 0x0068d8bc
int gCheckpoint_digit_center_x;

// GLOBAL: CARMA2_HW 0x0068c85c
tU32 gCheckpoint_digit_center_y;

// GLOBAL: CARMA2_HW 0x0068d6f8
tOppo_status_messages gOppo_status_messages[9];

// CarArrowColour

// ToggleTargetLock

// CycleTargetLock

// CycleMapOptions

// ToggleMapTransparency

// InitTargetLock

// SetIntegerMapRenders

// OppositeColour

// DrawArrow

// DrawMapBlip

// DrawSmashBlip

// DrawMapSmallBlip

// CopyMapToScreen

// DrawCarArrow

// DoSomeThingsToCheckpoints

// CalcMapCheckpoint

// CalcMapCheckpoint2

// DrawCheckpoint

// DrawOffsetCheckpoint

// DoMapOverlays

// StartMap

// FinishMap

// CheckpointLine

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

// STUB: CARMA2_HW 0x00496e30
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
    gINT_00655f04 = gHeadup_map_x;
    gINT_00655f08 = gHeadup_map_y;
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
    gFLOAT_0074ab9c = REC2_SQR((float)(gHeadup_map_half_height + gHeadup_map_half_width) * 0.45);
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
void C2_HOOK_FASTCALL CheckMapMoveKeys(int pKey0) {

    if (KeyIsDown(31) && pKey0) {
        gHeadup_map_y_float -= (float)gFrame_period / 20.0;
        if (gHeadup_map_y_float < 0.0f) {
            gHeadup_map_y_float = 0.0f;
        }
        MapChanged();
    } else if (KeyIsDown(32) && pKey0) {
        gHeadup_map_y_float += (float)gFrame_period / 20.0;
        if (gHeadup_map_y_float + gHeadup_map_h_float > (float)gBack_screen->height) {
            gHeadup_map_y_float = (float)(gBack_screen->height - gHeadup_map_h);
        }
        MapChanged();
    }
    if (KeyIsDown(33) && pKey0) {
        gHeadup_map_x_float -= (float)gFrame_period / 20.0;
        if (gHeadup_map_x_float < 0.0f) {
            gHeadup_map_x_float = 0.0f;
        }
        MapChanged();
    } else if (KeyIsDown(34) && pKey0) {
        gHeadup_map_x_float += (float)gFrame_period / 20.0;
        if (gHeadup_map_x_float + gHeadup_map_w_float + 8.0f > (float)gBack_screen->width) {
            gHeadup_map_x_float = (float)(gBack_screen->width - gHeadup_map_w - 8);
        }
        MapChanged();
    }
}

// InitSmashTargets

// AddSmashTarget

int NearestPowerOfTwo(int pValue) {
    int power;

    for (power = 1; power < 32; power++) {
        if (pValue <= 1 << power) {
            return 1 << power;
        }
    }
    return INT_MIN;
}

