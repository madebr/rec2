#include "world.h"

#include "car.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "sound.h"
#include "utility.h"

#include <brender/brender.h>
#include "rec2_macros.h"

#include "c2_string.h"
#include "c2_sys/c2_stat.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSoundType_Choices, 2, 0x00660268, {"SATURATED", "SCATTERED"});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tCar_texturing_level, gCar_texturing_level, 0x00591374, eCTL_full);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tCar_texturing_level, gRoad_texturing_level, 0x0059136c, eRTL_full);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tWall_texturing_level, gWall_texturing_level, 0x00591370, eWTL_full);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gRendering_accessories, 0x00591368, 1);
C2_HOOK_VARIABLE_IMPLEMENT(tBrender_storage*, gStorageForCallbacks, 0x006b7820);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gAddedPixelmap, 0x006aaa20);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisallowDuplicates, 0x006aaa2c);

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

void C2_HOOK_FASTCALL ParseSpecialVolume(tTWTFILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx) {
    char s[256];

    REC2_BUG_ON(sizeof(tSpecial_volume) != 220);
    REC2_BUG_ON(sizeof(tSpecial_volume_soundfx_data) != 36);
    REC2_BUG_ON(offsetof(tSpecial_volume, gravity_multiplier) != 136);

    pSpec->gravity_multiplier = GetAFloat(pF);
    if (pSpec->gravity_multiplier < 0 && !C2V(gRusselsFannies)) {
        pSpec->gravity_multiplier = 1.f;

    }
    pSpec->viscosity_multiplier = GetAFloat(pF);
    pSpec->car_damage_per_ms = GetAScalar(pF);
    pSpec->ped_damage_per_ms = GetAScalar(pF);
    pSpec->camera_special_effect_index = GetAnInt(pF);
    pSpec->sky_col = GetAnInt(pF);

    GetAString(pF, s);
    if (pScreen_name_str != NULL) {
        c2_strcpy(pScreen_name_str, s);
    } else {
        pSpec->screen_material = BrMaterialFind(s);
    }

    pSpec->entry_noise = GetAnInt(pF);
    pSpec->exit_noise = GetAnInt(pF);
    pSpec->engine_noise_index = GetAnInt(pF);
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

br_pixelmap* (C2_HOOK_FASTCALL * LoadTiffTexture_Ex2_original)(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode, int useTiffx);
br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex2(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode, int useTiffx) {
#if defined(C2_HOOKS_ENABLED)
    return LoadTiffTexture_Ex2_original(texturePathDir, textureName, pPalette, flags, errorCode, useTiffx);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00485750, LoadTiffTexture_Ex2, LoadTiffTexture_Ex2_original)

br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode) {
    br_pixelmap* texture;

    texture = NULL;
    if ((flags & 0x1) != 0x1) {
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
        return LoadTiffTexture_Ex(texturePathDir, textureName, pPalette, flags | 0x40, errorCode);
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

    list.size = 0;
    c2_strcpy(pathCopy, path);
    C2V(gStorageForCallbacks) = pStorage_space;
    if (C2V(gDisableTiffConversion)) {
        TWT_EnumPath(pathCopy, (tEnumPathCallback)AddTextureFileStemToList, &list);
    }
    if (!C2V(gDisableTiffConversion)) {
        PathCat(tempPath, pathCopy, "TIFFX");
        TWT_EnumPath(tempPath, (tEnumPathCallback)AddTexturePixTifFileStemToList, &list);
    }
    PathCat(tempPath, pathCopy, "PIX8");
    TWT_EnumPath(tempPath, (tEnumPathCallback)AddTextureFileStemToList, &list);
    if (!C2V(gDisableTiffConversion)) {
        PathCat(tempPath, pathCopy, "TIFFRGB");
        TWT_EnumPath(tempPath, (tEnumPathCallback)AddTextureFileStemToList, &list);
    }
    PathCat(tempPath, pathCopy, "PIX16");
    TWT_EnumPath(tempPath, (tEnumPathCallback)AddTextureFileStemToList, &list);
    for (i = 0; i < list.size; i++) {
        PathCat(tempPath, path, list.items[i]);
        LoadNPixelmapsFromPath(C2V(gStorageForCallbacks), tempPath);
    }
}
C2_HOOK_FUNCTION(0x005028f0, LoadAllTexturesFromTexSubdirectories)

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
    tTWTFILE* f;
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
