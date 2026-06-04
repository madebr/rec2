#include "replay.h"

#include "car.h"
#include "controls.h"
#include "displays.h"
#include "drone.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "input.h"
#include "loading.h"
#include "main.h"
#include "pedestrn.h"
#include "piping.h"
#include "platform.h"
#include "replay_callbacks.h"
#include "utility.h"

#include "c2_string.h"

#include "rec2_macros.h"


// GLOBAL: CARMA2_HW 0x0079efa8
tActionReplayCameraMode gAction_replay_camera_mode;

// GLOBAL: CARMA2_HW 0x00679278
int gAction_replay_manual_camera_target_type;

// GLOBAL: CARMA2_HW 0x0058f600
tU8 gCamera_type_allowed_replay[9] = {
    1, 1, 1, 1, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x0058f610
tU8 gCamera_type_allowed_gameplay[9] = {
    1, 0, 0, 0, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x006a23c4
int gSingle_frame_mode;

// GLOBAL: CARMA2_HW 0x006a23d4
tU32 gLast_synch_time;

// GLOBAL: CARMA2_HW 0x006a2374
int gKey_down;

// FUNCTION: CARMA2_HW 0x004e1740
void C2_HOOK_FASTCALL SetQuickTimeDefaults(void) {

    if (gQuick_time_quality[0] == '\0') {
        strcpy(gQuick_time_quality, "normal");
    }
    if (gQuick_time_compressor[0] == '\0') {
        strcpy(gQuick_time_compressor, "animation");
    }
    if (gQuick_time_temp_path[0] == '\0') {
        PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
    }
    if (gQuick_time_movie_path_stub[0] == '\0') {
        PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
    }
    if (gQuick_time_banner_texture_name[0] == '\0') {
        strcpy(gQuick_time_banner_texture_name, "ARBANNER.PIX");
    }
}

// FUNCTION: CARMA2_HW 0x0040e700
void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam) {
    int i;

    for (i = 0; i < pCar->count_window_materials; i++) {
        tCarCockpitMaterial* cockpit_material;

        int two_sided_material = pInternal_cam;
        cockpit_material = &pCar->window_materials[i];
        if (pInternal_cam) {
            int j;

            for (j = 0; j < cockpit_material->count_maps; j++) {
                if (cockpit_material->material->colour_map == cockpit_material->maps[j]) {
                    two_sided_material = 0;
                }
            }
        }
        if (two_sided_material) {
            cockpit_material->material->flags |= BR_MATF_TWO_SIDED;
        } else {
            cockpit_material->material->flags &= ~BR_MATF_TWO_SIDED;
        }
        BrMaterialUpdate(cockpit_material->material, BR_MATU_RENDERING);
    }
}

// FUNCTION: CARMA2_HW 0x0040e790
void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos) {

    gAction_replay_camera_mode = pCamPos;
    switch (pCamPos) {
    case kActionReplayCameraMode_Standard:
    case kActionReplayCameraMode_Rigid:
    case kActionReplayCameraMode_Reversing:
        InitialiseExternalCamera();
        break;
    case kActionReplayCameraMode_Manual:
        gAction_replay_manual_camera_target_type = 0;
        break;
    default:
        break;
    }
    MungeCarMaterials(&gProgram_state.current_car, gAction_replay_camera_mode == kActionReplayCameraMode_Internal);
}

int C2_HOOK_FASTCALL IsCameraTypeAllowed(tActionReplayCameraMode pCamera_type) {

    if (gAction_replay_mode && !gCamera_type_allowed_replay[pCamera_type]) {
        return 0;
    }
    if (!gAction_replay_mode && !gCamera_type_allowed_gameplay[pCamera_type]) {
        return 0;
    }
    if (pCamera_type > 8) {
        return 0;
    }
    if (pCamera_type == kActionReplayCameraMode_Drone && !OKToViewDrones()) {
        return 0;
    }
    if (pCamera_type == kActionReplayCameraMode_Peds && gPed_count == 0) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0040ea30
void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime) {
    br_camera* camera;
    // GLOBAL: CARMA2_HW 0x0058f62c
    static tActionReplayCameraMode old_camera_mode = kActionReplayCameraMode_Invalid;

    camera = gCamera->type_data;
    CheckCameraHither();
    if (!IsCameraTypeAllowed(gAction_replay_camera_mode)) {
        ChangeCameraType();
    }
    AmIGettingBoredWatchingCameraSpin();
    if (!gProgram_state.cockpit_on) {
        tCar_spec* c;

        if (gOpponent_viewing_mode && gAction_replay_mode) {
            c = &gProgram_state.current_car;
        } else {
            c = gCar_to_view;
        }
        if (c->car_master_actor->t.t.translate.t.v[0] <= 500.f) {
            if (old_camera_mode != gAction_replay_camera_mode) {
                camera->field_of_view = BR_ANGLE_DEG(gCamera_angle);
            }
            camera->hither_z = gCamera_hither;
            switch (gAction_replay_camera_mode) {
            case kActionReplayCameraMode_Standard:
            case kActionReplayCameraMode_Rigid:
            case kActionReplayCameraMode_Reversing:
                NormalPositionExternalCamera(c, pTime);
                break;
            case kActionReplayCameraMode_Panning:
                CheckDisablePlingMaterials(c);
                SetPanningFieldOfView();
                if (old_camera_mode != kActionReplayCameraMode_Panning) {
                    SetUpPanningCamera(c);
                }
                PanningExternalCamera(c, pTime);
                EnablePlingMaterials();
                break;
            case kActionReplayCameraMode_ActionTracking:
                CheckDisablePlingMaterials(c);
                SetPanningFieldOfView();
                if (!IncidentCam(c, pTime)) {
                    if (old_camera_mode != gAction_replay_camera_mode) {
                        SetUpPanningCamera(c);
                    }
                    PanningExternalCamera(c, pTime);
                    EnablePlingMaterials();
                }
                break;
            case kActionReplayCameraMode_Manual:
                FrozenCamera(c, pTime);
                break;
            case kActionReplayCameraMode_Peds:
                PositionPedCam(GetTestPed(), pTime);
                break;
            case kActionReplayCameraMode_Drone:
                PositionDroneCam(pTime);
                break;
            case kActionReplayCameraMode_Internal:
                camera->hither_z = gCamera_cockpit_hither;
                PositionCarMountedCamera(c, pTime);
                break;
#ifdef REC2_FIX_BUGS
            default:
                abort();
#endif
            }
            old_camera_mode = gAction_replay_camera_mode;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00403d40
void C2_HOOK_FASTCALL ARMainLoopStart(void) {

    gAction_replay_mode = 0;
    gLast_replay_frame_time = PDGetTotalTime();
}

// FUNCTION: CARMA2_HW 0x00403d30
void C2_HOOK_FASTCALL ARService(void) {

}

// FUNCTION: CARMA2_HW 0x004e71c0
void C2_HOOK_FASTCALL TurnOnActionReplay(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e7270
void C2_HOOK_FASTCALL TurnOffActionReplay(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e72d0
void C2_HOOK_FASTCALL AfterActionReplay(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e6900
void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e6950
void C2_HOOK_FASTCALL PreProcess(int pFrame_period) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e6980
void C2_HOOK_FASTCALL PostProcess(int pFrame_period) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e72e0
void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2) {

    if (gAction_replay_mode) {
        RenderAFrame(1);
        RenderAFrame(1);
    }
    if (!ARToggleReplay(TurnOnActionReplay, TurnOffActionReplay, AfterActionReplay,
            DoZappyActionReplayHeadups, PreProcess, PostProcess, pArg1, pArg2)) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_action_replay_unavailable));
    }
}

// FUNCTION: CARMA2_HW 0x004c6bf0
void C2_HOOK_FASTCALL InitialiseActionReplay(void) {

    C2_HOOK_BUG_ON(REC2_ASIZE(gReplay_callbacks) != 70);

    ARInitialise(!gAusterity_mode && gNet_mode == eNet_mode_none, REC2_ASIZE(gReplay_callbacks), gReplay_callbacks);
    if (!gAusterity_mode && gNet_mode == eNet_mode_none) {
        gCrush_space = BrMemAllocate(0x4000, kMem_pipe_model_geometry);
    } else {
        gCrush_space = NULL;
    }
    gSmudge_space = (tPipe_smudge_data*)gCrush_space;
}

// FUNCTION: CARMA2_HW 0x004e6ff0
void C2_HOOK_FASTCALL CheckReplayTurnOn(void) {

    if (!gAction_replay_mode) {
        if (!KeyIsDown(58) || gEntering_message) {
            gKey_down = -1;
        } else if (gKey_down == -1) {
            ToggleReplay(NULL, NULL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e68e0
void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e69b0
void C2_HOOK_FASTCALL PollActionReplayControls(tU32 *pFrame_period, tU32* pAverage_frame_period) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00402360
int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {

    return gReplay_rate == 0.f;
}

// FUNCTION: CARMA2_HW 0x00402390
float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    return gReplay_rate;
}

// FUNCTION: CARMA2_HW 0x004c80b0
void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e6280
void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e6277
void C2_HOOK_FASTCALL DoActionReplayHeadups(void) {

    ActualActionReplayHeadups(0);
}

// FUNCTION: CARMA2_HW 0x004023a0
void C2_HOOK_FASTCALL ARSetReplayRate(float rate) {
    gReplay_rate = rate;
}

// FUNCTION: CARMA2_HW 0x004e7110
void C2_HOOK_FASTCALL SynchronizeActionReplay(void) {

    while (ARGetReplayRate() != 0.f && (float)(PDGetTotalTime() - gLast_synch_time) < (float)gFrame_period / fabsf(ARGetReplayRate())) {
        ServiceGameInRace();
    }
    gLast_synch_time = PDGetTotalTime();
    if (gSingle_frame_mode) {
        ARSetReplayRate(0.f);
        gSingle_frame_mode = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e6260
void C2_HOOK_FASTCALL DoActionReplayPostSwap(void) {

    RemoveTransientBitmaps(1);
}
