/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "66-tintedpoly.h"

#include "00-car.h"
#include "08-loading1.h"
#include "18-graphics2.h"
#include "28-world3.h"
#include "34-finteray.h"
#include "41-utility.h"
#include "42-input.h"
#include "globvars.h"
#include "globvrkm.h"
#include "platform.h"
#include "rec2_macros.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0065e560
// GLOBAL: CARMA2_SW 0x00658e98
const char* gForm_non_humanoid_names[1] = {
    "NONHUMANOID",
};

// GLOBAL: CARMA2_HW 0x00705c80
// GLOBAL: CARMA2_SW 0x006fc740
tTintedPoly gTintedPolys[10];

// GLOBAL: CARMA2_HW 0x006a0430
// GLOBAL: CARMA2_SW 0x0069a488
br_actor* gTintedPolyCamera;

#ifdef REC2_FIX_BUGS
#define TINTED_COUNT_TOKENS 3
#else
#define TINTED_COUNT_TOKENS 2
#endif

#define FIRE_WIDTH  64
#define FIRE_HEIGHT 32

// GLOBAL: CARMA2_HW 0x0065e840
// GLOBAL: CARMA2_SW 0x00659190
br_token_value gTinted_poly_material_tokens[TINTED_COUNT_TOKENS] = {
  { BRT_BLEND_B, { 1 } },
  { BRT_OPACITY_X, { BR_FIXED_INT(0x80) } },
};

// GLOBAL: CARMA2_HW 0x0065e850
// GLOBAL: CARMA2_SW 0x006591a0
br_token_value gTinted_poly_zero_opacity_tokens[TINTED_COUNT_TOKENS] = {
  { BRT_BLEND_B, { 1 } },
  { BRT_OPACITY_X, { BR_FIXED_INT(0x80) } },
};

// GLOBAL: CARMA2_HW 0x0065e860
// GLOBAL: CARMA2_SW 0x006591b0
br_token_value gTinted_poly_custom_opacity_tokens[TINTED_COUNT_TOKENS] = {
  { BRT_BLEND_B, { 1 } },
  { BRT_OPACITY_X, { BR_FIXED_INT(0x80) } },
};

// GLOBAL: CARMA2_HW 0x0065e870
// GLOBAL: CARMA2_SW 0x006591c0
br_uint_32 gTintedColourMap_red = 0x80;

// GLOBAL: CARMA2_HW 0x006a0438
// GLOBAL: CARMA2_SW 0x0069a490
br_uint_32 gTintedColourMap_grn;

// GLOBAL: CARMA2_HW 0x006a043c
// GLOBAL: CARMA2_SW 0x0069a494
br_uint_32 gTintedColourMap_blu;

// GLOBAL: CARMA2_HW 0x006a0440
// GLOBAL: CARMA2_SW 0x0069a498
int gFire_poly_short_iteration;

// GLOBAL: CARMA2_HW 0x006a0444
// GLOBAL: CARMA2_SW 0x0069a49c
int gFire_poly_iteration;

// GLOBAL: CARMA2_HW 0x006a0448
// GLOBAL: CARMA2_SW 0x0069a4a0
int gFire_poly_enabled_006a0448;

// GLOBAL: CARMA2_HW 0x006a044c
// GLOBAL: CARMA2_SW 0x0069a4a4
int gFire_poly_delayed_iteration;

// GLOBAL: CARMA2_HW 0x0065e874
// GLOBAL: CARMA2_SW 0x006591c4
int gDefaultOpacity_TintedPoly = 0x80;

#ifdef REC2_SW
// GLOBAL: CARMA2_SW 0x0069a48c
br_colour* gTinted_original_palette_colours;
#endif


// FUNCTION: CARMA2_HW 0x004d7040
// FUNCTION: CARMA2_SW 0x004d3640
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
// FUNCTION: CARMA2_SW 0x004d36c0
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
#ifdef REC2_HW
            gTintedPolys[tintedIndex].material = BuildTintedPolyMaterial(gDefaultOpacity_TintedPoly);
            gTintedPolys[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            gTintedPolys[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gTintedPolys[tintedIndex].actor->material = gTintedPolys[tintedIndex].material;
            for (i = 0; i < gTintedPolys[tintedIndex].model->nfaces; i++) {
                gTintedPolys[tintedIndex].model->faces[i].material = gTintedPolys[tintedIndex].material;
            }
            gTintedPolys[tintedIndex].actor->model = gTintedPolys[tintedIndex].model;
            BrModelAdd(gTintedPolys[tintedIndex].model);
#endif
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
                    gTintedPolys[tintedIndex].tints1[y][x] = v < 0 ? 0 : (br_uint_32)v > (br_uint_32)arg1 ? (br_uint_32)arg1 : (br_uint_32)v;
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
// FUNCTION: CARMA2_SW 0x004d3fe0
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
            face[0].vertices[0] = (br_uint_16)(vert_above + i);
            face[0].vertices[1] = (br_uint_16)(vert_above + i + 1);
            face[0].vertices[2] = (br_uint_16)(vert_below + i);
            face[1].vertices[0] = (br_uint_16)(vert_above + i + 1);
            face[1].vertices[1] = (br_uint_16)(vert_below + i + 1);
            face[1].vertices[2] = (br_uint_16)(vert_below + i);
            face += 2;
        }
    }
    model->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL;
    return model;
}

// FUNCTION: CARMA2_HW 0x004d7c80
// FUNCTION: CARMA2_SW 0x004d4190
static void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex) {
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
// FUNCTION: CARMA2_SW 0x004d4220
void C2_HOOK_FASTCALL FreeAllTintedPolyActor(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gTintedPolys); i++) {
        FreeTintedPolyActor(i);
    }
}

// FUNCTION: CARMA2_HW 0x004d7d80
// FUNCTION: CARMA2_SW 0x004d4290
void C2_HOOK_FASTCALL UpdateTintedPolyByMaterial(int pIndex) {
    br_uint_32 new_grn, new_blu, new_red;
    tU32 time;
    int material_changed;
    tU32 time_since_last_update_s;

    if (gTintedPolys[pIndex].visible && gTintedPolys[pIndex].material2 != NULL) {

        if (gTintedPolys[pIndex].material->colour_map == NULL || gTintedPolys[pIndex].material->colour_map->pixels == NULL) {
            new_red = gTintedColourMap_red;
            new_grn = gTintedColourMap_grn;
            new_blu = gTintedColourMap_blu;
        } else {
            br_uint_32 v_tl, v_tr, v_bl, v_br;
            br_uint_32 w = gTintedPolys[pIndex].material->colour_map->width;
            br_uint_32 h = gTintedPolys[pIndex].material->colour_map->height;
            br_uint_32 quarter_width = w / 4;
            br_uint_32 quarter_height = h / 4;

            v_tl = BrPixelmapPixelGet(gTintedPolys[pIndex].material2->colour_map, quarter_width, quarter_height);
#ifdef REC2_SW
            v_tl = gTinted_original_palette_colours[v_tl];
#endif
            v_tr = BrPixelmapPixelGet(gTintedPolys[pIndex].material2->colour_map, w - quarter_width, quarter_height);
#ifdef REC2_SW
            v_tr = gTinted_original_palette_colours[v_tr];
#endif
            v_bl = BrPixelmapPixelGet(gTintedPolys[pIndex].material2->colour_map, quarter_width, quarter_height);
#ifdef REC2_SW
            v_bl = gTinted_original_palette_colours[v_bl];
#endif
            v_br = BrPixelmapPixelGet(gTintedPolys[pIndex].material2->colour_map, w - quarter_width, h - quarter_height);
#ifdef REC2_SW
            v_br = gTinted_original_palette_colours[v_br];
#endif

            new_grn = (REC2_RGB888_G(v_tl) + REC2_RGB888_G(v_tr) + REC2_RGB888_G(v_bl) + REC2_RGB888_G(v_br)) / 4;
            new_blu = (REC2_RGB888_B(v_tl) + REC2_RGB888_B(v_tr) + REC2_RGB888_B(v_bl) + REC2_RGB888_B(v_br)) / 4;
            new_red = (REC2_RGB888_R(v_tl) + REC2_RGB888_R(v_tr) + REC2_RGB888_R(v_bl) + REC2_RGB888_R(v_br)) / 4;
        }
        time = GetTotalTime();
        if (gTintedPolys[pIndex].color_red + gTintedPolys[pIndex].color_grn + gTintedPolys[pIndex].color_blu == 0) {
            gTintedPolys[pIndex].color_red = new_red;
            gTintedPolys[pIndex].color_grn = new_grn;
            gTintedPolys[pIndex].color_blu = new_blu;
            gTintedPolys[pIndex].lastTime = time;
#ifdef REC2_HW
            gTintedPolys[pIndex].material->colour = BR_COLOUR_RGB(new_red, new_grn, new_blu);
            BrMaterialUpdate(gTintedPolys[pIndex].material, BR_MATU_LIGHTING);
            return;
#endif
        } else {
            material_changed = 0;
            time_since_last_update_s = (time - gTintedPolys[pIndex].lastTime) / 4096;
            if (time_since_last_update_s != 0) {
                if (gTintedPolys[pIndex].color_red != new_red) {
                    if (gTintedPolys[pIndex].color_red < new_red) {
                        gTintedPolys[pIndex].color_red += time_since_last_update_s;
                    } else {
                        gTintedPolys[pIndex].color_red -= time_since_last_update_s;
                    }
                    if (new_red - time_since_last_update_s < gTintedPolys[pIndex].color_red && gTintedPolys[pIndex].color_red < time_since_last_update_s + new_red) {
                        gTintedPolys[pIndex].color_red = new_red;
                    }
                    material_changed = 1;
                }
                if (gTintedPolys[pIndex].color_grn != new_grn) {
                    if (gTintedPolys[pIndex].color_grn < new_grn) {
                        gTintedPolys[pIndex].color_grn += time_since_last_update_s;
                    } else {
                        gTintedPolys[pIndex].color_grn -= time_since_last_update_s;
                    }
                    if (new_grn - time_since_last_update_s < gTintedPolys[pIndex].color_grn && gTintedPolys[pIndex].color_grn < time_since_last_update_s + new_grn) {
                        gTintedPolys[pIndex].color_grn = new_grn;
                    }
                    material_changed = 1;
                }
                if (gTintedPolys[pIndex].color_blu != new_blu) {
                    if (gTintedPolys[pIndex].color_blu < new_blu) {
                        gTintedPolys[pIndex].color_blu += time_since_last_update_s;
                    } else {
                        gTintedPolys[pIndex].color_blu -= time_since_last_update_s;
                    }
                    if (new_blu - time_since_last_update_s < gTintedPolys[pIndex].color_blu && gTintedPolys[pIndex].color_blu < time_since_last_update_s + new_blu) {
                        gTintedPolys[pIndex].color_blu = new_blu;
                    }
                    material_changed = 1;
                }
#ifdef REC2_HW
                if (material_changed) {
                    gTintedPolys[pIndex].material->colour = BR_COLOUR_RGB(gTintedPolys[pIndex].color_red, gTintedPolys[pIndex].color_grn, gTintedPolys[pIndex].color_blu);
                    BrMaterialUpdate(gTintedPolys[pIndex].material, BR_MATU_LIGHTING);
                }
#endif
#ifdef REC2_SW
                if (!material_changed) {
                    return;
                }
#endif
            }
        }
#ifdef REC2_SW
        UpdateTintedPalette(pIndex);
        ResetPalette();
#endif
    }
}

#ifdef REC2_SW
#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(off)
#endif

// STUB: CARMA2_SW 0x004d4520
void C2_HOOK_FASTCALL UpdateTintedPalette(int pIndex) {
    NOT_IMPLEMENTED();
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma auto_inline(on)
#endif
#endif

// FUNCTION: CARMA2_HW 0x004d8220
// FUNCTION: CARMA2_SW 0x004d46f0
void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex) {
#ifdef REC2_HW
    if (gTintedPolys[pTintedIndex].used) {
        gTintedPolys[pTintedIndex].actor->render_style = BR_RSTYLE_FACES;
        gTintedPolys[pTintedIndex].visible = 1;
    }
#endif
#ifdef REC2_SW
    if (gTintedPolys[pTintedIndex].used && !gTintedPolys[pTintedIndex].visible) {
        gTintedPolys[pTintedIndex].visible = 1;
        if (gTintedPolys[pTintedIndex].class == 2) {
            if (gTinted_original_palette_colours == NULL) {
                gTinted_original_palette_colours = BrMemAllocate(256 * sizeof(br_colour), kMem_misc);
                memcpy(gTinted_original_palette_colours, gRender_palette->pixels, 256 * sizeof(br_colour));
            }
            ResetPalette();
        }
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004d8250
// FUNCTION: CARMA2_SW 0x004d4770
void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex) {
#ifdef REC2_HW
    if (gTintedPolys[pTintedIndex].used) {
#elif defined(REC2_SW)
    if (gTintedPolys[pTintedIndex].visible) {
#endif

#ifdef REC2_HW
        gTintedPolys[pTintedIndex].actor->render_style = BR_RSTYLE_NONE;
#endif
#ifdef REC2_SW
        if (gTintedPolys[pTintedIndex].class == 2) {
            memcpy(gRender_palette->pixels, gTinted_original_palette_colours, 256 * sizeof(br_colour));
            ResetPalette();
        }
#endif
        gTintedPolys[pTintedIndex].color_red = 0;
        gTintedPolys[pTintedIndex].color_grn = 0;
        gTintedPolys[pTintedIndex].color_blu = 0;
        gTintedPolys[pTintedIndex].visible = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004d8290
// FUNCTION: CARMA2_SW 0x004d47d0
void C2_HOOK_FASTCALL RenderTintedPolys(void) {
#ifdef REC2_HW
    BrZbsSceneRender(gTintedPolyCamera, gTintedPolyCamera, gBack_screen, gDepth_buffer);
#endif
}

// FUNCTION: CARMA2_HW 0x004d8350
// FUNCTION: CARMA2_SW 0x004d4880
void C2_HOOK_FASTCALL SetTintedPolyRefMaterial(int pTintedIndex, br_vector3* pPosition) {
    tSpecial_volume* volume;
    br_vector3 dir;
    br_vector3 nor;
    br_scalar t;
    br_material* mat;

    volume = gCar_to_view->collision_info->last_special_volume;
    if ((volume != NULL && volume->gravity_multiplier < 1.0f) || gAction_replay_mode) {
        BrVector3Set(&dir, 0.0f, 200.0f, 0.0f);
        DisablePlingMaterials();
        FindFace(pPosition, &dir, &nor, &t, &mat);
        EnablePlingMaterials();
        if (!(t < 100.0f) || mat == NULL || mat->identifier == NULL || strlen(mat->identifier) == 0) {
            gTintedPolys[pTintedIndex].field_0x28 = 0;
            gTintedPolys[pTintedIndex].material2 = NULL;
        } else if (mat->identifier[0] == '!' || mat->identifier[0] == '#') {
            gTintedPolys[pTintedIndex].field_0x28 = '#';
            gTintedPolys[pTintedIndex].material2 = mat;
        } else if (mat->identifier[0] == '@') {
            gTintedPolys[pTintedIndex].field_0x28 = '@';
            gTintedPolys[pTintedIndex].material2 = mat;
        } else {
            gTintedPolys[pTintedIndex].field_0x28 = 0;
            gTintedPolys[pTintedIndex].material2 = NULL;
        }
    } else {
        gTintedPolys[pTintedIndex].field_0x28 = 0;
        gTintedPolys[pTintedIndex].material2 = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004d82b0
// FUNCTION: CARMA2_SW 0x004d47e0
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
        gTintedPolys[pTintedIndex].material->colour = BR_COLOUR_RGB(gTintedPolys[pTintedIndex].color_red, gTintedPolys[pTintedIndex].color_grn, gTintedPolys[pTintedIndex].color_blu);
        BrMaterialUpdate(gTintedPolys[pTintedIndex].material ,BR_MATU_LIGHTING);
    }
}

// FUNCTION: CARMA2_HW 0x004d8470
// FUNCTION: CARMA2_SW 0x004d49a0
br_material* C2_HOOK_FASTCALL InWater(int pTintedIndex) {

    if (gTintedPolys[pTintedIndex].field_0x28 == '#') {
        return gTintedPolys[pTintedIndex].material2;
    }
    return NULL;
}

static void C2_HOOK_FASTCALL UpdatePulsePoly(int pIndex) {
    unsigned int red;
    unsigned int grn;
    unsigned int blu;
    unsigned int value;

    if (gTintedPolys[pIndex].visible) {
        switch (gTintedPolys[pIndex].subClass) {
        case 1:
            UpdatePolyVertexColours(pIndex);
            /* fall through */
        default:
            red = grn = blu = pIndex;
            break;
        case 0:
            value = (GetTotalTime() >> gTintedPolys[pIndex].colour) & 0x1ff;
            if (value < 255) {
                value &= 0xff;
            } else {
                value = (-2 - value) & 0xff;
            }
            red = gTintedPolys[pIndex].color2_red & value;
            grn = gTintedPolys[pIndex].color2_grn & value;
            blu = gTintedPolys[pIndex].color2_blu & value;
            break;
        }
        gTintedPolys[pIndex].material->colour = BR_COLOUR_RGB(red, grn, blu);
        BrMaterialUpdate(gTintedPolys[pIndex].material, BR_MATU_LIGHTING);
    }
}

// FUNCTION: CARMA2_HW 0x004d84a0
// FUNCTION: CARMA2_SW 0x004d49d0
void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex) {

    if (gTintedPolys[pIndex].used) {
        switch (gTintedPolys[pIndex].class) {
        case 2: {
            tSpecial_volume *volume = gCar_to_view->collision_info->last_special_volume;
            if ((volume != NULL && volume->gravity_multiplier < 1.0f) || gAction_replay_mode) {
                UpdateTintedPolyByMaterial(pIndex);
                return;
            }
            break;
        }
        case 3:
            UpdatePolyVertexColours(pIndex);
            return;
        case 4:
            if (PDKeyDown(32) || (gFire_poly_iteration > 0 && gFire_poly_iteration <= 200)) {
                UpdateFirePoly(pIndex);
            }
            if (PDKeyDown(35) || (gFire_poly_iteration > 0 && gFire_poly_iteration <= 200)) {
                gFire_poly_enabled_006a0448 = 1;
                UpdateFirePoly(pIndex);
                return;
            }
            break;
        case 6:
            UpdatePulsePoly(pIndex);
            break;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004d8630
// FUNCTION: CARMA2_SW 0x004d4b60
void C2_HOOK_FASTCALL UpdatePolyVertexColours(int pIndex) {
    int i;
    int new_color;

    if (gTintedPolys[pIndex].actor->render_style == BR_RSTYLE_FACES) {
        new_color = GetTotalTime() >> gTintedPolys[pIndex].colour;
        for (i = 0; i < gTintedPolys[pIndex].model->nvertices; i++) {
            gTintedPolys[pIndex].model->vertices[i].red = new_color + 128;
            gTintedPolys[pIndex].model->vertices[i].grn = new_color + 30 * i;
            gTintedPolys[pIndex].model->vertices[i].blu = new_color;
        }
        BrModelUpdate(gTintedPolys[pIndex].model, BR_MODU_VERTICES);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL ClearTheFlame(int pIndex) {
    C2_HOOK_BUG_ON(REC2_ASIZE(gTintedPolys[pIndex].tints2) != FIRE_HEIGHT);
    C2_HOOK_BUG_ON(REC2_ASIZE(gTintedPolys[pIndex].tints2[0]) != FIRE_WIDTH);
    C2_HOOK_BUG_ON(sizeof(gTintedPolys[pIndex].tints2) != 8192);
    C2_HOOK_BUG_ON(sizeof(gTintedPolys[pIndex].tints3) != 8192);

    memset(gTintedPolys[pIndex].tints2, 0, sizeof(gTintedPolys[pIndex].tints2));
    memset(gTintedPolys[pIndex].tints3, 0, sizeof(gTintedPolys[pIndex].tints2));
}

void C2_HOOK_FASTCALL GetUniqueFileName(const char *pDirectory, const char *pExtension, char *pDest) {
    tPath_name path;
    FILE* f;
    int i;

    for (i = 1; i < 10000; i++) {

        PathCat(path, gApplication_path, pDirectory);
        sprintf(&path[strlen(path)], "%02d.%s", i, pExtension);
        f = DRfopen(path, "rt");
        if (f == NULL) {
          sprintf(pDest, "%s", path);
          break;
        }
        fclose(f);
    }
}

// FUNCTION: CARMA2_HW 0x004d86e0
// FUNCTION: CARMA2_SW 0x004d4c10
void C2_HOOK_FASTCALL UpdateFirePoly(int pIndex) {
    int i;
    int j;

    if (gFire_poly_iteration == 0) {
        ClearTheFlame(pIndex);
    }

    /* Insert heat */
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            gTintedPolys[pIndex].tints2[IRandomBetween(-(5 - i), 5 - i) + IRandomBetween(13, 19)][i + 64 - 5] = 255 - gFire_poly_delayed_iteration;
        }
    }
    if (gFire_poly_iteration > 150) {
        gFire_poly_delayed_iteration += 1;
    }
    gFire_poly_short_iteration += 1;
    if (gFire_poly_short_iteration >= 64) {
        gFire_poly_short_iteration = 0;
    }

    /* Propagate heat */
    for (i = 1; i < FIRE_WIDTH - 1; i++) {
        for (j = 1; j < FIRE_HEIGHT - 1; j++) {
            /* Average heat left-right-up-below */
            int avg =
                ((gTintedPolys[pIndex].tints2[j + 1][i + 0]       /* up */
                + gTintedPolys[pIndex].tints2[j - 1][i + 0]       /* below */
                + gTintedPolys[pIndex].tints2[j + 0][i + 1]       /* right */
                + gTintedPolys[pIndex].tints2[j + 0][i - 1]) / 4  /* left */
                + gTintedPolys[pIndex].tints2[j][i]) / 2          /* pixel itself */
            - gTintedPolys[pIndex].tints1[j][i]
            - gFire_poly_delayed_iteration;
            gTintedPolys[pIndex].tints3[j][i + 1] = MAX(0, avg);
        }
    }

    /* Add sparks */
    if (gFire_poly_iteration < 150 && IRandomBetween(0, 6) > 3) {
        int y;
        int x;

        y = IRandomBetween(12, 20);
        x = IRandomBetween(30, 55);
        gTintedPolys[pIndex].tints2[y][x] = 0xff;
        gTintedPolys[pIndex].tints3[y][x] = 0xbf;
    }
    memcpy(gTintedPolys[pIndex].tints2, gTintedPolys[pIndex].tints3, sizeof(gTintedPolys[pIndex].tints2));

    for (i = 0; i < FIRE_WIDTH; i++) {
        for (j = 0; j < FIRE_HEIGHT; j++) {
            int idx = gTintedPolys[pIndex].tints3[j][i];
            BrPixelmapPixelSet(gTintedPolys[pIndex].pixelmap, i, j, gTintedPolys[pIndex].colours[idx]);
        }
    }
    BrMapUpdate(gTintedPolys[pIndex].pixelmap, BR_MAPU_ALL);

    if (gFire_poly_iteration % 10 == 0 && gFire_poly_enabled_006a0448 && gFire_poly_iteration > 10) {
        tPath_name save_path;
        GetUniqueFileName("FLM", "PIX", save_path);
        BrPixelmapSave(save_path, gTintedPolys[pIndex].pixelmap);
    }
    gFire_poly_iteration += 1;
    if (gFire_poly_iteration >= 200) {
        gFire_poly_iteration = 0;
        gFire_poly_enabled_006a0448 = 0;
        gFire_poly_delayed_iteration = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004d8a20
// FUNCTION: CARMA2_SW 0x004d4f50
void C2_HOOK_FASTCALL UpdateTintedPolys(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(gTintedPolys); i++) {
        ProcessTintedPoly(i);
    }
}

// FUNCTION: CARMA2_HW 0x004d8bb0
// FUNCTION: CARMA2_SW 0x004d50e0
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
            BrVector2Set(&vertex->map, (br_scalar)dx / (br_scalar)width, (br_scalar)dy / (br_scalar)height);
            vertex++;
        }
    }
    BrModelUpdate(tinted->model, BR_MODU_VERTICES);
}

// FUNCTION: CARMA2_HW 0x004d8cf0
// FUNCTION: CARMA2_SW 0x004d5220
int C2_HOOK_FASTCALL TintedPolyIsOn(int pIndex) {

  return gTintedPolys[pIndex].visible;
}
