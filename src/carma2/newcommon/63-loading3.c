#include "63-loading3.h"

#include "08-loading1.h"
#include "18-graphics2.h"
#include "41-utility.h"
#include "70-packfile.h"
#include "rec2_types.h"
#include "globvars.h"
#include "platform.h"
#include "loadtif.h"
#include "rec2_macros.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x0065852c
char* gPedTexturePath = "PIXELMAP";

// NearestPowerOfTwo

// FUNCTION: CARMA2_HW 0x0048ea40
int C2_HOOK_FASTCALL LoadBunchOfPixies(const char* pathRoot, const char* textureName, br_pixelmap** textureBuffer, size_t bufferCapacity) {
    tPath_name tempPath;
    br_uint_32 nb;

    PathCat(tempPath, pathRoot, textureName);
    // strcat(tempPath, (gPixelFlags & kPixelFlags_16bbp) ? ".P16" : ".P08");
    if (gPixelFlags & kPixelFlags_16bbp) {
        strcat(tempPath, ".P16");
    } else {
        strcat(tempPath, ".P08");
    }
    nb = BrPixelmapLoadMany(tempPath, textureBuffer, (br_uint_16)bufferCapacity);
    if (nb == 0) {
        if (gPixelFlags & kPixelFlags_16bbp) {
            PathCat(tempPath, pathRoot, "PIX16");
        } else {
            PathCat(tempPath, pathRoot, "PIX8");
        }
        PathCat(tempPath, tempPath, textureName);
        strcat(tempPath, ".PIX");
        nb = BrPixelmapLoadMany(tempPath, textureBuffer, (br_uint_16)bufferCapacity);
    }
    if (nb != 0) {
        if (nb == 1 && !gDisableTiffConversion) {
            if (textureBuffer[0]->identifier != NULL) {
                BrResFree(textureBuffer[0]->identifier);
            }
            textureBuffer[0]->identifier = BrResStrDup(textureBuffer[0], textureName);
        }
    }
    return nb;
}

// FUNCTION: CARMA2_HW 0x0048eb80
br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode) {
    br_pixelmap* textures[1000];
    int nb;

    if (gDisableTiffConversion) {
        nb = LoadBunchOfPixies(texturePathDir, textureName, textures, REC2_ASIZE(textures));
        if (nb != 0) {
            *errorCode = 0;
            return textures[0];
        } else {
            *errorCode = 1;
            return NULL;
        }
    } else {
        return LoadTiffTexture_Ex(texturePathDir, textureName, pPalette, flags | kLoadTextureFlags_SaveTextureCompressed, errorCode);
    }
}

// FUNCTION: CARMA2_HW 0x0048ec20
br_pixelmap* C2_HOOK_FASTCALL RealLoadPixelmap(const char* pPath_name) {
    const char* ext;
    tPath_name path;
    char texture_name[256];
    int error;
    br_pixelmap* pm;

    ext = strrchr(pPath_name, '.');
    if (ext == NULL) {
        ext = pPath_name + strlen(pPath_name);
    }
    if (ext - pPath_name == 4) {
        if (memcmp(pPath_name, "none", 4) == 0) {
            return NULL;
        }
    }
    PossibleService();
    PathCat(path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(path, path, "PIXELMAP");
    strcpy(texture_name, pPath_name);
    texture_name[strlen(texture_name) - 4] = '\0';
    AllowOpenToFail();
    pm = DRLdImg(path, texture_name, gRender_palette, gPixelFlags, &error);
    DoNotAllowOpenToFail();
    if (pm == NULL) {
        PathCat(path, gApplication_path, gPedTexturePath);
        return DRLdImg(path, texture_name, gRender_palette, gPixelFlags, &error);
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x0048ec00
br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name) {
    br_pixelmap *pm;

    pm = RealLoadPixelmap(pPath_name);
    EnsurePixelmapAllowed(pm, 0);
    return pm;
}

// LoadShadeTable

// LoadMaterial

// LoadModel

// LoadActor

// DRLoadPalette

// DRLoadShadeTable

// DRLoadMaterials

// DRLoadModels

// DRLoadActors

// DRLoadLights

// LoadInFiles

// STUB: CARMA2_HW 0x0048f3b0
int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0048f7f0
void C2_HOOK_FASTCALL SetDefaultPixelmapFolderName(void) {
    gPedTexturePath = "PIXELMAP";
}

