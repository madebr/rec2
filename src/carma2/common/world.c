#include "world.h"

#include "car.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "sound.h"
#include "utility.h"

#include "platform.h"

#include <brender/brender.h>
#include "rec2_macros.h"

#include <tiffio.h>

#include "c2_string.h"
#include "c2_sys/c2_stat.h"

#define RGB565_R(V) (((V) & 0xf800) >> 11)
#define RGB565_G(V) (((V) & 0x07e0) >> 5)
#define RGB565_B(V) (((V) & 0x001f) >> 0)
#define RGB888_R(V) (((V) >> 16) & 0xff)
#define RGB888_G(V) (((V) >> 8) & 0xff)
#define RGB888_B(V) (((V) >> 0) & 0xff)

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSoundType_Choices, 2, 0x00660268, {"SATURATED", "SCATTERED"});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tCar_texturing_level, gCar_texturing_level, 0x00591374, eCTL_full);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tCar_texturing_level, gRoad_texturing_level, 0x0059136c, eRTL_full);
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

C2_HOOK_VARIABLE_IMPLEMENT(tMaterial_exception*, gMaterial_exceptions, 0x0074ca04);

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

int C2_HOOK_FASTCALL LoadBrenderTextures(const char* pathRoot, const char* textureName, br_pixelmap** textureBuffer, size_t bufferCapacity) {
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
C2_HOOK_FUNCTION(0x0048ea40, LoadBrenderTextures)

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
            ParseSoundFxDetails(pF, &pSpec->soundfx_data);
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
    StringToUpper(pathUpper, pathCopy);
    if (c2_strstr(pathUpper, ".PIX") == NULL
            && c2_strstr(pathUpper, ".TIF") == NULL) {
        return 0;
    }
    ExtractPath_Dirname_Stem(pathUpper, dir_path, stem_path);
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
    StringToUpper(upperPath, pathCopy);

    if (c2_strstr(upperPath, ".PIX") == NULL
            && c2_strstr(upperPath, ".P16") == NULL
            && c2_strstr(upperPath, ".P08") == NULL
            && c2_strstr(upperPath, ".TIF") == NULL) {
        return 0;
    }
    ExtractPath_Dirname_Stem(upperPath, dir_path, stem_path);

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
    pStorage_space->sounds = BrMemCalloc(pMax_sounds, sizeof(undefined4*), kMem_stor_space_table); // FIXME: fill in correct type
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

tAdd_to_storage_result (C2_HOOK_FASTCALL * LoadSoundInStorage_original)(tBrender_storage* pStorage_space, int pSound_id);
tAdd_to_storage_result C2_HOOK_FASTCALL LoadSoundInStorage(tBrender_storage* pStorage_space, int pSound_id) {

#if defined(C2_HOOKS_ENABLED)
    return LoadSoundInStorage_original(pStorage_space, pSound_id);
#else
#error "Not implemented"
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
        DRfclose(f);
        *errorCode = 5;
        return NULL;
    }
    DRfclose(f);
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

br_pixelmap* C2_HOOK_FASTCALL LoadTiffOrBrenderTexture_Ex(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode) {
    br_pixelmap* textures[1000];
    int nb;

    if (C2V(gDisableTiffConversion)) {
        nb = LoadBrenderTextures(texturePathDir, textureName, textures, REC2_ASIZE(textures));
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
C2_HOOK_FUNCTION(0x0048eb80, LoadTiffOrBrenderTexture_Ex)

int C2_HOOK_FASTCALL LoadTiffOrBrenderTexture(const char* texturePathNoExt, br_pixelmap** pixelmaps, size_t capacity) {
    tPath_name texturePath;
    tPath_name texturePathDir;
    tPath_name texturePathStem;
    int errorCode;

    c2_strcpy(texturePath, texturePathNoExt);
    c2_strcat(texturePath, ".TIF");
    ExtractPath_Dirname_Stem(texturePath, texturePathDir, texturePathStem);
    pixelmaps[0] = LoadTiffOrBrenderTexture_Ex(texturePathDir, texturePathStem, C2V(gRender_palette), C2V(gPixelFlags), &errorCode);
    return (pixelmaps[0] != NULL && errorCode == 0) ? 1 : 0;
}
C2_HOOK_FUNCTION(0x00514570, LoadTiffOrBrenderTexture)

int C2_HOOK_FASTCALL LoadNPixelmapsFromPath(tBrender_storage* pStorage_space, const char* path) {
    int i;
    int nbLoaded;
    int result;
    tPath_name path_dirname;
    tPath_name path_stem;
    tAdd_to_storage_result addResult;
    br_pixelmap* pixelmaps[500];

    result = 0;
    if (C2V(gDisableTiffConversion)) {
        ExtractPath_Dirname_Stem(path, path_dirname, path_stem);
        nbLoaded = LoadBrenderTextures(path_dirname, path_stem, pixelmaps, REC2_ASIZE(pixelmaps));
    } else {
        nbLoaded = LoadTiffOrBrenderTexture(path, pixelmaps, REC2_ASIZE(pixelmaps));
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
C2_HOOK_FUNCTION(0x005024f0, LoadNPixelmapsFromPath)

void C2_HOOK_FASTCALL LoadAllTexturesFromTexSubdirectories(tBrender_storage* pStorage_space, const char* path) {
    tPath_name pathCopy;
    tPath_name tempPath;
    tName_list list;
    size_t i;

    C2_HOOK_BUG_ON(sizeof(tBrender_storage) != 68);

    list.size = 0;
    c2_strcpy(pathCopy, path);
    C2V(gStorageForCallbacks) = pStorage_space;
    if (C2V(gDisableTiffConversion)) {
        DREnumPath(pathCopy, (tEnumPathCallback)AddTextureFileStemToList, &list);
    }
    if (!C2V(gDisableTiffConversion)) {
        PathCat(tempPath, pathCopy, "TIFFX");
        DREnumPath(tempPath, (tEnumPathCallback)AddTexturePixTifFileStemToList, &list);
    }
    PathCat(tempPath, pathCopy, "PIX8");
    DREnumPath(tempPath, (tEnumPathCallback)AddTextureFileStemToList, &list);
    if (!C2V(gDisableTiffConversion)) {
        PathCat(tempPath, pathCopy, "TIFFRGB");
        DREnumPath(tempPath, (tEnumPathCallback)AddTextureFileStemToList, &list);
    }
    PathCat(tempPath, pathCopy, "PIX16");
    DREnumPath(tempPath, (tEnumPathCallback)AddTextureFileStemToList, &list);
    for (i = 0; i < list.size; i++) {
        PathCat(tempPath, path, list.items[i]);
        LoadNPixelmapsFromPath(C2V(gStorageForCallbacks), tempPath);
    }
}
C2_HOOK_FUNCTION(0x005028f0, LoadAllTexturesFromTexSubdirectories)

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
    DRfclose(f);
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

void (C2_HOOK_FASTCALL * LoadTrack_original)(char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info);
void C2_HOOK_FASTCALL LoadTrack(char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info) {
#if defined(C2_HOOKS_ENABLED)
    LoadTrack_original(pFile_name, pTrack_spec, pRace_info);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00504bf0, LoadTrack, LoadTrack_original)

tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st) {
    int i;

    if (pStorage_space->shade_tables_count >= pStorage_space->max_shade_tables) {
        return eStorage_not_enough_room;
    }

    for (i = 0; i < pStorage_space->shade_tables_count; i++) {
        if (pStorage_space->shade_tables[i]->identifier
            && pThe_st->identifier
            && !c2_strcmp(pStorage_space->shade_tables[i]->identifier, pThe_st->identifier)) {
            return eStorage_duplicate;
        }
    }
    pStorage_space->shade_tables[pStorage_space->shade_tables_count] = pThe_st;
    pStorage_space->shade_tables_count++;
    return eStorage_allocated;
}

void C2_HOOK_FASTCALL LoadShadeTableCallback(const char* pPath) {
    br_pixelmap* temp_array[50];
    int total;
    int i;
    tBrender_storage* storage_space;
    char s[256];

    StringToUpper(s, pPath);
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
C2_HOOK_FUNCTION(0x00502b80, LoadShadeTableCallback)

void C2_HOOK_FASTCALL LoadSomeShadeTables(tBrender_storage* pStorage, const char* pPath) {

    C2V(gStorageForCallbacks) = pStorage;
    DRForEveryFile(pPath, LoadShadeTableCallback);
}
C2_HOOK_FUNCTION(0x00502b60, LoadSomeShadeTables)

void C2_HOOK_FASTCALL LoadPixelmapCallback(const char* pPath) {
    char s[256];

    StringToUpper(s, pPath);
    if (c2_strstr(s, ".PIX") != NULL) {
        LoadNPixelmapsFromPath(C2V(gStorageForCallbacks), pPath);
    }
}
C2_HOOK_FUNCTION(0x005024b0, LoadPixelmapCallback)

void C2_HOOK_FASTCALL LoadSomePixelmaps(tBrender_storage* pStorage, const char* pPath) {

    C2V(gStorageForCallbacks) = pStorage;
    DRForEveryFile(pPath, LoadPixelmapCallback);
}
C2_HOOK_FUNCTION(0x00502490, LoadSomePixelmaps)

void C2_HOOK_FASTCALL LoadMaterialCallback(const char* pPath) {
    tRendererShadingType shading;
    char s[256];

    shading = C2V(gMaterial_shading_for_callback);
    if (C2V(gMaterial_shading_for_callback) == kRendererShadingType_Undefined) {
        shading = kRendererShadingType_Default;
    }
    StringToUpper(s, pPath);
    if (c2_strstr(s, ".MAT") != NULL) {
        LoadMaterialsInto(C2V(gStorageForCallbacks), pPath, shading);
    }
}
C2_HOOK_FUNCTION(0x00502a70, LoadMaterialCallback)

void C2_HOOK_FASTCALL LoadSomeMaterialsWithShading(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    C2V(gMaterial_shading_for_callback) = pShading;
    C2V(gStorageForCallbacks) = pStorage;
    DRForEveryFile(pPath, LoadMaterialCallback);
    C2V(gMaterial_shading_for_callback) = kRendererShadingType_Undefined;
}
C2_HOOK_FUNCTION(0x00502ad0, LoadSomeMaterialsWithShading)

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

int C2_HOOK_FASTCALL LoadMaterialsInto(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading) {
    int i;
    int new_ones;
    int total;
    br_material* temp_array[500];

    new_ones = 0;
    total = BrMaterialLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadMaterialFileOrItIsEmpty_S, pPath);
    }
    AdaptMaterialsForRenderer(temp_array, total, pShading);
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
C2_HOOK_FUNCTION(0x00502060, LoadMaterialsInto)

void C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage *pStorage, FILE* pFile, tRendererShadingType pShading) {
    char s1[256];
    char s2[256];

    PossibleService();
    GetALineAndDontArgue(pFile, s1);
    PathCat(s2, C2V(gApplication_path), "MATERIAL");
    PathCat(s2, s2, c2_strtok(s1, "\t ,/"));
    LoadMaterialsInto(pStorage, s2, pShading);
}
C2_HOOK_FUNCTION(0x00501fe0, LoadSomeMaterials)


tAdd_to_storage_result AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod) {
    int i;

    C2V(gDuplicate_model) = NULL;
    if (pStorage_space->materials_count >= pStorage_space->max_models) {
        return eStorage_not_enough_room;
    }
    if (pStorage_space->flags & 0x1) {
        for (i = 0; i < pStorage_space->models_count; i++) {
            if (pStorage_space->models[i]
                && pStorage_space->models[i]->identifier
                && pThe_mod->identifier
                && !c2_strcmp(pStorage_space->models[i]->identifier, pThe_mod->identifier)) {
                C2V(gDuplicate_model) = pStorage_space->models[i];
                return eStorage_duplicate;
            }
        }
    }
    pStorage_space->models[pStorage_space->models_count] = pThe_mod;
    pStorage_space->models_count++;
    return eStorage_allocated;
}

int C2_HOOK_FASTCALL LoadModelsInto(tBrender_storage* pStorage_space, const char* pPath) {
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
C2_HOOK_FUNCTION(0x00501e40, LoadModelsInto)

void C2_HOOK_FASTCALL LoadModelCallback(const char* pPath) {
    char s[256];

    StringToUpper(s, pPath);
    if (c2_strstr(s, ".DAT") != NULL) {
        LoadModelsInto(C2V(gStorageForCallbacks), pPath);
    }
}
C2_HOOK_FUNCTION(0x00502b20, LoadModelCallback)

void C2_HOOK_FASTCALL LoadSomeModels(tBrender_storage *pStorage, const char* pPath) {

    C2V(gStorageForCallbacks) = pStorage;
    DRForEveryFile(pPath, LoadModelCallback);
}
C2_HOOK_FUNCTION(0x00502b00, LoadSomeModels)

void C2_HOOK_FASTCALL LoadFolderInStorageWithShading(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    LoadSomeShadeTables(pStorage, pPath);
    LoadSomePixelmaps(pStorage, pPath);
    LoadAllTexturesFromTexSubdirectories(pStorage, pPath);
    LoadSomeMaterialsWithShading(pStorage, pPath, pShading);
    LoadSomeModels(pStorage, pPath);
}
C2_HOOK_FUNCTION(0x00502cf0, LoadFolderInStorageWithShading)

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
