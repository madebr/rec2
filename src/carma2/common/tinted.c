#include "tinted.h"

#include "globvars.h"

#include "rec2_types.h"
#include "brender/brender.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTintedPoly, gTintedPolys, 10, 0x00705c80);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gTintedPolyCamera, 0x006a0430);

void C2_HOOK_FASTCALL InitTintedPolys(void) {
    br_camera *camera;

    C2_HOOK_BUG_ON(sizeof(tTintedPoly) != 0x6450);

    memset(C2V(gTintedPolys), 0, sizeof(C2V(gTintedPolys)));

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
C2_HOOK_FUNCTION(0x004d7040, InitTintedPolys);

void C2_HOOK_FASTCALL SetTintedColour(int pTintedIndex, int pRed, int pGreen, int pBlue) {
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
C2_HOOK_FUNCTION(0x004d82b0, SetTintedColour)

br_model* C2_HOOK_FASTCALL CreateInterpolatedQuadModel(int x0, int y0, int width, int height, int nbX, int nbY) {
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
C2_HOOK_FUNCTION(0x004d7ad0, CreateInterpolatedQuadModel)

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
