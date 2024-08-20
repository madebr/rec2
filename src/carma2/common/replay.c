#include "replay.h"

#include "globvars.h"
#include "platform.h"
#include "utility.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT(tActionReplayCameraMode, gAction_replay_camera_mode, 0x0079efa8);

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

void (C2_HOOK_FASTCALL * SetCameraType_original)(tActionReplayCameraMode pCamPos);
void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos) {

#if defined(C2_HOOKS_ENABLED)
    SetCameraType_original(pCamPos);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040e790, SetCameraType, SetCameraType_original)

void (C2_HOOK_FASTCALL * PositionExternalCamera_original)(tCar_spec* pCar_spec, tU32 pTime);
void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    PositionExternalCamera_original(pCar_spec, pTime);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040ea30, PositionExternalCamera, PositionExternalCamera_original)

void (C2_HOOK_FASTCALL * ActionReplayFinishRecording_original)(void);
void C2_HOOK_FASTCALL ActionReplayFinishRecording(void) {

#if defined(C2_HOOKS_ENABLED)
    ActionReplayFinishRecording_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e1a20, ActionReplayFinishRecording, ActionReplayFinishRecording_original)

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
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e72e0, ToggleReplay, ToggleReplay_original)

void (C2_HOOK_FASTCALL * InitialiseActionReplay_original)(void);
void C2_HOOK_FASTCALL InitialiseActionReplay(void) {

#if defined(C2_HOOKS_ENABLED)
    InitialiseActionReplay_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c6bf0, InitialiseActionReplay, InitialiseActionReplay_original)

void (C2_HOOK_FASTCALL * CheckReplayTurnOn_original)(void);
void C2_HOOK_FASTCALL CheckReplayTurnOn(void) {

#if defined(C2_HOOKS_ENABLED)
    CheckReplayTurnOn_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e6ff0, CheckReplayTurnOn, CheckReplayTurnOn_original)
