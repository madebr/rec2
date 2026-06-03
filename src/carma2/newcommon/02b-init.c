#include "02-init.h"

#include "09-funks.h"
#include "41-utility.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrkm.h"

#include "rec2_macros.h"

#include <stdio.h>

// GLOBAL: CARMA2_HW 0x00762284
int gDemo;

// GLOBAL: CARMA2_HW 0x0074d354
float gCamera_angle;

// GLOBAL: CARMA2_HW 0x00703e24
int gWidth;

// GLOBAL: CARMA2_HW 0x00703e20
int gHeight;

// FUNCTION: CARMA2_HW 0x004815d0
void C2_HOOK_FASTCALL CheckTimedDemo(void) {
    FILE* f;
    tPath_name path;

    PathCat(path, gApplication_path, "DEMO.TXT");
    f = PFfopen(path, "rt");
    gDemo = f != NULL;
    if (gDemo) {
        PFfclose(f);
    }
}

// FUNCTION: CARMA2_HW 0x0047e560
void C2_HOOK_FASTCALL Init2DStuffForPolyFonts(void) {
    br_camera* camera_data;

    gHUD_root = BrActorAllocate(BR_ACTOR_NONE, NULL);
    gHUD_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    if (gHUD_root == NULL || gHUD_camera == NULL) {
        FatalError(kFatalError_OOM_S);
    }
    gHUD_camera->identifier = "HUD Camera";
    camera_data = gHUD_camera->type_data;
    camera_data->type = BR_CAMERA_PARALLEL;
    camera_data->hither_z = 1.0f;
    camera_data->yon_z = 3.0f;
    camera_data->width = gBack_screen->width;
    camera_data->height = gBack_screen->height;
    BrActorAdd(gHUD_root, gHUD_camera);
}

// FUNCTION: CARMA2_HW 0x0047e500
void C2_HOOK_FASTCALL AllocateStandardLamp(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        br_actor *light = gLight_array[i];
        BrActorAdd(gUniverse_actor, light);
        if (light->children != NULL) {
            br_actor *child = light->children;
            BrActorRemove(child);
            light->children = NULL;
            BrActorFree(child);
        }
        BrLightEnable(light);
    }
}

// FUNCTION: CARMA2_HW 0x0047e320
void C2_HOOK_FASTCALL AllocateActors(void) {
    int i;

    gSelf = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (gSelf == NULL) {
        FatalError(kFatalError_CannotAllocateSelf);
    }
    gSelf = BrActorAdd(gUniverse_actor, gSelf);
    if (gSelf == NULL) {
        FatalError(kFatalError_CannotAllocateSelf);
    }

    for (i = 0; i < (int)REC2_ASIZE(gOther_selfs); i++) {
        gOther_selfs[i] = BrActorAllocate(BR_ACTOR_NONE, NULL);
        if (gOther_selfs[i] == NULL) {
            FatalError(kFatalError_CannotAllocateSelf);
        } else {
            BrActorAdd(gSelf, gOther_selfs[i]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047e3b0
void C2_HOOK_FASTCALL AllocateCamera(void) {
    br_camera* camera_ptr;
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); i++) {
        gCamera_list[i] = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
        if (gCamera_list[i] == NULL) {
            FatalError(kFatalError_CannotAllocateCamera);
        }

        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->type = BR_CAMERA_PERSPECTIVE_FOV;
        camera_ptr->field_of_view = BrDegreeToAngle(gCamera_angle);
        camera_ptr->hither_z = gCamera_hither;
        camera_ptr->yon_z = gCamera_yon;
        camera_ptr->aspect = (float)gWidth / (float)gHeight;
    }

    if (gCamera_list[0] == NULL) {
        FatalError(kFatalError_CannotAllocateCamera);
    }
    gCamera_list[1] = BrActorAdd(gUniverse_actor, gCamera_list[1]);
    if (gCamera_list[1] == NULL) {
        FatalError(kFatalError_CannotAllocateCamera);
    }
    gCamera = gCamera_list[1];
    gRearview_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    if (gRearview_camera == NULL) {
        FatalError(kFatalError_CannotAllocateCamera);
    }

    gRearview_camera->t.t.mat.m[2][2] = -1.0f;
    camera_ptr = (br_camera*)gRearview_camera->type_data;
    camera_ptr->type = BR_CAMERA_PERSPECTIVE_FOV;
    camera_ptr->field_of_view = BrDegreeToAngle(gCamera_angle);
    camera_ptr->hither_z = gCamera_hither;
    camera_ptr->yon_z = (br_scalar)((double)gCamera_yon / 2.0);
    camera_ptr->aspect = (float)gWidth / (float)gHeight;
    if (gRearview_camera == NULL) {
        FatalError(kFatalError_CannotAllocateCamera);
    }
    SetSightDistance(camera_ptr->yon_z);
}
