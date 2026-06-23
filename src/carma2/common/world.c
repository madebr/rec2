#include "world.h"

#include "animation.h"
#include "brucetrk.h"
#include "car.h"
#include "crush.h"
#include "depth.h"
#include "drmem.h"
#include "52-errors.h"
#include "finteray.h"
#include "flicplay.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "opponent.h"
#include "pedestrn.h"
#include "piping.h"
#include "replay.h"
#include "shrapnel.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "trig.h"
#include "utility.h"

#include "platform.h"

#include <brender/brender.h>
#include "rec2_macros.h"

#include <tiffio.h>
#include <zlib.h>

#include <ctype.h>
#include "c2_stdlib.h"
#include "c2_string.h"
#include "c2_sys_stat.h"
#include "rec2_types.h"

#include <assert.h>
#include <c2_sys_stat.h>

#define RGB565_R(V) (((V) & 0xf800) >> 11)
#define RGB565_G(V) (((V) & 0x07e0) >> 5)
#define RGB565_B(V) (((V) & 0x001f) >> 0)
#define RGB888_R(V) (((V) >> 16) & 0xff)
#define RGB888_G(V) (((V) >> 8) & 0xff)
#define RGB888_B(V) (((V) >> 0) & 0xff)


// GLOBAL: CARMA2_HW 0x00660268
const char* gSoundType_Choices[2] = {"SATURATED", "SCATTERED"};

// GLOBAL: CARMA2_HW 0x00591374
tCar_texturing_level gCar_texturing_level = eCTL_full;

// GLOBAL: CARMA2_HW 0x0059136c
tRoad_texturing_level gRoad_texturing_level = eRTL_full;

// GLOBAL: CARMA2_HW 0x00591370
tWall_texturing_level gWall_texturing_level = eWTL_full;

// GLOBAL: CARMA2_HW 0x00591368
int gRendering_accessories = 1;

// GLOBAL: CARMA2_HW 0x006b7820
tBrender_storage* gStorage_for_callbacks;

// GLOBAL: CARMA2_HW 0x006aaa20
br_pixelmap* gDuplicate_pixelmap;

// GLOBAL: CARMA2_HW 0x006aaa2c
int gDisallow_duplicates;

// GLOBAL: CARMA2_HW 0x00660cb8
tRendererShadingType gMaterial_shading_for_callback = kRendererShadingType_Undefined;

// GLOBAL: CARMA2_HW 0x006aaa24
br_model* gDuplicate_model;

// GLOBAL: CARMA2_HW 0x006aaa28
br_material* gDuplicate_material;

// GLOBAL: CARMA2_HW 0x0068b840
br_scalar gSight_distance_squared;

// GLOBAL: CARMA2_HW 0x006a80f8
tDecal gDecals[50];

// GLOBAL: CARMA2_HW 0x006a3940
tQueued_net_smash gQueued_net_smashes[50];

// GLOBAL: CARMA2_HW 0x006a55bc
int gSize_powerup_queue;

// GLOBAL: CARMA2_HW 0x006a6d3c
int gCount_host_smashes;


// GLOBAL: CARMA2_HW 0x006631c0
br_filesystem gZlibBrFileSystem = {
    "Zlib filesystem",
    ZlibFsGetAttributes,
    ZlibFsOpenRead,
    ZlibFsOpenWrite,
    ZlibFsClose,
    ZlibFsEof,
    ZlibFsGetChr,
    ZlibFsPutChr,
    ZlibFsRead,
    ZlibFsWrite,
    ZlibFsGetLine,
    ZlibFsPutLine,
    ZlibFsAdvance,
};

// GLOBAL: CARMA2_HW 0x0068b848
int gGroovidelics_array_size;

// GLOBAL: CARMA2_HW 0x0068b850
tGroovidelic_spec* gGroovidelics_array;


// GLOBAL: CARMA2_HW 0x0074d0a0
char gRace_path[256];


// GLOBAL: CARMA2_HW 0x0074ca04
tMaterial_exception* gMaterial_exceptions;


// GLOBAL: CARMA2_HW 0x0065fe88
const char* gSmashable_item_mode_names[6] = {
    "nochange",
    "decal",
    "texturechange",
    "remove",
    "replacemodel",
    "crush",
};

// GLOBAL: CARMA2_HW 0x006b788c
int gCount_smashable_noncars;

// GLOBAL: CARMA2_HW 0x006a55b0
int gCount_smashable_noncar_shrapnel_actors;

// GLOBAL: CARMA2_HW 0x006b7888
tSmashable_environment_name* gSmashable_noncars;

// GLOBAL: CARMA2_HW 0x006a55b8
const char* gSmashable_track_environment_path;


// GLOBAL: CARMA2_HW 0x006a55b4
int gCount_track_smashable_environment_specs;

// GLOBAL: CARMA2_HW 0x006a8288
int gCount_track_smashable_environment_specs_2;

// GLOBAL: CARMA2_HW 0x006a3330
int gCapacity_track_smashables;

// GLOBAL: CARMA2_HW 0x006a5138
tSmashable_item_spec* gTrack_smashable_environment_specs;

// GLOBAL: CARMA2_HW 0x006ba4a0
tSmashable_item_spec* gSmashable_track_indexable_triggers[100];


// GLOBAL: CARMA2_HW 0x00660e90
const char* gDepth_effect_names[3] = {
    "dark",
    "fog",
    "colour",
};

// GLOBAL: CARMA2_HW 0x006b6400
char gAdditional_model_path[256];

// GLOBAL: CARMA2_HW 0x006b6500
char gAdditional_actor_path[256];

// GLOBAL: CARMA2_HW 0x006b6620
br_model* gAdditional_models[1000];

// GLOBAL: CARMA2_HW 0x007634b4
int gTrack_version;

// GLOBAL: CARMA2_HW 0x006ab948
int gNumber_of_additional_models;

// GLOBAL: CARMA2_HW 0x006b75c0
br_actor* gAdditional_actors;

// GLOBAL: CARMA2_HW 0x006b6600
br_actor* gLast_actor;

// GLOBAL: CARMA2_HW 0x006aaf40
tU8* gTrack_flic_buffer;

// GLOBAL: CARMA2_HW 0x006aaf48
tU32 gTrack_flic_buffer_size;

// GLOBAL: CARMA2_HW 0x006aaf50
tFlic_descriptor gTrack_flic_descriptor;


// GLOBAL: CARMA2_HW 0x0079ec2e
br_angle gSky_image_width;

// GLOBAL: CARMA2_HW 0x0079ec2c
br_angle gSky_image_height;

// GLOBAL: CARMA2_HW 0x0079ec30
br_angle gSky_image_underground;

// GLOBAL: CARMA2_HW 0x0074caa8
int gTrack_depth_colour_red;

// GLOBAL: CARMA2_HW 0x0074cf2c
int gTrack_depth_colour_green;

// GLOBAL: CARMA2_HW 0x0074cad0
int gTrack_depth_colour_blue;

// GLOBAL: CARMA2_HW 0x0079ef40
tU8 gNon_car_spec_indices[100];

// GLOBAL: CARMA2_HW 0x0079ed30
tU8 gNon_car_indices[88];

// GLOBAL: CARMA2_HW 0x006a7fc8
tRepair_animation gRepair_animations[25];

// GLOBAL: CARMA2_HW 0x00660258
const char* gSpecial_effects_boundary_choices[4] = {
    "BOX",
    "PLANE",
    "DEFAULT",
    "NEW",
};

// GLOBAL: CARMA2_HW 0x0068b878
int gDefault_engine_noise_index;

// GLOBAL: CARMA2_HW 0x00595c38
const char* gSoundGeneratorTypeNames[3] = {
    "NONCAR",
    "ACTOR",
    "POINT",
};

// GLOBAL: CARMA2_HW 0x006a22c0
int gCount_extra_renders;

// GLOBAL: CARMA2_HW 0x0068b830
tFunk_temp_buffer* gFunk_temp_vertices;

// GLOBAL: CARMA2_HW 0x0068b84c
tFunkotronic_spec* gFunkotronics_array;

// GLOBAL: CARMA2_HW 0x0068b844
int gFunkotronics_array_size;

// GLOBAL: CARMA2_HW 0x00688b30
tGroove_funk_binding gGroove_funk_bindings[1440];

// GLOBAL: CARMA2_HW 0x00655ba0
const char* gFunk_nature_names[4] = {
    "constant",
    "distance",
    "lastlap",
    "otherlaps",
};

// GLOBAL: CARMA2_HW 0x00655b50
const char* gFunk_type_names[5] = {
    "spin",
    "rock",
    "throb",
    "slither",
    "roll",
};

// GLOBAL: CARMA2_HW 0x00655b80
const char* gFunk_move_names[7] = {
    "linear",
    "harmonic",
    "flash",
    "controlled",
    "absolute",
    "continuous",
    "texturebits",
};

// GLOBAL: CARMA2_HW 0x00655b68
const char* gFunk_anim_names[4] = {
    "frames",
    "flic",
    "camera",
    "mirror",
};

// GLOBAL: CARMA2_HW 0x00655b78
const char* gTime_mode_names[2] = {
    "approximate",
    "accurate",
};

// GLOBAL: CARMA2_HW 0x00655bf0
const char* gCamera_animation_names[2] = {
    "static",
    "tracking",
};

// GLOBAL: CARMA2_HW 0x0068b854
br_actor* gGroove_by_proxy_actor;

// GLOBAL: CARMA2_HW 0x00655bc8
const char* gLollipop_names[3] = {
    "xlollipop",
    "ylollipop",
    "zlollipop",
};

// GLOBAL: CARMA2_HW 0x00655bb0
const char* gGroove_nature_names[2] = {
    "constant",
    "distance",
};

// GLOBAL: CARMA2_HW 0x00655bd8
const char* gGroove_path_names[2] = {
    "straight",
    "circular",
};

// GLOBAL: CARMA2_HW 0x00655be0
const char* gGroove_object_names[4] = {
    "spin",
    "rock",
    "throb",
    "shear",
};

// GLOBAL: CARMA2_HW 0x006a55c0
tNet_stored_smash* gNet_host_smashes;

// GLOBAL: CARMA2_HW 0x006a22c8
tExtra_render gExtra_renders[6];

// GLOBAL: CARMA2_HW 0x006ab7a8
int gDelete_count;

// GLOBAL: CARMA2_HW 0x006aafc8
br_actor* gDelete_list[500];

// GLOBAL: CARMA2_HW 0x006ab940
int gSpec_vol_mode;

// GLOBAL: CARMA2_HW 0x006ab7b0
br_actor* gSpec_vol_actors[100];

// GLOBAL: CARMA2_HW 0x0068b838
tU32 gPrevious_groove_times[2];

// GLOBAL: CARMA2_HW 0x00704e60
br_actor* gHud_actor_storage[128];

#define SAW(T, PERIOD) (fmodf((T), (PERIOD)) / (PERIOD))

#define MOVE_FUNK_PARAMETER(DEST, MODE, PERIOD, TEXTUREBITS, AMPLITUDE, FLASH_VALUE)            \
    do {                                                                                        \
        switch (MODE) {                                                                         \
        case eMove_continuous:                                                                  \
            if ((PERIOD) == 0.f) {                                                              \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*SAW(f_the_time, (PERIOD));                                   \
            }                                                                                   \
            break;                                                                              \
        case eMove_controlled:                                                                  \
            DEST = (PERIOD) * (AMPLITUDE);                                                      \
            break;                                                                              \
        case eMove_absolute:                                                                    \
            DEST = (PERIOD);                                                                    \
            break;                                                                              \
        case eMove_linear:                                                                      \
            if ((PERIOD) == 0.f) {                                                              \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*MapSawToTriangle(SAW(f_the_time, (PERIOD)));                 \
            }                                                                                   \
            break;                                                                              \
        case eMove_flash:                                                                       \
            if (2 * fmodf(f_the_time, (PERIOD)) > (PERIOD)) {                                   \
                DEST = (FLASH_VALUE);                                                           \
            } else {                                                                            \
                DEST = -(FLASH_VALUE);                                                          \
            }                                                                                   \
            break;                                                                              \
        case eMove_texturebits:                                                                 \
            {                                                                                   \
                int sum_flags = 0;                                                              \
                int ii;                                                                         \
                for (ii = 0; ii < (TEXTUREBITS)->count; ii++) {                                 \
                    if ((TEXTUREBITS)->car->field_0x18cc & (1 << (TEXTUREBITS)->bits[ii])) {    \
                        sum_flags |= 1 << ii;                                                   \
                    }                                                                           \
                }                                                                               \
                DEST = (br_scalar)sum_flags;                                                    \
            }                                                                                   \
            break;                                                                              \
        case eMove_harmonic:                                                                    \
        default:                                                                                \
            if ((PERIOD) == 0.f) {                                                              \
                DEST = 0.f;                                                                     \
            } else {                                                                            \
                DEST = (AMPLITUDE)*BR_SIN(BrDegreeToAngle(SAW(f_the_time, (PERIOD)) * 360.f));  \
            }                                                                                   \
            break;                                                                              \
        }                                                                                       \
    } while (0)

// FUNCTION: CARMA2_HW 0x00447640
tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void) {

    return gCar_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00447650
void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel) {

    if (gCar_texturing_level != pLevel) {
        if (gOur_car_storage_space.models_count != 0) {
            SetCarStorageTexturingLevel(&gOur_car_storage_space, pLevel, gCar_texturing_level);
        }
        if (gTheir_cars_storage_space.models_count != 0) {
            SetCarStorageTexturingLevel(&gTheir_cars_storage_space, pLevel, gCar_texturing_level);
        }
        if (gNet_cars_storage_space.models_count != 0) {
            SetCarStorageTexturingLevel(&gNet_cars_storage_space, pLevel, gCar_texturing_level);
        }
    }
    gCar_texturing_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00448770
tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void) {

    return gRoad_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00448780
void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel) {

    gRoad_texturing_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x004488f0
tWall_texturing_level C2_HOOK_FASTCALL GetWallTexturingLevel(void) {

    return gWall_texturing_level;
}

// FUNCTION: CARMA2_HW 0x00448900
void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel) {

    gWall_texturing_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00448f10
void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel) {

    gCar_simplification_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00448f20
int C2_HOOK_FASTCALL GetCarSimplificationLevel(void) {

    return gCar_simplification_level;
}

// FUNCTION: CARMA2_HW 0x00448f90
intptr_t C2_HOOK_CDECL ProcessMaterials(br_actor* pActor, material_cbfn* pCallback) {

    if (pActor->material != NULL) {
        pCallback(pActor->material);
    }
    if (pActor->type == BR_ACTOR_MODEL && pActor->model != NULL) {
        ProcessModelFaceMaterials2(pActor->model, pCallback);
    }
    return BrActorEnum(pActor, (br_actor_enum_cbfn*)ProcessMaterials, pCallback);
}

// FUNCTION: CARMA2_HW 0x00448ea0
intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB(br_actor* pActor, void* pFlag) {
    if (pActor->identifier != NULL && pActor->identifier[0] == '&') {
        pActor->render_style = *(br_uint_8*)pFlag;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00448ec0
void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn) {
    int style;

    if (gTrack_actor != NULL)  {
        if (pOn) {
            style = BR_RSTYLE_FACES;
        } else {
            style = BR_RSTYLE_NONE;
        }
        DRActorEnumRecurse(gTrack_actor, (br_actor_enum_cbfn*)SetAccessoryRenderingCB, &style);
    }
    gRendering_accessories = pOn;
}

// FUNCTION: CARMA2_HW 0x00448f00
int C2_HOOK_FASTCALL GetAccessoryRendering(void) {

    return gRendering_accessories;
}

// FUNCTION: CARMA2_HW 0x0048ea40
int C2_HOOK_FASTCALL LoadBunchOfPixies(const char* pathRoot, const char* textureName, br_pixelmap** textureBuffer, size_t bufferCapacity) {
    tPath_name tempPath;
    br_uint_32 nb;

    PathCat(tempPath, pathRoot, textureName);
    strcat(tempPath, (gPixelFlags & kPixelFlags_16bbp) ? ".P16" : ".P08");
    nb = BrPixelmapLoadMany(tempPath, textureBuffer, (br_uint_16)bufferCapacity);
    if (nb == 0) {
        PathCat(tempPath, pathRoot, (gPixelFlags & kPixelFlags_16bbp) ? "PIX16" : "PIX8");
        PathCat(tempPath, tempPath, textureName);
        strcat(tempPath, ".PIX");
        nb = BrPixelmapLoadMany(tempPath, textureBuffer, (br_uint_16)bufferCapacity);
    }
    if (nb == 0) {
        return 0;
    }
    if (nb == 1 && !gDisableTiffConversion) {
        if (textureBuffer[0]->identifier != NULL) {
            BrResFree(textureBuffer[0]->identifier);
        }
        textureBuffer[0]->identifier = BrResStrDup(textureBuffer[0], textureName);
    }
    return nb;
}

// FUNCTION: CARMA2_HW 0x004ff8d0
void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx) {
    char s[256];

    REC2_BUG_ON(sizeof(tSpecial_volume) != 220);
    REC2_BUG_ON(sizeof(tSpecial_volume_soundfx_data) != 36);
    REC2_BUG_ON(offsetof(tSpecial_volume, gravity_multiplier) != 136);

    /* gravity multiplier */
    pSpec->gravity_multiplier = GetAFloat(pF);
    if (pSpec->gravity_multiplier < 0 && !gRussels_fannies) {
        pSpec->gravity_multiplier = 1.f;
    }
    /* viscosity multiplier */
    pSpec->viscosity_multiplier = GetAFloat(pF);
    /* Car damage per millisecond */
    pSpec->car_damage_per_ms = GetAScalar(pF);
    /* Pedestrian damage per millisecond */
    pSpec->ped_damage_per_ms = GetAScalar(pF);
    /* camera effect index */
    pSpec->camera_special_effect_index = GetAnInt(pF);
    /* Sky colour */
    pSpec->sky_col = GetAnInt(pF);

    /* Windscreen material to use */
    GetAString(pF, s);
    if (pScreen_name_str != NULL) {
        strcpy(pScreen_name_str, s);
    } else {
        pSpec->screen_pixelmap = BrMapFind(s);
    }

    /* Sound ID of entry noise */
    pSpec->entry_noise = GetAnInt(pF);
    /* Sound ID of exit noise */
    pSpec->exit_noise = GetAnInt(pF);
    /* Engine noise index */
    pSpec->engine_noise_index = GetAnInt(pF);
    /* material index */
    pSpec->material_modifier_index = GetAnInt(pF);
    if (soundfx) {
        pSpec->soundfx_type = GetALineAndInterpretCommand(pF, gSoundType_Choices, REC2_ASIZE(gSoundType_Choices));
        if (pSpec->soundfx_type != kSoundFx_None) {
            ReadSoundSpec(pF, &pSpec->soundfx_data);
        }
    } else {
        pSpec->soundfx_type = kSoundFx_None;
    }
}

// FUNCTION: CARMA2_HW 0x005026b0
int C2_HOOK_FASTCALL GetFileName(const char *path, tName_list *pList) {
    tPath_name pathCopy;
    tPath_name pathUpper;
    tPath_name dir_path;
    tPath_name stem_path;

    strcpy(pathCopy, path);
    Uppercaseificate(pathUpper, pathCopy);
    if (strstr(pathUpper, ".PIX") == NULL
            && strstr(pathUpper, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(pathUpper, dir_path, stem_path);
    strcpy(pList->items[pList->size], stem_path);
    if (pList->size < REC2_ASIZE(pList->items)) {
        pList->size += 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00502780
int C2_HOOK_FASTCALL GetAdditionalFileName(const char* path, tName_list* pList) {
    tPath_name pathCopy;
    tPath_name upperPath;
    tPath_name dir_path;
    tPath_name stem_path;
    int alreadyInList;
    size_t i;

    strcpy(pathCopy, path);
    Uppercaseificate(upperPath, pathCopy);

    if (strstr(upperPath, ".PIX") == NULL
            && strstr(upperPath, ".P16") == NULL
            && strstr(upperPath, ".P08") == NULL
            && strstr(upperPath, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(upperPath, dir_path, stem_path);

    alreadyInList = 0;
    for (i = 0; i < pList->size; i++) {
        if (strcmp(pList->items[i], stem_path) == 0) {
            alreadyInList = 1;
            break;
        }
    }
    if (!alreadyInList) {
        strcpy(pList->items[pList->size], stem_path);
        if (pList->size < REC2_ASIZE(pList->items)) {
            pList->size += 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00500d50
void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds) {

    pStorage_space->pixelmaps_count = 0;
    pStorage_space->shade_tables_count = 0;
    pStorage_space->materials_count = 0;
    pStorage_space->models_count = 0;
    pStorage_space->sounds_count = 0;
    pStorage_space->flags = pUnknown;
    pStorage_space->max_pixelmaps = pMax_pixelmaps;
    pStorage_space->max_shade_tables = pMax_shade_tables;
    pStorage_space->max_materials = pMax_materials;
    pStorage_space->max_models = pMax_models;
    pStorage_space->max_sounds = pMax_sounds;
    pStorage_space->pixelmaps = BrMemCalloc(pMax_pixelmaps, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->shade_tables = BrMemCalloc(pMax_shade_tables, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->materials = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
    pStorage_space->models = BrMemCalloc(pMax_models, sizeof(br_model*), kMem_stor_space_table);
    pStorage_space->sounds = BrMemCalloc(pMax_sounds, sizeof(int), kMem_stor_space_table);
    pStorage_space->materialProps = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
}

// FUNCTION: CARMA2_HW 0x00500e10
void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage) {
    BrMemFree(pStorage->pixelmaps);
    BrMemFree(pStorage->shade_tables);
    BrMemFree(pStorage->materials);
    BrMemFree(pStorage->models);
    BrMemFree(pStorage->sounds);
    BrMemFree(pStorage->materialProps);
}


// FUNCTION: CARMA2_HW 0x00501020
tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm) {
    int i;

    gDuplicate_pixelmap = NULL;
    if (pStorage_space->pixelmaps_count >= pStorage_space->max_pixelmaps) {
        gDuplicate_pixelmap = NULL;
        return eStorage_not_enough_room;
    }

    for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
        if (pStorage_space->pixelmaps[i]->identifier != NULL
            && pThe_pm->identifier != NULL
            && strcmp(pStorage_space->pixelmaps[i]->identifier, pThe_pm->identifier) == 0) {
            gDuplicate_pixelmap = pStorage_space->pixelmaps[i];
            return eStorage_duplicate;
        }
    }
    pStorage_space->pixelmaps[pStorage_space->pixelmaps_count] = (br_pixelmap*)pThe_pm;
    pStorage_space->pixelmaps_count++;
    return eStorage_allocated;
}

tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id) {
    int i;

    if (pStorage_space->sounds_count >= pStorage_space->max_sounds) {
        return eStorage_not_enough_room;
    }

    for (i = 0; i < pStorage_space->sounds_count; i++) {
        if (pStorage_space->sounds[i] != 0
            && pStorage_space->sounds[i] == pSound_id) {
            return eStorage_duplicate;
        }
    }
    pStorage_space->sounds[pStorage_space->sounds_count] = pSound_id;
    pStorage_space->sounds_count++;
    return eStorage_allocated;
}

// FUNCTION: CARMA2_HW 0x00501930
tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id) {

    if (S3GetBufferDescription(pSound_id) != NULL) {
        return pSound_id;
    }

    switch (AddSoundToStorage(pStorage_space, pSound_id)) {
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientSoundSlotsInStorageArea);
        break;

    case eStorage_duplicate:
        return pSound_id;

    case eStorage_allocated:
        S3LoadSample(pSound_id);
        return pSound_id;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00486c00
int C2_HOOK_FASTCALL IsValidFile(const char* path) {
    struct_c2_stat32 s;

    return c2_stat32(path, &s) == 0;
}

// FUNCTION: CARMA2_HW 0x00486be0
int C2_HOOK_FASTCALL GetLastModificationTime(const char* path) {
    struct_c2_stat32 s;
    int res;

    res = c2_stat32(path, &s);
    if (res == -1) {
        return 0;
    }
    return (int)s.st_mtime;
}

br_pixelmap* C2_HOOK_FASTCALL Read_DEFAULT_ACT(const char* textureDir, int flags, int *errorCode) {
    int i;
    tPath_name path;
    br_uint_8 defaultActBuffer[0x300];
    FILE* f;
    br_pixelmap* pm;
    br_uint_8* src;
    br_uint_8* dst;

    sprintf(path, "%s%s%s%s%s", textureDir, gDir_separator, "PALETTE", gDir_separator, "DEFAULT.ACT");
    f = DRfopen(path, "rb");
    if (f == NULL) {
        *errorCode = 5;
        return NULL;
    }
    if (fread(defaultActBuffer, REC2_ASIZE(defaultActBuffer), 1, (FILE*)f) == 0) {
        PFfclose(f);
        *errorCode = 5;
        return NULL;
    }
    PFfclose(f);
    src = defaultActBuffer;
    if ((flags & kLoadTextureFlags_PalatteRGB555) != 0) {
        pm = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 0x100, NULL, 0);
        if (pm == NULL) {
            *errorCode = 2;
            return NULL;
        }
        dst = pm->pixels;
        for (i = 0; i < 0x100; i++) {
            *(br_uint_32*)dst = ((src[0] & 0xf8) << 8) | ((src[1] & 0xfc) << 3) | (src[2] >> 3);
            src += 3;
            dst += 4;
        }
    } else {
        pm = BrPixelmapAllocate(BR_PMT_RGBX_888, 1, 0x100, NULL, 0);
        if (pm == NULL) {
            *errorCode = 2;
            return NULL;
        }
        dst = pm->pixels;
        dst[0] = 0;
        dst[1] = 0;
        dst[2] = 0;
        src += 3;
        dst += 4;
        for (i = 0; i < 0xff; i++) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            if (dst[0] == 0 && dst[1] == 0 && dst[2] == 0) {
                dst[0] = 1;
                dst[1] = 1;
                dst[2] = 1;
            }
            src += 3;
            dst += 4;
        }
    }
    pm->identifier = BrResStrDup(pm, "DEFAULT.ACT");
    if (pm->identifier == NULL) {
        BrPixelmapFree(pm);
        *errorCode = 2;
        return NULL;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x004862b0
br_uint_8 C2_HOOK_FASTCALL FindBestMatch_ShadeTable(br_colour rgb, br_pixelmap *shadeTable) {
    int i;
    int ref_r, ref_g, ref_b;
    int dr, dg, db;
    br_uint_32 c;
    int min_index;
    int min_error;
    int error;

    ref_r = RGB888_R(rgb);
    ref_g = RGB888_G(rgb);
    ref_b = RGB888_B(rgb);
    if (shadeTable->type == BR_PMT_RGB_555) {
        c = ((br_uint_16*)shadeTable->pixels)[1];
        dr = (RGB565_R(c) << 3) - ref_r;
        dg = (RGB565_G(c) << 2) - ref_g;
        db = (RGB565_B(c) << 3) - ref_b;
        min_index = 1;
        min_error = BR_SQR3(dr, dg, db);
        for (i = 2; i < 256; i++) {
            c = ((br_uint_16*)shadeTable->pixels)[i];
            dr = (RGB565_R(c) << 3) - ref_r;
            dg = (RGB565_G(c) << 2) - ref_g;
            db = (RGB565_B(c) << 3) - ref_b;
            error = BR_SQR3(dr, dg, db);
            if (error < min_error) {
                min_error = error;
                min_index = i;
            }
        }
    } else {
        c = ((br_uint_32 *) shadeTable->pixels)[1];
        dr = RGB888_R(c) - ref_r;
        dg = RGB888_G(c) - ref_g;
        db = RGB888_B(c) - ref_b;
        min_index = 1;
        min_error = BR_SQR3(dr, dg, db);
        for (i = 2; i < 256; i++) {
            c = ((br_uint_32 *) shadeTable->pixels)[i];
            dr = RGB888_R(c) - ref_r;
            dg = RGB888_G(c) - ref_g;
            db = RGB888_B(c) - ref_b;
            error = BR_SQR3(dr, dg, db);
            if (error < min_error) {
                min_error = error;
                min_index = i;
            }
        }
    }
    return min_index;
}

// FUNCTION: CARMA2_HW 0x00485590
br_pixelmap* C2_HOOK_FASTCALL CreatePalettePixelmapFromRGBChannels(br_uint_16* redChannel, br_uint_16* greenChannel, br_uint_16* blueChannel, int isRGB555) {
    br_pixelmap* pm;
    int i;

    if (isRGB555) {
        pm = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 0x100, NULL, 0);
        if (pm == NULL) {
            return NULL;
        }
        for (i = 0; i < 0x100; i++) {
            ((br_uint_16*)pm->pixels)[i] = (redChannel[i] & 0xf800) | ((greenChannel[i] & 0xfc00) >> 5) | ((blueChannel[i] & 0xf800) >> 11);
        }
    } else {
        pm = BrPixelmapAllocate(BR_PMT_RGBX_888, 1, 0x100, NULL, 0);
        if (pm == NULL) {
            return NULL;
        }
        for (i = 0; i < 0x100; i++) {
            ((br_uint_8*)pm->pixels)[4 * i + 0] = redChannel[i] >> 8;
            ((br_uint_8*)pm->pixels)[4 * i + 1] = greenChannel[i] >> 8;
            ((br_uint_8*)pm->pixels)[4 * i + 2] = blueChannel[i] >> 8;
        }
    }
    pm->identifier = BrResStrDup(pm, "palette");
    if (pm == NULL) {
        BrPixelmapFree(pm);
        return NULL;
    }
    return pm;
}

br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_MappedToShadeTable(const char* path, br_pixelmap* shadeTable, int* errorCode) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanlineBuffer;
    br_uint_8* curSrcPixel;
    br_uint_32 x;
    br_uint_32 y;

    tif = TIFFOpen(path, "r");
    if (tif == NULL) {
        *errorCode = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *errorCode = 2;
        return NULL;
    }
    scanlineBuffer = malloc(TIFFScanlineSize(tif));
    if (scanlineBuffer == NULL) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *errorCode = 2;
        return NULL;
    }
    for (y = 0; y < height; y++) {
        if (TIFFReadScanline(tif, scanlineBuffer, y, 0) < 1) {
            break;
        }
        curSrcPixel = scanlineBuffer;
        if (samples_per_pixel == 3) {
            for (x = 0; x < width; x++) {
                *(((br_uint_8*)pm->pixels) + y * pm->row_bytes + x) = FindBestMatch_ShadeTable(BR_COLOUR_RGB(curSrcPixel[0], curSrcPixel[1], curSrcPixel[2]), shadeTable);
                curSrcPixel += 3;
            }
        } else {
            for (x = 0; x < width; x++) {
                if (curSrcPixel[3] < 0x80) {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = FindBestMatch_ShadeTable(BR_COLOUR_RGB(curSrcPixel[0], curSrcPixel[1], curSrcPixel[2]), shadeTable);
                } else {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = 0;
                }
                curSrcPixel += 4;
            }
        }
    }
    free(scanlineBuffer);
    TIFFClose(tif);
    if (y < height) {
        BrPixelmapFree(pm);
        *errorCode = 2;
        return NULL;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00485fe0
br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_WithShadeTable(const char *path, int flags, int *errorCode) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanlineBuffer;
    br_uint_8* curSrcPos;
    br_uint_32 x;
    br_uint_32 y;
    br_uint_16* colorMap_Red;
    br_uint_16* colorMap_Green;
    br_uint_16* colorMap_Blue;

    tif = TIFFOpen(path, "r");
    if (tif == NULL) {
        *errorCode = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *errorCode = 2;
        return NULL;
    }
    if (samples_per_pixel == 1) {
        for (y = 0; y < height; y++) {
            TIFFReadScanline(tif, (br_uint_8*)pm->pixels + y * pm->row_bytes, y, 0);
        }
    } else {
        scanlineBuffer = malloc(TIFFScanlineSize(tif));
        if (scanlineBuffer == NULL) {
            TIFFClose(tif);
            BrPixelmapFree(pm);
            *errorCode = 2;
            return NULL;
        }
        for (y = 0; y < height; y++) {
            if (TIFFReadScanline(tif, scanlineBuffer, y, 0) < 1) {
                break;
            }
            curSrcPos = scanlineBuffer;
            for (x = 0; x < width; x++) {
                if (curSrcPos[1] < 0x80) {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = 0;
                } else {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = curSrcPos[0];
                }
                curSrcPos += samples_per_pixel;
            }
        }
        free(scanlineBuffer);
    }
    if (y < height) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *errorCode = 4;
        return NULL;
    }
    if ((flags & kLoadTextureFlags_KeepShadeTable) != 0) {
        TIFFGetField(tif, TIFFTAG_COLORMAP, &colorMap_Red, &colorMap_Green, &colorMap_Blue);
        pm->map = CreatePalettePixelmapFromRGBChannels(colorMap_Red, colorMap_Green, colorMap_Blue, flags & kLoadTextureFlags_PalatteRGB555);
        if (pm->map == NULL) {
            TIFFClose(tif);
            BrPixelmapFree(pm);
            *errorCode = 2;
            return NULL;
        }
        BrResAdd(pm, pm->map);
    }
    TIFFClose(tif);
    return pm;
}

// FUNCTION: CARMA2_HW 0x004864a0
br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_16BitRGB(const char *path, int flags, int *errorCode) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanlineBuffer;
    br_uint_8* curSrcPos;
    br_uint_32 x;
    br_uint_32 y;

    tif = TIFFOpen(path, "r");
    if (tif == NULL) {
        *errorCode = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate((samples_per_pixel == 3) ? BR_PMT_RGB_565 : ((flags & kLoadTextureFlags_UseARGB1555) ? BR_PMT_ARGB_1555 : BR_PMT_RGBA_4444), width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *errorCode = 2;
        return NULL;
    }
    scanlineBuffer = malloc(TIFFScanlineSize(tif));
    if (scanlineBuffer == NULL) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *errorCode = 2;
        return NULL;
    }

    for (y = 0; y < height; y++) {
        if (TIFFReadScanline(tif, scanlineBuffer, y, 0) < 1) {
            break;
        }
        curSrcPos = scanlineBuffer;
        if (samples_per_pixel == 3) {
            // RGB888 -> RGB565
            for (x = 0; x < width; x++) {
                *((br_uint_16*)((br_uint_8*)pm->pixels + y * pm->row_bytes + sizeof(br_uint_16) * x)) = ((curSrcPos[0] >> 3) << 11) | ((curSrcPos[1] >> 2) << 5) | (curSrcPos[2] >> 3);
                curSrcPos += 3;
            }
        }
        else if (flags & kLoadTextureFlags_UseARGB1555) {
            // RGBA8888 -> ARGB1555?
            for (x = 0; x < width; x++) {
                *((br_uint_16*)((br_uint_8*)pm->pixels + y * pm->row_bytes + sizeof(br_uint_16) * x)) = ((curSrcPos[3] >> 7) << 15) | ((curSrcPos[0] >> 3) << 10) | ((curSrcPos[1] >> 3) << 5) | (curSrcPos[2] >> 3);
                curSrcPos += 4;
            }
        } else {
            // RGBA8888 -> ARGB4444?
            for (x = 0; x < width; x++) {
                *((br_uint_16*)((br_uint_8*)pm->pixels + y * pm->row_bytes + sizeof(br_uint_16) * x)) = ((curSrcPos[3] >> 4) << 12) | ((curSrcPos[0] >> 4) << 8) | ((curSrcPos[1] >> 4) << 4) | (curSrcPos[2] >> 4);
                curSrcPos += 4;
            }
        }
    }
    free(scanlineBuffer);
    TIFFClose(tif);
    if (y < height) {
        BrPixelmapFree(pm);
        *errorCode = 4;
        return NULL;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00485750
br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex2(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode, int useTiffx) {
    int usePix16;
    tPath_name tifPath;
    tPath_name pixPath;
    tPath_name pixPathTarget;
    tPath_name trgbPath;
    tPath_name path;
    int pixPathIsLink;
    int tifPathIsLink;
    int pixExists;
    int tifExists;
    br_pixelmap* realPalette;
    br_pixelmap* texture;
    br_filesystem* prevFilesystem;
    tPath_name buf1;
    int indexSuffix;
    int indexLastSeparator;
    size_t posDir;

    sprintf(tifPath, "%s%s%s%s%s%s", texturePathDir, gDir_separator, useTiffx ? "TIFFX" : "TIFFRGB", gDir_separator, textureName, ".TIF");
    usePix16 = flags & kLoadTextureFlags_16bbp;
    sprintf(pixPath, "%s%s%s%s%s%s", texturePathDir, gDir_separator, usePix16 ? "PIX16" : "PIX8", gDir_separator, textureName, ".PIX");
    tifPathIsLink = ResolveTexturePathLink(tifPath, tifPath);
    pixPathIsLink = ResolveTexturePathLink(pixPathTarget, pixPath);
    pixExists = IsValidFile(pixPathIsLink ? pixPathTarget : pixPath);
    tifExists = IsValidFile(tifPath);

    if (useTiffx && pixExists) {
        sprintf(trgbPath, "%s%s%s%s%s%s", texturePathDir, gDir_separator, "TIFFRGB", gDir_separator, textureName, ".TIF");
        ResolveTexturePathLink(trgbPath, trgbPath);
        if (IsValidFile(trgbPath)) {
            if (GetLastModificationTime(pixPathIsLink ? pixPathTarget : pixPath) < GetLastModificationTime(trgbPath)) {
                return NULL;
            }
        }
    }
    if (!tifExists) {
        if (pixExists && (usePix16 || useTiffx)) {
            prevFilesystem = BrFilesystemSet(&gZlibBrFileSystem);
            texture = BrPixelmapLoad(pixPathIsLink ? pixPathTarget : pixPath);
            BrFilesystemSet(prevFilesystem);
            *errorCode = (texture != NULL) ? 0 : 3;
            return texture;
        }
        *errorCode = 1;
        return NULL;
    }

    if ((usePix16 || useTiffx) && ((flags & kLoadTextureFlags_ForceTiff) == 0) && pixExists) {
        if (GetLastModificationTime(tifPath) <= GetLastModificationTime(pixPathIsLink ? pixPathTarget : pixPath)) {
            prevFilesystem = BrFilesystemSet(&gZlibBrFileSystem);
            texture = BrPixelmapLoad(pixPathIsLink ? pixPathTarget : pixPath);
            BrFilesystemSet(prevFilesystem);
            *errorCode = (texture != NULL) ? 0 : 3;
            return texture;
        }
    }

    if (useTiffx) {
        texture = LoadTiffTexture_WithShadeTable(tifPath, flags, errorCode);
    } else if (usePix16) {
        texture = LoadTiffTexture_16BitRGB(tifPath, flags, errorCode);
    } else {
        if (pPalette != NULL) {
            realPalette = pPalette;
        } else {
            realPalette = Read_DEFAULT_ACT(texturePathDir, flags, errorCode);
            if (realPalette == NULL) {
                return NULL;
            }
        }

        texture = LoadTiffTexture_MappedToShadeTable(tifPath, realPalette, errorCode);
        if (texture == NULL || ((flags & kLoadTextureFlags_KeepShadeTable) == 0)) {
            if (pPalette == NULL) {
                BrPixelmapFree(realPalette);
            }
        } else {
            texture->map = realPalette;
            if (pPalette == NULL) {
                BrResAdd(texture, realPalette);
            }
        }
    }
    if (texture == NULL) {
        return NULL;
    }
    if ((flags & kLoadTextureFlags_SaveBrenderTexture) != 0) {
        if (!pixPathIsLink) {
            sprintf(path, "%s%s%s", texturePathDir, gDir_separator, usePix16 ? "PIX16" : "PIX8");
            PDmkdir(path);
            if (tifPathIsLink && !pixExists) {
                if (FindLastOccurrenceOfString_CaseInsensitive(&indexSuffix, tifPath, strlen(tifPath), ".TIF")) {
                    if (FindLastOccurrenceOfString_CaseInsensitive(&indexLastSeparator, tifPath, indexSuffix, gDir_separator)) {
                        int texRootIndex;
                        if (FindLastOccurrenceOfString_CaseInsensitive(&texRootIndex, tifPath, indexLastSeparator, gDir_separator)) {
                            posDir = strlen(gDir_separator) + indexLastSeparator;
                            sprintf(buf1, "%.*s%s%s", texRootIndex, tifPath, gDir_separator, usePix16 ? "PIX16" : "PIX8");
                            sprintf(pixPathTarget, "%s%s%.*s%s", buf1, gDir_separator, indexSuffix - posDir, tifPath + posDir, ".PIX");
                        }
                    }
                }
                PDmkdir(buf1);
                CreatePathLink(pixPathTarget, pixPath);
                pixPathIsLink = 1;
            }
        }
        if ((flags & kLoadTextureFlags_SaveTextureCompressed) == 0) {
            prevFilesystem = BrFilesystemSet(&gZlibBrFileSystem);
        }
        BrPixelmapSave(pixPathIsLink ? pixPathTarget : pixPath, texture);
        if ((flags & kLoadTextureFlags_SaveTextureCompressed) == 0) {
            BrFilesystemSet(prevFilesystem);
        }
    }
    if (texture != NULL) {
        *errorCode = 0;
    }
    return texture;
}

// FUNCTION: CARMA2_HW 0x004856c0
br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode) {
    br_pixelmap* texture;

    texture = NULL;
    if ((flags & kLoadTextureFlags_16bbp) == 0) {
        texture = LoadTiffTexture_Ex2(texturePathDir, textureName, pPalette, flags, errorCode, 1);
    }
    if (texture == NULL) {
        texture = LoadTiffTexture_Ex2(texturePathDir, textureName, pPalette, flags, errorCode, 0);
    }
    if (texture == NULL) {
        return NULL;
    }
    texture->identifier = BrResStrDup(texture, textureName);
    if (texture->identifier == NULL) {
        BrPixelmapFree(texture);
        *errorCode = 2;
        return NULL;
    }
    return texture;
}

// FUNCTION: CARMA2_HW 0x0048eb80
br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode) {
    br_pixelmap* textures[1000];
    int nb;

    if (gDisableTiffConversion) {
        nb = LoadBunchOfPixies(texturePathDir, textureName, textures, REC2_ASIZE(textures));
        if (nb == 0) {
            *errorCode = 1;
            return NULL;
        } else {
            *errorCode = 0;
            return textures[0];
        }
    } else {
        return LoadTiffTexture_Ex(texturePathDir, textureName, pPalette, flags | kLoadTextureFlags_SaveTextureCompressed, errorCode);
    }
}

// FUNCTION: CARMA2_HW 0x00514570
int C2_HOOK_FASTCALL DRPixelmapLoadMany(const char* texturePathNoExt, br_pixelmap** pixelmaps, size_t capacity) {
    tPath_name texturePath;
    tPath_name texturePathDir;
    tPath_name texturePathStem;
    int errorCode;

    strcpy(texturePath, texturePathNoExt);
    strcat(texturePath, ".TIF");
    SepDirAndFilename(texturePath, texturePathDir, texturePathStem);
    pixelmaps[0] = DRLdImg(texturePathDir, texturePathStem, gRender_palette, gPixelFlags, &errorCode);
    return (pixelmaps[0] != NULL && errorCode == 0) ? 1 : 0;
}

// FUNCTION: CARMA2_HW 0x005024f0
int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path) {
    int i;
    int nbLoaded;
    int result;
    tPath_name path_dirname;
    tPath_name path_stem;
    tAdd_to_storage_result addResult;
    br_pixelmap* pixelmaps[500];

    result = 0;
    if (gDisableTiffConversion) {
        SepDirAndFilename(path, path_dirname, path_stem);
        nbLoaded = LoadBunchOfPixies(path_dirname, path_stem, pixelmaps, REC2_ASIZE(pixelmaps));
    } else {
        nbLoaded = DRPixelmapLoadMany(path, pixelmaps, REC2_ASIZE(pixelmaps));
    }
    if (nbLoaded == 0) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, path);
    }
    for (i = 0; i < nbLoaded; i++) {
        addResult = AddPixelmapToStorage(pStorage_space, pixelmaps[i]);
        switch (addResult) {
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientPixelmapSlots);
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicatePixelmap_S, pixelmaps[i]->identifier);
            }
            BrPixelmapFree(pixelmaps[i]);
            break;
        case eStorage_allocated:
            BrMapAdd(pixelmaps[i]);
            result += 1;
            break;
        }
    }
    return result;
}

void C2_HOOK_FASTCALL TwatPIX16(const char* pPath) {

}

// FUNCTION: CARMA2_HW 0x005028f0
void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path) {
    tPath_name pathCopy;
    tPath_name tempPath;
    tName_list list;
    size_t i;

    C2_HOOK_BUG_ON(sizeof(tBrender_storage) != 68);

    TwatPIX16(path);
    list.size = 0;
    strcpy(pathCopy, path);
    gStorage_for_callbacks = pStorage_space;
    if (gDisableTiffConversion) {
        PFForEveryFile2(pathCopy, (tEnumPathCallback)GetAdditionalFileName, &list);
    }
    if (!gDisableTiffConversion) {
        PathCat(tempPath, pathCopy, "TIFFX");
        PFForEveryFile2(tempPath, (tEnumPathCallback)GetFileName, &list);
    }
    PathCat(tempPath, pathCopy, "PIX8");
    PFForEveryFile2(tempPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    if (!gDisableTiffConversion) {
        PathCat(tempPath, pathCopy, "TIFFRGB");
        PFForEveryFile2(tempPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    }
    PathCat(tempPath, pathCopy, "PIX16");
    PFForEveryFile2(tempPath, (tEnumPathCallback)GetAdditionalFileName, &list);
    for (i = 0; i < list.size; i++) {
        PathCat(tempPath, path, list.items[i]);
        AddPixelmaps(gStorage_for_callbacks, tempPath);
    }
}

// FUNCTION: CARMA2_HW 0x00486860
int C2_HOOK_FASTCALL LoadTiffTextureCB(const char* filePath, tLoadDirectoryStructureCBData* data) {
    size_t filePathLength;
    tPath_name textureName;
    tLoadDirectoryStructureCBResult* itemResult;
    br_pixelmap* texture;

    filePathLength = strlen(filePath);
    if (DRstrcmpi(filePath + filePathLength - 4, ".TIF") != 0) {
        return 0;
    }
    sprintf(textureName, "%.*s", filePathLength - 4, filePath);
    if (!data->isTiffx && data->results != NULL) {
        for (itemResult = data->results; itemResult != NULL; itemResult = itemResult->next) {
            if (DRstrcmpi(textureName, itemResult->name) == 0) {
                return 0;
            }
        }
    }
    if (!(data->loadFlags & kLoadTextureFlags_16bbp)) {
        texture = LoadTiffTexture_Ex2(data->directory, textureName, data->pPalette, data->loadFlags, data->errorCode, 1);
        if (texture == NULL) {
            texture = LoadTiffTexture_Ex2(data->directory, textureName, data->pPalette, data->loadFlags, data->errorCode, 0);
        }
        if (texture != NULL) {
            texture->identifier = BrResStrDup(texture, textureName);
            if (texture->identifier == NULL) {
                BrPixelmapFree(texture);
                *data->errorCode = 2;
                texture = NULL;
            }
        }
        if (texture != NULL) {
            BrPixelmapFree(texture);
        }
    }
    if (*data->errorCode != 0) {
        return *data->errorCode;
    }
    if (!data->isTiffx) {
        return 0;
    }
    itemResult = BrMemAllocate(sizeof(*itemResult), BR_MEMORY_APPLICATION);
    if (itemResult == NULL) {
        *data->errorCode = 2;
        return 2;
    }
    itemResult->name = BrMemStrDup(textureName);
    if (itemResult->name == NULL) {
        *data->errorCode = 2;
        return 2;
    }
    itemResult->next = data->results;
    data->results = itemResult;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00486760
void C2_HOOK_FASTCALL LoadAllTiffTexturesInDirectory(const char* directory, br_pixelmap* pPalette, int loadFlags, int* errorCode) {
    tLoadDirectoryStructureCBData data;
    tLoadDirectoryStructureCBResult* nextItem;
    tPath_name pathBuffer;

    data.loadFlags = loadFlags | kLoadTextureFlags_SaveBrenderTexture;
    data.errorCode = errorCode;
    data.results = NULL;
    data.directory = directory;
    data.pPalette = pPalette;
    if (!(loadFlags & kLoadTextureFlags_16bbp)) {
        sprintf(pathBuffer, "%s%s%s", directory, gDir_separator, "TIFFX");
        data.isTiffx = 0x1;
        PDEnumPath(pathBuffer, (tEnumPathCallback)LoadTiffTextureCB, &data);
    }
    if (*errorCode != 0) {
        return;
    }
    sprintf(pathBuffer, "%s%s%s", directory, gDir_separator, "TIFFRGB");
    data.isTiffx = 0x0;
    PDEnumPath(pathBuffer, (tEnumPathCallback)LoadTiffTextureCB, &data);

    while (data.results != NULL) {
        nextItem = data.results->next;
        if (data.results->name != NULL) {
            BrMemFree(data.results->name);
        }
        BrMemFree(data.results);
        data.results = nextItem;
    }
}

void UseNativeDirSeparator(char* nativePath, const char* path) {
    size_t i;
    size_t len;
    char c;

    len = strlen(path);
    for (i = 0; i < len; i++) {
        c = path[i];
        if (c == '\\') {
            c = *gDir_separator;
        }
        nativePath[i] = c;
    }
    nativePath[len] = '\0';
}

// FUNCTION: CARMA2_HW 0x004869e0
int C2_HOOK_FASTCALL ResolveTexturePathLink(char* realPath, const char* path) {
    FILE* f;
    tPath_name linkPath;
    tPath_name nativeLinkPath;

    f = DRfopen(path, "rt");
    if (f == NULL) {
        return 0;
    }
    PFfclose(f);
    GetALineAndDontArgue(f, linkPath);
    if (strstr(linkPath, ".TIF") != NULL) {
        UseNativeDirSeparator(nativeLinkPath, linkPath);
    } else if (strstr(linkPath, ".PIX") != NULL) {
        UseNativeDirSeparator(nativeLinkPath, linkPath);
    } else {
        return 0;
    }
    PathCat(linkPath, gApplication_path, nativeLinkPath);
    strcpy(realPath, linkPath);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00486b20
int C2_HOOK_FASTCALL CreatePathLink(const char* targetPath, const char* linkPath) {
    tPath_name buffer;
    tPath_name cleanedTargetRelPath;
    const char* targetRelPath;
    char c;
    int pos;
    FILE *f;

    strcpy(buffer, targetPath);
    targetRelPath = strstr(buffer, "DATA") + strlen("DATA") + 1;
    pos = 0;
    while ((c = *targetRelPath) != '\0') {
        if (c == ':' || c == '\\') {
            c = '\\';
        }
        cleanedTargetRelPath[pos] = c;
        targetRelPath++;
        pos++;
    }
    cleanedTargetRelPath[pos] = '\0';
    f = (FILE*)DRfopen(linkPath, "wt");
    if (f == NULL) {
        return 0;
    }
    fputs(cleanedTargetRelPath, f);
    fclose(f);
    return 1;

}

// FUNCTION: CARMA2_HW 0x0047d850
int C2_HOOK_FASTCALL DRstrcmpi(const char* str1, const char* str2) {
    return strcasecmp(str1, str2);
}

// FUNCTION: CARMA2_HW 0x0047d860
int C2_HOOK_FASTCALL DRmemicmp(const char* str1, const char* str2, size_t count) {
    return c2_memicmp(str1, str2, count);
}

// FUNCTION: CARMA2_HW 0x00486240
int C2_HOOK_FASTCALL FindLastOccurrenceOfString_CaseInsensitive(int* offset, const char* haystack, size_t haystackLen, const char* needle) {
    size_t needleLen;
    size_t haystackPos;

    needleLen = strlen(needle);
    if (haystackLen < needleLen) {
        return 0;
    }
    haystackPos = haystackLen - needleLen;
    while (1) {
        if (DRmemicmp(&haystack[haystackPos], needle, needleLen) == 0) {
            *offset = haystackPos;
            return 1;
        }
        if (haystackPos == 0) {
            return 0;
        }
        haystackPos--;
    }
}

// FUNCTION: CARMA2_HW 0x005010e0
tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st) {
    int i;

    if (pStorage_space->shade_tables_count >= pStorage_space->max_shade_tables) {
        return eStorage_not_enough_room;
    }

    for (i = 0; i < pStorage_space->shade_tables_count; i++) {
        if (pStorage_space->shade_tables[i]->identifier
            && pThe_st->identifier
            && strcmp(pStorage_space->shade_tables[i]->identifier, pThe_st->identifier) == 0) {
            return eStorage_duplicate;
        }
    }
    pStorage_space->shade_tables[pStorage_space->shade_tables_count] = pThe_st;
    pStorage_space->shade_tables_count++;
    return eStorage_allocated;
}

// FUNCTION: CARMA2_HW 0x00502b80
void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath) {
    br_pixelmap* temp_array[50];
    int total;
    int i;
    tBrender_storage* storage_space;
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".TAB") == NULL) {
        return;
    }
    storage_space = gStorage_for_callbacks;
    total = BrPixelmapLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadShadeTableFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddShadeTableToStorage(storage_space, temp_array[i])) {
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientShadeTableSlots);
            break;

        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicatePixelmap_S, temp_array[i]->identifier);
            }
            BrPixelmapFree(temp_array[i]);
            break;
        case eStorage_allocated:
            BrTableAdd(temp_array[i]);
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00502b60
void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAShadeTable);
}

// FUNCTION: CARMA2_HW 0x005024b0
void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".PIX") != NULL) {
        AddPixelmaps(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502490
void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAPixelmap);
}

// FUNCTION: CARMA2_HW 0x00501560
br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName) {
    br_pixelmap* map;
    tAdd_to_storage_result addResult;

    map = LoadPixelmap(pName);
    if (map == NULL) {
        return BrMapFind(pName);
    }

    addResult = AddPixelmapToStorage(pStorage, map);
    switch (addResult) {
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientPixelmapSlots);
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicatePixelmap_S, map->identifier);
            }
            BrPixelmapFree(map);
            return gDuplicate_pixelmap;
        case eStorage_allocated:
            BrMapAdd(map);
            return map;
        default:
            return NULL;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00502a70
void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath) {
    tRendererShadingType shading;
    char s[256];

    shading = gMaterial_shading_for_callback;
    if (gMaterial_shading_for_callback == kRendererShadingType_Undefined) {
        shading = kRendererShadingType_Default;
    }
    Uppercaseificate(s, pPath);
    if (strstr(s, ".MAT") != NULL) {
        AddMaterials(gStorage_for_callbacks, pPath, shading);
    }
}

// FUNCTION: CARMA2_HW 0x00502ad0
void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    gMaterial_shading_for_callback = pShading;
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
}

// FUNCTION: CARMA2_HW 0x00501190
tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat) {
    int i;

    gDuplicate_material = NULL;
    if (pStorage_space->materials_count >= pStorage_space->max_materials) {
        return eStorage_not_enough_room;
    }
    for (i = 0; i < pStorage_space->materials_count; i++) {
        if (pStorage_space->materials[i]->identifier
            && pThe_mat->identifier
            && !strcmp(pStorage_space->materials[i]->identifier, pThe_mat->identifier)) {
            gDuplicate_material = pStorage_space->materials[i];
            return eStorage_duplicate;
        }
    }
    pStorage_space->materialProps[pStorage_space->materials_count] = 0; /* FIXME */
    pStorage_space->materials[pStorage_space->materials_count] = pThe_mat;
    pStorage_space->materials_count++;
    return eStorage_allocated;
}

// FUNCTION: CARMA2_HW 0x00502060
int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading) {
    int i;
    int new_ones;
    int total;
    br_material* temp_array[500];

    new_ones = 0;
    total = BrMaterialLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadMaterialFileOrItIsEmpty_S, pPath);
    }
    GlorifyMaterial(temp_array, total, pShading);
    for (i = 0; i < total; ++i) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddMaterialToStorage(pStorage_space, temp_array[i])) {
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientMaterialSlots);
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicateMaterial_S, temp_array[i]->identifier);
            }
            BrMaterialFree(temp_array[i]);
            break;
        case eStorage_allocated:
            BrMaterialAdd(temp_array[i]);
            new_ones++;
            break;
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x005017e0
br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName) {
    br_material* temp;

    temp = LoadMaterial(pName);
    if (temp == NULL) {
        return BrMaterialFind(pName);
    }

    switch (AddMaterialToStorage(pStorage_space, temp)) {
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientMaterialSlots);
            break;

        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicateMaterial_S, temp->identifier);
            }
            BrMaterialFree(temp);
            return gDuplicate_material;

        case eStorage_allocated:
            BrMaterialAdd(temp);
            return temp;
    }

    return NULL;
}

// FUNCTION: CARMA2_HW 0x00501fe0
void C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage *pStorage, FILE* pFile, tRendererShadingType pShading) {
    char s1[256];
    char s2[256];

    PossibleService();
    GetALineAndDontArgue(pFile, s1);
    PathCat(s2, gApplication_path, "MATERIAL");
    PathCat(s2, s2, strtok(s1, "\t ,/"));
    AddMaterials(pStorage, s2, pShading);
}

// FUNCTION: CARMA2_HW 0x0048ef40
br_pixelmap* C2_HOOK_FASTCALL LoadShadeTable(const char* pName) {
    tPath_name the_path;

    PathCat(the_path, gApplication_path, "SHADETAB");
    PathCat(the_path, the_path, pName);
    return BrPixelmapLoad(the_path);
}

// FUNCTION: CARMA2_HW 0x005016a0
br_pixelmap* C2_HOOK_FASTCALL LoadSingleShadeTable(tBrender_storage* pStorage_space, const char* pName) {
    br_pixelmap* temp;

    temp = LoadShadeTable(pName);
    if (temp == NULL) {
        return BrTableFind(pName);
    }

    switch (AddShadeTableToStorage(pStorage_space, temp)) {
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientShadeTableSlots);
        break;

    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicatePixelmap_S, temp->identifier);
        }
        BrPixelmapFree(temp);
        return BrTableFind(pName);

    case eStorage_allocated:
        BrTableAdd(temp);
        return temp;
    }

    return NULL;
}

// FUNCTION: CARMA2_HW 0x00501260
tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod) {
    int i;

    gDuplicate_model = NULL;
    if (pStorage_space->materials_count >= pStorage_space->max_models) {
        return eStorage_not_enough_room;
    }
    if (pStorage_space->flags & 0x1) { /* FIXME: add enum (0x1 -> eStorage_space_flags_No_duplicates*/
        for (i = 0; i < pStorage_space->models_count; i++) {
            if (pStorage_space->models[i] != NULL
                && pStorage_space->models[i]->identifier != NULL
                && pThe_mod->identifier != NULL
                && strcmp(pStorage_space->models[i]->identifier, pThe_mod->identifier) == 0) {
                gDuplicate_model = pStorage_space->models[i];
                return eStorage_duplicate;
            }
        }
    }
    pStorage_space->models[pStorage_space->models_count] = pThe_mod;
    pStorage_space->models_count++;
    return eStorage_allocated;
}

// FUNCTION: CARMA2_HW 0x00501e40
int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath) {
    int i;
    int new_ones;
    int total;
    br_model* temp_array[2000];

    new_ones = 0;
    total = BrModelLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    WhitenVertexRGB(temp_array, total);
    if (total == 0) {
        FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddModelToStorage(pStorage_space, temp_array[i])) {
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientModelSlots);
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicateModel_S, temp_array[i]->identifier);
            }
            BrModelFree(temp_array[i]);
            break;
        case eStorage_allocated:
            temp_array[i]->flags |= BR_MODF_UPDATEABLE;
            BrModelAdd(temp_array[i]);
            new_ones++;
            break;
        }
    }

    return new_ones;
}

// FUNCTION: CARMA2_HW 0x00502b20
void C2_HOOK_FASTCALL LoadIfItsAModel(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".DAT") != NULL) {
        AddModels(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b00
void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAModel);
}

// FUNCTION: CARMA2_HW 0x00502cf0
void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    LoadAllShadeTablesInDirectory(pStorage, pPath);
    LoadAllPixelmapsInDirectory(pStorage, pPath);
    LoadAllImagesInDirectory(pStorage, pPath);
    LoadAllMaterialsInDirectory(pStorage, pPath, pShading);
    LoadAllModelsInDirectory(pStorage, pPath);
}

// FUNCTION: CARMA2_HW 0x00502d60
void C2_HOOK_FASTCALL DisallowDuplicates(void) {
}

// FUNCTION: CARMA2_HW 0x00502d70
void C2_HOOK_FASTCALL AllowDuplicates(void) {

    gDisallow_duplicates = 0;
}

// FUNCTION: CARMA2_HW 0x00502d80
br_pixelmap* C2_HOOK_FASTCALL SwapPixelmapInStorage(br_pixelmap* pNew, br_pixelmap* pOriginal, tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->pixelmaps_count; i++) {
        if (pStorage->pixelmaps[i] == pOriginal) {
            BrMapRemove(pOriginal);
            BrPixelmapFree(pOriginal);
            pStorage->pixelmaps[i] = pNew;
            BrMapAdd(pNew);
            return pNew;
        }
    }
    PDFatalError("Can't swap in pixelmap");
}

// FUNCTION: CARMA2_HW 0x004475c0
int C2_HOOK_FASTCALL DRPixelmapHasZeros(br_pixelmap* pm) {
    int x;
    int y;
    tU8* row_ptr;
    tU8* pp;

    if (pm->flags & BR_PMF_NO_ACCESS) {
        return 1;
    }
    row_ptr = (tU8*)pm->pixels + (pm->row_bytes * pm->base_y) + pm->base_x;
    for (y = 0; y < pm->height; y++) {
        pp = row_ptr;
        for (x = 0; x < pm->width; x++) {
            if (*pp == '\0') {
                return 1;
            }
            pp++;
        }
        row_ptr += pm->row_bytes;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004479c0
int C2_HOOK_FASTCALL StorageContainsPixelmap(tBrender_storage* pStorage, br_pixelmap* pMap) {
    int i;

    for (i = 0; i < pStorage->pixelmaps_count; i++) {

        if (pStorage->pixelmaps[i] == pMap) {
            return 1;
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL HideStoredTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materials[i]->colour_map;

        if (colour_map != NULL && StorageContainsPixelmap(pStorage, colour_map)) {
            pStorage->materialProps[i] = colour_map;
            pStorage->materials[i]->colour_map = NULL;
            pStorage->materials[i]->flags &= ~BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

void C2_HOOK_FASTCALL RevealStoredTransparentTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materialProps[i];

        if (colour_map != NULL && DRPixelmapHasZeros(colour_map)) {
            pStorage->materials[i]->colour_map = pStorage->materialProps[i];
            pStorage->materialProps[i] = NULL;
            pStorage->materials[i]->flags |= BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

void C2_HOOK_FASTCALL HideStoredOpaqueTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materials[i]->colour_map;

        if (colour_map != NULL && StorageContainsPixelmap(pStorage, colour_map) && !DRPixelmapHasZeros(colour_map)) {
            pStorage->materialProps[i] = colour_map;
            pStorage->materials[i]->colour_map = NULL;
            pStorage->materials[i]->flags &= ~BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

void C2_HOOK_FASTCALL RevealStoredTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materialProps[i];

        if (colour_map != NULL) {
            pStorage->materials[i]->colour_map = colour_map;
            pStorage->materialProps[i] = NULL;
            pStorage->materials[i]->flags |= BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00447350
void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld) {
    switch (pNew) {
    case eCTL_none:
        HideStoredTextures(pStorage);
        break;
    case eCTL_transparent:
        switch (pOld) {
        case eCTL_none:
            RevealStoredTransparentTextures(pStorage);
            break;
        case eCTL_full:
            HideStoredOpaqueTextures(pStorage);
            break;
        default:
            break;
        }
        break;
    case eCTL_full:
        RevealStoredTextures(pStorage);
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00474880
void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon) {
    gSight_distance_squared = pYon * 1.02f * pYon * 1.02f;
}

// FUNCTION: CARMA2_HW 0x004761e0
br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData) {
    int i;
    tFunkotronic_spec* the_funk = pData;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, material, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_count, 0x150);

    if (pActor->model != NULL) {
        for (i = 0; i < pActor->model->nfaces; i++) {
            br_face* the_face;

            the_face = &pActor->model->faces[i];
            if (the_face->material == the_funk->material) {
                the_funk->proximity_count += 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00476230
br_uint_32 C2_HOOK_FASTCALL AddProximities(br_actor* pActor, br_material* pMat, void* pData) {
    tFunkotronic_spec* the_funk = pData;

    if (pActor->model != NULL) {
        int i;
        for (i = 0; i < pActor->model->nfaces; i++) {
            br_face* the_face = &pActor->model->faces[i];
            if (the_face->material == the_funk->material) {
                int j;

                for (j = 0; j < 3; j++) {
                    BrVector3Copy(
                        &the_funk->proximity_array[the_funk->proximity_count].v[j],
                        &pActor->model->vertices[the_face->vertices[j]].p);
                    gFunk_temp_vertices[the_funk->proximity_count].vertices[j] = &pActor->model->vertices[the_face->vertices[j]];
                }
                BrVector3Normalise(&the_funk->proximity_array[the_funk->proximity_count].n, &the_face->n);
                the_funk->proximity_array[the_funk->proximity_count].d = -the_face->d;
                the_funk->proximity_count += 1;
            }
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL ShiftBoundGrooveFunks(char* pStart, char* pEnd, ptrdiff_t pDelta) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gGroove_funk_bindings) != 1440);
    C2_HOOK_BUG_ON(sizeof(tGroove_funk_binding) != 0x8);

    for (i = 0; i < REC2_ASIZE(gGroove_funk_bindings); i++) {
        if (pStart <= (char*)gGroove_funk_bindings[i].v && (char*)gGroove_funk_bindings[i].v < pEnd) {
            gGroove_funk_bindings[i].v = (float*)((char*)gGroove_funk_bindings[i].v + (pDelta & ~(sizeof(void*) - 1)));
        }
    }
}

tFunkotronic_spec* C2_HOOK_FASTCALL AddNewFunkotronic(void) {
    void* new_array;
    int i;

    C2_HOOK_BUG_ON(sizeof(tFunkotronic_spec) != 0x158);
    C2_HOOK_BUG_ON(sizeof(tFunk_proximity) != 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, owner, 0x0);

    for (i = 0; i < gFunkotronics_array_size; i++) {
        if (gFunkotronics_array[i].owner == -999) {
            memset(&gFunkotronics_array[i], 0, sizeof(tFunkotronic_spec));
            return &gFunkotronics_array[i];
        }
    }
    gFunkotronics_array_size += 16;
    new_array = BrMemCalloc(gFunkotronics_array_size, sizeof(tFunkotronic_spec), kMem_funk_spec);
    if (gFunkotronics_array != NULL) {
        memcpy(new_array, gFunkotronics_array, (gFunkotronics_array_size - 16) * sizeof(tFunkotronic_spec));
        ShiftBoundGrooveFunks(
                (char*)gFunkotronics_array,
                (char*)&gFunkotronics_array[gFunkotronics_array_size - 16],
                (char*)new_array - (char*)gFunkotronics_array);
        BrMemFree(gFunkotronics_array);
    }
    gFunkotronics_array = new_array;
    for (i = 0; i < 16; i++) {
        gFunkotronics_array[gFunkotronics_array_size - 16 + i].owner = -999;
    }
    return &gFunkotronics_array[gFunkotronics_array_size - 16];
}

br_material* C2_HOOK_FASTCALL TryThisFunkLink(tCar_crush_buffer_entry* pFunk_link, const char* pStr, tFunkotronic_spec* pFunk) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_buffer_entry, count_smashables, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_buffer_entry, smashables, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_smashable_part, funk_material, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_smashable_part, material_name, 0x0);

    if (pFunk_link == NULL) {
        return NULL;
    }
    for (i = 0; i < pFunk_link->count_smashables; i++) {
        if (strcmp(pFunk_link->smashables[i].material_name, pStr) == 0) {
            pFunk_link->smashables[i].funk = pFunk - gFunkotronics_array;
            return pFunk_link->smashables[i].funk_material;
        }
    }
    return NULL;
}

br_material* C2_HOOK_FASTCALL FindSmashableMaterial(const char* pStr, tFunkotronic_spec* pFunk, tCar_crush_buffer* pCar_crush_datas) {
    int i;

    for (i = 0; i < pCar_crush_datas->count_entries; i++) {
        br_material* material;

        material = TryThisFunkLink(pCar_crush_datas->entries[i], pStr, pFunk);
        if (material != NULL) {
            return material;
        }
    }
    return NULL;
}

void C2_HOOK_FASTCALL AddFunkGrooveBinding(int pSlot_number, float* pPeriod_address) {

    if (pSlot_number < 0 || pSlot_number >= REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
    }

    gGroove_funk_bindings[pSlot_number].v = pPeriod_address;
    gGroove_funk_bindings[pSlot_number].field_0x4 = 0;
    *pPeriod_address = 0.f;
}

static void texture_string_to_bits(tU8 *bits, const char *s) {
    const char CHAR_BITS[] = "THBVLRF";
    unsigned int i;
    for (i = 0; i < strlen(s); i++) {
        unsigned int j;
        for (j = 0; j < strlen(CHAR_BITS); j++) {
            if (CHAR_BITS[j] == s[i]) {
                bits[i] = j;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00474ac0
void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas) {
    int first_time;
    char s[256];
    tFunkotronic_spec* the_funk;
    char* str;
    float x_0;
    float x_1;
    int g0;
    int g1;
    float speed1;
    float speed2;
    int i;
    float a_min;
    float d_min;
    float s_min;
    float a_max;
    float d_max;
    float s_max;
    float yon_factor;
    float fov_factor;

#ifndef REC2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, flags, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, material, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, mode, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_type, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mode, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_count, 0x150);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_array, 0x154);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.spin_info.period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.texture_info.data, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.rock_info.period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.rock_info.x_centre, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.rock_info.y_centre, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.rock_info.rock_angle, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.x_period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.y_period, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.x_centre, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.y_centre, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.x_magnitude, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.throb_info.y_magnitude, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.slither_info.x_period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.slither_info.y_period, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.slither_info.x_magnitude, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.slither_info.y_magnitude, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.roll_info.x_period, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, matrix_mod_data.roll_info.y_period, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, lighting_animation_type, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, lighting_animation_data.controlled.period, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, lighting_animation_data.texture_info.data, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, lighting_animation_data.rock_info.period, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, ambient_base, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, ambient_delta, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, direct_base, 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, direct_delta, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, specular_base, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, specular_delta, 0x4c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_type, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, time_mode, 0x54);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, last_frame, 0x58);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.mode, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.controlled.period, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.texture_info.data, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.rock_info.period, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.texture_count, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.current_frame, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.has_matrix, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.textures, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.frames_info.mat, 0x90);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_data, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_data_length, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_descriptor, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_descriptor.width, 0xa8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.flic_info.flic_descriptor.height, 0xac);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.camera_info.mode, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.camera_info.field_0x60, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.camera_info.count, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.camera_info.actors, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.mirror_info.yon, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.mirror_info.actor, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.mirror_info.mat, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, texture_animation_data.mirror_info.bounds, 0x98);
    C2_HOOK_BUG_ON(sizeof(tFunk_temp_buffer) != 0x30);
    C2_HOOK_BUG_ON(sizeof(tFunk_texturebits) != 0x28);
    C2_HOOK_BUG_ON(500 * sizeof(tFunk_temp_buffer) != 24000);
#endif

    gFunk_temp_vertices = BrMemAllocate(500 * sizeof(tFunk_temp_buffer), BR_MEMORY_APPLICATION);
    first_time = 1;
    while (!PFfeof(pF)) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF FUNK") == 0) {
            break;
        }
        if (!first_time) {
            if (strcmp(s, "NEXT FUNK") != 0) {
                FatalError(kFatalError_FunkotronicFile);
            }
            GetALineAndDontArgue(pF, s);
        }
        first_time = 0;
        str = strtok(s, "\t ,/");

        the_funk = AddNewFunkotronic();
        the_funk->owner = pOwner;
        the_funk->material = NULL;
        if (pCar_crush_datas != NULL) {
            the_funk->material = FindSmashableMaterial(s, the_funk, pCar_crush_datas);
        }
        if (the_funk->material == NULL) {
            the_funk->material = BrMaterialFind(str);
        }
        if (the_funk->material == NULL) {
            FatalError(kFatalError_CannotFindMainMaterialInFunkotronicFile_S, str);
        }
        str = strtok(NULL, "\t ,/");
        if (str != NULL && strcmp(str, "multiple") == 0 && the_funk->material->identifier != NULL) {
            BrResFree(the_funk->material->identifier);
            the_funk->material->identifier = NULL;
        }
        the_funk->mode = GetALineAndInterpretCommand(pF, gFunk_nature_names, REC2_ASIZE(gFunk_nature_names));
        the_funk->proximity_count = 0;
        the_funk->proximity_array = NULL;
        if (the_funk->mode == eFunk_mode_distance) {
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, CalcProximities, the_funk);
            the_funk->proximity_array = BrMemAllocate(the_funk->proximity_count * sizeof(tFunk_proximity), kMem_funk_prox_array);
            the_funk->proximity_count = 0;
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, AddProximities, the_funk);
        }
        the_funk->matrix_mod_type = GetALineAndInterpretCommand(pF, gFunk_type_names, REC2_ASIZE(gFunk_type_names));
        the_funk->flags &= ~0x1;
        if (the_funk->matrix_mod_type != eMatrix_mod_none) {
            the_funk->matrix_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }
        switch (the_funk->matrix_mod_type) {
        case eMatrix_mod_spin:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.spin_info.period);
                break;
            case eMove_texturebits:
                the_funk->matrix_mod_data.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_funk->matrix_mod_data.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_funk->matrix_mod_data.texture_info.data->bits, s);
                the_funk->matrix_mod_data.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_funk->matrix_mod_data.rock_info.period = (x_0 == 0.0f) ? 0.0f : 1000.0f / x_0;
                break;
            }
            break;
        case eMatrix_mod_rock:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.rock_info.period);
                break;
            case eMove_texturebits:
                the_funk->matrix_mod_data.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_funk->matrix_mod_data.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_funk->matrix_mod_data.texture_info.data->bits, s);
                the_funk->matrix_mod_data.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_funk->matrix_mod_data.rock_info.period = (x_0 == 0.0f) ? 0.0f : 1000.0f / x_0;
                break;
            }
            the_funk->matrix_mod_data.rock_info.rock_angle = GetAScalar(pF);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.rock_info.x_centre = x_0 / 100.0f;
            the_funk->matrix_mod_data.rock_info.y_centre = x_1 / 100.0f;
            break;
        case eMatrix_mod_throb:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.throb_info.x_period);
                }
                if (g1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g1, &the_funk->matrix_mod_data.throb_info.y_period);
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.throb_info.x_period = (speed1 == 0.0f) ? 0.0f : 1000.0f / speed1;
                the_funk->matrix_mod_data.throb_info.y_period = (speed2 == 0.0f) ? 0.0f : 1000.0f / speed2;
                break;
            }
            GetPairOfFloatPercents(pF,
                &the_funk->matrix_mod_data.throb_info.x_magnitude,
                &the_funk->matrix_mod_data.throb_info.y_magnitude);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.throb_info.x_centre = x_0 / 100.0f;
            the_funk->matrix_mod_data.throb_info.y_centre = x_1 / 100.0f;
            if (the_funk->matrix_mode != eMove_controlled) {
                if (the_funk->matrix_mod_data.throb_info.x_period == 0.0f) {
                    the_funk->matrix_mod_data.throb_info.x_period = 1.0f;
                    the_funk->matrix_mod_data.throb_info.x_magnitude = 0.0f;
                }
                if (the_funk->matrix_mod_data.throb_info.y_period == 0.0f) {
                    the_funk->matrix_mod_data.throb_info.y_period = 1.0f;
                    the_funk->matrix_mod_data.throb_info.y_magnitude = 0.0f;
                }
            }
            break;
        case eMatrix_mod_slither:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.slither_info.x_period);
                }
                if (g1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g1, &the_funk->matrix_mod_data.slither_info.y_period);
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.slither_info.x_period = (speed1 == 0.0f) ? 0.0f : 1000.0f / speed1;
                the_funk->matrix_mod_data.slither_info.y_period = (speed2 == 0.0f) ? 0.0f : 1000.0f / speed2;
                break;
            }
            GetPairOfFloatPercents(pF,
                &the_funk->matrix_mod_data.slither_info.x_magnitude,
                &the_funk->matrix_mod_data.slither_info.y_magnitude);
            if (the_funk->matrix_mode != eMove_controlled) {
                if (the_funk->matrix_mod_data.slither_info.x_period == 0.0f) {
                    the_funk->matrix_mod_data.slither_info.x_period = 1.0f;
                    the_funk->matrix_mod_data.slither_info.x_magnitude = 0.0f;
                }
                if (the_funk->matrix_mod_data.slither_info.y_period == 0.0f) {
                    the_funk->matrix_mod_data.slither_info.y_period = 1.0f;
                    the_funk->matrix_mod_data.slither_info.y_magnitude = 0.0f;
                }
            }
            break;
        case eMatrix_mod_roll:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.roll_info.x_period);
                }
                if (g1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g1, &the_funk->matrix_mod_data.roll_info.y_period);
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.roll_info.x_period = speed1 == 0.0f ? 0.0f : 1000.0f / speed1;
                the_funk->matrix_mod_data.roll_info.y_period = speed2 == 0.0f ? 0.0f : 1000.0f / speed2;
                break;
            }
            break;
        default:
            break;
        }
        the_funk->lighting_animation_type = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        if (the_funk->lighting_animation_type != eMove_none) {
            switch (the_funk->lighting_animation_type) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g1, &the_funk->lighting_animation_data.controlled.period);
                break;
            case eMove_texturebits:
                the_funk->lighting_animation_data.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_funk->lighting_animation_data.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_funk->lighting_animation_data.texture_info.data->bits, s);
                the_funk->lighting_animation_data.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_funk->lighting_animation_data.rock_info.period = (x_0 == 0.0f) ? 0.0f : 1000.0f / x_0;
                break;
            }
            GetThreeFloatPercents(pF, &a_min, &d_min, &s_min);
            GetThreeFloatPercents(pF, &a_max, &d_max, &s_max);
            the_funk->ambient_base = (a_min + a_max) / 2.0f;
            the_funk->direct_base = (d_min + d_max) / 2.0f;
            the_funk->specular_base = (s_min + s_max) / 2.0f;
            the_funk->ambient_delta = (a_max - a_min) / 2.0f;
            the_funk->direct_delta = (d_max - d_min) / 2.0f;
            the_funk->specular_delta = (s_max - s_min) / 2.0f;
        }
        the_funk->texture_animation_type = GetALineAndInterpretCommand(pF, gFunk_anim_names, REC2_ASIZE(gFunk_anim_names));
        if (the_funk->texture_animation_type != eTexture_animation_none
                && the_funk->texture_animation_type != eTexture_animation_mirror
                && the_funk->texture_animation_type != eTexture_animation_camera) {
            the_funk->time_mode = GetALineAndInterpretCommand(pF, gTime_mode_names, REC2_ASIZE(gTime_mode_names));
        }
        if (the_funk->texture_animation_type == eTexture_animation_flic && gAusterity_mode) {
            the_funk->texture_animation_type = eTexture_animation_none;
            GetALineAndDontArgue(pF, s);
        }
        the_funk->last_frame = 0.0f;
        switch (the_funk->texture_animation_type) {
        case eTexture_animation_frames:
            the_funk->texture_animation_data.frames_info.mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
            switch (the_funk->texture_animation_data.frames_info.mode) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->texture_animation_data.frames_info.controlled.period);
                break;
            case eMove_texturebits:
                the_funk->texture_animation_data.frames_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_funk->texture_animation_data.frames_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_funk->texture_animation_data.frames_info.texture_info.data->bits, s);
                the_funk->texture_animation_data.frames_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_funk->texture_animation_data.frames_info.rock_info.period = (x_0 == 0.0f) ? 0.0f : 1000.0F / x_0;
                break;
            }
            the_funk->texture_animation_data.frames_info.texture_count = (int)GetAScalar(pF);
            the_funk->texture_animation_data.frames_info.current_frame = 0;
            the_funk->texture_animation_data.frames_info.has_matrix = 0;
            for (i = 0; i < the_funk->texture_animation_data.frames_info.texture_count; i++) {
                GetALineAndDontArgue(pF, s);
                str = strtok(s, "\t ,/");
                the_funk->texture_animation_data.frames_info.textures[i] = BrMapFind(str);
                if (the_funk->texture_animation_data.frames_info.textures[i] == NULL) {
                    FatalError(kFatalError_CannotFindAnimationFramePixelmapReferencedInFunkotronicFile);
                }
                BrMatrix23Identity(&the_funk->texture_animation_data.frames_info.mat[i]);
                str = strtok(NULL, "\t ,/");
                if (str != NULL && strlen(str) != 0) {
                    int size_x;
                    int pos_x;
                    int size_y;
                    int pos_y;

                    the_funk->texture_animation_data.frames_info.has_matrix = 1;
                    sscanf(str, "%d", &size_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &size_y);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_y);
                    the_funk->texture_animation_data.frames_info.mat[i].m[0][0] = 1.f / (float)size_x;
                    the_funk->texture_animation_data.frames_info.mat[i].m[1][1] = 1.f / (float)size_y;
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][0] = (float)pos_x / (float)size_x;
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][1] = (float)pos_y / (float)size_y;
                }
            }
            break;
        case eTexture_animation_flic:
            GetAString(pF, s);
            the_funk->texture_animation_data.flic_info.flic_data = NULL;
            if (LoadFlicData(s, &the_funk->texture_animation_data.flic_info.flic_data, &the_funk->texture_animation_data.flic_info.flic_data_length)) {
                char *the_pixels;
                br_pixelmap *the_pixelmap;

                the_funk->texture_animation_data.flic_info.flic_descriptor.data_start = NULL;
                StartFlic(s, -1, &the_funk->texture_animation_data.flic_info.flic_descriptor, the_funk->texture_animation_data.flic_info.flic_data_length, (tS8*)the_funk->texture_animation_data.flic_info.flic_data, NULL, 0, 0, 0);
                the_funk->last_frame = 0.0f;
                the_pixels = BrMemAllocate(
                    the_funk->texture_animation_data.flic_info.flic_descriptor.height
                        * ((the_funk->texture_animation_data.flic_info.flic_descriptor.width + 3) & ~3),
                        kMem_video_pixels);
                if (gScreen->row_bytes < 0) {
                    BrFatal("C:\\Carma2\\Source\\Common\\Funkgroo.c", 732,
                            "Bruce bug at line %d, file C:\\Carma2\\Source\\Common\\Funkgroo.c", 732);
                }
                the_pixelmap = DRPixelmapAllocate(gScreen->type,
                    the_funk->texture_animation_data.flic_info.flic_descriptor.width,
                    the_funk->texture_animation_data.flic_info.flic_descriptor.height,
                    the_pixels,
                    0);
                AssertFlicPixelmap(&the_funk->texture_animation_data.flic_info.flic_descriptor, the_pixelmap);
                the_funk->material->colour_map = the_pixelmap;
                the_funk->material->user = the_pixelmap;
                BrMaterialUpdate(the_funk->material, BR_MATU_ALL);
            } else {
                the_funk->texture_animation_type = eTexture_animation_none;
            }
            break;
        case eTexture_animation_camera:
            the_funk->texture_animation_data.camera_info.mode = GetALineAndInterpretCommand(pF, gCamera_animation_names, REC2_ASIZE(gCamera_animation_names));
            GetPairOfFloats(pF, &yon_factor, &fov_factor);
            the_funk->texture_animation_data.camera_info.field_0x60 = GetAnInt(pF);
            the_funk->texture_animation_data.camera_info.count = GetAnInt(pF);
            for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                br_actor* actor;
                br_camera* camera;
                br_camera* global_camera;
                br_vector3 cam_pos;

                actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                the_funk->texture_animation_data.camera_info.actors[i] = actor;
                camera = actor->type_data;
                global_camera = gCamera->type_data;
                camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                camera->field_of_view = (br_angle)(fov_factor * global_camera->field_of_view);
                camera->hither_z = global_camera->hither_z;
                camera->yon_z = yon_factor * global_camera->yon_z;
                actor = BrActorAdd(gUniverse_actor, actor);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                GetThreeFloats(pF, &cam_pos.v[0], &cam_pos.v[1], &cam_pos.v[2]);
                if (the_funk->texture_animation_data.camera_info.mode == eFunk_camera_static) {
                    br_euler e;
                    float angle1;
                    float angle2;

                    GetPairOfFloats(pF, &angle1, &angle2);
                    e.order = 0x12;
                    e.a = BrDegreeToAngle(angle1);
                    e.b = BrDegreeToAngle(angle2);
                    e.c = 0;
                    BrEulerToMatrix34(&actor->t.t.mat, &e);
                }
                BrVector3Copy(&actor->t.t.translate.t, &cam_pos);
            }
            break;
        case eTexture_animation_mirror:
            if (the_funk->proximity_count == 0) {
                the_funk->texture_animation_data.mirror_info.actor = NULL;
                GetALineAndDontArgue(pF, s);
                GetALineAndDontArgue(pF, s);
            } else {
                br_actor* actor;
                br_camera* camera;
                br_camera* global_camera;
                br_vector3 n;
                br_matrix34 mat;

                global_camera = gCamera->type_data;
                yon_factor = GetAScalar(pF);
                the_funk->texture_animation_data.mirror_info.yon = yon_factor * global_camera->yon_z;
                the_funk->texture_animation_data.mirror_info.field_0x5c = GetAnInt(pF);
                actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                the_funk->texture_animation_data.mirror_info.actor = actor;
                camera = actor->type_data;
                camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                camera->field_of_view = global_camera->field_of_view;
                camera->hither_z = global_camera->hither_z;
                camera->yon_z = global_camera->yon_z + global_camera->hither_z;
                actor = BrActorAdd(gUniverse_actor, actor);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                BrVector3Copy(&n, &the_funk->proximity_array->n);
                BrVector3Set((br_vector3*)the_funk->texture_animation_data.mirror_info.mat.m[0],
                    1.f - n.v[0] * n.v[0] + n.v[0] * n.v[0],
                    n.v[0] * n.v[1] + n.v[0] * n.v[1],
                    n.v[0] * n.v[2] + n.v[0] * n.v[2]);
                BrVector3Set((br_vector3*)the_funk->texture_animation_data.mirror_info.mat.m[1],
                    n.v[0] * n.v[1] + n.v[0] * n.v[1],
                    1.f - n.v[1] * n.v[1] + n.v[1] * n.v[1],
                    n.v[1] * n.v[2] + n.v[1] * n.v[2]);
                BrVector3Set((br_vector3*)the_funk->texture_animation_data.mirror_info.mat.m[2],
                    n.v[0] * n.v[2] + n.v[0] * n.v[2],
                    n.v[1] * n.v[2] + n.v[1] * n.v[2],
                    1.f - n.v[2] * n.v[2] + n.v[2] * n.v[2]);
                BrVector3Scale((br_vector3*)the_funk->texture_animation_data.mirror_info.mat.m[3], &n, -2.f * the_funk->proximity_array->d);
                BrVector3Sub((br_vector3*)mat.m[0], &the_funk->proximity_array->v[1], &the_funk->proximity_array->v[0]);
                BrVector3Copy((br_vector3*)mat.m[1], &the_funk->proximity_array->n);
                BrVector3Cross((br_vector3*)mat.m[2], (br_vector3*)mat.m[0], (br_vector3*)mat.m[1]);
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.min, (float)0x7fffffff, (float)0x7fffffff, (float)0x7fffffff);
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.max, (float)-0x7fffffff, (float)-0x7fffffff, (float)-0x7fffffff);
                for (i = 0; i < the_funk->proximity_count; i++) {
                    int j;

                    for (j = 0; j < 3; j++) {
                        int k;

                        BrMatrix34ApplyP(&gFunk_temp_vertices[i].points[j],
                            &the_funk->proximity_array[i].v[j], /* FIXME: original has &the_funk->proximity_array->v[j] */
                            &mat);
                        for (k = 0; k < 3; k++) {
                            if (the_funk->texture_animation_data.mirror_info.bounds.min.v[k] > gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (the_funk->texture_animation_data.mirror_info.bounds.max.v[k] < gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
                for (i = 0; i < the_funk->proximity_count; i++) {
                    int j;

                    for (j = 0; j < 3; j++) {
                        int k;

                        gFunk_temp_vertices[i].points[j].v[0] =
                            (gFunk_temp_vertices[i].points[j].v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0])
                                / (the_funk->texture_animation_data.mirror_info.bounds.max.v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0]);
                        gFunk_temp_vertices[i].points[j].v[1] =
                            (gFunk_temp_vertices[i].points[j].v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[2])
                                / (the_funk->texture_animation_data.mirror_info.bounds.max.v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[2]);
                        for (k = 0; k < 3; k++) {
                            if (the_funk->texture_animation_data.mirror_info.bounds.min.v[k] > gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (the_funk->texture_animation_data.mirror_info.bounds.max.v[k] < gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    BrMemFree(gFunk_temp_vertices);
}

// FUNCTION: CARMA2_HW 0x00474950
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
    int i;

    if (gFunkotronics_array == NULL) {
        return;
    }
    for (i = 0; i < gFunkotronics_array_size; i++) {
        tFunkotronic_spec* the_funk = &gFunkotronics_array[i];

        PossibleService();
        if (the_funk->owner == pOwner) {
            the_funk->owner = -999;
            if (the_funk->proximity_array != NULL) {
                BrMemFree(the_funk->proximity_array);
            }
            if (the_funk->texture_animation_type == eTexture_animation_frames &&
                    the_funk->texture_animation_data.frames_info.mode == eMove_texturebits) {
                BrMemFree(the_funk->texture_animation_data.frames_info.texture_info.data);
            }
            switch (the_funk->texture_animation_type) {
            case eTexture_animation_flic:
                BrMemFree(the_funk->texture_animation_data.flic_info.flic_data);
                EndFlic(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                BrMemFree(the_funk->material->colour_map->pixels);
                the_funk->material->colour_map->pixels = NULL;
                BrPixelmapFree(the_funk->material->colour_map);
                the_funk->material->colour_map = NULL;
                break;
            case eTexture_animation_camera:
                for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                    BrActorRemove(the_funk->texture_animation_data.camera_info.actors[i]);
                    BrActorFree(the_funk->texture_animation_data.camera_info.actors[i]);
                }
                break;
            case eTexture_animation_mirror:
                if (the_funk->texture_animation_data.mirror_info.actor != NULL) {
                    BrActorRemove(the_funk->texture_animation_data.mirror_info.actor);
                    BrActorFree(the_funk->texture_animation_data.mirror_info.actor);
                }
                break;
            default:
                break;
            }
        }
    }
}

tGroovidelic_spec* C2_HOOK_FASTCALL AddNewGroovidelic(void) {
    void* new_array;
    int i;

    C2_HOOK_BUG_ON(sizeof(tGroovidelic_spec) != 0x84);

    for (i = 0; i < gGroovidelics_array_size; i++) {
        if (gGroovidelics_array[i].owner == -999) {
            memset(&gGroovidelics_array[i], 0, sizeof(tGroovidelic_spec));
            return &gGroovidelics_array[i];
        }
    }
    gGroovidelics_array_size += 16;
    new_array = BrMemCalloc(gGroovidelics_array_size, sizeof(tGroovidelic_spec), kMem_groove_spec);
    if (gGroovidelics_array != NULL) {
        memcpy(new_array, gGroovidelics_array, (gGroovidelics_array_size - 16) * sizeof(tGroovidelic_spec));
        ShiftBoundGrooveFunks(
                (char*)gGroovidelics_array,
                (char*)&gGroovidelics_array[gGroovidelics_array_size - 16],
                (char*)new_array - (char*)gGroovidelics_array);
        BrMemFree(gGroovidelics_array);
    }
    gGroovidelics_array = new_array;
    for (i = 0; i < 16; i++) {
        gGroovidelics_array[i + gGroovidelics_array_size - 16].owner = -999;
    }
    return &gGroovidelics_array[gGroovidelics_array_size - 16];
}

// FUNCTION: CARMA2_HW 0x00476470
void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent) {
    int first_time;
    char s[256];
    char* str;
    float x_0;
    float x_1;
    float x_2;
    int d_0;
    int d_1;
    int d_2;
    br_vector3 p;

#ifndef REC2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, owner, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, block_flags, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, actor, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, lollipop_mode, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, mode, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_type, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_mode, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_interrupt_status, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.texture_info.data, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.period.value, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.x_delta, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.y_delta, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.z_delta, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.centre, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.period.value, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.texture_info.data, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.radius, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.centre, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.axis, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_centre, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_position, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_type, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_mode, 0x60);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_interrupt_status, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.period.value, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.texture_info.data, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.axis, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.period.value, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.texture_info.data, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.max_angle, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.axis, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.x_period, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.y_period, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.z_period, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.x_magnitude, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.y_magnitude, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.z_magnitude, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.x_period, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.y_period, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.z_period, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.x_magnitude, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.y_magnitude, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.z_magnitude, 0x80);
#endif

    first_time = 1;
    while (!PFfeof(pF)) {
        tGroovidelic_spec* the_groove;

        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF GROOVE") == 0) {
            break;
        }

        if (!first_time) {
            if (strcmp(s, "NEXT GROOVE") != 0) {
                FatalError(kFatalError_ErrorWithinGroovidelicFile);
            }
            GetALineAndDontArgue(pF, s);
        }
        first_time = 0;

        str = strtok(s, "\t ,/");
        the_groove = AddNewGroovidelic();
        the_groove->owner = pOwner;
        the_groove->block_flags = 0;
        the_groove->actor = DRActorFindRecurse(pParent_actor, str);

        if (the_groove->actor == NULL) {
            if (!pAllowed_to_be_absent && !gAusterity_mode) {
                FatalError(kFatalError_CannotFindActorReferencedInGroovidelicFile_S, str);
            }
            if (gGroove_by_proxy_actor == NULL) {
                gGroove_by_proxy_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
                gGroove_by_proxy_actor->model = LoadModel("PROXY.DAT");
                BrModelAdd(gGroove_by_proxy_actor->model);
                BrActorAdd(gDont_render_actor, gGroove_by_proxy_actor);
            }
            the_groove->actor = gGroove_by_proxy_actor;
        }
        the_groove->lollipop_mode = GetALineAndInterpretCommand(pF, gLollipop_names, REC2_ASIZE(gLollipop_names));
        the_groove->mode = GetALineAndInterpretCommand(pF, gGroove_nature_names, REC2_ASIZE(gGroove_nature_names));
        the_groove->path_type = GetALineAndInterpretCommand(pF, gGroove_path_names, REC2_ASIZE(gGroove_path_names));
        the_groove->path_interrupt_status = eInterrupt_none;
        the_groove->object_interrupt_status = eInterrupt_none;
        if (the_groove->path_type != eGroove_path_none) {
            the_groove->path_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->path_type) {
        case eGroove_path_straight:
            GetThreeFloats(pF,
                &the_groove->path_data.straight_info.centre.v[0],
                &the_groove->path_data.straight_info.centre.v[1],
                &the_groove->path_data.straight_info.centre.v[2]);

            if (Vector3IsZero(&the_groove->path_data.straight_info.centre)) {
                BrVector3Copy(&the_groove->path_data.straight_info.centre,
                    &the_groove->actor->t.t.translate.t);
            }
            switch (the_groove->path_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->path_data.straight_info.period.value);
                break;
            case eMove_texturebits:
                the_groove->path_data.straight_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_groove->path_data.straight_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_groove->path_data.straight_info.texture_info.data->bits, s);
                the_groove->path_data.straight_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_groove->path_data.straight_info.period.value = x_0 == 0.0f ? 0.0f : 1000.0f / x_0;
                break;
            }
            GetThreeFloats(pF,
                &the_groove->path_data.straight_info.x_delta,
                &the_groove->path_data.straight_info.y_delta,
                &the_groove->path_data.straight_info.z_delta);
            break;
        case eGroove_path_circular:
             GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
             BrVector3Copy(&the_groove->path_data.circular_info.centre, &p);
             if (Vector3IsZero(&the_groove->path_data.circular_info.centre)) {
                 BrVector3Copy(&the_groove->path_data.circular_info.centre,
                    &the_groove->actor->t.t.translate.t);
             }
            switch (the_groove->path_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->path_data.circular_info.period.value);
                break;
            case eMove_texturebits:
                the_groove->path_data.circular_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_groove->path_data.circular_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_groove->path_data.circular_info.texture_info.data->bits, s);
                the_groove->path_data.circular_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_groove->path_data.circular_info.period.value = x_0 == 0.0f ? 0.0f : 1000.0f / x_0;
                break;
            }
            the_groove->path_data.circular_info.radius = GetAFloat(pF);
            the_groove->path_data.circular_info.axis =  GetALineAndInterpretCommand(pF, gAxis_names, REC2_ASIZE(gAxis_names));
            break;
        default:
            break;
        }

        the_groove->object_type = GetALineAndInterpretCommand(pF, gGroove_object_names, REC2_ASIZE(gGroove_object_names));
        BrVector3Copy(&the_groove->object_position, &the_groove->actor->t.t.translate.t);
        if (the_groove->object_type != eGroove_object_none) {
            the_groove->object_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->object_type) {
        case eGroove_object_spin:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->object_data.spin_info.period.value);
                break;
            case eMove_texturebits:
                the_groove->object_data.spin_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_groove->object_data.spin_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_groove->object_data.spin_info.texture_info.data->bits, s);
                the_groove->object_data.spin_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_groove->object_data.spin_info.period.value = (x_0 == 0.0f) ? 0.0f : (1000.0f / x_0);
            }
            GetThreeFloats(pF,
                &the_groove->object_centre.v[0],
                &the_groove->object_centre.v[1],
                &the_groove->object_centre.v[2]);
            the_groove->object_data.spin_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, REC2_ASIZE(gAxis_names));
            break;
        case eGroove_object_rock:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->object_data.spin_info.period.value);
                break;
            case eMove_texturebits:
                the_groove->object_data.rock_info.texture_info.data = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                GetAString(pF, s);
                the_groove->object_data.rock_info.texture_info.data->count = (tU8)strlen(s);
                texture_string_to_bits(the_groove->object_data.rock_info.texture_info.data->bits, s);
                the_groove->object_data.rock_info.texture_info.data->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAFloat(pF);
                the_groove->object_data.spin_info.period.value = (x_0 == 0.0f) ? 0.0f : (1000.0f / x_0);
            }
            GetThreeFloats(pF,
                &the_groove->object_centre.v[0],
                &the_groove->object_centre.v[1],
                &the_groove->object_centre.v[2]);
            the_groove->object_data.rock_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, REC2_ASIZE(gAxis_names));
            the_groove->object_data.rock_info.max_angle = GetAFloat(pF);
            break;
        case eGroove_object_throb:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetThreeInts(pF, &d_0, &d_1, &d_2);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_0, &the_groove->object_data.throb_info.x_period.value);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_1, &the_groove->object_data.throb_info.y_period.value);
                }
                if (d_2 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_2, &the_groove->object_data.throb_info.z_period.value);
                }
                break;
            default:
                GetThreeFloats(pF, &x_0, &x_1, &x_2);
                the_groove->object_data.throb_info.x_period.value = (x_0 == 0.0f) ? 0.0f : (1000.0f / x_0);
                the_groove->object_data.throb_info.y_period.value = (x_1 == 0.0f) ? 0.0f : (1000.0f / x_1);
                the_groove->object_data.throb_info.z_period.value = (x_2 == 0.0f) ? 0.0f : (1000.0f / x_2);
                break;
            }
            GetThreeFloats(pF,
                &the_groove->object_centre.v[0],
                &the_groove->object_centre.v[1],
                &the_groove->object_centre.v[2]);
            GetThreeFloatPercents(pF,
                &the_groove->object_data.throb_info.x_magnitude,
                &the_groove->object_data.throb_info.y_magnitude,
                &the_groove->object_data.throb_info.z_magnitude);
            break;
        case eGroove_object_shear:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetThreeInts(pF, &d_0, &d_1, &d_2);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_0, &the_groove->object_data.shear_info.x_period.value);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_1, &the_groove->object_data.shear_info.y_period.value);
                }
                if (d_2 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_2, &the_groove->object_data.shear_info.z_period.value);
                }
                break;
            default:
                GetThreeFloats(pF, &x_0, &x_1, &x_2);
                the_groove->object_data.shear_info.x_period.value = x_0 == 0.0f ? 0.0f : 1000.0f / x_0;
                the_groove->object_data.shear_info.y_period.value = x_1 == 0.0f ? 0.0f : 1000.0f / x_1;
                the_groove->object_data.shear_info.z_period.value = x_2 == 0.0f ? 0.0f : 1000.0f / x_2;
            }
            GetThreeFloats(pF,
                &the_groove->object_centre.v[0],
                &the_groove->object_centre.v[1],
                &the_groove->object_centre.v[2]);
            GetThreeFloatPercents(pF,
                &the_groove->object_data.shear_info.x_magnitude,
                &the_groove->object_data.shear_info.y_magnitude,
                &the_groove->object_data.shear_info.z_magnitude);
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00476430
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
    int i;

    if (gGroovidelics_array == NULL) {
        return;
    }
    for (i = 0; i < gGroovidelics_array_size; i++) {
        tGroovidelic_spec* the_groove = &gGroovidelics_array[i];

        PossibleService();
        if (the_groove->owner == pOwner) {
            the_groove->owner = -999;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ff110
tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void) {

    return gDefault_water_spec_vol_real;
}

// FUNCTION: CARMA2_HW 0x00486dc0
void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF) {

    /* //////////// GLOBAL LIGHTING DATA /////////// */

    /* RGB for main directional light-source */
    GetThreeInts(pF, &gLighting_data.directional.red, &gLighting_data.directional.green, &gLighting_data.directional.blue);
    /* Ambient/Diffuse light to be used when plaything ambient says 0 */
    GetPairOfScalars(pF, &gLighting_data.ambient_0, &gLighting_data.diffuse_0);
    /* Ambient/Diffuse light to be used when plaything ambient says 1 */
    GetPairOfScalars(pF, &gLighting_data.ambient_1, &gLighting_data.diffuse_1);
    /* Ambient/Diffuse light to be used when plaything ambient says anything else */
    GetPairOfScalars(pF, &gLighting_data.ambient_else, &gLighting_data.diffuse_else);
}

void C2_HOOK_FASTCALL InitRepairAnimations(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tRepair_animation) != 0xc);
    C2_HOOK_BUG_ON(REC2_ASIZE(gRepair_animations) != 25);

    for (i = 0; i < REC2_ASIZE(gRepair_animations); i++) {
        gRepair_animations[i].field_0x0 = 0;
    }
}

void C2_HOOK_FASTCALL ReadSmokeFireChance(FILE* pF, tSmokeFireChance* pSmokeFireChance) {

    /* %chance fire */
    pSmokeFireChance->chance_fire = GetAnInt(pF);
    if (pSmokeFireChance->chance_fire != 0) {
        pSmokeFireChance->model_1_int = GetAnInt(pF);
        GetPairOfInts(pF, &pSmokeFireChance->model_2_int, &pSmokeFireChance->model_3_int);
    }
}

void C2_HOOK_FASTCALL ReadSmashReplace(tSmashReplace* pReplace, FILE* pF) {
    char s[256];

    /* new model */
    GetAString(pF, s);
    pReplace->model = BrModelFind(s);
    if (pReplace->model == NULL) {
        FatalError(kFatalError_CannotFindSmashActorModel_S, s);
    }
    ReadSmokeFireChance(pF, &pReplace->smoke_fire_chance);
}

void C2_HOOK_FASTCALL ReadSmashTexture(tSmashable_item_spec* pSmash_item, tSmashable_item_spec_texture_change* pTexture_change, FILE* pF) {
    char s[256] = "";

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, levels, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, trigger, 0x44);

    pTexture_change->levels = NULL;
    strcpy(pTexture_change->undefined_0x0, s);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, field_0x40, 0x40);

    pTexture_change->field_0x40 = -1;
    pTexture_change->trigger = pSmash_item->trigger_object;
    pTexture_change->field_0x4c = 0;
    pTexture_change->field_0x40 = -1;
    pTexture_change->field_0x6c = 0;
    pTexture_change->field_0x70 = 0;
    pTexture_change->field_0x74 = 0;
    pTexture_change->field_0x78 = 0;
    pTexture_change->field_0x54 = 0.f;
    pTexture_change->field_0x58 = 0.f;
    pTexture_change->field_0x5c = 0.f;
    pTexture_change->field_0x60 = 1.f;
    pTexture_change->field_0x64 = 0.f;
    pTexture_change->field_0x68 = 0.f;
    LoadSmashableLevels(pF,
        &pTexture_change->levels,
        &pTexture_change->count_levels,
        pSmash_item->mode == kSmashableMode_TextureChange,
        &gTrack_storage_space);
}

void C2_HOOK_FASTCALL ReadSmashSounds(FILE* pF, tConnotations* pConnotations, tBrender_storage* pStorage) {
    int i;

    /* number of possible sounds */
    pConnotations->count_sounds = GetAnInt(pF);

    for (i = 0; i < pConnotations->count_sounds; i++) {
        int sound;

        /* sound id */
        sound = GetAnInt(pF);

        pConnotations->sounds[i] = LoadSingleSound(pStorage, sound);
    }
}

void C2_HOOK_FASTCALL ReadSpecialEffectsSpec(FILE* pF, tSpecial_effects_spec* pSpecial_effects) {

    ReadExplosion(pF, &pSpecial_effects->explosion_animation);
    ReadSpillData(pF, &pSpecial_effects->slick);
}

void C2_HOOK_FASTCALL ReadSideEffects(FILE* pF, tSide_effects* pSide_effects) {

    ReadNonCarCuboidActivation(pF, &pSide_effects->activations);
    ReadShrapnelSideEffects(pF, &pSide_effects->side_effects);

    /* Extensions flags */
    pSide_effects->extension_flags = GetAnInt(pF);
    if (pSide_effects->extension_flags & 0x1) {
        pSide_effects->extension_arg = GetAnInt(pF);
    }
    /* Room turn on code */
    pSide_effects->room_turn_on_code = GetAnInt(pF);
}

void C2_HOOK_FASTCALL ReadVariableChanges(FILE* pF, tVariable_changes* pVariable_changes) {
    int i;

    /* run-time variable changes */
    pVariable_changes->count = GetAnInt(pF);
    for (i = 0; i < pVariable_changes->count; i++) {
        int v1, v2;

        GetPairOfInts(pF, &v1, &v2);
        pVariable_changes->runtime_changes[i].field_0x0 = v2;
        pVariable_changes->runtime_changes[i].field_0x2 = v1;
    }
}

void C2_HOOK_FASTCALL ReadConnotations(FILE* pF, tConnotations* pConnotations, tBrender_storage* pStorage) {

    ReadSmashSounds(pF, pConnotations, pStorage);

#ifndef REC2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, mode_data, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_shrapnel, connotations.count_shrapnel, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_shrapnel, connotations.shrapnel, 0x18);
#endif

    ReadShrapnelSpec(pF, pConnotations->shrapnel, &pConnotations->count_shrapnel);
    ReadSpecialEffectsSpec(pF, &pConnotations->special_effects);
    ReadSideEffects(pF, &pConnotations->side_effects);
    ReadAward(pF, &pConnotations->award);
    ReadVariableChanges(pF, &pConnotations->variable_changes);
}

// FUNCTION: CARMA2_HW 0x004f0450
void C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath) {
    int i;
    char s[256];

    gCount_smashable_noncars = 0;
    gCount_smashable_noncar_shrapnel_actors = 0;
    gSmashable_noncars = BrMemAllocate(100 * sizeof(tSmashable_environment_name), kMem_smashable_env_info);
    C2_HOOK_BUG_ON(100 * sizeof(tSmashable_environment_name) != 3200);

    /* Number of smash specs */
    gCount_track_smashable_environment_specs = GetAnInt(pF);

    gCapacity_track_smashables = 5 * (gCount_track_smashable_environment_specs + 30);
    gCount_track_smashable_environment_specs_2 = gCount_track_smashable_environment_specs;
    gTrack_smashable_environment_specs = BrMemAllocate(gCapacity_track_smashables * sizeof(tSmashable_item_spec), kMem_smashable_env_info);
    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 736);

    gSmashable_track_environment_path = pPath;

    memset(gSmashable_track_indexable_triggers, 0, sizeof(gSmashable_track_indexable_triggers));
    C2_HOOK_BUG_ON(sizeof(gSmashable_track_indexable_triggers) != 400);

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec* spec = &gTrack_smashable_environment_specs[i];

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, flags, 0x0);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, field_0x10, 0x10);

        /* Flags */
        spec->flags = GetAnInt(pF);
        spec->field_0x10 = 0;

        /* Name of trigger material */
        GetAString(pF, s);
        if (s[0] == '&') {
            if (s[1] >= '0' && s[1] <= '9' && s[3] == '\0') {
                spec->trigger_type = kSmashableTrigger_Number;
            } else {
                spec->trigger_type = kSmashableTrigger_Model;
            }
        } else {
            if (strstr(s, ".DAT") != NULL || strstr(s, ".dat") != NULL || strstr(s, ".ACT") != NULL || strstr(s, ".act") != NULL) {
                spec->trigger_type = kSmashableTrigger_Model;
            } else {
                spec->trigger_type = kSmashableTrigger_Material;
            }
        }

        switch (spec->trigger_type) {
        case kSmashableTrigger_Material:
            spec->trigger_object.material = BrMaterialFind(s);
            if (spec->trigger_object.material == NULL) {
                FatalError(kFatalError_CannotFindSmashMaterial_S, s);
            }
            break;
        case kSmashableTrigger_Model:
            spec->trigger_object.model = BrModelFind(s);
            if (spec->trigger_object.model == NULL) {
                FatalError(kFatalError_CannotFindSmashModel_S, s);
            }
            break;
        case kSmashableTrigger_Number:
            spec->trigger_object.number.field_0x0 = (s[1] + 24) * 10 + s[2]; /* FIXME: why 24? (-'0' == -48) */
            spec->trigger_object.number.field_0x1 = GetAnInt(pF);
            gSmashable_track_indexable_triggers[spec->trigger_object.number.field_0x0] = spec;
            break;
        }

        /* Mode */
        spec->mode = GetALineAndInterpretCommand(pF, gSmashable_item_mode_names, REC2_ASIZE(gSmashable_item_mode_names));
        switch (spec->mode) {
        case kSmashableMode_Decal:
        case kSmashableMode_TextureChange:
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, mode_data, 0x14);
            ReadSmashTexture(spec, &spec->mode_data.texture_change, pF);
            break;
        default:
            /* Removal threshold */
            spec->mode_data.shrapnel.removal_threshold = GetAScalar(pF);
            ReadConnotations(pF, &spec->mode_data.shrapnel.connotations, &gTrack_storage_space);
            break;
        }
        if (spec->mode == kSmashableMode_ReplaceModel) {
            ReadSmashReplace(&spec->replace, pF);
        }
        /* reserved 1 */
        GetAnInt(pF);
        /* reserved 2 */
        GetAnInt(pF);
        /* reserved 3 */
        GetAnInt(pF);
        GetAnInt(pF);

        spec->field_0x2d4 = 0;
        spec->field_0x2d8 = 0;
        spec->field_0x2dc = 0;
    }
    for (i = gCount_track_smashable_environment_specs; i < gCapacity_track_smashables; i++) {
        tSmashable_item_spec* spec = &gTrack_smashable_environment_specs[i];

        spec->trigger_type = -1;
    }
}

// FUNCTION: CARMA2_HW 0x004f5470
int C2_HOOK_FASTCALL SmashFaceMustBeUpdateable(br_material *pMaterial) {
    int i;

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec *spec = &gTrack_smashable_environment_specs[i];

        if ((spec->trigger_type == kSmashableTrigger_Material && spec->trigger_object.material == pMaterial)
                && ((spec->mode == kSmashableMode_TextureChange && spec->mode_data.texture_change.count_levels > 0)
                || spec->mode == kSmashableMode_Remove
                || spec->mode == kSmashableMode_ReplaceModel)) {
            C2_HOOK_ASSERT(SmashFaceMustBeUpdateable_original(pMaterial) == 1);
            return 1;
        }
    }
    C2_HOOK_ASSERT(SmashFaceMustBeUpdateable_original(pMaterial) == 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f09c0
void C2_HOOK_FASTCALL FreeEnvSmash(tSmashable_item_spec* pSmash) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f0960
void C2_HOOK_FASTCALL DisposeSmashableEnvironment(void) {
    int i;

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec* smash = &gTrack_smashable_environment_specs[i];

        FreeEnvSmash(smash);
    }
    if (gCount_track_smashable_environment_specs != 0) {
        BrMemFree(gTrack_smashable_environment_specs);
    }
    if (gNet_mode == eNet_mode_host) {
        BrMemFree(gNet_host_smashes);
    }
}

// FUNCTION: CARMA2_HW 0x004f5750
void C2_HOOK_FASTCALL ReinitSmashing(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gSmash_glass_fragments) != 200);
    C2_HOOK_BUG_ON(sizeof(gSmash_glass_fragments[0]) != 0x38);
    C2_HOOK_BUG_ON(REC2_ASIZE(gDecals) != 50);
    C2_HOOK_BUG_ON(sizeof(gDecals[0]) != 0x8);
    C2_HOOK_BUG_ON(REC2_ASIZE(gQueued_net_smashes) != 50);
    C2_HOOK_BUG_ON(sizeof(gQueued_net_smashes[0]) != 0x38);
    C2_HOOK_BUG_ON(sizeof(tStored_smash) != 0x38);

    for (i = 0; i < REC2_ASIZE(gSmash_glass_fragments); i++) {
        gSmash_glass_fragments[i].end_time = 0;
    }
    for (i = 0; i < REC2_ASIZE(gDecals); i++) {
        gDecals[i].time = 0;
    }
    for (i = 0; i < REC2_ASIZE(gQueued_net_smashes); i++) {
        gQueued_net_smashes[i].field_0x4 = 0;
    }
    gSize_powerup_queue = 0;
    gCount_host_smashes = 0;
    if (gNet_mode == eNet_mode_host) {
        gNet_host_smashes = BrMemAllocate(1000 * sizeof(tStored_smash), kMem_stored_smash);
    }
}

// FUNCTION: CARMA2_HW 0x004f0940
void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void) {

    BrMemFree(gSmashable_noncars);
}

// FUNCTION: CARMA2_HW 0x004ffd80
void C2_HOOK_FASTCALL ReadSpecialVolumes(FILE* pF) {

    /* DEFAULT ENGINE NOISE */
    gDefault_engine_noise_index = GetAnInt(pF);

    gDefault_water_spec_vol_real = &gDefault_default_water_spec_vol;

    /* # special effects volumes */
    gProgram_state.special_volume_count = GetAnInt(pF);
    if (gProgram_state.special_volume_count != 0) {
        int i;

        C2_HOOK_BUG_ON(sizeof(tSpecial_volume) != 0xdc);
        gProgram_state.special_volumes = BrMemAllocate(gProgram_state.special_volume_count * sizeof(tSpecial_volume), kMem_special_volume);
        for (i = 0; i < gProgram_state.special_volume_count; i++) {
            br_bounds unit_bounds;
            tSpecial_volume* spec = &gProgram_state.special_volumes[i];

            PossibleService();

            C2_HOOK_BUG_ON(REC2_ASIZE(gSpecial_effects_boundary_choices) != 4);
            spec->boundary_type = GetALineAndInterpretCommand(pF, gSpecial_effects_boundary_choices, REC2_ASIZE(gSpecial_effects_boundary_choices));
            if (spec->boundary_type == eFx_boundary_new) {
                spec->boundary_type = eFx_boundary_box;
            }
            switch (spec->boundary_type) {
            case eFx_boundary_box:
                GetThreeScalars(pF, &spec->boundary.box.mat.m[0][0], &spec->boundary.box.mat.m[0][1], &spec->boundary.box.mat.m[0][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[1][0], &spec->boundary.box.mat.m[1][1], &spec->boundary.box.mat.m[1][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[2][0], &spec->boundary.box.mat.m[2][1], &spec->boundary.box.mat.m[2][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[3][0], &spec->boundary.box.mat.m[3][1], &spec->boundary.box.mat.m[3][2]);
                unit_bounds.min.v[0] = -1.0f;
                unit_bounds.min.v[1] = -1.0f;
                unit_bounds.min.v[2] = -1.0f;
                unit_bounds.max.v[0] =  1.0f;
                unit_bounds.max.v[1] =  1.0f;
                unit_bounds.max.v[2] =  1.0f;
                GetNewBoundingBox(&spec->boundary.box.bounds, &unit_bounds, &spec->boundary.box.mat);
                BrMatrix34Inverse(&spec->boundary.box.inv_mat, &spec->boundary.box.mat);
                spec->boundary.box.axis_length.v[0] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[0]);
                spec->boundary.box.axis_length.v[1] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[1]);
                spec->boundary.box.axis_length.v[2] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[2]);
                ParseSpecialVolume(pF, spec, NULL, spec->boundary_type == eFx_boundary_box);
                if (spec->soundfx_type != kSoundFx_None) {
                    int j;

                    for (j = 0; j < 3; j++) {
                        spec->boundary.box.bounds.min.v[j] -= 10.f;
                        spec->boundary.box.bounds.max.v[j] += 10.f;
                    }
                }
                break;
            case eFx_boundary_plane:
                GetFourScalars(pF, &spec->boundary.plane.v[0], &spec->boundary.plane.v[1], &spec->boundary.plane.v[2], &spec->boundary.plane.v[3]);
                ParseSpecialVolume(pF, spec, NULL, 1);
                break;
            case eFx_boundary_default:
                ParseSpecialVolume(pF, spec, NULL, 0);
                gDefault_water_spec_vol_real = spec;
                break;
            default:
                FatalError(kFatalError_ProblemWithSpecialVolumeNumber_I, i);
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004572f0
void C2_HOOK_FASTCALL ReadSoundGenerators(tTrack_spec* pTrack_spec, FILE* pF) {
    int i;

    gProgram_state.count_track_sound_generators = GetAnInt(pF);
    if (gProgram_state.count_track_sound_generators != 0) {
        C2_HOOK_BUG_ON(sizeof(tTrackSoundGenerator) != 0x7c);
        gProgram_state.track_sound_generators = BrMemAllocate(gProgram_state.count_track_sound_generators * sizeof(tTrackSoundGenerator), kMem_sound_generator);
    }
    for (i = 0; i < gProgram_state.count_track_sound_generators; i++) {
        tTrackSoundGenerator* generator;
        char name[32];

        generator = &gProgram_state.track_sound_generators[i];
        generator->type = GetALineAndInterpretCommand(pF, gSoundGeneratorTypeNames, REC2_ASIZE(gSoundGeneratorTypeNames));
        if (generator->type == kSoundGeneratorType_noncar) {
            char identifier[4];
            br_model* model;

            GetAString(pF, name);
            sprintf(identifier, "%c%02d", 'L', i);
            model = BrModelFind(name);
            if (model == NULL || model->identifier == NULL) {
                FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S, name);
            }
            memcpy(model->identifier, identifier, sizeof(identifier));
        } else if (generator->type == kSoundGeneratorType_actor) {
            char identifier[4];
            br_model* model;

            GetAString(pF, name);
            sprintf(identifier, "%c%02d", ')', i);
            model = BrModelFind(name);
            if (model == NULL || model->identifier == NULL) {
                FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S, name);
            }
            memcpy(model->identifier, identifier, sizeof(identifier));
        } else if (generator->type == kSoundGeneratorType_noncar) {
            GetThreeFloats(pF,
                &generator->point.v[0],
                &generator->point.v[1],
                &generator->point.v[2]);
        }
        ReadSoundSpec(pF, &generator->fx);
        if (generator->type == kSoundGeneratorType_noncar) {
            ReadSoundSpec(pF, &generator->fx1_noncar);
            ReadSoundSpec(pF, &generator->fx2_noncar);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00502210
void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM) {

    BrResFree(pM->faces);
    BrResFree(pM->vertices);
    pM->nfaces = 0;
    pM->nvertices = 0;
    pM->faces = NULL;
    pM->vertices = NULL;
}

void C2_HOOK_FASTCALL AddExceptionToList(tMaterial_exception** pList, tMaterial_exception* pItem) {

    pItem->next = *pList;
    *pList = pItem;
}

void C2_HOOK_FASTCALL LoadExceptionsFile(const char* pPath) {
    FILE* f;
    char* str;
    char s[256];
    int count;
    int version;

    f = DRfopen(pPath, "rt");
    if (f == NULL) {
        return;
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,");
    if (DRStricmp(str, "VERSION") != 0) {
        FatalError(kFatalError_FileMustStartWith_SS, pPath, "VERSION");
    }
    str = strtok(NULL, "\t ,");
    count = sscanf(str, "%d", &version);
    if (count == 0 || version != 1) {
        FatalError(kFatalError_CantCopeWithVersionFor_SS, str, pPath);
    }
    while (1) {
        tMaterial_exception* matexc;

        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,");
        if (DRStricmp(str, "end") == 0) {
            break;
        }
        C2_HOOK_BUG_ON(sizeof(tMaterial_exception) != 12);
        matexc = BrMemAllocate(sizeof(tMaterial_exception), kMem_exception);
        matexc->texture_name = BrMemAllocate(strlen(str) + 1, kMem_misc_string);
        strcpy(matexc->texture_name, str);
        matexc->flags = 0;
        while (1) {
            str = strtok(NULL, "\t ,");
            if (str == NULL) {
                break;
            }
            if (!isalnum(str[0])) {
                break;
            }
            if (DRStricmp(str, "nobilinear") == 0) {
                matexc->flags |= eMaterial_exception_nobilinear;
            } else if (DRStricmp(str, "wrap") == 0) {
                matexc->flags |= eMaterial_exception_wrap;
            } else {
                FatalError(kFatalError_Mysterious_SS, str, pPath);
            }
        }
        AddExceptionToList(&gMaterial_exceptions, matexc);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL LoadExceptionsFileForTrack(const char* pTrack_name) {
    tPath_name path;

    sprintf(path, "%s%s%s%s",
               pTrack_name, gDir_separator,
               gRenderer_fixup_basename, gRenderer_fixup_extension);
    LoadExceptionsFile(path);
}

void C2_HOOK_FASTCALL MungeTrackModel(br_model* pModel) {
    int* vertex_index_buffer;
    int (* face_index_buffer)[3];
    int i;

    vertex_index_buffer = BrMemAllocate((pModel->nvertices + 1) * sizeof(int), BR_MEMORY_APPLICATION);
    face_index_buffer = BrMemAllocate(pModel->nfaces * 3 * sizeof(int), BR_MEMORY_APPLICATION);

    for (i = 1; i < pModel->nvertices; i++) {
        if (vertex_index_buffer[i] == 0) {
            int j;

            vertex_index_buffer[i] = i;
            for (j = i + 1; j <= pModel->nvertices; j++) {
                br_vector3 d;

                BrVector3Sub(&d, &pModel->vertices[i - 1].p, &pModel->vertices[j - 1].p);
                if (BrVector3LengthSquared(&d) < 1e-8f) {
                    vertex_index_buffer[j] = i;
                }
            }
        }
    }
    for (i = 0; i < pModel->nfaces; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            face_index_buffer[i][j] = vertex_index_buffer[1 + pModel->faces[i].vertices[j]];
        }
    }
    if (pModel->nfaces > 1) {
        for (i = 0; i < pModel->nfaces - 1; i++) {
            int j;

            for (j = i + 1; j < pModel->nfaces; j++) {
                int l;

                for (l = 0; l < 3; l++) {
                    int m;
                    int s1a = face_index_buffer[i][l];
                    int s1b = face_index_buffer[i][(l + 1) % 3];

                    for (m = 0; m < 3; m++) {
                        int s2a = face_index_buffer[j][m];
                        int s2b = face_index_buffer[j][(m + 1) % 3];

                        if ((s1a == s2a && s1b == s2b) || (s1a == s2b && s1b == s2a)) {
                            br_vector3 d21, o31b, o31a, c2;
                            float d;
                            int s1c, s2c;

                            pModel->faces[j].flags |= 1 << m;
                            BrVector3Sub(&d21, &pModel->vertices[s2b - 1].p, &pModel->vertices[s2a - 1].p);
                            s1c = face_index_buffer[i][(l + 2) % 3];
                            s2c = face_index_buffer[j][(m + 2) % 3];
                            BrVector3Sub(&o31b, &pModel->vertices[s2c - 1].p, &pModel->vertices[s2a - 1].p);
                            BrVector3Sub(&o31a, &pModel->vertices[s1c - 1].p, &pModel->vertices[s2c - 1].p);
                            BrVector3Cross(&c2, &o31b, &d21);
                            d = BrVector3Dot(&o31a, &c2);
                            if (d < .0001f && (((pModel->faces[i].material == NULL ||
                                                 !(pModel->faces[i].material->flags & BR_MATF_TWO_SIDED)) &&
                                                (pModel->faces[j].material == NULL ||
                                                 !(pModel->faces[j].material->flags & BR_MATF_TWO_SIDED))) ||
                                               d >= -.0001f)) {
                                pModel->faces[j].flags |= 1 << l;
                            }
                        }
                    }
                }
            }
        }
    }
    BrMemFree(vertex_index_buffer);
    BrMemFree(face_index_buffer);
    pModel->flags |= BR_MODF_UPDATEABLE;
    BrModelUpdate(pModel, BR_MODU_FACES);
}

int C2_HOOK_FASTCALL ModelIsATree(br_model* pModel, char* pName_replacement) {
    int i;

    for (i = 0; i < gTree_surgery_pass1_count; i++) {
        if (strstr(pModel->identifier, gTree_surgery_pass1->name) == pModel->identifier) {
            break;
        }
    }
    if (i == gTree_surgery_pass1_count) {
        return 0;
    }
    for (i = 0; i < gTree_surgery_pass2_count; i++) {
        if (strcmp(pModel->identifier, gTree_surgery_pass2->original) == 0) {
            strcpy(pName_replacement, gTree_surgery_pass2->replacement);
            return 1;
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL PerformTreeSurgery(tBrender_storage* pStorage, br_model* pModel, const char* pName_replacement) {
    br_model *replacement;

    replacement = LoadModel(pName_replacement);
    if (replacement == NULL) {
        return;
    }
    BrResFree(pModel->faces);
    BrResFree(pModel->vertices);
    pModel->faces = BrResAllocate(pModel, BrResSize(replacement->faces),
                                  BrResClass(replacement->faces));
    memmove(pModel->faces, replacement->faces, BrResSize(replacement->faces));
    pModel->nfaces = replacement->nfaces;
    pModel->vertices = BrResAllocate(pModel, BrResSize(replacement->vertices),
                                     BrResClass(replacement->vertices));
    memmove(pModel->vertices, replacement->vertices, BrResSize(replacement->vertices));
    pModel->nvertices = replacement->nvertices;
    BrModelUpdate(pModel, BR_MODU_ALL);
}

void C2_HOOK_FASTCALL PossibleTreeSurgery(br_model* pModel) {
    char name_replacement[36];

    if (pModel == NULL || pModel->identifier == NULL) {
        return;
    }
    if (ModelIsATree(pModel, name_replacement)) {
        PerformTreeSurgery(&gTrack_storage_space, pModel, name_replacement);
    }
}

void C2_HOOK_FASTCALL CheckNonCarModelBounds(void) {
    int i;

    for (i = 0; i < gProgram_state.track_spec.count_non_cars; i++) {
        br_actor* actor = gProgram_state.track_spec.non_car_list[i];
        int idx;
        tNon_car_spec* non_car;
        int j;

        if (actor == NULL) {
            continue;
        }
        idx = gNon_car_spec_indices[10 * (actor->identifier[1] - '0') + (actor->identifier[2] - '0')] + 40 - 1;
        non_car = &gProgram_state.non_cars[idx];
        if (non_car == NULL || non_car->collision_info->actor != NULL) {
            continue;
        }
        non_car->collision_info->actor = actor;
        for (j = 0; j < 3; j++) {
            actor->model->bounds.min.v[j] = MIN(actor->model->bounds.min.v[j], non_car->collision_info->bb2.min.v[j]);
            actor->model->bounds.max.v[j] = MAX(actor->model->bounds.max.v[j], non_car->collision_info->bb2.max.v[j]);
        }
    }
}

void C2_HOOK_FASTCALL FreeExceptions(void) {
    tMaterial_exception* material_exception;

    for (material_exception = gMaterial_exceptions; material_exception != NULL; ) {
        tMaterial_exception* next_material_exception = material_exception->next;
        BrMemFree(material_exception->texture_name);
        BrMemFree(material_exception);
        material_exception = next_material_exception;
    }
    gMaterial_exceptions = NULL;
}

void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* race_lighting_path) {

}

// FUNCTION: CARMA2_HW 0x00504bf0
void C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info) {
    int i;
    int j;
    char short_track_name[32];
    char local_directory[256];
    char local_name[256];
    char local_race_path[256];
    char race_lighting_path[256];
    char race_desc_path[256];
    char actor_path[256];
    char s[256];
    FILE* f;
    char *str;
    tTWTVFS twt, twt2;
    br_pixelmap* sky;
    tU16 sky_pixels_high;
    int killed_sky;
    int count_material_modifiers;
    int count_noncar_objects;

    PrintMemoryDump(0, "AT THE START OF LOAD TRACK");
    strcpy(gCurrent_load_directory, "RACES");
    strcpy(gCurrent_load_name, pFile_name);
    gCurrent_load_name[strlen(gCurrent_load_name) - 4] = '\0';

    strcpy(local_directory, gCurrent_load_directory);
    strcpy(local_name, gCurrent_load_name);

    PathCat(gRace_path, gApplication_path, local_directory);
    strcpy(local_race_path, gRace_path);

    LoadExceptionsFileForTrack(local_race_path);

    PathCat(gRace_path, gRace_path, local_name);
    twt = OpenPackFileAndSetTiffLoading(gRace_path);
    PathCat(race_lighting_path, gRace_path, "LIGHTING.TXT");
    PathCat(race_desc_path, gRace_path, pFile_name);
    f = DRfopen(race_desc_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }

    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    if (strcmp(str, "VERSION") == 0) {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &gTrack_version);
        if (gTrack_version == 8) {
            gTrack_version = 0;
        }
    } else {
        gTrack_version = 0;
    }

    if (gTrack_version > 0) {
        ReadGlobalLightingInfo(f);
    }

    /* Position of centre of start of grid */
    GetThreeFloats(f,
        &pRace_info->initial_position.v[0],
        &pRace_info->initial_position.v[1],
        &pRace_info->initial_position.v[2]);

    /* Direction that grid faces in */
    pRace_info->initial_yaw = GetAFloat(f);

    /* Laps, checkpoints etc */

    /* # checkpoints */
    pRace_info->check_point_count = GetAnInt(f);
    for (i = 0; i < pRace_info->check_point_count; i++) {
        /* Checkpoint #x */
        PossibleService();

        /* Timer increment for each skill level (ped mode) */
        GetThreeInts(f,
            &pRace_info->checkpoints[i].timer_increments[0],
            &pRace_info->checkpoints[i].timer_increments[1],
            &pRace_info->checkpoints[i].timer_increments[2]);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, checkpoints, 0x21c);

        /* # quads for this checkpoint */
        pRace_info->checkpoints[i].count_quads = GetAnInt(f);

        BrVector3Set(&pRace_info->checkpoints[i].pos, 0.f, 0.f, 0.f);

        for (j = 0; j < pRace_info->checkpoints[i].count_quads; j++) {
            br_vector3 tmp1, tmp2;
            int k;

            for (k = 0; k < 4; k++) {
                /* Point #[0-3] */
                GetThreeScalars(f,
                    &pRace_info->checkpoints[i].quads[j].points[k].v[0],
                    &pRace_info->checkpoints[i].quads[j].points[k].v[1],
                    &pRace_info->checkpoints[i].quads[j].points[k].v[2]);
                BrVector3Accumulate(&pRace_info->checkpoints[i].pos, &pRace_info->checkpoints[i].quads[j].points[k]);
            }
            BrVector3Sub(&tmp1,
                &pRace_info->checkpoints[i].quads[j].points[2],
                &pRace_info->checkpoints[i].quads[j].points[0]);
            BrVector3Sub(&tmp2,
                &pRace_info->checkpoints[i].quads[j].points[1],
                &pRace_info->checkpoints[i].quads[j].points[0]);
            BrVector3Cross(&pRace_info->checkpoints[i].normals[j], &tmp2, &tmp1);
        }
        BrVector3InvScale(&pRace_info->checkpoints[i].pos,
            &pRace_info->checkpoints[i].pos,
            (float)(4 * pRace_info->checkpoints[i].count_quads));
    }

    PossibleService();
    strcpy(short_track_name, pFile_name);
    short_track_name[4] = '\0';
    PathCat(race_desc_path, gApplication_path, "RACES");
    PathCat(race_desc_path, race_desc_path, short_track_name);
    twt2 = OpenPackFile(race_desc_path);
    LoadAllImagesInDirectory(&gTrack_storage_space, race_desc_path);
    TWT_Unmount(twt2);

    PossibleService();
    LoadAllImagesInDirectory(&gTrack_storage_space, gRace_path);

    PossibleService();
    LoadAllShadeTablesInDirectory(&gTrack_storage_space, gRace_path);

    PossibleService();
    LoadTrackMaterials(&gTrack_storage_space, gRace_path);

    PossibleService();
    LoadTrackModels(&gTrack_storage_space, gRace_path);

    for (i = 0; i < gTrack_storage_space.models_count; i++) {

        PossibleService();
        MungeTrackModel(gTrack_storage_space.models[i]);
    }
    PrintMemoryDump(0, "JUST LOADED IN TEXTURES/MATS/MODELS FOR TRACK");

    PathCat(actor_path, gRace_path, local_name);
    strcat(actor_path, ".ACT");
    pTrack_spec->the_actor = BrActorLoad(actor_path);
    PrintMemoryDump(0, "AFTER LOADING TRACK ACTORS");
    PossibleService();

    /* Smashable environment specs */
    ReadSmashableEnvironment(f, gRace_path);
    PrintMemoryDump(0, "AFTER LOADING SMASHABLE ENVIRONMENT");

    /* Ped specs */
    ReadPedSpecs(f);
    PossibleService();

    PackFileRevertTiffLoading();
    ExtractColumns(pTrack_spec);
    PackFileRerevertTiffLoading();

    FinishUpLoadingPeds();
    PrintMemoryDump(0, "JUST EXTRACTED COLUMNS AND LOADED IN PEDS");

    gTrack_actor = pTrack_spec->the_actor;
    if (!gRendering_accessories && gNet_mode == eNet_mode_none) {
        int rstyle;

        rstyle = BR_RSTYLE_NONE;
        PossibleService();
        DRActorEnumRecurse(gTrack_actor, SetAccessoryRenderingCB, &rstyle);
    }
    BrActorAdd(gUniverse_actor, pTrack_spec->the_actor);

    /* Additional actor */
    GetALineAndDontArgue(f, s);
    str = strtok(strtok(s, "\t ,/"), ".");
    strcat(str, ".DAT");
    PathCat(gAdditional_model_path, gApplication_path, "MODELS");
    PathCat(gAdditional_model_path, gAdditional_model_path, str);
    PossibleService();
    gNumber_of_additional_models = BrModelLoadMany(gAdditional_model_path, gAdditional_models, REC2_ASIZE(gAdditional_models));
    for (i = 0; i < gNumber_of_additional_models; i++) {
        gAdditional_models[i]->flags = BR_MODF_UPDATEABLE;
    }
    BrModelAddMany(gAdditional_models, gNumber_of_additional_models);
    PossibleService();

    str = strtok(strtok(s, "\t ,/"), ".");
    strcat(str, ".ACT");
    PathCat(gAdditional_actor_path, gApplication_path, "ACTORS");
    PathCat(gAdditional_actor_path, gAdditional_actor_path, str);
    gAdditional_actors = BrActorLoad(gAdditional_actor_path);
    if (gAdditional_actors == NULL) {
        gAdditional_actors = BrActorAllocate(BR_ACTOR_NONE, NULL);
    }
    BrActorAdd(gUniverse_actor, gAdditional_actors);
    gLast_actor = NULL;

    /* Name of sky texture pixelmap (or "none") */
    GetAString(f, s);
    if (!gAusterity_mode && strcmp(&s[strlen(s) - 4], ".FLI") == 0) {
        void* flic_pixels;

        gTrack_flic_buffer = NULL;
        if (!LoadFlicData(s, &gTrack_flic_buffer, &gTrack_flic_buffer_size)) {
            FatalError(kFatalError_CantFindFile_S, s);
        }
        StartFlic(s, -1, &gTrack_flic_descriptor, gTrack_flic_buffer_size, (tS8*)gTrack_flic_buffer, NULL, 0, 0, 0);
        flic_pixels = BrMemAllocate((gTrack_flic_descriptor.width + 3) & (~3) * gTrack_flic_descriptor.height, kMem_video_pixels);
        sky = DRPixelmapAllocate(gScreen->type, gTrack_flic_descriptor.width, gTrack_flic_descriptor.height, flic_pixels, 0);
        BrMapAdd(sky);
        AssertFlicPixelmap(&gTrack_flic_descriptor, sky);
    } else {
        gTrack_flic_buffer = NULL;
        sky = BrMapFind(s);
    }
    killed_sky = 0;
    if (gAusterity_mode && sky != NULL) {
        for (i = 0; i < gTrack_storage_space.pixelmaps_count; i++) {
            if (gTrack_storage_space.pixelmaps[i] == sky) {
                BrMapRemove(gTrack_storage_space.pixelmaps[i]);
                BrPixelmapFree(gTrack_storage_space.pixelmaps[i]);
                gTrack_storage_space.pixelmaps[i] = gTrack_storage_space.pixelmaps[gTrack_storage_space.pixelmaps_count - 1];
                gTrack_storage_space.pixelmaps_count--;
                break;
            }
        }
        sky = NULL;
        killed_sky = 1;
    }
    gProgram_state.default_depth_effect.sky_texture = sky;
    if (sky != NULL) {
        sky_pixels_high = gProgram_state.default_depth_effect.sky_texture->height;
    } else {
        sky_pixels_high = 100;
    }
    PossibleService();

    /* Horizontal repetitions of sky texture */
    gSky_image_width = BrDegreeToAngle(360.0 / GetAnInt(f));

    /* Vertical size of sky texture (degrees) */
    gSky_image_height = BrDegreeToAngle(GetAScalar(f));

    /* Position of horizon (pixels below top) */
    gSky_image_underground = gSky_image_height * (sky_pixels_high - GetAnInt(f)) / sky_pixels_high;

    MungeForwardSky();
    MungeRearviewSky();
    PossibleService();

    /* Depth cue mode ("none", "dark" or "fog") */
    gProgram_state.default_depth_effect.type = GetALineAndInterpretCommand(f, gDepth_effect_names, REC2_ASIZE(gDepth_effect_names));

    /* Degree of fog/darkness */
    GetPairOfInts(f,
        &gProgram_state.default_depth_effect.start,
        &gProgram_state.default_depth_effect.end);

    /* Depth cue colour (red, green, blue ) */
    GetThreeInts(f,
        &gProgram_state.default_depth_effect.colour.red,
        &gProgram_state.default_depth_effect.colour.green,
        &gProgram_state.default_depth_effect.colour.blue);
    gTrack_depth_colour_red = gProgram_state.default_depth_effect.colour.red;
    gTrack_depth_colour_green = gProgram_state.default_depth_effect.colour.green;
    gTrack_depth_colour_blue = gProgram_state.default_depth_effect.colour.blue;
    ChangeDepthEffect();

    if (killed_sky && gProgram_state.default_depth_effect.type != eDepth_effect_fog) {
        gProgram_state.default_depth_effect.type = eDepth_effect_fog;
        gProgram_state.default_depth_effect.start = 7;
        gProgram_state.default_depth_effect.end = 0;
        gProgram_state.default_depth_effect.colour.red = 0xf8;
        gProgram_state.default_depth_effect.colour.green = 0xf8;
        gProgram_state.default_depth_effect.colour.blue = 0xf8;
    }
    PossibleService();
    ReadSpecialVolumes(f);
    ReadSoundGenerators(pTrack_spec, f);

    /* REFLECTIVE WINDSCREEN SPECIFICATIONS */

    /* Material to use for default screens */
    GetAString(f, s);
    gProgram_state.standard_screen = BrMapFind(s);

    /* Material to use for default screens during darkness */
    GetAString(f, s);
    gProgram_state.standard_screen_dark = BrMapFind(s);

    /* Material to use for default screens during fog */
    GetAString(f, s);
    gProgram_state.standard_screen_fog = BrMapFind(s);

    /* (ignore) # areas with different screens */
    gProgram_state.special_screens_count = GetAnInt(f);
    if (gProgram_state.special_screens_count != 0) {
        gProgram_state.special_screens = BrMemAllocate(sizeof(tSpecial_screen) * gProgram_state.special_screens_count, kMem_special_screen);
        C2_HOOK_BUG_ON(sizeof(tSpecial_screen) != 20);
        for (i = 0; i < gProgram_state.special_screens_count; i++) {
            GetFourScalars(f,
                &gProgram_state.special_screens[i].min_x,
                &gProgram_state.special_screens[i].min_z,
                &gProgram_state.special_screens[i].max_x,
                &gProgram_state.special_screens[i].max_z);
            GetAString(f, s);
            gProgram_state.special_screens[i].material = BrMaterialFind(s);
        }
    }

    PossibleService();

    /* MAP DETAILS */

    /* Map pixelmap name */
    GetAString(f, s);
    pRace_info->map_image = BrMapFind(s);
    if (pRace_info->map_image == NULL) {
        strtok(s, ".");
        pRace_info->map_image = BrMapFind(s);
        if (pRace_info->map_image == NULL) {
            Uppercaseificate(s, s);
            pRace_info->map_image = BrMapFind(s);
        }
    }
    BRPM_convert(pRace_info->map_image, gBack_screen->type);
    PrintMemoryDump(0, "JUST LOADING SKY/SPEC VOLS/SCREENS/MAP");
    /* World->map transformation matrix */
    for (i = 0; i < 4; ++i) {
        GetThreeScalars(f,
            &pRace_info->map_transformation.m[i][0],
            &pRace_info->map_transformation.m[i][1],
            &pRace_info->map_transformation.m[i][2]);
    }
    if (gGraf_specs[gGraf_spec_index].total_width != 640 || gGraf_specs[gGraf_spec_index].total_height != 480) {
        pRace_info->map_transformation.m[3][0] -= 320.f;
        pRace_info->map_transformation.m[3][1] -= 240.f;
        BrMatrix34PostRotateX(&pRace_info->map_transformation, BrDegreeToAngle(90));
        BrMatrix34PostScale(&pRace_info->map_transformation,
            (float)gGraf_specs[gGraf_spec_index].total_width / (float)640,
            0.f,
            (float)gGraf_specs[gGraf_spec_index].total_height / (float)480);
        BrMatrix34PostRotateX(&pRace_info->map_transformation, BrDegreeToAngle(270));
        pRace_info->map_transformation.m[3][0] += (float)gGraf_specs[gGraf_spec_index].total_width / 2;
        pRace_info->map_transformation.m[3][1] += (float)gGraf_specs[gGraf_spec_index].total_height / 2;
    }

    /* ****** START OF FUNK AND GROOVE STUFF ****** */

    /* START OF FUNK */
    GetALineAndDontArgue(f, s);
    AddFunkotronics(f, -2, 30 * GROOVE_FUNK_MAX_PER_CAR, NULL);

    /* FIXME: What is magic 1080 / 30 * 30 * GROOVE_FUNK_MAX_PER_CAR ?*/
    C2_HOOK_BUG_ON(30 * GROOVE_FUNK_MAX_PER_CAR != 1080);

    /* START OF GROOVE */
    GetALineAndDontArgue(f, s);
    C2_HOOK_ASSERT(strcmp(s, "START OF GROOVE") == 0);

    AddGroovidelics(f, -2, gUniverse_actor, 30 * GROOVE_FUNK_MAX_PER_CAR, 0);

    PossibleService();
    PrintMemoryDump(0, "JUST LOADING IN FUNKS AND GROOVES");

    for (i = 0; i < gTrack_storage_space.models_count; i++) {
        br_model* model;

        PossibleService();
        model = gTrack_storage_space.models[i];
        PossibleTreeSurgery(model);
        if (model != NULL && (model->flags & (BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE))) {
            if (model->identifier != NULL && model->identifier[0] != '-') {
                model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
            }
            for (j = 0; j < V11MODEL(model)->ngroups; j++) {
                v11group* v11_group = &V11MODEL(model)->groups[j];
                v11_group->face_colours.material = model->faces[*v11_group->face_user].material;
                if (v11_group->face_colours.material != NULL) {
                    SmoothificateWorldMaterial(v11_group->face_colours.material);
                    if (v11_group->face_colours.material->index_shade == NULL) {
                        v11_group->face_colours.material->index_shade = BrTableFind("DRRENDER.TAB");
                        BrMaterialUpdate(v11_group->face_colours.material, BR_MATU_ALL);
                    }
                }
            }
            if (model->flags & BR_MODF_UPDATEABLE) {
                BrModelUpdate(model, BR_MODU_ALL);
            } else {
                DodgyModelUpdate(model);
            }
        }
    }
    PossibleService();

    PrintMemoryDump(0, "JUST ABOUT TO LOAD IN AI WORLD");
    LoadAIWorldTrackInfo(f);
    PrintMemoryDump(0, "JUST LOADED IN AI WORLD");
    MungeSmashMaterialNames();
    PackFileRevertTiffLoading();

    /* number of material modifiers */
    count_material_modifiers = GetAnInt(f);

    for (i = 0; i < count_material_modifiers; i++) {
        tMaterial_modifiers* modifier = &pRace_info->material_modifiers[i];

        /* car wall friction */
        modifier->car_wall_friction = GetAFloat(f);

        /* tyre road friction */
        modifier->tyre_road_friction = GetAFloat(f);

        /* down force */
        modifier->down_force = GetAFloat(f);

        /* bumpiness */
        modifier->bumpiness = GetAFloat(f);

        /* tyre sound index */
        modifier->tyre_noise_index = GetAnInt(f);

        /* crash sound index */
        modifier->crash_noise_index = GetAnInt(f);

        /* scrape noise index */
        modifier->scrape_noise_index = GetAnInt(f);

        /* sparkiness */
        modifier->sparkiness = GetAFloat(f);

        /* room for expansion */
        modifier->smoke_type = GetAnInt(f);

        /* skid mark material */
        GetAString(f, s);
        str = strtok(s, ".");
        if (strcmp(str, "NONE") == 0) {
            modifier->skid_mark_material = NULL;
        } else if (strcmp(str, "0") == 0) {
            modifier->skid_mark_material = NULL;
        } else if (strcmp(str, "1") == 0) {
            modifier->skid_mark_material = NULL;
        } else {
            strcat(str, ".PIX");
            LoadSinglePixelmap(&gTrack_storage_space, str);
            str[strlen(str) - 4] = '\0';
            strcat(str, ".MAT");
            modifier->skid_mark_material = LoadSingleMaterial(&gTrack_storage_space, str);
        }
    }
    PackFileRerevertTiffLoading();
    for (i = count_material_modifiers; i < REC2_ASIZE(pRace_info->material_modifiers) - 1; i++) {
        tMaterial_modifiers* modifier = &pRace_info->material_modifiers[i];

        modifier->car_wall_friction = 1.f;
        modifier->tyre_road_friction = 1.f;
        modifier->down_force = 1.f;
        modifier->bumpiness = 0.f;
        modifier->tyre_noise_index = 0;
        modifier->crash_noise_index = 0;
        modifier->scrape_noise_index = 0;
        modifier->sparkiness = 1.f;
        modifier->smoke_type = 1;
        modifier->skid_mark_material = NULL;
    }
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].car_wall_friction = 1.f;
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].tyre_road_friction = 1.f;
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].down_force = 0.f;
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].bumpiness = 0.f;
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].tyre_noise_index = -1;
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].crash_noise_index = 0;
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].scrape_noise_index = 0;
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].sparkiness = 0.f;
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].smoke_type = 1;
    pRace_info->material_modifiers[REC2_ASIZE(pRace_info->material_modifiers) - 1].skid_mark_material = NULL;

    gDefault_water_spec_vol_real->material_modifier_index = 10;

    /* Non CarObjects */
    count_noncar_objects = GetAnInt(f);
    if (count_noncar_objects > 40) { /* FIXME: magic number */
        PDFatalError("Too many non-car types");
    }
    gProgram_state.num_non_car_spaces = count_noncar_objects + gCount_smashable_noncars + 40;
    gProgram_state.non_cars = BrMemCalloc(gProgram_state.num_non_car_spaces, sizeof(tNon_car_spec), kMem_non_car_spec);
    C2_HOOK_BUG_ON(sizeof(tNon_car_spec) != 260);
    if (gProgram_state.non_cars == NULL && count_noncar_objects != 0) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }
    memset(gNon_car_spec_indices, 0, sizeof(gNon_car_spec_indices));
    C2_HOOK_BUG_ON(sizeof(gNon_car_spec_indices) != 100);
    for (i = 0; i < 40; i++) { /* FIXME: magic */
        tPhysics_object *collision_info;
        tNon_car_spec* non_car;

        non_car = &gProgram_state.non_cars[i];
        non_car->driver = eDriver_non_car_unused_slot;
        collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
        C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 1240);
        non_car->collision_info = collision_info;
        collision_info->owner = non_car;
        collision_info->flags_0x238 = 1;
        collision_info->field_0x1a0 = 0xffff;
        collision_info->field_0x1a4 = 0;
    }
    for (i = 0; i < count_noncar_objects + gCount_smashable_noncars; i++) {
        tPhysics_object* collision_info;
        tNon_car_spec* non_car;
        char non_cars_path[256];
        FILE* g;

        PossibleService();
        non_car = &gProgram_state.non_cars[40 + i]; /* FIXME: magic number */
        collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
        C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 1240);
        non_car->collision_info = collision_info;
        collision_info->owner = non_car;
        collision_info->flags_0x238 = 1;
        collision_info->field_0x1a0 = 0xffff;
        collision_info->field_0x1a4 = 0;

        if (i < count_noncar_objects) {
            GetAString(f, s);
        } else {
            strcpy(s, gSmashable_noncars[i - count_noncar_objects]);
        }
        PathCat(non_cars_path, gApplication_path, "NONCARS");
        PathCat(non_cars_path, non_cars_path, s);
        g = DRfopen(non_cars_path, "rt");
        if (g == NULL) {
            FatalError(kFatalError_CantOpen_S, non_cars_path);
        }
        ReadNonCarMechanicsData(g, non_car);
        PossibleService();
        gNon_car_spec_indices[non_car->index] = i + 1;
        PFfclose(g);
    }
    DisposeSmashEnvNonCars();
    CheckNonCarModelBounds();
    GetSmokeShadeTables(f);
    pRace_info->count_network_start_points = GetAnInt(f);
    for (i = 0; i < pRace_info->count_network_start_points; i++) {
        GetThreeScalars(f,
            &pRace_info->net_starts[i].pos.v[0],
            &pRace_info->net_starts[i].pos.v[1],
            &pRace_info->net_starts[i].pos.v[2]);
        pRace_info->net_starts[i].yaw = GetAFloat(f);
    }
    LoadInKevStuff(f);
    gYon_multiplier = GetAFloat(f);
    GetAString(f, s);
    if (DRStricmp(s, pFile_name) != 0) {
        FatalError(kFatalError_FileIsCorrupted_S, pFile_name);
    }
    PFfclose(f);
    FreeExceptions();
    PrintMemoryDump(0, "FINISHED LOADING TRACK");
    LoadStaticLightingForRace(race_lighting_path);
    ClosePackFileAndSetTiffLoading(twt);
}

// FUNCTION: CARMA2_HW 0x00500f30
void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space) {
    int i;

    DRS3StopAllOutletSoundsExceptCDA();
    for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
        if (pStorage_space->pixelmaps[i] != NULL) {
            BrMapRemove(pStorage_space->pixelmaps[i]);
            BrPixelmapFree(pStorage_space->pixelmaps[i]);
        }
    }
    pStorage_space->pixelmaps_count = 0;
    for (i = 0; i < pStorage_space->shade_tables_count; i++) {
        if (pStorage_space->shade_tables[i] != NULL) {
            BrTableRemove(pStorage_space->shade_tables[i]);
            BrPixelmapFree(pStorage_space->shade_tables[i]);
        }
    }
    pStorage_space->shade_tables_count = 0;
    for (i = 0; i < pStorage_space->materials_count; i++) {
        if (pStorage_space->materials[i] != NULL) {
            BrMaterialRemove(pStorage_space->materials[i]);
            BrMaterialFree(pStorage_space->materials[i]);
        }
    }
    pStorage_space->materials_count = 0;
    for (i = 0; i < pStorage_space->models_count; i++) {
        if (pStorage_space->models[i] != NULL) {
            BrModelRemove(pStorage_space->models[i]);
            BrModelFree(pStorage_space->models[i]);
        }
    }
    pStorage_space->models_count = 0;
}

void C2_HOOK_FASTCALL ProcessModelFaceMaterials(br_model* pModel, tPMFMCB* pCallback) {
    tU16 f;
    br_material* new_mat = NULL;

    for (f = 0; f < pModel->nfaces; f++) {
        br_face* face = &pModel->faces[f];
        br_material* possible_mat;

        if (face->material == NULL) {
            continue;
        }
        possible_mat = pCallback(pModel, f);
        if (possible_mat == NULL) {
            continue;
        }
        face->material = possible_mat;
        new_mat = possible_mat;
    }
    if (new_mat != NULL) {
        BrModelUpdate(pModel, BR_MODU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x00448850
intptr_t C2_HOOK_CDECL ProcessFaceMaterials(br_actor* pActor, void* pData) {
    tPMFMCB* callback = pData;

    if (pActor->identifier != NULL && pActor->identifier[0] =='&') {
        return 0;
    }
    if (pActor->type == BR_ACTOR_MODEL && pActor->model != NULL) {
        ProcessModelFaceMaterials(pActor->model, callback);
    }
    return BrActorEnum(pActor, ProcessFaceMaterials, pData);
}

// FUNCTION: CARMA2_HW 0x00448ce0
int C2_HOOK_FASTCALL HasThisSuffix(char* pIdent, char* pSuffix) {
    size_t len_ident;
    size_t len_suffix;

    len_ident = strlen(pIdent);
    len_suffix = strlen(pSuffix);
    if (pIdent == NULL) {
        return 0;
    }
    if (len_ident < len_suffix) {
        return 0;
    }
    return strcmp(pIdent + len_ident - len_suffix, pSuffix) == 0;
}

// FUNCTION: CARMA2_HW 0x00448d70
br_material* C2_HOOK_FASTCALL UnsuffixedMaterial(char* pOld_ident, char* pSuffix) {
    br_material* result;
    int unsuffixed_len;
    char* new_id;

    unsuffixed_len = strlen(pOld_ident) - strlen(pSuffix);
    new_id = BrMemAllocate(unsuffixed_len + 1, kMem_new_mat_id);
    sprintf(new_id, "%.*s", unsuffixed_len, pOld_ident);
    result = BrMaterialFind(new_id);
    BrMemFree(new_id);
    return result;
}

// FUNCTION: CARMA2_HW 0x00448390
br_material* C2_HOOK_FASTCALL WallLinearToPersp(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".pwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".pwall");
    } else {
        new_mat = NULL;
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x004481d0
br_material* C2_HOOK_FASTCALL WallUntexToPersp(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".lwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".lwall");
    } else if (HasThisSuffix(old_mat->identifier, ".pwall")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".pwall");
    } else {
        new_mat = NULL;
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x00447c60
br_material* C2_HOOK_FASTCALL RoadUntexToPersp(br_model* pModel, tU16 pFace) {
    br_material* old_mat;
    br_material* new_mat;

    old_mat = pModel->faces[pFace].material;
    if (HasThisSuffix(old_mat->identifier, ".road")) {
        new_mat = UnsuffixedMaterial(old_mat->identifier, ".road");
    } else {
        new_mat = NULL;
    }
    return new_mat;
}

// FUNCTION: CARMA2_HW 0x00448dd0
br_material* C2_HOOK_FASTCALL DisposeSuffixedMaterials(br_model* pModel, tU16 pFace) {
    size_t max_suffix_len;
    br_material* mat;
    br_material* victim;
// GLOBAL: CARMA2_HW 0x005913a0
    static const char* suffixes[3] = {
        ".road",
        ".pwall",
        ".lwall",
    };
    int s;
    char* id;

    mat = pModel->faces[pFace].material;
    if (mat->identifier == NULL) {
        return NULL;
    }
    max_suffix_len = 0;
    for (s = 0; s < REC2_ASIZE(suffixes); s++) {
        if (max_suffix_len < strlen(suffixes[s])) {
            max_suffix_len = strlen(suffixes[s]);
        }
    }
    id = BrMemAllocate(strlen(mat->identifier) + max_suffix_len + 1, kMem_new_mat_id);
    for (s = 0; s < REC2_ASIZE(suffixes); s++) {
        sprintf(id, "%s%s", mat->identifier, suffixes[s]);
        victim = BrMaterialFind(id);
        if (victim != NULL) {
            BrMaterialRemove(victim);
            BrMaterialFree(victim);
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004489d0
void C2_HOOK_FASTCALL DisposeTexturingMaterials(void) {

    switch (gWall_texturing_level) {
    case eWTL_linear:
        ProcessFaceMaterials(gProgram_state.track_spec.the_actor, WallLinearToPersp);
        break;
    case eWTL_none:
        ProcessFaceMaterials(gProgram_state.track_spec.the_actor, WallUntexToPersp);
        break;
    default:
        break;
    }
    switch (gRoad_texturing_level) {
    case eRTL_none:
        ProcessFaceMaterials(gProgram_state.track_spec.the_actor, RoadUntexToPersp);
        break;
    default:
        break;
    }
    if (gWall_texturing_level != eWTL_full || gRoad_texturing_level != eRTL_full) {
        ProcessFaceMaterials(gProgram_state.track_spec.the_actor, DisposeSuffixedMaterials);
    }
}

// FUNCTION: CARMA2_HW 0x00506e20
intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg) {

    if (pActor->type == BR_ACTOR_BOUNDS || pActor->type == BR_ACTOR_BOUNDS_CORRECT) {
        BrResFree(pActor->type_data);
        pActor->type_data = NULL;
    }
    return 0;
}

void C2_HOOK_FASTCALL RemoveBoundsStructures(br_actor* pActor) {

    DRActorEnumRecurse(pActor, RemoveBounds, NULL);
}

// FUNCTION: CARMA2_HW 0x00506c20
void C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec) {
    int i;

    DisposeSmashableEnvironment();
    if (gAdditional_actors != NULL) {
        BrActorRemove(gAdditional_actors);
        BrActorFree(gAdditional_actors);
    }
    PossibleService();
    DisposeTexturingMaterials();
    PossibleService();
    RemoveBoundsStructures(pTrack_spec->the_actor);
    PossibleService();
    DisposeColumns(pTrack_spec);
    PossibleService();
    DisposeFunkotronics(-2);
    PossibleService();
    ClearOutStorageSpace(&gTrack_storage_space);
    gProgram_state.current_depth_effect.sky_texture = NULL;
    PossibleService();
    DisposeGroovidelics(-2);
    PossibleService();
    DisposeOpponentPaths();
    PossibleService();
    DisposeKevStuff();
    PossibleService();
    BrActorRemove(pTrack_spec->the_actor);
    BrActorFree(pTrack_spec->the_actor);
    pTrack_spec->the_actor = NULL;
    gTrack_actor = NULL;
    if (gProgram_state.special_volume_count != 0) {
        BrMemFree(gProgram_state.special_volumes);
    }
    if (gProgram_state.special_screens_count != 0) {
        BrMemFree(gProgram_state.special_screens);
    }
    PossibleService();
    for (i = 0; i < 40; i++) { /* FIXME: magic number 40 */
        tNon_car_spec *non_car;

        non_car = &gProgram_state.non_cars[i];
        non_car->collision_info->shape = NULL;
        non_car->collision_info->physics_joint1 = NULL;
        non_car->collision_info->physics_joint2 = NULL;
    }
    for (i = 0; i < gProgram_state.num_non_car_spaces; i++) {
        tNon_car_spec* non_car;

        non_car = &gProgram_state.non_cars[i];
        PossibleService();
        /* FIXME: comparison with eDriver_non_car in dethrace */
        if (non_car->driver == eDriver_4 && non_car->actor != NULL) {
            if (non_car->actor->parent != NULL) {
                BrActorRemove(non_car->actor);
            }
            BrActorFree(non_car->actor);
        }
        DisposePhysicsObject(non_car->collision_info);
    }
    if (gProgram_state.non_cars != NULL) {
        BrMemFree(gProgram_state.non_cars);
        /* gProgram_state.non_cars = NULL; */
    }
    FreeSmokeShadeTables();
    if (gTrack_flic_buffer != NULL) {
        BrMapRemove(gProgram_state.default_depth_effect.sky_texture);
        BrMemFree(gTrack_flic_buffer);
        EndFlic(&gTrack_flic_descriptor);
        BrMemFree(gProgram_state.default_depth_effect.sky_texture->pixels);
        gProgram_state.default_depth_effect.sky_texture->pixels = NULL;
        BrPixelmapFree(gProgram_state.default_depth_effect.sky_texture);
        gTrack_flic_buffer = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004ffd20
void C2_HOOK_FASTCALL FindInverseAndWorldBox(tSpecial_volume* pSpec) {
    br_bounds bnds;

    BrVector3Set(&bnds.min, -1.f, -1.f, -1.f);
    BrVector3Set(&bnds.max,  1.f,  1.f,  1.f);
    GetNewBoundingBox(&pSpec->boundary.box.bounds, &bnds, &pSpec->boundary.box.mat);
    BrMatrix34Inverse(&pSpec->boundary.box.inv_mat, &pSpec->boundary.box.mat);
}

// FUNCTION: CARMA2_HW 0x005074d0
void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* pMat, br_material* pMat_1, br_material* pMat_2, int pAxis_0, int pAxis_1) {

    pMat_1->map_transform.m[0][0] = 6.f * BrVector3Length((br_vector3*)pMat->m[pAxis_0]);
    pMat_1->map_transform.m[1][1] = 6.f * BrVector3Length((br_vector3*)pMat->m[pAxis_1]);
    BrMatrix23Copy(&pMat_2->map_transform, &pMat_1->map_transform);
}

// FUNCTION: CARMA2_HW 0x005072b0
br_uint_32 C2_HOOK_FASTCALL ApplyTransToModels(br_actor* pActor, br_matrix34* pMat, void* pData) {
    int i;

    if (pActor->identifier == NULL) {
        return 0;
    }
    if (pActor->identifier[0] == '&') {
        return 0;
    }
    if (pActor->model != NULL) {
        for (i = 0; i < pActor->model->nvertices; i++) {
            br_vector3 p;

            BrVector3Copy(&p, &pActor->model->vertices[i].p);
            BrMatrix34ApplyP(&pActor->model->vertices[i].p, &p, pMat);
        }
        BrModelUpdate(pActor->model, BR_MODU_ALL);
    }
    BrMatrix34Identity(&pActor->t.t.mat);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00507210
br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* pActor, br_matrix34* pMat, void* pData) {

    if ((pActor != gAdditional_actors && (pActor->identifier == NULL || pActor->identifier[0] == '&') && Vector3IsZero((br_vector3*)pMat->m[3]))
            || (pActor->model == NULL && pActor->type == BR_ACTOR_MODEL)) {
        int i;

        for (i = 0; i < gDelete_count; i++) {
            if (gDelete_list[i] == pActor) {
                return 0;
            }
        }
        gDelete_list[gDelete_count] = pActor;
        gDelete_count += 1;
    }
    return 0;
}

void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToAdditionalActors(void) {
    int i;

    for (i = 0; i < gNum_active_non_cars; i++) {
        tNon_car_spec *non_car = gActive_non_car_list[i];

        if (non_car->car_ID == -1 && non_car->actor->parent == gNon_track_actor) {
            BrActorRemove(non_car->actor);
            BrActorAdd(gAdditional_actors, non_car->actor);
        }
    }
}

void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToNonTrackActor(void) {
    int i;

    for (i = 0; i < gNum_active_non_cars; i++) {
        tNon_car_spec *non_car = gActive_non_car_list[i];

        if (non_car->car_ID == -1 && non_car->actor->parent == gAdditional_actors) {
            BrActorRemove(non_car->actor);
            BrActorAdd(gNon_track_actor, non_car->actor);
        }
    }
}

void C2_HOOK_FASTCALL DeleteAnyZeroBastards(void) {
    int i;

    gDelete_count = 0;
    DRActorEnumRecurseWithTrans(gAdditional_actors, NULL, DeleteBastards, NULL);
    for (i = gDelete_count - 1; i >= 0; i--) {
        BrActorRemove(gDelete_list[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00506f40
void C2_HOOK_FASTCALL SaveAdditionalActors(void) {

    PutAdditionalNonCarsOnToAdditionalActors();
    DeleteAnyZeroBastards();
    if (gLast_actor != NULL) {
        DRActorEnumRecurseWithTrans(gLast_actor, NULL, ApplyTransToModels, NULL);
    }
    if (strstr(gAdditional_actor_path, "autosave") != NULL) {
        BrActorSave(gAdditional_actor_path, gAdditional_actors);
        BrModelSaveMany(gAdditional_model_path, gAdditional_models, gNumber_of_additional_models);
    } else {
        tPath_name path1;
        tPath_name path2;
        char* str;
        int i;

        sprintf(path1, "%s", gAdditional_actor_path);
        str = strstr(path1, ".");
        if (str != NULL) {
            *str = '\0';
        }
        for (i = 0;; i++) {
            FILE* f;

            sprintf(path2, "%s%04d", path1, i);
            f = DRfopen(path2, "rb");
            if (f == NULL) {
                break;
            }
            PFfclose(f);
        }
        strcpy(path2, path1);
        sprintf(path1, "%s%04d.ACT", path2, i);
        BrActorSave(path1, gAdditional_actors);
        BrActorSave(gAdditional_actor_path, gAdditional_actors);
        sprintf(path2, "%s", gAdditional_model_path);
        str = strstr(path2, ".");
        if (str != NULL) {
            *str = '\0';
        }
        sprintf(path1, "%s%04d.DAT", path2, i);
        BrModelSaveMany(path1, gAdditional_models, gNumber_of_additional_models);
        BrModelSaveMany(gAdditional_model_path, gAdditional_models, gNumber_of_additional_models);
    }
    PutAdditionalNonCarsOnToNonTrackActor();
}

// FUNCTION: CARMA2_HW 0x004ffa20
void C2_HOOK_FASTCALL SaveSpecialVolumes(void) {
    tPath_name the_path;
    FILE* f;
    int i;

    PathCat(the_path, gApplication_path, "SPECSAVE.TXT");
    f = DRfopen(the_path, "wt");
    if (f == NULL) {
        return;
    }
    fprintf(f, "// SPECIAL EFFECTS VOLUMES\n\n");
    fprintf(f, "%d\t\t\t\t// # special effects volumes\n\n", gProgram_state.special_volume_count);
    for (i = 0; i < gProgram_state.special_volume_count; i++) {
        tSpecial_volume* v;

        v = &gProgram_state.special_volumes[i];
        switch (v->boundary_type) {
        case eFx_boundary_box:
            fprintf(f, "BOX\n");
            fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[0][0], v->boundary.box.mat.m[0][1], v->boundary.box.mat.m[0][2]);
            fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[1][0], v->boundary.box.mat.m[1][1], v->boundary.box.mat.m[1][2]);
            fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[2][0], v->boundary.box.mat.m[2][1], v->boundary.box.mat.m[2][2]);
            fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[3][0], v->boundary.box.mat.m[3][1], v->boundary.box.mat.m[3][2]);
            break;
        case eFx_boundary_plane:
            fprintf(f, "%.3f, %.3f, %.3f, %.3f\n", v->boundary.plane.v[0], v->boundary.plane.v[1], v->boundary.plane.v[2], v->boundary.plane.v[3]);
            break;
        case eFx_boundary_default:
            fprintf(f, "DEFAULT\n");
            break;
        default:
            break;
        }
        fprintf(f, "%.2f\t\t\t\t// gravity multiplier\n", v->gravity_multiplier);
        fprintf(f, "%.2f\t\t\t\t// viscosity multiplier\n", v->viscosity_multiplier);
        fprintf(f, "%.2f\t\t\t\t// Car damage per millisecond\n", v->car_damage_per_ms);
        fprintf(f, "%.2f\t\t\t\t// Pedestrian damage per millisecond\n", v->ped_damage_per_ms);
        fprintf(f, "%d\t\t\t\t\t// camera effect index\n", v->camera_special_effect_index);
        fprintf(f, "%d\t\t\t\t\t// sky colour\n", v->sky_col);
        fprintf(f, "%s\t\t\t\t// Windscreen texture to use\n", (v->screen_pixelmap != NULL) ? v->screen_pixelmap->identifier : "NONE");
        fprintf(f, "%d\t\t\t\t\t// Sound ID of entry noise\n", v->entry_noise);
        fprintf(f, "%d\t\t\t\t\t// Sound ID of exit noise\n", v->exit_noise);
        fprintf(f, "%d\t\t\t\t\t// Engine noise index\n", v->engine_noise_index);
        fprintf(f, "%d\t\t\t\t\t// material index\n", v->material_modifier_index);
        if (v->boundary_type == eFx_boundary_box || v->boundary_type == eFx_boundary_plane) {
            if (v->soundfx_type == kSoundFx_None) {
                fprintf(f,"NONE\t\t\t\t\t// sound type\n");
            } else {
                fprintf(f,"%s\t\t\t\t\t// sound type\n", gSoundType_Choices[v->soundfx_type]);
                WriteOutSoundSpec(f, &v->soundfx_data);
            }
        }
        fprintf(f, "\n");
    }
    PFfclose(f);
}

int C2_HOOK_FASTCALL FindSpecVolIndex(br_actor* pActor) {
    int i;

    if (pActor == NULL) {
        return -1;
    }
    for (i = 0; i < gProgram_state.special_volume_count; i++) {

        if (gSpec_vol_actors[i] == pActor) {
            return i;
        }
    }
    return -1;
}

void C2_HOOK_FASTCALL SetSpecVolMatSize(br_actor* pActor) {
    br_model* model;

    model = pActor->model;
    MungeMaterialSV(&pActor->t.t.mat, model->faces[ 5].material, model->faces[17].material, 0, 1);
    MungeMaterialSV(&pActor->t.t.mat, model->faces[11].material, model->faces[23].material, 1, 2);
    MungeMaterialSV(&pActor->t.t.mat, model->faces[ 7].material, model->faces[19].material, 0, 2);
}

void C2_HOOK_FASTCALL UpdateSpecVol(void) {
    int index;

    index = FindSpecVolIndex(gLast_actor);
    if (index >= 0) {
        tSpecial_volume* v;

        v = &gProgram_state.special_volumes[index];
        BrMatrix34Copy(&v->boundary.box.mat, &gLast_actor->t.t.mat);
        FindInverseAndWorldBox(v);
        SetSpecVolMatSize(gLast_actor);
    }
}

void C2_HOOK_FASTCALL DoSaveAdditionalStuff(void) {

    if (gSpec_vol_mode == 0) {
        SaveAdditionalActors();
    } else {
        UpdateSpecVol();
        SaveSpecialVolumes();
    }
}

// FUNCTION: CARMA2_HW 0x00507360
void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void) {
    size_t len_additional_actor_path;
    size_t len_additional_model_path;

    len_additional_actor_path = strlen(gAdditional_actor_path);
    len_additional_model_path = strlen(gAdditional_model_path);
    strcat(gAdditional_actor_path, " autosave");
    strcat(gAdditional_model_path, " autosave");
    DoSaveAdditionalStuff();
    gAdditional_actor_path[len_additional_actor_path] = '\0';
    gAdditional_model_path[len_additional_model_path] = '\0';
    FlushAllPedCaches();
}

// FUNCTION: CARMA2_HW 0x00506e80
void C2_HOOK_FASTCALL AnimateSky(void) {
    // GLOBAL: CARMA2_HW 0x006aafc0
    static tU32 prev_sky_animation;
    tU32 now;
    tU32 now2;
    int frame_increment;

    now = GetTotalTime();

    if (gTrack_flic_buffer == NULL) {
        return;
    }
    now2 = now;
    if (now < prev_sky_animation) {
        now2 = 2 * prev_sky_animation - now;
    }
    if (prev_sky_animation == 0) {
        frame_increment = 1;
    } else {
        frame_increment = ((now2 - prev_sky_animation) / (gTrack_flic_descriptor.frame_period)) % (gTrack_flic_descriptor.current_frame + gTrack_flic_descriptor.frames_left);
    }

    for (; frame_increment != 0; frame_increment -= 1) {
        if (PlayNextFlicFrame(&gTrack_flic_descriptor)) {
            EndFlic(&gTrack_flic_descriptor);
            StartFlic(gTrack_flic_descriptor.file_name, -1, &gTrack_flic_descriptor,
                gTrack_flic_buffer_size, (tS8*)gTrack_flic_buffer, NULL, 0, 0, 0);
            AssertFlicPixelmap(&gTrack_flic_descriptor, gProgram_state.default_depth_effect.sky_texture);
        }
        prev_sky_animation = now;
        SkyTextureChanged();
    }
}

// FUNCTION: CARMA2_HW 0x00515b80
void C2_HOOK_FASTCALL CalcActorGlobalPos(br_vector3* pResult, br_actor* pActor) {

    BrVector3Set(pResult, 0.f, 0.f, 0.f);
    for (;pActor != NULL && pActor != gNon_track_actor; pActor = pActor->parent) {
        if (pActor->t.t.mat.m[0][0] == 1.f) {
            BrVector3Accumulate(pResult, &pActor->t.t.translate.t);
        } else {
            br_vector3 tv;
            BrMatrix34ApplyP(&tv, pResult, &pActor->t.t.mat);
            BrVector3Accumulate(pResult, &tv);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e5ce0
int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, undefined4 pArg2, br_scalar pMax_distance) {
    br_vector3 distance_vector;
    int i;

#define CAMERA_MAX_DISTANCE(A) ((pMax_distance != 0.f) ? pMax_distance : REC2_SQR(((br_camera*)(A)->type_data)->yon_z))

    if (gMirror_on__graphics) {
        BrVector3Sub(&distance_vector, pPoint, (br_vector3*)gRearview_camera_to_world.m[3]);
        if (BrVector3LengthSquared(&distance_vector) < CAMERA_MAX_DISTANCE(gRearview_camera)
                && BrVector3Dot(&distance_vector, (br_vector3*)gRearview_camera_to_world.m[2]) < 0.f) {

            return 0;
        }
    }

    for (i = 0; i < gCount_extra_renders; i++) {
        br_actor* a = gExtra_renders[i].actor;

        BrVector3Sub(&distance_vector, pPoint, &a->t.t.translate.t);
        if (BrVector3LengthSquared(&distance_vector) < CAMERA_MAX_DISTANCE(a)) {

            return 0;
        }
    }

    BrVector3Sub(&distance_vector, pPoint, (br_vector3*)gCamera_to_world.m[3]);
    if (BrVector3LengthSquared(&distance_vector) < CAMERA_MAX_DISTANCE(gCamera)
            && BrVector3Dot(&distance_vector, (br_vector3*)gCamera_to_world.m[2]) < 0.f) {

        return 0;
    }
#undef CAMERA_MAX_DISTANCE
    return 1;
}

// FUNCTION: CARMA2_HW 0x00478e30
void C2_HOOK_FASTCALL PathGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    br_scalar pos;
    float f_the_time = (float)pTime;

#ifndef REC2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_interrupt_status, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_resumption_value, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.period.value, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.x_delta, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.y_delta, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.z_delta, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.straight_info.centre, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.period.value, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, path_data.circular_info.centre, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_position, 0x50);
#endif

    switch (pGroove->path_type) {
    case eGroove_path_straight:
        if (pGroove->path_data.straight_info.x_delta != 0.0f) {

            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, pGroove->path_data.straight_info.x_delta, -pGroove->path_data.straight_info.x_delta);
            pos += pGroove->path_data.straight_info.centre.v[0];
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][0]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                pMat->m[3][0] = pos;
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
            } else {
                if (pGroove->path_interrupt_status == eInterrupt_none) {
                    pMat->m[3][0] = pos;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                    if (pos < pGroove->path_resumption_value) {
                        pGroove->path_interrupt_status = eInterrupt_none;
                        pMat->m[3][0] = pos;
                        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    }
                } else if (pos > pGroove->path_resumption_value) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pMat->m[3][0] = pos;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                }
            }
        }

        if (pGroove->path_data.straight_info.y_delta != 0.0f) {

            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, pGroove->path_data.straight_info.y_delta, -pGroove->path_data.straight_info.y_delta);
            pos += pGroove->path_data.straight_info.centre.v[1];
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][1]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                pMat->m[3][1] = pos;
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
            } else {
                if (pGroove->path_interrupt_status == eInterrupt_none) {
                    pMat->m[3][1] = pos;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                    if (pos < pGroove->path_resumption_value) {
                        pGroove->path_interrupt_status = eInterrupt_none;
                        pMat->m[3][1] = pos;
                        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    }
                } else if (pos > pGroove->path_resumption_value) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pMat->m[3][1] = pos;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                }
            }
        }

        if (pGroove->path_data.straight_info.z_delta != 0.0f) {

            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, pGroove->path_data.straight_info.z_delta, -pGroove->path_data.straight_info.z_delta);
            pos += pGroove->path_data.straight_info.centre.v[2];
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][2]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                pMat->m[3][2] = pos;
            } else {
                if (pGroove->path_interrupt_status == eInterrupt_none) {
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][2] = pos;
                } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                    if (pos < pGroove->path_resumption_value) {
                        pGroove->path_interrupt_status = eInterrupt_none;
                        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                        pMat->m[3][2] = pos;
                    }
                } else if (pos > pGroove->path_resumption_value) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                    pMat->m[3][2] = pos;
                }
            }
        }
        BrVector3Copy(&pGroove->object_position, &pGroove->actor->t.t.translate.t);
        break;
    case eGroove_path_circular:
        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.circular_info.centre);
        if (pGroove->path_data.circular_info.axis == eAxis_y) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = cosf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0] + pos;
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = sinf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0] + pos;
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = sinf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1] + pos;
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = cosf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1] + pos;
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = cosf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[1] + pos;
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            if (pGroove->path_data.circular_info.period.value == 0.0f) {
                pos = 0.f;
            } else {
                pos = sinf(BrAngleToRadian(BrDegreeToAngle(fmodf((float)pTime, pGroove->path_data.circular_info.period.value) / pGroove->path_data.circular_info.period.value * 360.0f))) * pGroove->path_data.circular_info.radius;
            }
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[1] + pos;
        }
        BrVector3Copy(&pGroove->object_position, &pGroove->actor->t.t.translate.t);
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00479890
void C2_HOOK_FASTCALL ObjectGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    int rock_it;
    br_scalar x_size;
    br_scalar y_size;
    br_scalar z_size;
    br_scalar pos;
    br_bounds* bounds;
    float f_the_time = (float)pTime;

#ifndef REC2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_type, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_interrupt_status, 0x64);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_resumption_value, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.period, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.spin_info.axis, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.max_angle, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.rock_info.current_angle, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.x_period.value, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.y_period.value, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.z_period.value, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.x_magnitude, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.y_magnitude, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.throb_info.z_magnitude, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.x_period.value, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.y_period.value, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.z_period.value, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.x_magnitude, 0x78);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.y_magnitude, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroovidelic_spec, object_data.shear_info.z_magnitude, 0x80);
#endif

    switch (pGroove->object_type) {
    case eGroove_object_spin:
        switch (pGroove->object_data.spin_info.axis) {
        case eAxis_x:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pos));
            break;
        case eAxis_y:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pos));
            break;
        case eAxis_z:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pos));
            break;
        }
        break;
    case eGroove_object_rock:
        MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.rock_info.period.value, pGroove->object_data.rock_info.texture_info.data, pGroove->object_data.rock_info.max_angle, pGroove->object_data.rock_info.max_angle);
        rock_it = 1;
        if (pInterrupt_it) {
            pGroove->object_resumption_value = pos;
            if (pos >= pGroove->object_data.rock_info.current_angle) {
                pGroove->object_interrupt_status = eInterrupt_greater_than;
            } else {
                pGroove->object_interrupt_status = eInterrupt_less_than;
            }
        } else {
            if (pGroove->object_interrupt_status == eInterrupt_less_than) {
                if (pos >= pGroove->object_resumption_value || gAction_replay_mode) {
                    rock_it = 0;
                } else {
                    pGroove->object_interrupt_status = eInterrupt_none;
                }
            } else if (pGroove->object_interrupt_status == eInterrupt_greater_than) {
                if (pos <= pGroove->object_resumption_value || gAction_replay_mode) {
                    rock_it = 0;
                } else {
                    pGroove->object_interrupt_status = eInterrupt_none;
                }
            }
        }
        if (rock_it) {
            pGroove->object_data.rock_info.current_angle = pos;
        }

        switch (pGroove->object_data.rock_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        }
        break;
    case eGroove_object_throb:
        MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.throb_info.z_period.value, pGroove->object_data.throb_info.z_texture_info.data, pGroove->object_data.throb_info.z_magnitude, -pGroove->object_data.throb_info.z_magnitude);
        MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.throb_info.y_period.value, pGroove->object_data.throb_info.y_texture_info.data, pGroove->object_data.throb_info.y_magnitude, -pGroove->object_data.throb_info.y_magnitude);
        MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.throb_info.x_period.value, pGroove->object_data.throb_info.x_texture_info.data, pGroove->object_data.throb_info.x_magnitude, -pGroove->object_data.throb_info.x_magnitude);

        BrMatrix34PostScale(pMat, x_size + 1.f, y_size + 1.f, z_size + 1.f);
        /* FALLTHROUGH */
    case eGroove_object_shear:
        bounds = &pGroove->actor->model->bounds;
        if (pGroove->object_data.shear_info.x_magnitude == 0.0) {
            MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude, -pGroove->object_data.shear_info.z_magnitude);
            MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude, -pGroove->object_data.shear_info.y_magnitude);
            BrMatrix34PostShearX(pMat, y_size / (bounds->max.v[1] - bounds->min.v[1]), z_size / bounds->max.v[2] - bounds->min.v[2]);
        } else if (pGroove->object_data.shear_info.y_magnitude == 0.0) {
            MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude, -pGroove->object_data.shear_info.z_magnitude);
            MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude, -pGroove->object_data.shear_info.x_magnitude);
            BrMatrix34PostShearY(pMat, x_size / (bounds->max.v[0] - bounds->min.v[0]), z_size / (bounds->max.v[2] - bounds->min.v[2]));
        } else {
            MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude, -pGroove->object_data.shear_info.y_magnitude);
            MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude, -pGroove->object_data.shear_info.x_magnitude);
            BrMatrix34PostShearZ(pMat, x_size / (bounds->max.v[0] - bounds->min.v[0]), y_size / (bounds->max.v[1] - bounds->min.v[1]));
        }
        break;
    default:
        return;
    }
}

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
        BrVector3SetFloat(&vector_a, 1.f, 0.f, 0.f);
        break;
    case eLollipop_y_match:
        BrVector3SetFloat(&vector_a, 0.f, 1.f, 0.f);
        break;
    case eLollipop_z_match:
        BrVector3SetFloat(&vector_a, 0.f, 0.f, 1.f);
        break;
    case eLollipop_none:
        abort();
        break;
    }
    BrVector3Cross(&vector_b, &ref_to_subject, &vector_a);
    BrVector3Normalise(&vector_b, &vector_b);

    BrVector3Cross(&fixed_axis, &vector_a, &vector_b);

    switch (pWhich_axis) {
    case eLollipop_none:
        break;
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
        mat.m[3][0] = 0.f;
        mat.m[3][1] = 0.f;
        mat.m[3][2] = 0.f;
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
        mat.m[3][0] = 0.f;
        mat.m[3][1] = 0.f;
        mat.m[3][2] = 0.f;
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
        mat.m[3][0] = 0.f;
        mat.m[3][1] = 0.f;
        mat.m[3][2] = 0.f;
        break;
    }
    BrMatrix34Pre(&pSubject_actor->t.t.mat, &mat);
}

// FUNCTION: CARMA2_HW 0x00478c80
void C2_HOOK_FASTCALL GrooveThisDelic(tGroovidelic_spec* pGroove, tU32 pTime, int pInterrupt_it) {
    br_actor* the_actor;
    br_vector3 actor_pos;
    br_matrix34* the_mat;
    tInterrupt_status old_path_interrupt;
    tInterrupt_status old_object_interrupt;

    old_path_interrupt = pGroove->path_interrupt_status;
    old_object_interrupt = pGroove->object_interrupt_status;
    the_actor = pGroove->actor;
    pGroove->done_this_frame = 1;
    CalcActorGlobalPos(&actor_pos, the_actor);
    if (PointOutOfSight(&actor_pos, 0, pGroove->mode == eGroove_mode_distance ? gYon_squared : 36.f)) {
        return;
    }

    the_mat = &the_actor->t.t.mat;
    if ((!gAction_replay_mode
        || !ARReplayIsReallyPaused()
        || pGroove->path_mode == eMove_controlled
        || pGroove->path_mode == eMove_absolute) && !(pGroove->block_flags & 0x1)) {
        PathGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
    }
    if (((pGroove->object_type != eGroove_object_none || pGroove->lollipop_mode != eLollipop_none)
            && (!gAction_replay_mode
                    || !ARReplayIsReallyPaused()
                    || pGroove->object_mode == eMove_controlled
                    || pGroove->object_mode == eMove_absolute)) & !(pGroove->block_flags & 0x2)) {
        the_mat->m[0][0] = 1.0f;
        the_mat->m[0][1] = 0.0f;
        the_mat->m[0][2] = 0.0f;
        the_mat->m[1][0] = 0.0f;
        the_mat->m[1][1] = 1.0f;
        the_mat->m[1][2] = 0.0f;
        the_mat->m[2][0] = 0.0f;
        the_mat->m[2][1] = 0.0f;
        the_mat->m[2][2] = 1.0f;
        the_mat->m[3][0] = -pGroove->object_centre.v[0];
        the_mat->m[3][1] = -pGroove->object_centre.v[1];
        the_mat->m[3][2] = -pGroove->object_centre.v[2];
        ObjectGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
        the_actor->t.t.mat.m[3][0] += pGroove->object_position.v[0] + pGroove->object_centre.v[0];
        the_actor->t.t.mat.m[3][1] += pGroove->object_position.v[1] + pGroove->object_centre.v[1];
        the_actor->t.t.mat.m[3][2] += pGroove->object_position.v[2] + pGroove->object_centre.v[2];
        if (pGroove->lollipop_mode != eLollipop_none) {
            LollipopizeActor(pGroove->actor, &gCamera_to_world, pGroove->lollipop_mode);
        }
    }
    if (pGroove->path_interrupt_status != old_path_interrupt || pGroove->object_interrupt_status != old_object_interrupt) {
        PipeSingleGrooveStop(
            pGroove - gGroovidelics_array,
            the_mat,
            pGroove->path_interrupt_status,
            pGroove->object_interrupt_status,
            pGroove->path_resumption_value,
            pGroove->object_resumption_value);
    }
}

// FUNCTION: CARMA2_HW 0x00478c00
void C2_HOOK_FASTCALL GrooveThoseDelics(void) {
    int i;
    float f_the_time;

    if (gGroovidelics_array != NULL) {
        f_the_time = (float)GetTotalTime();
        gPrevious_groove_times[1] = gPrevious_groove_times[0];
        gPrevious_groove_times[0] = (tU32)f_the_time;

        for (i = 0; i < gGroovidelics_array_size; i++) {
            tGroovidelic_spec* the_groove;

            the_groove = &gGroovidelics_array[i];
            if (the_groove->owner != -999 && !the_groove->done_this_frame) {
                GrooveThisDelic(the_groove, (tU32)f_the_time, 0);
            }
        }
    }
}

float C2_HOOK_FASTCALL MapSawToTriangle(float pNumber) {

    if (pNumber >= 0.5) {
        return 3.0f - pNumber * 4.0f;
    } else {
        return pNumber * 4.0f - 1.0f;
    }
}

br_scalar C2_HOOK_FASTCALL NormaliseDegreeAngle(br_scalar pAngle) {

    while (pAngle < .0f) {
        pAngle += 360.f;
    }
    return pAngle;
}

// FUNCTION: CARMA2_HW 0x00477230
void C2_HOOK_FASTCALL FunkThoseTronics(void) {
    int i;
    int j;
    int iteration_count;
    int finished;
    tFunkotronic_spec* the_funk;
    br_vector3* the_proximity;
    tS32 the_time;
    br_matrix23* mat_matrix;
    br_material* the_material;
    float f_the_time;
    float rot_amount;
    float f_time_diff;
    br_vector2 tmp_v2;

    if (gFunkotronics_array == NULL) {
        return;
    }
    DontLetFlicFuckWithPalettes();
    the_time = GetTotalTime();
    f_the_time = (float)the_time;
    for (i = 0; i < gFunkotronics_array_size; i++) {
        the_funk = &gFunkotronics_array[i];
        if (the_funk->owner == -999) {
            continue;
        }
        if (the_funk->flags != 0) {
            continue;
        }
        j = 0;
        if (the_funk->mode == eFunk_mode_distance && the_funk->proximity_array != NULL) {
            for (j = 0; j < the_funk->proximity_count; j++) {
                int k;
                for (k = 0; k < 3; k++) {
                    the_proximity = &the_funk->proximity_array[j].v[k];
                    if (Vector3DistanceSquared(the_proximity, gOur_pos) <= gSight_distance_squared) {
                        br_vector3 tmp;

                        BrVector3Sub(&tmp, the_proximity, (br_vector3*)gCamera_to_world.m[3]);
                        if (BrVector3Dot(&tmp, &the_funk->proximity_array[j].n) < 0.f && BrVector3Dot(&tmp, (br_vector3*)gCamera_to_world.m[2]) < 0.f) {
                            j = -999;
                            break;
                        }
                    }
                }
                if (j < 0) {
                    break;
                }
            }
            if (j >= 0) {
                continue;
            }
        } else if (the_funk->mode == eFunk_mode_last_lap_only && gLap < gTotal_laps) {
            continue;
        } else if (the_funk->mode == eFunk_mode_all_laps_but_last && gLap >= gTotal_laps) {
            continue;
        }

        the_material = the_funk->material;
        mat_matrix = &the_material->map_transform;
        if (!gAction_replay_mode || !ARReplayIsReallyPaused() || the_funk->matrix_mode == eMove_controlled || the_funk->matrix_mode == eMove_absolute) {
            switch (the_funk->matrix_mod_type) {
            case eMatrix_mod_spin:
                BrMatrix23Identity(mat_matrix);
                the_material->map_transform.m[2][0] -= .5f;
                the_material->map_transform.m[2][1] -= .5f;
                if (the_funk->matrix_mod_data.spin_info.period > 0.f) {
                    f_time_diff = 1.f - fmodf(the_funk->matrix_mod_data.spin_info.period, 1.f);
                } else {
                    f_time_diff = fmodf(-the_funk->matrix_mod_data.spin_info.period, 1.f);
                }

                MOVE_FUNK_PARAMETER(rot_amount, the_funk->matrix_mode, f_time_diff, the_funk->matrix_mod_data.texture_info.data, 65536.f, -65536.f);
                DRMatrix23PostRotate(mat_matrix, (br_angle)rot_amount);

                the_material->map_transform.m[2][0] += .5f;
                the_material->map_transform.m[2][1] += .5f;
                break;
            case eMatrix_mod_rock:
                BrMatrix23Identity(mat_matrix);
                the_material->map_transform.m[2][0] -= the_funk->matrix_mod_data.rock_info.x_centre;
                the_material->map_transform.m[2][1] -= the_funk->matrix_mod_data.rock_info.y_centre;

                MOVE_FUNK_PARAMETER(rot_amount,
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.rock_info.period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.rock_info.rock_angle,
                    the_funk->matrix_mod_data.rock_info.rock_angle);
                DRMatrix23PostRotate(mat_matrix, BrDegreeToAngle(NormaliseDegreeAngle(rot_amount)));

                the_material->map_transform.m[2][0] += the_funk->matrix_mod_data.rock_info.x_centre;
                the_material->map_transform.m[2][1] += the_funk->matrix_mod_data.rock_info.y_centre;
                break;
            case eMatrix_mod_throb:
                BrMatrix23Identity(mat_matrix);
                the_material->map_transform.m[2][0] -= the_funk->matrix_mod_data.throb_info.x_centre;
                the_material->map_transform.m[2][1] -= the_funk->matrix_mod_data.throb_info.y_centre;

                MOVE_FUNK_PARAMETER(tmp_v2.v[1],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.throb_info.y_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.throb_info.y_magnitude,
                    the_funk->matrix_mod_data.throb_info.y_magnitude);
                MOVE_FUNK_PARAMETER(tmp_v2.v[0],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.throb_info.x_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.throb_info.x_magnitude,
                    the_funk->matrix_mod_data.throb_info.x_magnitude);
                BrMatrix23PostScale(mat_matrix, tmp_v2.v[0] + 1.f, tmp_v2.v[1] + 1.f);

                the_material->map_transform.m[2][0] += the_funk->matrix_mod_data.throb_info.x_centre;
                the_material->map_transform.m[2][1] += the_funk->matrix_mod_data.throb_info.y_centre;
                break;
            case eMatrix_mod_slither:
                MOVE_FUNK_PARAMETER(tmp_v2.v[0],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.slither_info.x_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.slither_info.x_magnitude,
                    the_funk->matrix_mod_data.slither_info.x_magnitude);
                the_material->map_transform.m[2][0] = tmp_v2.v[0];
                MOVE_FUNK_PARAMETER(tmp_v2.v[1],
                    the_funk->matrix_mode,
                    the_funk->matrix_mod_data.slither_info.y_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    the_funk->matrix_mod_data.slither_info.y_magnitude,
                    the_funk->matrix_mod_data.slither_info.y_magnitude);
                the_material->map_transform.m[2][1] = tmp_v2.v[1];
                break;
            case eMatrix_mod_roll:
                MOVE_FUNK_PARAMETER(tmp_v2.v[0], the_funk->matrix_mode,
                    the_funk->matrix_mod_data.roll_info.x_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    1.f,
                    -1.f);
                the_material->map_transform.m[2][0] = tmp_v2.v[0];
                MOVE_FUNK_PARAMETER(tmp_v2.v[1], the_funk->matrix_mode,
                    the_funk->matrix_mod_data.roll_info.y_period,
                    the_funk->matrix_mod_data.texture_info.data,
                    1.f,
                    -1.f);
                the_material->map_transform.m[2][1] = tmp_v2.v[1];
                break;
            case eMatrix_mod_none:
                break;
            }
            if (the_funk->matrix_mod_type != eMatrix_mod_none) {
                BrMaterialUpdate(the_funk->material, BR_MATU_MAP_TRANSFORM);
            }
        }
        if (the_funk->lighting_animation_type != eMove_none) {
            MOVE_FUNK_PARAMETER(the_material->ka, the_funk->lighting_animation_type,
                the_funk->lighting_animation_data.controlled.period,
                the_funk->lighting_animation_data.texture_info.data,
                the_funk->ambient_delta, -the_funk->ambient_delta);
            the_material->ka += the_funk->ambient_base;

            MOVE_FUNK_PARAMETER(the_material->kd, the_funk->lighting_animation_type,
                the_funk->lighting_animation_data.controlled.period,
                the_funk->lighting_animation_data.texture_info.data,
                the_funk->direct_delta,
                -the_funk->direct_delta);
            the_material->kd += the_funk->direct_base;

            MOVE_FUNK_PARAMETER(the_material->ks, the_funk->lighting_animation_type,
                the_funk->lighting_animation_data.controlled.period,
                the_funk->lighting_animation_data.texture_info.data,
                the_funk->specular_delta,
                -the_funk->specular_delta);
            the_material->ks += the_funk->specular_base;
        }
        switch (the_funk->texture_animation_type) {
        case eTexture_animation_none:
            break;
        case eTexture_animation_frames:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused() ||
                the_funk->mode == eFunk_mode_all_laps_but_last || the_funk->mode == 4) {
                br_uint_16 update_flags;
                br_pixelmap* new_colour_map;

                if (the_funk->time_mode == eTime_mode_accurate) {
                    MOVE_FUNK_PARAMETER(rot_amount, the_funk->texture_animation_data.frames_info.mode,
                        the_funk->texture_animation_data.frames_info.controlled.period,
                        the_funk->texture_animation_data.frames_info.texture_info.data,
                        (br_scalar)the_funk->texture_animation_data.frames_info.texture_count,
                        (br_scalar)-the_funk->texture_animation_data.frames_info.texture_count);
                    the_funk->texture_animation_data.frames_info.current_frame = (int) rot_amount;
                } else if (f_the_time - the_funk->last_frame >= the_funk->texture_animation_data.frames_info.controlled.period) {
                    the_funk->last_frame = f_the_time;
                    the_funk->texture_animation_data.frames_info.current_frame += 1;
                    if (the_funk->texture_animation_data.frames_info.current_frame >=
                        the_funk->texture_animation_data.frames_info.texture_count) {
                        the_funk->texture_animation_data.frames_info.current_frame = 0;
                    }
                }
                update_flags = 0;
                assert(the_funk->texture_animation_data.frames_info.current_frame >= 0);
                assert(the_funk->texture_animation_data.frames_info.current_frame < REC2_ASIZE(the_funk->texture_animation_data.frames_info.textures));
                new_colour_map = the_funk->texture_animation_data.frames_info.textures[the_funk->texture_animation_data.frames_info.current_frame];
                if (the_material->colour_map != new_colour_map) {
                    the_material->colour_map = new_colour_map;
                    the_material->user = new_colour_map;
                    update_flags |= BR_MATU_COLOURMAP;
                }
                the_material->colour_map = the_funk->texture_animation_data.frames_info.textures[the_funk->texture_animation_data.frames_info.current_frame];
                if (the_material->colour_map != new_colour_map) {
                    BrMaterialUpdate(the_funk->material, BR_MATU_COLOURMAP);
                }
                if (the_funk->texture_animation_data.frames_info.has_matrix) {
                    br_matrix23 *frame_mat = &the_funk->texture_animation_data.frames_info.mat[the_funk->texture_animation_data.frames_info.current_frame];
                    if (the_material->map_transform.m[0][0] != frame_mat->m[0][0]
                        || the_material->map_transform.m[1][1] != frame_mat->m[1][1]
                        || the_material->map_transform.m[2][0] != frame_mat->m[2][0]
                        || the_material->map_transform.m[2][1] != frame_mat->m[2][1]) {

                        BrMatrix23Copy(&the_material->map_transform, frame_mat);
                        update_flags |= BR_MATU_MAP_TRANSFORM;
                    }
                }
                if (update_flags != 0) {
                    BrMaterialUpdate(the_material, update_flags);
                }
            }
            break;
        case eTexture_animation_flic:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                tS32 i_last_frame = (tS32)the_funk->last_frame;
                tS32 i_time_diff;
                if (the_time < i_last_frame) {
                    i_time_diff = 2 * i_last_frame - the_time;
                } else {
                    i_time_diff = the_time;
                }
                if (the_funk->time_mode == eTime_mode_accurate) {
                    if (the_funk->last_frame) {
                        iteration_count = (i_time_diff - i_last_frame) /
                                          the_funk->texture_animation_data.flic_info.flic_descriptor.frame_period;
                    } else {
                        iteration_count = 1;
                    }
                } else {
                    if ((br_scalar)(i_time_diff - the_funk->last_frame) >= the_funk->texture_animation_data.flic_info.flic_descriptor.frame_period) {
                        iteration_count = 1;
                    } else {
                        iteration_count = 0;
                    }
                }
                for (j = 0; j < iteration_count; j++) {
                    finished = PlayNextFlicFrame(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                    BrMapUpdate(the_funk->material->colour_map, BR_MAPU_ALL);
                    BrMaterialUpdate(the_funk->material, BR_MATU_COLOURMAP);
                    if (finished) {
                        EndFlic(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                        StartFlic(
                                the_funk->texture_animation_data.flic_info.flic_descriptor.file_name,
                                -1,
                                &the_funk->texture_animation_data.flic_info.flic_descriptor,
                                the_funk->texture_animation_data.flic_info.flic_data_length,
                                (tS8 *) the_funk->texture_animation_data.flic_info.flic_data,
                                the_material->colour_map, 0, 0, 0);
                    }
                    the_funk->last_frame = f_the_time;
                }
            }
            break;
        case eTexture_animation_camera:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                br_vector3 camera_look;
                br_actor *camera_actor;
                if (the_funk->texture_animation_data.camera_info.count == 1) {
                    camera_actor = the_funk->texture_animation_data.camera_info.actors[0];
#ifdef REC2_FIX_BUGS
                    BrVector3Sub(&camera_look, &gProgram_state.current_car.pos,
                                 &camera_actor->t.t.translate.t);
#endif
                } else {
                    float closest_distance = (float) 0xffffffffu;
                    camera_actor = NULL;
                    for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                        br_vector3 delta;
                        br_actor *act = the_funk->texture_animation_data.camera_info.actors[i];
                        float dist;
                        BrVector3Sub(&delta, &gProgram_state.current_car.pos, &act->t.t.translate.t);
                        dist = BrVector3LengthSquared(&delta);
                        if (dist < closest_distance) {
                            closest_distance = dist;
                            BrVector3Copy(&camera_look, &delta);
                            camera_actor = act;
                        }
                    }
                }
                if (camera_actor != NULL) {
                    if (the_funk->texture_animation_data.camera_info.mode == 1) {
                        camera_actor->t.type = BR_TRANSFORM_LOOK_UP;
                        BrVector3Copy(&camera_actor->t.t.look_up.look, &camera_look);
                        BrVector3Set(&camera_actor->t.t.look_up.up, 0.f, 1.f, 0.f);
                    }
                    AddExtraRender(camera_actor, the_material);
                }
            }
            break;
        case eTexture_animation_mirror:
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                if (the_funk->texture_animation_data.mirror_info.actor != NULL) {
                    BrMatrix34Mul(&the_funk->texture_animation_data.mirror_info.actor->t.t.mat,
                                  &gCamera_to_world,
                                  &the_funk->texture_animation_data.mirror_info.mat);
                    AddExtraRender(the_funk->texture_animation_data.mirror_info.actor, the_material);
                }
            }
            break;
        }
    }
    LetFlicFuckWithPalettes();
}

// FUNCTION: CARMA2_HW 0x004e5ad0
void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor) {

    if (gHud_actor_storage_size >= REC2_ASIZE(gHud_actor_storage)) {
        BrFailure("Not enough HUD actor storage");
    }
    gHud_actor_storage[gHud_actor_storage_size] = pActor;
    gHud_actor_storage_size += 1;
}

// FUNCTION: CARMA2_HW 0x004e5cb0
void C2_HOOK_FASTCALL InitialiseExtraRenders(void) {

    gCount_extra_renders = 0;
}

// FUNCTION: CARMA2_HW 0x004e5cc0
void C2_HOOK_FASTCALL AddExtraRender(br_actor* pActor, br_material* pMaterial) {

    gExtra_renders[gCount_extra_renders].actor = pActor;
    gExtra_renders[gCount_extra_renders].material = pMaterial;
    gCount_extra_renders += 1;
}

// FUNCTION: CARMA2_HW 0x0047b1e0
void C2_HOOK_FASTCALL ResetGrooveFlags(void) {
    int i;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        tGroovidelic_spec* the_groove;

        the_groove = &gGroovidelics_array[i];
        the_groove->done_this_frame = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0047b250
void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex) {

    gFunkotronics_array[pIndex].flags |= 0x1;
    PipeSingleFunkEnable(pIndex, 0);
}

// FUNCTION: CARMA2_HW 0x0047b280
void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex) {

    gFunkotronics_array[pIndex].flags &= ~0x1;
    PipeSingleFunkEnable(pIndex, 1);
}

// FUNCTION: CARMA2_HW 0x0047b0e0
void C2_HOOK_FASTCALL StopGroovidelic(br_actor* actor) {
    int i;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        tGroovidelic_spec* groove = &gGroovidelics_array[i];
        if (groove->path_interrupt_status == eInterrupt_none &&
                groove->object_interrupt_status == eInterrupt_none) {
            GrooveThisDelic(groove, gPrevious_groove_times[i], 1);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b360
tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor) {
    int i;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        tGroovidelic_spec* groove = &gGroovidelics_array[i];

        if (groove->actor == pActor) {
            return groove;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0047b160
void C2_HOOK_FASTCALL EnableGroovidelic(br_actor *pActor, tU32 pBlock_flags) {
    int i;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        tGroovidelic_spec* groove = &gGroovidelics_array[i];

        if (groove->actor == pActor) {
            groove->block_flags &= ~pBlock_flags;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0051db90
br_uint_32 C2_HOOK_CDECL ZlibFsGetAttributes(void) {

    return BR_FS_ATTR_READABLE | BR_FS_ATTR_WRITEABLE | BR_FS_ATTR_HAS_BINARY;
}

// FUNCTION: CARMA2_HW 0x0051dc10
void* C2_HOOK_CDECL ZlibFsOpenRead(const char* buffer, br_size_t capacity, br_mode_test_cbfn* cbfn, int* type) {

    return gzopen(buffer, "rb");
}

// FUNCTION: CARMA2_HW 0x0051dc20
void* C2_HOOK_CDECL ZlibFsOpenWrite(const char* path, int type) {

    if (type != BR_FS_MODE_BINARY) {
        return NULL;
    }
    return gzopen(path, "wb");
}

// FUNCTION: CARMA2_HW 0x0051dba0
void C2_HOOK_CDECL ZlibFsClose(void* context) {

    gzclose(context);
}

// FUNCTION: CARMA2_HW 0x0051dbb0
int C2_HOOK_CDECL ZlibFsEof(void* context) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0051db40
int C2_HOOK_CDECL ZlibFsGetChr(void* context) {

    return gzgetc((gzFile)context);
}

// FUNCTION: CARMA2_HW 0x0051dc40
void C2_HOOK_CDECL ZlibFsPutChr(int chr, void* context) {

    gzputc(context, chr);
}

// FUNCTION: CARMA2_HW 0x0051dca0
br_size_t C2_HOOK_CDECL ZlibFsRead(void* buffer, br_size_t elem_size, unsigned int count, void* context) {

    int bytes_read = gzread(context, buffer, elem_size * count);
    if (bytes_read < 0) {
        return 0;
    }
    return bytes_read / elem_size;
}

// FUNCTION: CARMA2_HW 0x0051dcd0
br_size_t C2_HOOK_CDECL ZlibFsWrite(const void* buffer, br_size_t elem_size, unsigned int count, void* context) {
    int bytes_written = gzwrite(context, buffer, elem_size * count);

    return bytes_written / elem_size;
}

// FUNCTION: CARMA2_HW 0x0051dbc0
br_size_t C2_HOOK_CDECL ZlibFsGetLine(char* buffer, br_size_t capacity, void* context) {
    br_size_t i;

    for (i = 0; i < capacity - 1; i++) {
        int c = gzgetc((gzFile)context);
        if (c == EOF) {
            return i;
        }
        if (c == '\n') {
            return i;
        }
        buffer[i] = c;
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x0051dc60
void C2_HOOK_CDECL ZlibFsPutLine(char* line, void* context) {
    char newline = '\n';
    size_t len = strlen(line);

    gzwrite(context, line, len);
    gzwrite(context, &newline, 1);
}

// FUNCTION: CARMA2_HW 0x0051db60
void C2_HOOK_CDECL ZlibFsAdvance(br_size_t advance, void* context) {

    while (advance != 0) {
        gzgetc((gzFile)context);
        advance -= 1;
    }
}

// FUNCTION: CARMA2_HW 0x004ff120
tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3) {
    int i;
    tSpecial_volume* result;

    result = NULL;
    for (i = 0; i < gProgram_state.special_volume_count && (result == NULL || pArg3); i++) {
        tSpecial_volume* v = &gProgram_state.special_volumes[i];
        tSpecial_volume* sound_volume = NULL;
        tSpecial_volume* containing_volume = sound_volume;
        float sound_dist = 1e7f;
        br_vector3 sound_dir;
        int fartest_bnds_axis;

#ifdef REC2_FIX_BUGS
        fartest_bnds_axis = 0;
#endif

        if (v->boundary_type == eFx_boundary_box) {
            if (pP->v[0] > v->boundary.box.bounds.min.v[0]
                    && pP->v[0] < v->boundary.box.bounds.max.v[0]
                    && pP->v[1] > v->boundary.box.bounds.min.v[1]
                    && pP->v[1] < v->boundary.box.bounds.max.v[1]
                    && pP->v[2] > v->boundary.box.bounds.min.v[2]
                    && pP->v[2] < v->boundary.box.bounds.max.v[2]) {
                br_vector3 pnt_norm;
                float fartest_dist;
                int j;

                BrMatrix34ApplyP(&pnt_norm, pP, &v->boundary.box.inv_mat);
                fartest_dist = 0.f;
                for (j = 0; j < 3; j++) {
                    float dist;
                    int bnds_axis;
                    if (pnt_norm.v[j] <= -1.f) {
                        dist = -1.f - pnt_norm.v[j];
                        bnds_axis = j + 3;
                    } else if (pnt_norm.v[j] > 1.f) {
                        dist = pnt_norm.v[j] - 1.f;
                        bnds_axis = j;
                    } else {
                        dist = 0.f;
                    }
                    if (dist > fartest_dist) {
                        fartest_bnds_axis = bnds_axis;
                        fartest_dist = dist;
                    }
                }
                if (fartest_dist != 0.f && v->soundfx_type != kSoundFx_None) {
                    int axis = fartest_bnds_axis % 3;

                    fartest_dist /= v->boundary.box.axis_length.v[axis];
                    if (fartest_dist < 1e7f) {
                        float delta = (float)(fartest_bnds_axis < 3 ? 1 : -1);
                        BrVector3Scale(&sound_dir, (br_vector3*)v->boundary.box.mat.m[axis], delta);
                        sound_volume = v;
                        sound_dist = fartest_dist;
                    }
                } else if (fartest_dist == 0.f) {
                    containing_volume = v;
                }
            }
        } else if (v->boundary_type == eFx_boundary_plane) {
            float dist;

            dist = DistanceFromFaceND(pP, (br_vector3*)&v->boundary.plane, v->boundary.box.mat.m[1][0]);
            containing_volume = v;
            if (dist >= 0.f) {
                containing_volume = NULL;
                if (dist < 1e7f && v->soundfx_type != kSoundFx_None) {
                    BrVector3Copy(&sound_dir, &v->boundary.plane);
                    sound_volume = v;
                    sound_dist = dist;
                }
            }
        }
        if (containing_volume != NULL) {
            sound_dist = 0.f;
            sound_volume = containing_volume;
            if (result == NULL) {
                result = containing_volume;
            }
        }
        if (pArg3 && sound_volume != NULL && sound_dist < 10.f) {
            DoEnvSound(sound_volume, pP, sound_volume->soundfx_type, &sound_volume->soundfx_data, sound_dist, &sound_dir);
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004748a0
void C2_HOOK_FAKE_THISCALL ControlBoundFunkGroove(int pSlot_number, undefined4 pArg2, float pValue) {

    if (pSlot_number < 0) {
        return;
    }
    if (pSlot_number >= REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = pValue;
    }
}

// FUNCTION: CARMA2_HW 0x004748e0
float C2_HOOK_FAKE_THISCALL ControlBoundFunkGroovePlus(int pSlot_number, undefined4 pArg2, float pValue) {

    if (pSlot_number < 0) {
        return 0.f;
    }
    if (pSlot_number >= REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = fmodf(*gGroove_funk_bindings[pSlot_number].v + pValue, 1.f);
    }
    return *gGroove_funk_bindings[pSlot_number].v;
}

// FUNCTION: CARMA2_HW 0x00506e50
void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {

    RenderTrack(pWorld, pTrack_spec, pCamera, pCamera_to_world_transform);
    if (gAdditional_actors) {
        BrZbSceneRenderAdd(gAdditional_actors);
    }
}
