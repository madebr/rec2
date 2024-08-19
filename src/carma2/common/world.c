#include "world.h"

#include "animation.h"
#include "brucetrk.h"
#include "car.h"
#include "crush.h"
#include "depth.h"
#include "drmem.h"
#include "errors.h"
#include "finteray.h"
#include "flicplay.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "opponent.h"
#include "pedestrn.h"
#include "shrapnel.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "utility.h"

#include "platform.h"

#include <brender/brender.h>
#include "rec2_macros.h"

#include <tiffio.h>

#include "c2_ctype.h"
#include "c2_string.h"
#include "c2_sys/c2_stat.h"
#include "rec2_types.h"

#define RGB565_R(V) (((V) & 0xf800) >> 11)
#define RGB565_G(V) (((V) & 0x07e0) >> 5)
#define RGB565_B(V) (((V) & 0x001f) >> 0)
#define RGB888_R(V) (((V) >> 16) & 0xff)
#define RGB888_G(V) (((V) >> 8) & 0xff)
#define RGB888_B(V) (((V) >> 0) & 0xff)

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSoundType_Choices, 2, 0x00660268, {"SATURATED", "SCATTERED"});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tCar_texturing_level, gCar_texturing_level, 0x00591374, eCTL_full);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tRoad_texturing_level, gRoad_texturing_level, 0x0059136c, eRTL_full);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tWall_texturing_level, gWall_texturing_level, 0x00591370, eWTL_full);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gRendering_accessories, 0x00591368, 1);
C2_HOOK_VARIABLE_IMPLEMENT(tBrender_storage*, gStorageForCallbacks, 0x006b7820);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gAddedPixelmap, 0x006aaa20);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisallowDuplicates, 0x006aaa2c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tRendererShadingType, gMaterial_shading_for_callback, 0x00660cb8, kRendererShadingType_Undefined);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gDuplicate_model, 0x006aaa24);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gDuplicate_material, 0x006aaa28);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gSight_distance_squared, 0x0068b840);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_filesystem, zlibFilesystem, 0x006631c0, TODO);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGroovidelics_array_size, 0x0068b848);
C2_HOOK_VARIABLE_IMPLEMENT(tGroovidelic_spec*, gGroovidelics_array, 0x0068b850);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gRace_path, 256, 0x0074d0a0);

C2_HOOK_VARIABLE_IMPLEMENT(tMaterial_exception*, gMaterial_exceptions, 0x0074ca04);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSmashable_item_mode_names, 6, 0x0065fe88, {
    "nochange",
    "decal",
    "texturechange",
    "remove",
    "replacemodel",
    "crush",
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_smashable_noncars, 0x006b788c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_smashable_noncar_shrapnel_actors, 0x006a55b0);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_environment_name*, gSmashable_noncars, 0x006b7888);
C2_HOOK_VARIABLE_IMPLEMENT(const char*, gSmashable_track_environment_path, 0x006a55b8);

C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_track_smashable_environment_specs, 0x006a55b4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_track_smashable_environment_specs_2, 0x006a8288);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCapacity_track_smashables, 0x006a3330);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_item_spec*, gTrack_smashable_environment_specs, 0x006a5138);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmashable_item_spec*, gSmashable_track_indexable_triggers, 100, 0x006ba4a0);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gDepth_effect_names, 3, 0x00660e90, {
    "dark",
    "fog",
    "colour",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gAdditional_model_path, 256, 0x006b6400);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gAdditional_actor_path, 256, 0x006b6500);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_model*, gAdditional_models, 1000, 0x006b6620);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTrack_version, 0x007634b4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_additional_models, 0x006ab948);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gAdditional_actors, 0x006b75c0);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gLast_actor, 0x006b6600);
C2_HOOK_VARIABLE_IMPLEMENT(tU8*, gTrack_flic_buffer, 0x006aaf40);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gTrack_flic_buffer_size, 0x006aaf48);
C2_HOOK_VARIABLE_IMPLEMENT(tFlic_descriptor, gTrack_flic_descriptor, 0x006aaf50);

C2_HOOK_VARIABLE_IMPLEMENT(br_angle, gSky_image_width, 0x0079ec2e);
C2_HOOK_VARIABLE_IMPLEMENT(br_angle, gSky_image_height, 0x0079ec2c);
C2_HOOK_VARIABLE_IMPLEMENT(br_angle, gSky_image_underground, 0x0079ec30);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTrack_depth_colour_red, 0x0074caa8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTrack_depth_colour_green, 0x0074cf2c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTrack_depth_colour_blue, 0x0074cad0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tU8, gNon_car_spec_indices, 100, 0x0079ef40);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tU8, gNon_car_indices, 88, 0x0079ed30);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tUnknown_0x006a7fc8, gUnknown_0x006a7fc8, 25, 0x006a7fc8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSpecial_effects_boundary_choices, 4, 0x00660258, {
    "BOX",
    "PLANE",
    "DEFAULT",
    "NEW",
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gDefault_engine_noise_index, 0x0068b878);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSoundGeneratorTypeNames, 3, 0x00595c38, {
    "NONCAR",
    "ACTOR",
    "POINT",
});

tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void) {

    return C2V(gCar_texturing_level);
}
C2_HOOK_FUNCTION(0x00447640, GetCarTexturingLevel)

void (C2_HOOK_FASTCALL * SetCarTexturingLevel_original)(tCar_texturing_level pLevel);
void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel) {
#if defined(C2_HOOKS_ENABLED)
    SetCarTexturingLevel_original(pLevel);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447650, SetCarTexturingLevel, SetCarTexturingLevel_original)

tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void) {

    return C2V(gRoad_texturing_level);
}
C2_HOOK_FUNCTION(0x00448770, GetRoadTexturingLevel)

void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel) {

    C2V(gRoad_texturing_level) = pLevel;
}
C2_HOOK_FUNCTION(0x00448780, SetRoadTexturingLevel)

tWall_texturing_level C2_HOOK_FASTCALL GetWallTexturingLevel(void) {

    return C2V(gWall_texturing_level);
}
C2_HOOK_FUNCTION(0x004488f0, GetWallTexturingLevel)

void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel) {

    C2V(gWall_texturing_level) = pLevel;
}
C2_HOOK_FUNCTION(0x00448900, SetWallTexturingLevel)

void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel) {

    C2V(gCar_simplification_level) = pLevel;
}
C2_HOOK_FUNCTION(0x00448f10, SetCarSimplificationLevel)

int C2_HOOK_FASTCALL GetCarSimplificationLevel(void) {

    return C2V(gCar_simplification_level);
}
C2_HOOK_FUNCTION(0x00448f20, GetCarSimplificationLevel)

void C2_HOOK_FASTCALL ApplyMaterialCallbackOnAllMaterials(br_model* pModel, material_cbfn* pCallback) {
    if (pModel->faces == NULL) {
        v11model* v11m = pModel->prepared;
        if (v11m != NULL) {
            int g;

            for (g = 0; g < v11m->ngroups; g++) {
                int f;
                v11group* v11g = &v11m->groups[g];

                for (f = 0; f < v11g->nfaces; f++) {
                    br_material* mat = *v11g->face_colours.materials;
                    if (mat != NULL) {
                        pCallback(mat);
                    }
                }
            }
        }
    } else {
        int i;

        for (i = 0; i < pModel->nfaces; i++) {
            br_face* face = &pModel->faces[i];
            if (face->material != NULL) {
                pCallback(face->material);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x00448fd0, ApplyMaterialCallbackOnAllMaterials)

intptr_t C2_HOOK_CDECL ApplyMaterialCallbackOnAllModelActorsCallback(br_actor* pActor, material_cbfn* pCallback) {

    if (pActor->material != NULL) {
        pCallback(pActor->material);
    }
    if (pActor->type == BR_ACTOR_MODEL && pActor->model != NULL) {
        ApplyMaterialCallbackOnAllMaterials(pActor->model, pCallback);
    }
    return BrActorEnum(pActor, (br_actor_enum_cbfn*)ApplyMaterialCallbackOnAllModelActorsCallback, pCallback);
}
C2_HOOK_FUNCTION(0x00448f90, ApplyMaterialCallbackOnAllModelActorsCallback)

intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB(br_actor* pActor, void* pFlag) {
    if (pActor->identifier != NULL && pActor->identifier[0] == '&') {
        pActor->render_style = *(br_uint_8*)pFlag;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00448ea0, SetAccessoryRenderingCB)

void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn) {
    int style;

    if (C2V(gTrack_actor) != NULL)  {
        if (pOn) {
            style = BR_RSTYLE_FACES;
        } else {
            style = BR_RSTYLE_NONE;
        }
        DRActorEnumRecurse(C2V(gTrack_actor), (br_actor_enum_cbfn*)SetAccessoryRenderingCB, &style);
    }
    C2V(gRendering_accessories) = pOn;
}
C2_HOOK_FUNCTION(0x00448ec0, SetAccessoryRendering)

int C2_HOOK_FASTCALL GetAccessoryRendering(void) {

    return C2V(gRendering_accessories);
}
C2_HOOK_FUNCTION(0x00448f00, GetAccessoryRendering)

int C2_HOOK_FASTCALL LoadBunchOfPixies(const char* pathRoot, const char* textureName, br_pixelmap** textureBuffer, size_t bufferCapacity) {
    tPath_name tempPath;
    br_uint_32 nb;

    PathCat(tempPath, pathRoot, textureName);
    c2_strcat(tempPath, (C2V(gPixelFlags) & kPixelFlags_16bbp) ? ".P16" : ".P08");
    nb = BrPixelmapLoadMany(tempPath, textureBuffer, (br_uint_16)bufferCapacity);
    if (nb == 0) {
        PathCat(tempPath, pathRoot, (C2V(gPixelFlags) & kPixelFlags_16bbp) ? "PIX16" : "PIX8");
        PathCat(tempPath, tempPath, textureName);
        c2_strcat(tempPath, ".PIX");
        nb = BrPixelmapLoadMany(tempPath, textureBuffer, (br_uint_16)bufferCapacity);
    }
    if (nb == 0) {
        return 0;
    }
    if (nb == 1 && !C2V(gDisableTiffConversion)) {
        if (textureBuffer[0]->identifier != NULL) {
            BrResFree(textureBuffer[0]->identifier);
        }
        textureBuffer[0]->identifier = BrResStrDup(textureBuffer[0], textureName);
    }
    return nb;
}
C2_HOOK_FUNCTION(0x0048ea40, LoadBunchOfPixies)

void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx) {
    char s[256];

    REC2_BUG_ON(sizeof(tSpecial_volume) != 220);
    REC2_BUG_ON(sizeof(tSpecial_volume_soundfx_data) != 36);
    REC2_BUG_ON(offsetof(tSpecial_volume, gravity_multiplier) != 136);

    /* gravity multiplier */
    pSpec->gravity_multiplier = GetAFloat(pF);
    if (pSpec->gravity_multiplier < 0 && !C2V(gRussels_fannies)) {
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
        c2_strcpy(pScreen_name_str, s);
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
        pSpec->soundfx_type = GetALineAndInterpretCommand(pF, C2V(gSoundType_Choices), REC2_ASIZE(C2V(gSoundType_Choices)));
        if (pSpec->soundfx_type != kSoundFx_None) {
            ReadSoundSpec(pF, &pSpec->soundfx_data);
        }
    } else {
        pSpec->soundfx_type = kSoundFx_None;
    }
}
C2_HOOK_FUNCTION(0x004ff8d0, ParseSpecialVolume)

int C2_HOOK_FASTCALL AddTexturePixTifFileStemToList(const char *path, tName_list *pList) {
    tPath_name pathCopy;
    tPath_name pathUpper;
    tPath_name dir_path;
    tPath_name stem_path;

    c2_strcpy(pathCopy, path);
    Uppercaseificate(pathUpper, pathCopy);
    if (c2_strstr(pathUpper, ".PIX") == NULL
            && c2_strstr(pathUpper, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(pathUpper, dir_path, stem_path);
    c2_strcpy(pList->items[pList->size], stem_path);
    if (pList->size < REC2_ASIZE(pList->items)) {
        pList->size += 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x005026b0, AddTexturePixTifFileStemToList)

int C2_HOOK_FASTCALL AddTextureFileStemToList(const char* path, tName_list* pList) {
    tPath_name pathCopy;
    tPath_name upperPath;
    tPath_name dir_path;
    tPath_name stem_path;
    int alreadyInList;
    size_t i;

    c2_strcpy(pathCopy, path);
    Uppercaseificate(upperPath, pathCopy);

    if (c2_strstr(upperPath, ".PIX") == NULL
            && c2_strstr(upperPath, ".P16") == NULL
            && c2_strstr(upperPath, ".P08") == NULL
            && c2_strstr(upperPath, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(upperPath, dir_path, stem_path);

    alreadyInList = 0;
    for (i = 0; i < pList->size; i++) {
        if (c2_strcmp(pList->items[i], stem_path) == 0) {
            alreadyInList = 1;
            break;
        }
    }
    if (!alreadyInList) {
        c2_strcpy(pList->items[pList->size], stem_path);
        if (pList->size < REC2_ASIZE(pList->items)) {
            pList->size += 1;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00502780, AddTextureFileStemToList);

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
C2_HOOK_FUNCTION(0x00500d50, InitialiseStorageSpace)

void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage) {
    BrMemFree(pStorage->pixelmaps);
    BrMemFree(pStorage->shade_tables);
    BrMemFree(pStorage->materials);
    BrMemFree(pStorage->models);
    BrMemFree(pStorage->sounds);
    BrMemFree(pStorage->materialProps);
}
C2_HOOK_FUNCTION(0x00500e10, DisposeStorageSpace)


tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm) {
    int i;

    C2V(gAddedPixelmap) = NULL;
    if (pStorage_space->pixelmaps_count >= pStorage_space->max_pixelmaps) {
        C2V(gAddedPixelmap) = NULL;
        return eStorage_not_enough_room;
    }

    for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
        if (pStorage_space->pixelmaps[i]->identifier != NULL
            && pThe_pm->identifier != NULL
            && c2_strcmp(pStorage_space->pixelmaps[i]->identifier, pThe_pm->identifier) == 0) {
            C2V(gAddedPixelmap) = pStorage_space->pixelmaps[i];
            return eStorage_duplicate;
        }
    }
    pStorage_space->pixelmaps[pStorage_space->pixelmaps_count] = (br_pixelmap*)pThe_pm;
    pStorage_space->pixelmaps_count++;
    return eStorage_allocated;
}
C2_HOOK_FUNCTION(0x00501020, AddPixelmapToStorage)

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

tAdd_to_storage_result (C2_HOOK_FASTCALL * LoadSoundInStorage_original)(tBrender_storage* pStorage_space, int pSound_id);
tAdd_to_storage_result C2_HOOK_FASTCALL LoadSoundInStorage(tBrender_storage* pStorage_space, int pSound_id) {

#if 0//defined(C2_HOOKS_ENABLED)
    return LoadSoundInStorage_original(pStorage_space, pSound_id);
#else

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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00501930, LoadSoundInStorage, LoadSoundInStorage_original)

int C2_HOOK_FASTCALL IsValidFile(const char* path) {
    struct c2_stat stat;

    return c2_stat32(path, &stat) == 0;
}
C2_HOOK_FUNCTION(0x00486c00, IsValidFile)

int C2_HOOK_FASTCALL GetLastModificationTime(const char* path) {
    struct c2_stat stat;
    int res;

    res = c2_stat32(path, &stat);
    if (res == -1) {
        return 0;
    }
    return stat.st_mtime;
}
C2_HOOK_FUNCTION(0x00486be0, GetLastModificationTime)

br_pixelmap* C2_HOOK_FASTCALL Read_DEFAULT_ACT(int flags, int *errorCode) {
    int i;
    tPath_name path;
    br_uint_8 defaultActBuffer[0x300];
    FILE* f;
    br_pixelmap* pm;
    br_uint_8* src;
    br_uint_8* dst;
    c2_sprintf(path, "%s%s%s%s%s", path, C2V(gDir_separator), "PALETTE", C2V(gDir_separator), "DEFAULT.ACT");
    f = DRfopen(path, "rb");
    if (f == NULL) {
        *errorCode = 5;
        return NULL;
    }
    if (fread(defaultActBuffer, 0x300, 1, (FILE*)f) == 0) {
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
C2_HOOK_FUNCTION(0x004862b0, FindBestMatch_ShadeTable)

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
C2_HOOK_FUNCTION(0x00485590, CreatePalettePixelmapFromRGBChannels)

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
    scanlineBuffer = c2_malloc(TIFFScanlineSize(tif));
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
    c2_free(scanlineBuffer);
    TIFFClose(tif);
    if (y < height) {
        BrPixelmapFree(pm);
        *errorCode = 2;
        return NULL;
    }
    return pm;
}

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
        scanlineBuffer = c2_malloc(TIFFScanlineSize(tif));
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
        c2_free(scanlineBuffer);
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
C2_HOOK_FUNCTION(0x00485fe0, LoadTiffTexture_WithShadeTable)

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
    scanlineBuffer = c2_malloc(TIFFScanlineSize(tif));
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
    c2_free(scanlineBuffer);
    TIFFClose(tif);
    if (y < height) {
        BrPixelmapFree(pm);
        *errorCode = 4;
        return NULL;
    }
    return pm;
}
C2_HOOK_FUNCTION(0x004864a0, LoadTiffTexture_16BitRGB)

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

    c2_sprintf(tifPath, "%s%s%s%s%s%s", texturePathDir, C2V(gDir_separator), useTiffx ? "TIFFX" : "TIFFRGB", C2V(gDir_separator), textureName, ".TIF");
    usePix16 = flags & kLoadTextureFlags_16bbp;
    c2_sprintf(pixPath, "%s%s%s%s%s%s", texturePathDir, C2V(gDir_separator), usePix16 ? "PIX16" : "PIX8", C2V(gDir_separator), textureName, ".PIX");
    tifPathIsLink = ResolveTexturePathLink(tifPath, tifPath);
    pixPathIsLink = ResolveTexturePathLink(pixPathTarget, pixPath);
    pixExists = IsValidFile(pixPathIsLink ? pixPathTarget : pixPath);
    tifExists = IsValidFile(tifPath);

    if (useTiffx && pixExists) {
        c2_sprintf(trgbPath, "%s%s%s%s%s%s", texturePathDir, C2V(gDir_separator), "TIFFRGB", C2V(gDir_separator), textureName, ".TIF");
        ResolveTexturePathLink(trgbPath, trgbPath);
        if (IsValidFile(trgbPath)) {
            if (GetLastModificationTime(pixPathIsLink ? pixPathTarget : pixPath) < GetLastModificationTime(trgbPath)) {
                return NULL;
            }
        }
    }
    if (!tifExists) {
        if (pixExists && (usePix16 || useTiffx)) {
            prevFilesystem = BrFilesystemSet(&C2V(zlibFilesystem));
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
            prevFilesystem = BrFilesystemSet(&C2V(zlibFilesystem));
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
            realPalette = Read_DEFAULT_ACT(flags, errorCode);
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
            c2_sprintf(path, "%s%s%s", texturePathDir, C2V(gDir_separator), usePix16 ? "PIX16" : "PIX8");
            PDmkdir(path);
            if (tifPathIsLink && !pixExists) {
                if (FindLastOccurrenceOfString_CaseInsensitive(&indexSuffix, tifPath, c2_strlen(tifPath), ".TIF")) {
                    if (FindLastOccurrenceOfString_CaseInsensitive(&indexLastSeparator, tifPath, indexSuffix, C2V(gDir_separator))) {
                        int texRootIndex;
                        if (FindLastOccurrenceOfString_CaseInsensitive(&texRootIndex, tifPath, indexLastSeparator, C2V(gDir_separator))) {
                            posDir = c2_strlen(C2V(gDir_separator)) + indexLastSeparator;
                            c2_sprintf(buf1, "%.*s%s%s", texRootIndex, tifPath, C2V(gDir_separator), C2V(gDir_separator), usePix16 ? "PIX16" : "PIX8");
                            c2_sprintf(pixPathTarget, "%s%s%.*s%s", buf1, C2V(gDir_separator), indexSuffix - posDir, tifPath + posDir, ".PIX");
                        }
                    }
                }
                PDmkdir(buf1);
                CreatePathLink(pixPathTarget, pixPath);
                pixPathIsLink = 1;
            }
        }
        if ((flags & kLoadTextureFlags_SaveTextureCompressed) == 0) {
            prevFilesystem = BrFilesystemSet(&C2V(zlibFilesystem));
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
C2_HOOK_FUNCTION(0x00485750, LoadTiffTexture_Ex2)

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
C2_HOOK_FUNCTION(0x004856c0, LoadTiffTexture_Ex)

br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode) {
    br_pixelmap* textures[1000];
    int nb;

    if (C2V(gDisableTiffConversion)) {
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
C2_HOOK_FUNCTION(0x0048eb80, DRLdImg)

int C2_HOOK_FASTCALL DRPixelmapLoadMany(const char* texturePathNoExt, br_pixelmap** pixelmaps, size_t capacity) {
    tPath_name texturePath;
    tPath_name texturePathDir;
    tPath_name texturePathStem;
    int errorCode;

    c2_strcpy(texturePath, texturePathNoExt);
    c2_strcat(texturePath, ".TIF");
    SepDirAndFilename(texturePath, texturePathDir, texturePathStem);
    pixelmaps[0] = DRLdImg(texturePathDir, texturePathStem, C2V(gRender_palette), C2V(gPixelFlags), &errorCode);
    return (pixelmaps[0] != NULL && errorCode == 0) ? 1 : 0;
}
C2_HOOK_FUNCTION(0x00514570, DRPixelmapLoadMany)

int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path) {
    int i;
    int nbLoaded;
    int result;
    tPath_name path_dirname;
    tPath_name path_stem;
    tAdd_to_storage_result addResult;
    br_pixelmap* pixelmaps[500];

    result = 0;
    if (C2V(gDisableTiffConversion)) {
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
            if (C2V(gDisallowDuplicates)) {
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
C2_HOOK_FUNCTION(0x005024f0, AddPixelmaps)

void C2_HOOK_FASTCALL TwatPIX16(const char* pPath) {

}

void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path) {
    tPath_name pathCopy;
    tPath_name tempPath;
    tName_list list;
    size_t i;

    C2_HOOK_BUG_ON(sizeof(tBrender_storage) != 68);

    TwatPIX16(path);
    list.size = 0;
    c2_strcpy(pathCopy, path);
    C2V(gStorageForCallbacks) = pStorage_space;
    if (C2V(gDisableTiffConversion)) {
        PFForEveryFile2(pathCopy, (tEnumPathCallback)AddTextureFileStemToList, &list);
    }
    if (!C2V(gDisableTiffConversion)) {
        PathCat(tempPath, pathCopy, "TIFFX");
        PFForEveryFile2(tempPath, (tEnumPathCallback)AddTexturePixTifFileStemToList, &list);
    }
    PathCat(tempPath, pathCopy, "PIX8");
    PFForEveryFile2(tempPath, (tEnumPathCallback)AddTextureFileStemToList, &list);
    if (!C2V(gDisableTiffConversion)) {
        PathCat(tempPath, pathCopy, "TIFFRGB");
        PFForEveryFile2(tempPath, (tEnumPathCallback)AddTextureFileStemToList, &list);
    }
    PathCat(tempPath, pathCopy, "PIX16");
    PFForEveryFile2(tempPath, (tEnumPathCallback)AddTextureFileStemToList, &list);
    for (i = 0; i < list.size; i++) {
        PathCat(tempPath, path, list.items[i]);
        AddPixelmaps(C2V(gStorageForCallbacks), tempPath);
    }
}
C2_HOOK_FUNCTION(0x005028f0, LoadAllImagesInDirectory)

int C2_HOOK_FASTCALL LoadTiffTextureCB(const char* filePath, tLoadDirectoryStructureCBData* data) {
    size_t filePathLength;
    tPath_name textureName;
    tLoadDirectoryStructureCBResult* itemResult;
    br_pixelmap* texture;

    filePathLength = c2_strlen(filePath);
    if (DRstrcmpi(filePath + filePathLength - 4, ".TIF") != 0) {
        return 0;
    }
    c2_sprintf(textureName, "%.*s", filePathLength - 4, filePath);
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
C2_HOOK_FUNCTION(0x00486860, LoadTiffTextureCB)

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
        c2_sprintf(pathBuffer, "%s%s%s", directory, C2V(gDir_separator), "TIFFX");
        data.isTiffx = 0x1;
        PDEnumPath(pathBuffer, (tEnumPathCallback)LoadTiffTextureCB, &data);
    }
    if (*errorCode != 0) {
        return;
    }
    c2_sprintf(pathBuffer, "%s%s%s", directory, C2V(gDir_separator), "TIFFRGB");
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
C2_HOOK_FUNCTION(0x00486760, LoadAllTiffTexturesInDirectory)

void UseNativeDirSeparator(char* nativePath, const char* path) {
    size_t i;
    size_t len;
    char c;

    len = c2_strlen(path);
    for (i = 0; i < len; i++) {
        c = path[i];
        if (c == '\\') {
            c = *C2V(gDir_separator);
        }
        nativePath[i] = c;
    }
    nativePath[len] = '\0';
}

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
    if (c2_strstr(linkPath, ".TIF") != NULL) {
        UseNativeDirSeparator(nativeLinkPath, linkPath);
    } else if (c2_strstr(linkPath, ".PIX") != NULL) {
        UseNativeDirSeparator(nativeLinkPath, linkPath);
    } else {
        return 0;
    }
    PathCat(linkPath, C2V(gApplication_path), nativeLinkPath);
    c2_strcpy(realPath, linkPath);
    return 1;
}
C2_HOOK_FUNCTION(0x004869e0, ResolveTexturePathLink)

int C2_HOOK_FASTCALL CreatePathLink(const char* targetPath, const char* linkPath) {
    tPath_name buffer;
    tPath_name cleanedTargetRelPath;
    const char* targetRelPath;
    char c;
    int pos;
    FILE *f;

    c2_strcpy(buffer, targetPath);
    targetRelPath = c2_strstr(buffer, "DATA") + c2_strlen("DATA") + 1;
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
    c2_fputs(cleanedTargetRelPath, f);
    fclose(f);
    return 1;

}
C2_HOOK_FUNCTION(0x00486b20, CreatePathLink)

int C2_HOOK_FASTCALL DRstrcmpi(const char* str1, const char* str2) {
    return c2_strcasecmp(str1, str2);
}
C2_HOOK_FUNCTION(0x0047d850, DRstrcmpi)

int C2_HOOK_FASTCALL DRmemicmp(const char* str1, const char* str2, size_t count) {
    return c2_memicmp(str1, str2, count);
}
C2_HOOK_FUNCTION(0x0047d860, DRmemicmp)

int C2_HOOK_FASTCALL FindLastOccurrenceOfString_CaseInsensitive(int* offset, const char* haystack, size_t haystackLen, const char* needle) {
    size_t needleLen;
    size_t haystackPos;

    needleLen = c2_strlen(needle);
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
C2_HOOK_FUNCTION(0x00486240, FindLastOccurrenceOfString_CaseInsensitive)

tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st) {
    int i;

    if (pStorage_space->shade_tables_count >= pStorage_space->max_shade_tables) {
        return eStorage_not_enough_room;
    }

    for (i = 0; i < pStorage_space->shade_tables_count; i++) {
        if (pStorage_space->shade_tables[i]->identifier
            && pThe_st->identifier
            && c2_strcmp(pStorage_space->shade_tables[i]->identifier, pThe_st->identifier) == 0) {
            return eStorage_duplicate;
        }
    }
    pStorage_space->shade_tables[pStorage_space->shade_tables_count] = pThe_st;
    pStorage_space->shade_tables_count++;
    return eStorage_allocated;
}

void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath) {
    br_pixelmap* temp_array[50];
    int total;
    int i;
    tBrender_storage* storage_space;
    char s[256];

    Uppercaseificate(s, pPath);
    if (c2_strstr(s, ".TAB") == NULL) {
        return;
    }
    storage_space = C2V(gStorageForCallbacks);
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
            if (C2V(gDisallowDuplicates)) {
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
C2_HOOK_FUNCTION(0x00502b80, LoadIfItsAShadeTable)

void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath) {

    C2V(gStorageForCallbacks) = pStorage;
    PFForEveryFile(pPath, LoadIfItsAShadeTable);
}
C2_HOOK_FUNCTION(0x00502b60, LoadAllShadeTablesInDirectory)

void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (c2_strstr(s, ".PIX") != NULL) {
        AddPixelmaps(C2V(gStorageForCallbacks), pPath);
    }
}
C2_HOOK_FUNCTION(0x005024b0, LoadIfItsAPixelmap)

void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath) {

    C2V(gStorageForCallbacks) = pStorage;
    PFForEveryFile(pPath, LoadIfItsAPixelmap);
}
C2_HOOK_FUNCTION(0x00502490, LoadAllPixelmapsInDirectory)

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
            if (C2V(gDisallowDuplicates)) {
                FatalError(kFatalError_DuplicatePixelmap_S, map->identifier);
            }
            BrPixelmapFree(map);
            return C2V(gAddedPixelmap);
        case eStorage_allocated:
            BrMapAdd(map);
            return map;
        default:
            return NULL;
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x00501560, LoadSinglePixelmap)

void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath) {
    tRendererShadingType shading;
    char s[256];

    shading = C2V(gMaterial_shading_for_callback);
    if (C2V(gMaterial_shading_for_callback) == kRendererShadingType_Undefined) {
        shading = kRendererShadingType_Default;
    }
    Uppercaseificate(s, pPath);
    if (c2_strstr(s, ".MAT") != NULL) {
        AddMaterials(C2V(gStorageForCallbacks), pPath, shading);
    }
}
C2_HOOK_FUNCTION(0x00502a70, LoadIfItsAMaterial)

void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    C2V(gMaterial_shading_for_callback) = pShading;
    C2V(gStorageForCallbacks) = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    C2V(gMaterial_shading_for_callback) = kRendererShadingType_Undefined;
}
C2_HOOK_FUNCTION(0x00502ad0, LoadAllMaterialsInDirectory)

tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat) {
    int i;

    C2V(gDuplicate_material) = NULL;
    if (pStorage_space->materials_count >= pStorage_space->max_materials) {
        return eStorage_not_enough_room;
    }
    for (i = 0; i < pStorage_space->materials_count; i++) {
        if (pStorage_space->materials[i]->identifier
            && pThe_mat->identifier
            && !c2_strcmp(pStorage_space->materials[i]->identifier, pThe_mat->identifier)) {
            C2V(gDuplicate_material) = pStorage_space->materials[i];
            return eStorage_duplicate;
        }
    }
    pStorage_space->materialProps[pStorage_space->materials_count] = 0; /* FIXME */
    pStorage_space->materials[pStorage_space->materials_count] = pThe_mat;
    pStorage_space->materials_count++;
    return eStorage_allocated;
}
C2_HOOK_FUNCTION(0x00501190, AddMaterialToStorage)

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
            if (C2V(gDisallowDuplicates)) {
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
C2_HOOK_FUNCTION(0x00502060, AddMaterials)

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
            if (C2V(gDisallowDuplicates)) {
                FatalError(kFatalError_DuplicateMaterial_S, temp->identifier);
            }
            BrMaterialFree(temp);
            return C2V(gDuplicate_material);

        case eStorage_allocated:
            BrMaterialAdd(temp);
            return temp;
    }

    return NULL;
}
C2_HOOK_FUNCTION(0x005017e0, LoadSingleMaterial)

void C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage *pStorage, FILE* pFile, tRendererShadingType pShading) {
    char s1[256];
    char s2[256];

    PossibleService();
    GetALineAndDontArgue(pFile, s1);
    PathCat(s2, C2V(gApplication_path), "MATERIAL");
    PathCat(s2, s2, c2_strtok(s1, "\t ,/"));
    AddMaterials(pStorage, s2, pShading);
}
C2_HOOK_FUNCTION(0x00501fe0, LoadSomeMaterials)

br_pixelmap* C2_HOOK_FASTCALL LoadShadeTable(const char* pName) {
    tPath_name the_path;

    PathCat(the_path, C2V(gApplication_path), "SHADETAB");
    PathCat(the_path, the_path, pName);
    return BrPixelmapLoad(the_path);
}
C2_HOOK_FUNCTION(0x0048ef40, LoadShadeTable)

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
        if (C2V(gDisallowDuplicates)) {
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
C2_HOOK_FUNCTION(0x005016a0, LoadSingleShadeTable)

tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod) {
    int i;

    C2V(gDuplicate_model) = NULL;
    if (pStorage_space->materials_count >= pStorage_space->max_models) {
        return eStorage_not_enough_room;
    }
    if (pStorage_space->flags & 0x1) { /* FIXME: add enum (0x1 -> eStorage_space_flags_No_duplicates*/
        for (i = 0; i < pStorage_space->models_count; i++) {
            if (pStorage_space->models[i] != NULL
                && pStorage_space->models[i]->identifier != NULL
                && pThe_mod->identifier != NULL
                && c2_strcmp(pStorage_space->models[i]->identifier, pThe_mod->identifier) == 0) {
                C2V(gDuplicate_model) = pStorage_space->models[i];
                return eStorage_duplicate;
            }
        }
    }
    pStorage_space->models[pStorage_space->models_count] = pThe_mod;
    pStorage_space->models_count++;
    return eStorage_allocated;
}
C2_HOOK_FUNCTION(0x00501260, AddModelToStorage)

int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath) {
    int i;
    int new_ones;
    int total;
    br_model* temp_array[2000];

    new_ones = 0;
    total = BrModelLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    DisableVertexColours(temp_array, total);
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
            if (C2V(gDisallowDuplicates)) {
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
C2_HOOK_FUNCTION(0x00501e40, AddModels)

void C2_HOOK_FASTCALL LoadIfItsAMode(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (c2_strstr(s, ".DAT") != NULL) {
        AddModels(C2V(gStorageForCallbacks), pPath);
    }
}
C2_HOOK_FUNCTION(0x00502b20, LoadIfItsAMode)

void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath) {

    C2V(gStorageForCallbacks) = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMode);
}
C2_HOOK_FUNCTION(0x00502b00, LoadAllModelsInDirectory)

void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    LoadAllShadeTablesInDirectory(pStorage, pPath);
    LoadAllPixelmapsInDirectory(pStorage, pPath);
    LoadAllImagesInDirectory(pStorage, pPath);
    LoadAllMaterialsInDirectory(pStorage, pPath, pShading);
    LoadAllModelsInDirectory(pStorage, pPath);
}
C2_HOOK_FUNCTION(0x00502cf0, LoadAllStuffInDirectory)

void C2_HOOK_FASTCALL AllowDuplicates(void) {

    C2V(gDisallowDuplicates) = 0;
}
C2_HOOK_FUNCTION(0x00502d70, AllowDuplicates);

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
C2_HOOK_FUNCTION(0x00502d80, SwapPixelmapInStorage)

void (C2_HOOK_FASTCALL * SetCarStorageTexturingLevel_original)(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld);
void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld) {

#if defined(C2_HOOKS_ENABLED)
    SetCarStorageTexturingLevel_original(pStorage, pNew, pOld);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447350, SetCarStorageTexturingLevel, SetCarStorageTexturingLevel_original)

void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon) {
    C2V(gSight_distance_squared) = pYon * 1.02f * pYon * 1.02f;
}
C2_HOOK_FUNCTION(0x00474880, SetSightDistance)

void (C2_HOOK_FASTCALL * AddFunkotronics_original)(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas);
void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas) {

#if defined(C2_HOOKS_ENABLED)
    AddFunkotronics_original(pF, pOwner, pRef_offset, pCar_crush_datas);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00474ac0, AddFunkotronics, AddFunkotronics_original)

void (C2_HOOK_FASTCALL * DisposeFunkotronics_original)(int pOwner);
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {

#if defined(C2_HOOKS_ENABLED)
    DisposeFunkotronics_original(pOwner);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00474950, DisposeFunkotronics, DisposeFunkotronics_original)

void (C2_HOOK_FASTCALL * AddGroovidelics_original)(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent);
void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent) {

#if defined(C2_HOOKS_ENABLED)
    AddGroovidelics_original(pF, pOwner, pParent_actor, pRef_offset, pAllowed_to_be_absent);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00476470, AddGroovidelics, AddGroovidelics_original);

void (C2_HOOK_FASTCALL * DisposeGroovidelics_original)(int pOwner);
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {

#if defined(C2_HOOKS_ENABLED)
    DisposeGroovidelics_original(pOwner);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00476430, DisposeGroovidelics, DisposeGroovidelics_original)

tGroovidelic_spec* C2_HOOK_FASTCALL FindGroovidelicForActor(br_actor* pActor) {
    int i;

    for (i = 0; i < C2V(gGroovidelics_array_size); i++) {
        tGroovidelic_spec* the_groove;

        the_groove = &C2V(gGroovidelics_array)[i];
        if (the_groove->actor == pActor) {
            return the_groove;
        }
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x0047b360, FindGroovidelicForActor)

tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void) {

    return C2V(gDefault_water_spec_vol_real);
}
C2_HOOK_FUNCTION(0x004ff110, GetDefaultSpecialVolumeForWater)

void C2_HOOK_FASTCALL LoadGlobalLighting(FILE* pF) {

    /* //////////// GLOBAL LIGHTING DATA /////////// */

    /* RGB for main directional light-source */
    GetThreeInts(pF, &C2V(gLighting_data).directional.red, &C2V(gLighting_data).directional.green, &C2V(gLighting_data).directional.blue);
    /* Ambient/Diffuse light to be used when plaything ambient says 0 */
    GetPairOfScalars(pF, &C2V(gLighting_data).ambient_0, &C2V(gLighting_data).diffuse_0);
    /* Ambient/Diffuse light to be used when plaything ambient says 1 */
    GetPairOfScalars(pF, &C2V(gLighting_data).ambient_1, &C2V(gLighting_data).diffuse_1);
    /* Ambient/Diffuse light to be used when plaything ambient says anything else */
    GetPairOfScalars(pF, &C2V(gLighting_data).ambient_else, &C2V(gLighting_data).diffuse_else);
}
C2_HOOK_FUNCTION(0x00486dc0, LoadGlobalLighting)

void C2_HOOK_FASTCALL FUN_004f0430(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tUnknown_0x006a7fc8) != 0xc);
    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gUnknown_0x006a7fc8)) != 25);

    for (i = 0; i < REC2_ASIZE(C2V(gUnknown_0x006a7fc8)); i++) {
        C2V(gUnknown_0x006a7fc8)[i].field_0x0 = 0;
    }
}

void (C2_HOOK_FASTCALL * LoadSmashableTrackEnvironment_original)(FILE* pF, const char* pPath);
void C2_HOOK_FASTCALL LoadSmashableTrackEnvironment(FILE* pF, const char* pPath) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadSmashableTrackEnvironment_original(pF, pPath);
#else
    int i;
    int j;
    char s[256];
    char s2[256];

    s2[0] = '\0';
    C2V(gCount_smashable_noncars) = 0;
    C2V(gCount_smashable_noncar_shrapnel_actors) = 0;
    C2V(gSmashable_noncars) = BrMemAllocate(100 * sizeof(tSmashable_environment_name), kMem_smashable_env_info);
    C2_HOOK_BUG_ON(100 * sizeof(tSmashable_environment_name) != 3200);

    /* Number of smash specs */
    C2V(gCount_track_smashable_environment_specs) = GetAnInt(pF);

    C2V(gCapacity_track_smashables) = 5 * (C2V(gCount_track_smashable_environment_specs) + 30);
    C2V(gCount_track_smashable_environment_specs_2) = C2V(gCount_track_smashable_environment_specs);
    C2V(gTrack_smashable_environment_specs) = BrMemAllocate(C2V(gCapacity_track_smashables) * sizeof(tSmashable_item_spec), kMem_smashable_env_info);
    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 736);

    C2V(gSmashable_track_environment_path) = pPath;

    c2_memset(C2V(gSmashable_track_indexable_triggers), 0, sizeof(C2V(gSmashable_track_indexable_triggers)));
    C2_HOOK_BUG_ON(sizeof(C2V(gSmashable_track_indexable_triggers)) != 400);

    for (i = 0; i < C2V(gCount_track_smashable_environment_specs); i++) {
        tSmashable_item_spec* spec = &C2V(gTrack_smashable_environment_specs)[i];

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
            if (c2_strstr(s, ".DAT") != NULL || c2_strstr(s, ".dat") != NULL || c2_strstr(s, ".ACT") != NULL || c2_strstr(s, ".act") != NULL) {
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
            C2V(gSmashable_track_indexable_triggers)[spec->trigger_object.number.field_0x0] = spec;
            break;
        }

        /* Mode */
        spec->mode = GetALineAndInterpretCommand(pF, C2V(gSmashable_item_mode_names), REC2_ASIZE(C2V(gSmashable_item_mode_names)));
        switch (spec->mode) {
        case kSmashableMode_Decal:
        case kSmashableMode_TextureChange:
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, mode_data, 0x14);

            spec->mode_data.texture_change.levels = NULL;
            c2_strcpy(spec->mode_data.texture_change.undefined_0x0, s2);

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, field_0x40, 0x40);

            spec->mode_data.texture_change.field_0x40 = -1;
            spec->mode_data.texture_change.trigger = spec->trigger_object;
            spec->mode_data.texture_change.field_0x4c = 0;
            spec->mode_data.texture_change.field_0x40 = -1;
            spec->mode_data.texture_change.field_0x6c = 0;
            spec->mode_data.texture_change.field_0x70 = 0;
            spec->mode_data.texture_change.field_0x74 = 0;
            spec->mode_data.texture_change.field_0x78 = 0;
            spec->mode_data.texture_change.field_0x54 = 0;
            spec->mode_data.texture_change.field_0x58 = 0;
            spec->mode_data.texture_change.field_0x5c = 0;
            spec->mode_data.texture_change.field_0x60 = 1.f;
            spec->mode_data.texture_change.field_0x64 = 0;
            spec->mode_data.texture_change.field_0x68 = 0;
            LoadSmashableLevels(pF,
                &spec->mode_data.texture_change.levels,
                &spec->mode_data.texture_change.count_levels,
                spec->mode == kSmashableMode_TextureChange,
                &C2V(gTrack_storage_space));
            break;
        default:
            /* Removal threshold */
            spec->mode_data.shrapnel.removal_threshold = GetAScalar(pF);

            /* number of possible sounds */
            spec->mode_data.shrapnel.count_sounds = GetAnInt(pF);

            for (j = 0; j < spec->mode_data.shrapnel.count_sounds; j++) {
                /* sound id */
                spec->mode_data.shrapnel.sounds[j] = LoadSoundInStorage(&C2V(gTrack_storage_space), GetAnInt(pF));
            }

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, mode_data, 0x14);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_shrapnel, count_shrapnel, 0x14);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_shrapnel, shrapnel, 0x18);

            ReadShrapnel(pF, spec->mode_data.shrapnel.shrapnel, &spec->mode_data.shrapnel.count_shrapnel);
            ReadExplosionAnimation(pF, &spec->mode_data.shrapnel.explosion_animation);
            ReadSlick(pF, &spec->mode_data.shrapnel.slick);
            ReadNonCarCuboidActivation(pF, &spec->mode_data.shrapnel.activations);
            ReadShrapnelSideEffects(pF, &spec->mode_data.shrapnel.side_effects);

            /* Extensions flags */
            spec->mode_data.shrapnel.extension_flags = GetAnInt(pF);
            if (spec->mode_data.shrapnel.extension_flags & 0x1) {
                spec->mode_data.shrapnel.extension_arg = GetAnInt(pF);
            }
            /* Room turn on code */
            spec->mode_data.shrapnel.room_turn_on_code = GetAnInt(pF);
            LoadAward(pF, &spec->mode_data.shrapnel.award);
            /* run-time variable changes */
            spec->mode_data.shrapnel.count_runtime_variable_changes = GetAnInt(pF);
            for (j = 0; j < spec->mode_data.shrapnel.count_runtime_variable_changes; j++) {
                int v1, v2;

                GetPairOfInts(pF, &v1, &v2);
                spec->mode_data.shrapnel.runtime_variable_changes[j].field_0x0 = v2;
                spec->mode_data.shrapnel.runtime_variable_changes[j].field_0x2 = v1;
            }
            break;
        }
        if (spec->mode == kSmashableMode_ReplaceModel) {
            /* new model */
            GetAString(pF, s2);
            spec->replace_model = BrModelFind(s2);
            if (spec->replace_model == NULL) {
                FatalError(kFatalError_CannotFindSmashActorModel_S, s2);
            }
            /* %chance fire */
            spec->replace_modelchance_fire = GetAnInt(pF);
            if (spec->replace_modelchance_fire != 0) {
                spec->replace_model_2_int = GetAnInt(pF);
                GetPairOfInts(pF, &spec->replace_model_3_int, &spec->replace_model_4_int);
            }
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
    for (i = C2V(gCount_track_smashable_environment_specs); i < C2V(gCapacity_track_smashables); i++) {
        tSmashable_item_spec* spec = &C2V(gTrack_smashable_environment_specs)[i];

        spec->trigger_type = -1;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f0450, LoadSmashableTrackEnvironment, LoadSmashableTrackEnvironment_original)

int (C2_HOOK_FASTCALL * MaterialIsSmashableTrigger_original)(br_material *pMaterial);
int C2_HOOK_FASTCALL MaterialIsSmashableTrigger(br_material *pMaterial) {
    int i;

    for (i = 0; i < C2V(gCount_track_smashable_environment_specs); i++) {
        tSmashable_item_spec *spec = &C2V(gTrack_smashable_environment_specs)[i];

        if ((spec->trigger_type == kSmashableTrigger_Material && spec->trigger_object.material == pMaterial)
                && ((spec->mode == kSmashableMode_TextureChange && spec->mode_data.texture_change.count_levels > 0)
                || spec->mode == kSmashableMode_Remove
                || spec->mode == kSmashableMode_ReplaceModel)) {
#if defined(C2_HOOKS_ENABLED)
            C2_HOOK_ASSERT(MaterialIsSmashableTrigger_original(pMaterial) == 1);
#endif
            return 1;
        }
    }
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_ASSERT(MaterialIsSmashableTrigger_original(pMaterial) == 0);
#endif
    return 0;
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f5470, MaterialIsSmashableTrigger, MaterialIsSmashableTrigger_original)

void (C2_HOOK_FASTCALL * DisposeSmashableTrackEnvironment_original)(void);
void C2_HOOK_FASTCALL DisposeSmashableTrackEnvironment(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposeSmashableTrackEnvironment_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f0960, DisposeSmashableTrackEnvironment, DisposeSmashableTrackEnvironment_original)

void (C2_HOOK_FASTCALL * ResetSmashing_original)(void);
void C2_HOOK_FASTCALL ResetSmashing(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetSmashing_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f5750, ResetSmashing, ResetSmashing_original)

void C2_HOOK_FASTCALL FreeSmashableNonCarNames(void) {

    BrMemFree(C2V(gSmashable_noncars));
}
C2_HOOK_FUNCTION(0x004f0940, FreeSmashableNonCarNames)

void (C2_HOOK_FASTCALL * LoadTrackSpecialVolumes_original)(FILE* pF);
void C2_HOOK_FASTCALL LoadTrackSpecialVolumes(FILE* pF) {

#if defined(C2_HOOKS_ENABLED)
    LoadTrackSpecialVolumes_original(pF);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ffd80, LoadTrackSpecialVolumes, LoadTrackSpecialVolumes_original)

void (C2_HOOK_FASTCALL * LoadTrackSoundGenerators_original)(tTrack_spec* pTrack_spec, FILE* pF);
void C2_HOOK_FASTCALL LoadTrackSoundGenerators(tTrack_spec* pTrack_spec, FILE* pF) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadTrackSoundGenerators_original(pTrack_spec, pF);
#else
    int i;

    C2V(gProgram_state).count_track_sound_generators = GetAnInt(pF);
    if (C2V(gProgram_state).count_track_sound_generators != 0) {
        C2_HOOK_BUG_ON(sizeof(tTrackSoundGenerator) != 0x7c);
        C2V(gProgram_state).track_sound_generators = BrMemAllocate(C2V(gProgram_state).count_track_sound_generators * sizeof(tTrackSoundGenerator), kMem_sound_generator);
    }
    for (i = 0; i < C2V(gProgram_state).count_track_sound_generators; i++) {
        tTrackSoundGenerator* generator;
        char name[32];

        generator = &C2V(gProgram_state).track_sound_generators[i];
        generator->type = GetALineAndInterpretCommand(pF, C2V(gSoundGeneratorTypeNames), REC2_ASIZE(C2V(gSoundGeneratorTypeNames)));
        if (generator->type == kSoundGeneratorType_noncar) {
            char identifier[4];
            br_model* model;

            GetAString(pF, name);
            c2_sprintf(identifier, "%c%02d", 'L', i);
            model = BrModelFind(name);
            if (model == NULL || model->identifier == NULL) {
                FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S, name);
            }
            c2_memcpy(model->identifier, identifier, sizeof(identifier));
        } else if (generator->type == kSoundGeneratorType_actor) {
            char identifier[4];
            br_model* model;

            GetAString(pF, name);
            c2_sprintf(identifier, "%c%02d", ')', i);
            model = BrModelFind(name);
            if (model == NULL || model->identifier == NULL) {
                FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S, name);
            }
            c2_memcpy(model->identifier, identifier, sizeof(identifier));
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004572f0, LoadTrackSoundGenerators, LoadTrackSoundGenerators_original)

void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM) {

    BrResFree(pM->faces);
    BrResFree(pM->vertices);
    pM->nfaces = 0;
    pM->nvertices = 0;
    pM->faces = NULL;
    pM->vertices = NULL;
}
C2_HOOK_FUNCTION(0x00502210, DodgyModelUpdate)

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
    str = c2_strtok(s, "\t ,");
    if (DRStricmp(str, "VERSION") != 0) {
        FatalError(kFatalError_FileMustStartWith_SS, pPath, "VERSION");
    }
    str = c2_strtok(NULL, "\t ,");
    count = c2_sscanf(str, "%d", &version);
    if (count == 0 || version != 1) {
        FatalError(kFatalError_CantCopeWithVersionFor_SS, str, pPath);
    }
    while (1) {
        tMaterial_exception* matexc;

        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,");
        if (DRStricmp(str, "end") == 0) {
            break;
        }
        C2_HOOK_BUG_ON(sizeof(tMaterial_exception) != 12);
        matexc = BrMemAllocate(sizeof(tMaterial_exception), kMem_exception);
        matexc->texture_name = BrMemAllocate(c2_strlen(str) + 1, kMem_misc_string);
        c2_strcpy(matexc->texture_name, str);
        matexc->flags = 0;
        while (1) {
            str = c2_strtok(NULL, "\t ,");
            if (str == NULL) {
                break;
            }
            if (!c2_isalnum(str[0])) {
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
        AddExceptionToList(&C2V(gMaterial_exceptions), matexc);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL LoadExceptionsFileForTrack(const char* pTrack_name) {
    tPath_name path;

    c2_sprintf(path, "%s%s%s%s",
               pTrack_name, C2V(gDir_separator),
               C2V(gRenderer_fixup_basename), C2V(gRenderer_fixup_extension));
    LoadExceptionsFile(path);
}

void (C2_HOOK_FASTCALL * LoadTrack_original)(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info);
void C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadTrack_original(pFile_name, pTrack_spec, pRace_info);
#else
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
    tMaterial_exception* material_exception;
    br_pixelmap* sky;
    tU16 sky_pixels_high;
    int killed_sky;
    int count_material_modifiers;
    int count_noncar_objects;

    PrintMemoryDump(0, "AT THE START OF LOAD TRACK");
    c2_strcpy(C2V(gCurrent_load_directory), "RACES");
    c2_strcpy(C2V(gCurrent_load_name), pFile_name);
    C2V(gCurrent_load_name)[c2_strlen(C2V(gCurrent_load_name)) - 4] = '\0';

    c2_strcpy(local_directory, C2V(gCurrent_load_directory));
    c2_strcpy(local_name, C2V(gCurrent_load_name));

    PathCat(C2V(gRace_path), C2V(gApplication_path), local_directory);
    c2_strcpy(local_race_path, C2V(gRace_path));

    LoadExceptionsFileForTrack(local_race_path);

    PathCat(C2V(gRace_path), C2V(gRace_path), local_name);
    twt = OpenPackFileAndSetTiffLoading(C2V(gRace_path));
    PathCat(race_lighting_path, C2V(gRace_path), "LIGHTING.TXT");
    PathCat(race_desc_path, C2V(gRace_path), pFile_name);
    f = DRfopen(race_desc_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }

    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    if (c2_strcmp(str, "VERSION") == 0) {
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &C2V(gTrack_version));
        if (C2V(gTrack_version) == 8) {
            C2V(gTrack_version) = 0;
        }
    } else {
        C2V(gTrack_version) = 0;
    }

    if (C2V(gTrack_version) > 0) {
        LoadGlobalLighting(f);
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
    c2_strcpy(short_track_name, pFile_name);
    short_track_name[4] = '\0';
    PathCat(race_desc_path, C2V(gApplication_path), "RACES");
    PathCat(race_desc_path, race_desc_path, short_track_name);
    twt2 = OpenPackFile(race_desc_path);
    LoadAllImagesInDirectory(&C2V(gTrack_storage_space), race_desc_path);
    TWT_Unmount(twt2);

    PossibleService();
    LoadAllImagesInDirectory(&C2V(gTrack_storage_space), C2V(gRace_path));

    PossibleService();
    LoadAllShadeTablesInDirectory(&C2V(gTrack_storage_space), C2V(gRace_path));

    PossibleService();
    LoadTrackMaterials(&C2V(gTrack_storage_space), C2V(gRace_path));

    PossibleService();
    LoadTrackModels(&C2V(gTrack_storage_space), C2V(gRace_path));

    for (i = 0; i < C2V(gTrack_storage_space).models_count; i++) {
        br_model* model;
        int* vertex_index_buffer;
        int (* face_index_buffer)[3];

        PossibleService();
        model = C2V(gTrack_storage_space).models[i];
        vertex_index_buffer = BrMemAllocate((model->nvertices + 1) * sizeof(int), BR_MEMORY_APPLICATION);
        face_index_buffer = BrMemAllocate(model->nfaces * 3 * sizeof(int), BR_MEMORY_APPLICATION);

        for (j = 1; j < model->nvertices; j++) {
            if (vertex_index_buffer[j] == 0) {
                int k;

                vertex_index_buffer[j] = j;
                for (k = j + 1; k <= model->nvertices; k++) {
                    br_vector3 d;

                    BrVector3Sub(&d, &model->vertices[j - 1].p, &model->vertices[k - 1].p);
                    if (BrVector3LengthSquared(&d) < 1e-8f) {
                        vertex_index_buffer[k] = j;
                    }
                }
            }
        }
        for (j = 0; j < model->nfaces; j++) {
            int k;

            for (k = 0; k < 3; k++) {
                face_index_buffer[j][k] = vertex_index_buffer[1 + model->faces[j].vertices[k]];
            }
        }
        if (model->nfaces > 1) {
            for (j = 0; j < model->nfaces - 1; j++) {
                int k;

                for (k = j + 1; k < model->nfaces; k++) {
                    int l;

                    for (l = 0; l < 3; l++) {
                        int m;
                        int s1a = face_index_buffer[j][l];
                        int s1b = face_index_buffer[j][(l + 1) % 3];

                        for (m = 0; m < 3; m++) {
                            int s2a = face_index_buffer[k][m];
                            int s2b = face_index_buffer[k][(m + 1) % 3];

                            if ((s1a == s2a && s1b == s2b) || (s1a == s2b && s1b == s2a)) {
                                br_vector3 d21, o31b, o31a, c2;
                                float d;
                                int s1c, s2c;

                                model->faces[k].flags |= 1 << m;
                                BrVector3Sub(&d21, &model->vertices[s2b - 1].p, &model->vertices[s2a - 1].p);
                                s1c = face_index_buffer[j][(l + 2) % 3];
                                s2c = face_index_buffer[k][(m + 2) % 3];
                                BrVector3Sub(&o31b, &model->vertices[s2c - 1].p, &model->vertices[s2a - 1].p);
                                BrVector3Sub(&o31a, &model->vertices[s1c - 1].p, &model->vertices[s2c - 1].p);
                                BrVector3Cross(&c2, &o31b, &d21);
                                d = BrVector3Dot(&o31a, &c2);
                                if (d < .0001f && (((model->faces[j].material == NULL || !(model->faces[j].material->flags & BR_MATF_TWO_SIDED)) && (model->faces[k].material == NULL || !(model->faces[k].material->flags & BR_MATF_TWO_SIDED))) || d >= -.0001f)) {
                                    model->faces[k].flags |= 1 << l;
                                }
                            }
                        }
                    }
                }
            }
        }
        BrMemFree(vertex_index_buffer);
        BrMemFree(face_index_buffer);
        model->flags |= BR_MODF_UPDATEABLE;
        BrModelUpdate(model, BR_MODU_FACES);
    }
    PrintMemoryDump(0, "JUST LOADED IN TEXTURES/MATS/MODELS FOR TRACK");

    PathCat(actor_path, C2V(gRace_path), local_name);
    c2_strcat(actor_path, ".ACT");
    pTrack_spec->the_actor = BrActorLoad(actor_path);
    PrintMemoryDump(0, "AFTER LOADING TRACK ACTORS");
    PossibleService();

    /* Smashable environment specs */
    LoadSmashableTrackEnvironment(f, C2V(gRace_path));
    PrintMemoryDump(0, "AFTER LOADING SMASHABLE ENVIRONMENT");

    /* Ped specs */
    LoadTrackPedestrians(f);
    PossibleService();

    ApplyPreviousTiffConversion();
    ExtractColumns(pTrack_spec);
    ApplyTopTiffConversion();

    AllocateTrackPedestrians();
    PrintMemoryDump(0, "JUST EXTRACTED COLUMNS AND LOADED IN PEDS");

    C2V(gTrack_actor) = pTrack_spec->the_actor;
    if (!C2V(gRendering_accessories) && C2V(gNet_mode) == eNet_mode_none) {
        int rstyle;

        rstyle = BR_RSTYLE_NONE;
        PossibleService();
        DRActorEnumRecurse(C2V(gTrack_actor), SetAccessoryRenderingCB, &rstyle);
    }
    BrActorAdd(C2V(gUniverse_actor), pTrack_spec->the_actor);

    /* Additional actor */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(c2_strtok(s, "\t ,/"), ".");
    c2_strcat(str, ".DAT");
    PathCat(C2V(gAdditional_model_path), C2V(gApplication_path), "MODELS");
    PathCat(C2V(gAdditional_model_path), C2V(gAdditional_model_path), str);
    PossibleService();
    C2V(gNumber_of_additional_models) = BrModelLoadMany(C2V(gAdditional_model_path), C2V(gAdditional_models), REC2_ASIZE(C2V(gAdditional_models)));
    for (i = 0; i < C2V(gNumber_of_additional_models); i++) {
        C2V(gAdditional_models)[i]->flags = BR_MODF_UPDATEABLE;
    }
    BrModelAddMany(C2V(gAdditional_models), C2V(gNumber_of_additional_models));
    PossibleService();

    str = c2_strtok(c2_strtok(s, "\t ,/"), ".");
    c2_strcat(str, ".ACT");
    PathCat(C2V(gAdditional_actor_path), C2V(gApplication_path), "ACTORS");
    PathCat(C2V(gAdditional_actor_path), C2V(gAdditional_actor_path), str);
    C2V(gAdditional_actors) = BrActorLoad(C2V(gAdditional_actor_path));
    if (C2V(gAdditional_actors) == NULL) {
        C2V(gAdditional_actors) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    }
    BrActorAdd(C2V(gUniverse_actor), C2V(gAdditional_actors));
    C2V(gLast_actor) = NULL;

    /* Name of sky texture pixelmap (or "none") */
    GetAString(f, s);
    if (!C2V(gAusterity_mode) && c2_strcmp(&s[c2_strlen(s) - 4], ".FLI") == 0) {
        void* flic_pixels;

        C2V(gTrack_flic_buffer) = NULL;
        if (!LoadFlicData(s, &C2V(gTrack_flic_buffer), &C2V(gTrack_flic_buffer_size))) {
            FatalError(kFatalError_CantFindFile_S, s);
        }
        StartFlic(s, -1, &C2V(gTrack_flic_descriptor), C2V(gTrack_flic_buffer_size), (tS8*)C2V(gTrack_flic_buffer), NULL, 0, 0, 0);
        flic_pixels = BrMemAllocate((C2V(gTrack_flic_descriptor).width + 3) & (~3) * C2V(gTrack_flic_descriptor).height, kMem_video_pixels);
        sky = DRPixelmapAllocate(C2V(gScreen)->type, C2V(gTrack_flic_descriptor).width, C2V(gTrack_flic_descriptor).height, flic_pixels, 0);
        BrMapAdd(sky);
        AssertFlicPixelmap(&C2V(gTrack_flic_descriptor), sky);
    } else {
        C2V(gTrack_flic_buffer) = NULL;
        sky = BrMapFind(s);
    }
    killed_sky = 0;
    if (C2V(gAusterity_mode) && sky != NULL) {
        for (i = 0; i < C2V(gTrack_storage_space).pixelmaps_count; i++) {
            if (C2V(gTrack_storage_space).pixelmaps[i] == sky) {
                BrMapRemove(C2V(gTrack_storage_space).pixelmaps[i]);
                BrPixelmapFree(C2V(gTrack_storage_space).pixelmaps[i]);
                C2V(gTrack_storage_space).pixelmaps[i] = C2V(gTrack_storage_space).pixelmaps[C2V(gTrack_storage_space).pixelmaps_count - 1];
                C2V(gTrack_storage_space).pixelmaps_count--;
                break;
            }
        }
        sky = NULL;
        killed_sky = 1;
    }
    C2V(gProgram_state).default_depth_effect.sky_texture = sky;
    if (sky != NULL) {
        sky_pixels_high = C2V(gProgram_state).default_depth_effect.sky_texture->height;
    } else {
        sky_pixels_high = 100;
    }
    PossibleService();

    /* Horizontal repetitions of sky texture */
    C2V(gSky_image_width) = BrDegreeToAngle(360.0 / GetAnInt(f));

    /* Vertical size of sky texture (degrees) */
    C2V(gSky_image_height) = BrDegreeToAngle(GetAScalar(f));

    /* Position of horizon (pixels below top) */
    C2V(gSky_image_underground) = C2V(gSky_image_height) * (sky_pixels_high - GetAnInt(f)) / sky_pixels_high;

    MungeForwardSky();
    MungeRearviewSky();
    PossibleService();

    /* Depth cue mode ("none", "dark" or "fog") */
    C2V(gProgram_state).default_depth_effect.type = GetALineAndInterpretCommand(f, C2V(gDepth_effect_names), REC2_ASIZE(C2V(gDepth_effect_names)));

    /* Degree of fog/darkness */
    GetPairOfInts(f,
        &C2V(gProgram_state).default_depth_effect.start,
        &C2V(gProgram_state).default_depth_effect.end);

    /* Depth cue colour (red, green, blue ) */
    GetThreeInts(f,
        &C2V(gProgram_state).default_depth_effect.colour.red,
        &C2V(gProgram_state).default_depth_effect.colour.green,
        &C2V(gProgram_state).default_depth_effect.colour.blue);
    C2V(gTrack_depth_colour_red) = C2V(gProgram_state).default_depth_effect.colour.red;
    C2V(gTrack_depth_colour_green) = C2V(gProgram_state).default_depth_effect.colour.green;
    C2V(gTrack_depth_colour_blue) = C2V(gProgram_state).default_depth_effect.colour.blue;
    ResetDepthEffect();

    if (killed_sky && C2V(gProgram_state).default_depth_effect.type != eDepth_effect_fog) {
        C2V(gProgram_state).default_depth_effect.type = eDepth_effect_fog;
        C2V(gProgram_state).default_depth_effect.start = 7;
        C2V(gProgram_state).default_depth_effect.end = 0;
        C2V(gProgram_state).default_depth_effect.colour.red = 0xf8;
        C2V(gProgram_state).default_depth_effect.colour.green = 0xf8;
        C2V(gProgram_state).default_depth_effect.colour.blue = 0xf8;
    }
    PossibleService();
    LoadTrackSpecialVolumes(f);
    LoadTrackSoundGenerators(pTrack_spec, f);

    /* REFLECTIVE WINDSCREEN SPECIFICATIONS */

    /* Material to use for default screens */
    GetAString(f, s);
    C2V(gProgram_state).standard_screen = BrMapFind(s);

    /* Material to use for default screens during darkness */
    GetAString(f, s);
    C2V(gProgram_state).standard_screen_dark = BrMapFind(s);

    /* Material to use for default screens during fog */
    GetAString(f, s);
    C2V(gProgram_state).standard_screen_fog = BrMapFind(s);

    /* (ignore) # areas with different screens */
    C2V(gProgram_state).special_screens_count = GetAnInt(f);
    if (C2V(gProgram_state).special_screens_count != 0) {
        C2V(gProgram_state).special_screens = BrMemAllocate(sizeof(tSpecial_screen) * C2V(gProgram_state).special_screens_count, kMem_special_screen);
        C2_HOOK_BUG_ON(sizeof(tSpecial_screen) != 20);
        for (i = 0; i < C2V(gProgram_state).special_screens_count; i++) {
            GetFourScalars(f,
                &C2V(gProgram_state).special_screens[i].min_x,
                &C2V(gProgram_state).special_screens[i].min_z,
                &C2V(gProgram_state).special_screens[i].max_x,
                &C2V(gProgram_state).special_screens[i].max_z);
            GetAString(f, s);
            C2V(gProgram_state).special_screens[i].material = BrMaterialFind(s);
        }
    }

    PossibleService();

    /* MAP DETAILS */

    /* Map pixelmap name */
    GetAString(f, s);
    pRace_info->map_image = BrMapFind(s);
    if (pRace_info->map_image == NULL) {
        c2_strtok(s, ".");
        pRace_info->map_image = BrMapFind(s);
        if (pRace_info->map_image == NULL) {
            Uppercaseificate(s, s);
            pRace_info->map_image = BrMapFind(s);
        }
    }
    BRPM_convert(pRace_info->map_image, C2V(gBack_screen)->type);
    PrintMemoryDump(0, "JUST LOADING SKY/SPEC VOLS/SCREENS/MAP");
    /* World->map transformation matrix */
    for (i = 0; i < 4; ++i) {
        GetThreeScalars(f,
            &pRace_info->map_transformation.m[i][0],
            &pRace_info->map_transformation.m[i][1],
            &pRace_info->map_transformation.m[i][2]);
    }
    if (C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width != 640 || C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height != 480) {
        pRace_info->map_transformation.m[3][0] -= 320.f;
        pRace_info->map_transformation.m[3][1] -= 240.f;
        BrMatrix34PostRotateX(&pRace_info->map_transformation, BrDegreeToAngle(90));
        BrMatrix34PostScale(&pRace_info->map_transformation,
            (float)C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width / (float)640,
            0.f,
            (float)C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height / (float)480);
        BrMatrix34PostRotateX(&pRace_info->map_transformation, BrDegreeToAngle(270));
        pRace_info->map_transformation.m[3][0] += (float)C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width / 2;
        pRace_info->map_transformation.m[3][1] += (float)C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height / 2;
    }

    /* ****** START OF FUNK AND GROOVE STUFF ****** */

    /* START OF FUNK */
    GetALineAndDontArgue(f, s);
    AddFunkotronics(f, -2, 30 * GROOVE_FUNK_MAX_PER_CAR, NULL);

    /* FIXME: What is magic 1080 / 30 * 30 * GROOVE_FUNK_MAX_PER_CAR ?*/
    C2_HOOK_BUG_ON(30 * GROOVE_FUNK_MAX_PER_CAR != 1080);

    /* START OF GROOVE */
    GetALineAndDontArgue(f, s);
    C2_HOOK_ASSERT(c2_strcmp(s, "START OF GROOVE") == 0);

    AddGroovidelics(f, -2, C2V(gUniverse_actor), 30 * GROOVE_FUNK_MAX_PER_CAR, 0);

    PossibleService();
    PrintMemoryDump(0, "JUST LOADING IN FUNKS AND GROOVES");

    for (i = 0; i < C2V(gTrack_storage_space).models_count; i++) {
        br_model* model;

        PossibleService();
        model = C2V(gTrack_storage_space).models[i];
        if (model != NULL && model->identifier != NULL) {
            for (j = 0; j < C2V(gTree_surgery_pass1_count); j++) {
                if (c2_strstr(model->identifier, C2V(gTree_surgery_pass1)->name) == model->identifier) {
                    break;
                }
            }
            if (j != C2V(gTree_surgery_pass1_count)) {
                for (j = 0; j < C2V(gTree_surgery_pass2_count); j++) {
                    if (c2_strcmp(model->identifier, C2V(gTree_surgery_pass2)->original) == 0) {
                        br_model* replacement;

                        c2_strcpy(s, C2V(gTree_surgery_pass2)->replacement);
                        replacement = LoadModel(s);
                        if (replacement != NULL) {
                            BrResFree(model->faces);
                            BrResFree(model->vertices);
                            model->faces = BrResAllocate(model, BrResSize(replacement->faces), BrResClass(replacement->faces));
                            c2_memcpy(model->faces, replacement->faces, BrResSize(replacement->faces));
                            model->nfaces = replacement->nfaces;
                            model->vertices = BrResAllocate(model, BrResSize(replacement->vertices), BrResClass(replacement->vertices));
                            c2_memcpy(model->vertices, replacement->vertices, BrResSize(replacement->vertices));
                            model->nvertices = replacement->nvertices;
                            BrModelUpdate(model, BR_MODU_ALL);
                            break;
                        }
                    }
                }
            }
        }
        if (model != NULL && (model->flags & (BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE))) {
            if (model->identifier != NULL && model->identifier[0] != '-') {
                model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
            }
            for (j = 0; j < V11MODEL(model)->ngroups; j++) {
                v11group* v11_group = &V11MODEL(model)->groups[j];
                v11_group->face_colours.material = model->faces[*v11_group->face_user].material;
                if (v11_group->face_colours.material != NULL) {
                    SetMaterialTrackLighting(v11_group->face_colours.material);
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
    LoadAIWorld(f);
    PrintMemoryDump(0, "JUST LOADED IN AI WORLD");
    PrepareSmashableTrackItemIdentifiers();
    ApplyPreviousTiffConversion();

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
        str = c2_strtok(s, ".");
        if (c2_strcmp(str, "NONE") == 0) {
            modifier->skid_mark_material = NULL;
        } else if (c2_strcmp(str, "0") == 0) {
            modifier->skid_mark_material = NULL;
        } else if (c2_strcmp(str, "1") == 0) {
            modifier->skid_mark_material = NULL;
        } else {
            c2_strcat(str, ".PIX");
            LoadSinglePixelmap(&C2V(gTrack_storage_space), str);
            str[c2_strlen(str) - 4] = '\0';
            c2_strcat(str, ".MAT");
            modifier->skid_mark_material = LoadSingleMaterial(&C2V(gTrack_storage_space), str);
        }
    }
    ApplyTopTiffConversion();
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

    C2V(gDefault_water_spec_vol_real)->material_modifier_index = 10;

    /* Non CarObjects */
    count_noncar_objects = GetAnInt(f);
    if (count_noncar_objects > 40) { /* FIXME: magic number */
        PDFatalError("Too many non-car types");
    }
    C2V(gProgram_state).num_non_car_spaces = count_noncar_objects + C2V(gCount_smashable_noncars) + 40;
    C2V(gProgram_state).non_cars = BrMemCalloc(C2V(gProgram_state).num_non_car_spaces, sizeof(tNon_car_spec), kMem_non_car_spec);
    C2_HOOK_BUG_ON(sizeof(tNon_car_spec) != 260);
    if (C2V(gProgram_state).non_cars == NULL && count_noncar_objects != 0) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }
    c2_memset(C2V(gNon_car_spec_indices), 0, sizeof(C2V(gNon_car_spec_indices)));
    C2_HOOK_BUG_ON(sizeof(C2V(gNon_car_spec_indices)) != 100);
    for (i = 0; i < 40; i++) { /* FIXME: magic */
        tCollision_info *collision_info;
        tNon_car_spec* non_car;

        non_car = &C2V(gProgram_state).non_cars[i];
        non_car->driver = eDriver_non_car_unused_slot;
        collision_info = BrMemAllocate(sizeof(tCollision_info), kMem_collision_object);
        C2_HOOK_BUG_ON(sizeof(tCollision_info) != 1240);
        non_car->collision_info = collision_info;
        collision_info->owner = non_car;
        collision_info->flags_0x238 = 1;
        collision_info->field_0x1a0 = 0xffff;
        collision_info->field_0x1a4 = 0;
    }
    for (i = 0; i < count_noncar_objects + C2V(gCount_smashable_noncars); i++) {
        tCollision_info* collision_info;
        tNon_car_spec* non_car;
        char non_cars_path[256];
        FILE* g;

        PossibleService();
        non_car = &C2V(gProgram_state).non_cars[40 + i]; /* FIXME: magic number */
        collision_info = BrMemAllocate(sizeof(tCollision_info), kMem_collision_object);
        C2_HOOK_BUG_ON(sizeof(tCollision_info) != 1240);
        non_car->collision_info = collision_info;
        collision_info->owner = non_car;
        collision_info->flags_0x238 = 1;
        collision_info->field_0x1a0 = 0xffff;
        collision_info->field_0x1a4 = 0;

        if (i < count_noncar_objects) {
            GetAString(f, s);
        } else {
            c2_strcpy(s, C2V(gSmashable_noncars)[i - count_noncar_objects]);
        }
        PathCat(non_cars_path, C2V(gApplication_path), "NONCARS");
        PathCat(non_cars_path, non_cars_path, s);
        g = DRfopen(non_cars_path, "rt");
        if (g == NULL) {
            FatalError(kFatalError_CantOpen_S, non_cars_path);
        }
        LoadNonCar(g, non_car);
        PossibleService();
        C2V(gNon_car_spec_indices)[non_car->index] = i + 1;
        PFfclose(g);
    }
    FreeSmashableNonCarNames();
    for (i = 0; i < C2V(gProgram_state).track_spec.count_non_cars; i++) {
        br_actor* actor = C2V(gProgram_state).track_spec.non_car_list[i];
        int idx;
        tNon_car_spec* non_car;
        if (actor == NULL) {
            continue;
        }
        idx = C2V(gNon_car_spec_indices)[10 * (actor->identifier[1] - '0') + (actor->identifier[2] - '0')] + 40 - 1;
        non_car = &C2V(gProgram_state).non_cars[idx];
        if (non_car == NULL || non_car->collision_info->actor != NULL) {
            continue;
        }
        non_car->collision_info->actor = actor;
        for (j = 0; j < 3; j++) {
            actor->model->bounds.min.v[j] = MIN(actor->model->bounds.min.v[j], non_car->collision_info->bb2.min.v[j]);
            actor->model->bounds.max.v[j] = MAX(actor->model->bounds.max.v[j], non_car->collision_info->bb2.max.v[j]);
        }
    }
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
    C2V(gYon_multiplier) = GetAFloat(f);
    GetAString(f, s);
    if (DRStricmp(s, pFile_name) != 0) {
        FatalError(kFatalError_FileIsCorrupted_S, pFile_name);
    }
    PFfclose(f);
    for (material_exception = C2V(gMaterial_exceptions); material_exception != NULL; ) {
        tMaterial_exception* next_material_exception = material_exception->next;
        BrMemFree(material_exception->texture_name);
        BrMemFree(material_exception);
        material_exception = next_material_exception;
    }
    C2V(gMaterial_exceptions) = NULL;
    PrintMemoryDump(0, "FINISHED LOADING TRACK");
    /* nop_FUN_00486db0(race_lighting_path); */
    ClosePackFileAndSetTiffLoading(twt);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00504bf0, LoadTrack, LoadTrack_original)

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
C2_HOOK_FUNCTION(0x00500f30, ClearOutStorageSpace)

void (C2_HOOK_FASTCALL * DisposeTexturingMaterials_original)(void);
void C2_HOOK_FASTCALL DisposeTexturingMaterials(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposeTexturingMaterials_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004489d0, DisposeTexturingMaterials, DisposeTexturingMaterials_original)

intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg) {

    if (pActor->type == BR_ACTOR_BOUNDS || pActor->type == BR_ACTOR_BOUNDS_CORRECT) {
        BrResFree(pActor->type_data);
        pActor->type_data = NULL;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00506e20, RemoveBounds)

void C2_HOOK_FASTCALL RemoveBoundsStructures(br_actor* pActor) {

    DRActorEnumRecurse(pActor, RemoveBounds, NULL);
}

void (C2_HOOK_FASTCALL * FreeTrack_original)(tTrack_spec* pTrack_spec);
void C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec) {
    int i;

    DisposeSmashableTrackEnvironment();
    if (C2V(gAdditional_actors) != NULL) {
        BrActorRemove(C2V(gAdditional_actors));
        BrActorFree(C2V(gAdditional_actors));
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
    ClearOutStorageSpace(&C2V(gTrack_storage_space));
    C2V(gProgram_state).current_depth_effect.sky_texture = NULL;
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
    C2V(gTrack_actor) = NULL;
    if (C2V(gProgram_state).special_volume_count != 0) {
        BrMemFree(C2V(gProgram_state).special_volumes);
    }
    if (C2V(gProgram_state).special_screens_count != 0) {
        BrMemFree(C2V(gProgram_state).special_screens);
    }
    PossibleService();
    for (i = 0; i < 40; i++) { /* FIXME: magic number 40 */
        tNon_car_spec *non_car;

        non_car = &C2V(gProgram_state).non_cars[i];
        non_car->collision_info->shape = NULL;
        non_car->collision_info->physics_joint1 = NULL;
        non_car->collision_info->physics_joint2 = NULL;
    }
    for (i = 0; i < C2V(gProgram_state).num_non_car_spaces; i++) {
        tNon_car_spec* non_car;

        non_car = &C2V(gProgram_state).non_cars[i];
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
    if (C2V(gProgram_state).non_cars != NULL) {
        BrMemFree(C2V(gProgram_state).non_cars);
        /* C2V(gProgram_state).non_cars = NULL; */
    }
    FreeSmokeShadeTables();
    if (C2V(gTrack_flic_buffer) != NULL) {
        BrMapRemove(C2V(gProgram_state).default_depth_effect.sky_texture);
        BrMemFree(C2V(gTrack_flic_buffer));
        EndFlic(&C2V(gTrack_flic_descriptor));
        BrMemFree(C2V(gProgram_state).default_depth_effect.sky_texture->pixels);
        C2V(gProgram_state).default_depth_effect.sky_texture->pixels = NULL;
        BrPixelmapFree(C2V(gProgram_state).default_depth_effect.sky_texture);
        C2V(gTrack_flic_buffer) = NULL;
    }
}
C2_HOOK_FUNCTION_ORIGINAL(0x00506c20, FreeTrack, FreeTrack_original)

void (C2_HOOK_FASTCALL * SaveAdditionalStuff_original)(void);
void C2_HOOK_FASTCALL SaveAdditionalStuff(void) {

#if defined(C2_HOOKS_ENABLED)
    SaveAdditionalStuff_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00507360, SaveAdditionalStuff, SaveAdditionalStuff_original)
