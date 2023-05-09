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
