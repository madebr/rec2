#include "frontend.h"

#include "drmem.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "init.h"
#include "loading.h"
#include "polyfont.h"
#include "platform.h"
#include "sound.h"
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
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_main, 0x005a80f0, FIXMEFIXME);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_options, 0x00632c60, FIXMEFIXME);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_wrecks, 0x005f8e68, FIXMEFIXME);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_netsync, 0x0061bad0, FIXMEFIXME);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_networksummary, 0x00627398, FIXMEFIXME);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_credits, 0x005cab48, FIXMEFIXME);
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

#define COUNT_FRONTEND_INTERPOLATE_STEPS 16

void C2_HOOK_FASTCALL LoadFrontendStrings(void) {
    tPath_name the_path;
    FILE* f;
    char s[256];
    int i;

    if (C2V(gCount_interface_strings) != 0) {
        return;
    }
    PathCat(the_path, C2V(gApplication_path), "INTRFACE");
    PathCat(the_path, the_path, "interface.txt");
    f = TWT_fopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    C2V(gCount_interface_strings) = 0;
    for (i = 0; !DRfeof(f) && i < REC2_ASIZE(C2V(gInterface_strings)); i++) {
        GetALineAndDontArgue(f, s);
        C2V(gInterface_strings)[i] = BrMemAllocate(c2_strlen(s) + 1, kMem_misc_string);
        c2_strcpy(C2V(gInterface_strings)[i], s);
        C2V(gCount_interface_strings)++;
    }
    DRfclose(f);
}
C2_HOOK_FUNCTION(0x00484fd0, LoadFrontendStrings)

void C2_HOOK_FASTCALL FreeInterfaceStrings(void) {
    int i;

    for (i = 0; i < C2V(gCount_interface_strings); i++) {
        BrMemFree(C2V(gInterface_strings)[i]);;
    }
    C2V(gCount_interface_strings) = 0;
}
C2_HOOK_FUNCTION(0x004850d0, FreeInterfaceStrings)

const char* C2_HOOK_FASTCALL GetInterfaceString(int pIndex) {

    if (pIndex > C2V(gCount_interface_strings)) {
        return NULL;
    }
    return C2V(gInterface_strings)[pIndex];
}
C2_HOOK_FUNCTION(0x00485110, GetInterfaceString)

static void C2_HOOK_FASTCALL LoadMenuImages(void) {
    int i;
    char s[256];
    char s2[256];
    FILE* f;
    int count;

    /* 1 to 6 are car images, 7 is driver image */
    for (i = 1; i < 8; i++) { /* FIXME: magic number */
        C2V(gFrontend_images)[i] = DRLoadPixelmap("64by64.tif");
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
        C2V(gFrontend_images)[i + 8] = DRLoadPixelmap(s); /* FIXME: magic number */
    }

#if defined(REC2_FIX_BUGS)
    DRfclose(f);
#endif
}

void (C2_HOOK_FASTCALL * CreateMenuActors_original)(void);
void C2_HOOK_FASTCALL CreateMenuActors(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    CreateMenuActors_original();
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
C2_HOOK_FUNCTION_ORIGINAL(0x0046cf10, CreateMenuActors, CreateMenuActors_original)

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
    DRfclose(f);
#endif
}

br_pixelmap* (C2_HOOK_FASTCALL * Frontend_LoadFrontendPixelmap_original)(const char* pFolder, const char* pName);
br_pixelmap* C2_HOOK_FASTCALL Frontend_LoadFrontendPixelmap(const char* pFolder, const char* pName) {

#if 0//defined(C2_HOOKS_ENABLED)
    return Frontend_LoadFrontendPixelmap_original(pFolder, pName);
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
    f = TWT_fopen(the_path, "rb");
    if (f == NULL) {
        PathCat(the_path, pFolder, pName);
        result = DRLoadUpdatePixelmapFromTif(the_path);
        /* FUN_005193f0(result, 0); */
        return result;
    }
    DRfclose(f);
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
C2_HOOK_FUNCTION_ORIGINAL(0x0046abf0, Frontend_LoadFrontendPixelmap, Frontend_LoadFrontendPixelmap_original)

void (C2_HOOK_FASTCALL * FRONTEND_CreateItemBrenderObjects_original)(tFrontend_brender_item* Frontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText);
void C2_HOOK_FASTCALL FRONTEND_CreateItemBrenderObjects(tFrontend_brender_item* pFrontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText) {

    C2_HOOK_BUG_ON(sizeof(tFrontend_brender_item) != 44);
#if 0//defined(C2_HOOKS_ENABLED)
    FRONTEND_CreateItemBrenderObjects_original(pFrontend_brender_item, pX, pY, pWidth, pHeight, pColour, pMap, pText);
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
        dx = Fix2DTextureWidth(pMap->width);
        dy = Fix2DTextureWidth(pMap->height);
    } else {
        dx = Fix2DTextureWidth(8);
        dy = Fix2DTextureWidth(8);
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
C2_HOOK_FUNCTION_ORIGINAL(0x0046c5c0, FRONTEND_CreateItemBrenderObjects, FRONTEND_CreateItemBrenderObjects_original)

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
            twt = TWT_MountEx(s2);
            C2V(gFrontend_backdrop) = Frontend_LoadFrontendPixelmap(s2, name);
            TWT_UnmountEx(twt);

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
            twt = TWT_MountEx(s2);
            C2V(gFrontend_backdrop) = Frontend_LoadFrontendPixelmap(s2, name);
            TWT_UnmountEx(twt);

            if (C2V(gFrontend_backdrop) == NULL) {
                BrFailure("FRONTEND: Error loading background graphics.", 0);
            }
            /* FUN_005191f0(C2V(gFrontend_backdrop)); */
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
        FRONTEND_CreateItemBrenderObjects(
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
    C2V(gFrontend_time_last_input) = PDGetTotalTime();
    FRONTEND_CreateItemBrenderObjects(&C2V(gFrontend_brender_items)[99],
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
        LoadFrontendStrings();
        InitPolyFonts();
        LoadMenuImages();
        PrintMemoryDump(0, "AFTER LoadMenuImages");
        CreateMenuActors();
        LoadMenuModels();
        PrintMemoryDump(0, "AFTER LoadMenuModels");
        C2V(gFrontend_stuff_not_loaded) = 0;
    }
    switch (pType) {
    case kFrontend_menu_main:
        FRONTEND_CreateMenu(&C2V(gFrontend_main));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_main);
        break;
    case kFrontend_menu_options:
        FRONTEND_CreateMenu(&C2V(gFrontend_options));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_options);
        break;
    case kFrontend_menu_wrecks:
        FRONTEND_CreateMenu(&C2V(gFrontend_wrecks));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_wrecks);
        break;
    case kFrontend_menu_netsync:
        FRONTEND_CreateMenu(&C2V(gFrontend_netsync));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_netsync);
        break;
    case kFrontend_menu_networksummary:
        FRONTEND_CreateMenu(&C2V(gFrontend_networksummary));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_networksummary);
        break;
    case kFrontend_menu_credits:
        FRONTEND_CreateMenu(&C2V(gFrontend_credits));
        C2V(gCurrent_frontend_spec) = &C2V(gFrontend_credits);
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


void (C2_HOOK_FASTCALL * DoFrontendMenu_original)(tFrontendMenuType pFrontend);
void C2_HOOK_FASTCALL DoFrontendMenu(tFrontendMenuType pFrontendType) {

#if defined(C2_HOOKS_ENABLED)
    DoFrontendMenu_original(pFrontendType);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046d8e0, DoFrontendMenu, DoFrontendMenu_original)

void C2_HOOK_FASTCALL RenderFrontendBillboard(int pX, int pY, int pIndex, int pTex_index) {
    br_model *model;

    model = C2V(gFrontend_billboard_actors)[pIndex]->model;
    BrVector2Set(&model->vertices[0].map, 0.f, pTex_index * .1875f);
    BrVector2Set(&model->vertices[1].map, 0.625f, (float)(pTex_index + 1) * .1875f);
    BrVector2Set(&model->vertices[2].map, 0.f, pTex_index * .1875f);
    BrVector2Set(&model->vertices[3].map, 0.625f, (float)(pTex_index + 1) * .1875f);
    BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
    BrVector3Set(&C2V(gFrontend_billboard_actors)[pIndex]->t.t.translate.t, (float)pX, (float)-pY, 0.f);
    BrActorAdd(C2V(gFrontend_menu_camera), C2V(gFrontend_billboard_actors)[pIndex]);
    BrZbSceneRender(C2V(gFrontend_menu_camera), C2V(gFrontend_menu_camera), C2V(gBack_screen), C2V(gDepth_buffer));
    BrActorRemove(C2V(gFrontend_billboard_actors)[pIndex]);
}
C2_HOOK_FUNCTION(0x004708a0, RenderFrontendBillboard)

void C2_HOOK_FASTCALL UpdateThrobFactor(void) {
    tU32 time;

    time = PDGetTotalTime() % 750;

    if (time < 375) {
        C2V(gFrontend_throb_factor) = (double)time / 375.;
    } else {
        C2V(gFrontend_throb_factor) = (double)(750 - time) / 375.;
    }
}
C2_HOOK_FUNCTION(0x0046cec0, UpdateThrobFactor)

void C2_HOOK_FASTCALL FRONTEND_InterpolateModel(br_model* pModel_from, br_model* pModel_to, br_model* pModel, int pStep, int pCount_steps) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {

        pModel->vertices[i].p.v[0] = pModel_from->vertices[i].p.v[0] + (pModel_to->vertices[i].p.v[0] - pModel_from->vertices[i].p.v[0]) * ((float)pStep / (float)pCount_steps);
        pModel->vertices[i].p.v[1] = pModel_from->vertices[i].p.v[1] + (pModel_to->vertices[i].p.v[1] - pModel_from->vertices[i].p.v[1]) * ((float)pStep / (float)pCount_steps);
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}
C2_HOOK_FUNCTION(0x0046f5b0, FRONTEND_InterpolateModel)

void C2_HOOK_FASTCALL FRONTEND_CompleteItemSizes(tFrontend_spec* pFrontend) {
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
            text = GetInterfaceString(item->stringId);
        } else if (item->stringId == 0x404) {
            text = item->text;
        }
        if (text != NULL && !item->wrapText) {
            item->width = GetPolyFontTextWidth(item->unlitFont, text);
            item->height = GetPolyFontHeight(item->unlitFont);
        }
    }
}
C2_HOOK_FUNCTION(0x004666f0, FRONTEND_CompleteItemSizes)

void C2_HOOK_FASTCALL FRONTEND_UpdateScrollerModels(tFrontend_spec* pFrontend, int pIndex) {
    int i;

    tFrontend_scroller_spec* scroller = &pFrontend->scrollers[pIndex];

    for (i = 0; i < scroller->nbDisplayedAtOnce; i++) {
        int item_idx = scroller->indexFirstScrollableItem + i;
        br_model* model = C2V(gFrontend_brender_items)[item_idx].model;

        model->vertices[3].p.v[0] = model->vertices[1].p.v[0] + pFrontend->items[item_idx].width;
        model->vertices[2].p.v[0] = model->vertices[3].p.v[0];
        model->vertices[2].p.v[1] = model->vertices[3].p.v[1] - pFrontend->items[item_idx].height;
        model->vertices[1].p.v[0] = model->vertices[2].p.v[1];
    }
}
C2_HOOK_FUNCTION(0x00466ce0, FRONTEND_UpdateScrollerModels)

void C2_HOOK_FASTCALL FRONTEND_MainMenu_UpdateRaces(tFrontend_spec* pFrontend) {
    char group_text[12];
    int group;
    int i;
    int race_i;

    group = 1 + (C2V(gCurrent_race_group) - C2V(gRaceGroups)) % 10;
    c2_sprintf(group_text, "%s %d", GetInterfaceString(78), group);
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
    FRONTEND_CompleteItemSizes(pFrontend);
    if (C2V(gIs_boundary_race) || C2V(gProgram_state).game_completed) {
        pFrontend->items[7].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[7].enabled = kFrontendItemEnabled_disabled;
    }
    FRONTEND_UpdateScrollerModels(pFrontend, 0);
}
C2_HOOK_FUNCTION(0x00467b30, FRONTEND_MainMenu_UpdateRaces)

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

void C2_HOOK_FASTCALL FRONTEND_SetTransitionModels(tFrontend_spec* pCurrent, tFrontend_spec* pNext) {

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
C2_HOOK_FUNCTION(0x00470a90, FRONTEND_SetTransitionModels)

int C2_HOOK_FASTCALL FRONTEND_DefaultItem_Action(tFrontend_spec* pFrontend) {

    return 0;
}
C2_HOOK_FUNCTION(0x00466450, FRONTEND_DefaultItem_Action)

int C2_HOOK_FASTCALL FRONTEND_Default_Destroy(tFrontend_spec* pFrontend) {

    return 0;
}
C2_HOOK_FUNCTION(0x00470c10, FRONTEND_Default_Destroy)
