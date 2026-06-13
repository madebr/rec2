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

// FUNCTION: CARMA2_HW 0x0048ef40
br_pixelmap* C2_HOOK_FASTCALL LoadShadeTable(const char* pName) {
    tPath_name the_path;

    PathCat(the_path, gApplication_path, "SHADETAB");
    PathCat(the_path, the_path, pName);
    return BrPixelmapLoad(the_path);
}

// FUNCTION: CARMA2_HW 0x0048ef80
br_material* C2_HOOK_FASTCALL LoadMaterial(const char* pThe_path) {
    tPath_name the_path;

    PossibleService();
    PathCat(the_path, gApplication_path, "MATERIAL");
    PathCat(the_path, the_path, pThe_path);
    return BrMaterialLoad(the_path);
}

// FUNCTION: CARMA2_HW 0x0048efd0
br_model* C2_HOOK_FASTCALL LoadModel(const char* pName) {
    tPath_name the_path;

    PossibleService();
    PathCat(the_path, gApplication_path, "MODELS");
    PathCat(the_path, the_path, pName);
    return BrModelLoad(the_path);
}

// FUNCTION: CARMA2_HW 0x0048f020
br_actor* C2_HOOK_FASTCALL LoadActor(const char* pName) {
    tPath_name the_path;
    br_actor* actor;

    PossibleService();
    PathCat(the_path, gApplication_path, "ACTORS");
    PathCat(the_path, the_path, pName);
    actor = BrActorLoad(the_path);
    if (actor == NULL) {
        PathCat(the_path, gRace_path, pName);
        actor = BrActorLoad(the_path);
    }
    return actor;
}

// FUNCTION: CARMA2_HW 0x0048f090
void C2_HOOK_FASTCALL DRLoadPalette(const char* pPath_name) {
    br_pixelmap* palette_array[100];
    int number_of_palettes;
    int i;

    number_of_palettes = BrPixelmapLoadMany(pPath_name, palette_array, REC2_ASIZE(palette_array));
    for (i = 0; i < number_of_palettes; i++) {
        palette_array[i]->row_bytes = (palette_array[i]->row_bytes + 3) & ~0x3;
        palette_array[i]->base_x = 0;
        palette_array[i]->base_y = 0;
    }
    BrTableAddMany(palette_array, number_of_palettes);
}

// FUNCTION: CARMA2_HW 0x0048f100
void C2_HOOK_FASTCALL DRLoadShadeTable(const char* pPath_name) {
    br_pixelmap* table_array[100];
    int number_of_tables;
    int i;

    number_of_tables = BrPixelmapLoadMany(pPath_name, table_array, REC2_ASIZE(table_array));
    for (i = 0; i < number_of_tables; i++) {
        table_array[i]->row_bytes = (table_array[i]->row_bytes + 3) & ~0x3;
        table_array[i]->base_x = 0;
        table_array[i]->base_y = 0;
    }
    BrTableAddMany(table_array, number_of_tables);
}

// FUNCTION: CARMA2_HW 0x0048f1d0
void C2_HOOK_FASTCALL DRLoadMaterials(const char* pPath_name) {
    br_material* material_array[100];
    int i;
    int number_of_materials;

    PossibleService();
    number_of_materials = BrMaterialLoadMany(pPath_name, material_array, REC2_ASIZE(material_array));
    for (i = 0; i < number_of_materials; i++) {
        material_array[i]->flags &= ~BR_MATF_LIGHT;
    }
    BrMaterialAddMany(material_array, number_of_materials);
}

// FUNCTION: CARMA2_HW 0x0048f230
void C2_HOOK_FASTCALL DRLoadModels(const char* pPath_name) {
    int i;
    br_model* model_array[100];
    int number_of_models;

    PossibleService();
    number_of_models = BrModelLoadMany(pPath_name, model_array, REC2_ASIZE(model_array));
    WhitenVertexRGB(model_array, number_of_models);
    for (i = 0; i < number_of_models; i++) {
        model_array[i]->flags = BR_MODF_UPDATEABLE;
    }
    BrModelAddMany(model_array, number_of_models);
}

// FUNCTION: CARMA2_HW 0x0048f290
void C2_HOOK_FASTCALL DRLoadActors(const char* pPath_name) {
    int i;
    br_actor* actor_array[100];
    int number_of_actors;

    PossibleService();
    number_of_actors = BrActorLoadMany(pPath_name, actor_array, REC2_ASIZE(actor_array));
    for (i = 0; i < number_of_actors; i++) {
        gActor_array[gNumber_of_actors] = actor_array[i];
        gNumber_of_actors++;
    }
}

// FUNCTION: CARMA2_HW 0x0048f2e0
void C2_HOOK_FASTCALL DRLoadLights(const char* pPath_name) {
    br_actor* actor;
    br_light* light;

    actor = BrActorAllocate(BR_ACTOR_LIGHT, NULL);
    light = actor->type_data;
    light->type = BR_LIGHT_DIRECT;
    light->colour = BR_COLOUR_RGB(gLighting_data.directional.red, gLighting_data.directional.green, gLighting_data.directional.blue);
    light->attenuation_c = 1.0f;
    BrMatrix34RotateX(&actor->t.t.mat, BR_ANGLE_DEG(-60));
    BrMatrix34PostRotateY(&actor->t.t.mat, BR_ANGLE_DEG(30));
    gLight_array[gNumber_of_lights] = actor;
    gNumber_of_lights++;
    EnableLights();
}

// FUNCTION: CARMA2_HW 0x0048f360
void C2_HOOK_FASTCALL LoadInFiles(const char* pThe_path, const char* pArchive_name, tPDForEveryFileRecurse_cbfn pAction_routine) {
    tPath_name the_path;
    tTWTVFS twt;

    PathCat(the_path, pThe_path, pArchive_name);
    twt = OpenPackFileAndSetTiffLoading(the_path);
    PFForEveryFile(the_path, pAction_routine);
    ClosePackFileAndSetTiffLoading(twt);
}

// STUB: CARMA2_HW 0x0048f3b0
int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0048f7f0
void C2_HOOK_FASTCALL SetDefaultPixelmapFolderName(void) {
    gPedTexturePath = "PIXELMAP";
}

