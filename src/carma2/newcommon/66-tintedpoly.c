#include "66-tintedpoly.h"

#include "08-loading1.h"
#include "18-graphics2.h"
#include "28-world3.h"
#include "41-utility.h"
#include "42-input.h"
#include "globvars.h"
#include "globvrkm.h"
#include "platform.h"
#include "rec2_macros.h"
#include "c2_string.h"
#include "c2_stdio.h"

// GLOBAL: CARMA2_HW 0x00705c80
tTintedPoly gTintedPolys[10];

// GLOBAL: CARMA2_HW 0x006a0430
br_actor* gTintedPolyCamera;

// GLOBAL: CARMA2_HW 0x0065e840
br_token_value gTinted_poly_tokens[9] = {
    { BRT_BLEND_B, { 1 } },
    { BRT_OPACITY_X, { BR_FIXED_INT(0x80) } },
    { BRT_BLEND_B, { 1 } },
    { BRT_OPACITY_X, { BR_FIXED_INT(0x80) } },
    { BRT_BLEND_B, { 1 } },
    { BRT_OPACITY_X, { BR_FIXED_INT(0x80) } },
    { BRT_DECAL_B, { 0x80 } },
    { BRT_DECAL_B, { 0x746e6974 } },
    { BR_NULL_TOKEN, { 0 } },
};

/* The original three unterminated lists share their suffixes. The renderer
 * also consumes the following two decal tokens, then ignores string data up
 * to a zero token. Keep the observable token stream within one bounded array.
 * Entry 6's value is also the original default-opacity global at 0x0065e874.
 */
#define gTinted_poly_material_tokens (&gTinted_poly_tokens[0])
#define gTinted_poly_zero_opacity_tokens (&gTinted_poly_tokens[2])
#define gTinted_poly_custom_opacity_tokens (&gTinted_poly_tokens[4])



// GLOBAL: CARMA2_HW 0x006a0434
br_colour* gTinted_poly_palette;
// GLOBAL: CARMA2_HW 0x006a0438
br_uint_32 gTintedColourMap_grn;
// GLOBAL: CARMA2_HW 0x006a043c
br_uint_32 gTintedColourMap_blu;
// GLOBAL: CARMA2_HW 0x006a0440
int gTinted_fire_phase;
// GLOBAL: CARMA2_HW 0x006a0444
int gTinted_fire_frame;
// GLOBAL: CARMA2_HW 0x006a0448
int gTinted_fire_save;
// GLOBAL: CARMA2_HW 0x006a044c
int gTinted_fire_cooling;

// FUNCTION: CARMA2_HW 0x004d7040
void C2_HOOK_FASTCALL InitTintedPolyStuff(void) {
    br_camera *camera;

    memset(gTintedPolys, 0, sizeof(gTintedPolys));

    if (gTintedPolyCamera == NULL) {
        gTintedPolyCamera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
        gTintedPolyCamera->identifier = "tinted_poly_camera";

        camera = gTintedPolyCamera->type_data;
        camera->type = BR_CAMERA_PARALLEL;
        camera->hither_z = 1.0f;
        camera->yon_z = 3.0f;
        camera->width = gScreen->width;
        camera->height = gScreen->height;
    }
}

static br_material* C2_HOOK_FASTCALL BuildTintedPolyMaterial(int pOpacity) {
    br_material *material;

    material = BrMaterialAllocate("Tint Poly Mat");
    if (material == NULL) {
        return NULL;
    }
    material->colour = BR_COLOUR_RGB(0xc8, 0xc8, 0xc8);
    material->flags = BR_MATF_ALWAYS_VISIBLE;
    BrMaterialAdd(material);
    if (pOpacity != 0) {
        material->extra_prim = gTinted_poly_material_tokens;
        material->extra_prim[1].v.x = BR_FIXED_INT(pOpacity);
        BrMaterialUpdate(material, BR_MATU_EXTRA_PRIM);
    }
    return material;
}

static void C2_HOOK_FASTCALL GetRangeOfValuesFromPixelmap(br_pixelmap* pMap, int* pDarkest, int* pBrightest) {
    int y;
    int x;
    br_uint_32 colour;
    int colour_component;

    *pDarkest = 255;
    *pBrightest = 0;
    for (y = 0; y < pMap->height; y++) {
        for (x = 0; x < pMap->width; x++) {
            colour = BrPixelmapPixelGet(pMap, x, y);
            colour_component = REC2_RGB555_B(colour);
            if (colour_component < *pDarkest) {
                *pDarkest = colour_component;
            }
            if (colour_component > *pBrightest) {
                *pBrightest = colour_component;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d70c0
int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3) {
    tPath_name the_path;
    int i;
    int x;
    int y;
    int tintedIndex;
    br_pixelmap* map;
    int darkest_colour;
    int brightest_colour;
    br_uint_32 colour;
    br_scalar colour_scale;

    tintedIndex = -1;
    for (i = 0; i < (int)REC2_ASIZE(gTintedPolys); i++) {
        if (!gTintedPolys[i].used) {
            tintedIndex = i;
            break;
        }
    }
    if (tintedIndex == -1) {
        return -1;
    }
    switch (class) {
        case 2:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            break;
        case 3:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].material->flags |= BR_MATF_PRELIT | BR_MATF_SMOOTH;
            BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 4, 4);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            gTintedPolys[tintedIndex].colour = arg1;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            break;
        case 4:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(0);
            gTintedPolys[tintedIndex].material->flags |= BR_MATF_QUAD_MAPPING;
            PathCat(the_path, gApplication_path, "fire");
            LoadAllImagesInDirectory(&gMisc_storage_space, the_path);
            map = BrMapFind("testfire");
            gTintedPolys[tintedIndex].pixelmap = map;
            gTintedPolys[tintedIndex].material->colour_map = map;
            BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);

            map = BrMapFind("firepal1");
            for (i = 0; i < 256; i++) {
                gTintedPolys[tintedIndex].colours[i] = BrPixelmapPixelGet(map, 0, i);
            }

            map = BrMapFind("coolmapx");
            GetRangeOfValuesFromPixelmap(map, &darkest_colour, &brightest_colour);

            // Map `coolmapx` colors to [0, arg1]
            colour_scale = (br_scalar)((double)(br_uint_32)arg1 / (brightest_colour - darkest_colour));
            for (y = 0; y < 64; y++) {
                for (x = 0; x < 32; x++) {
                    int v;
                    colour = BrPixelmapPixelGet(map, x, y);
                    /* Preserve x87 multiplication and __ftol's low 32 bits. */
                    v = (int)(br_int_64)(((int)REC2_RGB555_B(colour) - darkest_colour) * (double)colour_scale);
                    gTintedPolys[tintedIndex].tints1[y + x * 64] = v < 0 ? 0 : (br_uint_32)v > (br_uint_32)arg1 ? (br_uint_32)arg1 : (br_uint_32)v;
                }
            }
            memset(gTintedPolys[tintedIndex].tints2, 0, sizeof(gTintedPolys[tintedIndex].tints2));
            memset(gTintedPolys[tintedIndex].tints3, 0, sizeof(gTintedPolys[tintedIndex].tints3));

            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            break;
        case 5:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].material->flags |= BR_MATF_PRELIT | BR_MATF_SMOOTH;
            gTintedPolys[tintedIndex].material->extra_prim = gTinted_poly_zero_opacity_tokens;
            gTintedPolys[tintedIndex].material->extra_prim[1].v.x = BR_FIXED_INT(0);
            BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            gTintedPolys[tintedIndex].unknown_1 = 0;
            gTintedPolys[tintedIndex].unknown_2 = arg1;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            SetTintedPolyColour(tintedIndex, 0, 0, 0);
            break;
        case 6:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            gTintedPolys[tintedIndex].subClass = arg1;
            switch (arg1) {
                case 0:
                    gTintedPolys[tintedIndex].color2_red = REC2_RGB888_R(arg2);
                    gTintedPolys[tintedIndex].color2_grn = REC2_RGB888_G(arg2);
                    gTintedPolys[tintedIndex].color2_blu = REC2_RGB888_B(arg2);
                    gTintedPolys[tintedIndex].colour = arg3;
                    gTintedPolys[tintedIndex].material->colour = BR_COLOUR_RGB(gTintedPolys[tintedIndex].color2_red, gTintedPolys[tintedIndex].color2_grn, gTintedPolys[tintedIndex].color2_blu);
                    BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
                    break;
                case 1:
                    gTintedPolys[tintedIndex].material->flags |= BR_MATF_PRELIT | BR_MATF_SMOOTH;
                    BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
                    gTintedPolys[tintedIndex].colour = arg2;
                    break;
                default:
                    PDFatalError("Invalid Pulse Poly subclass");
            }
            break;
        default:
            gTintedPolys[tintedIndex].class = class;
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            BrModelAdd(gTintedPolys[tintedIndex].model);
            gTintedPolys[tintedIndex].material->colour = arg1;
            gTintedPolys[tintedIndex].material->extra_prim = gTinted_poly_custom_opacity_tokens;
            gTintedPolys[tintedIndex].material->extra_prim[1].v.x = BR_FIXED_INT(arg2);
            BrMaterialUpdate(gTintedPolys[tintedIndex].material, BR_MATU_ALL);
            break;
    }
    if (gTintedPolys[tintedIndex].actor != NULL) {
        BrActorAdd(gTintedPolyCamera, gTintedPolys[tintedIndex].actor);
    }
    gTintedPolys[tintedIndex].used = 1;
    if (gTintedPolys[tintedIndex].actor != NULL) {
        (gTintedPolys[tintedIndex].actor)->render_style = BR_RSTYLE_NONE;
    }
    gTintedPolys[tintedIndex].color_red = 0;
    gTintedPolys[tintedIndex].color_grn = 0;
    gTintedPolys[tintedIndex].color_blu = 0;
    gTintedPolys[tintedIndex].visible = 0;
    return tintedIndex;
}

// FUNCTION: CARMA2_HW 0x004d7ad0
br_model* C2_HOOK_FASTCALL BuildTintedPolyModel(int x0, int y0, int width, int height, int nbX, int nbY) {
    br_model* model;
    int dx;
    int dy;
    int step_x;
    int step_y;
    int i;
    int j;
    int vert_below;
    int vert_above;
    br_vertex* vertex;
    br_face* face;

    if (nbX <= 0 || nbY < 0) {
        return NULL;
    }
    model = BrModelAllocate("Tint Poly", (nbX + 1) * (nbY + 1), 2 * nbX * nbY);
    if (model == NULL) {
        return NULL;
    }
    step_x = width / nbX;
    step_y = height / nbY;
    vertex = model->vertices;
    for (j = 0; j < nbY + 1; j++) {
        dy = j * step_y;
        for (i = 0; i < nbX + 1; i++) {
            dx = i * step_x;
            vertex->p.v[0] = (br_scalar)(x0 + dx);
            vertex->p.v[1] = (br_scalar)(-(y0 + dy));
            vertex->p.v[2] = -1.02f;
            /* Keep integer operands exact through the floating divide, as in the original x87 code. */
            vertex->map.v[0] = (br_scalar)((double)dx / width);
            vertex->map.v[1] = (br_scalar)((double)dy / height);
            vertex++;
        }
    }
    face = model->faces;
    for (j = 0; j < nbY; j++) {
        vert_above = j * (nbX + 1);
        vert_below = (j + 1) * (nbX + 1);
        for (i = 0; i < nbX; i++) {
            face[0].vertices[0] = (br_uint_16)(vert_above + i );
            face[0].vertices[1] = (br_uint_16)(vert_above + i + 1);
            face[0].vertices[2] = (br_uint_16)(vert_below + i );
            face[1].vertices[0] = (br_uint_16)(vert_above + i + 1);
            face[1].vertices[1] = (br_uint_16)(vert_below + i + 1);
            face[1].vertices[2] = (br_uint_16)(vert_below + i);
            face += 2;
        }
    }
    model->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL;
    return model;
}

// FUNCTION: CARMA2_HW 0x004d82b0
void C2_HOOK_FASTCALL SetTintedPolyColour(int pTintedIndex, int pRed, int pGreen, int pBlue) {
    int changed;

    changed = 0;
    if (gTintedPolys[pTintedIndex].color_red != (br_uint_32)pRed) {
        gTintedPolys[pTintedIndex].color_red = pRed;
        changed = 1;
    }
    if (gTintedPolys[pTintedIndex].color_grn != (br_uint_32)pGreen) {
        gTintedPolys[pTintedIndex].color_grn = pGreen;
        changed = 1;
    }
    if (gTintedPolys[pTintedIndex].color_blu != (br_uint_32)pBlue) {
        gTintedPolys[pTintedIndex].color_blu = pBlue;
        changed = 1;
    }
    if (changed) {
        (gTintedPolys[pTintedIndex].material)->colour = BR_COLOUR_RGB(gTintedPolys[pTintedIndex].color_red, gTintedPolys[pTintedIndex].color_grn, gTintedPolys[pTintedIndex].color_blu);
        BrMaterialUpdate(gTintedPolys[pTintedIndex].material ,BR_MATU_LIGHTING);
    }
}

// FUNCTION: CARMA2_HW 0x004d7c80
void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex) {
    if (!gTintedPolys[pTintedIndex].used) {
        return;
    }
    if (gTintedPolys[pTintedIndex].material != NULL) {
        BrMaterialFree(gTintedPolys[pTintedIndex].material);
    }
    if (gTintedPolys[pTintedIndex].model != NULL) {
        BrModelFree(gTintedPolys[pTintedIndex].model);
    }
    if (gTintedPolys[pTintedIndex].actor != NULL) {
        BrActorRemove(gTintedPolys[pTintedIndex].actor);
        BrActorFree(gTintedPolys[pTintedIndex].actor);
        gTintedPolys[pTintedIndex].actor = NULL;
    }
    gTintedPolys[pTintedIndex].class = 0;
    gTintedPolys[pTintedIndex].material2 = NULL;
    gTintedPolys[pTintedIndex].visible = 0;
    gTintedPolys[pTintedIndex].used = 0;
}

// FUNCTION: CARMA2_HW 0x004d7d10
void C2_HOOK_FASTCALL FreeAllTintedPolyActor(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gTintedPolys); i++) {
        FreeTintedPolyActor(i);
    }
}

// FUNCTION: CARMA2_HW 0x004d8220
void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex) {
    if (gTintedPolys[pTintedIndex].used) {
        gTintedPolys[pTintedIndex].actor->render_style = BR_RSTYLE_FACES;
        gTintedPolys[pTintedIndex].visible = 1;
    }
}

// FUNCTION: CARMA2_HW 0x004d8250
void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex) {
    if (gTintedPolys[pTintedIndex].used) {
        (gTintedPolys[pTintedIndex].actor)->render_style = BR_RSTYLE_NONE;
        gTintedPolys[pTintedIndex].color_red = 0;
        gTintedPolys[pTintedIndex].color_grn = 0;
        gTintedPolys[pTintedIndex].color_blu = 0;
        gTintedPolys[pTintedIndex].visible = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004d8290
void C2_HOOK_FASTCALL RenderTintedPolys(void) {
    BrZbsSceneRender(gTintedPolyCamera, gTintedPolyCamera, gBack_screen, gDepth_buffer);
}

// FUNCTION: CARMA2_HW 0x004d8470
br_material* C2_HOOK_FASTCALL InWater(int pTintedIndex) {

    if (gTintedPolys[pTintedIndex].field_0x28 != '#') {
        return NULL;
    }
    return gTintedPolys[pTintedIndex].material2;
}

// FUNCTION: CARMA2_HW 0x004d8cf0
int C2_HOOK_FASTCALL TintedPolyIsOn(int pIndex) {

    return gTintedPolys[pIndex].visible;
}

// FUNCTION: CARMA2_HW 0x004d8bb0
void C2_HOOK_FASTCALL SetTintedPolySize(int pTintedIndex, int x0, int y0, int width, int height) {
    tTintedPoly* tinted;
    int nb_x;
    int nb_y;
    int step_x;
    int step_y;
    int i;
    int j;
    br_vertex* vertex;

    if (pTintedIndex == -1) {
        return;
    }
    tinted = &gTintedPolys[pTintedIndex];
    if (tinted->class == 3) {
        nb_x = 4;
        nb_y = 4;
    } else {
        nb_x = 1;
        nb_y = 1;
    }
    if (tinted->model == NULL) {
        return;
    }
    step_x = width / nb_x;
    step_y = height / nb_y;
    vertex = tinted->model->vertices;
    for (i = 0; i <= nb_y; i++) {
        int dy = i * step_y;
        for (j = 0; j <= nb_x; j++) {
            int dx = j * step_x;

            BrVector3Set(&vertex->p, (br_scalar)(x0 + dx), (br_scalar)(-(y0 + dy)), -1.02f);
            BrVector2Set(&vertex->map, (br_scalar)((double)dx / width), (br_scalar)((double)dy / height));
            vertex++;
        }
    }
    BrModelUpdate(tinted->model, BR_MODU_VERTICES);
}

// FUNCTION: CARMA2_HW 0x004d7d80
void C2_HOOK_FASTCALL UpdateTintedPolyByMaterial(int pTintedIndex) {
    br_uint_32 new_grn, new_blu, new_red;
    br_pixelmap* map;
    tU32 time;
    int material_changed;
    tU32 time_since_last_update_s;

    if (!gTintedPolys[pTintedIndex].visible || gTintedPolys[pTintedIndex].material2 == NULL) {
        return;
    }

    map = gTintedPolys[pTintedIndex].material2->colour_map;
    new_grn = gTintedColourMap_grn;
    new_blu = gTintedColourMap_blu;
    new_red = gTintedColourMap_red;
    if (map != NULL && map->pixels != NULL) {
        br_uint_32 v_tl, v_tr, v_bl, v_br;

        v_tl = BrPixelmapPixelGet(map, map->width / 4, map->height / 4);
        v_tr = BrPixelmapPixelGet(map, map->width - map->width / 4, map->height / 4);
        v_bl = BrPixelmapPixelGet(map, map->width / 4, map->height - map->height / 4);
        v_br = BrPixelmapPixelGet(map, map->width - map->width / 4, map->height - map->height / 4);

        new_red = (REC2_RGB888_R(v_tl) + REC2_RGB888_R(v_tr) + REC2_RGB888_R(v_bl) + REC2_RGB888_R(v_br)) / 4;
        new_grn = (REC2_RGB888_G(v_tl) + REC2_RGB888_G(v_tr) + REC2_RGB888_G(v_bl) + REC2_RGB888_G(v_br)) / 4;
        new_blu = (REC2_RGB888_B(v_tl) + REC2_RGB888_B(v_tr) + REC2_RGB888_B(v_bl) + REC2_RGB888_B(v_br)) / 4;
    }
    time = GetTotalTime();
    if (gTintedPolys[pTintedIndex].color_red + gTintedPolys[pTintedIndex].color_grn + gTintedPolys[pTintedIndex].color_blu == 0) {
        gTintedPolys[pTintedIndex].color_red = new_red;
        gTintedPolys[pTintedIndex].color_grn = new_grn;
        gTintedPolys[pTintedIndex].color_blu = new_blu;
        gTintedPolys[pTintedIndex].lastTime = time;
        gTintedPolys[pTintedIndex].material->colour = BR_COLOUR_RGB(new_red, new_grn, new_blu);
        BrMaterialUpdate(gTintedPolys[pTintedIndex].material, BR_MATU_LIGHTING);
        return;
    }

    material_changed = 0;
    time_since_last_update_s = (time - (tU32)gTintedPolys[pTintedIndex].lastTime) >> 12;
    if (time_since_last_update_s != 0) {
        if (gTintedPolys[pTintedIndex].color_red != new_red) {
            if ((br_int_32)gTintedPolys[pTintedIndex].color_red < (br_int_32)new_red) {
                gTintedPolys[pTintedIndex].color_red += time_since_last_update_s;
            } else {
                gTintedPolys[pTintedIndex].color_red -= time_since_last_update_s;
            }
            if ((br_int_32)(new_red - time_since_last_update_s) < (br_int_32)gTintedPolys[pTintedIndex].color_red && (br_int_32)gTintedPolys[pTintedIndex].color_red < (br_int_32)(time_since_last_update_s + new_red)) {
                gTintedPolys[pTintedIndex].color_red = new_red;
            }
            material_changed = 1;
        }
        if (gTintedPolys[pTintedIndex].color_grn != new_grn) {
            if ((br_int_32)gTintedPolys[pTintedIndex].color_grn < (br_int_32)new_grn) {
                gTintedPolys[pTintedIndex].color_grn += time_since_last_update_s;
            } else {
                gTintedPolys[pTintedIndex].color_grn -= time_since_last_update_s;
            }
            if ((br_int_32)(new_grn - time_since_last_update_s) < (br_int_32)gTintedPolys[pTintedIndex].color_grn && (br_int_32)gTintedPolys[pTintedIndex].color_grn < (br_int_32)(time_since_last_update_s + new_grn)) {
                gTintedPolys[pTintedIndex].color_grn = new_grn;
            }
            material_changed = 1;
        }
        if (gTintedPolys[pTintedIndex].color_blu != new_blu) {
            if ((br_int_32)gTintedPolys[pTintedIndex].color_blu < (br_int_32)new_blu) {
                gTintedPolys[pTintedIndex].color_blu += time_since_last_update_s;
            } else {
                gTintedPolys[pTintedIndex].color_blu -= time_since_last_update_s;
            }
            if ((br_int_32)(new_blu - time_since_last_update_s) < (br_int_32)gTintedPolys[pTintedIndex].color_blu && (br_int_32)gTintedPolys[pTintedIndex].color_blu < (br_int_32)(time_since_last_update_s + new_blu)) {
                gTintedPolys[pTintedIndex].color_blu = new_blu;
            }
            material_changed = 1;
        }
        if (material_changed) {
            gTintedPolys[pTintedIndex].material->colour = BR_COLOUR_RGB(gTintedPolys[pTintedIndex].color_red, gTintedPolys[pTintedIndex].color_grn, gTintedPolys[pTintedIndex].color_blu);
            BrMaterialUpdate(gTintedPolys[pTintedIndex].material, BR_MATU_LIGHTING);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d8050
void C2_HOOK_FASTCALL UpdatePolyVertexColours(int pTintedIndex) {
    br_uint_32 maximum;
    double scale;
    double tint_red;
    double tint_grn;
    double tint_blu;
    int i;

    if (gTinted_poly_palette == NULL || pTintedIndex == -1) {
        return;
    }
    maximum = (br_int_32)gTintedPolys[pTintedIndex].color_red > (br_int_32)gTintedPolys[pTintedIndex].color_grn
        ? gTintedPolys[pTintedIndex].color_red : gTintedPolys[pTintedIndex].color_grn;
    if (maximum <= gTintedPolys[pTintedIndex].color_blu) {
        maximum = gTintedPolys[pTintedIndex].color_blu;
    }
    scale = 50.0 / maximum;
    tint_red = 0.5 - ((br_int_32)gTintedPolys[pTintedIndex].color_red * scale) * -0.01;
    tint_grn = 0.5 - ((br_int_32)gTintedPolys[pTintedIndex].color_grn * scale) * -0.01;
    tint_blu = 0.5 - ((br_int_32)gTintedPolys[pTintedIndex].color_blu * scale) * -0.01;
    for (i = 1; i < 256; i++) {
        br_colour colour = gTinted_poly_palette[i];
        br_uint_32 red = REC2_RGB888_R(colour);
        br_uint_32 grn = REC2_RGB888_G(colour);
        br_uint_32 blu = REC2_RGB888_B(colour);
        br_uint_32 half_brightness = ((red + grn + blu) / 3) >> 1;

        if (2 * half_brightness != 0) {
            double amount = (100.0 / (2 * half_brightness)) * half_brightness * 0.01;
            br_uint_32 new_grn = (br_uint_32)(br_int_64)((half_brightness + grn) * tint_grn * amount) & 0xff;
            br_uint_32 new_red = (br_uint_32)(br_int_64)((half_brightness + red) * tint_red * amount) & 0xff;
            br_uint_32 new_blu = (br_uint_32)(br_int_64)((half_brightness + blu) * tint_blu * amount) & 0xff;

            ((br_colour*)gRender_palette->pixels)[i] = BR_COLOUR_RGB(new_red, new_grn, new_blu);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d8630
void C2_HOOK_FASTCALL UpdatePulsePoly(int pIndex) {

    if ((gTintedPolys[pIndex].actor)->render_style == BR_RSTYLE_FACES) {
        tU32 time;
        int i;
        tU8 new_color;

        time = GetTotalTime();

        new_color = (tU8)(time >> (gTintedPolys[pIndex].colour & 31));
        for (i = 0; i < gTintedPolys[pIndex].model->nvertices; i++) {
            gTintedPolys[pIndex].model->vertices[i].red = new_color + 128;
            gTintedPolys[pIndex].model->vertices[i].grn = new_color + 30 * i;
            gTintedPolys[pIndex].model->vertices[i].blu = new_color;
        }
        BrModelUpdate(gTintedPolys[pIndex].model, BR_MODU_VERTICES);
    }
}

// FUNCTION: CARMA2_HW 0x004d86e0
void C2_HOOK_FASTCALL UpdateFirePoly(int pIndex) {
    int i;
    int j;

    if (!gTinted_fire_frame) {
        C2_HOOK_BUG_ON(sizeof(gTintedPolys[pIndex].tints2) != 8192);
        C2_HOOK_BUG_ON(sizeof(gTintedPolys[pIndex].tints3) != 8192);

        memset(gTintedPolys[pIndex].tints2, 0, sizeof(gTintedPolys[pIndex].tints2));
        memset(gTintedPolys[pIndex].tints3, 0, sizeof(gTintedPolys[pIndex].tints2));
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            int heat = 255 - gTinted_fire_cooling;
            int offset = IRandomBetween(i - 5, 5 - i);
            int row = IRandomBetween(13, 19);

            gTintedPolys[pIndex].tints2[64 * (offset + row) + i + 64 - 5] = heat;
        }
    }
    if (gTinted_fire_frame > 150) {
        gTinted_fire_cooling += 1;
    }
    gTinted_fire_phase += 1;
    if (gTinted_fire_phase >= 64) {
        gTinted_fire_phase = 0;
    }

    for (i = 0; i < 62; i++) {
        for (j = 0; j < 30; j++) {
            int avg =
                ((gTintedPolys[pIndex].tints2[i + (j + 2) * 64 + 1]
                + gTintedPolys[pIndex].tints2[i + (j + 0) * 64 + 1]
                + gTintedPolys[pIndex].tints2[i + (j + 1) * 64 + 2]
                + gTintedPolys[pIndex].tints2[i + (j + 1) * 64 + 0]) / 4
                + gTintedPolys[pIndex].tints2[i + (j + 1) * 64 + 1]) / 2
            - gTintedPolys[pIndex].tints1[i + (j + 1) * 64 + 1]
            - gTinted_fire_cooling;
            gTintedPolys[pIndex].tints3[i + (j + 1) * 64] = MAX(0, avg);
        }
    }

    if (gTinted_fire_frame < 150 && IRandomBetween(0, 6) > 3) {
        int idx;

        int row = IRandomBetween(12, 20);
        int column = IRandomBetween(30, 55);

        idx = row * 64 + column;
        gTintedPolys[pIndex].tints2[idx] = 0xff;
        gTintedPolys[pIndex].tints3[idx] = 0xbf;
    }
    memcpy(gTintedPolys[pIndex].tints2, gTintedPolys[pIndex].tints3, sizeof(gTintedPolys[pIndex].tints2));

    for (i = 0; i < 64; i++) {
        for (j = 0; j < 32; j++) {
            int idx = gTintedPolys[pIndex].tints3[j * 64 + i];
            BrPixelmapPixelSet(gTintedPolys[pIndex].pixelmap, j, i, gTintedPolys[pIndex].colours[idx]);
        }
    }
    BrMapUpdate(gTintedPolys[pIndex].pixelmap, BR_MAPU_ALL);

    if (gTinted_fire_frame % 10 == 0 && gTinted_fire_save && gTinted_fire_frame > 10) {
        tPath_name save_path;
        for (i = 1; i < 10000; i++) {
            tPath_name path;
            FILE* f;

            PathCat(path, gApplication_path, "FLM");
            sprintf(&path[strlen(path)], "%02d.%s", i, "PIX");
            f = DRfopen(path, "rt");
            if (f == NULL) {
                sprintf(save_path, "%s", path);
                break;
            }
            fclose(f);
        }
        BrPixelmapSave(save_path, gTintedPolys[pIndex].pixelmap);
    }
    gTinted_fire_frame += 1;
    if (gTinted_fire_frame >= 200) {
        gTinted_fire_frame = 0;
        gTinted_fire_save = 0;
        gTinted_fire_cooling = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004d84a0
void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex) {

    if (!gTintedPolys[pIndex].used) {
        return;
    }
    switch (gTintedPolys[pIndex].class) {
    case 2: {
        tSpecial_volume *volume = gCar_to_view->collision_info->last_special_volume;
        /* The original x87 below test also accepts unordered gravity. */
        if ((volume != NULL && !(volume->gravity_multiplier >= 1.0f)) || gAction_replay_mode) {
            UpdateTintedPolyByMaterial(pIndex);
        }
        break;
    }
    case 3:
        UpdatePulsePoly(pIndex);
        return;
    case 4:
        if (PDKeyDown(32) || (gTinted_fire_frame > 0 && gTinted_fire_frame <= 200)) {
            UpdateFirePoly(pIndex);
        }
        if (PDKeyDown(35) || (gTinted_fire_frame > 0 && gTinted_fire_frame <= 200)) {
            gTinted_fire_save = 1;
            UpdateFirePoly(pIndex);
        }
        break;
    case 6:
        if (gTintedPolys[pIndex].visible) {
            int red, grn, blu;

            if (gTintedPolys[pIndex].subClass == 0) {
                unsigned int mask;
                mask = GetTotalTime() >> (gTintedPolys[pIndex].colour & 31);
                if ((mask & 0x1ff) > 254) {
                    mask = -2 - (mask & 0x1ff);
                }
                mask &= 0xff;
                red = gTintedPolys[pIndex].color2_red & mask;
                grn = gTintedPolys[pIndex].color2_grn & mask;
                blu = gTintedPolys[pIndex].color2_blu & mask;
            } else {
                red = grn = blu = pIndex;
                if (gTintedPolys[pIndex].subClass == 1) {
                    UpdatePulsePoly(pIndex);
                }
            }
            gTintedPolys[pIndex].material->colour = BR_COLOUR_RGB(red, grn, blu);
            BrMaterialUpdate(gTintedPolys[pIndex].material, BR_MATU_LIGHTING);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d8a20
void C2_HOOK_FASTCALL UpdateTintedPolys(void) {
    int i;
    br_uint_32 red = 0;
    br_uint_32 grn = 0;
    br_uint_32 blu = 0;

    /* The original inlined pulse path retains these colours between slots. */
    for (i = 0; i < (int)REC2_ASIZE(gTintedPolys); i++) {
        if (!gTintedPolys[i].used) {
            continue;
        }
        switch (gTintedPolys[i].class) {
        case 2: {
            tSpecial_volume *volume = gCar_to_view->collision_info->last_special_volume;
            if ((volume != NULL && !(volume->gravity_multiplier >= 1.0f)) || gAction_replay_mode) {
                UpdateTintedPolyByMaterial(i);
            }
            break;
        }
        case 3:
            UpdatePulsePoly(i);
            break;
        case 4:
            if (PDKeyDown(32) || (gTinted_fire_frame > 0 && gTinted_fire_frame <= 200)) {
                UpdateFirePoly(i);
            }
            if (PDKeyDown(35) || (gTinted_fire_frame > 0 && gTinted_fire_frame <= 200)) {
                gTinted_fire_save = 1;
                UpdateFirePoly(i);
            }
            break;
        case 6:
            if (gTintedPolys[i].visible) {
                if (gTintedPolys[i].subClass == 0) {
                    unsigned int mask;
                    mask = GetTotalTime() >> (gTintedPolys[i].colour & 31);
                    if ((mask & 0x1ff) > 254) {
                        mask = -2 - (mask & 0x1ff);
                    }
                    mask &= 0xff;
                    red = gTintedPolys[i].color2_red & mask;
                    grn = gTintedPolys[i].color2_grn & mask;
                    blu = gTintedPolys[i].color2_blu & mask;
                } else {
                    if (gTintedPolys[i].subClass == 1) {
                        UpdatePulsePoly(i);
                    }
                }
                gTintedPolys[i].material->colour = BR_COLOUR_RGB(red, grn, blu);
                BrMaterialUpdate(gTintedPolys[i].material, BR_MATU_LIGHTING);
            }
        }
    }
}
