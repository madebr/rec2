#include "tinted.h"

#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "input.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"
#include "world.h"

#include "rec2_macros.h"
#include "rec2_types.h"
#include "brender/brender.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHud_tinted1, 0x00655e48, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHud_tinted2, 0x00655e4c, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHud_tinted3, 0x00655e50, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gHud_tinted4, 0x0065fb54, -1);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTintedPoly, gTintedPolys, 10, 0x00705c80);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gTintedPolyCamera, 0x006a0430);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDefaultOpacity_TintedPoly, 0x0065e874, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_uint_32, gTintedColourMap_red, 0x0065e870, 0x80);
C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, gTintedColourMap_grn, 0x006a0438);
C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, gTintedColourMap_blu, 0x006a043c);

C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006a0440, 0x006a0440);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006a0444, 0x006a0444);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006a0448, 0x006a0448);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006a044c, 0x006a044c);

void C2_HOOK_FASTCALL InitTintedPolyStuff(void) {
    br_camera *camera;

    C2_HOOK_BUG_ON(sizeof(tTintedPoly) != 0x6450);

    c2_memset(C2V(gTintedPolys), 0, sizeof(C2V(gTintedPolys)));

    if (C2V(gTintedPolyCamera) == NULL) {
        C2V(gTintedPolyCamera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
        C2V(gTintedPolyCamera)->identifier = "tinted_poly_camera";

        camera = C2V(gTintedPolyCamera)->type_data;
        camera->type = BR_CAMERA_PARALLEL;
        camera->hither_z = 1.0f;
        camera->yon_z = 3.0f;
        camera->width = C2V(gScreen)->width;
        camera->height = C2V(gScreen)->height;
    }
}
C2_HOOK_FUNCTION(0x004d7040, InitTintedPolyStuff);

br_material* C2_HOOK_FASTCALL BuildTintedPolyMaterial(int pOpacity) {
    static br_token_value extra_prims[2] = {
        {
            BRT_BLEND_B,
            { 1 },
        },
        {
            BRT_OPACITY_X,
            { BR_FIXED_INT(0x80), }
        },
    };
    br_material *material;

    material = BrMaterialAllocate("Tint Poly Mat");
    if (material == NULL) {
        return NULL;
    }
    material->colour = BR_COLOUR_RGB(0xc8, 0xc8, 0xc8);
    material->flags = BR_MATF_ALWAYS_VISIBLE;
    BrMaterialAdd(material);
    if (pOpacity != 0) {
        extra_prims[1].v.x = BR_FIXED_INT(pOpacity);
        material->extra_prim = extra_prims;
        BrMaterialUpdate(material, BR_MATU_EXTRA_PRIM);
    }
    return material;
}

void C2_HOOK_FASTCALL GetRangeOfValuesFromPixelmap(br_pixelmap* pMap, br_uint_32* pDarkest, br_uint_32* pBrightest) {
    int y;
    int x;
    br_uint_32 colour;
    br_uint_32 colour_component;

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

int C2_HOOK_FASTCALL CreateTintedPoly(int x0, int y0, int width, int height, int class, int arg1, int arg2, int arg3) {
    tPath_name the_path;
    int i;
    int x;
    int y;
    int tintedIndex;
    br_pixelmap* map;
    br_uint_32 darkest_colour;
    br_uint_32 brightest_colour;
    br_uint_32 colour;
    br_uint_32 colour_range;

    tintedIndex = -1;
    for (i = 0; i < REC2_ASIZE(C2V(gTintedPolys)); i++) {
        if (C2V(gTintedPolys)[i].actor == NULL) {
            tintedIndex = i;
            break;
        }
    }
    if (tintedIndex == -1) {
        return -1;
    }
    switch (class) {
        case 2:
            C2V(gTintedPolys)[tintedIndex].class = class;
            C2V(gTintedPolys)[tintedIndex].material = BuildTintedPolyMaterial(C2V(gDefaultOpacity_TintedPoly));
            C2V(gTintedPolys)[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            C2V(gTintedPolys)[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            C2V(gTintedPolys)[tintedIndex].actor->material = C2V(gTintedPolys)[tintedIndex].material;
            for (i = 0; i < C2V(gTintedPolys)[tintedIndex].model->nfaces; i++) {
                C2V(gTintedPolys)[tintedIndex].model->faces[i].material = C2V(gTintedPolys)[tintedIndex].material;
            }
            C2V(gTintedPolys)[tintedIndex].actor->model = C2V(gTintedPolys)[tintedIndex].model;
            BrModelAdd(C2V(gTintedPolys)[tintedIndex].model);
            break;
        case 3:
            C2V(gTintedPolys)[tintedIndex].class = class;
            C2V(gTintedPolys)[tintedIndex].material = BuildTintedPolyMaterial(C2V(gDefaultOpacity_TintedPoly));
            C2V(gTintedPolys)[tintedIndex].material->flags |= BR_MATF_PRELIT | BR_MATF_SMOOTH;
            BrMaterialUpdate(C2V(gTintedPolys)[tintedIndex].material, BR_MATU_ALL);
            C2V(gTintedPolys)[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 4, 4);
            C2V(gTintedPolys)[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            C2V(gTintedPolys)[tintedIndex].actor->material = C2V(gTintedPolys)[tintedIndex].material;
            for (i = 0; i < C2V(gTintedPolys)[tintedIndex].model->nfaces; i++) {
                C2V(gTintedPolys)[tintedIndex].model->faces[i].material = C2V(gTintedPolys)[tintedIndex].material;
            }
            C2V(gTintedPolys)[tintedIndex].actor->model = C2V(gTintedPolys)[tintedIndex].model;
            C2V(gTintedPolys)[tintedIndex].colour = arg1;
            BrModelAdd(C2V(gTintedPolys)[tintedIndex].model);
            break;
        case 4:
            C2V(gTintedPolys)[tintedIndex].class = class;
            C2V(gTintedPolys)[tintedIndex].material = BuildTintedPolyMaterial(0);
            C2V(gTintedPolys)[tintedIndex].material->flags |= BR_MATF_QUAD_MAPPING;
            PathCat(the_path, C2V(gApplication_path), "fire");
            LoadAllImagesInDirectory(&C2V(gMisc_storage_space), the_path);
            map = BrMapFind("testfire");
            C2V(gTintedPolys)[tintedIndex].pixelmap = map;
            C2V(gTintedPolys)[tintedIndex].material->colour_map = map;
            BrMaterialUpdate(C2V(gTintedPolys)[tintedIndex].material, BR_MATU_ALL);

            map = BrMapFind("firepall");
            for (i = 0; i < 256; i++) {
                C2V(gTintedPolys)[tintedIndex].colours[i] = BrPixelmapPixelGet(map, 0, i);
            }

            map = BrMapFind("coolmapx");
            GetRangeOfValuesFromPixelmap(map, &darkest_colour, &brightest_colour);

            // Map `coolmapx` colors to [0, arg1]
            colour_range = brightest_colour - darkest_colour;
            for (y = 0; y < 64; y++) {
                for (x = 0; x < 32; x++) {
                    br_uint_32 v;
                    colour = BrPixelmapPixelGet(map, x, y);
                    v = (br_uint_32)((float)(REC2_RGB555_B(colour) - darkest_colour) * arg1 / colour_range);
                    C2V(gTintedPolys)[tintedIndex].tints1[y + x * 64] = REC2_CLAMP(v, 0u, (br_uint_32)arg1);
                }
            }
            c2_memset(C2V(gTintedPolys)[tintedIndex].tints2, 0, sizeof(C2V(gTintedPolys)[tintedIndex].tints2));
            c2_memset(C2V(gTintedPolys)[tintedIndex].tints3, 0, sizeof(C2V(gTintedPolys)[tintedIndex].tints3));

            C2V(gTintedPolys)[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            C2V(gTintedPolys)[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            C2V(gTintedPolys)[tintedIndex].actor->material = C2V(gTintedPolys)[tintedIndex].material;
            for (i = 0; i < C2V(gTintedPolys)[tintedIndex].model->nfaces; i++) {
                C2V(gTintedPolys)[tintedIndex].model->faces[i].material = C2V(gTintedPolys)[tintedIndex].material;
            }
            C2V(gTintedPolys)[tintedIndex].actor->model = C2V(gTintedPolys)[tintedIndex].model;
            BrModelAdd(C2V(gTintedPolys)[tintedIndex].model);
            break;
        case 5:
            C2V(gTintedPolys)[tintedIndex].class = class;
            C2V(gTintedPolys)[tintedIndex].material = BuildTintedPolyMaterial(C2V(gDefaultOpacity_TintedPoly));
            C2V(gTintedPolys)[tintedIndex].material->flags |= BR_MATF_PRELIT | BR_MATF_SMOOTH;
            {
                static br_token_value extra_prims[2] = {
                        {
                                BRT_BLEND_B,
                                { 1 },
                        }, {
                                BRT_OPACITY_X,
                                { BR_FIXED_INT(80) },
                        },
                };
                C2V(gTintedPolys)[tintedIndex].material->extra_prim = extra_prims;
                extra_prims[1].v.x = BR_FIXED_INT(0);
            }
            BrMaterialUpdate(C2V(gTintedPolys)[tintedIndex].material, BR_MATU_ALL);
            C2V(gTintedPolys)[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            C2V(gTintedPolys)[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            C2V(gTintedPolys)[tintedIndex].actor->material = C2V(gTintedPolys)[tintedIndex].material;
            for (i = 0; i < C2V(gTintedPolys)[tintedIndex].model->nfaces; i++) {
                C2V(gTintedPolys)[tintedIndex].model->faces[i].material = C2V(gTintedPolys)[tintedIndex].material;
            }
            C2V(gTintedPolys)[tintedIndex].actor->model = C2V(gTintedPolys)[tintedIndex].model;
            C2V(gTintedPolys)[tintedIndex].unknown_1 = 0;
            C2V(gTintedPolys)[tintedIndex].unknown_2 = arg1;
            BrModelAdd(C2V(gTintedPolys)[tintedIndex].model);
            SetTintedPolyColour(tintedIndex, 0, 0, 0);
            break;
        case 6:
            C2V(gTintedPolys)[tintedIndex].class = class;
            C2V(gTintedPolys)[tintedIndex].material = BuildTintedPolyMaterial(C2V(gDefaultOpacity_TintedPoly));
            C2V(gTintedPolys)[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            C2V(gTintedPolys)[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            C2V(gTintedPolys)[tintedIndex].actor->material = C2V(gTintedPolys)[tintedIndex].material;
            for (i = 0; i < C2V(gTintedPolys)[tintedIndex].model->nfaces; i++) {
                C2V(gTintedPolys)[tintedIndex].model->faces[i].material = C2V(gTintedPolys)[tintedIndex].material;
            }
            C2V(gTintedPolys)[tintedIndex].actor->model = C2V(gTintedPolys)[tintedIndex].model;
            BrModelAdd(C2V(gTintedPolys)[tintedIndex].model);
            C2V(gTintedPolys)[tintedIndex].subClass = arg1;
            switch (arg1) {
                case 0:
                    C2V(gTintedPolys)[tintedIndex].color2_red = REC2_RGB888_R(arg2);
                    C2V(gTintedPolys)[tintedIndex].color2_grn = REC2_RGB888_G(arg2);
                    C2V(gTintedPolys)[tintedIndex].color2_blu = REC2_RGB888_B(arg2);
                    C2V(gTintedPolys)[tintedIndex].colour = arg3;
                    C2V(gTintedPolys)[tintedIndex].material->colour = BR_COLOUR_RGB(C2V(gTintedPolys)[tintedIndex].color2_red, C2V(gTintedPolys)[tintedIndex].color2_grn, C2V(gTintedPolys)[tintedIndex].color2_blu);
                    BrMaterialUpdate(C2V(gTintedPolys)[tintedIndex].material, BR_MATU_ALL);
                    break;
                case 1:
                    C2V(gTintedPolys)[tintedIndex].material->flags |= BR_MATF_PRELIT | BR_MATF_SMOOTH;
                    BrMaterialUpdate(C2V(gTintedPolys)[tintedIndex].material, BR_MATU_ALL);
                    C2V(gTintedPolys)[tintedIndex].colour = arg2;
                    break;
                default:
                    PDFatalError("Invalid Pulse Poly subclass");
            }
            break;
        default:
            C2V(gTintedPolys)[tintedIndex].class = class;
            C2V(gTintedPolys)[tintedIndex].material = BuildTintedPolyMaterial(C2V(gDefaultOpacity_TintedPoly));
            C2V(gTintedPolys)[tintedIndex].model = BuildTintedPolyModel(x0, y0, width, height, 1, 1);
            C2V(gTintedPolys)[tintedIndex].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            C2V(gTintedPolys)[tintedIndex].actor->material = C2V(gTintedPolys)[tintedIndex].material;
            for (i = 0; i < C2V(gTintedPolys)[tintedIndex].model->nfaces; i++) {
                C2V(gTintedPolys)[tintedIndex].model->faces[i].material = C2V(gTintedPolys)[tintedIndex].material;
            }
            C2V(gTintedPolys)[tintedIndex].actor->model = C2V(gTintedPolys)[tintedIndex].model;
            C2V(gTintedPolys)[tintedIndex].material->colour = arg1;
            {
                static br_token_value extra_prims[2] = {
                        {
                                BRT_BLEND_B,
                                { 1 },
                        }, {
                                BRT_OPACITY_X,
                                { BR_FIXED_INT(80) },
                        },
                };
                C2V(gTintedPolys)[tintedIndex].material->extra_prim = extra_prims;
                extra_prims[1].v.x = BR_FIXED_INT(arg2);
            }
            BrMaterialUpdate(C2V(gTintedPolys)[tintedIndex].material, BR_MATU_ALL);
            break;
    }
    if (C2V(gTintedPolys)[tintedIndex].actor != NULL) {
        BrActorAdd(C2V(gTintedPolyCamera), C2V(gTintedPolys)[tintedIndex].actor);
    }
    C2V(gTintedPolys)[tintedIndex].used = 1;
    if (C2V(gTintedPolys)[tintedIndex].actor != NULL) {
        (C2V(gTintedPolys)[tintedIndex].actor)->render_style = BR_RSTYLE_NONE;
    }
    C2V(gTintedPolys)[tintedIndex].color_red = 0;
    C2V(gTintedPolys)[tintedIndex].color_grn = 0;
    C2V(gTintedPolys)[tintedIndex].color_blu = 0;
    C2V(gTintedPolys)[tintedIndex].visible = 0;
    return tintedIndex;
}
C2_HOOK_FUNCTION(0x004d70c0, CreateTintedPoly)

br_model* C2_HOOK_FASTCALL BuildTintedPolyModel(int x0, int y0, int width, int height, int nbX, int nbY) {
    br_model* model;
    int dx;
    int dy;
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
    dx = width / nbX;
    for (j = 0; j < nbY + 1; j++) {
        dy = j * (height / nbY);
        for (i = 0; i < nbX + 1; i++) {
            dx = i * (width / nbX);
            vertex = &model->vertices[j * (nbX + 1) + i];
            vertex->p.v[0] = (br_scalar)(x0 + dx);
            vertex->p.v[1] = (br_scalar)(-(y0 + dy));
            vertex->p.v[2] = -1.02f;
            vertex->map.v[0] = (br_scalar)dx / width;
            vertex->map.v[1] = (br_scalar)dy / height;
        }
    }
    for (j = 0; j < nbY; j++) {
        vert_above = j * (nbX + 1);
        vert_below = (j + 1) * (nbX + 1);
        for (i = 0; i < nbX; i++) {
            face = &model->faces[2 * (j * nbX + i)];
            face[0].vertices[0] = vert_above + i ;
            face[0].vertices[1] = vert_above + i + 1;
            face[0].vertices[2] = vert_below + i ;
            face[1].vertices[0] = vert_above + i + 1;
            face[1].vertices[1] = vert_below + i + 1;
            face[1].vertices[2] = vert_below + i;
        }
    }
    model->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL;
    return model;
}
C2_HOOK_FUNCTION(0x004d7ad0, BuildTintedPolyModel)

void C2_HOOK_FASTCALL FreeTintedPolyActor(int pTintedIndex) {
    if (!C2V(gTintedPolys)[pTintedIndex].used) {
        return;
    }
    if (C2V(gTintedPolys)[pTintedIndex].material != NULL) {
        BrMaterialFree(C2V(gTintedPolys)[pTintedIndex].material);
    }
    if (C2V(gTintedPolys)[pTintedIndex].model != NULL) {
        BrModelFree(C2V(gTintedPolys)[pTintedIndex].model);
    }
    if (C2V(gTintedPolys)[pTintedIndex].actor != NULL) {
        BrActorRemove(C2V(gTintedPolys)[pTintedIndex].actor);
        BrActorFree(C2V(gTintedPolys)[pTintedIndex].actor);
        C2V(gTintedPolys)[pTintedIndex].actor = NULL;
    }
    C2V(gTintedPolys)[pTintedIndex].class = 0;
    C2V(gTintedPolys)[pTintedIndex].material2 =NULL;
    C2V(gTintedPolys)[pTintedIndex].visible = 0;
    C2V(gTintedPolys)[pTintedIndex].used = 0;
}
C2_HOOK_FUNCTION(0x004d7c80, FreeTintedPolyActor)

void C2_HOOK_FASTCALL FreeAllTintedPolyActor(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gTintedPolys)); i++) {
        FreeTintedPolyActor(i);
    }
}
C2_HOOK_FUNCTION(0x004d7d10, FreeAllTintedPolyActor)

void C2_HOOK_FASTCALL UpdateTintedPolyActor(int pTintedIndex) {
    br_uint_32 new_grn, new_blu, new_red;
    br_pixelmap* map;
    tU32 time;
    int material_changed;
    tU32 time_since_last_update_s;

    if (!C2V(gTintedPolys)[pTintedIndex].visible || C2V(gTintedPolys)[pTintedIndex].material2 == NULL) {
        return;
    }

    map = C2V(gTintedPolys)[pTintedIndex].material->colour_map;
    new_grn = C2V(gTintedColourMap_grn);
    new_blu = C2V(gTintedColourMap_blu);
    new_red = C2V(gTintedColourMap_red);
    if (map != NULL && map->pixels != NULL) {
        br_uint_32 v_tl, v_tr, v_bl, v_br;

        v_tl = BrPixelmapPixelGet(map, map->width / 4, map->height / 4);
        v_tr = BrPixelmapPixelGet(map, map->width - map->width / 4, map->height / 4);
        v_bl = BrPixelmapPixelGet(map, map->width / 4, map->height / 4);
        v_br = BrPixelmapPixelGet(map, map->width - map->width / 4, map->height - map->height / 4);

        new_red = (REC2_RGB888_R(v_tl) + REC2_RGB888_R(v_tr) + REC2_RGB888_R(v_bl) + REC2_RGB888_R(v_br)) / 4;
        new_grn = (REC2_RGB888_G(v_tl) + REC2_RGB888_G(v_tr) + REC2_RGB888_G(v_bl) + REC2_RGB888_G(v_br)) / 4;
        new_grn = (REC2_RGB888_B(v_tl) + REC2_RGB888_B(v_tr) + REC2_RGB888_B(v_bl) + REC2_RGB888_B(v_br)) / 4;
    }
    time = GetTotalTime();
    if (C2V(gTintedPolys)[pTintedIndex].color_red + C2V(gTintedPolys)[pTintedIndex].color_grn + C2V(gTintedPolys)[pTintedIndex].color_blu == 0) {
        C2V(gTintedPolys)[pTintedIndex].color_red = new_red;
        C2V(gTintedPolys)[pTintedIndex].color_grn = new_grn;
        C2V(gTintedPolys)[pTintedIndex].color_blu = new_blu;
        C2V(gTintedPolys)[pTintedIndex].lastTime = time;
        C2V(gTintedPolys)[pTintedIndex].material->colour = BR_COLOUR_RGB(new_red, new_grn, new_blu);
        BrMaterialUpdate(C2V(gTintedPolys)[pTintedIndex].material, BR_MATU_LIGHTING);
        return;
    }

    material_changed = 0;
    time_since_last_update_s = time - C2V(gTintedPolys)[pTintedIndex].lastTime;
    if (time <= 0) {
        time_since_last_update_s = 0;
    }
    if (time_since_last_update_s != 0) {
        if (C2V(gTintedPolys)[pTintedIndex].color_red != new_red) {
            if (C2V(gTintedPolys)[pTintedIndex].color_red < new_red) {
                C2V(gTintedPolys)[pTintedIndex].color_red += time_since_last_update_s;
            } else {
                C2V(gTintedPolys)[pTintedIndex].color_red -= time_since_last_update_s;
            }
            if (new_red - time_since_last_update_s < C2V(gTintedPolys)[pTintedIndex].color_red && C2V(gTintedPolys)[pTintedIndex].color_red < time_since_last_update_s + new_red) {
                C2V(gTintedPolys)[pTintedIndex].color_red = new_red;
            }
            material_changed = 1;
        }
        if (C2V(gTintedPolys)[pTintedIndex].color_grn != new_grn) {
            if (C2V(gTintedPolys)[pTintedIndex].color_grn < new_grn) {
                C2V(gTintedPolys)[pTintedIndex].color_grn += time_since_last_update_s;
            } else {
                C2V(gTintedPolys)[pTintedIndex].color_grn -= time_since_last_update_s;
            }
            if (new_grn - time_since_last_update_s < C2V(gTintedPolys)[pTintedIndex].color_grn && C2V(gTintedPolys)[pTintedIndex].color_grn < time_since_last_update_s + new_grn) {
                C2V(gTintedPolys)[pTintedIndex].color_grn = new_grn;
            }
            material_changed = 1;
        }
        if (C2V(gTintedPolys)[pTintedIndex].color_blu != new_blu) {
            if (C2V(gTintedPolys)[pTintedIndex].color_blu < new_blu) {
                C2V(gTintedPolys)[pTintedIndex].color_blu += time_since_last_update_s;
            } else {
                C2V(gTintedPolys)[pTintedIndex].color_blu -= time_since_last_update_s;
            }
            if (new_blu - time_since_last_update_s < C2V(gTintedPolys)[pTintedIndex].color_blu && C2V(gTintedPolys)[pTintedIndex].color_blu < time_since_last_update_s + new_blu) {
                C2V(gTintedPolys)[pTintedIndex].color_blu = new_blu;
            }
            material_changed = 1;
        }
        if (material_changed) {
            C2V(gTintedPolys)[pTintedIndex].material->colour = BR_COLOUR_RGB(C2V(gTintedPolys)[pTintedIndex].color_red, C2V(gTintedPolys)[pTintedIndex].color_grn, C2V(gTintedPolys)[pTintedIndex].color_blu);
            BrMaterialUpdate(C2V(gTintedPolys)[pTintedIndex].material, BR_MATU_LIGHTING);
        }
    }
}
C2_HOOK_FUNCTION(0x004d7d80, UpdateTintedPolyActor)

void C2_HOOK_FASTCALL TurnTintedPolyOn(int pTintedIndex) {
    if (C2V(gTintedPolys)[pTintedIndex].used) {
        C2V(gTintedPolys)[pTintedIndex].actor->render_style = BR_RSTYLE_FACES;
        C2V(gTintedPolys)[pTintedIndex].visible = 1;
    }
}
C2_HOOK_FUNCTION(0x004d8220, TurnTintedPolyOn)

void C2_HOOK_FASTCALL TurnTintedPolyOff(int pTintedIndex) {
    if (C2V(gTintedPolys)[pTintedIndex].used) {
        (C2V(gTintedPolys)[pTintedIndex].actor)->render_style = BR_RSTYLE_NONE;
        C2V(gTintedPolys)[pTintedIndex].color_red = 0;
        C2V(gTintedPolys)[pTintedIndex].color_grn = 0;
        C2V(gTintedPolys)[pTintedIndex].color_blu = 0;
        C2V(gTintedPolys)[pTintedIndex].visible = 0;
    }
}
C2_HOOK_FUNCTION(0x004d8250, TurnTintedPolyOff)

void C2_HOOK_FASTCALL RenderTintedPolys(void) {
    BrZbsSceneRender(C2V(gTintedPolyCamera), C2V(gTintedPolyCamera), C2V(gBack_screen), C2V(gDepth_buffer));
}
C2_HOOK_FUNCTION(0x004d8290, RenderTintedPolys)

void C2_HOOK_FASTCALL SetTintedPolyColour(int pTintedIndex, int pRed, int pGreen, int pBlue) {
    int red_differs;
    int grn_differs;
    int blu_differs;

    red_differs = C2V(gTintedPolys)[pTintedIndex].color_red != pRed;
    if (red_differs) {
        C2V(gTintedPolys)[pTintedIndex].color_red = pRed;
    }
    grn_differs = C2V(gTintedPolys)[pTintedIndex].color_grn != pGreen;
    if (grn_differs) {
        C2V(gTintedPolys)[pTintedIndex].color_grn = pGreen;
    }
    blu_differs = C2V(gTintedPolys)[pTintedIndex].color_blu != pBlue;
    if (blu_differs) {
        C2V(gTintedPolys)[pTintedIndex].color_blu = pBlue;
    }
    if (red_differs || (grn_differs || blu_differs)) {
        (C2V(gTintedPolys)[pTintedIndex].material)->colour = BR_COLOUR_RGB(C2V(gTintedPolys)[pTintedIndex].color_red, C2V(gTintedPolys)[pTintedIndex].color_grn, C2V(gTintedPolys)[pTintedIndex].color_blu);
        BrMaterialUpdate(C2V(gTintedPolys)[pTintedIndex].material ,BR_MATU_LIGHTING);
    }
}
C2_HOOK_FUNCTION(0x004d82b0, SetTintedPolyColour)

void C2_HOOK_FASTCALL SetTintedPolySize(int pTintedIndex, int x0, int y0, int width, int height) {
    tTintedPoly* tinted;
    int nb_x;
    int nb_y;
    int i;
    int j;
    br_vertex* vertex;

    if (pTintedIndex == -1) {
        return;
    }
    tinted = &C2V(gTintedPolys)[pTintedIndex];
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
    vertex = tinted->model->vertices;
    for (i = 0; i <= nb_y; i++) {
        br_scalar dy = i * (br_scalar)height / (br_scalar)nb_y;
        for (j = 0; j <= nb_x; j++) {
            float dx = j * (br_scalar)width / (br_scalar)nb_x;
            BrVector3Set(&vertex->p, (br_scalar)x0 + dx, (br_scalar)-y0 - dy, -1.02f);
            BrVector2Set(&vertex->map, dx / width, dy / height);
            vertex++;
        }
    }
    BrModelUpdate(tinted->model, BR_MODU_VERTICES);
}
C2_HOOK_FUNCTION(0x004d8bb0, SetTintedPolySize)

void (C2_HOOK_FASTCALL * SetTintedFromSpecialVolume_original)(int pIndex, br_vector3* pPosition);
void C2_HOOK_FASTCALL SetTintedFromSpecialVolume(int pIndex, br_vector3* pPosition) {

#if 0//defined(C2_HOOKS_ENABLED)
    SetTintedFromSpecialVolume_original(pIndex, pPosition);
#else
    tSpecial_volume* volume;

    volume = C2V(gCar_to_view)->collision_info->last_special_volume;
    if ((volume != NULL && volume->gravity_multiplier < 1.f) || C2V(gAction_replay_mode)) {
        br_vector3 dir, nor;
        br_scalar t;
        br_material *mat;

        BrVector3Set(&dir, 0.f, 200.f, 0.f);
        DisablePlingMaterials();
        FindFace(pPosition, &dir, &nor, &t, &mat);
        EnablePlingMaterials();
        if (t < 100.f && mat != NULL && mat->identifier != NULL) {
            if (c2_strlen(mat->identifier) >= 1) {
                if (mat->identifier[0] == '!' || mat->identifier[0] == '#') {
                    C2V(gTintedPolys)[pIndex].field_0x28 = 35;
                    C2V(gTintedPolys)[pIndex].material2 = mat;
                    return;
                } else if (mat->identifier[0] == '@') {
                    C2V(gTintedPolys)[pIndex].field_0x28 = 64;
                    C2V(gTintedPolys)[pIndex].material2 = mat;
                    return;
                }
            }
        }
    }
    C2V(gTintedPolys)[pIndex].field_0x28 = 0;
    C2V(gTintedPolys)[pIndex].material2 = NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d8350, SetTintedFromSpecialVolume, SetTintedFromSpecialVolume_original)

void (C2_HOOK_FASTCALL * ProcessTintedPoly_original)(int pIndex);
void C2_HOOK_FASTCALL ProcessTintedPoly(int pIndex) {

#if 0//defined(C2_HOOKS_ENABLED)
    ProcessTintedPoly_original(pIndex);
#else

    if (!C2V(gTintedPolys)[pIndex].used) {
        return;
    }
    switch (C2V(gTintedPolys)[pIndex].class) {
    case 2: {
        tSpecial_volume *volume = C2V(gCar_to_view)->collision_info->last_special_volume;
        if ((volume != NULL && volume->gravity_multiplier < 1.0f) || C2V(gAction_replay_mode)) {
            UpdateTintedPolyActor(pIndex);
        }
        break;
    }
    case 3:
        TintedAnimateSawToothColor(pIndex);
        return;
    case 4:
        if (PDKeyDown(32) || (C2V(gINT_006a0444) > 0 && C2V(gINT_006a0444) <= 200)) {
            FUN_004d86e0(pIndex);
        }
        if (PDKeyDown(35) || (C2V(gINT_006a0444) > 0 && C2V(gINT_006a0444) <= 200)) {
            C2V(gINT_006a0448) = 1;
            FUN_004d86e0(pIndex);
        }
        break;
    case 6:
        if (C2V(gTintedPolys)[pIndex].visible) {
            int red, grn, blu;

            if (C2V(gTintedPolys)[pIndex].subClass == 0) {
                unsigned int mask;
                mask = GetTotalTime() >> C2V(gTintedPolys)[pIndex].colour;
                if ((mask & 0x1ff) > 254) {
                    mask = -2 - (mask & 0x1ff);
                }
                mask &= 0xff;
                red = C2V(gTintedPolys)[pIndex].color2_red & mask;
                grn = C2V(gTintedPolys)[pIndex].color2_grn & mask;
                blu = C2V(gTintedPolys)[pIndex].color2_blu & mask;
            } else {
                red = grn = blu = pIndex;
                if (C2V(gTintedPolys)[pIndex].subClass == 1) {
                    TintedAnimateSawToothColor(pIndex);
                }
            }
            C2V(gTintedPolys)[pIndex].material->colour = BR_COLOUR_RGB(red, grn, blu);
            BrMaterialUpdate(C2V(gTintedPolys)[pIndex].material, BR_MATU_LIGHTING);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d84a0, ProcessTintedPoly, ProcessTintedPoly_original)

void C2_HOOK_FASTCALL TintedAnimateSawToothColor(int pIndex) {

    if ((C2V(gTintedPolys)[pIndex].actor)->render_style == BR_RSTYLE_FACES) {
        tU32 time;
        int i;
        tU8 new_color;

        time = GetTotalTime();

        new_color = (tU8)(time >> C2V(gTintedPolys)[pIndex].colour);
        for (i = 0; i < C2V(gTintedPolys)[pIndex].model->nvertices; i++) {
            C2V(gTintedPolys)[pIndex].model->vertices[i].red = new_color + 128;
            C2V(gTintedPolys)[pIndex].model->vertices[i].grn = new_color + 30 * i;
            C2V(gTintedPolys)[pIndex].model->vertices[i].blu = new_color;
        }
        BrModelUpdate(C2V(gTintedPolys)[pIndex].model, BR_MODU_VERTICES);
    }
}
C2_HOOK_FUNCTION(0x004d8630, TintedAnimateSawToothColor)

void C2_HOOK_FASTCALL FUN_004d86e0(int pIndex) {
    int i;
    int j;

    if (!C2V(gINT_006a0444)) {
        C2_HOOK_BUG_ON(sizeof(C2V(gTintedPolys)[pIndex].tints2) != 8192);
        C2_HOOK_BUG_ON(sizeof(C2V(gTintedPolys)[pIndex].tints3) != 8192);

        c2_memset(C2V(gTintedPolys)[pIndex].tints2, 0, sizeof(C2V(gTintedPolys)[pIndex].tints2));
        c2_memset(C2V(gTintedPolys)[pIndex].tints3, 0, sizeof(C2V(gTintedPolys)[pIndex].tints2));
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            C2V(gTintedPolys)[pIndex].tints2[64 * (IRandomBetween(i - 5, 5 - i) + IRandomBetween(13, 19)) + i + 64 - 5] = 255 - C2V(gINT_006a044c);
        }
    }
    if (C2V(gINT_006a0444) > 150) {
        C2V(gINT_006a044c) += 1;
    }
    C2V(gINT_006a0440) += 1;
    if (C2V(gINT_006a0440) >= 64) {
        C2V(gINT_006a0440) = 0;
    }

    for (i = 0; i < 62; i++) {
        for (j = 0; j < 30; j++) {
            int avg =
                ((C2V(gTintedPolys)[pIndex].tints2[i + (j + 2) * 64 + 1]
                + C2V(gTintedPolys)[pIndex].tints2[i + (j + 0) * 64 + 1]
                + C2V(gTintedPolys)[pIndex].tints2[i + (j + 1) * 64 + 2]
                + C2V(gTintedPolys)[pIndex].tints2[i + (j + 1) * 64 + 0]) / 4
                + C2V(gTintedPolys)[pIndex].tints2[i + (j + 1) * 64 + 1]) / 2
            - C2V(gTintedPolys)[pIndex].tints1[i + (j + 1) * 64 + 1]
            - C2V(gINT_006a044c);
            C2V(gTintedPolys)[pIndex].tints3[i + (j + 1) * 64] = MAX(0, avg);
        }
    }

    if (C2V(gINT_006a0444) < 150 && IRandomBetween(0, 6) > 3) {
        int idx;

        idx = IRandomBetween(12, 20) * 64 + IRandomBetween(30,55);
        C2V(gTintedPolys)[pIndex].tints2[idx] = 0xff;
        C2V(gTintedPolys)[pIndex].tints3[idx] = 0xbf;
    }
    c2_memcpy(C2V(gTintedPolys)[pIndex].tints2, C2V(gTintedPolys)[pIndex].tints3, sizeof(C2V(gTintedPolys)[pIndex].tints2));

    for (i = 0; i < 64; i++) {
        for (j = 0; j < 32; j++) {
            int idx = C2V(gTintedPolys)[pIndex].tints3[j * 64 + i];
            BrPixelmapPixelSet(C2V(gTintedPolys)[pIndex].pixelmap, i, j, C2V(gTintedPolys)[pIndex].colours[idx]);
        }
    }
    BrMapUpdate(C2V(gTintedPolys)[pIndex].pixelmap, BR_MAPU_ALL);

    if (C2V(gINT_006a0444) % 10 == 0 && C2V(gINT_006a0448) && C2V(gINT_006a0444) > 10) {
        tPath_name save_path;
        for (i = 1; i < 10000; i++) {
            tPath_name path;
            FILE* f;

            PathCat(path, C2V(gApplication_path), "FLM");
            c2_sprintf(&path[c2_strlen(path)], "%02d.%s", i, "PIX");
            f = DRfopen(path, "rt");
            if (f == NULL) {
                c2_sprintf(save_path, "%s", path);
                break;
            }
        }
        BrPixelmapSave(save_path, C2V(gTintedPolys)[pIndex].pixelmap);
    }
    C2V(gINT_006a0444) += 1;
    if (C2V(gINT_006a0444) >= 200) {
        C2V(gINT_006a0444) = 0;
        C2V(gINT_006a0448) = 0;
        C2V(gINT_006a044c) = 0;
    }
}
C2_HOOK_FUNCTION(0x004d86e0, FUN_004d86e0)

int C2_HOOK_FASTCALL TintedPolyIsOn(int pIndex) {

    return C2V(gTintedPolys)[pIndex].visible;
}
C2_HOOK_FUNCTION(0x004d8cf0, TintedPolyIsOn)
