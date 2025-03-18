#include "replay.h"

#include "car.h"
#include "globvars.h"
#include "piping.h"
#include "platform.h"
#include "utility.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT(tActionReplayCameraMode, gAction_replay_camera_mode, 0x0079efa8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAction_replay_manual_camera_target_type, 0x00679278);

void C2_HOOK_FASTCALL SetQuickTimeDefaults(void) {

    if (C2V(gQuick_time_quality)[0] == '\0') {
        c2_strcpy(C2V(gQuick_time_quality), "normal");
    }
    if (C2V(gQuick_time_compressor)[0] == '\0') {
        c2_strcpy(C2V(gQuick_time_compressor), "animation");
    }
    if (C2V(gQuick_time_temp_path)[0] == '\0') {
        PathCat(C2V(gQuick_time_temp_path), C2V(gApplication_path), "QTTMP");
    }
    if (C2V(gQuick_time_movie_path_stub)[0] == '\0') {
        PathCat(C2V(gQuick_time_temp_path), C2V(gApplication_path), "MOVIE");
    }
    if (C2V(gQuick_time_banner_texture_name)[0] == '\0') {
        c2_strcpy(C2V(gQuick_time_banner_texture_name), "ARBANNER.PIX");
    }
}
C2_HOOK_FUNCTION(0x004e1740, SetQuickTimeDefaults)

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
C2_HOOK_FUNCTION(0x0040e700, MungeCarMaterials)

void (C2_HOOK_FASTCALL * SetCameraType_original)(tActionReplayCameraMode pCamPos);
void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos) {

#if defined(C2_HOOKS_ENABLED)
    SetCameraType_original(pCamPos);
#else

    C2V(gAction_replay_camera_mode) = pCamPos;
    switch (pCamPos) {
    case kActionReplayCameraMode_Standard:
    case kActionReplayCameraMode_Rigid:
    case kActionReplayCameraMode_Reversing:
        InitialiseExternalCamera();
        break;
    case kActionReplayCameraMode_Manual:
        C2V(gAction_replay_manual_camera_target_type) = 0;
        break;
    }
    MungeCarMaterials(&C2V(gProgram_state).current_car, C2V(gAction_replay_camera_mode) == kActionReplayCameraMode_Internal);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040e790, SetCameraType, SetCameraType_original)

void (C2_HOOK_FASTCALL * PositionExternalCamera_original)(tCar_spec* pCar_spec, tU32 pTime);
void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    PositionExternalCamera_original(pCar_spec, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040ea30, PositionExternalCamera, PositionExternalCamera_original)

void C2_HOOK_FASTCALL ARMainLoopStart(void) {

    C2V(gAction_replay_mode) = 0;
    C2V(gLast_replay_frame_time) = PDGetTotalTime();
}
C2_HOOK_FUNCTION(0x00403d40, ARMainLoopStart)

void C2_HOOK_FASTCALL ARService(void) {

}
C2_HOOK_FUNCTION(0x00403d30, ARService)

void (C2_HOOK_FASTCALL * ToggleReplay_original)(int* pArg1, int* pArg2);
void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2) {

#if defined(C2_HOOKS_ENABLED)
    ToggleReplay_original(pArg1, pArg2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e72e0, ToggleReplay, ToggleReplay_original)

void (C2_HOOK_FASTCALL * InitialiseActionReplay_original)(void);
void C2_HOOK_FASTCALL InitialiseActionReplay(void) {

#if defined(C2_HOOKS_ENABLED)
    InitialiseActionReplay_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c6bf0, InitialiseActionReplay, InitialiseActionReplay_original)

void (C2_HOOK_FASTCALL * CheckReplayTurnOn_original)(void);
void C2_HOOK_FASTCALL CheckReplayTurnOn(void) {

#if defined(C2_HOOKS_ENABLED)
    CheckReplayTurnOn_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e6ff0, CheckReplayTurnOn, CheckReplayTurnOn_original)

void (C2_HOOK_FASTCALL * DoActionReplay_original)(tU32 pFrame_period);
void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period) {

#if defined(C2_HOOKS_ENABLED)
    DoActionReplay_original(pFrame_period);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e68e0, DoActionReplay, DoActionReplay_original)

void (C2_HOOK_FASTCALL * PollActionReplayControls_original)(tU32 *pFrame_period, tU32* pAverage_frame_period);
void C2_HOOK_FASTCALL PollActionReplayControls(tU32 *pFrame_period, tU32* pAverage_frame_period) {

#if defined(C2_HOOKS_ENABLED)
    PollActionReplayControls_original(pFrame_period, pAverage_frame_period);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e69b0, PollActionReplayControls, PollActionReplayControls_original)

int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {

    return C2V(gReplay_rate) == 0.f;
}
C2_HOOK_FUNCTION(0x00402360, ARReplayIsReallyPaused)

float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    return C2V(gReplay_rate);
}
C2_HOOK_FUNCTION(0x00402390, ARGetReplayRate)

void (C2_HOOK_FASTCALL * PipeSingleGrooveStop_original)(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption);
void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption) {

#if defined(C2_HOOKS_ENABLED)
    PipeSingleGrooveStop_original(pGroove_index, pMatrix, pPath_interrupt, pObject_interrupt, pPath_resumption, pObject_resumption);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c80b0, PipeSingleGrooveStop, PipeSingleGrooveStop_original)
