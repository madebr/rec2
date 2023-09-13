#include "frontend.h"

#include "drmem.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "polyfont.h"
#include "platform.h"
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
C2_HOOK_VARIABLE_IMPLEMENT(int, gMouse_in_use, 0x0079ecb8);

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

#if defined(C2_HOOKS_ENABLED)
    CreateMenuActors_original();
#else
#error "Not implemented"
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

#if defined(C2_HOOKS_ENABLED)
    return Frontend_LoadFrontendPixelmap_original(pFolder, pName);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046abf0, Frontend_LoadFrontendPixelmap, Frontend_LoadFrontendPixelmap_original)

void (C2_HOOK_FASTCALL * FRONTEND_CreateItemBrenderObjects_original)(tFrontend_brender_item* Frontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText);
void C2_HOOK_FASTCALL FRONTEND_CreateItemBrenderObjects(tFrontend_brender_item* Frontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText) {

    C2_HOOK_BUG_ON(sizeof(tFrontend_brender_item) != 44);
#if defined(C2_HOOKS_ENABLED)
    FRONTEND_CreateItemBrenderObjects_original(Frontend_brender_item, pX, pY, pWidth, pHeight, pColour, pMap, pText);
#else
#error "Not implemented"
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
