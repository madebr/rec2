#include "oil.h"

#include "globvars.h"
#include "graphics.h"
#include "loading.h"

#include <brender/brender.h>

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gOil_pixie_names, 1, 0x0065a398, {
    "OIL.PIX",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gOil_pixies, 1, 0x00690c88);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tOil_spill_info, gOily_spills, 32, 0x00690c90);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, oily_material, 0x0074a68c);

void (C2_HOOK_FASTCALL * InitOilSpills_original)(void);
void C2_HOOK_FASTCALL InitOilSpills(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitOilSpills_original();
#else
    int i;
    br_model* the_model;

    C2_HOOK_BUG_ON(sizeof(tOil_spill_info) != 0x54);

    for (i = 0; i < REC2_ASIZE(C2V(gOil_pixie_names)); i++) {
        C2V(gOil_pixies)[i] = LoadPixelmap(C2V(gOil_pixie_names)[i]);
        BrMapAdd(C2V(gOil_pixies)[i]);
    }

    for (i = 0; i < REC2_ASIZE(C2V(gOily_spills)); i++) {
        C2V(oily_material) = BrMaterialAllocate(NULL);
        BrMaterialAdd(C2V(oily_material));
        C2V(oily_material)->flags |= BR_MATF_LIGHT;
        C2V(oily_material)->flags |= BR_MATF_PERSPECTIVE;
        C2V(oily_material)->flags |= BR_MATF_SMOOTH;
        C2V(oily_material)->ka = 0.99f;
        C2V(oily_material)->kd = 0.0f;
        C2V(oily_material)->ks = 0.0f;
        C2V(oily_material)->power = 0.0f;
        C2V(oily_material)->index_base = 0;
        C2V(oily_material)->index_range = 0;
        C2V(oily_material)->colour_map = C2V(gOil_pixies)[0];
        BrMatrix23Identity(&C2V(oily_material)->map_transform);
        C2V(oily_material)->index_shade = BrTableFind("IDENTITY.TAB");
        GlorifyMaterial(&C2V(oily_material), 1, kRendererShadingType_Specular);
        BrMaterialUpdate(C2V(oily_material), BR_MATU_ALL);
        the_model = BrModelAllocate(NULL, 4, 2);
        the_model->flags |= BR_MODF_KEEP_ORIGINAL;

        the_model->faces[0].vertices[0] = 2;
        the_model->faces[0].vertices[1] = 1;
        the_model->faces[0].vertices[2] = 0;
        the_model->faces[0].material = NULL;
        the_model->faces[0].smoothing = 1;

        the_model->faces[1].vertices[0] = 3;
        the_model->faces[1].vertices[1] = 2;
        the_model->faces[1].vertices[2] = 0;
        the_model->faces[1].material = NULL;
        the_model->faces[1].smoothing = 1;

        BrVector3Set(&the_model->vertices[0].p, -1.f, 0.f, -1.f);
        BrVector2Set(&the_model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&the_model->vertices[1].p, 1.f, 0.f, 1.f);
        BrVector2Set(&the_model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&the_model->vertices[2].p, 1.f, 0.f, -1.f);
        BrVector2Set(&the_model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&the_model->vertices[3].p, -1.f, 0.f, 1.f);
        BrVector2Set(&the_model->vertices[3].map, 1.f, 1.f);
        C2V(gOily_spills)[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        C2V(gOily_spills)[i].actor->model = the_model;
        C2V(gOily_spills)[i].actor->render_style = BR_RSTYLE_NONE;
        C2V(gOily_spills)[i].actor->material = C2V(oily_material);
        BrActorAdd(C2V(gOther_selfs)[2], C2V(gOily_spills)[i].actor);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a6a10, InitOilSpills, InitOilSpills_original)

void C2_HOOK_FASTCALL ResetOilSpills(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gOily_spills)); i++) {
        C2V(gOily_spills)[i].actor->render_style = BR_RSTYLE_NONE;
        C2V(gOily_spills)[i].car = NULL;
        C2V(gOily_spills)[i].car_actor = NULL;
        C2V(gOily_spills)[i].field_0x14 = 0;
    }
}
C2_HOOK_FUNCTION(0x004a6c50, ResetOilSpills)
