#include "62-graphics3.h"

#include "19-font.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "52-errors.h"
#include "globvars.h"
#include "rec2_macros.h"

#include <limits.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x00655dfc
int gMini_map_visible = 2;

// GLOBAL: CARMA2_HW 0x0074a780
tCar_icon gCar_icons[128];

// GLOBAL: CARMA2_HW 0x0068648c
int gSize_font_texture_pages;

// GLOBAL: CARMA2_HW 0x0074ab84
br_actor* gCar_icons_model_actor;

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

// ReadMoodMessages

// STUB: CARMA2_HW 0x00496e30
void C2_HOOK_FASTCALL InitMap(void) {
    NOT_IMPLEMENTED();
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

// MapChanged

// CheckMapMoveKeys

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

