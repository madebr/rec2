#include "frontend.h"

#include "drmem.h"
#include "52-errors.h"
#include "font.h"
#include "frontend_controls.h"
#include "frontend_credits.h"
#include "frontend_loadgame.h"
#include "frontend_main.h"
#include "frontend_netsync.h"
#include "frontend_network.h"
#include "frontend_networksummary.h"
#include "frontend_newgame.h"
#include "frontend_options.h"
#include "frontend_quit.h"
#include "frontend_startgame.h"
#include "frontend_wrecks.h"
#include "globvars.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "intrface.h"
#include "loading.h"
#include "main.h"
#include "options.h"
#include "polyfont.h"
#include "platform.h"
#include "sound.h"
#include "tinted.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_macros.h"
#include "c2_string.h"
#include "brender/br_types.h"


// GLOBAL: CARMA2_HW 0x0068c6e0
int gCount_interface_strings;

// GLOBAL: CARMA2_HW 0x0068c230
char* gInterface_strings[300];

// GLOBAL: CARMA2_HW 0x00686850
br_pixelmap* gFrontend_images[100];

// GLOBAL: CARMA2_HW 0x006870b8
tFrontend_model gFrontend_A_models[13];

// GLOBAL: CARMA2_HW 0x00687188
tFrontend_model gFrontend_B_models[11];

// GLOBAL: CARMA2_HW 0x00687058
tFrontend_model gFrontend_C_models[6];

// GLOBAL: CARMA2_HW 0x00687030
br_actor* gFrontend_backdrop_actors[3];

// GLOBAL: CARMA2_HW 0x0059b0d0
int gFrontend_stuff_not_loaded = 1;

// GLOBAL: CARMA2_HW 0x00688abc
tFrontend_spec* gCurrent_frontend_spec;

// GLOBAL: CARMA2_HW 0x00688378
br_model* gFrontend_A_model_from;

// GLOBAL: CARMA2_HW 0x0068844c
br_model* gFrontend_A_model_to;

// GLOBAL: CARMA2_HW 0x00688768
br_model* gFrontend_B_model_from;

// GLOBAL: CARMA2_HW 0x006886e0
br_model* gFrontend_B_model_to;

// GLOBAL: CARMA2_HW 0x00686504
br_model* gFrontend_C_model_from;

// GLOBAL: CARMA2_HW 0x006886bc
br_model* gFrontend_C_model_to;

// GLOBAL: CARMA2_HW 0x006864e0
int gFrontend_backdrop0_opacity_mode;

// GLOBAL: CARMA2_HW 0x006864dc
int gFrontend_backdrop1_opacity_mode;

// GLOBAL: CARMA2_HW 0x006864d4
int gFrontend_backdrop2_opacity_mode;

// GLOBAL: CARMA2_HW 0x00686ef8
int gFrontend_interpolate_steps_left;

// GLOBAL: CARMA2_HW 0x0068683c
int gFrontend_count_brender_items;

// GLOBAL: CARMA2_HW 0x00764ee0
int gFrontend_remove_current_backdrop;

// GLOBAL: CARMA2_HW 0x00686f8c
br_pixelmap* gFrontend_backdrop;

// GLOBAL: CARMA2_HW 0x00688770
int gFrontend_selected_item_index;

// GLOBAL: CARMA2_HW 0x00687248
tFrontend_brender_item gFrontend_brender_items[100]; /* FIXME: parametrize size + index of last item */

// GLOBAL: CARMA2_HW 0x00688ae8
br_colour gFrontend_some_color;

// GLOBAL: CARMA2_HW 0x0068875c
tU32 gFrontend_time_last_input;

// GLOBAL: CARMA2_HW 0x0068650c
br_actor* gFrontend_actor;

// GLOBAL: CARMA2_HW 0x00686f94
br_actor* gFrontend_camera;

// GLOBAL: CARMA2_HW 0x00686f50
br_token_value gFrontend_backdrop0_material_prims[3];

// GLOBAL: CARMA2_HW 0x00686f20
br_token_value gFrontend_backdrop1_material_prims[3];

// GLOBAL: CARMA2_HW 0x00686f38
br_token_value gFrontend_backdrop2_material_prims[3];

// GLOBAL: CARMA2_HW 0x00686f10
br_material* gFrontend_backdrop_materials[3];

// GLOBAL: CARMA2_HW 0x00763940
br_actor* gFrontend_billboard_actors[8];

// GLOBAL: CARMA2_HW 0x00688b10
br_actor* gFrontend_menu_camera;

// GLOBAL: CARMA2_HW 0x006886d0
double gFrontend_throb_factor;

// GLOBAL: CARMA2_HW 0x0075b8fc
int gTyping_slot;

// GLOBAL: CARMA2_HW 0x006883a8
int gFrontend_leave_current_menu;

// GLOBAL: CARMA2_HW 0x00688b20
int gFrontend_suppress_mouse;

// GLOBAL: CARMA2_HW 0x007635f0
br_uint_32 gFrontend_APO_Colour_1;

// GLOBAL: CARMA2_HW 0x007635e0
br_uint_32 gFrontend_APO_Colour_2;

// GLOBAL: CARMA2_HW 0x00763700
br_uint_32 gFrontend_APO_Colour_3;

// GLOBAL: CARMA2_HW 0x00688aec
br_actor* gFrontend_wrecks_actor;

// GLOBAL: CARMA2_HW 0x00688af0
br_actor* gFrontend_wrecks_camera;

// GLOBAL: CARMA2_HW 0x007635e4
br_pixelmap* gFrontend_wrecks_pixelmap;

// GLOBAL: CARMA2_HW 0x00764eec
tFrontendMenuType gFrontend_next_menu;

// GLOBAL: CARMA2_HW 0x00686834
tU32 gAuthor_credits_scroll_start_time;

// GLOBAL: CARMA2_HW 0x00688ab0
tConnected_items* gConnected_items;

// GLOBAL: CARMA2_HW 0x00686f08
int gCount_connected_items_indices;

// GLOBAL: CARMA2_HW 0x00687018
int gConnected_items_indices[6];

// GLOBAL: CARMA2_HW 0x006864f4
int gFrontend_scrollbars_updated;

// GLOBAL: CARMA2_HW 0x00688af4
char* gFrontend_current_input;

// GLOBAL: CARMA2_HW 0x0059b0d49
int gFrontend_maximum_input_length = 9;

// GLOBAL: CARMA2_HW 0x00763900
char gFrontend_original_player_name[32];

// GLOBAL: CARMA2_HW 0x0068723c
int gFrontend_text_input_item_index;

// GLOBAL: CARMA2_HW 0x0059b0d8
int gINT_0059b0d8 = -1;

// GLOBAL: CARMA2_HW 0x0068843c
tU32 gFrontend_last_scroll;

// GLOBAL: CARMA2_HW 0x00686508
tStruct_00686508* gPTR_00686508;

// GLOBAL: CARMA2_HW 0x00686820
tFrontend_slider* gCurrent_frontend_scrollbars;

// GLOBAL: CARMA2_HW 0x00688444
int gINT_00688444;

// GLOBAL: CARMA2_HW 0x00688408
tConnected_items gControls_scroller;

// GLOBAL: CARMA2_HW 0x00604888
int gControls_frontend_to_key_mapping_lut[29] = {
    49, 50, 47, 48, 54, 45, 60, 58, 56, 46,
    57, 67, 68, 69, 71, 61, 62, 63, 64, 74,
    59, 70, 72, 73, 75, 76, 35, 65, 66,
};

// GLOBAL: CARMA2_HW 0x006883c0
int gAuthor_credits_line_count;

// GLOBAL: CARMA2_HW 0x00686f04
int* gAuthor_credits_heights;

// GLOBAL: CARMA2_HW 0x00687238
int* gAuthor_credits_throbs;

// GLOBAL: CARMA2_HW 0x00688448
char** gAuthor_credits_texts;

// GLOBAL: CARMA2_HW 0x0068682c
int* gAuthor_credits_fonts;

// GLOBAL: CARMA2_HW 0x00686f90
int gAuthor_credits_total_height;

// GLOBAL: CARMA2_HW 0x00763898
int gFrontend_scroll_time_left;

// GLOBAL: CARMA2_HW 0x0076389c
int gFrontend_scroll_time_increment;

// GLOBAL: CARMA2_HW 0x00763894
tU32 gFrontend_scroll_last_update;

#define COUNT_FRONTEND_INTERPOLATE_STEPS 16

// FUNCTION: CARMA2_HW 0x00484fd0
void C2_HOOK_FASTCALL IString_Load(void) {
    tPath_name the_path;
    FILE* f;
    char s[256];
    int i;

    if (gCount_interface_strings != 0) {
        return;
    }
    PathCat(the_path, gApplication_path, "INTRFACE");
    PathCat(the_path, the_path, "interface.txt");
    f = PFfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    gCount_interface_strings = 0;
    for (i = 0; !PFfeof(f) && i < REC2_ASIZE(gInterface_strings); i++) {
        GetALineAndDontArgue(f, s);
        gInterface_strings[i] = BrMemAllocate(strlen(s) + 1, kMem_misc_string);
        strcpy(gInterface_strings[i], s);
        gCount_interface_strings++;
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x004850d0
void C2_HOOK_FASTCALL FreeInterfaceStrings(void) {
    int i;

    for (i = 0; i < gCount_interface_strings; i++) {
        BrMemFree(gInterface_strings[i]);;
    }
    gCount_interface_strings = 0;
}

// FUNCTION: CARMA2_HW 0x00485110
const char* C2_HOOK_FASTCALL IString_Get(int pIndex) {

    if (pIndex > gCount_interface_strings) {
        return NULL;
    }
    return gInterface_strings[pIndex];
}

void C2_HOOK_FASTCALL LoadMenuImages(void) {
    int i;
    char s[256];
    char s2[256];
    FILE* f;
    int count;

    /* 1 to 6 are car images, 7 is driver image */
    for (i = 1; i < 8; i++) { /* FIXME: magic number */
        gFrontend_images[i] = LoadPixelmap("64by64.tif");
    }
    PathCat(s, gApplication_path, "INTRFACE");
    PathCat(s, s, "MENUIMAGES.TXT");
    /* menuimages.txt -  Menu button images, indexed from 1 */
    f = DRfopen(s, "rt");
    if (f == NULL) {
        FatalError(kFatalError_Mysterious_SS, "", "");
    }
    /* number of images */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s2);
        strcpy(s, s2);
        strcat(s, ".TIF");
        gFrontend_images[i + 8] = LoadPixelmap(s); /* FIXME: magic number */
    }

#if defined(REC2_FIX_BUGS)
    PFfclose(f);
#endif
}

// FUNCTION: CARMA2_HW 0x0046cf10
void C2_HOOK_FASTCALL FRONTEND_Setup2D(void) {
    br_camera* camera;

    gFrontend_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    gFrontend_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = gFrontend_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BrDegreeToAngle(45); /* 0x1ffe */
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;
    BrActorAdd(gFrontend_actor, gFrontend_camera);
    gFrontend_backdrop0_material_prims[0].t = BRT_BLEND_B;
    gFrontend_backdrop0_material_prims[0].v.b = 1;
    gFrontend_backdrop0_material_prims[1].t = BRT_OPACITY_X;
    gFrontend_backdrop0_material_prims[1].v.x = 0xb00000;
    gFrontend_backdrop0_material_prims[2].t = BR_NULL_TOKEN;
    gFrontend_backdrop0_material_prims[2].v.u32 = 0;
    gFrontend_backdrop1_material_prims[0].t = BRT_BLEND_B;
    gFrontend_backdrop1_material_prims[0].v.b = 1;
    gFrontend_backdrop1_material_prims[1].t = BRT_OPACITY_X;
    gFrontend_backdrop1_material_prims[1].v.x = 0xb00000;
    gFrontend_backdrop1_material_prims[2].t = BR_NULL_TOKEN;
    gFrontend_backdrop1_material_prims[2].v.u32 = 0;
    gFrontend_backdrop2_material_prims[0].t = BRT_BLEND_B;
    gFrontend_backdrop2_material_prims[0].v.b = 1;
    gFrontend_backdrop2_material_prims[1].t = BRT_OPACITY_X;
    gFrontend_backdrop2_material_prims[1].v.x = 0xb00000;
    gFrontend_backdrop2_material_prims[2].t = BR_NULL_TOKEN;
    gFrontend_backdrop2_material_prims[2].v.u32 = 0;
    gFrontend_backdrop_materials[0] = BrMaterialAllocate("Backdrop_material1");
    gFrontend_backdrop_materials[1] = BrMaterialAllocate("Backdrop_material2");
    gFrontend_backdrop_materials[2] = BrMaterialAllocate("Backdrop_material3");
    gFrontend_backdrop_actors[0] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gFrontend_backdrop_actors[1] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gFrontend_backdrop_actors[2] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gFrontend_backdrop_materials[0]->colour = 0;
    gFrontend_backdrop_materials[1]->colour = 0;
    gFrontend_backdrop_materials[2]->colour = 0;
    gFrontend_backdrop_materials[0]->index_base = 0;
    gFrontend_backdrop_materials[1]->index_base = 0;
    gFrontend_backdrop_materials[2]->index_base = 0;
    gFrontend_backdrop_materials[0]->index_range = 1;
    gFrontend_backdrop_materials[1]->index_range = 1;
    gFrontend_backdrop_materials[2]->index_range = 1;
    gFrontend_backdrop_materials[0]->extra_prim = gFrontend_backdrop0_material_prims;
    gFrontend_backdrop_materials[1]->extra_prim = gFrontend_backdrop1_material_prims;
    gFrontend_backdrop_materials[2]->extra_prim = gFrontend_backdrop2_material_prims;
    BrMaterialAdd(gFrontend_backdrop_materials[0]);
    BrMaterialAdd(gFrontend_backdrop_materials[1]);
    BrMaterialAdd(gFrontend_backdrop_materials[2]);
    gFrontend_backdrop_actors[0]->material = gFrontend_backdrop_materials[0];
    gFrontend_backdrop_actors[0]->identifier = BrResStrDup(gFrontend_backdrop_actors[0], "Backdrop");;
    gFrontend_backdrop_actors[1]->material = gFrontend_backdrop_materials[1];
    gFrontend_backdrop_actors[1]->identifier = BrResStrDup(gFrontend_backdrop_actors[1], "Backdrop");
    gFrontend_backdrop_actors[2]->material = gFrontend_backdrop_materials[2];
    gFrontend_backdrop_actors[2]->identifier = BrResStrDup(gFrontend_backdrop_actors[2], "Backdrop");
    BrActorAdd(gFrontend_actor, gFrontend_backdrop_actors[0]);
    BrActorAdd(gFrontend_actor, gFrontend_backdrop_actors[1]);
    BrActorAdd(gFrontend_actor, gFrontend_backdrop_actors[2]);
}

static void C2_HOOK_FASTCALL LoadMenuModels(void) {
    char s[256];
    char s2[256];
    char s3[256];
    FILE* f;
    int count;
    int i;

    PathCat(s, gApplication_path, "INTRFACE");
    PathCat(s2, s, "MENUMODELS.TXT");
    f = DRfopen(s2, "rt");
    if (f == NULL) {
        FatalError(kFatalError_Mysterious_SS, "", "");
    }

    C2_HOOK_BUG_ON(sizeof(tFrontend_model) != 16);
    PathCat(s, gApplication_path, "INTRFACE");
    /* number of A models */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s3);
        PathCat(s2, s, s3);
        strcat(s2, ".DAT");
        gFrontend_A_models[i].model = BrModelLoad(s2);
        gFrontend_A_models[i].model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(gFrontend_A_models[i].model);
        if (i == 0) {
            gFrontend_backdrop_actors[0]->model = BrModelLoad(s2);
            gFrontend_backdrop_actors[0]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(gFrontend_backdrop_actors[0]->model);
            BrMatrix34Translate(&gFrontend_backdrop_actors[0]->t.t.mat, 300.f, -250.f, -2.5f);
            BrMatrix34PreScale(&gFrontend_backdrop_actors[0]->t.t.mat, 1.25f, 1.25f, 1.0f);
            gFrontend_backdrop_actors[0]->render_style = BR_RSTYLE_FACES;
        }
    }
    /* number of B models */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s3);
        PathCat(s2, s, s3);
        strcat(s2, ".DAT");
        gFrontend_B_models[i].model = BrModelLoad(s2);
        gFrontend_B_models[i].model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(gFrontend_B_models[i].model);
        if (i == 0) {
            gFrontend_backdrop_actors[1]->model = BrModelLoad(s2);
            gFrontend_backdrop_actors[1]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(gFrontend_backdrop_actors[1]->model);
            BrMatrix34Translate(&gFrontend_backdrop_actors[1]->t.t.mat, 300.f, -250.f, -2.5f);
            BrMatrix34PreScale(&gFrontend_backdrop_actors[1]->t.t.mat, 1.25f, 1.25f, 1.0f);
            gFrontend_backdrop_actors[1]->render_style = BR_RSTYLE_FACES;
        }
    }
    /* number of C models */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s3);
        PathCat(s2, s, s3);
        strcat(s2, ".DAT");
        gFrontend_C_models[i].model = BrModelLoad(s2);
        gFrontend_C_models[i].model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(gFrontend_C_models[i].model);
        if (i == 0) {
            gFrontend_backdrop_actors[2]->model = BrModelLoad(s2);
            gFrontend_backdrop_actors[2]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(gFrontend_backdrop_actors[2]->model);
            BrMatrix34Translate(&gFrontend_backdrop_actors[2]->t.t.mat, 300.f, -250.f, -2.5f);
            BrMatrix34PreScale(&gFrontend_backdrop_actors[2]->t.t.mat, 1.25f, 1.25f, 1.0f);
            gFrontend_backdrop_actors[2]->render_style = BR_RSTYLE_FACES;
        }
    }

#if defined(REC2_FIX_BUGS)
    PFfclose(f);
#endif
}

// FUNCTION: CARMA2_HW 0x0046abf0
br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmapPleaseMrTwatter(const char* pFolder, const char* pName) {
    br_pixelmap* pixelmaps[1000];
    br_pixelmap* result;
    FILE* f;
    tPath_name the_path;
    char* str;
    int i;
    int count;

    result = NULL;
    PathCat(the_path, pFolder, "PIXIES.P16");
    f = PFfopen(the_path, "rb");
    if (f == NULL) {
        PathCat(the_path, pFolder, pName);
        result = DRImageLoad(the_path);
        /* FUN_005193f0(result, 0); */
        return result;
    }
    PFfclose(f);
    count = BrPixelmapLoadMany(the_path, pixelmaps, REC2_ASIZE(pixelmaps));
    strcpy(the_path, pName);
    str = strchr(the_path, '.');
    *str = '\0';
    for (i = 0; i < count; i++) {
        if (pixelmaps[i] != NULL) {
            if (DRStricmp(pixelmaps[i]->identifier, the_path) == 0) {
                result = pixelmaps[i];
            } else {
                BrPixelmapFree(pixelmaps[i]);
                pixelmaps[i] = NULL;
            }
        }
    }
    /* FUN_005193f0(result, 0); */
    return result;
}

// FUNCTION: CARMA2_HW 0x0046c5c0
void C2_HOOK_FASTCALL FRONTEND_CreateMenuButton(tFrontend_brender_item* pFrontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText) {
    br_actor* actor;
    br_model* model;
    br_material* material;
    int dx;
    int dy;
    float map_x;
    float map_y;

    C2_HOOK_BUG_ON(sizeof(tFrontend_brender_item) != 44);

    pFrontend_brender_item->actor = actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    pFrontend_brender_item->model = model = BrModelAllocate("ButtonModel", 4, 2);
    pFrontend_brender_item->material = material = BrMaterialAllocate("ButtonMaterial");
    if (model == NULL || material == NULL || actor == NULL) {
        FatalError(kFatalError_OOM_S, "");
    }
    if (pMap != NULL) {
        pFrontend_brender_item->field_0x10 = BrPixelmapAllocate(BR_PMT_RGBA_4444, pMap->width, pMap->height, NULL, 0);
        BrPixelmapCopy(pFrontend_brender_item->field_0x10, pMap);
        pFrontend_brender_item->field_0xc = BrPixelmapAllocate(BR_PMT_RGBA_4444, pMap->width, pMap->height, NULL, 0);
        BrPixelmapCopy(pFrontend_brender_item->field_0xc, pFrontend_brender_item->field_0x10);
    } else {
        pFrontend_brender_item->field_0x10 = BrPixelmapAllocate(BR_PMT_RGBA_4444, 8, 8, NULL, 0);
        BrPixelmapFill(pFrontend_brender_item->field_0x10, BR_COLOUR_RGBA(0, 0, 0x80, 0));
        pFrontend_brender_item->field_0xc = BrPixelmapAllocate(BR_PMT_RGBA_4444, 8, 8, NULL, 0);
        BrPixelmapCopy(pFrontend_brender_item->field_0xc, pFrontend_brender_item->field_0x10);
        pFrontend_brender_item->field_0xc->identifier = BrResStrDup(pFrontend_brender_item->field_0xc, pText);
    }
    BrMapAdd(pFrontend_brender_item->field_0xc);
    pFrontend_brender_item->prims[0].t = BRT_BLEND_B;
    pFrontend_brender_item->prims[0].v.b = 1;
    pFrontend_brender_item->prims[1].t = BRT_OPACITY_X;
    pFrontend_brender_item->prims[1].v.x = 0x800000;
    pFrontend_brender_item->prims[2].t = BR_NULL_TOKEN;
    pFrontend_brender_item->prims[2].v.u32 = 0;
    material->extra_prim = pFrontend_brender_item->prims;
    actor->model = model;
    actor->material = material;
    actor->identifier = BrResStrDup(actor, "Button");;
    BrMaterialAdd(material);
    material->colour = pColour;
    material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    material->colour_map = pFrontend_brender_item->field_0xc;
    BrMaterialUpdate(material, BR_MATU_ALL);
    model->flags |= BR_MODF_KEEP_ORIGINAL;

    model->vertices[0].p.v[0] = model->vertices[1].p.v[0] = (float)pX;
    model->vertices[0].p.v[1] = model->vertices[3].p.v[1] = -(float)pY;
    model->vertices[2].p.v[0] = model->vertices[3].p.v[0] = model->vertices[1].p.v[0] + (float)pWidth;
    model->vertices[1].p.v[1] = model->vertices[2].p.v[1] = model->vertices[3].p.v[1] - (float)pHeight;
    model->vertices[0].p.v[2] = -1.1f;
    model->vertices[1].p.v[2] = -1.1f;
    model->vertices[2].p.v[2] = -1.1f;
    model->vertices[3].p.v[2] = -1.1f;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 2;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 0;
    BrModelAdd(model);
    if (pMap != NULL) {
        dx = HighResHeadupWidth(pMap->width);
        dy = HighResHeadupWidth(pMap->height);
    } else {
        dx = HighResHeadupWidth(8);
        dy = HighResHeadupWidth(8);
    }
    map_x = (float)pWidth / (float)dx;
    map_y = (float)pHeight / (float)dy;
    BrVector2Set(&model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&model->vertices[1].map, 0.f, map_y);
    BrVector2Set(&model->vertices[2].map, map_x, map_y);
    BrVector2Set(&model->vertices[3].map, map_x, 0.f);
    BrModelUpdate(model, BR_MODU_ALL);
    BrActorAdd(gFrontend_actor, actor);
    actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x0046c970
int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec) {
    char s[256];
    char s2[256];
    const char* name;
    int i;

    sprintf(s, "START OF FRONTEND_CreateMenu for menu \'%s\'", pFrontend_spec->name);
    PrintMemoryDump(0, s);
    if (pFrontend_spec->create != NULL) {
        pFrontend_spec->create(pFrontend_spec);
    }
    gFrontend_count_brender_items = 0;
    name = pFrontend_spec->backdrop_name;
    if (name != NULL && strlen(name) != 0) {
        if (!gFrontend_remove_current_backdrop) {
            tTWTVFS twt;

            PathCat(s2, gApplication_path, "INTRFACE");
            PathCat(s2, s2, "BACKDROP");
            strcpy(s, name);
            C2_HOOK_ASSERT(s[strlen(s) - 4] == '.');
            s[strlen(s) - 4] = '\0';
            PathCat(s2, s2, s);
            twt = OpenPackFileAndSetTiffLoading(s2);
            gFrontend_backdrop = GetThisFuckingPixelmapPleaseMrTwatter(s2, name);
            ClosePackFileAndSetTiffLoading(twt);

            if (gFrontend_backdrop == NULL) {
                BrFailure("FRONTEND: Error loading background graphics.", 0);
            }
            /* FUN_005191f0(gFrontend_backdrop); */
            BrMapAdd(gFrontend_backdrop);
        } else if (gFrontend_backdrop != NULL) {
            tTWTVFS twt;

            BrMapRemove(gFrontend_backdrop);
            BrPixelmapFree(gFrontend_backdrop);

            PathCat(s2, gApplication_path, "INTRFACE");
            PathCat(s2, s2, "BACKDROP");
            strcpy(s, name);
            C2_HOOK_ASSERT(s[strlen(s) - 4] == '.');
            s[strlen(s) - 4] = '\0';
            PathCat(s2, s2, s);
            twt = OpenPackFileAndSetTiffLoading(s2);
            gFrontend_backdrop = GetThisFuckingPixelmapPleaseMrTwatter(s2, name);
            ClosePackFileAndSetTiffLoading(twt);

            if (gFrontend_backdrop == NULL) {
                BrFailure("FRONTEND: Error loading background graphics.", 0);
            }
            PixelmapSwapByteOrder(gFrontend_backdrop);
            BrMapAdd(gFrontend_backdrop);
        }
    }
    StartMouseCursor();
    for (i = 0; i < pFrontend_spec->count_items; i++) {
        tFrontend_item_spec* item = &pFrontend_spec->items[i];
        tS16 y;
        tS16 width;
        tS16 height;

        if (i == gFrontend_selected_item_index) {
            width = item->width;
            height = item->height;
            y = item->y;
        }
        else {
            width = item->width;
            height = item->height;
            y = item->y;
        }
        FRONTEND_CreateMenuButton(
            &gFrontend_brender_items[gFrontend_count_brender_items],
            (tS16)item->x,
            y,
            width,
            height,
            gFrontend_some_color,
            item->map_index == 0 ? NULL : gFrontend_images[item->map_index],
            item->text);
        gFrontend_count_brender_items++;
    }
    ResetInterfaceTimeout();
    FRONTEND_CreateMenuButton(&gFrontend_brender_items[99],
        0,
        0,
        0,
        0,
        gFrontend_some_color,
        NULL,
        "");
    gFrontend_selected_item_index = gMouse_in_use ? 99 : 0;
    sprintf(s, "END OF FRONTEND_CreateMenu for menu \'%s\'", pFrontend_spec->name);
    PrintMemoryDump(0, s);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046d1c0
void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pType) {

    PrintMemoryDump(0, "START OF FRONTEND_Setup");
    LoadInterfaceStuff(gProgram_state.racing);
    if (gFrontend_stuff_not_loaded) {
        IString_Load();
        InitPolyFonts();
        LoadMenuImages();
        PrintMemoryDump(0, "AFTER LoadMenuImages");
        FRONTEND_Setup2D();
        LoadMenuModels();
        PrintMemoryDump(0, "AFTER LoadMenuModels");
        gFrontend_stuff_not_loaded = 0;
    }
    switch (pType) {
    case kFrontend_menu_main:
        FRONTEND_CreateMenu(&gFrontend_MAIN);
        gCurrent_frontend_spec = &gFrontend_MAIN;
        break;
    case kFrontend_menu_options:
        FRONTEND_CreateMenu(&gFrontend_OPTIONS);
        gCurrent_frontend_spec = &gFrontend_OPTIONS;
        break;
    case kFrontend_menu_wrecks:
        FRONTEND_CreateMenu(&gFrontend_WRECKS);
        gCurrent_frontend_spec = &gFrontend_WRECKS;
        break;
    case kFrontend_menu_netsync:
        FRONTEND_CreateMenu(&gFrontend_NETSYNC);
        gCurrent_frontend_spec = &gFrontend_NETSYNC;
        break;
    case kFrontend_menu_networksummary:
        FRONTEND_CreateMenu(&gFrontend_NETWORK_SUMMARY);
        gCurrent_frontend_spec = &gFrontend_NETWORK_SUMMARY;
        break;
    case kFrontend_menu_credits:
        FRONTEND_CreateMenu(&gFrontend_CREDITS);
        gCurrent_frontend_spec = &gFrontend_CREDITS;
        break;
    case kFrontend_menu_newgame:
        FRONTEND_CreateMenu(&gFrontend_NEWGAME);
        gCurrent_frontend_spec = &gFrontend_NEWGAME;
        break;
    }
    PrintMemoryDump(0, "AFTER FRONTEND_CreateMenu");

    gCurrent_frontend_spec->previous = NULL;
    gFrontend_A_model_from = gFrontend_A_models[0].model;
    gFrontend_A_model_to = gFrontend_A_models[gCurrent_frontend_spec->model_A_index].model;
    gFrontend_B_model_from = gFrontend_B_models[0].model;
    gFrontend_B_model_to = gFrontend_B_models[gCurrent_frontend_spec->model_B_index].model;
    gFrontend_C_model_from = gFrontend_C_models[0].model;
    gFrontend_C_model_to = gFrontend_C_models[gCurrent_frontend_spec->model_C_index].model;
    gFrontend_backdrop0_opacity_mode = gCurrent_frontend_spec->model_A_index != 0 ? 1 : -2;
    gFrontend_backdrop1_opacity_mode = gCurrent_frontend_spec->model_B_index != 0 ? 1 : -2;
    gFrontend_backdrop2_opacity_mode = gCurrent_frontend_spec->model_C_index != 0 ? 1 : -1;
    gFrontend_interpolate_steps_left = COUNT_FRONTEND_INTERPOLATE_STEPS;
    PrintMemoryDump(0, "END OF FRONTEND_Setup");
}

int C2_HOOK_FASTCALL FRONTEND_Redraw(void) {
    int i;

    gBack_screen->origin_x = 0;
    gBack_screen->origin_y = 0;
    if (gFrontend_backdrop != NULL) {

        DrPixelmapRectangleCopyPossibleLock(gBack_screen, 0, 0,
            gFrontend_backdrop, 0, 0, gFrontend_backdrop->width, gFrontend_backdrop->height);
    }
    BrPixelmapFill(gDepth_buffer, 0xffffffff);
    for (i = 0; i < gFrontend_count_brender_items; i++) {
        br_actor* actor;

        actor = gFrontend_brender_items[i].actor;
        if (gCurrent_frontend_spec->items[i].visible) {
            actor->render_style = BR_RSTYLE_FACES;
        } else {
            actor->render_style = BR_RSTYLE_NONE;
        }
    }
    BrZbsSceneRender(gFrontend_actor, gFrontend_camera, gBack_screen, gDepth_buffer);
    FRONTEND_DrawMenu(gCurrent_frontend_spec);
    MaybeDoMouseCursor();
    PDScreenBufferSwap(0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046f630
void C2_HOOK_FASTCALL ScrollCredits(void) {
    int i;
    int y = (int)(430.f - 0.03f * (float)(PDGetTotalTime() - gAuthor_credits_scroll_start_time));

    for (i = 0; i < gAuthor_credits_line_count; i++) {
        y += gAuthor_credits_heights[i];
        if (y > 30 && y < 430) {
            if (gAuthor_credits_throbs[i]) {
                SolidPolyFontText(gAuthor_credits_texts[i], 320, y, gAuthor_credits_fonts[i] - 1, eJust_centre, 1);
                TransparentPolyFontText(gAuthor_credits_texts[i], 320, y, gAuthor_credits_fonts[i], eJust_centre, 1, gFrontend_throb_factor);
            } else {
                SolidPolyFontText(gAuthor_credits_texts[i], 320, y, gAuthor_credits_fonts[i], eJust_centre, 1);
            }
        }
    }
    if (gAuthor_credits_total_height < 0.03f * (PDGetTotalTime() - gAuthor_credits_scroll_start_time)) {
        gAuthor_credits_scroll_start_time = PDGetTotalTime();
    }
}

void C2_HOOK_FASTCALL ResetInterfaceTimeout(void) {

    gFrontend_time_last_input = PDGetTotalTime();
}

void C2_HOOK_FASTCALL Generic_LinkInEffect(void) {
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
}

void C2_HOOK_FASTCALL Generic_LinkOutEffect(void) {
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
}

// FUNCTION: CARMA2_HW 0x004709b0
void C2_HOOK_FASTCALL BuildAPO(int pCurrent, int pPotential, int pActorIdx, int pAPO) {
    int i;
    br_pixelmap* map;

    map = gFrontend_billboard_actors[pActorIdx]->material->colour_map;

    if (pAPO == 0) {
        BrPixelmapFill(map, 0);
    }

    for (i = 0; i < 30; i++) {
        br_uint_32 c;

        if (i >= pPotential) {
            c = gFrontend_APO_Colour_2;
        } else if (i < pCurrent) {
            c = gFrontend_APO_Colour_1;
        } else {
            c = gFrontend_APO_Colour_3;
        }

        BrPixelmapRectangleFill(map, 4 * (i % 10), 0 + 4 * (i / 10) + 12 * pAPO, 3, 3, c);
    }
}

// FUNCTION: CARMA2_HW 0x00470860
void C2_HOOK_FASTCALL PrepareAPO(int pActorIdx) {

    BrMapUpdate(gFrontend_billboard_actors[pActorIdx]->material->colour_map, BR_MAPU_ALL);
    BrMaterialUpdate(gFrontend_billboard_actors[pActorIdx]->material, BR_MATU_ALL);
    BrModelUpdate(gFrontend_billboard_actors[pActorIdx]->model, BR_MODU_VERTICES);
}

// FUNCTION: CARMA2_HW 0x0046d8e0
int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontendType) {
    // GLOBAL: CARMA2_HW 0x0076370c
    static int back_screen_base_x;
    // GLOBAL: CARMA2_HW 0x00763710
    static int back_screen_base_y;
    // GLOBAL: CARMA2_HW 0x00763704
    static int back_screen_origin_x;
    // GLOBAL: CARMA2_HW 0x00763708
    static int back_screen_origin_y;

    if (gSound_enabled) {
        DRS3StopAllOutletSoundsExceptCDA();
    }
    SwitchToHiresMode();
    WaitForNoKeys();
    gFrontend_remove_current_backdrop = 0;
    TurnTintedPolyOff(gHud_tinted1);
    TurnTintedPolyOff(gHud_tinted2);
    TurnTintedPolyOff(gHud_tinted3);
    FRONTEND_Setup(pFrontendType);
    gFrontend_remove_current_backdrop = 1;
    gFrontend_leave_current_menu = 0;
    ResetInterfaceTimeout();
    Generic_LinkInEffect();
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_spec, unknownLastInt, 0xb8c4);
    gCurrent_frontend_spec->unknownLastInt = 0;

    for (;;) {
        gBack_screen->origin_x = 0;
        gBack_screen->origin_y = 0;
        if (gFrontend_backdrop != NULL) {
            DRPixelmapRectangleCopy(gBack_screen, 0, 0,
                gFrontend_backdrop, 0, 0, gFrontend_backdrop->width, gFrontend_backdrop->height);
        }
        if (gCurrent_frontend_spec != &gFrontend_LOAD_GAME &&
              gCurrent_frontend_spec != &gFrontend_NETSYNC &&
              gCurrent_frontend_spec != &gFrontend_NETWORK_SUMMARY &&
              gFrontend_interpolate_steps_left == 0) {
            gCurrent_frontend_spec->unknownLastInt = FRONTEND_GenericMenuHandler(gCurrent_frontend_spec);
        }
        BrPixelmapFill(gDepth_buffer, 0xffffffff);
        gBack_screen->origin_x = 0;
        gBack_screen->origin_y = 0;

        if (gFrontend_interpolate_steps_left > 0) {
            int step = 16 - gFrontend_interpolate_steps_left + 1;

            MorphBlob(gFrontend_A_model_from, gFrontend_A_model_to, gFrontend_backdrop_actors[0]->model, step, 16);
            if (gFrontend_backdrop0_opacity_mode == -1) {
                gFrontend_backdrop0_material_prims[1].v.x = BR_FIXED_INT(176 * (16 - step)  / 16);
            } else if (gFrontend_backdrop0_opacity_mode == 1) {
                gFrontend_backdrop0_material_prims[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (gFrontend_backdrop0_opacity_mode == -2) {
                gFrontend_backdrop0_material_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(gFrontend_backdrop_materials[0], BR_MATU_ALL);

            MorphBlob(gFrontend_B_model_from, gFrontend_B_model_to, gFrontend_backdrop_actors[1]->model, step, 16);
            if (gFrontend_backdrop1_opacity_mode == -1) {
                gFrontend_backdrop1_material_prims[1].v.x = BR_FIXED_INT(176 * (16 - step)  / 16);
            } else if (gFrontend_backdrop1_opacity_mode == 1) {
                gFrontend_backdrop1_material_prims[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (gFrontend_backdrop1_opacity_mode == -2) {
                gFrontend_backdrop1_material_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(gFrontend_backdrop_materials[1], BR_MATU_ALL);

            MorphBlob(gFrontend_C_model_from, gFrontend_C_model_to, gFrontend_backdrop_actors[2]->model, step, 16);
            if (gFrontend_backdrop2_opacity_mode == -1) {
                gFrontend_backdrop2_material_prims[1].v.x = BR_FIXED_INT(176 * (16 - step)  / 16);
            } else if (gFrontend_backdrop2_opacity_mode == 1) {
                gFrontend_backdrop2_material_prims[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (gFrontend_backdrop2_opacity_mode == -2) {
                gFrontend_backdrop2_material_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(gFrontend_backdrop_materials[2], BR_MATU_ALL);
        }
        BrZbsSceneRender(gFrontend_actor, gFrontend_camera, gBack_screen, gDepth_buffer);

        if (gCurrent_frontend_spec == &gFrontend_LOAD_GAME ||
                gCurrent_frontend_spec == &gFrontend_NETSYNC ||
                gCurrent_frontend_spec == &gFrontend_NETWORK_SUMMARY) {
            if (gFrontend_interpolate_steps_left == 0) {
                gCurrent_frontend_spec->unknownLastInt = FRONTEND_GenericMenuHandler(gCurrent_frontend_spec);
            }
        }
        if (gFrontend_interpolate_steps_left != 0) {
            gFrontend_interpolate_steps_left -= 1;
        } else {
            if (gCurrent_frontend_spec == &gFrontend_WRECKS && gFrontend_wrecks_actor != NULL) {
                back_screen_base_x = gBack_screen->base_x;
                back_screen_base_y = gBack_screen->base_y;
                back_screen_origin_x = gBack_screen->origin_x;
                back_screen_origin_y = gBack_screen->origin_y;
                gBack_screen->base_x = 0;
                gBack_screen->base_y = 0;
                gBack_screen->origin_x = 320;
                gBack_screen->origin_y = 120;
                BrZbsSceneRender(gFrontend_wrecks_actor, gFrontend_wrecks_camera, gFrontend_wrecks_pixelmap, gDepth_buffer);
                gBack_screen->base_x = back_screen_base_x;
                gBack_screen->base_y = back_screen_base_y;
                gBack_screen->origin_x = back_screen_origin_x;
                gBack_screen->origin_y = back_screen_origin_y;
            }
            if (gCurrent_frontend_spec == &gFrontend_MAIN && gFrontend_menu_camera != NULL) {
                BuildAPO(gCurrent_APO_levels[0], gCurrent_APO_potential_levels[0], 0, 0);
                BuildAPO(gCurrent_APO_levels[1], gCurrent_APO_potential_levels[1], 0, 1);
                BuildAPO(gCurrent_APO_levels[2], gCurrent_APO_potential_levels[2], 0, 2);

                PrepareAPO(0);
                PrintAPO( 95, 348, 0, 0);
                PrintAPO(146, 348, 0, 1);
                PrintAPO(195, 348, 0, 2);
            }
            if (gCurrent_frontend_spec == &gFrontend_CREDITS) {
                ScrollCredits();
            }
            FRONTEND_DrawMenu(gCurrent_frontend_spec);
            if (gCurrent_frontend_spec->unknownLastInt == 0) {
                MaybeDoMouseCursor();
            }
        }
        PDScreenBufferSwap(0);
        if (gCurrent_frontend_spec->unknownLastInt == 1 && gCurrent_frontend_spec != &gFrontend_START_GAME) {
            FRONTEND_DestroyMenu(gCurrent_frontend_spec);
            strcpy(gFrontend_START_GAME.backdrop_name, gCurrent_frontend_spec->backdrop_name);
            Morph_Initialise(gCurrent_frontend_spec, &gFrontend_START_GAME);
            gCurrent_frontend_spec = &gFrontend_START_GAME;
            FRONTEND_CreateMenu(&gFrontend_START_GAME);
        }
        if (gCurrent_frontend_spec->unknownLastInt == 3) {
            FRONTEND_DestroyMenu(gCurrent_frontend_spec);
            FRONTEND_Setup(gFrontend_next_menu);
            gCurrent_frontend_spec->unknownLastInt = 0;
        }
        if (gCurrent_frontend_spec->unknownLastInt != 0) {
            FRONTEND_DestroyMenu(gCurrent_frontend_spec);
            if (gFrontend_backdrop != NULL) {
                BrMapRemove(gFrontend_backdrop);
                BrPixelmapFree(gFrontend_backdrop);
            }
            DisposeInterfaceFonts();
            return gCurrent_frontend_spec->unknownLastInt != 2;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004708a0
void C2_HOOK_FASTCALL PrintAPO(int pX, int pY, int pIndex, int pTex_index) {
    br_model *model;

    model = gFrontend_billboard_actors[pIndex]->model;
    BrVector2Set(&model->vertices[0].map, 0.f, pTex_index * .1875f);
    BrVector2Set(&model->vertices[1].map, 0.625f, pTex_index * .1875f);
    BrVector2Set(&model->vertices[2].map, 0.f, (float)(pTex_index + 1) * .1875f);
    BrVector2Set(&model->vertices[3].map, 0.625f, (float)(pTex_index + 1) * .1875f);
    BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
    BrVector3Set(&gFrontend_billboard_actors[pIndex]->t.t.translate.t, (float)pX, (float)-pY, 0.f);
    BrActorAdd(gFrontend_menu_camera, gFrontend_billboard_actors[pIndex]);
    BrZbSceneRender(gFrontend_menu_camera, gFrontend_menu_camera, gBack_screen, gDepth_buffer);
    BrActorRemove(gFrontend_billboard_actors[pIndex]);
}

// FUNCTION: CARMA2_HW 0x0046cec0
void C2_HOOK_FASTCALL FRONTEND_PingPongFlash(void) {
    tU32 time;

    time = PDGetTotalTime() % 750;

    if (time < 375) {
        gFrontend_throb_factor = (double)time / 375.;
    } else {
        gFrontend_throb_factor = (double)(750 - time) / 375.;
    }
}

// FUNCTION: CARMA2_HW 0x0046f5b0
void C2_HOOK_FASTCALL MorphBlob(br_model* pModel_from, br_model* pModel_to, br_model* pModel, int pStep, int pCount_steps) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {

        pModel->vertices[i].p.v[0] = pModel_from->vertices[i].p.v[0] + (pModel_to->vertices[i].p.v[0] - pModel_from->vertices[i].p.v[0]) * ((float)pStep / (float)pCount_steps);
        pModel->vertices[i].p.v[1] = pModel_from->vertices[i].p.v[1] + (pModel_to->vertices[i].p.v[1] - pModel_from->vertices[i].p.v[1]) * ((float)pStep / (float)pCount_steps);
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x004666f0
void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_spec, count_items, 0x104);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_spec, items, 0x130);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_item_spec, stringId, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_item_spec, width, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_item_spec, height, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_item_spec, wrapText, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_item_spec, unlitFont, 0x10);

    for (i = 0; i < pFrontend->count_items; i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];
        const char* text = NULL;

        if (item->stringId <= 0x400) {
            text = IString_Get(item->stringId);
        } else if (item->stringId == 0x404) {
            text = item->text;
        }
        if (text != NULL && !item->wrapText) {
            item->width = PolyFontTextWidth(item->unlitFont, text);
            item->height = PolyFontHeight(item->unlitFont);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00466ce0
void C2_HOOK_FASTCALL MungeButtonModels(tFrontend_spec* pFrontend, int pButton_index) {
    int i;

    tFrontend_scroller_spec* scroller = &pFrontend->scrollers[pIndex];

    for (i = 0; i < scroller->nbDisplayedAtOnce; i++) {
        int item_idx = scroller->indexFirstScrollableItem + i;
        br_model* model = gFrontend_brender_items[item_idx].model;

        model->vertices[3].p.v[0] = model->vertices[1].p.v[0] + pFrontend->items[item_idx].width;
        model->vertices[2].p.v[0] = model->vertices[3].p.v[0];
        model->vertices[2].p.v[1] = model->vertices[3].p.v[1] - pFrontend->items[item_idx].height;
        model->vertices[1].p.v[1] = model->vertices[2].p.v[1];
        BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);
    }
}

// FUNCTION: CARMA2_HW 0x00467a70
void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend) {

    if (gCurrent_frontend_spec == &gFrontend_MAIN) {
        RefreshRacesScroller(pFrontend);
    }
    if (gCurrent_frontend_spec == &gFrontend_NETWORK) {
        RefreshNetRacesScroller(pFrontend);
    }
}

// FUNCTION: CARMA2_HW 0x00467b30
void C2_HOOK_FASTCALL RefreshRacesScroller(tFrontend_spec* pFrontend) {
    char group_text[12];
    int group;
    int i;
    int race_i;

    group = 1 + (gCurrent_race_group - gRaceGroups) % 10;
    sprintf(group_text, "%s %d", IString_Get(78), group);
    strcpy(pFrontend->items[2].text, group_text);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_scroller_spec, indexFirstScrollableItem, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_scroller_spec, indexLastScrollableItem, 0x14);

    race_i = 0;
    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i < pFrontend->scrollers[0].indexLastScrollableItem; i++, race_i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        strcpy(item->text, gRace_list[4 * (group - 1) + race_i].name);
        item->radioButton_selected = race_i == gProgram_state.current_race_index;
        if (gRace_list[race_i].is_boundary) {
            item->unlitFont = 2;
            item->highFont = 2;
        } else {
            item->unlitFont = 0;
            item->highFont = 1;
        }
    }
    FuckWithWidths(pFrontend);
    if (gIs_boundary_race || gProgram_state.game_completed) {
        pFrontend->items[7].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[7].enabled = kFrontendItemEnabled_disabled;
    }
    MungeButtonModels(pFrontend, 0);
}

// FUNCTION: CARMA2_HW 0x004677d0
int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY) {
    int i;

    for (i = 0; i < pFrontend->count_items; i++) {
        br_model *model = gFrontend_brender_items[i].model;
        tFrontend_item_spec *item = &pFrontend->items[i];

        if (pX >= model->vertices[1].p.v[0]
                && pX <= model->vertices[3].p.v[0]
                && pY >= -model->vertices[0].p.v[1]
                && pY <= -model->vertices[1].p.v[1]
                && item->enabled > 0 /* default or disabled */
                && item->visible) {
            return i;
        }
    }
    return -1;
}

int C2_HOOK_FASTCALL FindNextActiveItem(tFrontend_spec* pFrontend, int pStart_index) {
    int i;

    for (i = pStart_index + 1; i < pFrontend->count_items; i++) {
        tFrontend_item_spec *item = &pFrontend->items[i];

        if (item->enabled > 0 && item->visible) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467a30
int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index) {
    int i;

    for (i = pStart_index - 1; i >= 0; i--) {
        tFrontend_item_spec *item = &pFrontend->items[i];

        if (item->enabled > 0 && item->visible) {
            return i;
        }
    }
    return FindPrevActiveItem(pFrontend, pFrontend->count_items);
}

// FUNCTION: CARMA2_HW 0x00467890
void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend) {

    int selected_index = gFrontend_selected_item_index;
    tFrontend_item_spec* selected_item = &pFrontend->items[selected_index];
    int selected_group = selected_item->group;
    if (selected_group != 0) {
        int i;

        for (i = 0; i < pFrontend->count_items; i++) {
            tFrontend_item_spec* item = &pFrontend->items[i];
            if (item->group == selected_group && selected_item->selectable != kFrontendSelectableButton) {
                pFrontend->items[i].radioButton_selected = 0;
            }
        }
        if (selected_item->selectable != kFrontendSelectableButton) {
            selected_item->radioButton_selected = !selected_item->radioButton_selected;
            DRS3StartSound(gEffects_outlet, eSoundId_Done);
        }
    } else if (selected_item->idLevelBar != 0) {
        int i;
        int level_id = selected_item->idLevelBar;

        for (i = pFrontend->levels[level_id - 1].first_item_id; i <= pFrontend->levels[level_id - 1].last_item_id; i++) {
            br_pixelmap* src;

            if (i > selected_index) {
                src = gFrontend_images[5];
            } else {
                src = gFrontend_images[4];
            }
            BrPixelmapCopy(gFrontend_brender_items[i].field_0xc, src);
            BrPixelmapCopy(gFrontend_brender_items[i].field_0x10, src);
            pFrontend->items[i].radioButton_selected = 0;
        }
        pFrontend->items[selected_index].radioButton_selected = 1;
    }
}

// FUNCTION: CARMA2_HW 0x0046ccb0
int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend) {
    char buffer[256];
    br_actor* actor;
    int i;

    sprintf(buffer, "START OF FRONTEND_DestroyMenu for menu '%s'", pFrontend->name);
    PrintMemoryDump(0, buffer);
    if (pFrontend->destroy != NULL) {
        pFrontend->destroy(pFrontend);
    }
    for (actor = gFrontend_actor->children; actor != NULL; actor = gFrontend_actor->children) {
        if (strcmp(actor->identifier, "Backdrop") == 0) {
            break;
        }
        BrActorRemove(actor);
    }
    for (i = 0; i < gFrontend_count_brender_items; i++) {

        BrMaterialRemove(gFrontend_brender_items[i].material);
        BrModelRemove(gFrontend_brender_items[i].model);
        gFrontend_brender_items[i].actor->render_style = BR_RSTYLE_NONE;
        BrMaterialFree(gFrontend_brender_items[i].material);
        gFrontend_brender_items[i].material = NULL;
        BrModelFree(gFrontend_brender_items[i].model);
        gFrontend_brender_items[i].model = NULL;
        BrActorFree(gFrontend_brender_items[i].actor);
        gFrontend_brender_items[i].actor = NULL;
        if (gFrontend_brender_items[i].field_0xc != NULL) {
            BrMapRemove(gFrontend_brender_items[i].field_0xc);
            BrPixelmapFree(gFrontend_brender_items[i].field_0xc);
            gFrontend_brender_items[i].field_0xc = NULL;
        }
        if (gFrontend_brender_items[i].field_0x10 != NULL) {
            BrPixelmapFree(gFrontend_brender_items[i].field_0x10);
            gFrontend_brender_items[i].field_0x10 = NULL;
        }
    }
    /* FIXME: parametrize last item */
    BrMaterialRemove(gFrontend_brender_items[99].material);
    BrModelRemove(gFrontend_brender_items[99].model);
    gFrontend_brender_items[99].actor->render_style = BR_RSTYLE_NONE;
    BrMaterialFree(gFrontend_brender_items[99].material);
    gFrontend_brender_items[99].material = NULL;
    BrModelFree(gFrontend_brender_items[99].model);
    gFrontend_brender_items[99].model = NULL;
    BrActorFree(gFrontend_brender_items[99].actor);
    gFrontend_brender_items[99].actor = NULL;
    EndMouseCursor();
    sprintf(buffer, "END OF FRONTEND_DestroyMenu for menu '%s'", pFrontend->name);
    PrintMemoryDump(0, buffer);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00470a90
void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext) {

    gFrontend_A_model_from = gFrontend_A_models[pCurrent->model_A_index].model;
    gFrontend_A_model_to = gFrontend_A_models[pNext->model_A_index].model;
    if (pCurrent->model_A_index == 0) {
        gFrontend_backdrop0_opacity_mode = pNext->model_A_index != 0 ? 1 : -2;
    } else {
        gFrontend_backdrop0_opacity_mode = pNext->model_A_index != 0 ? 0 : -1;
    }

    gFrontend_B_model_from = gFrontend_B_models[pCurrent->model_B_index].model;
    gFrontend_B_model_to = gFrontend_B_models[pNext->model_B_index].model;
    if (pCurrent->model_B_index == 0) {
        gFrontend_backdrop1_opacity_mode = pNext->model_B_index != 0 ? 1 : -2;
    } else {
        gFrontend_backdrop1_opacity_mode = pNext->model_B_index != 0 ? 0 : -1;
    }

    gFrontend_C_model_from = gFrontend_C_models[pCurrent->model_C_index].model;
    gFrontend_C_model_to = gFrontend_C_models[pNext->model_C_index].model;
    if (pCurrent->model_C_index == 0) {
        gFrontend_backdrop2_opacity_mode = pNext->model_C_index != 0 ? 1 : -2;
    } else {
        gFrontend_backdrop2_opacity_mode = pNext->model_C_index != 0 ? 0 : -1;
    }

    gFrontend_interpolate_steps_left = 16;
}

int C2_HOOK_FASTCALL Frontend_FindNextVisibleItem(tFrontend_spec* pFrontend, int pIndex) {
    int i;

    if (pIndex < pFrontend->count_items - 1) {
        for (i = pIndex + 1; pIndex < pFrontend->count_items; i++) {
            if (pFrontend->items[i].enabled > 0 && pFrontend->items[i].visible) {
                return i;
            }
        }
    } else {
        for (i = 0; i < pFrontend->count_items; i++) {
            if (pFrontend->items[i].enabled > 0 && pFrontend->items[i].visible) {
                return i;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046c0d0
int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend) {
    int original_selected_index;

    original_selected_index = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (pFrontend->tick != NULL) {
        int ret = pFrontend->tick(pFrontend);
        if (ret != 0) {
            return ret;
        }
    } else {
        int the_key;
        int item_under_mouse;
        int mouse_button;

        PollKeys();
        EdgeTriggerModeOff();
        the_key = PDAnyKeyDown();
        if (the_key != -1 && the_key != 4) {
            gMouse_in_use = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        mouse_button = 0;
        item_under_mouse = -1;
        if (gMouse_in_use) {
            int x;
            int y;

            ResetInterfaceTimeout();
            GetMousePosition(&x, &y);
            mouse_button = EitherMouseButtonDown();
            item_under_mouse = GetItemAtMousePos(gCurrent_frontend_spec, x, y);
            if (item_under_mouse != -1) {
                gFrontend_selected_item_index = item_under_mouse;
            } else {
                gFrontend_selected_item_index = 99;
            }
        }

        if (PDKeyDown(72) || PDKeyDown(89)) {
            gFrontend_selected_item_index = FindPrevActiveItem(pFrontend, gFrontend_selected_item_index);
            if (!gMouse_in_use
                    && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                    && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {
                ToggleSelection(pFrontend);
                if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                    pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                }
            }
        }

        if (PDKeyDown(73) || PDKeyDown(83)) {
            gFrontend_selected_item_index = Frontend_FindNextVisibleItem(pFrontend, gFrontend_selected_item_index);
            if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {
                ToggleSelection(pFrontend);
                if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                    pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                }
            }
        }

        if (PDKeyDown(63)) {
            if (pFrontend->previous == NULL) {
                return 2;
            } else {
                int i;

                gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
                FRONTEND_DestroyMenu(pFrontend);
                gCurrent_frontend_spec = pFrontend->previous;
                FRONTEND_CreateMenu(gCurrent_frontend_spec);
                Morph_Initialise(pFrontend, gCurrent_frontend_spec);
                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                    gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
                }
                return 0;
            }
        }

        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button == 1 && item_under_mouse != -1)) {
            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
                return pFrontend->items[gFrontend_selected_item_index].field_0xc;
            }
            if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 1) {
                gFrontend_leave_current_menu = 1;
            }
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
            if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
                int i;

                gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                    gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
                }
                FRONTEND_DestroyMenu(pFrontend);
                gCurrent_frontend_spec = pFrontend->items[gFrontend_selected_item_index].menuInfo;
                FRONTEND_CreateMenu(gCurrent_frontend_spec);
                if (gCurrent_frontend_spec != pFrontend->previous) {
                    gCurrent_frontend_spec->previous = pFrontend;
                }
                Morph_Initialise(pFrontend, gCurrent_frontend_spec);
                if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                    gFrontend_selected_item_index = 0;
                } else {
                    gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
                }
                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                    gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
                }
                RefreshRacesScroller(gCurrent_frontend_spec);
            }
            return pFrontend->items[original_selected_index].field_0xc;
        }
    }
    ServiceGame();
    return gFrontend_leave_current_menu != 0;
}

// FUNCTION: CARMA2_HW 0x00466450
int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x00470c10
int C2_HOOK_FASTCALL Generic_Outfunc(tFrontend_spec* pFrontend) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x00471ba0
int C2_HOOK_FASTCALL Ians_GetItemAtMousePos(tFrontend_spec* pFrontend, int pX, int pY) {
    int i;
    tFrontend_slider* slider;

    for (i = 0; i < pFrontend->count_items; i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];
        br_model* model = gFrontend_brender_items[i].model;
        if (pX >= model->vertices[1].p.v[0] && pX <= model->vertices[3].p.v[0]
                && pY >= -model->vertices[0].p.v[1] && pY <= -model->vertices[1].p.v[1]
                && item->visible
                && item->enabled > 0) {

            return i;
        }
    }
    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {
        for (i = 0; i < 3; i++) {
            br_model* model = gFrontend_brender_items[slider->itemid_start + i].model;
            if (pX >= model->vertices[1].p.v[0] && pX <= model->vertices[3].p.v[0]
                    && pY >= -model->vertices[0].p.v[1] && pY <= -model->vertices[1].p.v[1]) {

                return slider->itemid_start + i;
            }
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x00471cf0
tFrontend_slider* C2_HOOK_FASTCALL GetAnyActiveSlider(void) {
    tFrontend_slider* slider;

    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {

        if (gFrontend_selected_item_index >= slider->itemid_left_reference && gFrontend_selected_item_index <= slider->itemid_left_reference + 2) {

            return slider;
        }
        if (gFrontend_selected_item_index >= slider->itemid_start && gFrontend_selected_item_index <= slider->itemid_start + 2) {

            return slider;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00471d70
int C2_HOOK_FASTCALL ScrollSet_TranslateItemToIndex(tConnected_items* pConnected, int pItem) {
    int i;
    int offset;

    offset = -1;
    for (i = 0; i < pConnected->count_ranges; i++) {

        if (pItem >= pConnected->range_starts[i] && pItem < pConnected->range_starts[i] + pConnected->range_length) {

            offset = pItem - pConnected->range_starts[i];
            break;
        }
    }
    if (offset == -1) {
        return -1;
    }
    return pConnected->field_0x8 + offset;
}

// FUNCTION: CARMA2_HW 0x00471d30
tConnected_items* C2_HOOK_FASTCALL GetScrollSet(int pItem) {
    tConnected_items* scrollset;

    scrollset = gConnected_items;
    while (scrollset != NULL) {
        int i;

        for (i = 0; i < scrollset->count_ranges; i++) {
            if (pItem >= scrollset->range_starts[i] && pItem < scrollset->range_starts[i] + scrollset->range_length) {
                return scrollset;
            }
        }
    }
    return NULL;
}

void C2_HOOK_FASTCALL ScrollSet_DisplayEntry(tConnected_items* pScroll_set, int pItem) {

    if (pItem < pScroll_set->field_0x8 || pItem >= pScroll_set->field_0x8 + pScroll_set->range_length) {
        if (pItem < pScroll_set->field_0x0 - pScroll_set->range_length) {
            pScroll_set->field_0x8 = pItem;
        } else {
            pScroll_set->field_0x8 = pScroll_set->field_0x0 - pScroll_set->range_length;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004720e0
void C2_HOOK_FASTCALL RefreshScrollSet(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gControls_scroller.range_length; i++) {

        strcpy(pFrontend->items[43 + i].text, GetMiscString(140 + gControls_scroller.field_0x8 + i));
        strcpy(pFrontend->items[53 + i].text, gKey_names[gKey_mapping[gControls_frontend_to_key_mapping_lut[i + gControls_scroller.field_0x8]] + 2]);
    }
    pFrontend->items[63].visible = gControls_scroller.field_0x8 != 0;
    pFrontend->items[64].visible = gControls_scroller.field_0x8 != gControls_scroller.field_0x0 - gControls_scroller.range_length;
    FuckWithWidths(pFrontend);
}

int C2_HOOK_FASTCALL DetermineKeyArrayIndex(void) {
    int start;

    start = gFrontend_selected_item_index;
    if (gFrontend_selected_item_index > 52) {
        start = gFrontend_selected_item_index - 10;
    }
    return start + (gControls_scroller.field_0x8 - 43);
}

// FUNCTION: CARMA2_HW 0x00471dd0
int C2_HOOK_FASTCALL Generic_FindNextActiveItem(tFrontend_spec* pFrontend, int pItem) {
    int start_item_group;
    tConnected_items* connected;
    tFrontend_slider* start_active_slider;
    tStruct_00686508* start_up_down;
    int i;
    int original_selected_item_index;

    original_selected_item_index = gFrontend_selected_item_index;
    gFrontend_selected_item_index = pItem;
    start_active_slider = GetAnyActiveSlider();
    start_item_group = pFrontend->items[gFrontend_selected_item_index].group;
    start_up_down = GetUpDown(pItem);

    for (connected = gConnected_items; connected != NULL; connected = connected->next) {
        int i;

        for (i = 0; i < connected->count_ranges; i++) {

            if (pItem == connected->range_starts[i] + connected->range_length - 1) {
                break;
            }
        }
        if (i < connected->count_ranges) {
            break;
        }
    }
    if (connected != NULL) {
        int item = ScrollSet_TranslateItemToIndex(connected, pItem);

        if (item == connected->field_0x8 + connected->range_length - 1 && connected->field_0x0 > connected->field_0x8 + connected->range_length) {
            connected->field_0x8 += 1;
            RefreshScrollSet(pFrontend);
            gFrontend_selected_item_index = item;
            return pItem;
        }
    }

    for (i = 1; i < pFrontend->count_items; i++) {
        int next_item;
        tFrontend_slider* next_active_slider;
        tStruct_00686508* next_up_down;

        next_item = (pItem + i) % pFrontend->count_items;
        if (pFrontend->items[next_item].enabled <= 0) {
            continue;
        }
        if (!pFrontend->items[next_item].visible) {
            continue;
        }
        gFrontend_selected_item_index = pItem;

        next_active_slider = GetAnyActiveSlider();
        if (start_active_slider != NULL && next_active_slider == start_active_slider) {
            continue;
        }
        if (next_active_slider != NULL && (next_active_slider->flags & 0x1)) {
            continue;
        }
        if (pFrontend->count_groups != 0 && start_item_group != 0 && start_item_group == pFrontend->items[next_item].group) {
            continue;
        }
        if (connected != NULL && GetScrollSet(next_item) == connected) {
            continue;
        }
        if (start_up_down == NULL) {
            return next_item;
        }
        next_up_down = GetUpDown(next_item);
        if (start_up_down != next_up_down) {
            if (next_up_down == NULL) {
                return next_item;
            }
            if (next_up_down->field_0x0 == next_item) {
                return next_item;
            }
        }
    }
    gFrontend_selected_item_index = original_selected_item_index;
    return 0;
}

int C2_HOOK_FASTCALL Generic_FindPrevActiveItem(tFrontend_spec* pFrontend, int pItem) {
    int start_selected_item_index;
    int start_item_group;
    tConnected_items* connected;
    tFrontend_slider* start_active_slider;
    tStruct_00686508* start_up_down;
    int i;

    start_selected_item_index = gFrontend_selected_item_index;
    gFrontend_selected_item_index = pItem;
    start_active_slider = GetAnyActiveSlider();
    start_item_group = pFrontend->items[pItem].group;
    start_up_down = GetUpDown(pItem);

    for (connected = gConnected_items; connected != NULL; connected = connected->next) {
        int i;

        for (i = 0; i < connected->count_ranges; i++) {

            if (gFrontend_selected_item_index == connected->range_starts[i]) {
                break;
            }
        }
        if (i < connected->count_ranges) {
            break;
        }
    }
    if (connected != NULL) {
        int item = ScrollSet_TranslateItemToIndex(connected, pItem);

        if (item == connected->field_0x8 && connected->field_0x8 > 0) {
            connected->field_0x8 -= 1;
            RefreshScrollSet(pFrontend);
            return pItem;
        }
    }

    for (i = 1; i < pFrontend->count_items; i++) {
        int prev_item;
        int prev_group;
        tFrontend_slider* prev_active_slider;
        tStruct_00686508* prev_up_down;

        prev_item = pItem - i;
        if (prev_item < 0) {
            prev_item += pFrontend->count_items;
        }
        if (pFrontend->items[prev_item].enabled <= 0) {
            continue;
        }
        if (!pFrontend->items[prev_item].visible) {
            continue;
        }

        gFrontend_selected_item_index = prev_item;
        prev_active_slider = GetAnyActiveSlider();
        if (start_active_slider != NULL && prev_active_slider == start_active_slider) {
            continue;
        }
        if (prev_active_slider != NULL && (prev_active_slider->flags & 0x1)) {
            continue;
        }
        if (pFrontend->count_groups != 0 && start_item_group != 0) {
            int prev_group;

            prev_group = pFrontend->items[prev_item].group;
            if (prev_group == start_item_group) {
                continue;
            }
            if (prev_group != 0) {
                int prev_prev_item;

                prev_prev_item = prev_item - 1;
                if (prev_prev_item < 0) {
                    prev_prev_item = pFrontend->count_items - 1;
                }
                while (pFrontend->items[prev_prev_item].group == prev_group) {
                    prev_item = prev_prev_item;
                    prev_prev_item -= 1;
                    if (prev_prev_item < 0) {
                        prev_prev_item = pFrontend->count_items - 1;
                    }
                }
            }
        } else if (pFrontend->count_groups != 0) {
            int prev_group;

            prev_group = pFrontend->items[prev_item].group;
            if (prev_group != 0) {
                int prev_prev_item;

                prev_prev_item = prev_item - 1;
                if (prev_prev_item < 0) {
                    prev_prev_item = pFrontend->count_items - 1;
                }
                while (pFrontend->items[prev_prev_item].group == prev_group) {
                    prev_item = prev_prev_item;
                    prev_prev_item -= 1;
                    if (prev_prev_item < 0) {
                        prev_prev_item = pFrontend->count_items - 1;
                    }
                }
            }
        }

        if (connected != NULL && GetScrollSet(prev_item) == connected) {
            continue;
        }
        prev_up_down = GetUpDown(prev_item);
        if (start_up_down == NULL) {
            if (prev_up_down == NULL) {
                gFrontend_selected_item_index = start_selected_item_index;
                return prev_item;
            }
            prev_group = prev_up_down->field_0x0;
        } else {
            if (start_up_down == prev_up_down) {
                continue;
            }
            if (prev_up_down == NULL) {
                gFrontend_selected_item_index = start_selected_item_index;
                return prev_item;
            }
            prev_group = prev_up_down->field_0x0;
        }
        if (prev_group == prev_item) {
            gFrontend_selected_item_index = start_selected_item_index;
            return prev_item;
        }
    }
    gFrontend_selected_item_index = start_selected_item_index;
    return 0;
}

int C2_HOOK_FASTCALL TranslateSliderItem(tFrontend_slider* pScroller, int pIndex) {

    if (pIndex >= pScroller->itemid_left_reference && pIndex <= pScroller->itemid_left_reference + 2) {
        return pIndex;
    } else {
        return pScroller->itemid_left_reference + 2;
    }
}

void C2_HOOK_FASTCALL PrepareSliders(tFrontend_spec* pFrontend) {
    tFrontend_slider* slider;

    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {
        float v;
        br_model* model;

        pFrontend->items[slider->itemid_start + 0].x = pFrontend->items[slider->itemid_left_reference].x;
        pFrontend->items[slider->itemid_start + 0].width = 8;
        pFrontend->items[slider->itemid_start + 1].x = pFrontend->items[slider->itemid_left_reference].x + 8;
        pFrontend->items[slider->itemid_start + 1].width = slider->width;
        pFrontend->items[slider->itemid_start + 2].x = pFrontend->items[slider->itemid_left_reference].x + 8 + slider->width;
        pFrontend->items[slider->itemid_start + 2].width = 8;
        if (slider->value < 0.f) {
            slider->value = 0.f;
        } else if (slider->value > 1.f) {
            slider->value = 1.f;
        }

        model = gFrontend_brender_items[slider->itemid_left_reference + 1].model;
        v = model->vertices[1].p.v[0] + slider->value * slider->width;
        model->vertices[2].p.v[0] = model->vertices[3].p.v[0] = v;
        BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);

        model = gFrontend_brender_items[slider->itemid_left_reference + 2].model;
        model->vertices[0].p.v[0] = model->vertices[1].p.v[0] = v;
        model->vertices[2].p.v[0] = model->vertices[3].p.v[0] = v + 8.f;
        BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);
    }
}

tFrontend_slider* C2_HOOK_FASTCALL GetActiveSlider(void) {
    tFrontend_slider* slider;

    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {

        if (slider->flags & 0x1) {
            continue;
        }
        if ((gFrontend_selected_item_index >= slider->itemid_left_reference && gFrontend_selected_item_index <= slider->itemid_left_reference + 2)
                || (gFrontend_selected_item_index >= slider->itemid_start && gFrontend_selected_item_index <= slider->itemid_start + 2)) {

            return slider;
        }
    }
    return slider;
}

void C2_HOOK_FASTCALL Generic_EventEffect(void) {

    DRS3StartSound(gEffects_outlet, eSoundId_Done);
}

tStruct_00686508* C2_HOOK_FASTCALL GetUpDown(int pItem) {
    tStruct_00686508 *up_down;

    for (up_down = gPTR_00686508; up_down != NULL; up_down = up_down->next) {
        if (pItem == up_down->field_0x0
                || pItem == up_down->field_0x4
                || pItem == up_down->field_0x8) {

            return up_down;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00470c20
int C2_HOOK_FASTCALL Generic_MenuHandler(tFrontend_spec* pFrontend) {
    int timeout;
    int original_item;
    int selected_item;
    int item_mouse;
    int button_down;
    int key;
    int mouse_x;
    tFrontend_slider* slider;

    item_mouse = 0; /* Added by rec2 */

    if (gTyping) {
        int input;

        input = ProcessInputString();
        FuckWithWidths(pFrontend);
        if (input < 0) {
            NewGameToggleTyping(pFrontend);
        }
        FuckWithWidths(pFrontend);
        ServiceGame();
        return gFrontend_leave_current_menu ? 1 : 0;
    }

    timeout = pFrontend->timeout != 0 && PDGetTotalTime() >= gFrontend_time_last_input + pFrontend->timeout;

    if (!gFrontend_scrollbars_updated) {
        gFrontend_scrollbars_updated = 1;
        PrepareSliders(pFrontend);
    }
    original_item = gFrontend_selected_item_index;
    button_down = 0;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    PollKeys();
    EdgeTriggerModeOff();
    key = PDAnyKeyDown();
    if (key != -1 && key != 4) {
        gMouse_in_use = 0;
        ResetInterfaceTimeout();
    }
    EdgeTriggerModeOn();
    selected_item = gFrontend_selected_item_index;
    if (gINT_00688444) {
        int mouse_y;

        GetMousePosition(&mouse_x, &mouse_y);
        if (EitherMouseButtonDown()) {

            item_mouse = 1;
        } else {

            gINT_00688444 = 0;
            if (gMouse_in_use) {
                int item;

                ResetInterfaceTimeout();
                button_down = EitherMouseButtonDown();
                gFrontend_selected_item_index = Ians_GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
                if (gFrontend_selected_item_index == -1) {
                    gFrontend_selected_item_index = 99;
                }
                item = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
                selected_item = 99;
                item_mouse = button_down != 0;
                if (item != -1) {
                    selected_item = item;
                }
            }
        }
    } else {

        if (gMouse_in_use) {
            int mouse_y;
            int item;

            GetMousePosition(&mouse_x, &mouse_y);
            button_down = EitherMouseButtonDown();
            if (button_down) {
                ResetInterfaceTimeout();
            }
            /* ??? */
            if (original_item != mouse_x || original_item != mouse_y) {
                ResetInterfaceTimeout();
            }
            gFrontend_selected_item_index = Ians_GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            if (gFrontend_selected_item_index == -1) {
                gFrontend_selected_item_index = 99;
            }
            item = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            selected_item = 99;
            item_mouse = button_down != 0;
            if (item != -1) {
                selected_item = item;
            }
        }
    }

    slider = GetActiveSlider();

    if (slider != NULL) {
        int new_pos;
        int value_changed;

        if (item_mouse == 1) {
            value_changed = 1;
            new_pos = mouse_x - gCurrent_frontend_spec->items[slider->itemid_left_reference + 1].x;
        } else if (PDKeyDown(70)) {
            value_changed = 1;
            new_pos = (int)((float)slider->width * slider->value - (float)slider->width / 10);
        } else if (PDKeyDown(71)) {
            value_changed = 1;
            new_pos = (int)((float)slider->width * slider->value + (float)slider->width / 10);
        } else {
            value_changed = 0;
        }
        if (value_changed) {
            br_model* model;
            float new_vertex_pos;

            gINT_00688444 = 1;
            if (new_pos < 0) {
                new_pos = 0;
            } else if (new_pos > slider->width) {
                new_pos = slider->width;
            }

            model = gFrontend_brender_items[slider->itemid_left_reference + 1].model;
            new_vertex_pos = model->vertices[1].p.v[0] + new_pos;
            model->vertices[2].p.v[0] = model->vertices[3].p.v[0] = new_vertex_pos;
            BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);

            model = gFrontend_brender_items[slider->itemid_left_reference + 2].model;
            model->vertices[0].p.v[0] = model->vertices[1].p.v[0] = new_vertex_pos;
            model->vertices[2].p.v[0] = model->vertices[3].p.v[0] = new_vertex_pos + 8.f;
            BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);

            slider->value = (float)new_pos / (float)slider->width;
            if (slider->callback != NULL) {
                slider->callback(slider);
            }
        }
        selected_item = TranslateSliderItem(slider, gFrontend_selected_item_index);
    }
    gFrontend_selected_item_index = selected_item;

    if (PDKeyDown(72) || PDKeyDown(89)) {

        Generic_EventEffect();
        gFrontend_selected_item_index = Generic_FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);

        if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
        slider = GetActiveSlider();
        if (slider != NULL) {
            gFrontend_selected_item_index = TranslateSliderItem(slider, gFrontend_selected_item_index);
        }
    }
    if (PDKeyDown(73) || PDKeyDown(83)) {

        Generic_EventEffect();
        if (gFrontend_selected_item_index >= pFrontend->count_items - 1) {
            gFrontend_selected_item_index = -1;
        }
        gFrontend_selected_item_index = Generic_FindNextActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
        if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
        slider = GetActiveSlider();
        if (slider != NULL) {

            gFrontend_selected_item_index = TranslateSliderItem(slider, gFrontend_selected_item_index);
        }
    } else if (PDKeyDown(70) || PDKeyDown(85)) {
        int item;
        tStruct_00686508* up_down;
        int group;

        item = gFrontend_selected_item_index;
        up_down = GetUpDown(gFrontend_selected_item_index);
        if (up_down != NULL) {

            gFrontend_selected_item_index = up_down->field_0x8;
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
        gFrontend_selected_item_index = item;
        group = pFrontend->items[gFrontend_selected_item_index].group;
        if (group != 0) {
            if (gFrontend_selected_item_index == pFrontend->radios[group - 1].indexFirstItem) {
                gFrontend_selected_item_index = pFrontend->radios[group - 1].indexLastItem;
            } else {
                gFrontend_selected_item_index = item - 1;
            }
        }
    } else if (PDKeyDown(71) || PDKeyDown(87)) {
        int item;
        tStruct_00686508* up_down;
        int group;

        up_down = GetUpDown(gFrontend_selected_item_index);
        item = gFrontend_selected_item_index;
        if (up_down != NULL) {
            gFrontend_selected_item_index = up_down->field_0x4;
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
        gFrontend_selected_item_index = item;
        group = pFrontend->items[gFrontend_selected_item_index].group;
        if (group != 0) {
            if (item == pFrontend->radios[group - 1].indexLastItem) {
                gFrontend_selected_item_index = pFrontend->radios[group - 1].indexFirstItem;
            } else {
                gFrontend_selected_item_index = item + 1;
            }
        }
    }
    if (timeout || PDKeyDown(63)) {
        int i;

        if (pFrontend == &gFrontend_CONTROLS && Controls_Ok(&gFrontend_CONTROLS) == 4) {
            return 0;
        }
        Generic_LinkOutEffect();
        if (pFrontend->previous == NULL) {
            return 1;
        }
        gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
        FRONTEND_DestroyMenu(pFrontend);
        gCurrent_frontend_spec = pFrontend->previous;
        FRONTEND_CreateMenu(gCurrent_frontend_spec);
        gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;

        for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

            gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
        }
        UpdateScrollPositions(gCurrent_frontend_spec);
        Morph_Initialise(pFrontend, gCurrent_frontend_spec);
        return 0;
    } else if (PDKeyDown(51) || PDKeyDown(52) || (button_down == 1 && item_mouse != -1)) {
        int ret;
        tFrontend_spec* next_menu;
        int going_back;

        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 1) {
            gFrontend_leave_current_menu = 1;
        }
        next_menu = pFrontend->items[gFrontend_selected_item_index].menuInfo;
        going_back = next_menu == (tFrontend_spec*)0x1;
        if (going_back) {
            next_menu = pFrontend->previous;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action == NULL) {
            ret = pFrontend->items[gFrontend_selected_item_index].field_0xc;
        } else {
            if (next_menu == NULL && pFrontend->items[gFrontend_selected_item_index].action != temp) {
                Generic_EventEffect();
            }
            ret = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (ret == 4) {
            return 0;
        }
        if (next_menu != NULL) {
            int i;

            if (going_back) {
                Generic_LinkOutEffect();
            } else {
                Generic_LinkInEffect();
            }
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = next_menu;
            FRONTEND_CreateMenu(next_menu);
            if (gCurrent_frontend_spec != pFrontend->previous) {
                gCurrent_frontend_spec->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                gFrontend_selected_item_index = 0;
            } else {
                gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            }
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            return ret;
        }
        if (!going_back) {
            return ret;
        }
        return 1;
    } else {
        ServiceGame();
        return gFrontend_leave_current_menu ? 1 : 0;
    }
}

void C2_HOOK_FASTCALL Generic_MungeActiveItems(tFrontend_spec* pFrontend) {
    int i;

    gCount_connected_items_indices = 0;
    if (gFrontend_selected_item_index != -1) {
        tConnected_items* connected_items = gConnected_items;
        int scroll_area_containing_selected_item = 0;

        for (; connected_items != NULL; connected_items = connected_items->next) {
            for (i = 0; i < connected_items->count_ranges; i++) {
                if (connected_items->range_starts[i] <= gFrontend_selected_item_index
                        && gFrontend_selected_item_index < connected_items->range_starts[i] + connected_items->range_length) {
                    scroll_area_containing_selected_item = gFrontend_selected_item_index - connected_items->range_starts[i] + 1;
                    break;
                }
            }
            if (scroll_area_containing_selected_item != 0) {
                break;
            }
        }
        if (scroll_area_containing_selected_item == 0) {
            gCount_connected_items_indices = 1;
            gConnected_items_indices[0] = gFrontend_selected_item_index;
            pFrontend->items[gFrontend_selected_item_index].flags |= 0x1;
        } else {
            for (i = 0; i < connected_items->count_ranges; i++) {
                gConnected_items_indices[gCount_connected_items_indices] = scroll_area_containing_selected_item + connected_items->range_starts[i] - 1;
                pFrontend->items[scroll_area_containing_selected_item + connected_items->range_starts[i] - 1].flags |= 0x1;
                gCount_connected_items_indices += 1;
            }
        }
    }
}

void C2_HOOK_FASTCALL Generic_UnMungeActiveItems(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gCount_connected_items_indices; i++) {
        pFrontend->items[gConnected_items_indices[i]].flags &= ~0x1;
    }
    gCount_connected_items_indices = 0;
}

// FUNCTION: CARMA2_HW 0x0046e020
void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend) {
    int i;
    br_fixed_ls blend_x;

    if (gFrontend_leave_current_menu || pFrontend->count_items <= 0) {
        return;
    }
    Generic_MungeActiveItems(pFrontend);
    for (i = 0; i < gFrontend_count_brender_items; i++) {
        tFrontend_brender_item* brender_item = &gFrontend_brender_items[i];
        tFrontend_item_spec* item = &pFrontend->items[i];
        const char* text;

        if (!item->visible) {
            brender_item->actor->render_style = BR_RSTYLE_NONE;
            continue;
        }

        brender_item->actor->render_style = BR_RSTYLE_FACES;
        text = item->stringId >= 0x400 ? item->text : IString_Get(item->stringId);
        if (item->flags & 0x1) {
            if (text != NULL) {
                TransparentPolyFontTextInABox(item->unlitFont, text,
                    item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1, 0.6);
            }
            TransparentPolyFontTextInABox(item->highFont, text,
                item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1,
                item->radioButton_selected ? 0.3 - 0.7 * gFrontend_throb_factor : 0.6 * gFrontend_throb_factor);
        } else if (item->enabled >= 0) {
            if (item->radioButton_selected) {
                if (text != NULL) {
                    SolidPolyFontTextInABox(item->highFont, text,
                        item->x, item->y, item->x + item->width, item->y + item->height, eJust_centre, 1);
                }
            } else {
                if (text != NULL) {
                    SolidPolyFontTextInABox(item->unlitFont, text,
                        item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1);
                }
            }
            brender_item->prims[1].v.x = 0x800000;
            BrMaterialUpdate(brender_item->material, BR_MATU_EXTRA_PRIM);
        } else {
            if (item->map_index && brender_item->prims[1].v.x != 0x400000) {
                brender_item->prims[1].v.x = 0x400000;
                BrMaterialUpdate(brender_item->material, BR_MATU_EXTRA_PRIM);
            }
            if (text != NULL) {
                TransparentPolyFontTextInABox(item->unlitFont, text,
                    item->x, item->y, item->x + item->width, item->y + item->height, eJust_left, 1, 0.4);
            }
        }
    }
    blend_x = BR_FIXED_INT((int)(255. * (0.3 + 0.7 * gFrontend_throb_factor)));
    gFrontend_brender_items[gFrontend_selected_item_index].prims[1].v.x = blend_x;
    BrMaterialUpdate(gFrontend_brender_items[gFrontend_selected_item_index].material, BR_MATU_EXTRA_PRIM);
    if (pFrontend->items[gFrontend_selected_item_index].glowDisabled != 0) {
        for (i = 0; i < gFrontend_count_brender_items; i++) {
            if (pFrontend->items[i].glowDisabled == pFrontend->items[gFrontend_selected_item_index].glowDisabled
                    && gFrontend_brender_items[i].prims[1].v.x != blend_x) {
                gFrontend_brender_items[i].prims[1].v.x = blend_x;
                BrMaterialUpdate(gFrontend_brender_items[i].material, BR_MATU_EXTRA_PRIM);
            }
        }
    }
    FRONTEND_PingPongFlash();
    PossibleService();
    Generic_UnMungeActiveItems(pFrontend);
}

// FUNCTION: CARMA2_HW 0x0046d8b0
void C2_HOOK_FASTCALL MaybeDoMouseCursor(void) {
    if (!gFrontend_suppress_mouse) {
        PossibleLock(1);
        DoMouseCursor();
        PossibleUnlock(1);
    }
}

// FUNCTION: CARMA2_HW 0x00464df0
void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender) {

    if (gPoly_fonts[pFont].available == 0) {
        LoadInterfaceFonts();
    }
    TransparentPolyFontText(pText, pX, pY, pFont, pJust, pRender, 1.0);
}

// FUNCTION: CARMA2_HW 0x00518f20
char* C2_HOOK_FASTCALL MungeCommas(int pValue) {
    // GLOBAL: CARMA2_HW 0x006b5f20
    static char munge_global_buffer[32];
    char buffer[32];
    size_t len;
    size_t get_pos;
    size_t put_pos;
    size_t remaining;

    sprintf(buffer, "%i", pValue);
    len = strlen(buffer);
    for (remaining = len, get_pos = 0, put_pos = 0; get_pos < len; remaining--, get_pos++, put_pos++) {

        if (remaining % 3 == 0 && get_pos != 0) {
            munge_global_buffer[put_pos] = gMisc_strings[294][0];
            put_pos += 1;
        }
        munge_global_buffer[put_pos] = buffer[get_pos];
    }
    munge_global_buffer[put_pos] = '\0';
    return munge_global_buffer;
}

// FUNCTION: CARMA2_HW 0x0046f8a0
br_actor* C2_HOOK_FASTCALL CreateAPOactor(void) {
    br_pixelmap* map;
    br_actor* actor;
    br_material* material;
    br_model* model;

    map = BrPixelmapAllocate(gBack_screen->type, 64, 64, NULL, 0);
    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 1;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 2;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    BrVector3Set(&model->vertices[0].p,  0.f,   0.f, -1.05f);
    BrVector3Set(&model->vertices[1].p, 40.f,   0.f, -1.05f);
    BrVector3Set(&model->vertices[2].p,  0.f, -12.f, -1.05f);
    BrVector3Set(&model->vertices[3].p, 40.f, -12.f, -1.05f);
    material->colour = 0;
    material->colour_map = map;
    material->flags = BR_MATF_ALWAYS_VISIBLE;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(map);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
}

void C2_HOOK_FASTCALL KillAPOactor(br_actor* pActor) {

    if (pActor != NULL) {

        if (pActor->material->colour_map != NULL) {
            BrMapRemove(pActor->material->colour_map);
            BrPixelmapFree(pActor->material->colour_map);
        }
        if (pActor->material != NULL) {
            BrMaterialRemove(pActor->material);
            BrMaterialFree(pActor->material);
        }
        if (pActor->model != NULL) {
            BrModelRemove(pActor->model);
            BrModelFree(pActor->model);
        }
        if (pActor->parent != NULL) {
            BrActorRemove(pActor);
        }
        BrActorFree(pActor);
    }
}

// FUNCTION: CARMA2_HW 0x00518fa0
void C2_HOOK_FASTCALL MungeMetaCharacters(char* pText, char pMeta, const char* pRepl) {
    size_t len_text;
    size_t len_repl;
    size_t i;

    len_text = strlen(pText);
    len_repl = strlen(pRepl);

    for (i = 0; i < len_text; i++) {
        if (pText[i] == '@' && pText[i + 1] == pMeta) {
            memmove(&pText[i + len_repl], &pText[i + 2], len_text - i - 1);
            memcpy(&pText[i], pRepl, len_repl);
            i += len_repl;
            len_text += len_repl - 2;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00519040
void C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pMeta, char pChar) {

    MungeMetaCharacters(pText, pMeta, &pChar);
}

// FUNCTION: CARMA2_HW 0x005190f0
void C2_HOOK_FASTCALL MungeMetaCharactersNum(char* pText, char pMeta, int pNum) {
    char text[16];

    sprintf(text, "%d", pNum);
    MungeMetaCharacters(pText, pMeta, text);
}

// FUNCTION: CARMA2_HW 0x005191b0
void C2_HOOK_FASTCALL DrPixelmapRectangleCopyPossibleLock(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 w, br_int_32 h) {

#if 0
    if (gLock_often) {
        PossibleUnlock(0);
    }
#endif

    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);

#if 0
    if (gLock_often) {
        PossibleUnlock(0);
    }
#endif
}

void C2_HOOK_FASTCALL DefaultInfunc(tFrontend_spec* pFrontend) {

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous != NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    gFrontend_scrollbars_updated = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
}

void C2_HOOK_FASTCALL FillInRaceDescription(char *pDest, int pRace_index) {

    strcpy(pDest, gRace_list[pRace_index].description);
    MungeMetaCharactersChar(pDest, 'R', '\r');
    MungeMetaCharactersNum(pDest, 'O', gRace_list[pRace_index].count_explicit_opponents);
    MungeMetaCharactersNum(pDest, 'L', gRace_list[pRace_index].count_laps);
}

// FUNCTION: CARMA2_HW 0x0046aa20
void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index) {
    tPath_name pack_path;
    char pack_filename[64];
    char* pos_dot;
    tTWTVFS twt;
    int i;

    pos_dot = strrchr(gOpponents[pCar_index].car_file_name, '.');
    PathCat(pack_path, gApplication_path, "INTRFACE");
    PathCat(pack_path, pack_path, "CARIMAGE");

    strcpy(pack_filename, gOpponents[pCar_index].car_file_name);
    pack_filename[strlen(pack_filename) - 4] = '\0';
    strcat(pack_filename, "CI");
    PathCat(pack_path, pack_path, pack_filename);
    twt = OpenPackFileAndSetTiffLoading(pack_path);
    for (i = 0; i < 6; i++) {
        char pm_name[20];

        sprintf(pm_name, "%.*s%c.TIF", pos_dot - gOpponents[pCar_index].car_file_name, gOpponents[pCar_index].car_file_name, 'A' + i);
        BrMapRemove(gFrontend_images[i + 1]);
        BrPixelmapFree(gFrontend_images[i + 1]);
        gFrontend_images[i + 1] = GetThisFuckingPixelmapPleaseMrTwatter(pack_path, pm_name);
        if (gFrontend_images[i + 1] == NULL) {
            gFrontend_images[i + 1] = LoadPixelmap("64by64.tif");
        }
        BrPixelmapCopy(gFrontend_brender_items[pBrender_index + i].field_0xc, gFrontend_images[i + 1]);
        BrPixelmapCopy(gFrontend_brender_items[pBrender_index + i].field_0x10, gFrontend_brender_items[pBrender_index + i].field_0xc);
        BrMapUpdate(gFrontend_brender_items[pBrender_index + i].field_0xc, BR_MAPU_ALL);
        BrMaterialUpdate(gFrontend_brender_items[pBrender_index + i].material, BR_MATU_COLOURMAP);
    }
    ClosePackFileAndSetTiffLoading(twt);
}

// FUNCTION: CARMA2_HW 0x0046b820
void C2_HOOK_FASTCALL MenuSetDriverImage(int pOpponent_index, int pFrontend_index) {
    char* pos_dot;
    tPath_name pack_path;
    char pack_filename[40];
    char pm_name[20];
    tTWTVFS twt;

    pos_dot = strrchr(gOpponents[pOpponent_index].abbrev_name, '.');
    PathCat(pack_path, gApplication_path, "INTRFACE");
    PathCat(pack_path, pack_path, "CARIMAGE");
    strcpy(pack_filename, gOpponents[pOpponent_index].car_file_name);
    pack_filename[strlen(pack_filename) - 4] = '\0';
    strcat(pack_filename, "CI");
    PathCat(pack_path, pack_path, pack_filename);
    twt = OpenPackFileAndSetTiffLoading(pack_path);

    sprintf(pm_name, "%.*s%c%c.TIF", pos_dot - gOpponents[pOpponent_index].abbrev_name, gOpponents[pOpponent_index].abbrev_name, '6', '4');
    BrMapRemove(gFrontend_images[7]);
    BrPixelmapFree(gFrontend_images[7]);
    gFrontend_images[7] = GetThisFuckingPixelmapPleaseMrTwatter(pack_path, pm_name);
    if (gFrontend_images[7] == NULL) {
        gFrontend_images[7] = LoadPixelmap("64by64.tif");
    }
    BrPixelmapCopy(gFrontend_brender_items[pFrontend_index].field_0xc, gFrontend_images[7]);
    BrPixelmapCopy(gFrontend_brender_items[pFrontend_index].field_0x10, gFrontend_brender_items[pFrontend_index].field_0xc);
    BrMapUpdate(gFrontend_brender_items[pFrontend_index].field_0xc, BR_MAPU_ALL);

    ClosePackFileAndSetTiffLoading(twt);

    gFrontend_brender_items[pFrontend_index].model->vertices[0].p.v[2] = -1.f;
    gFrontend_brender_items[pFrontend_index].model->vertices[1].p.v[2] = -1.f;
    gFrontend_brender_items[pFrontend_index].model->vertices[2].p.v[2] = -1.f;
    gFrontend_brender_items[pFrontend_index].model->vertices[3].p.v[2] = -1.f;
    BrModelUpdate(gFrontend_brender_items[pFrontend_index].model, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x0046e470
int C2_HOOK_FASTCALL ProcessInputString(void) {
    int len;
    int key;
    int int_ch;

    if (gFrontend_current_input == NULL) {
        return 0;
    }

    len = strlen(gFrontend_current_input);
    if (len > gFrontend_maximum_input_length) {
        gFrontend_current_input[len - 1] = '\0';
        return -1;
    }
    PollKeys();
    key = PDAnyKeyDown();
    if (key == 50) {
        if (len > 0) {
            gFrontend_current_input[len - 1] = '\0';
        }
        return 1;
    }
    if (key == 63) {
        strcpy(gFrontend_current_input, gFrontend_original_player_name);
        return -2;
    }
    if (key == 51) {
        if (len == 0) {
            strcpy(gFrontend_current_input, gFrontend_original_player_name);
        }
        return -1;
    }
    int_ch = PDGetKeyboardCharacter();
    if (int_ch != 0) {
        char ch;

        dr_dprintf("FRONTEND: Got char %d", int_ch);
        ch = PDConvertToASCIILessThan128((char)int_ch);
        dr_dprintf("FRONTEND: Char converted to %d", ch);
        if (ch >= 0x20 && ch != 0x7f) {
            gFrontend_current_input[len + 0] = ch;
            gFrontend_current_input[len + 1] = '\0';
            dr_dprintf("FRONTEND: Adding char %d to string. String now '%s'", int_ch, gFrontend_current_input);
            return 1;
        }
        dr_dprintf("FRONTEND: Invalid Char", int_ch);
    }
    return 0;
}

void C2_HOOK_FASTCALL FrontEndShowMouse(void) {

    gFrontend_suppress_mouse = 0;
}

void C2_HOOK_FASTCALL FrontEndHideMouse(void) {

    gFrontend_suppress_mouse = 1;
}

void C2_HOOK_FASTCALL DodgyPause(tU32 pTime) {
    tU32 start;

    start = PDGetTotalTime();
    while (PDGetTotalTime() - start > pTime) {
        /* brrrr */
    }
}

void C2_HOOK_FASTCALL StartGettingInputString(char* pBuffer, int pBuffer_size) {

    gINT_0059b0d8 = -1;
    gFrontend_maximum_input_length = pBuffer_size;
    gFrontend_current_input = pBuffer;
    DodgyPause(200);
}

void C2_HOOK_FASTCALL StopGettingInputString(void) {

    gFrontend_current_input = NULL;
}

// FUNCTION: CARMA2_HW 0x00466ec0
int C2_HOOK_FASTCALL ToggleTyping(tFrontend_spec* pFrontend) {

    if (gTyping) {
        EdgeTriggerModeOff();
        WaitForNoKeys();
        EdgeTriggerModeOn();
        FrontEndShowMouse();
        gTyping = 0;
        StopGettingInputString();
        pFrontend->items[gFrontend_text_input_item_index].unlitFont = 1;
        pFrontend->items[gFrontend_text_input_item_index].highFont = 1;
        gFrontend_original_player_name[0] = '\0';
        SaveOptions();
        return 0;
    } else {
        FrontEndHideMouse();
        gTyping = 1;
        gFrontend_text_input_item_index = gFrontend_selected_item_index;
        pFrontend->items[gFrontend_text_input_item_index].unlitFont = 2;
        pFrontend->items[gFrontend_text_input_item_index].highFont = 3;
        strcpy(pFrontend->items[gFrontend_text_input_item_index].text, gProgram_state.player_name);
        strcpy(gFrontend_original_player_name, gProgram_state.player_name);
        PDClearKeyboardBuffer();
        StartGettingInputString(pFrontend->items[gFrontend_text_input_item_index].text, 8);
        return 0;
    }
}

int C2_HOOK_FASTCALL RaceIndex(const char* pName) {
    int i;

    for (i = 0; i < gNumber_of_races; i++) {

        if (DRStricmp(pName, gRace_list[i].name) == 0) {
            return i;
        }
    }
    return gNumber_of_races;
}

void C2_HOOK_FASTCALL ScrollUp(tFrontend_spec* pFrontend, int pScroller) {
    tFrontend_scroller_spec* scroller;

    scroller = &pFrontend->scrollers[pScroller];

    if (scroller->indexTopItem > scroller->indexFirstScrollableItem) {
        scroller->indexTopItem -= 1;
    }
}

void C2_HOOK_FASTCALL ScrollDn(tFrontend_spec* pFrontend, int pScroller) {
    tFrontend_scroller_spec* scroller;

    scroller = &pFrontend->scrollers[pScroller];

    if (scroller->indexTopItem + scroller->nbDisplayedAtOnce < scroller->indexFirstScrollableItem + scroller->count) {
        scroller->indexTopItem += 1;
    }
}

// FUNCTION: CARMA2_HW 0x00484f70
void C2_HOOK_FASTCALL SelectThisItemIn(tFrontend_spec* pFrontend, int pGroup, int pValue) {
    int i;

    for (i = pFrontend->radios[pGroup - 1].indexFirstItem; i <= pFrontend->radios[pGroup - 1].indexLastItem; i++) {

        pFrontend->items[i].radioButton_selected = i == pValue;
    }
}

// FUNCTION: CARMA2_HW 0x00484f40
int C2_HOOK_FASTCALL WhichItemIsSelectedIn(tFrontend_spec* pFrontend, int pGroup) {
    int i;

    for (i = 0; i < pFrontend->count_items; i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        if (item->group == pGroup && item->radioButton_selected) {
            return i;
        }
    }
    return -1;
}
