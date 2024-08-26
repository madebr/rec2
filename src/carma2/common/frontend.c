#include "frontend.h"

#include "drmem.h"
#include "errors.h"
#include "font.h"
#include "frontend_credits.h"
#include "frontend_loadgame.h"
#include "frontend_main.h"
#include "frontend_netsync.h"
#include "frontend_networksummary.h"
#include "frontend_quit.h"
#include "frontend_startgame.h"
#include "frontend_wrecks.h"
#include "globvars.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "main.h"
#include "polyfont.h"
#include "platform.h"
#include "sound.h"
#include "tinted.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_macros.h"
#include "c2_string.h"
#include "brender/br_types.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_interface_strings, 0x0068c6e0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char*, gInterface_strings, 300, 0x0068c230);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gFrontend_images, 100, 0x00686850);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tFrontend_model, gFrontend_A_models, 13, 0x006870b8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tFrontend_model, gFrontend_B_models, 11, 0x00687188);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tFrontend_model, gFrontend_C_models, 6, 0x00687058);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gFrontend_backdrop_actors, 3, 0x00687030);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gFrontend_stuff_not_loaded, 0x0059b0d0, 1);
C2_HOOK_VARIABLE_IMPLEMENT(tFrontend_spec*, gCurrent_frontend_spec, 0x00688abc);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_options, 0x00632c60, FIXMEFIXME);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_newgame, 0x005bf280, FIXMEFIXME);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gFrontend_A_model_from, 0x00688378);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gFrontend_A_model_to, 0x0068844c);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gFrontend_B_model_from, 0x00688768);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gFrontend_B_model_to, 0x006886e0);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gFrontend_C_model_from, 0x00686504);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gFrontend_C_model_to, 0x006886bc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_backdrop0_opacity_mode, 0x006864e0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_backdrop1_opacity_mode, 0x006864dc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_backdrop2_opacity_mode, 0x006864d4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_interpolate_steps_left, 0x00686ef8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_count_brender_items, 0x0068683c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_remove_current_backdrop, 0x00764ee0);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gFrontend_backdrop, 0x00686f8c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_selected_item_index, 0x00688770);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tFrontend_brender_item, gFrontend_brender_items, 100, 0x00687248); /* FIXME: parametrize size + index of last item */
C2_HOOK_VARIABLE_IMPLEMENT(br_colour, gFrontend_some_color, 0x00688ae8);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gFrontend_time_last_input, 0x0068875c);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gFrontend_actor, 0x0068650c);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gFrontend_camera, 0x00686f94);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_token_value, gFrontend_backdrop0_material_prims, 3, 0x00686f50);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_token_value, gFrontend_backdrop1_material_prims, 3, 0x00686f20);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_token_value, gFrontend_backdrop2_material_prims, 3, 0x00686f38);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gFrontend_backdrop_materials, 3, 0x00686f10);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gFrontend_billboard_actors, 8, 0x00763940);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gFrontend_menu_camera, 0x00688b10);
C2_HOOK_VARIABLE_IMPLEMENT(double, gFrontend_throb_factor, 0x006886d0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTyping_slot, 0x0075b8fc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_leave_current_menu, 0x006883a8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_suppress_mouse, 0x00688b20);
C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, gFrontend_APO_Colour_1, 0x007635f0);
C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, gFrontend_APO_Colour_2, 0x007635e0);
C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, gFrontend_APO_Colour_3, 0x00763700);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gFrontend_wrecks_actor, 0x00688aec);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gFrontend_wrecks_camera, 0x00688af0);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gFrontend_wrecks_pixelmap, 0x007635e4);
C2_HOOK_VARIABLE_IMPLEMENT(tFrontendMenuType, gFrontend_next_menu, 0x00764eec);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gAuthor_credits_scroll_start_time, 0x00686834);
C2_HOOK_VARIABLE_IMPLEMENT(tConnected_items*, gConnected_items, 0x00688ab0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_connected_items_indices, 0x00686f08);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gConnected_items_indices, 6, 0x00687018);

#define COUNT_FRONTEND_INTERPOLATE_STEPS 16

void C2_HOOK_FASTCALL IString_Load(void) {
    tPath_name the_path;
    FILE* f;
    char s[256];
    int i;

    if (C2V(gCount_interface_strings) != 0) {
        return;
    }
    PathCat(the_path, C2V(gApplication_path), "INTRFACE");
    PathCat(the_path, the_path, "interface.txt");
    f = PFfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    C2V(gCount_interface_strings) = 0;
    for (i = 0; !PFfeof(f) && i < REC2_ASIZE(C2V(gInterface_strings)); i++) {
        GetALineAndDontArgue(f, s);
        C2V(gInterface_strings)[i] = BrMemAllocate(c2_strlen(s) + 1, kMem_misc_string);
        c2_strcpy(C2V(gInterface_strings)[i], s);
        C2V(gCount_interface_strings)++;
    }
    PFfclose(f);
}
C2_HOOK_FUNCTION(0x00484fd0, IString_Load)

void C2_HOOK_FASTCALL FreeInterfaceStrings(void) {
    int i;

    for (i = 0; i < C2V(gCount_interface_strings); i++) {
        BrMemFree(C2V(gInterface_strings)[i]);;
    }
    C2V(gCount_interface_strings) = 0;
}
C2_HOOK_FUNCTION(0x004850d0, FreeInterfaceStrings)

const char* C2_HOOK_FASTCALL IString_Get(int pIndex) {

    if (pIndex > C2V(gCount_interface_strings)) {
        return NULL;
    }
    return C2V(gInterface_strings)[pIndex];
}
C2_HOOK_FUNCTION(0x00485110, IString_Get)

void C2_HOOK_FASTCALL LoadMenuImages(void) {
    int i;
    char s[256];
    char s2[256];
    FILE* f;
    int count;

    /* 1 to 6 are car images, 7 is driver image */
    for (i = 1; i < 8; i++) { /* FIXME: magic number */
        C2V(gFrontend_images)[i] = LoadPixelmap("64by64.tif");
    }
    PathCat(s, C2V(gApplication_path), "INTRFACE");
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
        c2_strcpy(s, s2);
        c2_strcat(s, ".TIF");
        C2V(gFrontend_images)[i + 8] = LoadPixelmap(s); /* FIXME: magic number */
    }

#if defined(REC2_FIX_BUGS)
    PFfclose(f);
#endif
}

void (C2_HOOK_FASTCALL * FRONTEND_Setup2D_original)(void);
void C2_HOOK_FASTCALL FRONTEND_Setup2D(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    FRONTEND_Setup2D_original();
#else
    br_camera* camera;

    C2V(gFrontend_actor) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    C2V(gFrontend_camera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = C2V(gFrontend_camera)->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BrDegreeToAngle(45); /* 0x1ffe */
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;
    BrActorAdd(C2V(gFrontend_actor), C2V(gFrontend_camera));
    C2V(gFrontend_backdrop0_material_prims)[0].t = BRT_BLEND_B;
    C2V(gFrontend_backdrop0_material_prims)[0].v.b = 1;
    C2V(gFrontend_backdrop0_material_prims)[1].t = BRT_OPACITY_X;
    C2V(gFrontend_backdrop0_material_prims)[1].v.x = 0xb00000;
    C2V(gFrontend_backdrop0_material_prims)[2].t = BR_NULL_TOKEN;
    C2V(gFrontend_backdrop0_material_prims)[2].v.u32 = 0;
    C2V(gFrontend_backdrop1_material_prims)[0].t = BRT_BLEND_B;
    C2V(gFrontend_backdrop1_material_prims)[0].v.b = 1;
    C2V(gFrontend_backdrop1_material_prims)[1].t = BRT_OPACITY_X;
    C2V(gFrontend_backdrop1_material_prims)[1].v.x = 0xb00000;
    C2V(gFrontend_backdrop1_material_prims)[2].t = BR_NULL_TOKEN;
    C2V(gFrontend_backdrop1_material_prims)[2].v.u32 = 0;
    C2V(gFrontend_backdrop2_material_prims)[0].t = BRT_BLEND_B;
    C2V(gFrontend_backdrop2_material_prims)[0].v.b = 1;
    C2V(gFrontend_backdrop2_material_prims)[1].t = BRT_OPACITY_X;
    C2V(gFrontend_backdrop2_material_prims)[1].v.x = 0xb00000;
    C2V(gFrontend_backdrop2_material_prims)[2].t = BR_NULL_TOKEN;
    C2V(gFrontend_backdrop2_material_prims)[2].v.u32 = 0;
    C2V(gFrontend_backdrop_materials)[0] = BrMaterialAllocate("Backdrop_material1");
    C2V(gFrontend_backdrop_materials)[1] = BrMaterialAllocate("Backdrop_material2");
    C2V(gFrontend_backdrop_materials)[2] = BrMaterialAllocate("Backdrop_material3");
    C2V(gFrontend_backdrop_actors)[0] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    C2V(gFrontend_backdrop_actors)[1] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    C2V(gFrontend_backdrop_actors)[2] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    C2V(gFrontend_backdrop_materials)[0]->colour = 0;
    C2V(gFrontend_backdrop_materials)[1]->colour = 0;
    C2V(gFrontend_backdrop_materials)[2]->colour = 0;
    C2V(gFrontend_backdrop_materials)[0]->index_base = 0;
    C2V(gFrontend_backdrop_materials)[1]->index_base = 0;
    C2V(gFrontend_backdrop_materials)[2]->index_base = 0;
    C2V(gFrontend_backdrop_materials)[0]->index_range = 1;
    C2V(gFrontend_backdrop_materials)[1]->index_range = 1;
    C2V(gFrontend_backdrop_materials)[2]->index_range = 1;
    C2V(gFrontend_backdrop_materials)[0]->extra_prim = C2V(gFrontend_backdrop0_material_prims);
    C2V(gFrontend_backdrop_materials)[1]->extra_prim = C2V(gFrontend_backdrop1_material_prims);
    C2V(gFrontend_backdrop_materials)[2]->extra_prim = C2V(gFrontend_backdrop2_material_prims);
    BrMaterialAdd(C2V(gFrontend_backdrop_materials)[0]);
    BrMaterialAdd(C2V(gFrontend_backdrop_materials)[1]);
    BrMaterialAdd(C2V(gFrontend_backdrop_materials)[2]);
    C2V(gFrontend_backdrop_actors)[0]->material = C2V(gFrontend_backdrop_materials)[0];
    C2V(gFrontend_backdrop_actors)[0]->identifier = BrResStrDup(C2V(gFrontend_backdrop_actors)[0], "Backdrop");;
    C2V(gFrontend_backdrop_actors)[1]->material = C2V(gFrontend_backdrop_materials)[1];
    C2V(gFrontend_backdrop_actors)[1]->identifier = BrResStrDup(C2V(gFrontend_backdrop_actors)[1], "Backdrop");
    C2V(gFrontend_backdrop_actors)[2]->material = C2V(gFrontend_backdrop_materials)[2];
    C2V(gFrontend_backdrop_actors)[2]->identifier = BrResStrDup(C2V(gFrontend_backdrop_actors)[2], "Backdrop");
    BrActorAdd(C2V(gFrontend_actor), C2V(gFrontend_backdrop_actors)[0]);
    BrActorAdd(C2V(gFrontend_actor), C2V(gFrontend_backdrop_actors)[1]);
    BrActorAdd(C2V(gFrontend_actor), C2V(gFrontend_backdrop_actors)[2]);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046cf10, FRONTEND_Setup2D, FRONTEND_Setup2D_original)

static void C2_HOOK_FASTCALL LoadMenuModels(void) {
    char s[256];
    char s2[256];
    char s3[256];
    FILE* f;
    int count;
    int i;

    PathCat(s, C2V(gApplication_path), "INTRFACE");
    PathCat(s2, s, "MENUMODELS.TXT");
    f = DRfopen(s2, "rt");
    if (f == NULL) {
        FatalError(kFatalError_Mysterious_SS, "", "");
    }

    C2_HOOK_BUG_ON(sizeof(tFrontend_model) != 16);
    PathCat(s, C2V(gApplication_path), "INTRFACE");
    /* number of A models */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s3);
        PathCat(s2, s, s3);
        c2_strcat(s2, ".DAT");
        C2V(gFrontend_A_models)[i].model = BrModelLoad(s2);
        C2V(gFrontend_A_models)[i].model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(C2V(gFrontend_A_models)[i].model);
        if (i == 0) {
            C2V(gFrontend_backdrop_actors)[0]->model = BrModelLoad(s2);
            C2V(gFrontend_backdrop_actors)[0]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(C2V(gFrontend_backdrop_actors)[0]->model);
            BrMatrix34Translate(&C2V(gFrontend_backdrop_actors)[0]->t.t.mat, 300.f, -250.f, -2.5f);
            BrMatrix34PreScale(&C2V(gFrontend_backdrop_actors)[0]->t.t.mat, 1.25f, 1.25f, 1.0f);
            C2V(gFrontend_backdrop_actors)[0]->render_style = BR_RSTYLE_FACES;
        }
    }
    /* number of B models */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s3);
        PathCat(s2, s, s3);
        c2_strcat(s2, ".DAT");
        C2V(gFrontend_B_models)[i].model = BrModelLoad(s2);
        C2V(gFrontend_B_models)[i].model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(C2V(gFrontend_B_models)[i].model);
        if (i == 0) {
            C2V(gFrontend_backdrop_actors)[1]->model = BrModelLoad(s2);
            C2V(gFrontend_backdrop_actors)[1]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(C2V(gFrontend_backdrop_actors)[1]->model);
            BrMatrix34Translate(&C2V(gFrontend_backdrop_actors)[1]->t.t.mat, 300.f, -250.f, -2.5f);
            BrMatrix34PreScale(&C2V(gFrontend_backdrop_actors)[1]->t.t.mat, 1.25f, 1.25f, 1.0f);
            C2V(gFrontend_backdrop_actors)[1]->render_style = BR_RSTYLE_FACES;
        }
    }
    /* number of C models */
    count = GetAnInt(f);
    for (i = 0; i < count; i++) {
        GetAString(f, s3);
        PathCat(s2, s, s3);
        c2_strcat(s2, ".DAT");
        C2V(gFrontend_C_models)[i].model = BrModelLoad(s2);
        C2V(gFrontend_C_models)[i].model->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(C2V(gFrontend_C_models)[i].model);
        if (i == 0) {
            C2V(gFrontend_backdrop_actors)[2]->model = BrModelLoad(s2);
            C2V(gFrontend_backdrop_actors)[2]->model->flags |= BR_MODF_KEEP_ORIGINAL;
            BrModelAdd(C2V(gFrontend_backdrop_actors)[2]->model);
            BrMatrix34Translate(&C2V(gFrontend_backdrop_actors)[2]->t.t.mat, 300.f, -250.f, -2.5f);
            BrMatrix34PreScale(&C2V(gFrontend_backdrop_actors)[2]->t.t.mat, 1.25f, 1.25f, 1.0f);
            C2V(gFrontend_backdrop_actors)[2]->render_style = BR_RSTYLE_FACES;
        }
    }

#if defined(REC2_FIX_BUGS)
    PFfclose(f);
#endif
}

br_pixelmap* (C2_HOOK_FASTCALL * GetThisFuckingPixelmapPleaseMrTwatter_original)(const char* pFolder, const char* pName);
br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmapPleaseMrTwatter(const char* pFolder, const char* pName) {

#if 0//defined(C2_HOOKS_ENABLED)
    return GetThisFuckingPixelmapPleaseMrTwatter_original(pFolder, pName);
#else

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
        result = DRLoadUpdatePixelmapFromTif(the_path);
        /* FUN_005193f0(result, 0); */
        return result;
    }
    PFfclose(f);
    count = BrPixelmapLoadMany(the_path, pixelmaps, REC2_ASIZE(pixelmaps));
    c2_strcpy(the_path, pName);
    str = c2_strchr(the_path, '.');
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046abf0, GetThisFuckingPixelmapPleaseMrTwatter, GetThisFuckingPixelmapPleaseMrTwatter_original)

void (C2_HOOK_FASTCALL * FRONTEND_CreateMenuButton_original)(tFrontend_brender_item* Frontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText);
void C2_HOOK_FASTCALL FRONTEND_CreateMenuButton(tFrontend_brender_item* pFrontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText) {

    C2_HOOK_BUG_ON(sizeof(tFrontend_brender_item) != 44);
#if 0//defined(C2_HOOKS_ENABLED)
    FRONTEND_CreateMenuButton_original(pFrontend_brender_item, pX, pY, pWidth, pHeight, pColour, pMap, pText);
#else
    br_actor* actor;
    br_model* model;
    br_material* material;
    int dx;
    int dy;
    float map_x;
    float map_y;

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
    BrActorAdd(C2V(gFrontend_actor), actor);
    actor->render_style = BR_RSTYLE_NONE;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046c5c0, FRONTEND_CreateMenuButton, FRONTEND_CreateMenuButton_original)

int (C2_HOOK_FASTCALL * FRONTEND_CreateMenu_original)(tFrontend_spec* pFrontend_spec);
int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec) {

#if 0//defined(C2_HOOKS_ENABLED)
    return FRONTEND_CreateMenu_original(pFrontend_spec);
#else
    char s[256];
    char s2[256];
    const char* name;
    int i;

    c2_sprintf(s, "START OF FRONTEND_CreateMenu for menu \'%s\'", pFrontend_spec->name);
    PrintMemoryDump(0, s);
    if (pFrontend_spec->create != NULL) {
        pFrontend_spec->create(pFrontend_spec);
    }
    C2V(gFrontend_count_brender_items) = 0;
    name = pFrontend_spec->backdrop_name;
    if (name != NULL && c2_strlen(name) != 0) {
        if (!C2V(gFrontend_remove_current_backdrop)) {
            tTWTVFS twt;

            PathCat(s2, C2V(gApplication_path), "INTRFACE");
            PathCat(s2, s2, "BACKDROP");
            c2_strcpy(s, name);
            C2_HOOK_ASSERT(s[c2_strlen(s) - 4] == '.');
            s[c2_strlen(s) - 4] = '\0';
            PathCat(s2, s2, s);
            twt = OpenPackFileAndSetTiffLoading(s2);
            C2V(gFrontend_backdrop) = GetThisFuckingPixelmapPleaseMrTwatter(s2, name);
            ClosePackFileAndSetTiffLoading(twt);

            if (C2V(gFrontend_backdrop) == NULL) {
                BrFailure("FRONTEND: Error loading background graphics.", 0);
            }
            /* FUN_005191f0(gFrontend_backdrop); */
            BrMapAdd(C2V(gFrontend_backdrop));
        } else if (C2V(gFrontend_backdrop) != NULL) {
            tTWTVFS twt;

            BrMapRemove(C2V(gFrontend_backdrop));
            BrPixelmapFree(C2V(gFrontend_backdrop));

            PathCat(s2, C2V(gApplication_path), "INTRFACE");
            PathCat(s2, s2, "BACKDROP");
            c2_strcpy(s, name);
            C2_HOOK_ASSERT(s[c2_strlen(s) - 4] == '.');
            s[c2_strlen(s) - 4] = '\0';
            PathCat(s2, s2, s);
            twt = OpenPackFileAndSetTiffLoading(s2);
            C2V(gFrontend_backdrop) = GetThisFuckingPixelmapPleaseMrTwatter(s2, name);
            ClosePackFileAndSetTiffLoading(twt);

            if (C2V(gFrontend_backdrop) == NULL) {
                BrFailure("FRONTEND: Error loading background graphics.", 0);
            }
            PixelmapSwapByteOrder(C2V(gFrontend_backdrop));
            BrMapAdd(C2V(gFrontend_backdrop));
        }
    }
    StartMouseCursor();
    for (i = 0; i < pFrontend_spec->count_items; i++) {
        tFrontend_item_spec* item = &pFrontend_spec->items[i];
        tS16 y;
        tS16 width;
        tS16 height;

        if (i == C2V(gFrontend_selected_item_index)) {
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
            &C2V(gFrontend_brender_items)[C2V(gFrontend_count_brender_items)],
            item->x,
            y,
            width,
            height,
            C2V(gFrontend_some_color),
            item->map_index == 0 ? NULL : C2V(gFrontend_images)[item->map_index],
            item->text);
        C2V(gFrontend_count_brender_items)++;
    }
    ResetInterfaceTimeout();
    FRONTEND_CreateMenuButton(&C2V(gFrontend_brender_items)[99],
        0,
        0,
        0,
        0,
        C2V(gFrontend_some_color),
        NULL,
        "");
    C2V(gFrontend_selected_item_index) = C2V(gMouse_in_use) ? 99 : 0;
    c2_sprintf(s, "END OF FRONTEND_CreateMenu for menu \'%s\'", pFrontend_spec->name);
    PrintMemoryDump(0, s);
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046c970, FRONTEND_CreateMenu, FRONTEND_CreateMenu_original)

void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pType) {

    PrintMemoryDump(0, "START OF FRONTEND_Setup");
    LoadInterfaceStuff(C2V(gProgram_state).racing);
    if (C2V(gFrontend_stuff_not_loaded)) {
        IString_Load();
        InitPolyFonts();
        LoadMenuImages();
        PrintMemoryDump(0, "AFTER LoadMenuImages");
        FRONTEND_Setup2D();
        LoadMenuModels();
        PrintMemoryDump(0, "AFTER LoadMenuModels");
        C2V(gFrontend_stuff_not_loaded) = 0;
    }
    switch (pType) {
    case kFrontend_menu_main:
        FRONTEND_CreateMenu(&C2V(gFrontend_MAIN));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_MAIN);
        break;
    case kFrontend_menu_options:
        FRONTEND_CreateMenu(&C2V(gFrontend_options));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_options);
        break;
    case kFrontend_menu_wrecks:
        FRONTEND_CreateMenu(&C2V(gFrontend_WRECKS));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_WRECKS);
        break;
    case kFrontend_menu_netsync:
        FRONTEND_CreateMenu(&C2V(gFrontend_NETSYNC));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_NETSYNC);
        break;
    case kFrontend_menu_networksummary:
        FRONTEND_CreateMenu(&C2V(gFrontend_NETWORK_SUMMARY));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_NETWORK_SUMMARY);
        break;
    case kFrontend_menu_credits:
        FRONTEND_CreateMenu(&C2V(gFrontend_CREDITS));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_CREDITS);
        break;
    case kFrontend_menu_newgame:
        FRONTEND_CreateMenu(&C2V(gFrontend_newgame));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_newgame);
        break;
    }
    PrintMemoryDump(0, "AFTER FRONTEND_CreateMenu");

    C2V(gCurrent_frontend_spec)->previous = NULL;
    C2V(gFrontend_A_model_from) = C2V(gFrontend_A_models)[0].model;
    C2V(gFrontend_A_model_to) = C2V(gFrontend_A_models)[C2V(gCurrent_frontend_spec)->model_A_index].model;
    C2V(gFrontend_B_model_from) = C2V(gFrontend_B_models)[0].model;
    C2V(gFrontend_B_model_to) = C2V(gFrontend_B_models)[C2V(gCurrent_frontend_spec)->model_B_index].model;
    C2V(gFrontend_C_model_from) = C2V(gFrontend_C_models)[0].model;
    C2V(gFrontend_C_model_to) = C2V(gFrontend_C_models)[C2V(gCurrent_frontend_spec)->model_C_index].model;
    C2V(gFrontend_backdrop0_opacity_mode) = C2V(gCurrent_frontend_spec)->model_A_index != 0 ? 1 : -2;
    C2V(gFrontend_backdrop1_opacity_mode) = C2V(gCurrent_frontend_spec)->model_B_index != 0 ? 1 : -2;
    C2V(gFrontend_backdrop2_opacity_mode) = C2V(gCurrent_frontend_spec)->model_C_index != 0 ? 1 : -1;
    C2V(gFrontend_interpolate_steps_left) = COUNT_FRONTEND_INTERPOLATE_STEPS;
    PrintMemoryDump(0, "END OF FRONTEND_Setup");
}
C2_HOOK_FUNCTION(0x0046d1c0, FRONTEND_Setup)

void (C2_HOOK_FASTCALL * FRONTEND_RenderAuthorCredits_original)(void);
void C2_HOOK_FASTCALL ScrollCredits(void) {

    C2_HOOK_VARIABLE_IMPLEMENT(int, gAuthor_credits_line_count, 0x006883c0);
    C2_HOOK_VARIABLE_IMPLEMENT(int*, gAuthor_credits_heights, 0x00686f04);
    C2_HOOK_VARIABLE_IMPLEMENT(int*, gAuthor_credits_throbs, 0x00687238);
    C2_HOOK_VARIABLE_IMPLEMENT(char**, gAuthor_credits_texts, 0x00688448);
    C2_HOOK_VARIABLE_IMPLEMENT(int*, gAuthor_credits_fonts, 0x0068682c);
    C2_HOOK_VARIABLE_IMPLEMENT(int, gAuthor_credits_total_height, 0x00686f90);

#if 0//defined(C2_HOOKS_ENABLED)
    FRONTEND_RenderAuthorCredits_original();
#else
    int i;
    int y = (int)(430.f - 0.03f * (float)(PDGetTotalTime() - C2V(gAuthor_credits_scroll_start_time)));

    for (i = 0; i < C2V(gAuthor_credits_line_count); i++) {
        y += C2V(gAuthor_credits_heights)[i];
        if (y > 30 && y < 430) {
            if (C2V(gAuthor_credits_throbs[i])) {
                SolidPolyFontText(C2V(gAuthor_credits_texts)[i], 320, y, C2V(gAuthor_credits_fonts)[i] - 1, eJust_centre, 1);
                TransparentPolyFontText(C2V(gAuthor_credits_texts)[i], 320, y, C2V(gAuthor_credits_fonts)[i], eJust_centre, 1, C2V(gFrontend_throb_factor));
            } else {
                SolidPolyFontText(C2V(gAuthor_credits_texts)[i], 320, y, C2V(gAuthor_credits_fonts)[i], eJust_centre, 1);
            }
        }
    }
    if (C2V(gAuthor_credits_total_height) < 0.03f * (PDGetTotalTime() - C2V(gAuthor_credits_scroll_start_time))) {
        C2V(gAuthor_credits_scroll_start_time) = PDGetTotalTime();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046f630, ScrollCredits, FRONTEND_RenderAuthorCredits_original)

void C2_HOOK_FASTCALL ResetInterfaceTimeout(void) {

    C2V(gFrontend_time_last_input) = PDGetTotalTime();
}

void C2_HOOK_FASTCALL Generic_LinkInEffect(void) {
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_Swingin);
}

void C2_HOOK_FASTCALL BuildAPO(int pCurrent, int pPotential, int pActorIdx, int pAPO) {
    int i;
    br_pixelmap* map;

    map = C2V(gFrontend_billboard_actors)[pActorIdx]->material->colour_map;

    if (pAPO == 0) {
        BrPixelmapFill(map, 0);
    }

    for (i = 0; i < 30; i++) {
        br_uint_32 c;

        if (i >= pPotential) {
            c = C2V(gFrontend_APO_Colour_2);
        } else if (i < pCurrent) {
            c = C2V(gFrontend_APO_Colour_1);
        } else {
            c = C2V(gFrontend_APO_Colour_3);
        }

        BrPixelmapRectangleFill(map, 4 * (i % 10), 0 + 4 * (i / 10) + 12 * pAPO, 3, 3, c);
    }
}
C2_HOOK_FUNCTION(0x004709b0, BuildAPO)

void C2_HOOK_FASTCALL PrepareAPO(int pActorIdx) {

    BrMapUpdate(C2V(gFrontend_billboard_actors)[pActorIdx]->material->colour_map, BR_MAPU_ALL);
    BrMaterialUpdate(C2V(gFrontend_billboard_actors)[pActorIdx]->material, BR_MATU_ALL);
    BrModelUpdate(C2V(gFrontend_billboard_actors)[pActorIdx]->model, BR_MODU_VERTICES);
}
C2_HOOK_FUNCTION(0x00470860, PrepareAPO)

int (C2_HOOK_FASTCALL * DoFrontendMenu_original)(tFrontendMenuType pFrontend);
int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontendType) {

#if 0//defined(C2_HOOKS_ENABLED)
    return DoFrontendMenu_original(pFrontendType);
#else
    static C2_HOOK_VARIABLE_IMPLEMENT(int, back_screen_base_x, 0x0076370c);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, back_screen_base_y, 0x00763710);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, back_screen_origin_x, 0x00763704);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, back_screen_origin_y, 0x00763708);

    if (C2V(gSound_enabled)) {
        DRS3StopAllOutletSoundsExceptCDA();
    }
    SwitchToHiresMode();
    WaitForNoKeys();
    C2V(gFrontend_remove_current_backdrop) = 0;
    TurnTintedPolyOff(C2V(gHud_tinted1));
    TurnTintedPolyOff(C2V(gHud_tinted2));
    TurnTintedPolyOff(C2V(gHud_tinted3));
    FRONTEND_Setup(pFrontendType);
    C2V(gFrontend_remove_current_backdrop) = 1;
    C2V(gFrontend_leave_current_menu) = 0;
    ResetInterfaceTimeout();
    Generic_LinkInEffect();
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_spec, unknownLastInt, 0xb8c4);
    C2V(gCurrent_frontend_spec)->unknownLastInt = 0;

    for (;;) {
        C2V(gBack_screen)->origin_x = 0;
        C2V(gBack_screen)->origin_y = 0;
        if (C2V(gFrontend_backdrop) != NULL) {
            DRPixelmapRectangleCopy(C2V(gBack_screen), 0, 0,
                C2V(gFrontend_backdrop), 0, 0, C2V(gFrontend_backdrop)->width, C2V(gFrontend_backdrop)->height);
        }
        if (C2V(gCurrent_frontend_spec) != &C2V(gFrontend_LOAD_GAME) &&
              C2V(gCurrent_frontend_spec) != &C2V(gFrontend_NETSYNC) &&
              C2V(gCurrent_frontend_spec) != &C2V(gFrontend_NETWORK_SUMMARY) &&
              C2V(gFrontend_interpolate_steps_left) == 0) {
            C2V(gCurrent_frontend_spec)->unknownLastInt = FRONTEND_GenericMenuHandler(C2V(gCurrent_frontend_spec));
        }
        BrPixelmapFill(C2V(gDepth_buffer), 0xffffffff);
        C2V(gBack_screen)->origin_x = 0;
        C2V(gBack_screen)->origin_y = 0;

        if (C2V(gFrontend_interpolate_steps_left) > 0) {
            int step = 16 - C2V(gFrontend_interpolate_steps_left) + 1;

            MorphBlob(C2V(gFrontend_A_model_from), C2V(gFrontend_A_model_to), C2V(gFrontend_backdrop_actors)[0]->model, step, 16);
            if (C2V(gFrontend_backdrop0_opacity_mode) == -1) {
                C2V(gFrontend_backdrop0_material_prims)[1].v.x = BR_FIXED_INT(176 * (16 - step)  / 16);
            } else if (C2V(gFrontend_backdrop0_opacity_mode) == 1) {
                C2V(gFrontend_backdrop0_material_prims)[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (C2V(gFrontend_backdrop0_opacity_mode) == -2) {
                C2V(gFrontend_backdrop0_material_prims)[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(C2V(gFrontend_backdrop_materials)[0], BR_MATU_ALL);

            MorphBlob(C2V(gFrontend_B_model_from), C2V(gFrontend_B_model_to), C2V(gFrontend_backdrop_actors)[1]->model, step, 16);
            if (C2V(gFrontend_backdrop1_opacity_mode) == -1) {
                C2V(gFrontend_backdrop1_material_prims)[1].v.x = BR_FIXED_INT(176 * (16 - step)  / 16);
            } else if (C2V(gFrontend_backdrop1_opacity_mode) == 1) {
                C2V(gFrontend_backdrop1_material_prims)[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (C2V(gFrontend_backdrop1_opacity_mode) == -2) {
                C2V(gFrontend_backdrop1_material_prims)[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(C2V(gFrontend_backdrop_materials)[1], BR_MATU_ALL);

            MorphBlob(C2V(gFrontend_C_model_from), C2V(gFrontend_C_model_to), C2V(gFrontend_backdrop_actors)[2]->model, step, 16);
            if (C2V(gFrontend_backdrop2_opacity_mode) == -1) {
                C2V(gFrontend_backdrop2_material_prims)[1].v.x = BR_FIXED_INT(176 * (16 - step)  / 16);
            } else if (C2V(gFrontend_backdrop2_opacity_mode) == 1) {
                C2V(gFrontend_backdrop2_material_prims)[1].v.x = BR_FIXED_INT(176 * step / 16);
            } else if (C2V(gFrontend_backdrop2_opacity_mode) == -2) {
                C2V(gFrontend_backdrop2_material_prims)[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(C2V(gFrontend_backdrop_materials)[2], BR_MATU_ALL);
        }
        BrZbsSceneRender(C2V(gFrontend_actor), C2V(gFrontend_camera), C2V(gBack_screen), C2V(gDepth_buffer));

        if (C2V(gCurrent_frontend_spec) == &C2V(gFrontend_LOAD_GAME) ||
                C2V(gCurrent_frontend_spec) == &C2V(gFrontend_NETSYNC) ||
                C2V(gCurrent_frontend_spec) == &C2V(gFrontend_NETWORK_SUMMARY)) {
            if (C2V(gFrontend_interpolate_steps_left) == 0) {
                C2V(gCurrent_frontend_spec)->unknownLastInt = FRONTEND_GenericMenuHandler(C2V(gCurrent_frontend_spec));
            }
        }
        if (C2V(gFrontend_interpolate_steps_left) != 0) {
            C2V(gFrontend_interpolate_steps_left) -= 1;
        } else {
            if (C2V(gCurrent_frontend_spec) == &C2V(gFrontend_WRECKS) && C2V(gFrontend_wrecks_actor) != NULL) {
                C2V(back_screen_base_x) = C2V(gBack_screen)->base_x;
                C2V(back_screen_base_y) = C2V(gBack_screen)->base_y;
                C2V(back_screen_origin_x) = C2V(gBack_screen)->origin_x;
                C2V(back_screen_origin_y) = C2V(gBack_screen)->origin_y;
                C2V(gBack_screen)->base_x = 0;
                C2V(gBack_screen)->base_y = 0;
                C2V(gBack_screen)->origin_x = 320;
                C2V(gBack_screen)->origin_y = 120;
                BrZbsSceneRender(C2V(gFrontend_wrecks_actor), C2V(gFrontend_wrecks_camera), C2V(gFrontend_wrecks_pixelmap), C2V(gDepth_buffer));
                C2V(gBack_screen)->base_x = C2V(back_screen_base_x);
                C2V(gBack_screen)->base_y = C2V(back_screen_base_y);
                C2V(gBack_screen)->origin_x = C2V(back_screen_origin_x);
                C2V(gBack_screen)->origin_y = C2V(back_screen_origin_y);
            }
            if (C2V(gCurrent_frontend_spec) == &C2V(gFrontend_MAIN) && C2V(gFrontend_menu_camera) != NULL) {
                BuildAPO(C2V(gCurrent_APO_levels)[0], C2V(gCurrent_APO_potential_levels)[0], 0, 0);
                BuildAPO(C2V(gCurrent_APO_levels)[1], C2V(gCurrent_APO_potential_levels)[1], 0, 1);
                BuildAPO(C2V(gCurrent_APO_levels)[2], C2V(gCurrent_APO_potential_levels)[2], 0, 2);

                PrepareAPO(0);
                PrintAPO( 95, 348, 0, 0);
                PrintAPO(146, 348, 0, 1);
                PrintAPO(195, 348, 0, 2);
            }
            if (C2V(gCurrent_frontend_spec) == &C2V(gFrontend_CREDITS)) {
                ScrollCredits();
            }
            FRONTEND_DrawMenu(C2V(gCurrent_frontend_spec));
            if (C2V(gCurrent_frontend_spec)->unknownLastInt == 0) {
                MaybeDoMouseCursor();
            }
        }
        PDScreenBufferSwap(0);
        if (C2V(gCurrent_frontend_spec)->unknownLastInt == 1 && C2V(gCurrent_frontend_spec) != &C2V(gFrontend_START_GAME)) {
            FRONTEND_DestroyMenu(C2V(gCurrent_frontend_spec));
            c2_strcpy(C2V(gFrontend_START_GAME).backdrop_name, C2V(gCurrent_frontend_spec)->backdrop_name);
            Morph_Initialise(C2V(gCurrent_frontend_spec), &C2V(gFrontend_START_GAME));
            C2V(gCurrent_frontend_spec) = &C2V(gFrontend_START_GAME);
            FRONTEND_CreateMenu(&C2V(gFrontend_START_GAME));
        }
        if (C2V(gCurrent_frontend_spec)->unknownLastInt == 3) {
            FRONTEND_DestroyMenu(C2V(gCurrent_frontend_spec));
            FRONTEND_Setup(C2V(gFrontend_next_menu));
            C2V(gCurrent_frontend_spec)->unknownLastInt = 0;
        }
        if (C2V(gCurrent_frontend_spec)->unknownLastInt != 0) {
            FRONTEND_DestroyMenu(C2V(gCurrent_frontend_spec));
            if (C2V(gFrontend_backdrop) != NULL) {
                BrMapRemove(C2V(gFrontend_backdrop));
                BrPixelmapFree(C2V(gFrontend_backdrop));
            }
            DisposeInterfaceFonts();
            return C2V(gCurrent_frontend_spec)->unknownLastInt != 2;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046d8e0, FRONTEND_Main, DoFrontendMenu_original)

void C2_HOOK_FASTCALL PrintAPO(int pX, int pY, int pIndex, int pTex_index) {
    br_model *model;

    model = C2V(gFrontend_billboard_actors)[pIndex]->model;
    BrVector2Set(&model->vertices[0].map, 0.f, pTex_index * .1875f);
    BrVector2Set(&model->vertices[1].map, 0.625f, pTex_index * .1875f);
    BrVector2Set(&model->vertices[2].map, 0.f, (float)(pTex_index + 1) * .1875f);
    BrVector2Set(&model->vertices[3].map, 0.625f, (float)(pTex_index + 1) * .1875f);
    BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
    BrVector3Set(&C2V(gFrontend_billboard_actors)[pIndex]->t.t.translate.t, (float)pX, (float)-pY, 0.f);
    BrActorAdd(C2V(gFrontend_menu_camera), C2V(gFrontend_billboard_actors)[pIndex]);
    BrZbSceneRender(C2V(gFrontend_menu_camera), C2V(gFrontend_menu_camera), C2V(gBack_screen), C2V(gDepth_buffer));
    BrActorRemove(C2V(gFrontend_billboard_actors)[pIndex]);
}
C2_HOOK_FUNCTION(0x004708a0, PrintAPO)

void C2_HOOK_FASTCALL FRONTEND_PingPongFlash(void) {
    tU32 time;

    time = PDGetTotalTime() % 750;

    if (time < 375) {
        C2V(gFrontend_throb_factor) = (double)time / 375.;
    } else {
        C2V(gFrontend_throb_factor) = (double)(750 - time) / 375.;
    }
}
C2_HOOK_FUNCTION(0x0046cec0, FRONTEND_PingPongFlash)

void C2_HOOK_FASTCALL MorphBlob(br_model* pModel_from, br_model* pModel_to, br_model* pModel, int pStep, int pCount_steps) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {

        pModel->vertices[i].p.v[0] = pModel_from->vertices[i].p.v[0] + (pModel_to->vertices[i].p.v[0] - pModel_from->vertices[i].p.v[0]) * ((float)pStep / (float)pCount_steps);
        pModel->vertices[i].p.v[1] = pModel_from->vertices[i].p.v[1] + (pModel_to->vertices[i].p.v[1] - pModel_from->vertices[i].p.v[1]) * ((float)pStep / (float)pCount_steps);
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}
C2_HOOK_FUNCTION(0x0046f5b0, MorphBlob)

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
            item->width = GetPolyFontTextWidth(item->unlitFont, text);
            item->height = PolyFontHeight(item->unlitFont);
        }
    }
}
C2_HOOK_FUNCTION(0x004666f0, FuckWithWidths)

void C2_HOOK_FASTCALL FRONTEND_UpdateScrollerModels(tFrontend_spec* pFrontend, int pIndex) {
    int i;

    tFrontend_scroller_spec* scroller = &pFrontend->scrollers[pIndex];

    for (i = 0; i < scroller->nbDisplayedAtOnce; i++) {
        int item_idx = scroller->indexFirstScrollableItem + i;
        br_model* model = C2V(gFrontend_brender_items)[item_idx].model;

        model->vertices[3].p.v[0] = model->vertices[1].p.v[0] + pFrontend->items[item_idx].width;
        model->vertices[2].p.v[0] = model->vertices[3].p.v[0];
        model->vertices[2].p.v[1] = model->vertices[3].p.v[1] - pFrontend->items[item_idx].height;
        model->vertices[1].p.v[1] = model->vertices[2].p.v[1];
        BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);
    }
}
C2_HOOK_FUNCTION(0x00466ce0, FRONTEND_UpdateScrollerModels)

void C2_HOOK_FASTCALL RefreshRacesScroller(tFrontend_spec* pFrontend) {
    char group_text[12];
    int group;
    int i;
    int race_i;

    group = 1 + (C2V(gCurrent_race_group) - C2V(gRaceGroups)) % 10;
    c2_sprintf(group_text, "%s %d", IString_Get(78), group);
    c2_strcpy(pFrontend->items[2].text, group_text);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_scroller_spec, indexFirstScrollableItem, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_scroller_spec, indexLastScrollableItem, 0x14);

    race_i = 0;
    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i < pFrontend->scrollers[0].indexLastScrollableItem; i++, race_i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        c2_strcpy(item->text, C2V(gRace_list)[4 * (group - 1) + race_i].name);
        item->radioButton_selected = race_i == C2V(gProgram_state).current_race_index;
        if (C2V(gRace_list)[race_i].is_boundary) {
            item->unlitFont = 2;
            item->highFont = 2;
        } else {
            item->unlitFont = 0;
            item->highFont = 1;
        }
    }
    FuckWithWidths(pFrontend);
    if (C2V(gIs_boundary_race) || C2V(gProgram_state).game_completed) {
        pFrontend->items[7].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[7].enabled = kFrontendItemEnabled_disabled;
    }
    FRONTEND_UpdateScrollerModels(pFrontend, 0);
}
C2_HOOK_FUNCTION(0x00467b30, RefreshRacesScroller)

int C2_HOOK_FASTCALL FRONTEND_FindItemUnderMouse(tFrontend_spec *pFrontend, int pX, int pY) {
    int i;

    for (i = 0; i < pFrontend->count_items; i++) {
        br_model *model = C2V(gFrontend_brender_items)[i].model;
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
C2_HOOK_FUNCTION(0x004677d0, FRONTEND_FindItemUnderMouse)

int C2_HOOK_FASTCALL FRONTEND_FindVisibleItem(tFrontend_spec* pFrontend, int pStart_index) {
    int i;

    for (;;) {
        for (i = pStart_index - 1; i >= 0; i--) {
            tFrontend_item_spec *item = &pFrontend->items[i];

            if (item->enabled > 0 && item->visible) {
                return i;
            }
        }
        pStart_index = pFrontend->count_items;
    }
}
C2_HOOK_FUNCTION(0x00467a30, FRONTEND_FindVisibleItem)

void C2_HOOK_FASTCALL FRONTEND_HandleClick(tFrontend_spec* pFrontend) {

    int selected_index = C2V(gFrontend_selected_item_index);
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
            DRS3StartSound(C2V(gEffects_outlet), eSoundId_Done);
        }
    } else if (selected_item->idLevelBar != 0) {
        int i;
        int level_id = selected_item->idLevelBar;

        for (i = pFrontend->levels[level_id - 1].first_item_id; i <= pFrontend->levels[level_id - 1].last_item_id; i++) {
            br_pixelmap* src;

            if (i > selected_index) {
                src = C2V(gFrontend_images)[5];
            } else {
                src = C2V(gFrontend_images)[4];
            }
            BrPixelmapCopy(C2V(gFrontend_brender_items)[i].field_0xc, src);
            BrPixelmapCopy(C2V(gFrontend_brender_items)[i].field_0x10, src);
            pFrontend->items[i].radioButton_selected = 0;
        }
        pFrontend->items[selected_index].radioButton_selected = 1;
    }
}
C2_HOOK_FUNCTION(0x00467890, FRONTEND_HandleClick)

int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend) {
    char buffer[256];
    br_actor* actor;
    int i;

    c2_sprintf(buffer, "START OF FRONTEND_DestroyMenu for menu '%s'", pFrontend->name);
    PrintMemoryDump(0, buffer);
    if (pFrontend->destroy != NULL) {
        pFrontend->destroy(pFrontend);
    }
    for (actor = C2V(gFrontend_actor)->children; actor != NULL; actor = C2V(gFrontend_actor)->children) {
        if (c2_strcmp(actor->identifier, "Backdrop") == 0) {
            break;
        }
        BrActorRemove(actor);
    }
    for (i = 0; i < C2V(gFrontend_count_brender_items); i++) {

        BrMaterialRemove(C2V(gFrontend_brender_items)[i].material);
        BrModelRemove(C2V(gFrontend_brender_items)[i].model);
        C2V(gFrontend_brender_items)[i].actor->render_style = BR_RSTYLE_NONE;
        BrMaterialFree(C2V(gFrontend_brender_items)[i].material);
        C2V(gFrontend_brender_items)[i].material = NULL;
        BrModelFree(C2V(gFrontend_brender_items)[i].model);
        C2V(gFrontend_brender_items)[i].model = NULL;
        BrActorFree(C2V(gFrontend_brender_items)[i].actor);
        C2V(gFrontend_brender_items)[i].actor = NULL;
        if (C2V(gFrontend_brender_items)[i].field_0xc != NULL) {
            BrMapRemove(C2V(gFrontend_brender_items)[i].field_0xc);
            BrPixelmapFree(C2V(gFrontend_brender_items)[i].field_0xc);
            C2V(gFrontend_brender_items)[i].field_0xc = NULL;
        }
        if (C2V(gFrontend_brender_items)[i].field_0x10 != NULL) {
            BrPixelmapFree(C2V(gFrontend_brender_items)[i].field_0x10);
            C2V(gFrontend_brender_items)[i].field_0x10 = NULL;
        }
    }
    /* FIXME: parametrize last item */
    BrMaterialRemove(C2V(gFrontend_brender_items)[99].material);
    BrModelRemove(C2V(gFrontend_brender_items)[99].model);
    C2V(gFrontend_brender_items)[99].actor->render_style = BR_RSTYLE_NONE;
    BrMaterialFree(C2V(gFrontend_brender_items)[99].material);
    C2V(gFrontend_brender_items)[99].material = NULL;
    BrModelFree(C2V(gFrontend_brender_items)[99].model);
    C2V(gFrontend_brender_items)[99].model = NULL;
    BrActorFree(C2V(gFrontend_brender_items)[99].actor);
    C2V(gFrontend_brender_items)[99].actor = NULL;
    EndMouseCursor();
    c2_sprintf(buffer, "END OF FRONTEND_DestroyMenu for menu '%s'", pFrontend->name);
    PrintMemoryDump(0, buffer);
    return 1;
}
C2_HOOK_FUNCTION(0x0046ccb0, FRONTEND_DestroyMenu)

void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext) {

    C2V(gFrontend_A_model_from) = C2V(gFrontend_A_models)[pCurrent->model_A_index].model;
    C2V(gFrontend_A_model_to) = C2V(gFrontend_A_models)[pNext->model_A_index].model;
    if (pCurrent->model_A_index == 0) {
        C2V(gFrontend_backdrop0_opacity_mode) = pNext->model_A_index != 0 ? 1 : -2;
    } else {
        C2V(gFrontend_backdrop0_opacity_mode) = pNext->model_A_index != 0 ? 0 : -1;
    }

    C2V(gFrontend_B_model_from) = C2V(gFrontend_B_models)[pCurrent->model_B_index].model;
    C2V(gFrontend_B_model_to) = C2V(gFrontend_B_models)[pNext->model_B_index].model;
    if (pCurrent->model_B_index == 0) {
        C2V(gFrontend_backdrop1_opacity_mode) = pNext->model_B_index != 0 ? 1 : -2;
    } else {
        C2V(gFrontend_backdrop1_opacity_mode) = pNext->model_B_index != 0 ? 0 : -1;
    }

    C2V(gFrontend_C_model_from) = C2V(gFrontend_C_models)[pCurrent->model_C_index].model;
    C2V(gFrontend_C_model_to) = C2V(gFrontend_C_models)[pNext->model_C_index].model;
    if (pCurrent->model_C_index == 0) {
        C2V(gFrontend_backdrop2_opacity_mode) = pNext->model_C_index != 0 ? 1 : -2;
    } else {
        C2V(gFrontend_backdrop2_opacity_mode) = pNext->model_C_index != 0 ? 0 : -1;
    }

    C2V(gFrontend_interpolate_steps_left) = 16;
}
C2_HOOK_FUNCTION(0x00470a90, Morph_Initialise)

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

int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend) {
    int original_selected_index;

    original_selected_index = C2V(gFrontend_selected_item_index);
    C2V(gTyping_slot) = -1;
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
            C2V(gMouse_in_use) = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        mouse_button = 0;
        item_under_mouse = -1;
        if (C2V(gMouse_in_use)) {
            int x;
            int y;

            ResetInterfaceTimeout();
            GetMousePosition(&x, &y);
            mouse_button = EitherMouseButtonDown();
            item_under_mouse = FRONTEND_FindItemUnderMouse(C2V(gCurrent_frontend_spec), x, y);
            if (item_under_mouse != -1) {
                C2V(gFrontend_selected_item_index) = item_under_mouse;
            } else {
                C2V(gFrontend_selected_item_index) = 99;
            }
        }

        if (PDKeyDown(72) || PDKeyDown(89)) {
            C2V(gFrontend_selected_item_index) = FRONTEND_FindVisibleItem(pFrontend, C2V(gFrontend_selected_item_index));
            if (!C2V(gMouse_in_use)
                    && C2V(gFrontend_selected_item_index) >= C2V(gCurrent_frontend_spec)->scrollers[0].indexFirstScrollableItem
                    && C2V(gFrontend_selected_item_index) <= C2V(gCurrent_frontend_spec)->scrollers[0].indexLastScrollableItem) {
                FRONTEND_HandleClick(pFrontend);
                if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
                    pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
                }
            }
        }

        if (PDKeyDown(73) || PDKeyDown(83)) {
            C2V(gFrontend_selected_item_index) = Frontend_FindNextVisibleItem(pFrontend, C2V(gFrontend_selected_item_index));
            if (!C2V(gMouse_in_use)
                && C2V(gFrontend_selected_item_index) >= C2V(gCurrent_frontend_spec)->scrollers[0].indexFirstScrollableItem
                && C2V(gFrontend_selected_item_index) <= C2V(gCurrent_frontend_spec)->scrollers[0].indexLastScrollableItem) {
                FRONTEND_HandleClick(pFrontend);
                if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
                    pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
                }
            }
        }

        if (PDKeyDown(63)) {
            if (pFrontend->previous == NULL) {
                return 2;
            } else {
                int i;

                C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);
                FRONTEND_DestroyMenu(pFrontend);
                C2V(gCurrent_frontend_spec) = pFrontend->previous;
                FRONTEND_CreateMenu(C2V(gCurrent_frontend_spec));
                Morph_Initialise(pFrontend, C2V(gCurrent_frontend_spec));
                for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {
                    C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem = C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop;
                }
                return 0;
            }
        }

        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button == 1 && item_under_mouse != -1)) {
            FRONTEND_HandleClick(pFrontend);
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc == 2) {
                return pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc;
            }
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc == 1) {
                C2V(gFrontend_leave_current_menu) = 1;
            }
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
                pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
            }
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo != NULL) {
                int i;

                C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);
                for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {
                    C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem = C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop;
                }
                FRONTEND_DestroyMenu(pFrontend);
                C2V(gCurrent_frontend_spec) = pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo;
                FRONTEND_CreateMenu(C2V(gCurrent_frontend_spec));
                if (C2V(gCurrent_frontend_spec) != pFrontend->previous) {
                    C2V(gCurrent_frontend_spec)->previous = pFrontend;
                }
                Morph_Initialise(pFrontend, C2V(gCurrent_frontend_spec));
                if (C2V(gCurrent_frontend_spec) == &C2V(gFrontend_QUIT)) {
                    C2V(gFrontend_selected_item_index) = 0;
                } else {
                    C2V(gFrontend_selected_item_index) = C2V(gCurrent_frontend_spec)->default_item;
                }
                for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {
                    C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem = C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop;
                }
                RefreshRacesScroller(C2V(gCurrent_frontend_spec));
            }
            return pFrontend->items[original_selected_index].field_0xc;
        }
    }
    ServiceGame();
    return C2V(gFrontend_leave_current_menu) != 0;
}
C2_HOOK_FUNCTION(0x0046c0d0, FRONTEND_GenericMenuHandler)

int C2_HOOK_FASTCALL FRONTEND_DefaultItem_Action(tFrontend_spec* pFrontend) {

    return 0;
}
C2_HOOK_FUNCTION(0x00466450, FRONTEND_DefaultItem_Action)

int C2_HOOK_FASTCALL FRONTEND_Default_Destroy(tFrontend_spec* pFrontend) {

    return 0;
}
C2_HOOK_FUNCTION(0x00470c10, FRONTEND_Default_Destroy)

int (C2_HOOK_FASTCALL * FRONTEND_Default_Tick_original)(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL FRONTEND_Default_Tick(tFrontend_spec* pFrontend) {

#if defined(C2_HOOKS_ENABLED)
    return FRONTEND_Default_Tick_original(pFrontend);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00470c20, FRONTEND_Default_Tick, FRONTEND_Default_Tick_original)

void C2_HOOK_FASTCALL Generic_MungeActiveItems(tFrontend_spec* pFrontend) {
    int i;

    C2V(gCount_connected_items_indices) = 0;
    if (C2V(gFrontend_selected_item_index) != -1) {
        tConnected_items* gConnected_items_indices = C2V(gConnected_items);
        int scroll_area_containing_selected_item = 0;

        for (; gConnected_items_indices != NULL; gConnected_items_indices = gConnected_items_indices->next) {
            for (i = 0; i < gConnected_items_indices->count_ranges; i++) {
                if (gConnected_items_indices->range_starts[i] <= C2V(gFrontend_selected_item_index) && C2V(gFrontend_selected_item_index) < gConnected_items_indices->range_starts[i] + gConnected_items_indices->range_length) {
                    scroll_area_containing_selected_item = C2V(gFrontend_selected_item_index) - gConnected_items_indices->range_starts[i] + 1;
                    break;
                }
            }
            if (scroll_area_containing_selected_item != 0) {
                break;
            }
        }
        if (scroll_area_containing_selected_item == 0) {
            C2V(gCount_connected_items_indices) = 1;
            C2V(gConnected_items_indices)[0] = C2V(gFrontend_selected_item_index);
            pFrontend->items[C2V(gFrontend_selected_item_index)].flags |= 0x1;
        } else {
            for (i = 0; i < gConnected_items_indices->count_ranges; i++) {
                C2V(gConnected_items_indices)[C2V(gCount_connected_items_indices)] = scroll_area_containing_selected_item + gConnected_items_indices->range_starts[i] - 1;
                pFrontend->items[i - 1].flags |= 0x1;
                C2V(gCount_connected_items_indices) += 1;
            }
        }
    }
}

void C2_HOOK_FASTCALL Generic_UnMungeActiveItems(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < C2V(gCount_connected_items_indices); i++) {
        pFrontend->items[C2V(gConnected_items_indices)[i]].flags &= ~0x1;
    }
    C2V(gCount_connected_items_indices) = 0;
}

void (C2_HOOK_FASTCALL * FRONTEND_RenderItems_original)(tFrontend_spec* pFrontend);
void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend) {

#if 0//defined(C2_HOOKS_ENABLED)
    FRONTEND_RenderItems_original(pFrontend);
#else
    int i;
    br_fixed_ls blend_x;

    if (C2V(gFrontend_leave_current_menu) || pFrontend->count_items <= 0) {
        return;
    }
    Generic_MungeActiveItems(pFrontend);
    for (i = 0; i < C2V(gFrontend_count_brender_items); i++) {
        tFrontend_brender_item* brender_item = &C2V(gFrontend_brender_items)[i];
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
                item->radioButton_selected ? 0.3 - 0.7 * C2V(gFrontend_throb_factor) : 0.6 * C2V(gFrontend_throb_factor));
        } else if (item->enabled >= 0) {
            if (item->radioButton_selected) {
                if (text != NULL) {
                    RenderInterfaceBlendedPolyText(item->highFont, text,
                        item->x, item->y, item->x + item->width, item->y + item->height, eJust_centre, 1);
                }
            } else {
                if (text != NULL) {
                    RenderInterfaceBlendedPolyText(item->unlitFont, text,
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
    blend_x = BR_FIXED_INT((int)(255. * (0.3 + 0.7 * C2V(gFrontend_throb_factor))));
    C2V(gFrontend_brender_items)[C2V(gFrontend_selected_item_index)].prims[1].v.x = blend_x;
    BrMaterialUpdate(C2V(gFrontend_brender_items)[C2V(gFrontend_selected_item_index)].material, BR_MATU_EXTRA_PRIM);
    if (pFrontend->items[C2V(gFrontend_selected_item_index)].glowDisabled != 0) {
        for (i = 0; i < C2V(gFrontend_count_brender_items); i++) {
            if (pFrontend->items[i].glowDisabled == pFrontend->items[C2V(gFrontend_selected_item_index)].glowDisabled
                    && C2V(gFrontend_brender_items)[i].prims[1].v.x != blend_x) {
                C2V(gFrontend_brender_items)[i].prims[1].v.x = blend_x;
                BrMaterialUpdate(C2V(gFrontend_brender_items)[i].material, BR_MATU_EXTRA_PRIM);
            }
        }
    }
    FRONTEND_PingPongFlash();
    PossibleService();
    Generic_UnMungeActiveItems(pFrontend);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046e020, FRONTEND_DrawMenu, FRONTEND_RenderItems_original)

void C2_HOOK_FASTCALL MaybeDoMouseCursor(void) {
    if (!C2V(gFrontend_suppress_mouse)) {
        PossibleLock(1);
        DoMouseCursor();
        PossibleUnlock(1);
    }
}
C2_HOOK_FUNCTION(0x0046d8b0, MaybeDoMouseCursor)

void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender) {

    if (C2V(gPolyFonts)[pFont].available == 0) {
        LoadInterfacePolyFonts();
    }
    TransparentPolyFontText(pText, pX, pY, pFont, pJust, pRender, 1.0);
}
C2_HOOK_FUNCTION(0x00464df0, SolidPolyFontText)

char* C2_HOOK_FASTCALL MungeCommas(int pValue) {
    static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, munge_global_buffer, 32, 0x006b5f20);
    char buffer[32];
    size_t len;
    size_t get_pos;
    size_t put_pos;
    size_t remaining;

    c2_sprintf(buffer, "%i", pValue);
    len = c2_strlen(buffer);
    for (remaining = len, get_pos = 0, put_pos = 0; get_pos < len; remaining--, get_pos++, put_pos++) {

        if (remaining % 3 == 0 && get_pos != 0) {
            C2V(munge_global_buffer)[put_pos] = C2V(gMisc_strings)[294][0];
            put_pos += 1;
        }
        C2V(munge_global_buffer)[put_pos] = buffer[get_pos];
    }
    C2V(munge_global_buffer)[put_pos] = '\0';
    return C2V(munge_global_buffer);
}
C2_HOOK_FUNCTION(0x00518f20, MungeCommas)

br_actor* C2_HOOK_FASTCALL CreateAPOactor(void) {
    br_pixelmap* map;
    br_actor* actor;
    br_material* material;
    br_model* model;

    map = BrPixelmapAllocate(C2V(gBack_screen)->type, 64, 64, NULL, 0);
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
C2_HOOK_FUNCTION(0x0046f8a0, CreateAPOactor)
